/* catalina-storage.c
 * 
 * Copyright (C) 2009 Christian Hergert <chris@dronelabs.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA
 * 02110-1301 USA
 */

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <tdb.h>

#include <iris/iris.h>

#include "catalina-formatter.h"
#include "catalina-storage.h"
#include "catalina-storage-private.h"
#include "catalina-transform.h"

#define CLEAR_DBT(dbt)   (memset(&(dbt), 0, sizeof(dbt)))
#define FREE_DBT(dbt)    if ((dbt.flags & (DB_DBT_MALLOC|DB_DBT_REALLOC)) && \
			      dbt.data != NULL) { g_free(dbt.data); dbt.data = NULL; }

/**
 * SECTION:catalina-storage
 * @title: CatalinaStorage
 * @short_description: asynchronous data-store using Berkeley DB
 *
 * #CatalinaStorage is an asynchronous data store.
 *
 * The asynchronous part of the data store is implemented using Iris.  iris_arbiter_coordinate()
 * is used to manage all of the concurrent vs exclusive requests to the data-store.
 *
 * There are two ways you can manipulate the data on the way in or out of the data-store.  Using
 * the "transform" property, you can alter the buffers directly before the write, or right after
 * the read.  Using the "formatter" property, you can control how the content in #GValue's are
 * serialized to disk.  This would allow the storage in formats such as JSON or a lightweight
 * binary format implemented in #CatalinaBinaryFormatter.
 *
 * The properties are not thread-safe, and therefore should only be changed before the storage
 * instance is opened.
 *
 * You can use the storage in either a synchronous or asynchronous manner.  You may even mix
 * and match.  However, keep in mind the code is written in such a way that prefers the
 * asynchronous implementations.
 *
 * Asynchronous example
 *
 * Compile with gcc -o example example.c `pkg-config --libs --cflags catalina-1.0`.
 *
 * |[
 * #include <gio/gio.h>
 * #include <catalina/catalina.h>
 * #include <iris/iris.h>
 * 
 *static void
 * test_get_cb (CatalinaStorage *storage,
 *               GAsyncResult    *result,
 *               gpointer         user_data)
 * {
 * 	gchar *buffer = NULL;
 * 	gsize  buffer_length = 0;
 * 	catalina_storage_get_finish (storage, result, &buffer, &buffer_length, NULL);
 * 	g_free (buffer);
 * 	g_main_loop_quit (user_data);
 *}
 *
 * static void
 * test_open_cb (CatalinaStorage *storage,
 *               GAsyncResult    *result,
 *              gpointer         user_data)
 * {
 * 	if (catalina_storage_open_finish (storage, result, NULL)) {
 * 		catalina_storage_get_async (storage, "key", -1, (GAsyncReadyCallback)test_get_cb, user_data);
 * 	}
 * }
 * 
 * int
 * main (int argc, char *argv[])
 * {
 * 	g_type_init ();
 * 	iris_init ();
 * 	CatalinaStorage *storage = catalina_storage_new ();
 * 	GMainLoop *loop = g_main_loop_new (NULL, FALSE);
 * 	catalina_storage_open_async (storage, ".", "catalina.db", (GAsyncReadyCallback)test_open_cb, loop);
 * 	g_main_loop_run (loop);
 * 	return 0;
 * }
 * ]|
 *
 * Synchronous example
 *
 * |[
 * CatalinaStorage *storage = catalina_storage_new ();
 * GError *oerror = NULL;
 * if (catalina_storage_open (storage, ".", "catalina.db", &oerror)) {
 * 	gchar *buffer = NULL;
 * 	gsize  buffer_length = 0;
 * 	GError *error = NULL;
 * 	if (catalina_storage_get (storage, "key", -1, &buffer, &buffer_length, &error)) {
 * 		g_print ("Found data!\n");
 * 		g_free (buffer);
 * 	}
 * 	else
 * 		g_printerr ("Error: %s\n", error->message);
 * }
 * else
 * 	g_printerr ("Error: %s", oerror->message);
 * ]|
 */

G_DEFINE_TYPE (CatalinaStorage, catalina_storage, G_TYPE_OBJECT)

