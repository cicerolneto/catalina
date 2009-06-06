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
static void g_type_serializer_init              (void);

static GHashTable *type_id_funcs = NULL;
static GHashTable *type_funcs    = NULL;

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

	g_type_serializer_init ();
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

typedef struct
{
	guchar   type_id;
	GType    g_type;
	gsize    (*get_length)  (guchar type_id, const GValue *value);
	gboolean (*serialize)   (guchar type_id, const GValue *value,       gchar *buffer, GError **error);
	gboolean (*deserialize) (guchar type_id,       GValue *value, const gchar *buffer, GError **error);
} GTypeSerializer;

static gsize
int_get_length (guchar        type_id,
                const GValue *value)
{
	return 4;
}

static gsize
long_get_length (guchar        type_id,
                 const GValue *value)
{
	return 8;
}

static gsize
string_get_length (guchar        type_id,
                   const GValue *value)
{
	const gchar *s = g_value_get_string (value);
	return (s != NULL) ? strlen (s) + 1 : 1;
}

static GTypeSerializer*
get_serializer_for_gtype (GType type)
{
	GTypeSerializer *s;

	gpointer       key, value;
	GHashTableIter iter;

	g_hash_table_iter_init (&iter, type_funcs);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		s = value;
		if (s != NULL && ((s->g_type == type) || g_type_is_a (s->g_type, type)))
			return s;
	}

	return NULL;
}

static GTypeSerializer*
get_serializer_for_type_id (guchar type_id)
{
	GTypeSerializer *s;
	gpointer         key, value;
	GHashTableIter   iter;

	g_hash_table_iter_init (&iter, type_funcs);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		s = value;
		if (s != NULL && s->type_id == type_id)
			return s;
	}

	return NULL;
}

static gsize
object_get_length (guchar        type_id,
                   const GValue *value)
{
	gsize        total  = 0;
	gsize        tmp    = 0;
	GType        type   = G_VALUE_TYPE (value);
	GObject     *object = g_value_get_object (value);
	GParamSpec **params;
	guint        n_props;
	gint         i;

	/* make sure name length is < 256 */
	if ((tmp = strlen (g_type_name (type)) + 1) > 0xFF)
		return 0;
	total += 1 + tmp;

	params = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_props);
	if (n_props == 0) {
		g_free (params);
		total += 1;
		return total;
	}

	for (i = 0; i < n_props; i++) {
		GTypeSerializer *s;
		GValue v = {0,};
		if ((s = get_serializer_for_gtype (G_PARAM_SPEC_VALUE_TYPE (params [i]))) != NULL) {
			total += 1 + strlen (g_param_spec_get_name (params [i])) + 1;
			g_object_get_property (object, g_param_spec_get_name (params [i]), &v);
			total += 1 + s->get_length (s->type_id, &v);
		}
	}

	return total;
}

static gboolean
int_serialize (guchar         type_id,
               const GValue  *value,
               gchar         *buffer,
               GError       **error)
{
	guint i = GUINT_TO_BE (value->data[0].v_uint);
	memcpy (buffer, &i, 4);
	return TRUE;
}

static gboolean
long_serialize (guchar         type_id,
                const GValue  *value,
                gchar         *buffer,
                GError       **error)
{
	guint64 i = GUINT64_TO_BE (value->data[0].v_uint64);
	memcpy (buffer, &i, 8);
	return TRUE;
}

static gboolean
string_serialize (guchar         type_id,
                  const GValue  *value,
                  gchar         *buffer,
                  GError       **error)
{
	const gchar *str = g_value_get_string (value);
	gsize len = strlen (str) + 1;
	memcpy (buffer, str, len);
	return TRUE;
}

