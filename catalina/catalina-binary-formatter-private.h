/* catalina-binary-formatter.c
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

#ifndef __CATALINA_BINARY_FORMATTER_PRIVATE_H__
#define __CATALINA_BINARY_FORMATTER_PRIVATE_H__

#include <glib.h>

#define BINARY_FORMATTER_TYPE_UNSIGNED (1 << 7)
#define BINARY_FORMATTER_TYPE_BOOLEAN  (1)
#define BINARY_FORMATTER_TYPE_INT64    (2)
#define BINARY_FORMATTER_TYPE_UINT64   (2 | BINARY_FORMATTER_TYPE_UNSIGNED)
#define BINARY_FORMATTER_TYPE_LONG     (3)
#define BINARY_FORMATTER_TYPE_ULONG    (3 | BINARY_FORMATTER_TYPE_UNSIGNED)
#define BINARY_FORMATTER_TYPE_INT      (4)
#define BINARY_FORMATTER_TYPE_UINT     (4 | BINARY_FORMATTER_TYPE_UNSIGNED)
#define BINARY_FORMATTER_TYPE_SHORT    (5)
#define BINARY_FORMATTER_TYPE_USHORT   (5 | BINARY_FORMATTER_TYPE_UNSIGNED)
#define BINARY_FORMATTER_TYPE_CHAR     (6)
#define BINARY_FORMATTER_TYPE_UCHAR    (6 | BINARY_FORMATTER_TYPE_UNSIGNED)
#define BINARY_FORMATTER_TYPE_DOUBLE   (7)
#define BINARY_FORMATTER_TYPE_FLOAT    (8)
#define BINARY_FORMATTER_TYPE_STRING   (14)
#define BINARY_FORMATTER_TYPE_OBJECT   (15)

struct _CatalinaBinaryFormatterPrivate
{
	gpointer dummy;
};

#endif /* __CATALINA_BINARY_FORMATTER_PRIVATE_H__ */