static void
catalina_storage_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
	switch (property_id) {
	case PROP_USE_IDLE:
		g_value_set_boolean (value, catalina_storage_get_use_idle ((gpointer)object));
		break;
	case PROP_FORMATTER:
		g_value_set_object (value, catalina_storage_get_formatter ((gpointer)object));
		break;
	case PROP_TRANSFORM:
		g_value_set_object (value, catalina_storage_get_transform ((gpointer)object));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
catalina_storage_set_property (GObject      *object,
                               guint         property_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
	switch (property_id) {
	case PROP_USE_IDLE:
		catalina_storage_set_use_idle ((gpointer)object, g_value_get_boolean (value));
		break;
	case PROP_FORMATTER:
		catalina_storage_set_formatter ((gpointer)object, g_value_get_object (value));
		break;
	case PROP_TRANSFORM:
		catalina_storage_set_transform ((gpointer)object, g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
catalina_storage_finalize (GObject *object)
{
	G_OBJECT_CLASS (catalina_storage_parent_class)->finalize (object);
}

static void
catalina_storage_dispose (GObject *object)
{
	CatalinaStoragePrivate *priv;

	priv = CATALINA_STORAGE (object)->priv;

	if (priv->transform) {
		g_object_unref (priv->transform);
		priv->transform = NULL;
	}

	if (priv->formatter) {
		g_object_unref (priv->formatter);
		priv->formatter = NULL;
	}
}

static void
catalina_storage_class_init (CatalinaStorageClass *klass)
{
	GObjectClass *object_class;

	g_type_class_add_private (klass, sizeof (CatalinaStoragePrivate));

	object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = catalina_storage_set_property;
	object_class->get_property = catalina_storage_get_property;
	object_class->finalize     = catalina_storage_finalize;
	object_class->dispose      = catalina_storage_dispose;

	/**
	 * CatalinaStorage:use-idle:
	 *
	 * The "use-idle" property determines if the callback for asynchronous requests
	 * should happen inside of an idle-timeout in the main-loop.
	 * 
	 * If use-idle is %TRUE, callbacks will be performed inside of the main-loop.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_USE_IDLE,
	                                 g_param_spec_boolean ("use-idle",
	                                                       "UseIdle",
	                                                       "If callbacks should occur in "
	                                                       "an idle-timeout of the main-"
	                                                       "loop.",
	                                                       FALSE,
	                                                       G_PARAM_READWRITE));

	/**
	 * CatalinaStorage:transform:
	 *
	 * The "transform" property.  The transform has the ability to manipulate the buffer
	 * on the way to and from the data-store.  This can be used to add features such as
	 * encryption or compression.
	 *
	 * See catalina_transform_read() and catalina_transform_write().
	 */
	g_object_class_install_property (object_class,
	                                 PROP_TRANSFORM,
	                                 g_param_spec_object ("transform",
	                                                      "Transform",
	                                                      "Storage transform",
	                                                      CATALINA_TYPE_TRANSFORM,
	                                                      G_PARAM_READWRITE));

	/**
	 * CatalinaStorage:formatter:
	 *
	 * The "formatter" property.  The formatter is used when catalina_storage_get_value()
	 * or catalina_storage_set_value() are called.  It is responsible for serializing and
	 * deserializing the contents of a #GValue into a buffer.
	 *
	 * See catalina_formatter_serialize() and catalina_formatter_deserialize().
	 */
	g_object_class_install_property (object_class,
	                                 PROP_FORMATTER,
	                                 g_param_spec_object ("formatter",
	                                                      "Formatter",
	                                                      "Storage formatter",
	                                                      CATALINA_TYPE_FORMATTER,
	                                                      G_PARAM_READWRITE));
}

static void
catalina_storage_init (CatalinaStorage *storage)
{
	storage->priv = G_TYPE_INSTANCE_GET_PRIVATE (storage,
	                                             CATALINA_TYPE_STORAGE,
	                                             CatalinaStoragePrivate);

	/* perform callbacks in main by default */
	storage->priv->use_idle = TRUE;

	/* message ports */
	storage->priv->ex_port = iris_port_new ();
	storage->priv->cn_port = iris_port_new ();

	/* message receivers */
	storage->priv->ex_receiver = iris_arbiter_receive (
		NULL, storage->priv->ex_port,
		(IrisMessageHandler)catalina_storage_ex_handle_message,
		storage, NULL);
	storage->priv->cn_receiver = iris_arbiter_receive (
		NULL, storage->priv->cn_port,
		(IrisMessageHandler)catalina_storage_cn_handle_message,
		storage, NULL);

	/* arbiter to handle concurrency management */
	storage->priv->arbiter = iris_arbiter_coordinate (
		storage->priv->ex_receiver,
		storage->priv->cn_receiver,
		NULL);
}

/**
 * catalina_storage_new:
 *
 * Creates a new instance of #CatalinaStorage.
 *
 * Return value: the newly created #CatalinaStorage instance
 */
CatalinaStorage*
catalina_storage_new (void)
{
	return g_object_new (CATALINA_TYPE_STORAGE, NULL);
}

/**
 * catalina_storage_get_use_idle:
 * @storage: A #CatalinaStorage
 *
 * Whether or not the asynchronous callbacks should occur from within an idle-timeout within
 * the applications main-loop.
 *
 * Return value: %TRUE if the callbacks will occur in the main-loop
 */
gboolean
catalina_storage_get_use_idle (CatalinaStorage *storage)
{
	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	return storage->priv->use_idle;
}

/**
 * catalina_storage_set_use_idle:
 * @storage: A #CatalinaStorage
 * @use_idle: a #gboolean
 *
 * Sets whether or not callbacks should occur from within an idle timeout in the main-loop.
 */
void
catalina_storage_set_use_idle (CatalinaStorage *storage,
                               gboolean         use_idle)
{
	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	storage->priv->use_idle = use_idle;
	g_object_notify (G_OBJECT (storage), "use-idle");
}

/**
 * catalina_storage_open_async:
 * @storage: A #CatalinaStorage
 * @env_dir: environment directory for database or %NULL for current working directory
 * @name: name of the database
 * @callback: A #GAsyncReadyCallback
 * @user_data: data for @callback
 *
 * Asynchronously opens the underlying data-store.  Call catalina_storage_open_finish()
 * from within @callback to complete the request.
 */
void
catalina_storage_open_async (CatalinaStorage     *storage,
                             const gchar         *env_dir,
                             const gchar         *name,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gchar                  *real_env_dir;
	IrisMessage            *message;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	g_return_if_fail (name != NULL);

	priv = storage->priv;

	real_env_dir = g_strdup (env_dir != NULL ? env_dir : ".");
	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_open_async);
	task->key = g_build_filename (real_env_dir, name, NULL);
	g_free (real_env_dir);

	message = iris_message_new_data (MESSAGE_OPEN, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	iris_message_unref (message);
}

/**
 * catalina_storage_open_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A location for a #GError or %NULL
 *
 * Completes an asynchronous operation to open the #CatalinaStorage.  If there was an error,
 * %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_open_finish (CatalinaStorage  *storage,
                              GAsyncResult     *result,
                              GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success = FALSE;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (g_simple_async_result_is_valid (result, G_OBJECT (storage),
	                                                      catalina_storage_open_async),
	                      FALSE);

	priv = storage->priv;

	if (!(task = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (result)))) {
		g_critical ("GSimpleAsyncResult does not have a StorageTask");
		return FALSE;
	}

	if (task->error) {
		if (error && *error == NULL)
			*error = g_error_copy (task->error);
		goto cleanup;
	}

	priv->flags |= FLAG_READY;
	success = TRUE;

cleanup:
	storage_task_free (task, TRUE, FALSE);

	return success;
}

/**
 * catalina_storage_open:
 * @storage: A #CatalinaStorage
 * @env_dir: the environment directory for the database
 * @name: the name of the database
 * @error: A location for a #GError or %NULL
 *
 * Synchronously opens the underlying data-store for use.  This method is not the preferred
 * way to perform the operation.  See using catalina_storage_open_async() for the preferred
 * alternative.
 *
 * On failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_open (CatalinaStorage  *storage,
                       const            gchar *env_dir,
                       const            gchar *name,
                       GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gchar                  *real_env_dir;
	IrisMessage            *message;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	priv = storage->priv;

	real_env_dir = g_strdup (env_dir != NULL ? env_dir : ".");
	task = storage_task_new (storage, FALSE, NULL, NULL, NULL);
	task->key = g_build_filename (real_env_dir, name, NULL);
	g_free (real_env_dir);

	message = iris_message_new_data (MESSAGE_OPEN, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	iris_message_unref (message);

	success = storage_task_wait (task, error);
	storage_task_free (task, TRUE, FALSE);

	return success;
}

/**
 * catalina_storage_close_async:
 * @storage: A #CatalinaStorage
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 *
 * Asynchronously closes the underlying data-store.  Use catalina_storage_close_finish()
 * from within @callback to complete the request.
 */
void
catalina_storage_close_async (CatalinaStorage     *storage,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_close_async);
	message = iris_message_new_data (MESSAGE_CLOSE, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	iris_message_unref (message);
}

/**
 * catalina_storage_close_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Completes the asynchronous request to close the underlying data-store.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_close_finish (CatalinaStorage  *storage,
                               GAsyncResult     *result,
                               GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success = FALSE;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (g_simple_async_result_is_valid (result, G_OBJECT (storage),
	                                                      catalina_storage_close_async),
	                      FALSE);

	priv = storage->priv;

	if (!(task = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (result)))) {
		g_critical ("GSimpleAsyncResult does not have a StorageTask");
		return FALSE;
	}

	if (task->error) {
		if (error && *error == NULL)
			*error = g_error_copy (task->error);
		goto cleanup;
	}

	priv->flags = 0;
	success = TRUE;

cleanup:
	storage_task_free (task, FALSE, FALSE);

	return success;
}

/**
 * catalina_storage_close:
 * @storage: A #CatalinaStorage
 * @error: A location for a #GError
 *
 * Synchronously closes the underlying data-store.  This method is not the preferred
 * implementation.  See catalina_storage_close_async() for the preferred method.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_close (CatalinaStorage  *storage,
                        GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;
	task = storage_task_new (storage, FALSE, NULL, NULL, NULL);
	message = iris_message_new_data (MESSAGE_CLOSE, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	success = storage_task_wait (task, error);
	iris_message_unref (message);
	storage_task_free (task, FALSE, FALSE);

	return success;
}

/**
 * catalina_storage_get_async:
 * @storage: A #CatalinaStorage
 * @key: the key to lookup
 * @key_length: the length of @key in bytes
 * @callback: A #GAsyncReadyCallback
 * @user_data: data for @callback
 *
 * Asynchronously requests the retrieval of the data associated with @key.
 *
 * The result will be completed asynchronously and provided to @callback which
 * should in turn call catalina_storage_get_finish() to complete the request.
 */
void
catalina_storage_get_async (CatalinaStorage     *storage,
                            const gchar         *key,
                            gssize               key_length,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;
	gchar                  *dup_key;
	gsize                   dup_len;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	g_return_if_fail (key != NULL);
	g_return_if_fail (key_length == -1 || key_length > 0);

	priv = storage->priv;

	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_get_async);

	if (key_length == -1) {
		dup_key = g_strdup (key);
		dup_len = strlen (dup_key) + 1;
	}
	else {
		dup_key = g_malloc (key_length);
		memcpy (dup_key, key, key_length);
		dup_len = key_length;
	}

	task->key = dup_key;
	task->key_length = dup_len;

	message = iris_message_new_data (MESSAGE_GET, G_TYPE_POINTER, task);
	iris_port_post (priv->cn_port, message);
	iris_message_unref (message);
}

/**
 * catalina_storage_get_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @value: A location for the requested buffer
 * @value_length: A location for the requested buffer size, or %NULL
 * @error: A location for a #GError or %NULL
 *
 * Completes an asynchronous get request.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * See catalina_storage_get_async().
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_get_finish (CatalinaStorage  *storage,
                             GAsyncResult     *result,
                             gchar           **value,
                             gsize            *value_length,
                             GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (g_simple_async_result_is_valid (result, G_OBJECT (storage),
	                                                      catalina_storage_get_async),
	                      FALSE);

	priv = storage->priv;

	if (!(task = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (result)))) {
		g_critical ("GSimpleAsyncResult does not have a StorageTask");
		return FALSE;
	}

	success = task->success;

	if (task->error) {
		if (error && *error == NULL)
			*error = g_error_copy (task->error);
		goto cleanup;
	}
	else {
		if (task->data)
			*value = task->data;
		if (value_length)
			*value_length = task->data_length;
	}

cleanup:
	storage_task_free (task, TRUE, FALSE);

	return success;
}

/**
 * catalina_storage_get:
 * @storage: A #CatalinaStorage
 * @key: the key to lookup
 * @key_length: the length of @key in bytes or -1 if it is %NULL terminated
 * @value: a location to store the resulting buffer
 * @value_length: a location to store the size of the resulting buffer or %NULL
 * @error: A location for a #GError or %NULL
 *
 * Synchronously retreives the data found for @key.  Keep in mind that
 * #CatalinaStorage is an asynchronous data-store in nature.  Therefore, making
 * synchronous calls is not the most efficient way to complete the task.
 *
 * In the case of an error, %FALSE is returned and @error is set.
 *
 * See catalina_storage_get_async().
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_get (CatalinaStorage  *storage,
                      const gchar      *key,
                      gssize            key_length,
                      gchar           **value,
                      gsize            *value_length,
                      GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;
	gchar                  *dup_key;
	gsize                   dup_len;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (key_length == -1 || key_length > 0, FALSE);

	priv = storage->priv;
	task = storage_task_new (storage, FALSE, NULL, NULL, NULL);

	if (key_length == -1) {
		dup_key = g_strdup (key);
		dup_len = strlen (dup_key) + 1;
	}
	else {
		dup_key = g_malloc (key_length);
		memcpy (dup_key, key, key_length);
		dup_len = key_length;
	}

	task->key = dup_key;
	task->key_length = dup_len;

	message = iris_message_new_data (MESSAGE_GET, G_TYPE_POINTER, task);
	iris_port_post (priv->cn_port, message);
	iris_message_unref (message);

	if ((success = storage_task_wait (task, error)) == TRUE) {
		*value = task->data;
		if (value_length)
			*value_length = task->data_length;
	}

	storage_task_free (task, TRUE, FALSE);

	return success;
}

/**
 * catalina_storage_set_async:
 * @storage: A #CatalinaStorage
 * @key: the key of which to assign @value
 * @key_length: the length of @key in bytes
 * @value: the content to store
 * @value_length: the length of @value in bytes
 * @callback: A #GAsyncReadyCallback
 * @user_data: data for @callback
 *
 * Asynchronously stores @value to the underlying data-store with @key.  Call
 * catalina_storage_set_finish() from within @callback.
 */
void
catalina_storage_set_async (CatalinaStorage     *storage,
                            const gchar         *key,
                            gssize               key_length,
                            const gchar         *value,
                            gssize               value_length,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;
	gchar                  *dup_key,
	                       *dup_value;
	gsize                   dup_key_length,
	                        dup_value_length;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	g_return_if_fail (key != NULL);
	g_return_if_fail (key_length == -1 || key_length > 0);
	g_return_if_fail (value != NULL);
	g_return_if_fail (value_length == -1 || value_length > 0);

	priv = storage->priv;
	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_set_async);

	if (key_length == -1) {
		dup_key = g_strdup (key);
		dup_key_length = strlen (dup_key) + 1;
	}
	else {
		dup_key = g_malloc (key_length);
		memcpy (dup_key, key, key_length);
		dup_key_length = key_length;
	}

	if (value_length == -1) {
		dup_value = g_strdup (value);
		dup_value_length = strlen (dup_value) + 1;
	}
	else {
		dup_value = g_malloc (value_length);
		memcpy (dup_value, value, value_length);
		dup_value_length = value_length;
	}

	task->key = dup_key;
	task->key_length = dup_key_length;
	task->data = dup_value;
	task->data_length = dup_value_length;

	message = iris_message_new_data (MESSAGE_SET, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	iris_message_unref (message);
}

/**
 * catalina_storage_set_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A location for #GError or %NULL
 *
 * Completes an asynchronous set request.  Upon failure, %FALSE is returned and @error is set.
 *
 * See catalina_storage_set_async().
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_set_finish (CatalinaStorage  *storage,
                             GAsyncResult     *result,
                             GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (g_simple_async_result_is_valid (result, G_OBJECT (storage),
	                                                      catalina_storage_set_async),
	                      FALSE);

	priv = storage->priv;

	if (!(task = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (result)))) {
		g_critical ("GSimpleAsyncResult does not have a StorageTask");
		return FALSE;
	}

	if (task->error) {
		if (error && *error == NULL)
			*error = g_error_copy (task->error);
	}

	success = task->success;
	storage_task_free (task, TRUE, TRUE);

	return success;
}

/**
 * catalina_storage_set:
 * @storage: A #CatalinaStorage
 * @key: the key of which to store @value
 * @key_length: the length of @key in bytes or -1 if the @key is %NULL terminated
 * @value: the data to store
 * @value_length: the length of @data in bytes
 * @error: A location for a #GError or %NULL
 *
 * Synchronously performs a set request.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * See catalina_storage_set_async().
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_set (CatalinaStorage  *storage,
                      const gchar      *key,
                      gssize            key_length,
                      const gchar      *value,
                      gssize            value_length,
                      GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	IrisMessage            *message;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (key_length == -1 || key_length > 0, FALSE);

	priv = storage->priv;
	task = storage_task_new (storage, FALSE, NULL, NULL, NULL);

	task->key = (gchar*)key;
	task->key_length = (key_length == -1) ? strlen (key) + 1 : key_length;
	task->data = (gchar*)value;
	task->data_length = (value_length == -1) ? strlen (value) + 1 : value_length;

	message = iris_message_new_data (MESSAGE_SET, G_TYPE_POINTER, task);
	iris_port_post (priv->ex_port, message);
	iris_message_unref (message);

	success = storage_task_wait (task, error);
	storage_task_free (task, FALSE, FALSE);

	return success;
}

static void
catalina_storage_get_value_async_cb (CatalinaStorage *storage,
                                     GAsyncResult    *result,
                                     StorageTask     *task)
{
	CatalinaStoragePrivate *priv;;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;

	g_return_if_fail (storage != NULL);
	g_return_if_fail (result != NULL);
	g_return_if_fail (task != NULL);

	priv = storage->priv;

	if (!catalina_storage_get_finish (storage, result,
	                                  &buffer, &buffer_length,
	                                  &task->error))
		goto failure;

	if (!priv->formatter) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "CatalinaStorage is missing a formatter for deserialization");
		goto failure;
	}

	if (!catalina_formatter_deserialize (priv->formatter, &task->value,
	                                     buffer, buffer_length,
	                                     &task->error))
		goto failure;

	g_free (buffer);
	storage_task_succeed (task);
	return;

failure:
	g_free (buffer);
	storage_task_fail (task);
	return;
}

/**
 * catalina_storage_get_value_async:
 * @storage: A #CatalinaStorage
 * @key: the key to lookup
 * @key_length: the length of @key in bytes
 * @callback: A #GAsyncReadyCallback
 * @user_data: data for @callback
 *
 * Asynchronously requests the content for @key.  In the process, the data is retrieved from
 * storage and deserialized by the #CatalinaStorage instance's "formatter" property.
 */
void
catalina_storage_get_value_async (CatalinaStorage     *storage,
                                  const gchar         *key,
                                  gssize               key_length,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;

	if (!priv->formatter)
		g_critical ("%s called without a formatter", __func__);

	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_get_value_async);
	catalina_storage_get_async (storage, key, key_length,
	                            (GAsyncReadyCallback)catalina_storage_get_value_async_cb,
	                            task);
}

/**
 * catalina_storage_get_value_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @value: A #GValue
 * @error: A location for a #GError or %NULL
 *
 * Completes an asynchronous request to retreive the content from storage and deserialize
 * using the instance's "formatter" property.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_get_value_finish (CatalinaStorage  *storage,
                                   GAsyncResult     *result,
                                   GValue           *value,
                                   GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (g_simple_async_result_is_valid (result, G_OBJECT (storage),
	                                                      catalina_storage_get_value_async),
	                      FALSE);

	priv = storage->priv;
	task = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (result));
	success = task->success;

	if (task->error) {
		if (error && *error == NULL)
			*error = g_error_copy (task->error);
	}

	if (success) {
		if (!G_VALUE_TYPE (value))
			g_value_init (value, G_VALUE_TYPE (&task->value));
		g_value_copy (&task->value, value);
	}

	storage_task_free (task, FALSE, TRUE);
	return success;
}

/**
 * catalina_storage_get_value:
 * @storage: A #CatalinaStorage
 * @key: A #const
 * @key_length: the length in bytes of @key or -1 if @key is %NULL terminated
 * @value: A #GValue to store the resulting value
 * @error: A location for a #GError or %NULL
 *
 * Synchronously requests the content for @key and deserializes using the instances
 * "formatter" property.
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_get_value (CatalinaStorage  *storage,
                            const gchar      *key,
                            gssize            key_length,
                            GValue           *value,
                            GError          **error)
{
	CatalinaStoragePrivate *priv;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (key_length == -1 || key_length > 0, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	priv = storage->priv;

	if (!priv->formatter) {
		g_set_error (error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "CatalinaStorage is missing a formatter for deserialization");
		return FALSE;
	}

	if (!catalina_storage_get (storage, key, key_length,
	                           &buffer, &buffer_length, error))
		return FALSE;

	success = catalina_formatter_deserialize (priv->formatter, value,
	                                          buffer, buffer_length,
	                                          error);
	g_free (buffer);
	return success;
}

static void
catalina_storage_set_value_async_cb (CatalinaStorage *storage,
                                     GAsyncResult    *result,
                                     StorageTask     *task)
{
	if (!catalina_storage_set_finish (storage, result, &task->error))
		storage_task_fail (task);
	else
		storage_task_succeed (task);
}

/**
 * catalina_storage_set_value_async:
 * @storage: A #CatalinaStorage
 * @key: A buffer containing the key
 * @key_length: the length of @key in bytes or -1 if @key is %NULL terminated
 * @value: A #GValue to serialize and store
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 *
 * Asynchronously requests the serialization of @value and storage to the underlying
 * data-store.
 *
 * @value is serialized using the instances "formatter" property.
 *
 * Call catalina_storage_set_value_finish() from within @callback.
 */
void
catalina_storage_set_value_async (CatalinaStorage     *storage,
                                  const gchar         *key,
                                  gssize               key_length,
                                  const GValue        *value,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
	task = storage_task_new (storage, TRUE, callback, user_data,
	                         catalina_storage_set_value_async);

	if (!priv->formatter) {
		g_critical ("%s: Storage instance missing a CatalinaFormatter", __func__);
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Storage instance missing a CatalinaFormatter");
		storage_task_fail (task);
		storage_task_free (task, FALSE, FALSE);
		return;
	}

	if (!catalina_formatter_serialize (priv->formatter, value,
	                                   &buffer, &buffer_length,
	                                   &task->error))
	{
		storage_task_fail (task);
		storage_task_free (task, FALSE, FALSE);
		return;
	}

	catalina_storage_set_async (storage, key, key_length,
	                            buffer, buffer_length,
	                            (GAsyncReadyCallback)catalina_storage_set_value_async_cb,
	                            task);

	g_free (buffer);
}

/**
 * catalina_storage_set_value_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A location for a #GError or %NULL
 *
 * Completes and asynchronous request to catalina_storage_set_value_async().
 *
 * Upon failure, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_set_value_finish (CatalinaStorage  *storage,
                                   GAsyncResult     *result,
                                   GError          **error)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;
	task = g_simple_async_result_get_op_res_gpointer ((gpointer)result);

	success = task->success;
	if (task->error && error && *error == NULL) {
		*error = task->error;
		task->error = NULL;
	}

	storage_task_free (task, TRUE, TRUE);

	return success;
}

/**
 * catalina_storage_set_value:
 * @storage: A #CatalinaStorage
 * @key: A buffer containing the key
 * @key_length: the length of @key in bytes or -1 if @key is %NULL terminated
 * @value: A #GValue to serialize and store
 * @error: A location for a #GError or %NULL
 *
 * Synchronously requests the serialization of @value and storage to the underlying data-store.
 *
 * Upon error, %FALSE is returned and @error is set.
 *
 * See catalina_storage_set_value_async().
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_storage_set_value (CatalinaStorage  *storage,
                            const gchar      *key,
                            gssize            key_length,
                            const GValue     *value,
                            GError          **error)
{
	CatalinaStoragePrivate *priv;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;
	gboolean                success;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (key_length == -1 || key_length > 0, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	priv = storage->priv;

	if (!priv->formatter) {
		g_set_error (error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Instance missing formatter for deserialization");
		return FALSE;
	}

	if (!catalina_formatter_serialize (priv->formatter, value,
	                                   &buffer, &buffer_length,
	                                   error))
		return FALSE;

	success = catalina_storage_set (storage, key, key_length,
	                                buffer, buffer_length, error);
	g_free (buffer);

	return success;
}

/**
 * catalina_storage_get_transform:
 * @storage: A #CatalinaStorage
 *
 * This method is not thread-safe.
 *
 * Return value: the #CatalinaTransform for the @storage instance.
 */
CatalinaTransform*
catalina_storage_get_transform (CatalinaStorage *storage)
{
	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), NULL);
	return CATALINA_STORAGE (storage)->priv->transform;
}

