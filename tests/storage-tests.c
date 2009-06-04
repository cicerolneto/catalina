#include <catalina/catalina.h>
#include <iris/iris.h>

static void
test1 (void)
{
	g_assert (catalina_storage_new ());
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	iris_init ();

	g_test_add_func ("/storage/new1", test1);

	return g_test_run ();
}