static gboolean
object_serialize (guchar         type_id,
                  const GValue  *value,
                  gchar         *buffer,
                  GError       **error)
{
	GType         type   = G_VALUE_TYPE (value);
	GObject      *object = g_value_get_object (value);
	GParamSpec  **params;
	const gchar  *type_name;
	guchar        type_name_len;
	guint         n_props;
	gint          i;

	type_name = g_type_name (type);

	/* make sure name length is < 256 */
	if ((type_name_len = strlen (type_name) + 1) >= 0xFF) {
		g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
		             CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
		             "The GType name must be < 256 bytes");
		return FALSE;
	}

	buffer [0] = type_name_len;
	memcpy (buffer + 1, type_name, type_name_len);
	buffer = buffer + 1 + type_name_len;

	params = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_props);
	if (n_props == 0) {
		buffer [0] = '\0';
		g_free (params);
		return TRUE;
	}

	for (i = 0; i < n_props; i++) {
		GTypeSerializer *s;
		GValue v = {0,};
		if ((s = get_serializer_for_gtype (G_PARAM_SPEC_VALUE_TYPE (params [i]))) != NULL) {
			type_name = g_param_spec_get_name (params [i]);
			type_name_len = strlen (type_name) + 1;
			if (type_name_len >= 0xFF)
				goto next_item;
			buffer [0] = type_name_len;
			g_object_get_property (object, type_name, &v);
			memcpy (buffer + 1, &type_name, type_name_len);
			buffer = buffer + 1 + type_name_len;
			if (!s->serialize (s->type_id, value, buffer, error)) {
				if (v.g_type)
					g_value_unset (&v);
				return FALSE;
			}
		}
	next_item:
		if (G_VALUE_TYPE (&v) != 0)
			g_value_unset (&v);
	}

	return TRUE;
}

static GType
g_type_for_type_id (guint type_id)
{
	GHashTableIter iter;
	gpointer key, value;
	g_hash_table_iter_init (&iter, type_funcs);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		GTypeSerializer *s = value;
		if (s->type_id == type_id)
			return s->g_type;
	}
	return 0;
}

static gboolean
int_deserialize (guchar        type_id,
                 GValue       *value,
                 const gchar  *buffer,
                 GError      **error)
{
	value->g_type = g_type_for_type_id (type_id);
	value->data[0].v_uint = GUINT_FROM_BE (*((guint*)buffer));
	return TRUE;
}

static gboolean
long_deserialize (guchar        type_id,
                  GValue       *value,
                  const gchar  *buffer,
                  GError      **error)
{
	value->g_type = g_type_for_type_id (type_id);
	value->data[0].v_uint64 = GUINT64_FROM_BE (*((guint64*)buffer));
	return TRUE;
}

static gboolean
string_deserialize (guchar        type_id,
                    GValue       *value,
                    const gchar  *buffer,
                    GError      **error)
{
	value->g_type = G_TYPE_STRING;
	value->data[0].v_pointer = g_strdup (buffer);
	return TRUE;
}

static gboolean
object_deserialize (guchar        type_id,
                    GValue       *value,
                    const gchar  *buffer,
                    GError      **error)
{
	return TRUE;
}

static gsize
get_buffer_length (const GValue *value)
{
	GTypeSerializer *s = get_serializer_for_gtype (G_VALUE_TYPE (value));
	if (s)
		return s->get_length (s->type_id, value);
	return 0;
}

static guchar
get_type_id (const GValue *value)
{
	GTypeSerializer *s = get_serializer_for_gtype (G_VALUE_TYPE (value));
	if (s)
		return s->type_id;
	return 0;
}

static gboolean
write_value (gchar         *buffer,
             const GValue  *value,
             GError       **error)
{
	GTypeSerializer *s = get_serializer_for_gtype (G_VALUE_TYPE (value));
	if (s)
		return s->serialize (s->type_id, value, buffer, error);
	return FALSE;
}

static gboolean
catalina_binary_formatter_real_serialize (CatalinaFormatter  *formatter,
                                          const GValue       *value,
                                          gchar             **buffer,
                                          gsize              *buffer_length,
                                          GError            **error)
{
	gchar *stream;
	gsize  stream_length;

	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length != NULL, FALSE);

	if ((stream_length = get_buffer_length (value)) == 0) {
		g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
		             CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
		             "Do not know how to format type \"%s\"",
		             g_type_name (G_VALUE_TYPE (value)));
		return FALSE;
	}

	stream = g_malloc0 (stream_length);
	stream [0] = get_type_id (value);
	if (write_value ((gchar*)stream + 1, value, error)) {
		*buffer = stream;
		*buffer_length = stream_length;
		return TRUE;
	}

	g_free (stream);
	return FALSE;
}

static gboolean
read_value (guchar        type_id,
            const gchar  *buffer,
            GValue       *value,
            GError      **error)
{
	g_return_val_if_fail (value != NULL, FALSE);

	GTypeSerializer *s = get_serializer_for_type_id (type_id);

	if (s != NULL) {
		if (G_VALUE_TYPE (value) == 0)
			g_value_init (value, s->g_type);
		return s->deserialize (s->type_id, value, buffer, error);
	}

	return FALSE;
}