/**
 * catalina_storage_set_transform:
 * @storage: A #CatalinaStorage
 * @transform: A #CatalinaTransform
 *
 * Sets the #CatalinaTransform for the storage.  This is the "transform" property.
 *
 * This method is not thread-safe.
 */
void
catalina_storage_set_transform (CatalinaStorage   *storage,
                                CatalinaTransform *transform)
{
	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	if (CATALINA_STORAGE (storage)->priv->transform)
		g_object_unref (CATALINA_STORAGE (storage)->priv->transform);
	CATALINA_STORAGE (storage)->priv->transform = g_object_ref (transform);
	g_object_notify (G_OBJECT (storage), "transform");
}

/**
 * catalina_storage_get_formatter:
 * @storage: A #CatalinaStorage
 *
 * This method is not thread-safe.
 *
 * Return value: The #CatalinaFormatter property
 */
CatalinaFormatter*
catalina_storage_get_formatter (CatalinaStorage *storage)
{
	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), NULL);
	return CATALINA_STORAGE (storage)->priv->formatter;
}

/**
 * catalina_storage_set_formatter:
 * @storage: A #CatalinaStorage
 * @formatter: A #CatalinaFormatter
 *
 * Sets the "formatter" property.
 *
 * This method is not thread-safe.
 */
