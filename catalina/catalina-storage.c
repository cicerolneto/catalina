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

/**
 * SECTION:catalina-storage
 * @title: CatalinaStorage
 * @short_description: asynchronous data-store using Berkeley DB
 *
 * #CatalinaStorage provides an asynchronous wrapper around Berkeley DB.
 */

static void handle_message (IrisMessage     *message,
                            CatalinaStorage *storage);

G_DEFINE_TYPE (CatalinaStorage, catalina_storage, G_TYPE_OBJECT)

struct _CatalinaStoragePrivate
{
	gboolean      use_idle;

	IrisPort     *port;
	IrisReceiver *receiver;
	IrisArbiter  *arbiter;
};

enum
{
	MESSAGE_0,
	MESSAGE_OPEN,
	MESSAGE_CLOSE,
	MESSAGE_GET,
	MESSAGE_SET,
};

static void
catalina_storage_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
	switch (property_id) {
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
	                                                (IrisMessageHandler)handle_message,
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
 * @gchar key: A #const
 * @key_length: A #gssize
 * @callback: A #GAsyncReadyCallback
 * @error: A #GError
 */
void
catalina_storage_get_async (CatalinaStorage      *storage,
                            const                gchar *key,
                            gssize                key_length,
                            GAsyncReadyCallback   callback,
                            GError              **error)
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
 * @gchar key: A #const
 * @key_length: A #gssize
 * @value: A #gchar
 * @value_length: A #gsize
 *
 * Return value: 
 */
gboolean
catalina_storage_get (CatalinaStorage  *storage,
                      const            gchar *key,
                      gssize            key_length,
                      gchar           **value,
                      gsize            *value_length)
{
	CatalinaStoragePrivate *priv;

	g_return_val_if_fail (CATALINA_IS_STORAGE (storage), FALSE);

	priv = storage->priv;

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

static void
handle_open (CatalinaStorage *storage,
             IrisMessage     *message)
{
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
handle_message (IrisMessage     *message,
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
