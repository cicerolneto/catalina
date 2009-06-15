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
 * @CATALINA_BINARY_FORMATTER_ERROR_BAD_DATA: The data within the buffer was invalid.
 *
 * #CatalinaBinaryFormatter error enumeration.
 */
typedef enum {
	CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
	CATALINA_BINARY_FORMATTER_ERROR_BAD_DATA,
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

GType              catalina_binary_formatter_get_type      (void);
CatalinaFormatter* catalina_binary_formatter_new           (void);
GQuark             catalina_binary_formatter_error_quark   (void);
gboolean           catalina_binary_formatter_write_boolean (CatalinaBinaryFormatter *formatter,
                                                            gboolean                 value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_boolean  (CatalinaBinaryFormatter *formatter,
                                                            gboolean                *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_char    (CatalinaBinaryFormatter *formatter,
                                                            gchar                    value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_char     (CatalinaBinaryFormatter *formatter,
                                                            gchar                   *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_uchar   (CatalinaBinaryFormatter *formatter,
                                                            guchar                   value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_uchar    (CatalinaBinaryFormatter *formatter,
                                                            guchar                  *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_short   (CatalinaBinaryFormatter *formatter,
                                                            gshort                   value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_short    (CatalinaBinaryFormatter *formatter,
                                                            gshort                  *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_ushort  (CatalinaBinaryFormatter *formatter,
                                                            gushort                  value,
                                                            gchar                 **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_ushort   (CatalinaBinaryFormatter *formatter,
                                                            gushort                 *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_int     (CatalinaBinaryFormatter *formatter,
                                                            gint                     value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_int      (CatalinaBinaryFormatter *formatter,
                                                            gint                    *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_uint (CatalinaBinaryFormatter *formatter,
                                                            guint                    value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_uint     (CatalinaBinaryFormatter *formatter,
                                                            guint                   *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_long    (CatalinaBinaryFormatter *formatter,
                                                            glong                    value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_long     (CatalinaBinaryFormatter *formatter,
                                                            glong                   *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_ulong   (CatalinaBinaryFormatter *formatter,
                                                            gulong                   value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_ulong    (CatalinaBinaryFormatter *formatter,
                                                            gulong                  *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_int64   (CatalinaBinaryFormatter *formatter,
                                                            gint64                   value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_int64    (CatalinaBinaryFormatter *formatter,
                                                            gint64                  *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_uint64  (CatalinaBinaryFormatter *formatter,
                                                            guint64                  value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_uint64  (CatalinaBinaryFormatter *formatter,
                                                            guint64                 *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_string  (CatalinaBinaryFormatter *formatter,
                                                            gchar                   *value,
                                                            gint                     value_length,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_string   (CatalinaBinaryFormatter  *formatter,
                                                            gchar                   **value,
                                                            guint                    *value_length,
                                                            gchar                    *buffer,
                                                            guint                     buffer_length,
                                                            GError                  **error);
gboolean           catalina_binary_formatter_write_double  (CatalinaBinaryFormatter *formatter,
                                                            gdouble                  value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_double   (CatalinaBinaryFormatter *formatter,
                                                            gdouble                 *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_float   (CatalinaBinaryFormatter *formatter,
                                                            gfloat                   value,
                                                            gchar                  **buffer,
                                                            guint                   *buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_read_float    (CatalinaBinaryFormatter *formatter,
                                                            gfloat                  *value,
                                                            gchar                   *buffer,
                                                            guint                    buffer_length,
                                                            GError                 **error);
gboolean           catalina_binary_formatter_write_object  (CatalinaBinaryFormatter  *formatter,
                                                            GObject                  *object,
                                                            gchar                   **buffer,
                                                            guint                    *buffer_length,
                                                            GError                  **error);
gboolean           catalina_binary_formatter_read_object   (CatalinaBinaryFormatter  *formatter,
                                                            GObject                 **object,
                                                            gchar                    *buffer,
                                                            guint                     buffer_length,
                                                            GError                  **error);
gboolean           catalina_binary_formatter_write_value   (CatalinaBinaryFormatter  *formatter,
                                                            GValue                   *value,
                                                            gchar                   **buffer,
                                                            guint                    *buffer_length,
                                                            GError                  **error);
gboolean           catalina_binary_formatter_read_value    (CatalinaBinaryFormatter  *formatter,
                                                            GValue                   *value,
                                                            gchar                    *buffer,
                                                            guint                     buffer_length,
                                                            GError                  **error);

G_END_DECLS

#endif /* __CATALINA_BINARY_FORMATTER_H__ */
