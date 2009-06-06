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
	GValue v = {G_TYPE_INT,};
	g_value_set_int (&v, 42);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
}

static void
test3 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, 42);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,g_value_get_int(&v2));
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/CatalinaBinaryFormatter/new(1)", test1);
	g_test_add_func ("/CatalinaBinaryFormatter/serialize<int>(1)", test2);
	g_test_add_func ("/CatalinaBinaryFormatter/deserialize<int>(1)", test3);

	return g_test_run ();
}