void
catalina_storage_set_formatter (CatalinaStorage   *storage,
                                CatalinaFormatter *formatter)
{
	g_return_if_fail (CATALINA_IS_STORAGE (storage));
	if (CATALINA_STORAGE (storage)->priv->formatter)
		g_object_unref (CATALINA_STORAGE (storage)->priv->formatter);
	CATALINA_STORAGE (storage)->priv->formatter = g_object_ref (formatter);
	g_object_notify (G_OBJECT (storage), "formatter");
}

GQuark
catalina_storage_error_quark (void)
{
	return g_quark_from_static_string ("catalina-storage-error-quark");
}

/***************************************************************************
 *               Asynchronous Message Handlers for Iris                    *
 ***************************************************************************/

static void
handle_open (CatalinaStorage *storage,
             IrisMessage     *message)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	TDB_CONTEXT            *db_ctx;
	gchar                  *dir;
	gint                    tdb_flags,
				o_flags;

	g_return_if_fail (storage != NULL);
	g_return_if_fail (message != NULL && message->what == MESSAGE_OPEN);

	priv = storage->priv;
	task = g_value_get_pointer (iris_message_get_data (message));

	if (priv->db_ctx) {
		/* already open, throw error */
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Storage already opened");
		storage_task_fail (task);
		return;
	}

	/* make sure the containing directory exists */
	dir = g_path_get_dirname (task->key);
	if (!g_file_test (dir, G_FILE_TEST_IS_DIR))
		g_mkdir_with_parents (dir, 0755);
	g_free (dir);

	/* always store in big-endian */
	tdb_flags  = (G_BYTE_ORDER == G_LITTLE_ENDIAN) ? TDB_CONVERT : 0;
	tdb_flags |= TDB_NOLOCK;

	if (!g_file_test (task->key, G_FILE_TEST_IS_REGULAR))
		o_flags = O_CREAT;

	if (!(db_ctx = tdb_open (task->key, 0, tdb_flags, o_flags | O_RDWR, 0755))) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_DB,
		             "Could not open the database");
		storage_task_fail (task);
		return;
	}

	priv->db_ctx = db_ctx;
	task->success = TRUE;
	storage_task_succeed (task);
}

