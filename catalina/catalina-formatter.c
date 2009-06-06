/* catalina-formatter.c
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

#include "catalina-formatter.h"

/**
 * SECTION:catalina-formatter
 * @title: CatalinaFormatter
 * @short_description: serialization for glib types
 *
 * #CatalinaFormatter provides a serialization format for converting
 * data-types from glib and gobject into data-streams.  This is particularly
 * useful for converting objects, integers, and strings into content to be
 * stored within #CatalinaStorage.
 *
 *
 * You can add a formatter to a #CatalinaStorage for transparent serialization
 * and deserializtion to and from storage.
 *
 * #CatalinaFormatter implementations must be thread safe for the interface methods.
 *
 * |[
 * CatalinaStorage *storage = catalina_storage_new ();
 * g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
 * ]|
 */

GType
catalina_formatter_get_type (void)
{
	static GType formatter_type = 0;

	if (G_UNLIKELY (!formatter_type)) {
		const GTypeInfo formatter_type_info = {
			sizeof (CatalinaFormatterIface),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};

		formatter_type = g_type_register_static (G_TYPE_INTERFACE,
		                                         "CatalinaFormatter",
		                                         &formatter_type_info,
		                                         0);
		g_type_interface_add_prerequisite (formatter_type,
		                                   G_TYPE_OBJECT);
	}

	return formatter_type;
}

/**
 * catalina_formatter_serialize:
 * @formatter: A #CatalinaFormatter
 * @value: A #GValue
 * @buffer: A #gchar
 * @buffer_length: A #gsize
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_formatter_serialize (CatalinaFormatter  *formatter,
                              const GValue       *value,
                              gchar             **buffer,
                              gsize              *buffer_length,
                              GError            **error)
{
	return CATALINA_FORMATTER_GET_INTERFACE (formatter)->serialize
		(formatter, value, buffer, buffer_length, error);
}

/**
 * catalina_formatter_deserialize:
 * @formatter: A #CatalinaFormatter
 * @value: A #GValue
 * @buffer: A #gchar
 * @buffer_length: A #gsize
 * @error: A #GError
 *
 * Return value: 
 */
gboolean
catalina_formatter_deserialize (CatalinaFormatter  *formatter,
                                GValue             *value,
                                gchar              *buffer,
                                gsize               buffer_length,
                                GError            **error)
{
	return CATALINA_FORMATTER_GET_INTERFACE (formatter)->deserialize
		(formatter, value, buffer, buffer_length, error);
}
