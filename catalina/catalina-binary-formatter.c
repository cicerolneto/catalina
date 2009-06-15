/* catalina-binary-formatter.c
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _ISOC99_SOURCE

#include <errno.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "catalina-binary-formatter.h"
#include "catalina-binary-formatter-private.h"

/**
 * SECTION:catalina-binary-formatter
 * @title: CatalinaBinaryFormatter
 * @short_description: binary serialization reader/writer
 *
 * Implements #CatalinaFormatter providing a compact binary serialization
 * format.
 */

static void catalina_binary_formatter_base_init (CatalinaFormatterIface *iface);

G_DEFINE_TYPE_EXTENDED (CatalinaBinaryFormatter,
                        catalina_binary_formatter,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (CATALINA_TYPE_FORMATTER,
                                               catalina_binary_formatter_base_init))

typedef struct {
	gchar *key;
	guint  key_length;
	gchar *value;
	guint  value_length;
} Chunk;

static gboolean
can_serialize_type (GType type)
{
	if (type == G_TYPE_NONE)
		return FALSE;
	else if (g_type_is_a (type, G_TYPE_OBJECT)) {
		return TRUE;
	}
	else if (type == G_TYPE_STRING)
		return TRUE;
	else if (type == G_TYPE_INT || type == G_TYPE_UINT)
		return TRUE;
	else if (type == G_TYPE_INT64 || type == G_TYPE_UINT64)
		return TRUE;
	else if (type == G_TYPE_LONG || type == G_TYPE_ULONG)
		return TRUE;
	else if (type == G_TYPE_DOUBLE || type == G_TYPE_FLOAT)
		return TRUE;
	else if (type == G_TYPE_CHAR || type == G_TYPE_UCHAR)
		return TRUE;
	else if (type == G_TYPE_BOOLEAN)
		return TRUE;
	else
		return FALSE;
}

