#include <glib.h>
#include <glib-object.h>

static void
test1 (void)
{
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/catalina/storage/new1", test1);

	return g_test_run ();
}
