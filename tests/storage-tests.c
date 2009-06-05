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

static void
test4_cb (GObject      *object,
          GAsyncResult *result,
          gpointer      user_data)
{
	AsyncTest *test = user_data;
	test->success = catalina_storage_open_finish ((void*)object, result, &test->error);
	g_assert (test->success);
	async_test_complete (test);
}

static void
test4 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	catalina_storage_set_use_idle (storage, FALSE);
	catalina_storage_open_async (storage, ".", "storage-tests.db", test4_cb, test);
	async_test_wait (test);
}

static void
test5 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	CatalinaFormatter *f1 = catalina_binary_formatter_new ();
	CatalinaFormatter *f2 = NULL;
	g_object_set (storage, "formatter", f1, NULL);
	g_object_get (storage, "formatter", &f2, NULL);
	g_assert (f1 == f2);
}

static void
test6 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
}

static void
test7_cb (GObject      *object,
          GAsyncResult *result,
          gpointer      user_data)
{
	AsyncTest *test = user_data;
	test->success = catalina_storage_close_finish (CATALINA_STORAGE (object), result, &test->error);
	async_test_complete (test);
}

static void
test7 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "use-idle", FALSE, NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_close_async (storage, test7_cb, test);
	async_test_wait (test);
}

static void
test8 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "use-idle", FALSE, NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_close (storage, NULL));
}


gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	iris_init ();

	g_test_add_func ("/CatalinaStorage/new(1)", test1);
	g_test_add_func ("/CatalinaStorage/:use-idle(1)", test2);
	g_test_add_func ("/CatalinaStorage/:transform(1)", test3);
	g_test_add_func ("/CatalinaStorage/:formatter(1)", test5);
	g_test_add_func ("/CatalinaStorage/open_async(1)", test4);
	g_test_add_func ("/CatalinaStorage/open(1)", test6);
	g_test_add_func ("/CatalinaStorage/close_async(1)", test7);
	g_test_add_func ("/CatalinaStorage/close(1)", test8);

	return g_test_run ();
}