static gboolean
catalina_binary_formatter_real_deserialize (CatalinaFormatter  *formatter,
                                            GValue             *value,
                                            gchar              *buffer,
                                            gsize               buffer_length,
                                            GError            **error)
{
	guchar type_id;

	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (buffer_length > 0, FALSE);

	type_id = buffer [0];

	if (read_value (type_id, (gchar*)buffer + 1, value, error))
		return TRUE;

	g_set_error (error, CATALINA_BINARY_FORMATTER_ERROR,
	             CATALINA_BINARY_FORMATTER_ERROR_BAD_TYPE,
	             "Do not know how to deserialize type %d", type_id);

	return FALSE;
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

/***************************************************************************
 *               Serializers for GLib and GObject Types 
 ***************************************************************************/

static GTypeSerializer int_s     = { 1,          0, int_get_length,    int_serialize,    int_deserialize    };
static GTypeSerializer uint_s    = { 1 | 1 << 7, 0, int_get_length,    int_serialize,    int_deserialize    };
static GTypeSerializer flags_s   = { 2,          0, int_get_length,    int_serialize,    int_deserialize    };
static GTypeSerializer enum_s    = { 2 | 1 << 7, 0, int_get_length,    int_serialize,    int_deserialize    };
static GTypeSerializer long_s    = { 3,          0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer ulong_s   = { 3 | 1 << 7, 0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer int64_s   = { 4,          0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer uint64_s  = { 4 | 1 << 7, 0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer double_s  = { 5,          0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer float_s   = { 5 | 1 << 7, 0, long_get_length,   long_serialize,   long_deserialize   };
static GTypeSerializer string_s  = { 6,          0, string_get_length, string_serialize, string_deserialize };
static GTypeSerializer object_s  = { 7,          0, object_get_length, object_serialize, object_deserialize };

static void
g_type_serializer_init (void)
{
	type_funcs = g_hash_table_new (g_int_hash, g_int_equal);
	type_id_funcs = g_hash_table_new (g_int_hash, g_int_equal);

	int_s.g_type = G_TYPE_INT;
	uint_s.g_type = G_TYPE_UINT;
	flags_s.g_type = G_TYPE_FLAGS;
	enum_s.g_type = G_TYPE_ENUM;
	long_s.g_type = G_TYPE_LONG;
	ulong_s.g_type = G_TYPE_ULONG;
	int64_s.g_type = G_TYPE_INT64;
	uint64_s.g_type = G_TYPE_UINT64;
	double_s.g_type = G_TYPE_DOUBLE;
	float_s.g_type = G_TYPE_FLOAT;
	string_s.g_type = G_TYPE_STRING;
	object_s.g_type = G_TYPE_OBJECT;

	/* indexed by GType */
	g_hash_table_insert (type_funcs, &int_s.g_type, &int_s);
	g_hash_table_insert (type_funcs, &uint_s.g_type, &uint_s);
	g_hash_table_insert (type_funcs, &flags_s.g_type, &flags_s);
	g_hash_table_insert (type_funcs, &enum_s.g_type, &enum_s);
	g_hash_table_insert (type_funcs, &long_s.g_type, &long_s);
	g_hash_table_insert (type_funcs, &ulong_s.g_type, &ulong_s);
	g_hash_table_insert (type_funcs, &int64_s.g_type, &int64_s);
	g_hash_table_insert (type_funcs, &uint64_s.g_type, &uint64_s);
	g_hash_table_insert (type_funcs, &double_s.g_type, &double_s);
	g_hash_table_insert (type_funcs, &float_s.g_type, &float_s);
	g_hash_table_insert (type_funcs, &string_s.g_type, &string_s);
	g_hash_table_insert (type_funcs, &object_s.g_type, &object_s);

	/* indexed by type_id */
	g_hash_table_insert (type_id_funcs, &int_s.type_id, &int_s);
	g_hash_table_insert (type_id_funcs, &uint_s.type_id, &uint_s);
	g_hash_table_insert (type_id_funcs, &enum_s.type_id, &enum_s);
	g_hash_table_insert (type_id_funcs, &flags_s.type_id, &flags_s);
	g_hash_table_insert (type_id_funcs, &long_s.type_id, &long_s);
	g_hash_table_insert (type_id_funcs, &ulong_s.type_id, &ulong_s);
	g_hash_table_insert (type_id_funcs, &int64_s.type_id, &int64_s);
	g_hash_table_insert (type_id_funcs, &uint64_s.type_id, &uint64_s);
	g_hash_table_insert (type_id_funcs, &double_s.type_id, &double_s);
	g_hash_table_insert (type_id_funcs, &float_s.type_id, &float_s);
	g_hash_table_insert (type_id_funcs, &object_s.type_id, &object_s);
}
