#include <catalina/catalina.h>
#include <iris/iris.h>
#include <string.h>
#include "mocks/long-test-data.h"

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
	g_assert (catalina_storage_set (storage, 0, TEST_KEY_STRING, -1, TEST_DATA_STRING, -1, NULL));
	g_assert (catalina_storage_get (storage, TEST_KEY_STRING, -1, &outbuf, &outlen, NULL));
	g_assert_cmpint (outlen,==,strlen (TEST_DATA_STRING) + 1);
	g_assert_cmpstr (outbuf,==,TEST_DATA_STRING);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test3 (void)
{
	gchar *outbuf = NULL;
	gsize  outlen = 0;
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_object_set (storage, "transform", catalina_zlib_transform_new (), NULL);
	g_assert (catalina_storage_set (storage, 0, LONG_TEST_KEY, -1, LONG_TEST_DATA, -1, NULL));
	g_assert (catalina_storage_get (storage, LONG_TEST_KEY, -1, &outbuf, &outlen, NULL));
	g_assert_cmpint (outlen,==,strlen (LONG_TEST_DATA) + 1);
	g_assert_cmpstr (outbuf,==,LONG_TEST_DATA);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test4 (void)
{
	GValue v1 = {0,}, v2 = {0,};
	GError *error = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_object_set (storage, "transform", catalina_zlib_transform_new (), NULL);
	g_value_init (&v1, G_TYPE_STRING);
	g_value_set_string (&v1, "my test string");
	if (!catalina_storage_set_value (storage, 0, "v-key", -1, &v1, &error))
		g_error ("%s", error->message);
	if (!catalina_storage_get_value (storage, "v-key", -1, &v2, &error))
		g_error ("%s", error->message);
	g_assert_cmpstr (g_value_get_string(&v1),==,g_value_get_string(&v2));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test5 (void)
{
	gint level = 0;
	CatalinaTransform *t = catalina_zlib_transform_new ();
	g_object_set (t, "level", 9, NULL);
	g_object_get (t, "level", &level, NULL);
	g_assert_cmpint (level,==,9);
}

static void
test6 (void)
{
	gint watermark = 0;
	CatalinaTransform *t = catalina_zlib_transform_new ();
	g_object_set (t, "watermark", 9, NULL);
	g_object_get (t, "watermark", &watermark, NULL);
	g_assert_cmpint (watermark,==,9);
}

static void
test7 (void)
{
	/* make sure a buffer <= than watermark is not compressed */
	CatalinaTransform *t = catalina_zlib_transform_new ();
	g_object_set (t, "watermark", 8, NULL);
	gchar buffer[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
	};

	gchar *outbuf = NULL;
	gsize  outlen = 0;
	g_assert (catalina_transform_write (t, buffer, sizeof (buffer), &outbuf, &outlen, NULL));
	g_assert_cmpint (outlen,==,9);
	g_assert_cmpint (outbuf [0],==,0x01);
	g_assert_cmpint (outbuf [1],==,0x02);
	g_assert_cmpint (outbuf [2],==,0x03);
	g_assert_cmpint (outbuf [3],==,0x04);
	g_assert_cmpint (outbuf [4],==,0x05);
	g_assert_cmpint (outbuf [5],==,0x06);
	g_assert_cmpint (outbuf [6],==,0x07);
	g_assert_cmpint (outbuf [7],==,0x08);
	g_assert_cmpint (outbuf [8],==,0x00);

	gchar *inbuf = NULL;
	gsize  inlen = 0;

	g_assert (catalina_transform_read (t, outbuf, outlen, &inbuf, &inlen, NULL));
	g_assert_cmpint (inlen,==,8);
	g_assert_cmpint (inbuf [0],==,0x01);
	g_assert_cmpint (inbuf [1],==,0x02);
	g_assert_cmpint (inbuf [2],==,0x03);
	g_assert_cmpint (inbuf [3],==,0x04);
	g_assert_cmpint (inbuf [4],==,0x05);
	g_assert_cmpint (inbuf [5],==,0x06);
	g_assert_cmpint (inbuf [6],==,0x07);
	g_assert_cmpint (inbuf [7],==,0x08);
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
	g_test_add_func ("/CatalinaZlibTransform/read-write(2)", test3);
	g_test_add_func ("/CatalinaZlibTransform/read-write(3)", test4);
	g_test_add_func ("/CatalinaZlibTransform/read-write(4)", test7);
	g_test_add_func ("/CatalinaZlibTransform/:level", test5);
	g_test_add_func ("/CatalinaZlibTransform/:watermark", test6);

	return g_test_run ();
}