static void
catalina_binary_formatter_get_property (GObject    *object,
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
catalina_binary_formatter_set_property (GObject      *object,
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
catalina_binary_formatter_finalize (GObject *object)
{
	G_OBJECT_CLASS (catalina_binary_formatter_parent_class)->finalize (object);
}

static void
catalina_binary_formatter_dispose (GObject *object)
{
}

static void
catalina_binary_formatter_class_init (CatalinaBinaryFormatterClass *klass)
{
	GObjectClass *object_class;

	g_type_class_add_private (klass, sizeof (CatalinaBinaryFormatterPrivate));

	object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = catalina_binary_formatter_set_property;
	object_class->get_property = catalina_binary_formatter_get_property;
	object_class->finalize     = catalina_binary_formatter_finalize;
	object_class->dispose      = catalina_binary_formatter_dispose;
}

static void
catalina_binary_formatter_init (CatalinaBinaryFormatter *binary_formatter)
{
	binary_formatter->priv = G_TYPE_INSTANCE_GET_PRIVATE (binary_formatter,
	                                                      CATALINA_TYPE_BINARY_FORMATTER,
	                                                      CatalinaBinaryFormatterPrivate);
}

static gboolean
catalina_binary_formatter_real_serialize (CatalinaFormatter  *formatter,
                                          const GValue       *value,
                                          gchar             **buffer,
                                          gsize              *buffer_length,
                                          GError            **error)
{
	gchar *tmp_buf = NULL;
	guint  tmp_len = 0;

	catalina_binary_formatter_write_value (CATALINA_BINARY_FORMATTER (formatter),
	                                       (GValue*)value,
	                                       &tmp_buf, &tmp_len, error);

	*buffer = g_malloc0 (2 + tmp_len);
	(*buffer) [0] = 0x01; /* Version */
	(*buffer) [1] = 0x00; /* Flags   */
	memcpy (*buffer + 2, tmp_buf, tmp_len);
	*buffer_length = tmp_len + 2;
	g_free (tmp_buf);

	return TRUE;
}

static gboolean
catalina_binary_formatter_real_deserialize (CatalinaFormatter  *formatter,
                                            GValue             *value,
                                            gchar              *buffer,
                                            gsize               buffer_length,
                                            GError            **error)
{
	gchar    *tmp_buf = NULL;
	guint     tmp_len = 0;

	if (buffer [0] != 0x01) {
		g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
		             CATALINA_BINARY_FORMATTER_ERROR_BAD_DATA,
		             "Serialization version %d is not supported",
		             (guchar)buffer [0]);
		return FALSE;
	}

	tmp_buf = buffer + 2;
	tmp_len = buffer_length - 2;

	return catalina_binary_formatter_read_value (CATALINA_BINARY_FORMATTER (formatter),
	                                             value, tmp_buf, tmp_len, error);
}

static void
catalina_binary_formatter_base_init (CatalinaFormatterIface *iface)
{
	iface->serialize = catalina_binary_formatter_real_serialize;
	iface->deserialize = catalina_binary_formatter_real_deserialize;
}

GQuark
catalina_binary_formatter_error_quark (void)
{
	return g_quark_from_static_string ("catalina-binary-formatter-error-quark");
}

/**
 * catalina_binary_formatter_new:
 *
 * Creates a new instance of the binary formatter.
 *
 * Return value: the newly created #CatalinaBinaryFormatter instance
 */
CatalinaFormatter*
catalina_binary_formatter_new (void)
{
	return g_object_new (CATALINA_TYPE_BINARY_FORMATTER, NULL);
}

/**
 * catalina_binary_formatter_write_boolean:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gboolean
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_boolean (CatalinaBinaryFormatter *formatter,
                                         gboolean                 value,
                                         gchar                  **buffer,
                                         guint                   *buffer_length,
                                         GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	*buffer = g_malloc0 (2);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_BOOLEAN;
	(*buffer) [1] = (guchar)value;
	*buffer_length = 2;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_boolean:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gboolean
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gboolean from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_boolean (CatalinaBinaryFormatter *formatter,
                                        gboolean                *value,
                                        gchar                   *buffer,
                                        guint                    buffer_length,
                                        GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 2, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_BOOLEAN)
		return FALSE;

	*value = buffer [1];
	return TRUE;
}

/**
 * catalina_binary_formatter_write_char:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gchar
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_char (CatalinaBinaryFormatter *formatter,
                                      gchar                    value,
                                      gchar                  **buffer,
                                      guint                   *buffer_length,
                                      GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	*buffer = g_malloc0 (2);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_CHAR;
	(*buffer) [1] = (guchar)value;
	*buffer_length = 2;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_char:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gchar
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gchar from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_char (CatalinaBinaryFormatter *formatter,
                                     gchar                   *value,
                                     gchar                   *buffer,
                                     guint                    buffer_length,
                                     GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 2, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_CHAR)
		return FALSE;

	*value = buffer [1];
	return TRUE;
}

/**
 * catalina_binary_formatter_write_uchar:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #guchar
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_uchar (CatalinaBinaryFormatter *formatter,
                                       guchar                   value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	*buffer = g_malloc0 (2);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_UCHAR;
	(*buffer) [1] = (guchar)value;
	*buffer_length = 2;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_uchar:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #guchar
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #guchar from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_uchar (CatalinaBinaryFormatter *formatter,
                                      guchar                  *value,
                                      gchar                   *buffer,
                                      guint                    buffer_length,
                                      GError                 **error)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 2, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_UCHAR)
		return FALSE;

	*value = buffer [1];
	return TRUE;
}

/**
 * catalina_binary_formatter_write_short:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gshort
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_short (CatalinaBinaryFormatter *formatter,
                                       gshort                   value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	gshort be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GINT16_TO_BE (value);

	*buffer = g_malloc0 (3);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_SHORT;
	memcpy ((*buffer) + 1, &be_value, 2);
	*buffer_length = 3;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_short:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gshort
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gshort from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_short (CatalinaBinaryFormatter *formatter,
                                      gshort                  *value,
                                      gchar                   *buffer,
                                      guint                    buffer_length,
                                      GError                 **error)
{
	gshort be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 3, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_SHORT)
		return FALSE;

	memcpy (&be_value, buffer + 1, 2);
	*value = GINT16_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_ushort:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gushort
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_ushort (CatalinaBinaryFormatter *formatter,
                                        gushort                  value,
                                        gchar                 **buffer,
                                        guint                   *buffer_length,
                                        GError                 **error)
{
	gushort be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GUINT16_TO_BE (value);

	*buffer = g_malloc0 (3);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_USHORT;
	memcpy ((*buffer) + 1, &be_value, 2);
	*buffer_length = 3;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_ushort:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gushort
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gushort from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_ushort (CatalinaBinaryFormatter *formatter,
                                       gushort                 *value,
                                       gchar                   *buffer,
                                       guint                    buffer_length,
                                       GError                 **error)
{
	gushort be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 3, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_USHORT)
		return FALSE;

	memcpy (&be_value, buffer + 1, 2);
	*value = GUINT16_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_int:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gint
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_int (CatalinaBinaryFormatter *formatter,
                                     gint                     value,
                                     gchar                  **buffer,
                                     guint                   *buffer_length,
                                     GError                 **error)
{
	gint be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GINT32_TO_BE (value);

	*buffer = g_malloc0 (5);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_INT;
	memcpy ((*buffer) + 1, &be_value, 4);
	*buffer_length = 5;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_int:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gint
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gint from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_int (CatalinaBinaryFormatter *formatter,
                                    gint                    *value,
                                    gchar                   *buffer,
                                    guint                    buffer_length,
                                    GError                 **error)
{
	gint be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 5, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_INT)
		return FALSE;

	memcpy (&be_value, buffer + 1, 4);
	*value = GINT32_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_uint:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #guint
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_uint (CatalinaBinaryFormatter *formatter,
                                      guint                    value,
                                      gchar                  **buffer,
                                      guint                   *buffer_length,
                                      GError                 **error)
{
	guint be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GUINT32_TO_BE (value);

	*buffer = g_malloc0 (5);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_UINT;
	memcpy ((*buffer) + 1, &be_value, 4);
	*buffer_length = 5;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_uint:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #guint
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #guint from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_uint (CatalinaBinaryFormatter *formatter,
                                     guint                   *value,
                                     gchar                   *buffer,
                                     guint                    buffer_length,
                                     GError                 **error)
{
	guint be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 5, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_UINT)
		return FALSE;

	memcpy (&be_value, buffer + 1, 4);
	*value = GUINT32_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_long:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #glong
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_long (CatalinaBinaryFormatter *formatter,
                                      glong                    value,
                                      gchar                  **buffer,
                                      guint                   *buffer_length,
                                      GError                 **error)
{
	glong be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GLONG_TO_BE (value);

	*buffer = g_malloc0 (9);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_LONG;
	memcpy ((*buffer) + 1, &be_value, 8);
	*buffer_length = 9;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_long:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #glong
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #glong from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_long (CatalinaBinaryFormatter *formatter,
                                     glong                   *value,
                                     gchar                   *buffer,
                                     guint                    buffer_length,
                                     GError                 **error)
{
	glong be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 9, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_LONG)
		return FALSE;

	memcpy (&be_value, buffer + 1, 8);
	*value = GLONG_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_ulong:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gulong
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_ulong (CatalinaBinaryFormatter *formatter,
                                       gulong                   value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	gulong be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GULONG_TO_BE (value);

	*buffer = g_malloc0 (9);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_ULONG;
	memcpy ((*buffer) + 1, &be_value, 8);
	*buffer_length = 9;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_ulong:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gulong
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gulong from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_ulong (CatalinaBinaryFormatter *formatter,
                                      gulong                  *value,
                                      gchar                   *buffer,
                                      guint                    buffer_length,
                                      GError                 **error)
{
	gulong be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 9, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_ULONG)
		return FALSE;

	memcpy (&be_value, buffer + 1, 8);
	*value = GULONG_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_int64:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gint64
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_int64 (CatalinaBinaryFormatter *formatter,
                                       gint64                   value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	gint64 be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GINT64_TO_BE (value);

	*buffer = g_malloc0 (9);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_INT64;
	memcpy ((*buffer) + 1, &be_value, 8);
	*buffer_length = 9;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_int64:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #gint64
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #gint64 from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_int64 (CatalinaBinaryFormatter *formatter,
                                      gint64                  *value,
                                      gchar                   *buffer,
                                      guint                    buffer_length,
                                      GError                 **error)
{
	gint64 be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 9, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_INT64)
		return FALSE;

	memcpy (&be_value, buffer + 1, 8);
	*value = GINT64_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_uint64:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #guint64
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_uint64 (CatalinaBinaryFormatter *formatter,
                                        guint64                  value,
                                        gchar                  **buffer,
                                        guint                   *buffer_length,
                                        GError                 **error)
{
	guint64 be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	be_value = GUINT64_TO_BE (value);

	*buffer = g_malloc0 (9);
	(*buffer) [0] = (guchar)BINARY_FORMATTER_TYPE_UINT64;
	memcpy ((*buffer) + 1, &be_value, 8);
	*buffer_length = 9;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_uint64:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a #guint64
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a #guint64 from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_uint64 (CatalinaBinaryFormatter *formatter,
                                       guint64                 *value,
                                       gchar                   *buffer,
                                       guint                    buffer_length,
                                       GError                 **error)
{
	guint64 be_value;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length == 9, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_UINT64)
		return FALSE;

	memcpy (&be_value, buffer + 1, 8);
	*value = GUINT64_FROM_BE (be_value);

	return TRUE;
}

/**
 * catalina_binary_formatter_write_string:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A string or %NULL
 * @value_length: the length of the string or -1 if it is %NULL terminated
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_string (CatalinaBinaryFormatter *formatter,
                                        gchar                   *value,
                                        gint                     value_length,
                                        gchar                  **buffer,
                                        guint                   *buffer_length,
                                        GError                 **error)
{
	guint length = 0;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	if (value && value_length == -1)
		length = strlen (value) + 1;
	else if (value_length > 0)
		length = value_length;

	*buffer_length = 1 + 4 + length;
	*buffer = g_malloc0 (*buffer_length);
	(*buffer) [0] = BINARY_FORMATTER_TYPE_STRING;

	if (value)
		memcpy ((*buffer) + 5, value, length);

	length = GUINT32_TO_BE (length);
	memcpy ((*buffer) + 1, &length, 4);

	return TRUE;
}

/**
 * catalina_binary_formatter_read_string:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A location for a string
 * @value_length: A location for the strings length
 * @buffer: the buffer to read
 * @buffer_length: the length of the buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes a string from @buffer and stores it at the location pointed
 * by @value.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_string (CatalinaBinaryFormatter  *formatter,
                                       gchar                   **value,
                                       guint                    *value_length,
                                       gchar                    *buffer,
                                       guint                     buffer_length,
                                       GError                  **error)
{
	guint length;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_STRING)
		return FALSE;

	memcpy (&length, buffer + 1, 4);
	length = GUINT32_FROM_BE (length);

	if (length + 5 != buffer_length) {
		return FALSE;
	}

	*value = g_malloc0 (length);
	memcpy (*value, buffer + 5, length);

	if (value_length)
		*value_length = length;

	return TRUE;
}

/**
 * catalina_binary_formatter_write_double:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gdouble
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_double (CatalinaBinaryFormatter *formatter,
                                        gdouble                  value,
                                        gchar                  **buffer,
                                        guint                   *buffer_length,
                                        GError                 **error)
{
	gchar *dbuf;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	dbuf = g_strdup_printf ("%g", value);
	if (!catalina_binary_formatter_write_string (formatter,
	                                             dbuf, -1,
	                                             buffer, buffer_length,
	                                             error)) {
		g_free (dbuf);
		return FALSE;
	}

	g_free (dbuf);
	(*buffer) [0] = BINARY_FORMATTER_TYPE_DOUBLE;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_double:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gdouble
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_double (CatalinaBinaryFormatter *formatter,
                                       gdouble                 *value,
                                       gchar                   *buffer,
                                       guint                    buffer_length,
                                       GError                 **error)
{
	gchar    *dbuf     = NULL;
	guint     dbuf_len = 0;
	gboolean  success  = FALSE;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	buffer [0] = BINARY_FORMATTER_TYPE_STRING;

	if (!catalina_binary_formatter_read_string (formatter,
	                                            &dbuf, &dbuf_len,
	                                            buffer, buffer_length,
	                                            error))
		goto cleanup;

	errno = 0;
	*value = strtod (dbuf, NULL);
	if (errno)
		goto cleanup;

	success = TRUE;

cleanup:
	buffer [0] = BINARY_FORMATTER_TYPE_DOUBLE;
	g_free (dbuf);

	return success;
}

/**
 * catalina_binary_formatter_write_float:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gfloat
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_float (CatalinaBinaryFormatter *formatter,
                                       gfloat                   value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	gchar *dbuf;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	dbuf = g_strdup_printf ("%g", value);
	if (!catalina_binary_formatter_write_string (formatter,
	                                             dbuf, -1,
	                                             buffer, buffer_length,
	                                             error)) {
		g_free (dbuf);
		return FALSE;
	}

	g_free (dbuf);
	(*buffer) [0] = BINARY_FORMATTER_TYPE_FLOAT;

	return TRUE;
}

/**
 * catalina_binary_formatter_read_float:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #gfloat
 * @buffer: A location for the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes @value into a buffer which is stored at the location @buffer.
 * The resulting buffer length is stored to @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_float (CatalinaBinaryFormatter *formatter,
                                      gfloat                  *value,
                                      gchar                   *buffer,
                                      guint                    buffer_length,
                                      GError                 **error)
{
	gchar    *dbuf     = NULL;
	guint     dbuf_len = 0;
	gboolean  success  = FALSE;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	buffer [0] = BINARY_FORMATTER_TYPE_STRING;

	if (!catalina_binary_formatter_read_string (formatter,
	                                            &dbuf, &dbuf_len,
	                                            buffer, buffer_length,
	                                            error))
		goto cleanup;

	errno = 0;
	*value = strtof (dbuf, NULL);
	if (errno)
		goto cleanup;

	success = TRUE;

cleanup:
	buffer [0] = BINARY_FORMATTER_TYPE_FLOAT;
	g_free (dbuf);

	return success;
}

/**
 * catalina_binary_formatter_write_object:
 * @formatter: A #CatalinaBinaryFormatter
 * @object: the object to serialize
 * @buffer: a location for the resulting buffer
 * @buffer_length: a location for the size of the resulting buffer
 * @error: a location for a #GError or %NULL
 *
 * Serialized @object and stores the resulting buffer into the location provided by
 * @buffer.  The buffer length is stored in @buffer_length.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_object (CatalinaBinaryFormatter  *formatter,
                                        GObject                  *object,
                                        gchar                   **buffer,
                                        guint                    *buffer_length,
                                        GError                  **error)
{
	GType         obj_type;
	const gchar  *type_name;
	guchar        type_name_len;
	gchar        *header;
	guint         header_length;
	GParamSpec  **params;
	guint         n_params;
	guint         total;
	guint         i;
	GList        *chunks = NULL,
		     *iter;

	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	obj_type = G_TYPE_FROM_INSTANCE (object);
	type_name = g_type_name (obj_type);
	type_name_len = strlen (type_name) + 1;

	header_length = 1 + 1 + type_name_len;
	header = g_malloc0 (header_length);
	header [0] = (gchar)BINARY_FORMATTER_TYPE_OBJECT;
	header [1] = (gchar)type_name_len;
	memcpy (header + 2, type_name, type_name_len);
	total += header_length;

	params = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_params);
	for (i = 0; i < n_params; i++) {
		if (can_serialize_type (params [i]->value_type)) {
			GValue  value = {0,};
			Chunk  *chunk = NULL;

			chunk = g_slice_new (Chunk);
			chunk->key = params [i]->name;
			chunk->key_length = strlen (chunk->key) + 1;

			g_value_init (&value, params [i]->value_type);
			g_object_get_property (object, chunk->key, &value);

			if (!catalina_binary_formatter_write_value (formatter,
			                                            &value,
			                                            &chunk->value,
			                                            &chunk->value_length,
			                                            error))
			{
				g_warning ("Skipping property \"%s\"", chunk->key);
				g_slice_free (Chunk, chunk);
				goto next_prop;
			}

			chunks = g_list_prepend (chunks, chunk);
			total += 1 + chunk->key_length + 4 + chunk->value_length;

		next_prop:
			g_value_unset (&value);
		}
	}

	/* null sentinal */
	total += 1;

	*buffer_length = total;
	*buffer = g_malloc0 (total);
	memcpy (*buffer, header, header_length);
	g_free (header);

	guint offset = header_length;

	for (iter = chunks; iter; iter = iter->next) {
		Chunk *chunk = iter->data;
		guint  be_length = GUINT32_TO_BE (chunk->value_length);

		(*buffer + offset) [0] = (guchar)chunk->key_length;
		offset++;
		memcpy ((*buffer) + offset, chunk->key, chunk->key_length);
		offset += chunk->key_length;
		memcpy ((*buffer) + offset, &be_length, 4);
		offset += 4;
		memcpy ((*buffer) + offset, chunk->value, chunk->value_length);
		offset += chunk->value_length;

		g_free (chunk->value);
		g_slice_free (Chunk, chunk);
	}

	return TRUE;
}

