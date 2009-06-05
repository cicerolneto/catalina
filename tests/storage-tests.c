#include <catalina/catalina.h>
#include <iris/iris.h>
#include "async-test.h"

static void
test1 (void)
{
	g_assert (catalina_storage_new ());
}

static void
test2 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert_cmpint (catalina_storage_get_use_idle (storage),==,TRUE);
	catalina_storage_set_use_idle (storage, FALSE);
	g_assert_cmpint (catalina_storage_get_use_idle (storage),==,FALSE);
	catalina_storage_set_use_idle (storage, TRUE);
	g_assert_cmpint (catalina_storage_get_use_idle (storage),==,TRUE);
}

static void
test3 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	CatalinaTransform *t1 = catalina_zlib_transform_new ();
	CatalinaTransform *t2 = NULL;
	g_object_set (storage, "transform", t1, NULL);
	g_object_get (storage, "transform", &t2, NULL);
	g_assert (t1 == t2);
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	iris_init ();

	g_test_add_func ("/storage/new1", test1);
	g_test_add_func ("/storage/use_idle1", test2);

	return g_test_run ();
}
