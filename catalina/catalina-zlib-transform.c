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

#include <string.h>
#include <zlib.h>

#include "catalina-zlib-transform.h"

#define CHUNK 16384

/**
 * SECTION:catalina-zlib-transform
 * @title: CatalinaZlibTransform
 * @short_description: zlib compression transform
 *
 * Provides a #CatalinaTransform implementation that can read and write
 * zlib compressed data.
 *
 * The compressed data includes as little as 1 byte and as many as 5 bytes on the tail end of the
 * compressed data.  The consumer of this will never know however, as the transform hides this
 * detail.  The last byte is always attached and it is 1 if the stream was compressed and 0 if the
 * stream was not compressed.  This is important as some data-sets are small enough where
 * compression does not make sense and actually slows processing down.  You can tune this to your
 * data-set using the "watermark" property.  The input length must be greater than the watermark to
 * be compressed.  The default is 0.
 *
 * If the data is compressed, the 4 bytes previous to the last byte will contain the length of the
 * uncompressed buffer.  This is so that we may do a single allocation on decompression to save
 * ourselves lots of allocations and joining into a single buffer later.
 */

enum
{
	PROP_0,
	PROP_LEVEL,
	PROP_WATERMARK,
};

static void catalina_zlib_transform_base_init (CatalinaTransformIface *iface);

G_DEFINE_TYPE_EXTENDED (CatalinaZlibTransform,
                        catalina_zlib_transform,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (CATALINA_TYPE_TRANSFORM,
                                               catalina_zlib_transform_base_init))

struct _CatalinaZlibTransformPrivate
{
	gsize watermark;
	guint level;
};

/**
 * catalina_zlib_transform_set_level:
 * @transform: A #CatalinaZlibTransform
 * @level: the compression level
 *
 * Sets the compression level to use by the zlib compressor.
 */
void
catalina_zlib_transform_set_level (CatalinaZlibTransform *transform,
                                   gint                   level)


{
	g_return_if_fail (CATALINA_IS_ZLIB_TRANSFORM (transform));
	transform->priv->level = level;
}

/**
 * catalina_zlib_transform_set_watermark:
 * @transform: A #CatalinaZlibTransform
 * @level: the watermark
 *
 * Sets the watermark to use by the zlib compressor.
 *
 * If the amount of data to be compressed is smaller than @watermark in bytes, then the data will
 * not be compressed.  This is useful for small objects where the size compressed is larger than the
 * data lone.
 */
void
catalina_zlib_transform_set_watermark (CatalinaZlibTransform *transform,
                                       gint                   watermark)
{
	g_return_if_fail (CATALINA_IS_ZLIB_TRANSFORM (transform));
	transform->priv->watermark = watermark;
}