/**
 * catalina_binary_formatter_read_object:
 * @formatter: A #CatalinaBinaryFormatter
 * @object: a location for a #GObject
 * @buffer: the buffer to deserialize
 * @buffer_length: the length of @buffer
 * @error: A location for a #GError or %NULL
 *
 * Deserializes @buffer and generates a #GObject instances from it.  Upon
 * error, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_object (CatalinaBinaryFormatter  *formatter,
                                       GObject                 **object,
                                       gchar                    *buffer,
                                       guint                     buffer_length,
                                       GError                  **error)
{
	GType   type;
	gchar  *type_name,
	       *cursor;
	guchar  type_name_len;

	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);

	if (((guchar)buffer [0]) != BINARY_FORMATTER_TYPE_OBJECT)
		return FALSE;

	type_name_len = (guchar)buffer [1];
	type_name = g_malloc0 (type_name_len);
	memcpy (type_name, buffer + 2, type_name_len);
	type = g_type_from_name (type_name);
	g_free (type_name);

	if (!type || !g_type_is_a (type, G_TYPE_OBJECT))
		return FALSE;

	*object = g_object_new (type, NULL);

	cursor = buffer + 1 + 1 + type_name_len;

	while (cursor [0] != '\0') {
		GValue v = {0,};
		guchar prop_name_len = (guchar)cursor [0];
		cursor++;
		gchar *prop_name = g_malloc0 (prop_name_len);
		memcpy (prop_name, cursor, prop_name_len);
		cursor += prop_name_len;

		guint prop_value_len;
		memcpy (&prop_value_len, cursor, 4);
		prop_value_len = GUINT32_FROM_BE (prop_value_len);
		cursor += 4;

		if (!catalina_binary_formatter_read_value (formatter, &v, cursor, prop_value_len, error)) {
			/* FIXME: error handling */
			g_debug ("Prop name: %s", prop_name);
			g_error ("blah");
			g_debug ("Uh, error reading value (type_id %d)", cursor [0]);
		}

		g_object_set_property (*object, prop_name, &v);

		cursor += prop_value_len;

		g_free (prop_name);
	}

	return TRUE;
}

