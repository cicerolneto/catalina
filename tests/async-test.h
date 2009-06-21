/* async-test.h: Asynchronous test helpers
 * 
 * Copyright (C) 2009 Christian Hergert <chris@dronelabs.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA
 * 02110-1301 USA
 */

#ifndef __ASYNC_TEST_H__
#define __ASYNC_TEST_H__

#include <glib-object.h>

typedef struct
{
	gboolean  success;
	GMutex   *mutex;
	GCond    *cond;
	GError   *error;

	gulong    txn1;
	gulong    txn2;
	gulong    txn3;
	gulong    txn4;
} AsyncTest;

static void
async_test_init (AsyncTest *test)
{
	test->success = FALSE;
	test->mutex = g_mutex_new ();
	test->cond = g_cond_new ();
	test->error = NULL;
	g_mutex_lock (test->mutex);
}

static AsyncTest*
async_test_new (void)
{
	AsyncTest *test = g_slice_new0 (AsyncTest);
	async_test_init (test);
	return test;
}

static void
async_test_wait (AsyncTest *test)
{
	/* start main loop in another thread */
	GMainLoop *main_loop = g_main_loop_new (NULL, FALSE);
	g_thread_create ((GThreadFunc)g_main_loop_run, main_loop, FALSE, NULL);
	g_cond_wait (test->cond, test->mutex);
	g_assert_cmpint (test->success,==,TRUE);
	g_mutex_unlock (test->mutex);
	g_mutex_free (test->mutex);
	g_cond_free (test->cond);
	g_main_loop_quit (main_loop);
}

static void
async_test_error (AsyncTest *test)
{
	g_return_if_fail (test != NULL);
	if (!test->success && test->error != NULL)
		g_error ("%s", test->error->message != NULL ? test->error->message : "No Error Message");
	g_mutex_lock (test->mutex);
	g_cond_signal (test->cond);
	g_mutex_unlock (test->mutex);
	g_assert (FALSE);
}

static void
async_test_complete (AsyncTest *test)
{
	if (test->error)
		async_test_error (test);
	test->success = TRUE;
	g_mutex_lock (test->mutex);
	g_cond_signal (test->cond);
	g_mutex_unlock (test->mutex);
}

#endif /* __ASYNC_TEST_H__ */