static void
catalina_zlib_transform_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
	switch (property_id) {
	case PROP_LEVEL:
		g_value_set_int (value, catalina_zlib_transform_get_level (CATALINA_ZLIB_TRANSFORM (object)));
		break;
	case PROP_WATERMARK:
		g_value_set_int (value, catalina_zlib_transform_get_watermark (CATALINA_ZLIB_TRANSFORM (object)));
		break;
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
	case PROP_LEVEL:
		catalina_zlib_transform_set_level (CATALINA_ZLIB_TRANSFORM (object), g_value_get_int (value));
		break;
	case PROP_WATERMARK:
		catalina_zlib_transform_set_watermark (CATALINA_ZLIB_TRANSFORM (object), g_value_get_int (value));
		break;
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

	/**
	 * CatalinaZlibTransform:level:
	 *
	 * The zlib compression level.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_LEVEL,
	                                 g_param_spec_int ("level",
	                                                   "Level",
	                                                   "The zlib compression level",
	                                                   0,
	                                                   9,
	                                                   6,
	                                                   G_PARAM_READWRITE));

	/**
	 * CatalinaZlibTransform:watermark:
	 *
	 * The watermark (in bytes) for compression.  The buffer must be larger than the watermark
	 * before it will be compressed.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_WATERMARK,
	                                 g_param_spec_int ("watermark",
	                                                   "Watermark",
	                                                   "The compression watermark",
	                                                   0,
	                                                   G_MAXINT,
	                                                   0,
	                                                   G_PARAM_READWRITE));
}

static void
catalina_zlib_transform_init (CatalinaZlibTransform *zlib_transform)
{
	zlib_transform->priv = G_TYPE_INSTANCE_GET_PRIVATE (zlib_transform,
	                                                    CATALINA_TYPE_ZLIB_TRANSFORM,
	                                                    CatalinaZlibTransformPrivate);

	zlib_transform->priv->level = 6;
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
	int        ret;
	z_stream   strm;
	guint      length;

	g_return_val_if_fail (output != NULL, FALSE);
	g_return_val_if_fail (output_length != NULL, FALSE);

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	/* if the last byte is 0 (FALSE) then the buffer was not compressed and we can
	 * simply copy the buffer -1 byte.  We chould probably take a shortcut here and
	 * just set *output_length = 0 since most stuff will be strings or ignore the
	 * essentially null pad at the end, but for now to be sure we will not.
	 */
	if (input [input_length - 1] == '\0') {
		*output_length = input_length - 1;
		*output = g_malloc (*output_length);
		memcpy (*output, input, *output_length);
		return TRUE;
	}

	if ((ret = inflateInit(&strm)) != Z_OK) {
		g_set_error (error, CATALINA_ZLIB_TRANSFORM_ERROR,
		             CATALINA_ZLIB_TRANSFORM_ERROR_ZLIB,
		             "There was an error initializing Zlib");
		return FALSE;
	}

	memcpy (&length, input + input_length - 5, 4);
	length = GUINT32_FROM_BE (length);
	*output = g_malloc (length);

	/* we ignore  the length and was-compressed flag */
	strm.avail_in = input_length - 5;
	strm.next_in = (guchar*)input;
	strm.avail_out = length;
	strm.next_out = (guchar*)*output;

	if ((ret = inflate (&strm, 0)) != Z_STREAM_END) {
		g_error ("Did not get Z_STREAM_END, %d ... %d", ret, Z_STREAM_ERROR);
	}

	(void)inflateEnd(&strm);

	*output_length = length;
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
	int       ret;
	gulong    have;
	z_stream  strm;
	gint      level;
	GSList   *parts    = NULL,
		 *iter;
	gpointer  part;
	guint     watermark,
	          length   = 0,
	          belength = 0,
	          offset   = 0;
	gboolean  success  = FALSE;

	g_return_val_if_fail (output != NULL, FALSE);
	g_return_val_if_fail (output_length != NULL, FALSE);

	watermark = ((CatalinaZlibTransform*)transform)->priv->watermark;

	/* only compress if the input buffer is large enough */
	if (input_length <= watermark) {
		*output = g_malloc0 (input_length + 1);
		*output_length = input_length + 1;
		memcpy (*output, input, input_length);
		(*output) [input_length] = (guchar)FALSE;
		return TRUE;
	}

	/* get the zlib compression level */
	level = ((CatalinaZlibTransform*)transform)->priv->level;

	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;

	if ((ret = deflateInit (&strm, level) != Z_OK)) {
		g_set_error (error, CATALINA_ZLIB_TRANSFORM_ERROR,
		             CATALINA_ZLIB_TRANSFORM_ERROR_ZLIB,
		             "There was an error initializing Zlib");
		return FALSE;
	}

	/* compress until end of input */
	strm.avail_in = input_length;
	strm.next_in = (guchar*)input;

	do {
		strm.avail_out = CHUNK;
		strm.next_out = part = g_malloc (CHUNK);

		if ((ret = deflate (&strm, Z_FINISH) == Z_STREAM_ERROR)) {
			g_free (part);
			goto cleanup;
		}

		have = CHUNK - strm.avail_out;
		parts = g_slist_prepend (parts, part);
		length += have;
	} while (strm.avail_out == 0);
	g_assert (ret == Z_OK);
	g_assert (strm.avail_in == 0);

	/* cleanup and finish */
	(void)deflateEnd(&strm);

	guchar compressed = TRUE;

	/* set the tail on the end */
	belength = GUINT32_TO_BE (input_length);
	*output_length = length + 5;
	*output = g_malloc (*output_length);
	memcpy (((*output + *output_length) - 5), &belength, 4);
	memcpy (((*output + *output_length) - 1), &compressed, 1);

	/* copy the resulting chunks into the output buffer */
	parts = g_slist_reverse (parts);
	for (iter = parts; iter; iter = iter->next) {
		if (length <= CHUNK) {
			memcpy (*output, iter->data, length);
			break;
		}

		have = length - offset;
		if (have > CHUNK)
			have = CHUNK;

		memcpy ((*output + offset), iter->data, have);
		offset += have;
		g_free (iter->data);
	}

	success = TRUE;

cleanup:
	g_slist_free (parts);

	if (!success) {
		g_set_error (error, CATALINA_ZLIB_TRANSFORM_ERROR,
		             CATALINA_ZLIB_TRANSFORM_ERROR_ZLIB,
		             "There was an internal error within zlib during compression");
	}

	return success;
}

static void
catalina_zlib_transform_base_init (CatalinaTransformIface *iface)
{
	iface->read = catalina_zlib_transform_real_read;
	iface->write = catalina_zlib_transform_real_write;
}

GQuark
catalina_zlib_transform_error_quark (void)
{
	return g_quark_from_static_string ("catalina-zlib-transform-error-quark");
}

/**
 * catalina_zlib_transform_get_level:
 * @transform: A #CatalinaZlibTransform
 *
 * Retrieves the current compression level.
 *
 * Return value: a #gint containing the compression level
 */
gint
catalina_zlib_transform_get_level (CatalinaZlibTransform *transform)
{
	g_return_val_if_fail (CATALINA_IS_ZLIB_TRANSFORM (transform), 0);
	return transform->priv->level;
}

/**
 * catalina_zlib_transform_get_watermark:
 * @transform: A #CatalinaZlibTransform
 *
 * Retrieves the current watermark.  The watermark is used to prevent compression of objects
 * smaller than or equal to the level in bytes.
 *
 * Return value: a #gint containing the watermark
 */
gint
catalina_zlib_transform_get_watermark (CatalinaZlibTransform *transform)
{
	g_return_val_if_fail (CATALINA_IS_ZLIB_TRANSFORM (transform), 0);
	return transform->priv->watermark;
}
