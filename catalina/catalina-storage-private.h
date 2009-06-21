/* catalina-storage-private.h
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

#ifndef __CATALINA_STORAGE_PRIVATE_H__
#define __CATALINA_STORAGE_PRIVATE_H__

#include <sys/types.h>
#include <tdb.h>
#include <glib-object.h>
#include <iris/iris.h>

#include "catalina-storage.h"
#include "catalina-formatter.h"
#include "catalina-transform.h"

G_BEGIN_DECLS

typedef struct _StorageTask StorageTask;
typedef struct _TxnState    TxnState;

struct _CatalinaStoragePrivate
{
	TDB_CONTEXT       *db_ctx;        /* TDB handle */
	gboolean           use_idle;      /* dispatch callbacks in main thread */
	guint              flags;         /* state flags */

	CatalinaFormatter *formatter;     /* (de)serialization to/from storage */
	CatalinaTransform *transform;     /* buffer transformations such as zlib */

	gulong             txn_seq;       /* transaction id sequence */
	gulong             txn;           /* current transaction */
	GList             *txn_commits;   /* pending commit queue */
	GHashTable        *txn_state;     /* transaction state */

	IrisPort          *ex_port,       /* exclusive operations, open/close/write/etc */
	                  *cn_port;       /* concurrent operations, get/etc */
	IrisReceiver      *ex_receiver,
	                  *cn_receiver;
	IrisArbiter       *arbiter;
};

struct _StorageTask
{
	CatalinaStorage     *storage;
	gulong               txn_id;

	GSimpleAsyncResult  *result;
	gboolean             success;

	/* for synchronous tasks */
	GMutex               *mutex;
	GCond                *cond;

	/* task information */
	gchar                *key;
	gsize                 key_length;
	gchar                *data;
	gsize                 data_length;
	GValue                value;

	/* task failure propagation */
	GError               *error;
};

struct _TxnState
{
	gulong           txn_id;
	CatalinaStorage *storage;
	GList           *msgs;
	GError          *error;
};

enum
{
	MESSAGE_0,
	MESSAGE_OPEN,
	MESSAGE_CLOSE,
	MESSAGE_GET,
	MESSAGE_GET_VALUE,
	MESSAGE_SET,
	MESSAGE_SET_VALUE,
	MESSAGE_COUNT_KEYS,
	MESSAGE_TXN_BEGIN,
	MESSAGE_TXN_COMMIT,
	MESSAGE_TXN_CANCEL,
};

enum
{
	PROP_0,
	PROP_USE_IDLE,
	PROP_FORMATTER,
	PROP_TRANSFORM,
};

enum
{
	FLAG_READY = 1 << 0,
};

static void         catalina_storage_ex_handle_message (IrisMessage     *message, CatalinaStorage  *storage);
static void         catalina_storage_cn_handle_message (IrisMessage     *message, CatalinaStorage  *storage);

static TxnState*    txn_state_new         (CatalinaStorage *storage, gulong            txn_id);
static gboolean     txn_state_run         (TxnState        *state,   GError          **error);
static void         txn_state_free        (TxnState        *state);
static StorageTask* storage_task_new      (CatalinaStorage *storage, gboolean          is_async, GAsyncReadyCallback  callback, gpointer user_data, gpointer source_tag);
static void         storage_task_free     (StorageTask     *task,    gboolean          free_key, gboolean             free_data);
static gboolean     storage_task_wait     (StorageTask     *task,    GError          **error);
static void         storage_task_complete (StorageTask     *task,    gboolean          success);
static void         storage_task_succeed  (StorageTask     *task);
static void         storage_task_fail     (StorageTask     *task);

G_END_DECLS

#endif /* __CATALINA_STORAGE_PRIVATE_H__ */
