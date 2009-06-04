/* catalina-transform.h
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

#ifndef __CATALINA_TRANSFORM_H__
#define __CATALINA_TRANSFORM_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define CATALINA_TYPE_TRANSFORM                (catalina_transform_get_type())    
#define CATALINA_TRANSFORM(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj),     CATALINA_TYPE_TRANSFORM, CatalinaTransform))     
#define CATALINA_IS_TRANSFORM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj),     CATALINA_TYPE_TRANSFORM))                        
#define CATALINA_TRANSFORM_GET_INTERFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj),  CATALINA_TYPE_TRANSFORM, CatalinaTransformIface))

typedef struct _CatalinaTransform        CatalinaTransform;
typedef struct _CatalinaTransformIface   CatalinaTransformIface;

struct _CatalinaTransformIface
{
	GTypeInterface parent;

	gboolean (*read)  (CatalinaTransform  *transform,
	                   const gchar        *input,
	                   gsize               input_length,
	                   gchar             **output,
	                   gsize              *output_length,
	                   GError            **error);

	gboolean (*write) (CatalinaTransform  *transform,
	                   const gchar        *input,
	                   gsize               input_length,
	                   gchar             **output,
	                   gsize              *output_length,
	                   GError            **error);
};

GType              catalina_transform_get_type (void);

gboolean           catalina_transform_read     (CatalinaTransform  *transform,
                                                const gchar        *input,
                                                gsize               input_length,
                                                gchar             **output,
                                                gsize              *output_length,
                                                GError            **error);

gboolean           catalina_transform_write    (CatalinaTransform  *transform,
                                                const gchar        *input,
                                                gsize               input_length,
                                                gchar             **output,
                                                gsize              *output_length,
                                                GError            **error);

G_END_DECLS

#endif /* __CATALINA_TRANSFORM_H__ */
