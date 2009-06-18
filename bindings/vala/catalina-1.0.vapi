/* catalina-1.0.vapi
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

using GLib;

[CCode (cheader_filename = "catalina/catalina.h")]
namespace Catalina {
	public errordomain StorageError {
		STATE,
		DB,
		NO_SUCH_KEY,
	}

	public class Storage: GLib.Object {
		public bool use_idle { get; set; }
		public Catalina.Transform transform { get; set; }
		public Catalina.Formatter formatter { get; set; }

		/* asynchronous methods */

		/* synchronous methods */
		public bool open (string env_dir, string name) throws Catalina.StorageError;
		public bool close () throws Catalina.StorageError;
		public bool get (string key, size_t key_length, out string value, out size_t value_length) throws Catalina.StorageError;
		public bool set (string key, size_t key_length, string value, size_t value_length) throws Catalina.StorageError;
		public bool get_value (string key, size_t key_length, Value value) throws Catalina.StorageError;
		public bool set_value (string key, size_t key_length, Value value) throws Catalina.StorageError;
	}

	public class Transform: GLib.Object {
		public virtual bool read (string input, size_t input_length, out string output, out size_t output_length) throws GLib.Error;
		public virtual bool write (string input, size_t input_length, out string output, out size_t output_length) throws GLib.Error;
	}

	public class Formatter: GLib.Object {
		public virtual bool serialize (Value value, out string buffer, out size_t buffer_length) throws GLib.Error;
		public virtual bool deserialize (Value value, string buffer, size_t buffer_length);
	}

	public class ZlibTransform: Catalina.Transform {
	}

	public class BinaryFormatter: Catalina.Formatter {
	}
}