static void
handle_close (CatalinaStorage *storage,
              IrisMessage     *message)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gint                    ret;

	g_return_if_fail (message->what == MESSAGE_CLOSE);
	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
	task = g_value_get_pointer (iris_message_get_data (message));

	if (!priv->db_ctx) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Storage is not currently open");
		storage_task_fail (task);
		return;
	}

	if ((ret = tdb_close (priv->db_ctx)) != 0) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "There was an error closing the storage: (%d)",
		             ret);
		storage_task_fail (task);
		return;
	}

	priv->db_ctx = NULL;
	task->success = TRUE;
	storage_task_succeed (task);
}

static void
handle_get (CatalinaStorage *storage,
            IrisMessage     *message)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success       = FALSE;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;

	g_return_if_fail (message->what == MESSAGE_GET);
	g_return_if_fail (storage != NULL);

	priv = storage->priv;
	task = g_value_get_pointer (iris_message_get_data (message));

	if (!priv->db_ctx) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Storage is not currently open");
		storage_task_fail (task);
		return;
	}

	{
		TDB_DATA db_key,
		         db_value;

		bzero (&db_key, sizeof (db_key));
		bzero (&db_value, sizeof (db_value));

		db_key.dptr = (guchar*)task->key;
		db_key.dsize = task->key_length;

		db_value = tdb_fetch (priv->db_ctx, db_key);

		if (!db_value.dptr) {
			g_set_error (&task->error, CATALINA_STORAGE_ERROR,
			             CATALINA_STORAGE_ERROR_NO_SUCH_KEY,
			             "%s", tdb_errorstr (priv->db_ctx));
		}
		else {
			if (priv->transform) {
				if (catalina_transform_read (priv->transform,
				                             (gchar*)db_value.dptr, db_value.dsize,
				                             &buffer, &buffer_length,
				                             &task->error))
				{
					if (buffer_length == 0)
						goto copy;
					success = TRUE;
					task->data = buffer;
					task->data_length = buffer_length;
				}
			}
			else {
			copy:
				success = TRUE;
				task->data_length = db_value.dsize;
				task->data = g_malloc (task->data_length);
				memcpy (task->data, db_value.dptr, task->data_length);
			}
		}
	}

	if (success)
		storage_task_succeed (task);
	else
		storage_task_fail (task);
}

