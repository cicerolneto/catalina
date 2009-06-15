#include <stdlib.h>
#include <catalina/catalina.h>
#include <iris/iris.h>

gint
main (gint   argc,
      gchar *argv[])
{
	CatalinaStorage *storage;
	GError          *error = NULL;
	GValue           value = {0,};

	g_type_init ();
	iris_init ();

	/* create storage */
	storage = catalina_storage_new ();

	/* support serialization and and compress data on disk */
	g_object_set (storage,
	              "formatter", catalina_binary_formatter_new (),
	              "transform", catalina_zlib_transform_new (),
	              NULL);

	/* open the database synchronously */
	if (!catalina_storage_open (storage,
	                            ".",          /* env dir */
	                            "storage.db", /* db name */
	                            &error))
	        g_error ("%s", error->message);

	g_value_init (&value, G_TYPE_STRING);
	g_value_set_string (&value, "This is some random string that gets serialized and "
	                            "compressed before written to disk.");

	/* store the value synchronously */
	if (!catalina_storage_set_value (storage,
	                                 "some-key", -1, /* key, key-length    */
	                                 &value,         /* value to serialize */
	                                 &error))
		g_error ("%s", error->message);

	g_print ("%s: serialized and stored value to storage\n", __func__);

	/* synchronously close the storage */
	catalina_storage_close (storage, NULL);

	return EXIT_SUCCESS;
}
