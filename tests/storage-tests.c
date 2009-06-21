#include <catalina/catalina.h>
#include <iris/iris.h>
#include <string.h>

#include "async-test.h"
#include "mocks/mock-person.h"

#define TEST_KEY  "test-key"
#define TEST_DATA "test-data"

#define TEST_KEY_ZLIB  "test-key-zlib"
#define TEST_DATA_ZLIB "test-data-zlib"

#define TEST_KEY_BINARY "test-key-binary"
static gchar *TEST_DATA_BINARY = NULL;

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
	if ((test->success = catalina_storage_open_finish ((void*)object, result, &test->error))) {
		GError *error = NULL;
		if (!catalina_storage_close ((void*)object, &error))
			g_error ("%s", error->message);
	}
	async_test_complete (test);
}

static void
test4 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
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
	GError *error = NULL;
	if (!catalina_storage_open (storage, ".", "storage-tests.db", &error))
		g_error ("%s", error->message);
	catalina_storage_close (storage, NULL);
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
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_close_async (storage, test7_cb, test);
	async_test_wait (test);
}

static void
test8 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test9_cb (GObject      *object,
          GAsyncResult *result,
          gpointer      user_data)
{
	gchar *buffer = NULL;
	gsize  length = 0;
	AsyncTest *test = user_data;
	catalina_storage_get_finish ((void*)object, result, &buffer, &length, &test->error);
	async_test_complete (test);
}