static void
handle_set (CatalinaStorage *storage,
            IrisMessage     *message)
{
	CatalinaStoragePrivate *priv;
	StorageTask            *task;
	gboolean                success       = FALSE;
	gchar                  *buffer        = NULL;
	gsize                   buffer_length = 0;

	g_return_if_fail (message->what == MESSAGE_SET);
	g_return_if_fail (storage != NULL);

	priv = storage->priv;
	task = g_value_get_pointer (iris_message_get_data (message));

	if (!priv->db_ctx) {
		g_set_error (&task->error, CATALINA_STORAGE_ERROR,
		             CATALINA_STORAGE_ERROR_STATE,
		             "Storage is not currently open");
		storage_task_fail (task);
		return;
	}

	if (priv->transform) {
		if (!catalina_transform_write (priv->transform,
		                               task->data, task->data_length,
		                               &buffer, &buffer_length,
		                               &task->error)) {
			storage_task_fail (task);
			return;
		}
	}

	{
		TDB_DATA key,
			 dbuf;
		gint     flags = 0,
		         ret;

		bzero (&key, sizeof (key));
		bzero (&dbuf, sizeof (dbuf));

		key.dptr = (guchar*)task->key;
		key.dsize = task->key_length;
		dbuf.dptr = (guchar*)(buffer != NULL ? buffer : task->data);
		dbuf.dsize = buffer != NULL ? buffer_length : task->data_length;

		flags = TDB_REPLACE;

		if ((ret = tdb_store (priv->db_ctx, key, dbuf, flags)) != 0) {
			g_set_error (&task->error, CATALINA_STORAGE_ERROR,
			             CATALINA_STORAGE_ERROR_NO_SUCH_KEY,
			             "tdb_store: %s",
			             tdb_errorstr (priv->db_ctx));
		}
		else {
			success = TRUE;
		}
	}

	g_free (buffer);

	if (success)
		storage_task_succeed (task);
	else
		storage_task_fail (task);
}