/**
 * catalina_binary_formatter_write_value:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #GValue
 * @buffer: a location to store the resulting buffer
 * @buffer_length: A location to store the resulting buffer length
 * @error: A location for a #GError or %NULL
 *
 * Serializes the real value stored in @value using the most appropriate
 * serialization method.  The resulting buffer is stored in @buffer.
 *
 * Upon error, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_write_value (CatalinaBinaryFormatter *formatter,
                                       GValue                  *value,
                                       gchar                  **buffer,
                                       guint                   *buffer_length,
                                       GError                 **error)
{
	GType type;

	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	type = G_VALUE_TYPE (value);

	if (type == G_TYPE_NONE)
		return FALSE;
	else if (type == G_TYPE_STRING)
		return catalina_binary_formatter_write_string (formatter, (gchar*)g_value_get_string (value), -1, buffer, buffer_length, error);
	else if (type == G_TYPE_INT)
		return catalina_binary_formatter_write_int (formatter, g_value_get_int (value), buffer, buffer_length, error);
	else if (type == G_TYPE_UINT)
		return catalina_binary_formatter_write_uint (formatter, g_value_get_uint (value), buffer, buffer_length, error);
	else if (type == G_TYPE_BOOLEAN)
		return catalina_binary_formatter_write_boolean (formatter, g_value_get_boolean (value), buffer, buffer_length, error);
	else if (type == G_TYPE_LONG)
		return catalina_binary_formatter_write_long (formatter, g_value_get_long (value), buffer, buffer_length, error);
	else if (type == G_TYPE_ULONG)
		return catalina_binary_formatter_write_ulong (formatter, g_value_get_ulong (value), buffer, buffer_length, error);
	else if (type == G_TYPE_INT64)
		return catalina_binary_formatter_write_int64 (formatter, g_value_get_int64 (value), buffer, buffer_length, error);
	else if (type == G_TYPE_UINT64)
		return catalina_binary_formatter_write_uint64 (formatter, g_value_get_uint64 (value), buffer, buffer_length, error);
	else if (type == G_TYPE_DOUBLE)
		return catalina_binary_formatter_write_double (formatter, g_value_get_double (value), buffer, buffer_length, error);
	else if (type == G_TYPE_FLOAT)
		return catalina_binary_formatter_write_float (formatter, g_value_get_float (value), buffer, buffer_length, error);
	else if (type == G_TYPE_CHAR)
		return catalina_binary_formatter_write_char (formatter, g_value_get_char (value), buffer, buffer_length, error);
	else if (type == G_TYPE_UCHAR)
		return catalina_binary_formatter_write_uchar (formatter, g_value_get_uchar (value), buffer, buffer_length, error);
	else if (g_type_is_a (type, G_TYPE_OBJECT))
		return catalina_binary_formatter_write_object (formatter, g_value_get_object (value), buffer, buffer_length, error);

	return FALSE;
}

/**
 * catalina_binary_formatter_read_value:
 * @formatter: A #CatalinaBinaryFormatter
 * @value: A #GValue
 * @buffer: the buffer to deserialize
 * @buffer_length: the length of @buffer
 * @error: a location for a #GError or %NULL
 *
 * Attempts to determine the data-type stored within the buffer and
 * deserializes it.  The resulting deserialized value is stored within
 * @value.
 *
 * Upon error, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE on success
 */
