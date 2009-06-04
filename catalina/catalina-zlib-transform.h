/* catalina-zlib-transform.h
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

#ifndef __CATALINA_ZLIB_TRANSFORM_H__
#define __CATALINA_ZLIB_TRANSFORM_H__

#include <glib-object.h>

#include "catalina-transform.h"

G_BEGIN_DECLS

#define CATALINA_TYPE_ZLIB_TRANSFORM            (catalina_zlib_transform_get_type())
#define CATALINA_ZLIB_TRANSFORM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),  CATALINA_TYPE_ZLIB_TRANSFORM, CatalinaZlibTransform))     
#define CATALINA_ZLIB_TRANSFORM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),   CATALINA_TYPE_ZLIB_TRANSFORM, CatalinaZlibTransformClass))
#define CATALINA_IS_ZLIB_TRANSFORM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),  CATALINA_TYPE_ZLIB_TRANSFORM))                            
#define CATALINA_IS_ZLIB_TRANSFORM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),   CATALINA_TYPE_ZLIB_TRANSFORM))                            
#define CATALINA_ZLIB_TRANSFORM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),   CATALINA_TYPE_ZLIB_TRANSFORM, CatalinaZlibTransformClass))

typedef struct _CatalinaZlibTransform        CatalinaZlibTransform;
typedef struct _CatalinaZlibTransformClass   CatalinaZlibTransformClass;
typedef struct _CatalinaZlibTransformPrivate CatalinaZlibTransformPrivate;

struct _CatalinaZlibTransform
{
	GObject parent;

	/*< private >*/
	CatalinaZlibTransformPrivate *priv;
};

struct _CatalinaZlibTransformClass
{
	GObjectClass parent_class;
};

GType              catalina_zlib_transform_get_type (void);
CatalinaTransform* catalina_zlib_transform_new      (void);

G_END_DECLS

#endif /* __CATALINA_ZLIB_TRANSFORM_H__ */