static void
catalina_storage_cn_handle_message (IrisMessage     *message,
                                    CatalinaStorage *storage)
{
	switch (message->what) {
	case MESSAGE_GET:
		handle_get (storage, message);
		break;
	default:
		g_warning ("Invalid message sent to storage: %d", message->what);
	}
}

static void
catalina_storage_ex_handle_message (IrisMessage     *message,
                                    CatalinaStorage *storage)
{
	switch (message->what) {
	case MESSAGE_SET:
		handle_set (storage, message);
		break;
	case MESSAGE_OPEN:
		handle_open (storage, message);
		break;
	case MESSAGE_CLOSE:
		handle_close (storage, message);
		break;
	default:
		g_warning ("Invalid exclusive message: %d", message->what);
	}
}

/***************************************************************************
 *     Storage Tasks - Asynchronous and Synchronous Task Encapsulation     *
 ***************************************************************************/

StorageTask*
storage_task_new (CatalinaStorage     *storage,
                  gboolean             is_async,
                  GAsyncReadyCallback  callback,
                  gpointer             user_data,
                  gpointer             source_tag)
{
	StorageTask *task;

	task = g_slice_new0 (StorageTask);
	task->storage = storage;

	if (!is_async) {
		/* FIXME: Use IrisFreeList to re-use mutex and cond */
		task->mutex = g_mutex_new ();
		task->cond = g_cond_new ();
		g_mutex_lock (task->mutex);
	}
	else {
		task->result = g_simple_async_result_new (G_OBJECT (storage),
		                                          callback, user_data,
		                                          source_tag);
		g_simple_async_result_set_op_res_gpointer (task->result, task, NULL);
	}

	return task;
}

