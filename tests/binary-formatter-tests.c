#include <catalina/catalina.h>

static void
test1 (void)
{
	g_assert (catalina_binary_formatter_new ());
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/binary-formatter/new1", test1);

	return g_test_run ();
}
