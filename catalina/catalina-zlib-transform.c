/* catalina-zlib-transform.c
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

#include "catalina-zlib-transform.h"

/**
 * SECTION:catalina-zlib-transform
 * @title: CatalinaZlibTransform
 * @short_description: zlib compression transform
 *
 * Provides a #CatalinaTransform implementation that can read and write
 * zlib compressed data.
 */

static void catalina_zlib_transform_base_init (CatalinaTransformIface *iface);

G_DEFINE_TYPE_EXTENDED (CatalinaZlibTransform,
                        catalina_zlib_transform,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (CATALINA_TYPE_TRANSFORM,
                                               catalina_zlib_transform_base_init))

struct _CatalinaZlibTransformPrivate
{
	gpointer dummy;
};

static void
catalina_zlib_transform_get_property (GObject    *object,
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
catalina_zlib_transform_set_property (GObject      *object,
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
catalina_zlib_transform_finalize (GObject *object)
{
	G_OBJECT_CLASS (catalina_zlib_transform_parent_class)->finalize (object);
}

static void
catalina_zlib_transform_dispose (GObject *object)
{
}

static void
catalina_zlib_transform_class_init (CatalinaZlibTransformClass *klass)
{
	GObjectClass *object_class;

	g_type_class_add_private (klass, sizeof (CatalinaZlibTransformPrivate));

	object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = catalina_zlib_transform_set_property;
	object_class->get_property = catalina_zlib_transform_get_property;
	object_class->finalize     = catalina_zlib_transform_finalize;
	object_class->dispose      = catalina_zlib_transform_dispose;
}

static void
catalina_zlib_transform_init (CatalinaZlibTransform *zlib_transform)
{
	zlib_transform->priv = G_TYPE_INSTANCE_GET_PRIVATE (zlib_transform,
	                                                    CATALINA_TYPE_ZLIB_TRANSFORM,
	                                                    CatalinaZlibTransformPrivate);
}

/**
 * catalina_zlib_transform_new:
 *
 * Creates a new instance of #CatalinaZlibTransform.
 *
 * Return value: the newly created #CatalinaZlibTransform instance
 */
CatalinaTransform*
catalina_zlib_transform_new (void)
{
	return g_object_new (CATALINA_TYPE_ZLIB_TRANSFORM, NULL);
}

static gboolean
catalina_zlib_transform_real_read  (CatalinaTransform  *transform,
                                    const gchar        *input,
                                    gsize               input_length,
                                    gchar             **output,
                                    gsize              *output_length,
                                    GError            **error)
{
	*output_length = 0;
	return TRUE;
}

static gboolean
catalina_zlib_transform_real_write (CatalinaTransform  *transform,
                                     const gchar        *input,
                                     gsize               input_length,
                                     gchar             **output,
                                     gsize              *output_length,
                                     GError            **error)
{
	*output_length = 0;
	return TRUE;
}

static void
catalina_zlib_transform_base_init (CatalinaTransformIface *iface)
{
	iface->read = catalina_zlib_transform_real_read;
	iface->write = catalina_zlib_transform_real_write;
}
