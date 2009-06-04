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

#include <db.h>
#include <string.h>
#include <iris/iris.h>

#include "catalina-storage.h"
#include "catalina-storage-private.h"

/**
 * SECTION:catalina-storage
 * @title: CatalinaStorage
 * @short_description: asynchronous data-store using Berkeley DB
 *
 * #CatalinaStorage provides an asynchronous wrapper around Berkeley DB.
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
}

static void
catalina_storage_init (CatalinaStorage *storage)
{
	storage->priv = G_TYPE_INSTANCE_GET_PRIVATE (storage,
	                                             CATALINA_TYPE_STORAGE,
	                                             CatalinaStoragePrivate);

	storage->priv->use_idle = TRUE;
	storage->priv->port = iris_port_new ();
	storage->priv->receiver = iris_arbiter_receive (NULL,
	                                                storage->priv->port,
	                                                (IrisMessageHandler)catalina_storage_handle_message,
	                                                storage, NULL);
	storage->priv->arbiter = iris_arbiter_coordinate (storage->priv->receiver,NULL, NULL);
}

/**
 * catalina_storage_new:
 *
 * Return value: 
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
 * @gchar env_dir: A #const
 * @gchar name: A #const
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 */
void
catalina_storage_open_async (CatalinaStorage     *storage,
                             const               gchar *env_dir,
                             const               gchar *name,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_open_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_open_finish (CatalinaStorage  *storage,
                              GAsyncResult     *result,
                              GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_open:
 * @storage: A #CatalinaStorage
 * @gchar env_dir: A #const
 * @gchar name: A #const
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_open (CatalinaStorage  *storage,
                       const            gchar *env_dir,
                       const            gchar *name,
                       GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_close_async:
 * @storage: A #CatalinaStorage
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 */
void
catalina_storage_close_async (CatalinaStorage     *storage,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_close_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_close_finish (CatalinaStorage  *storage,
                               GAsyncResult     *result,
                               GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_close:
 * @storage: A #CatalinaStorage
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_close (CatalinaStorage  *storage,
                        GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
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
catalina_storage_get_async (CatalinaStorage      *storage,
                            const                gchar *key,
                            gssize                key_length,
                            GAsyncReadyCallback   callback,
                            gpointer              user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_get_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @value: A #gchar
 * @value_length: A #gsize
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_get_finish (CatalinaStorage  *storage,
                             GAsyncResult     *result,
                             gchar           **value,
                             gsize            *value_length,
                             GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
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

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (key_length > 0, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	priv = storage->priv;
	task = storage_task_new (storage, TRUE);

	storage_task_free (task, FALSE, FALSE);

	storage_task_wait (task, NULL);
	storage_task_succeed (task);
	storage_task_fail (task, NULL);

	return FALSE;
}

/**
 * catalina_storage_set_async:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @gchar value: A #const
 * @value_length: A #gssize
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 */
void
catalina_storage_set_async (CatalinaStorage     *storage,
                            const               gchar *key,
                            gssize               key_length,
                            const               gchar *value,
                            gssize               value_length,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_set_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_set_finish (CatalinaStorage  *storage,
                             GAsyncResult     *result,
                             GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_set:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @gchar value: A #const
 * @value_length: A #gssize
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_set (CatalinaStorage  *storage,
                      const            gchar *key,
                      gssize            key_length,
                      const            gchar *value,
                      gssize            value_length,
                      GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_get_value_async:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 */
void
catalina_storage_get_value_async (CatalinaStorage     *storage,
                                  const               gchar *key,
                                  gssize               key_length,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_get_value_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @value: A #GValue
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_get_value_finish (CatalinaStorage  *storage,
                                   GAsyncResult     *result,
                                   GValue           *value,
                                   GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_get_value:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @value: A #GValue
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_get_value (CatalinaStorage  *storage,
                            const            gchar *key,
                            gssize            key_length,
                            GValue           *value,
                            GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_set_value_async:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @GValue value: A #const
 * @callback: A #GAsyncReadyCallback
 * @user_data: A #gpointer
 */
void
catalina_storage_set_value_async (CatalinaStorage     *storage,
                                  const               gchar *key,
                                  gssize               key_length,
                                  const               GValue *value,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
	CatalinaStoragePrivate *priv;

	g_return_if_fail (CATALINA_IS_STORAGE (storage));

	priv = storage->priv;
}

/**
 * catalina_storage_set_value_finish:
 * @storage: A #CatalinaStorage
 * @result: A #GAsyncResult
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_set_value_finish (CatalinaStorage  *storage,
                                   GAsyncResult     *result,
                                   GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

/**
 * catalina_storage_set_value:
 * @storage: A #CatalinaStorage
 * @gchar key: A #const
 * @key_length: A #gssize
 * @GValue value: A #const
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_storage_set_value (CatalinaStorage  *storage,
                            const            gchar *key,
                            gssize            key_length,
                            const            GValue *value,
                            GError          **error)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

	return FALSE;
}

static void
handle_open (CatalinaStorage *storage,
             IrisMessage     *message)
{
/*
	DB_ENV *db_env;
	DB     *db;
	gchar  *errmsg,
	       *env_dir,
	       *name;
	gint    ret,
	        flags = DB_CREATE     |
	                DB_INIT_LOG   |
	                DB_INIT_LOCK  |
	                DB_INIT_MPOOL |
	                DB_INIT_TXN   |
	                DB_PRIVATE;
*/

}

static void
handle_close (CatalinaStorage *storage,
              IrisMessage     *message)
{
}

static void
handle_get (CatalinaStorage *storage,
            IrisMessage     *message)
{
}

static void
handle_set (CatalinaStorage *storage,
            IrisMessage     *message)
{
}

static void
catalina_storage_handle_message (IrisMessage     *message,
                                 CatalinaStorage *storage)
{
	switch (message->what) {
	case MESSAGE_OPEN:
		handle_open (storage, message);
		break;
	case MESSAGE_CLOSE:
		handle_close (storage, message);
		break;
	case MESSAGE_GET:
		handle_get (storage, message);
		break;
	case MESSAGE_SET:
		handle_set (storage, message);
		break;
	default:
		g_warning ("Invalid message sent to storage: %d", message->what);
	}
}

/***************************************************************************
 *     Storage Tasks - Asynchronous and Synchronous Task Encapsulation     *
 ***************************************************************************/

StorageTask*
storage_task_new (CatalinaStorage *storage,
                  gboolean         is_async)
{
	StorageTask *task;

	task = g_slice_new0 (StorageTask);
	task->storage = storage;

	if (!is_async) {
		/* FIXME: We should use IrisFreeList here to re-use existing
		 *   mutex and condition instances. */
		task->mutex = g_mutex_new ();
		task->cond = g_cond_new ();
		g_mutex_lock (task->mutex);
	}

	return task;
}

static void
storage_task_free (StorageTask *task,
                   gboolean     free_key,
                   gboolean     free_data)
{
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

	g_slice_free (StorageTask, task);
}

static gboolean
storage_task_wait (StorageTask  *task,
                   GError      **error)
{
	g_cond_wait (task->cond, task->mutex);

	if (!task->result.bool_v && task->error && error && *error == NULL)
		*error = g_error_copy (task->error);

	return task->result.bool_v;
}

static void
storage_task_complete (StorageTask *task,
                       gboolean     result,
                       GError      *error)
{
	if (error)
		task->error = error;

	if (task->mutex) {
		task->result.bool_v = result;
		g_mutex_lock (task->mutex);
		g_cond_signal (task->cond);
		g_mutex_unlock (task->mutex);
	}
	else {
		g_simple_async_result_set_op_res_gboolean (task->result.async_v, result);
		if (task->storage->priv->use_idle)
			g_simple_async_result_complete_in_idle (task->result.async_v);
		else
			g_simple_async_result_complete (task->result.async_v);
	}
}

static void
storage_task_succeed (StorageTask *task)
{
	storage_task_complete (task, TRUE, NULL);
}

static void
storage_task_fail (StorageTask *task,
                   GError      *error)
{
	storage_task_complete (task, FALSE, error);
}
