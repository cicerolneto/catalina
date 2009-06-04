/* catalina-transform.c
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

#include "catalina-transform.h"

/**
 * SECTION:catalina-transform
 * @title: CatalinaTransform
 * @short_description: buffer transforms
 *
 * #CatalinaTransform provides a way to transform data buffers on the way
 * to and from storage.  A common use-case for this would be to add either
 * compression or encryption.
 */

GType
catalina_transform_get_type (void)
{
	static GType transform_type = 0;

	if (G_UNLIKELY (!transform_type)) {
		const GTypeInfo transform_type_info = {
			sizeof (CatalinaTransformIface),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};

		transform_type = g_type_register_static (G_TYPE_INTERFACE,
		                                         "CatalinaTransform",
		                                         &transform_type_info,
		                                         0);
		g_type_interface_add_prerequisite (transform_type,
		                                   G_TYPE_OBJECT);
	}

	return transform_type;
}


/**
 * catalina_transform_read:
 * @transform: A #CatalinaTransform
 * @gchar input:
 * @input_length:
 * @output:
 * @output_length:
 * @error:
 *
 * 
 *
 * Return value: 
 */
gboolean
catalina_transform_read (CatalinaTransform  *transform,
                         const gchar        *input,
                         gsize               input_length,
                         gchar             **output,
                         gsize              *output_length,
                         GError            **error)
{
	return CATALINA_TRANSFORM_GET_INTERFACE (transform)->read
		(transform, input, input_length, output, output_length, error);
}

/**
 * catalina_transform_write:
 * @transform: A #CatalinaTransform
 * @gchar input:
 * @input_length:
 * @output:
 * @output_length:
 * @error:
 *
 * 
 *
 * Return value: 
 */
gboolean
catalina_transform_write (CatalinaTransform  *transform,
                          const              gchar *input,
                          gsize               input_length,
                          gchar             **output,
                          gsize              *output_length,
                          GError            **error)
{
	return CATALINA_TRANSFORM_GET_INTERFACE (transform)->write
		(transform, input, input_length, output, output_length, error);
}

