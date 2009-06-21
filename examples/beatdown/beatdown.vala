using GLib;
using Catalina;

namespace Beatdown {
	Storage  storage = null;
	int      count   = 0;
	Mutex    mutex;
	Cond     cond;

	static void main (string[] args) {
		mutex = new Mutex ();
		cond = new Cond ();

		mutex.lock ();
		storage = new Storage ();
		storage.use_idle = false;
		storage.formatter = new BinaryFormatter ();
		storage.transform = new ZlibTransform ();

		storage.open_async (".", "beatdown.db", (_, result) => {
			try { storage.open_finish (result); }
			catch { error ("Could not open storage"); }

			debug ("Storage opened");

			for (int i = 0; i < 1000; i++) {
				Value v = Value (typeof (Person));
				var p = new Person ();
				p.first_name = "John";
				p.last_name = "Doe";
				p.website = "http://example.com";
				p.email = "example@example.com";
				v.set_object (p);
				storage.set_value_async ("%d".printf (i), -1, v, (_, result) => {
					AtomicInt.inc (ref count);
					try { storage.set_value_finish (result); }
					catch { error ("Could not store value"); }
					if (count >= 1000) {
						mutex.lock ();
						cond.signal ();
						mutex.unlock ();
					}
				});
				v.unset ();
			}
		});

		cond.wait (mutex);
		mutex.unlock ();

		try { storage.close (); } catch { error ("Error closing storage"); }
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

		/* G_TYPE_ENUM and TimeVal serialization not yet supported */
		public Gender gender      { get; set; }
		public TimeVal birth_date { get; set; }
	}
}
