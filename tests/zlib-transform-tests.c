#include <catalina/catalina.h>

static void
test1 (void)
{
	g_assert (catalina_zlib_transform_new ());
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/zlib-transform/new1", test1);

	return g_test_run ();
}
