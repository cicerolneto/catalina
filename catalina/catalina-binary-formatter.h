/* catalina-binary-formatter.h
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

#ifndef __CATALINA_BINARY_FORMATTER_H__
#define __CATALINA_BINARY_FORMATTER_H__

#include <glib-object.h>

#include "catalina-formatter.h"

G_BEGIN_DECLS

#define CATALINA_TYPE_BINARY_FORMATTER            (catalina_binary_formatter_get_type())
#define CATALINA_BINARY_FORMATTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),    CATALINA_TYPE_BINARY_FORMATTER, CatalinaBinaryFormatter))     
#define CATALINA_BINARY_FORMATTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),     CATALINA_TYPE_BINARY_FORMATTER, CatalinaBinaryFormatterClass))
#define CATALINA_IS_BINARY_FORMATTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),    CATALINA_TYPE_BINARY_FORMATTER))                              
#define CATALINA_IS_BINARY_FORMATTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),     CATALINA_TYPE_BINARY_FORMATTER))                              
#define CATALINA_BINARY_FORMATTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),     CATALINA_TYPE_BINARY_FORMATTER, CatalinaBinaryFormatterClass))

/**
 * CATALINA_BINARY_FORMATTER_ERROR:
 *
 * #CatalinaBinaryFormatter #GError domain.
 */
#define CATALINA_BINARY_FORMATTER_ERROR (catalina_binary_formatter_error_quark ())

/**
 * CatalinaBinaryFormatterError:
 * @CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE: The formatter could not determine how to format the rquested GType.
 *
 * #CatalinaBinaryFormatter error enumeration.
 */
typedef enum {
	CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
} CatalinaBinaryFormatterError;

typedef struct _CatalinaBinaryFormatter        CatalinaBinaryFormatter;
typedef struct _CatalinaBinaryFormatterClass   CatalinaBinaryFormatterClass;
typedef struct _CatalinaBinaryFormatterPrivate CatalinaBinaryFormatterPrivate;

struct _CatalinaBinaryFormatter
{
	GObject parent;

	/*< private >*/
	CatalinaBinaryFormatterPrivate *priv;
};

struct _CatalinaBinaryFormatterClass
{
	GObjectClass parent_class;
};

GType              catalina_binary_formatter_get_type    (void);
CatalinaFormatter* catalina_binary_formatter_new         (void);
GQuark             catalina_binary_formatter_error_quark (void);

G_END_DECLS

#endif /* __CATALINA_BINARY_FORMATTER_H__ */
