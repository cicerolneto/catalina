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

#include <string.h>

#include "catalina-binary-formatter.h"

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

struct _CatalinaBinaryFormatterPrivate
{
	gpointer dummy;
};

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

#define BINARY_STRING (1 << 0)
#define BINARY_INT    (1 << 1)

static gboolean
catalina_binary_formatter_real_serialize (CatalinaFormatter  *formatter,
                                          const GValue       *value,
                                          gchar             **buffer,
                                          gsize              *buffer_length,
                                          GError            **error)
{
	GType  value_type;

	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	value_type = G_VALUE_TYPE (value);

	if (value_type == G_TYPE_INT) {
		guint tmp_gint;
		*buffer = g_malloc (1 + sizeof (gint));
		*buffer [0] = BINARY_INT;
		tmp_gint = g_htonl (g_value_get_int (value));
		memcpy (*buffer + 1, &tmp_gint, sizeof (gint));
	}
	else if (value_type == G_TYPE_STRING) {
		const gchar *tmp = g_value_get_string (value);
		glong  len = strlen (tmp);
		*buffer_length = 1 + len + 1;
		*buffer = g_malloc (*buffer_length);
		*buffer [0] = BINARY_STRING;
		*buffer [*buffer_length - 1] = '\0';
		memcpy (*buffer + 1, tmp, len);
	}
	else {
		g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
		             CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
		             "Do not know how to format type \"%s\"",
		             g_type_name (value_type));
		return FALSE;
	}

	

	return TRUE;
}

static gboolean
catalina_binary_formatter_real_deserialize (CatalinaFormatter  *formatter,
                                            GValue             *value,
                                            gchar              *buffer,
                                            gsize               buffer_length,
                                            GError            **error)
{
	return TRUE;
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
