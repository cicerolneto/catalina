#include <stdlib.h>
#include <catalina/catalina.h>
#include <iris/iris.h>

static void
set_value_cb (GObject       *object,
              GAsyncResult  *result,
              gpointer       user_data)
{
	CatalinaStorage *storage   = CATALINA_STORAGE (object);
	GMainLoop       *main_loop = user_data;
	GError          *error     = NULL;

	if (!catalina_storage_set_value_finish (storage, result, &error))
		g_printerr ("%s: %s\n", __func__, error->message);
	else
		g_print ("%s: value stored successfully!\n", __func__);

	g_main_loop_quit (main_loop);
}

static void
open_cb (GObject       *object,
         GAsyncResult  *result,
         gpointer       user_data)
{
	CatalinaStorage *storage   = CATALINA_STORAGE (object);
	GMainLoop       *main_loop = user_data;
	GError          *error     = NULL;
	GValue           answer    = {0,};

	if (!catalina_storage_open_finish (storage, result, &error)) {
		g_printerr ("%s: %s\n", __func__, error->message);
		g_main_loop_quit (main_loop);
		return;
	}

	g_value_init (&answer, G_TYPE_INT);
	g_value_set_int (&answer, 42);
	catalina_storage_set_value_async (storage,
	                                  "the-answer", -1, /* key & key_length   */
	                                  &answer,          /* value to serialize */
	                                  set_value_cb,
	                                  main_loop);
}

gint
main (gint   argc,
      gchar *argv[])
{
	GMainLoop       *main_loop;
	CatalinaStorage *storage;

	g_type_init ();
	iris_init ();

	/* create main loop */
	main_loop = g_main_loop_new (NULL, FALSE);

	/* create storage */
	storage = catalina_storage_new ();

	/* support serialization and and compress data on disk */
	g_object_set (storage,
	              "formatter", catalina_binary_formatter_new (),
	              "transform", catalina_zlib_transform_new (),
	              NULL);

	/* open the database asynchronously */
	catalina_storage_open_async (storage,
	                             ".",          /* env dir */
	                             "storage.db", /* db name */
	                             open_cb,
	                             main_loop);

	/* run the main loop until the example finishes */
	g_main_loop_run (main_loop);

	return EXIT_SUCCESS;
}