static void
storage_task_free (StorageTask *task,
                   gboolean     free_key,
                   gboolean     free_data)
{
	/* FIXME: Use IrisFreeList to re-use mutex and cond */
	if (task->mutex) {
		g_mutex_unlock (task->mutex);
		g_mutex_free (task->mutex);
	}

	if (task->cond)
		g_cond_free (task->cond);

	if (G_VALUE_TYPE (&task->value))
		g_value_unset (&task->value);

	if (task->error)
		g_error_free (task->error);

	if (free_key)
		g_free (task->key);

	if (free_data)
		g_free (task->data);

	if (task->result)
		g_object_unref (task->result);

	g_slice_free (StorageTask, task);
}

static gboolean
storage_task_wait (StorageTask  *task,
                   GError      **error)
{
	g_cond_wait (task->cond, task->mutex);
	if (!task->success && task->error && error && *error == NULL)
		*error = g_error_copy (task->error);
	return task->success;
}

static void
storage_task_complete (StorageTask *task,
                       gboolean     success)
{
	task->success = success;

	if (task->mutex) {
		g_mutex_lock (task->mutex);
		g_cond_signal (task->cond);
		g_mutex_unlock (task->mutex);
	}
	else {
		if (task->storage->priv->use_idle)
			g_simple_async_result_complete_in_idle (task->result);
		else
			g_simple_async_result_complete (task->result);
	}
}

static void
storage_task_succeed (StorageTask *task)
{
	storage_task_complete (task, TRUE);
}

static void
storage_task_fail (StorageTask *task)
{
	storage_task_complete (task, FALSE);
}