gboolean
catalina_binary_formatter_read_value (CatalinaBinaryFormatter  *formatter,
                                      GValue                   *value,
                                      gchar                    *buffer,
                                      guint                     buffer_length,
                                      GError                  **error)
{
	guchar   type_id;
	gboolean success = FALSE;

	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);

	type_id = buffer [0];

	if (type_id == BINARY_FORMATTER_TYPE_BOOLEAN) {
		gboolean v = FALSE;
		success = catalina_binary_formatter_read_boolean (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_BOOLEAN);
		g_value_set_boolean (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_INT64) {
		gint64 v = 0;
		success = catalina_binary_formatter_read_int64 (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_INT64);
		g_value_set_int64 (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_UINT64) {
		guint64 v = 0;
		success = catalina_binary_formatter_read_uint64 (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_UINT64);
		g_value_set_uint64 (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_LONG) {
		glong v = 0;
		success = catalina_binary_formatter_read_long (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_LONG);
		g_value_set_long (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_ULONG) {
		gulong v = 0;
		success = catalina_binary_formatter_read_ulong (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_ULONG);
		g_value_set_ulong (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_INT) {
		gint v = 0;
		success = catalina_binary_formatter_read_int (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_INT);
		g_value_set_int (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_UINT) {
		guint v = 0;
		success = catalina_binary_formatter_read_uint (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_UINT);
		g_value_set_uint (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_SHORT) {
		gshort v = 0;
		success = catalina_binary_formatter_read_short (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_INT);
		g_value_set_int (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_USHORT) {
		gushort v = 0;
		success = catalina_binary_formatter_read_ushort (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_INT);
		g_value_set_int (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_CHAR) {
		gchar v = 0;
		success = catalina_binary_formatter_read_char (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_CHAR);
		g_value_set_char (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_UCHAR) {
		guchar v = 0;
		success = catalina_binary_formatter_read_uchar (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_UCHAR);
		g_value_set_uchar (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_DOUBLE) {
		gdouble v = 0;
		success = catalina_binary_formatter_read_double (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_DOUBLE);
		g_value_set_double (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_FLOAT) {
		gfloat v = 0;
		success = catalina_binary_formatter_read_float (formatter, &v, buffer, buffer_length, error);
		g_value_init (value, G_TYPE_FLOAT);
		g_value_set_float (value, v);
	}
	else if (type_id == BINARY_FORMATTER_TYPE_STRING) {
		gchar *v = NULL;
		guint  l = 0;
		success = catalina_binary_formatter_read_string (formatter, &v, &l, buffer, buffer_length, error);
		if (v && strlen(v) + 1 == l) {
			g_value_init (value, G_TYPE_STRING);
			g_value_take_string (value, v);
		}
		else if (v) {
			/* not a null terminated string */
		}
		else {
			g_value_init (value, G_TYPE_STRING);
			g_value_set_string (value, NULL);
		}
	}
	else if (type_id == BINARY_FORMATTER_TYPE_OBJECT) {
		GObject *v = NULL;
		success = catalina_binary_formatter_read_object (formatter, &v, buffer, buffer_length, error);
		if (v) {
			g_value_init (value, G_TYPE_FROM_INSTANCE (v));
			g_value_take_object (value, v);
		}
	}
	else {
		/*
		g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
		             CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
		             "No such type %d", type_id);
		*/
		return FALSE;
	}

	return success;
}