static void
test9 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_get_async (storage, TEST_KEY, -1, test9_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test10_cb (GObject      *object,
           GAsyncResult *result,
           gpointer      user_data)
{
	AsyncTest *test = user_data;
	test->success = catalina_storage_set_finish ((void*)object, result, &test->error);
	g_assert (test->success);
	async_test_complete (user_data);
}

static void
test10 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_set_async (storage, 0, TEST_KEY, -1, TEST_DATA, -1, test10_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test11 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_set (storage, 0, TEST_KEY, -1, TEST_DATA, -1, NULL));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test12 (void)
{
	gchar *buffer = NULL;
	gsize  length = 0;
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_get (storage, TEST_KEY, -1, &buffer, &length, NULL));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test13 (void)
{
	gchar  *buffer = NULL;
	gsize   length = 0;
	GError *error = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "transform", catalina_zlib_transform_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	if (!catalina_storage_get (storage, TEST_KEY_ZLIB, -1, &buffer, &length, &error))
		g_error ("%s", error->message);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test14 (void)
{
	GError *error = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "transform", catalina_zlib_transform_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	if (!catalina_storage_set (storage, 0, TEST_KEY_ZLIB, -1, TEST_DATA_ZLIB, -1, &error))
		g_error ("%s", error->message);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test15_cb (GObject      *object,
           GAsyncResult *result,
           gpointer      user_data)
{
	AsyncTest *test  = user_data;
	GValue     value = {0,};

	if (!catalina_storage_get_value_finish (CATALINA_STORAGE (object), result,
	                                        &value, &test->error))
		async_test_error (test);
	g_assert_cmpstr (g_value_get_string (&value),==,"abc123");
	async_test_complete (test);
}

static void
test15 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	GValue v = {0,};
	g_value_init (&v, G_TYPE_STRING);
	g_value_set_string (&v, "abc123");
	g_assert (catalina_storage_set_value (storage, 0, TEST_KEY_BINARY, -1, &v, NULL));
	catalina_storage_get_value_async (storage, TEST_KEY_BINARY, -1, test15_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test16_cb (GObject      *object,
           GAsyncResult *result,
           gpointer      user_data)
{
	AsyncTest *test = user_data;
	if (!catalina_storage_set_value_finish (CATALINA_STORAGE (object), result, &test->error))
		async_test_error (test);
	else
		async_test_complete (test);
}

static void
test16 (void)
{
	GValue value = {0,};
	g_value_init (&value, G_TYPE_INT);
	g_value_set_int (&value, 42);

	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_set_value_async (storage, 0, TEST_KEY_BINARY, -1, &value, test16_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test17 (void)
{
	GValue v = {0,};
	g_value_init (&v, G_TYPE_STRING);
	g_value_set_string (&v, TEST_DATA_BINARY);
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_set_value (storage, 0, TEST_KEY_BINARY, -1, &v, NULL));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test18 (void)
{
	GValue v = {0,};
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert (catalina_storage_get_value (storage, TEST_KEY_BINARY, -1, &v, NULL));
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test19 (void)
{
	GError *error = NULL;
	MockPerson *person = mock_person_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (person,
	              "first-name", "Christian",
	              "last-name",  "Hergert",
	              "birth-date", "August 16th, 1984",
	              NULL);
	GValue v1 = {0,}, v2 = {0,};
	g_value_init (&v1, MOCK_TYPE_PERSON);
	g_value_set_object (&v1, person);
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	if (!catalina_storage_set_value (storage, 0, "mock-person", -1, &v1, &error))
		g_error ("%s", error->message);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test20 (void)
{
	GError *error = NULL;
	GType mtype = MOCK_TYPE_PERSON; // until we get symbol lookup
	GValue v = {0,};
	MockPerson *person = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	if (!catalina_storage_get_value (storage, "mock-person", -1, &v, &error))
		g_error ("%s", error->message);
	person = g_value_get_object (&v);
	g_assert (catalina_storage_close (storage, NULL));
	g_assert_cmpstr (mock_person_get_first_name (person),==,"Christian");
	g_assert_cmpstr (mock_person_get_last_name (person),==,"Hergert");
	g_assert_cmpstr (mock_person_get_birth_date (person),==,"August 16th, 1984");
}

static void
test21 (void)
{
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	g_assert_cmpint (catalina_storage_count_keys (storage),>,1);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test22_cb2 (GObject      *obj,
            GAsyncResult *result,
            gpointer      user_data)
{
	CatalinaStorage *storage = (void*)obj;
	GError *error = NULL;
	if (!catalina_storage_transaction_commit_finish (storage, result, &error))
		g_error ("%s", error->message);
	async_test_complete (user_data);
}

static void
test22_cb (GObject      *obj,
           GAsyncResult *result,
           gpointer      user_data)
{
	CatalinaStorage *storage = (void*)obj;
	gulong txn_id = catalina_storage_transaction_begin_finish (storage, result);
	g_assert (txn_id == 1);
	g_assert (!catalina_storage_close (storage, NULL)); // expect error, pending txn
	catalina_storage_transaction_commit_async (storage, txn_id, test22_cb2, user_data);
}

static void
test22 (void)
{
	AsyncTest *test = async_test_new ();
	GError *error = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	catalina_storage_transaction_begin_async (storage, test22_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test23_cb3 (GObject      *obj,
            GAsyncResult *result,
            gpointer      user_data)
{
	CatalinaStorage *storage = (void*)obj;
	AsyncTest *test = user_data;
	GValue v = {0,};
	GError *error = NULL;
	if (!catalina_storage_transaction_commit_finish (storage, result, &error))
		g_error ("%s", error->message);
	g_assert (catalina_storage_get_value (storage, "test23", -1, &v, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,987789);
	async_test_complete (test);
}

static void
test23_cb2 (GObject      *obj,
            GAsyncResult *result,
            gpointer      user_data)
{
	CatalinaStorage *storage = (void*)obj;
	GError *error = NULL;
	if (!catalina_storage_set_value_finish (storage, result, &error))
		g_error ("%s", error->message);
}

static void
test23_cb (GObject      *obj,
           GAsyncResult *result,
           gpointer      user_data)
{
	AsyncTest *test = user_data;
	CatalinaStorage *storage = (void*)obj;
	GValue v = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, 987789);
	test->txn1 = catalina_storage_transaction_begin_finish (storage, result);
	catalina_storage_set_value_async (storage, test->txn1, "test23", -1, &v, test23_cb2, user_data);
	catalina_storage_transaction_commit_async (storage, test->txn1, test23_cb3, user_data);
}

static void
test23 (void)
{
	AsyncTest *test = async_test_new ();
	GError *error = NULL;
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	GValue v = {0,}, v1 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, 123321);
	if (!catalina_storage_set_value (storage, 0, "test23", -1, &v, &error))
		g_error ("%s", error->message);
	g_assert (catalina_storage_get_value (storage, "test23", -1, &v1, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,123321);
	catalina_storage_transaction_begin_async (storage, test23_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

static void
test24_cb3 (GObject      *obj,
            GAsyncResult *result,
            gpointer      user_data)
{
	AsyncTest *test = user_data;
	CatalinaStorage *storage = (void*)obj;
	catalina_storage_transaction_cancel_finish (storage, result);
	GValue v = {0,};
	g_assert (catalina_storage_get_value (storage, "test-24", -1, &v, NULL));
	g_assert_cmpint (g_value_get_int(&v),==,456654);
	async_test_complete (test);
}

static void
test24_cb2 (GObject      *obj,
            GAsyncResult *result,
            gpointer      user_data)
{
	g_error ("Shouldn't get set_value callback, txn was cancelled");
}

static void
test24_cb (GObject      *obj,
           GAsyncResult *result,
           gpointer      user_data)
{
	AsyncTest *test = user_data;
	CatalinaStorage *storage = (void*)obj;
	gulong txn_id;
	if (!(txn_id = catalina_storage_transaction_begin_finish (storage, result)))
		g_error ("Invalid txn id %lu", txn_id);
	GValue v = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, 123123);
	test->txn1 = txn_id;
	catalina_storage_set_value_async (storage, txn_id, "test-24", -1, &v, test24_cb2, test);
	catalina_storage_transaction_cancel_async (storage, txn_id, test24_cb3, test);
}

static void
test24 (void)
{
	AsyncTest *test = async_test_new ();
	CatalinaStorage *storage = catalina_storage_new ();
	g_object_set (storage, "formatter", catalina_binary_formatter_new (), NULL);
	g_assert (catalina_storage_open (storage, ".", "storage-tests.db", NULL));
	GValue v = {0,};
	GError *error = NULL;
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, 456654);
	if (!catalina_storage_set_value (storage, 0, "test-24", -1, &v, &error))
		g_error ("%s", error->message);
	catalina_storage_transaction_begin_async (storage, test24_cb, test);
	async_test_wait (test);
	g_assert (catalina_storage_close (storage, NULL));
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);
	iris_init ();

	TEST_DATA_BINARY = g_malloc0 (5);
	TEST_DATA_BINARY [0] = 1 << 0;
	guint i = g_htonl (42);
	memcpy (&TEST_DATA_BINARY [1], &i, 4);

	g_test_add_func ("/CatalinaStorage/new(1)", test1);
	g_test_add_func ("/CatalinaStorage/:use-idle(1)", test2);
	g_test_add_func ("/CatalinaStorage/:transform(1)", test3);
	g_test_add_func ("/CatalinaStorage/:formatter(1)", test5);
	g_test_add_func ("/CatalinaStorage/open_async(1)", test4);
	g_test_add_func ("/CatalinaStorage/open(1)", test6);
	g_test_add_func ("/CatalinaStorage/close_async(1)", test7);
	g_test_add_func ("/CatalinaStorage/close(1)", test8);
	g_test_add_func ("/CatalinaStorage/set_async(1)", test10);
	g_test_add_func ("/CatalinaStorage/set(1)", test11);
	g_test_add_func ("/CatalinaStorage/set(2)", test14);
	g_test_add_func ("/CatalinaStorage/get_async(1)", test9);
	g_test_add_func ("/CatalinaStorage/get(1)", test12);
	g_test_add_func ("/CatalinaStorage/get(2)", test13);
	g_test_add_func ("/CatalinaStorage/set_value_async(1)", test16);
	g_test_add_func ("/CatalinaStorage/get_value_async(1)", test15);
	g_test_add_func ("/CatalinaStorage/set_value(1)", test17);
	g_test_add_func ("/CatalinaStorage/set_value(2)", test19);
	g_test_add_func ("/CatalinaStorage/get_value(1)", test18);
	g_test_add_func ("/CatalinaStorage/get_value(2)", test20);
	g_test_add_func ("/CatalinaStorage/count_keys(1)", test21);
	g_test_add_func ("/CatalinaStorage/transaction_begin(1)", test22);
	g_test_add_func ("/CatalinaStorage/transaction_commit(1)", test23);
	g_test_add_func ("/CatalinaStorage/transaction_cancel(1)", test24);

	return g_test_run ();
}
