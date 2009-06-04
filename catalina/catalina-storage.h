/* catalina-storage.h
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

#ifndef __CATALINA_STORAGE_H__
#define __CATALINA_STORAGE_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define CATALINA_TYPE_STORAGE            (catalina_storage_get_type())      
#define CATALINA_STORAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CATALINA_TYPE_STORAGE, CatalinaStorage))     
#define CATALINA_STORAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  CATALINA_TYPE_STORAGE, CatalinaStorageClass))
#define CATALINA_IS_STORAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CATALINA_TYPE_STORAGE))                      
#define CATALINA_IS_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  CATALINA_TYPE_STORAGE))                      
#define CATALINA_STORAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  CATALINA_TYPE_STORAGE, CatalinaStorageClass))

typedef struct _CatalinaStorage        CatalinaStorage;
typedef struct _CatalinaStorageClass   CatalinaStorageClass;
typedef struct _CatalinaStoragePrivate CatalinaStoragePrivate;

struct _CatalinaStorage
{
	GObject parent;

	/*< private >*/
	CatalinaStoragePrivate *priv;
};

struct _CatalinaStorageClass
{
	GObjectClass parent_class;
};

GType            catalina_storage_get_type     (void);
CatalinaStorage* catalina_storage_new          (void);
void             catalina_storage_open_async   (CatalinaStorage      *storage,
                                                const gchar          *env_dir,
                                                const gchar          *name,
                                                GAsyncReadyCallback   callback,
                                                gpointer              user_data);
gboolean         catalina_storage_open_finish  (CatalinaStorage      *storage,
                                                GAsyncResult         *result,
						GError              **error);
gboolean         catalina_storage_open         (CatalinaStorage      *storage,
                                                const gchar          *env_dir,
                                                const gchar          *name,
                                                GError              **error);
void             catalina_storage_close_async  (CatalinaStorage      *storage,
                                                GAsyncReadyCallback   callback,
                                                gpointer              user_data);
gboolean         catalina_storage_close_finish (CatalinaStorage      *storage,
                                                GAsyncResult         *result,
                                                GError              **error);
gboolean         catalina_storage_close        (CatalinaStorage      *storage,
                                                GError              **error);
void             catalina_storage_get_async    (CatalinaStorage      *storage,
                                                const gchar          *key,
                                                gssize                key_length,
                                                GAsyncReadyCallback   callback,
                                                GError               **error);
gboolean         catalina_storage_get_finish   (CatalinaStorage      *storage,
                                                GAsyncResult         *result,
                                                gchar               **value,
                                                gsize                *value_length,
                                                GError              **error);
gboolean         catalina_storage_get          (CatalinaStorage      *storage,
                                                const gchar          *key,
                                                gssize                key_length,
                                                gchar               **value,
                                                gsize                *value_length);
void             catalina_storage_set_async    (CatalinaStorage      *storage,
                                                const gchar          *key,
                                                gssize                key_length,
                                                const gchar          *value,
                                                gssize                value_length,
                                                GAsyncReadyCallback   callback,
                                                gpointer              user_data);
gboolean         catalina_storage_set_finish   (CatalinaStorage      *storage,
                                                GAsyncResult *result,         GError **error);
gboolean         catalina_storage_set          (CatalinaStorage      *storage,
                                                const gchar          *key,
                                                gssize                key_length,
                                                const gchar          *value,
                                                gssize                value_length,
                                                GError              **error);

G_END_DECLS

#endif /* __CATALINA_STORAGE_H__ */
