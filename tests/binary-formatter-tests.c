#include <catalina/catalina.h>

static void
test1 (void)
{
	g_assert (catalina_binary_formatter_new ());
}

static void
test2 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, G_MININT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,g_value_get_int(&v2));
}

static void
test3 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT64);
	g_value_set_int64 (&v, G_MAXINT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test4 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_LONG);
	v.data[0].v_long = G_MAXLONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test5 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_DOUBLE);
	v.data[0].v_double = G_MAXDOUBLE;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_double,==,v2.data[0].v_double);
}

static void
test6 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_UINT);
	g_value_set_uint (&v, G_MAXUINT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_uint (&v),==,g_value_get_uint(&v2));
}

static void
test7 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_UINT64);
	g_value_set_uint64 (&v, G_MAXUINT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_uint64,==,v2.data[0].v_uint64);
}

static void
test8 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_ULONG);
	v.data[0].v_long = G_MAXULONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_ulong,==,v2.data[0].v_ulong);
}

static void
test9 (void)
{
	GValue v  = {0,};
	GValue v2 = {0,};
	gchar *b  = NULL;
	gsize  l  = 0;

	g_value_init (&v, G_TYPE_STRING);
	g_value_set_string (&v, "MY TEST STRING");

	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpstr (v.data[0].v_pointer,==,v2.data[0].v_pointer);
}

static void
test10 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, G_MAXINT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,g_value_get_int(&v2));
}

static void
test11 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT64);
	g_value_set_int64 (&v, G_MININT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test12 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_LONG);
	v.data[0].v_long = G_MINLONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test13 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_DOUBLE);
	v.data[0].v_double = G_MINDOUBLE;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_double,==,v2.data[0].v_double);
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/CatalinaBinaryFormatter/new(1)", test1);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int>(1)", test2);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int>(2)", test10);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int64>(1)", test3);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int64>(2)", test11);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<long>(1)", test4);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<long>(2)", test12);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<double>(1)", test5);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<double>(1)", test13);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<uint>(1)", test6);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<uint64>(1)", test7);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<ulong>(1)", test8);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<string>(1)", test9);

	return g_test_run ();
}
