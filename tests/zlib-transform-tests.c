#include <catalina/catalina.h>
#include <iris/iris.h>
#include <string.h>

#define TEST_KEY_STRING  "this is the zlib test key"
#define TEST_DATA_STRING "this is some longer bit of text for test data"
#define TEST_KEY_INT     "test-zlib-int-key"
#define TEST_DATA_INT    G_MAXINT

static void
test1 (void)
{
	g_assert (catalina_zlib_transform_new ());
}

static void
test2 (void)
{
	gchar *outbuf = NULL;
	gsize  outlen = 0;
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_object_set (storage, "transform", catalina_zlib_transform_new (), NULL);
	g_assert (catalina_storage_set (storage, TEST_KEY_STRING, -1, TEST_DATA_STRING, -1, NULL));
	g_assert (catalina_storage_get (storage, TEST_KEY_STRING, -1, &outbuf, &outlen, NULL));
	g_assert_cmpint (outlen,==,strlen (TEST_DATA_STRING) + 1);
	g_assert_cmpstr (outbuf,==,TEST_DATA_STRING);
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	iris_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/CatalinaZlibTransform/new(1)", test1);
	g_test_add_func ("/CatalinaZlibTransform/read-write(1)", test2);

	return g_test_run ();
}
