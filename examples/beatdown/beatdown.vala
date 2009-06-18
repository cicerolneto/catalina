using GLib;
using Catalina;

namespace Beatdown {
	static void main (string[] args) {
		var storage = new Storage ();
		storage.formatter = new BinaryFormatter ();
		storage.transform = new ZlibTransform ();

		try {
			storage.open (".", "beatdown.db");
		}
		catch (Error err) {
			error ("Error opening storage: %s", err.message);
		}

		debug ("Storage opened");

		for (int i = 0; i < 1000; i++) {
			Value v = Value (typeof (Person));
			try {
				var p = new Person ();
				p.first_name = "John";
				p.last_name = "Doe";
				p.website = "http://example.com";
				p.email = "example@example.com";
				v.set_object (p);
				storage.set_value ("%d".printf (i), -1, v);
			}
			catch (Error err) {
				error ("Error storing %d: %s", i, err.message);
			}
		}

		for (int i = 0; i < 1000; i++) {
			var key = "%d".printf (i);
			Value v = Value (typeof (Person));
			try {
				storage.get_value (key, -1, v);
				Person p = (Person)v.get_object ();
				if (p == null || p.first_name != "John")
					error ("Invalid person retrieved: %d", i);
			}
			catch (Error err) {
				error ("%s", err.message);
			}
			v.unset ();
		}

		try {
			storage.close ();
		}
		catch (Error err) {
			error ("Could not close the database");
		}

		debug ("Storage closed");
	}

	public enum Gender {
		UNKNOWN,
		MALE,
		FEMALE,
	}

	class Person: GLib.Object {
		public string first_name { get; set; }
		public string last_name  { get; set; }
		public string website    { get; set; }
		public string email      { get; set; }
		public bool   married    { get; set; }
		public Gender gender     { get; set; }
		public TimeVal birth_date { get; set; }
	}
}
