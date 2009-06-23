#include <string.h>
#include <catalina/catalina.h>
#include <catalina/catalina-binary-formatter-private.h>
#include "mocks/mock-person.h"

static void
test1 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_boolean (NULL, 255, &buffer, &length, NULL));
	g_assert_cmpint (length,==,2);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_BOOLEAN);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
}

static void
test2 (void)
{
	guchar buffer[2] = {BINARY_FORMATTER_TYPE_BOOLEAN,0xFF};
	gboolean value;

	g_assert (catalina_binary_formatter_read_boolean (NULL, &value, (gchar*)&buffer [0], 2, NULL));
	g_assert_cmpint ((guchar)value,==,0xFF);
}

static void
test3 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_char (NULL, 255, &buffer, &length, NULL));
	g_assert_cmpint (length,==,2);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_CHAR);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
}

static void
test4 (void)
{
	guchar buffer[2] = {BINARY_FORMATTER_TYPE_CHAR,0xFF};
	gchar value;

	g_assert (catalina_binary_formatter_read_char (NULL, &value, (gchar*)&buffer [0], 2, NULL));
	g_assert_cmpint ((guchar)value,==,0xFF);
}

static void
test5 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_uchar (NULL, 255, &buffer, &length, NULL));
	g_assert_cmpint (length,==,2);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_UCHAR);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
}

static void
test6 (void)
{
	guchar buffer[2] = {BINARY_FORMATTER_TYPE_UCHAR,0xFF};
	guchar value;

	g_assert (catalina_binary_formatter_read_uchar (NULL, &value, (gchar*)&buffer [0], 2, NULL));
	g_assert_cmpint ((guchar)value,==,0xFF);
}

static void
test7 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_short (NULL, 65535, &buffer, &length, NULL));
	g_assert_cmpint (length,==,3);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_SHORT);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
}

static void
test8 (void)
{
	guchar buffer[3] = {BINARY_FORMATTER_TYPE_SHORT,0xFF,0xFF};
	gshort value;

	g_assert (catalina_binary_formatter_read_short (NULL, &value, (gchar*)&buffer [0], 3, NULL));
	g_assert_cmpint ((gushort)value,==,0xFFFF);
}

static void
test9 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_ushort (NULL, 65535, &buffer, &length, NULL));
	g_assert_cmpint (length,==,3);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_USHORT);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
}

static void
test10 (void)
{
	guchar buffer[3] = {BINARY_FORMATTER_TYPE_USHORT,0xFF,0xFF};
	gushort value;

	g_assert (catalina_binary_formatter_read_ushort (NULL, &value, (gchar*)&buffer [0], 3, NULL));
	g_assert_cmpint ((gushort)value,==,0xFFFF);
}

static void
test11 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_int (NULL, 0xFFFFFFFF, &buffer, &length, NULL));
	g_assert_cmpint (length,==,5);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_INT);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);
}

static void
test12 (void)
{
	guchar buffer[5] = {BINARY_FORMATTER_TYPE_INT,0xFF,0xFF,0xFF,0xFF};
	gint value;

	g_assert (catalina_binary_formatter_read_int (NULL, &value, (gchar*)&buffer [0], 5, NULL));
	g_assert_cmpint ((guint)value,==,0xFFFFFFFF);
}

static void
test13 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_uint (NULL, 0xFFFFFFFF, &buffer, &length, NULL));
	g_assert_cmpint (length,==,5);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_UINT);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);
}

static void
test14 (void)
{
	guchar buffer[5] = {BINARY_FORMATTER_TYPE_UINT,0xFF,0xFF,0xFF,0xFF};
	guint value;

	g_assert (catalina_binary_formatter_read_uint (NULL, &value, (gchar*)&buffer [0], 5, NULL));
	g_assert_cmpint ((guint)value,==,0xFFFFFFFF);
}

static void
test15 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_long (NULL, G_MAXULONG, &buffer, &length, NULL));
	g_assert_cmpint (length,==,9);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_LONG);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);

	if (sizeof (glong) == 4) {
		g_assert_cmpint (((guchar)buffer[5]),==,0);
		g_assert_cmpint (((guchar)buffer[6]),==,0);
		g_assert_cmpint (((guchar)buffer[7]),==,0);
		g_assert_cmpint (((guchar)buffer[8]),==,0);
	}
	else {
		g_assert_cmpint (((guchar)buffer[5]),==,255);
		g_assert_cmpint (((guchar)buffer[6]),==,255);
		g_assert_cmpint (((guchar)buffer[7]),==,255);
		g_assert_cmpint (((guchar)buffer[8]),==,255);
	}
}

static void
test16 (void)
{
	guchar buffer[9] = {BINARY_FORMATTER_TYPE_LONG,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	glong value;

	g_assert (catalina_binary_formatter_read_long (NULL, &value, (gchar*)&buffer [0], 9, NULL));
	g_assert_cmpint ((guint64)value,==,G_GUINT64_CONSTANT(0xFFFFFFFFFFFFFFFF));
}

static void
test17 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_ulong (NULL, G_MAXULONG, &buffer, &length, NULL));
	g_assert_cmpint (length,==,9);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_ULONG);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);

	if (sizeof (gulong) == 4) {
		g_assert_cmpint (((guchar)buffer[5]),==,0);
		g_assert_cmpint (((guchar)buffer[6]),==,0);
		g_assert_cmpint (((guchar)buffer[7]),==,0);
		g_assert_cmpint (((guchar)buffer[8]),==,0);
	}
	else {
		g_assert_cmpint (((guchar)buffer[5]),==,255);
		g_assert_cmpint (((guchar)buffer[6]),==,255);
		g_assert_cmpint (((guchar)buffer[7]),==,255);
		g_assert_cmpint (((guchar)buffer[8]),==,255);
	}
}

static void
test18 (void)
{
	guchar buffer[9] = {BINARY_FORMATTER_TYPE_ULONG,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	gulong value;

	g_assert (catalina_binary_formatter_read_ulong (NULL, &value, (gchar*)&buffer [0], 9, NULL));

	if (sizeof (gulong) == 4) {
		g_assert_cmpint (value,==,0xFFFFFFFF);
	}
	else {
		g_assert_cmpint ((guint64)value,==,G_GUINT64_CONSTANT(0xFFFFFFFFFFFFFFFF));
	}
}

static void
test19 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_int64 (NULL, G_MAXUINT64, &buffer, &length, NULL));
	g_assert_cmpint (length,==,9);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_INT64);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);
	g_assert_cmpint (((guchar)buffer[5]),==,255);
	g_assert_cmpint (((guchar)buffer[6]),==,255);
	g_assert_cmpint (((guchar)buffer[7]),==,255);
	g_assert_cmpint (((guchar)buffer[8]),==,255);
}

static void
test20 (void)
{
	guchar buffer[9] = {BINARY_FORMATTER_TYPE_INT64,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	gint64 value;

	g_assert (catalina_binary_formatter_read_int64 (NULL, &value, (gchar*)&buffer [0], 9, NULL));
	g_assert_cmpint ((guint64)value,==,G_GUINT64_CONSTANT(0xFFFFFFFFFFFFFFFF));
}

static void
test21 (void)
{
	gchar *buffer = NULL;
	guint length = 0;

	g_assert (catalina_binary_formatter_write_uint64 (NULL, G_MAXUINT64, &buffer, &length, NULL));
	g_assert_cmpint (length,==,9);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_UINT64);
	g_assert_cmpint (((guchar)buffer[1]),==,255);
	g_assert_cmpint (((guchar)buffer[2]),==,255);
	g_assert_cmpint (((guchar)buffer[3]),==,255);
	g_assert_cmpint (((guchar)buffer[4]),==,255);
	g_assert_cmpint (((guchar)buffer[5]),==,255);
	g_assert_cmpint (((guchar)buffer[6]),==,255);
	g_assert_cmpint (((guchar)buffer[7]),==,255);
	g_assert_cmpint (((guchar)buffer[8]),==,255);
}

static void
test22 (void)
{
	guchar buffer[9] = {BINARY_FORMATTER_TYPE_UINT64,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	guint64 value;

	g_assert (catalina_binary_formatter_read_uint64 (NULL, &value, (gchar*)&buffer [0], 9, NULL));
	g_assert_cmpint ((guint64)value,==,G_GUINT64_CONSTANT(0xFFFFFFFFFFFFFFFF));
}

static void
test23 (void)
{
	gchar *buffer = NULL;
	guint  length = 0;

	g_assert (catalina_binary_formatter_write_string (NULL, "chris", -1, &buffer, &length, NULL));
	g_assert_cmpint (length,==,11);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_STRING);
	g_assert_cmpint (((guchar)buffer[1]),==,0x00);
	g_assert_cmpint (((guchar)buffer[2]),==,0x00);
	g_assert_cmpint (((guchar)buffer[3]),==,0x00);
	g_assert_cmpint (((guchar)buffer[4]),==,0x06);
	g_assert_cmpint (((guchar)buffer[5]),==,'c');
	g_assert_cmpint (((guchar)buffer[6]),==,'h');
	g_assert_cmpint (((guchar)buffer[7]),==,'r');
	g_assert_cmpint (((guchar)buffer[8]),==,'i');
	g_assert_cmpint (((guchar)buffer[9]),==,'s');
	g_assert_cmpint (((guchar)buffer[10]),==,'\0');
}

static void
test24 (void)
{
	guchar  buffer[11] = {BINARY_FORMATTER_TYPE_STRING,0x00,0x00,0x00,0x06,'c','h','r','i','s','\0'};
	guint   length     = 0;
	gchar  *str        = NULL;

	g_assert (catalina_binary_formatter_read_string (NULL, &str, &length, (gchar*)&buffer [0], sizeof (buffer), NULL));
	g_assert_cmpint (length,==,6);
	g_assert_cmpstr (str,==,"chris");
}

static void
test25 (void)
{
	gchar *buffer = NULL;
	guint  length = 0;

	g_assert (catalina_binary_formatter_write_double (NULL, 12313211234.12322, &buffer, &length, NULL));
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_DOUBLE);
	g_assert_cmpint (((guchar)buffer[1]),==,0x00);
	g_assert_cmpint (((guchar)buffer[2]),==,0x00);
	g_assert_cmpint (((guchar)buffer[3]),==,0x00);
	g_assert_cmpint (((guchar)buffer[4]),==,14);
	g_assert_cmpint (((guchar)buffer[5]),==,'1');
	g_assert_cmpint (((guchar)buffer[6]),==,'.');
	g_assert_cmpint (((guchar)buffer[7]),==,'2');
	g_assert_cmpint (((guchar)buffer[8]),==,'3');
	g_assert_cmpint (((guchar)buffer[9]),==,'1');
	g_assert_cmpint (((guchar)buffer[10]),==,'3');
	g_assert_cmpint (((guchar)buffer[11]),==,'2');
	g_assert_cmpint (((guchar)buffer[12]),==,'1');
	g_assert_cmpint (((guchar)buffer[13]),==,'1');
	g_assert_cmpint (((guchar)buffer[14]),==,'e');
	g_assert_cmpint (((guchar)buffer[15]),==,'+');
	g_assert_cmpint (((guchar)buffer[16]),==,'1');
	g_assert_cmpint (((guchar)buffer[17]),==,'0');
	g_assert_cmpint (((guchar)buffer[18]),==,'\0');
}

static void
test26 (void)
{
	guchar buffer[] = {BINARY_FORMATTER_TYPE_DOUBLE,0x000,0x00,0x00,12,'1','.','2','3','1','3','2','e','+','1','0','\0'};
	gdouble v = 0;

	g_assert (catalina_binary_formatter_read_double (NULL, &v, (gchar*)&buffer[0], sizeof (buffer), NULL));
	g_assert_cmpint (v,==,12313200000ull); // lost precision
}

static void
test27 (void)
{
	gchar *buffer = NULL;
	guint  length = 0;

	g_assert (catalina_binary_formatter_write_float (NULL, 12313211234.12322, &buffer, &length, NULL));
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_FLOAT);
	g_assert_cmpint (((guchar)buffer[1]),==,0x00);
	g_assert_cmpint (((guchar)buffer[2]),==,0x00);
	g_assert_cmpint (((guchar)buffer[3]),==,0x00);
	g_assert_cmpint (((guchar)buffer[4]),==,12);
	g_assert_cmpint (((guchar)buffer[5]),==,'1');
	g_assert_cmpint (((guchar)buffer[6]),==,'.');
	g_assert_cmpint (((guchar)buffer[7]),==,'2');
	g_assert_cmpint (((guchar)buffer[8]),==,'3');
	g_assert_cmpint (((guchar)buffer[9]),==,'1');
	g_assert_cmpint (((guchar)buffer[10]),==,'3');
	g_assert_cmpint (((guchar)buffer[11]),==,'2');
	g_assert_cmpint (((guchar)buffer[12]),==,'e');
	g_assert_cmpint (((guchar)buffer[13]),==,'+');
	g_assert_cmpint (((guchar)buffer[14]),==,'1');
	g_assert_cmpint (((guchar)buffer[15]),==,'0');
	g_assert_cmpint (((guchar)buffer[16]),==,'\0');
}

static void
test28 (void)
{
	guchar buffer[] = {BINARY_FORMATTER_TYPE_DOUBLE,0x000,0x00,0x00,12,'1','.','2','3','1','3','2','e','+','1','0','\0'};
	gfloat v = 0;

	g_assert (catalina_binary_formatter_read_float (NULL, &v, (gchar*)&buffer[0], sizeof (buffer), NULL));
	g_assert_cmpint (v,==,12313199616ull); // lost precision
}

static void
test29 (void)
{
	gchar   *buffer = NULL;
	guint    length = 0;
	GObject *o      = (void*)mock_person_new ();

	g_object_set (o, "first-name", "chris", NULL);
	g_assert (catalina_binary_formatter_write_object (NULL, o, &buffer, &length, NULL));
	g_assert_cmpint (length,==,82);
	g_assert_cmpint (((guchar)buffer[0]),==,BINARY_FORMATTER_TYPE_OBJECT);
	g_assert_cmpint (((guchar)buffer[1]),==,strlen("MockPerson") + 1);
	g_assert_cmpint (((guchar)buffer[2]),==,'M');
	g_assert_cmpint (((guchar)buffer[3]),==,'o');
	g_assert_cmpint (((guchar)buffer[4]),==,'c');
	g_assert_cmpint (((guchar)buffer[5]),==,'k');
	g_assert_cmpint (((guchar)buffer[6]),==,'P');
	g_assert_cmpint (((guchar)buffer[7]),==,'e');
	g_assert_cmpint (((guchar)buffer[8]),==,'r');
	g_assert_cmpint (((guchar)buffer[9]),==,'s');
	g_assert_cmpint (((guchar)buffer[10]),==,'o');
	g_assert_cmpint (((guchar)buffer[11]),==,'n');
	g_assert_cmpint (((guchar)buffer[12]),==,'\0');
	/* key */
	g_assert_cmpint (((guchar)buffer[13]),==,11);
	g_assert_cmpint (((guchar)buffer[14]),==,'b');
	g_assert_cmpint (((guchar)buffer[15]),==,'i');
	g_assert_cmpint (((guchar)buffer[16]),==,'r');
	g_assert_cmpint (((guchar)buffer[17]),==,'t');
	g_assert_cmpint (((guchar)buffer[18]),==,'h');
	g_assert_cmpint (((guchar)buffer[19]),==,'-');
	g_assert_cmpint (((guchar)buffer[20]),==,'d');
	g_assert_cmpint (((guchar)buffer[21]),==,'a');
	g_assert_cmpint (((guchar)buffer[22]),==,'t');
	g_assert_cmpint (((guchar)buffer[23]),==,'e');
	g_assert_cmpint (((guchar)buffer[24]),==,'\0');
	/* value */
	g_assert_cmpint (((guchar)buffer[25]),==,0);
	g_assert_cmpint (((guchar)buffer[26]),==,0);
	g_assert_cmpint (((guchar)buffer[27]),==,0);
	g_assert_cmpint (((guchar)buffer[28]),==,5);
	g_assert_cmpint (((guchar)buffer[29]),==,BINARY_FORMATTER_TYPE_STRING);
	g_assert_cmpint (((guchar)buffer[30]),==,0);
	g_assert_cmpint (((guchar)buffer[31]),==,0);
	g_assert_cmpint (((guchar)buffer[32]),==,0);
	g_assert_cmpint (((guchar)buffer[33]),==,0);
	/* key */
	g_assert_cmpint (((guchar)buffer[34]),==,10);
	g_assert_cmpint (((guchar)buffer[35]),==,'l');
	g_assert_cmpint (((guchar)buffer[36]),==,'a');
	g_assert_cmpint (((guchar)buffer[37]),==,'s');
	g_assert_cmpint (((guchar)buffer[38]),==,'t');
	g_assert_cmpint (((guchar)buffer[39]),==,'-');
	g_assert_cmpint (((guchar)buffer[40]),==,'n');
	g_assert_cmpint (((guchar)buffer[41]),==,'a');
	g_assert_cmpint (((guchar)buffer[42]),==,'m');
	g_assert_cmpint (((guchar)buffer[43]),==,'e');
	g_assert_cmpint (((guchar)buffer[44]),==,'\0');
	/* value */
	g_assert_cmpint (((guchar)buffer[45]),==,0);
	g_assert_cmpint (((guchar)buffer[46]),==,0);
	g_assert_cmpint (((guchar)buffer[47]),==,0);
	g_assert_cmpint (((guchar)buffer[48]),==,5);
	g_assert_cmpint (((guchar)buffer[49]),==,BINARY_FORMATTER_TYPE_STRING);
	g_assert_cmpint (((guchar)buffer[50]),==,0);
	g_assert_cmpint (((guchar)buffer[51]),==,0);
	g_assert_cmpint (((guchar)buffer[52]),==,0);
	g_assert_cmpint (((guchar)buffer[53]),==,0);
	/* key */
	g_assert_cmpint (((guchar)buffer[54]),==,11);
	g_assert_cmpint (((guchar)buffer[55]),==,'f');
	g_assert_cmpint (((guchar)buffer[56]),==,'i');
	g_assert_cmpint (((guchar)buffer[57]),==,'r');
	g_assert_cmpint (((guchar)buffer[58]),==,'s');
	g_assert_cmpint (((guchar)buffer[59]),==,'t');
	g_assert_cmpint (((guchar)buffer[60]),==,'-');
	g_assert_cmpint (((guchar)buffer[61]),==,'n');
	g_assert_cmpint (((guchar)buffer[62]),==,'a');
	g_assert_cmpint (((guchar)buffer[63]),==,'m');
	g_assert_cmpint (((guchar)buffer[64]),==,'e');
	g_assert_cmpint (((guchar)buffer[65]),==,'\0');
	/* value */
	g_assert_cmpint (((guchar)buffer[66]),==,0);
	g_assert_cmpint (((guchar)buffer[67]),==,0);
	g_assert_cmpint (((guchar)buffer[68]),==,0);
	g_assert_cmpint (((guchar)buffer[69]),==,11);
	g_assert_cmpint (((guchar)buffer[70]),==,BINARY_FORMATTER_TYPE_STRING);
	g_assert_cmpint (((guchar)buffer[71]),==,0);
	g_assert_cmpint (((guchar)buffer[72]),==,0);
	g_assert_cmpint (((guchar)buffer[73]),==,0);
	g_assert_cmpint (((guchar)buffer[74]),==,6);
	g_assert_cmpint (((guchar)buffer[75]),==,'c');
	g_assert_cmpint (((guchar)buffer[76]),==,'h');
	g_assert_cmpint (((guchar)buffer[77]),==,'r');
	g_assert_cmpint (((guchar)buffer[78]),==,'i');
	g_assert_cmpint (((guchar)buffer[79]),==,'s');
	g_assert_cmpint (((guchar)buffer[80]),==,'\0');
	/* sentinal */
	g_assert_cmpint (((guchar)buffer[81]),==,'\0');
}

static void
test30 (void)
{
	GObject *person;
	gchar   *birth_date = "dummy", *first_name = "dummy", *last_name = "dummy";
	guchar   buffer[] = {
		BINARY_FORMATTER_TYPE_OBJECT,
		strlen("MockPerson") + 1,
		'M','o','c','k','P','e','r','s','o','n','\0',
		11, 'b','i','r','t','h','-','d','a','t','e','\0',
		0, 0, 0, 5, BINARY_FORMATTER_TYPE_STRING, 0, 0, 0, 0, 
		11, 'f','i','r','s','t','-','n','a','m','e','\0',
		0, 0, 0, 11, BINARY_FORMATTER_TYPE_STRING, 0, 0, 0, 6,
		'c', 'h', 'r', 'i', 's', '\0',
		10, 'l','a','s','t','-','n','a','m','e','\0',
		0, 0, 0, 13, BINARY_FORMATTER_TYPE_STRING, 0, 0, 0, 8,
		'h', 'e', 'r', 'g', 'e', 'r', 't', '\0',
		'\0'
	};

	g_assert (catalina_binary_formatter_read_object (NULL, &person, (gchar*)buffer, sizeof (buffer), NULL));
	g_object_get (person, "birth-date", &birth_date, "first-name", &first_name, "last-name", &last_name, NULL);
	g_assert_cmpstr (birth_date,==,NULL);
	g_assert_cmpstr (first_name,==,"chris");
	g_assert_cmpstr (last_name,==,"hergert");
}



static void
test31 (void)
{
	g_assert (catalina_binary_formatter_new ());
}

static void
test32 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, G_MININT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,g_value_get_int(&v2));
}

static void
test33 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT64);
	g_value_set_int64 (&v, G_MAXINT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test34 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_LONG);
	v.data[0].v_long = G_MAXLONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test35 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_DOUBLE);
	v.data[0].v_double = G_MAXDOUBLE;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_double,==,v2.data[0].v_double);
}

static void
test36 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_UINT);
	g_value_set_uint (&v, G_MAXUINT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_uint (&v),==,g_value_get_uint(&v2));
}

static void
test37 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_UINT64);
	g_value_set_uint64 (&v, G_MAXUINT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_uint64,==,v2.data[0].v_uint64);
}

static void
test38 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_ULONG);
	v.data[0].v_long = G_MAXULONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_ulong,==,v2.data[0].v_ulong);
}

static void
test39 (void)
{
	GValue v  = {0,};
	GValue v2 = {0,};
	gchar *b  = NULL;
	gsize  l  = 0;

	g_value_init (&v, G_TYPE_STRING);
	g_value_set_string (&v, "MY TEST STRING");

	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpstr (v.data[0].v_pointer,==,v2.data[0].v_pointer);
}

static void
test40 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT);
	g_value_set_int (&v, G_MAXINT);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (g_value_get_int (&v),==,g_value_get_int(&v2));
}

static void
test41 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_INT64);
	g_value_set_int64 (&v, G_MININT64);
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test42 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	g_value_init (&v, G_TYPE_LONG);
	v.data[0].v_long = G_MINLONG;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	g_assert (catalina_formatter_deserialize (f, &v2, b, l, NULL));
	g_assert_cmpint (v.data[0].v_int64,==,v2.data[0].v_int64);
}

static void
test43 (void)
{
	gchar *b = NULL;
	gsize l = 0;
	GValue v = {0,}, v2 = {0,};
	GError *error = NULL;
	g_value_init (&v, G_TYPE_DOUBLE);
	v.data[0].v_double = G_MINDOUBLE;
	CatalinaFormatter *f = catalina_binary_formatter_new ();
	g_assert (catalina_formatter_serialize (f, &v, &b, &l, NULL));
	g_assert (b != NULL && l > 0);
	if (!catalina_formatter_deserialize (f, &v2, b, l, &error))
		g_error ("%s", error->message);
	g_assert_cmpint (v.data[0].v_double,==,v2.data[0].v_double);
}

gint
main (gint   argc,
      gchar *argv[])
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/CatalinaBinaryFormatter/write_boolean(1)", test1);
	g_test_add_func ("/CatalinaBinaryFormatter/read_boolean(1)", test2);
	g_test_add_func ("/CatalinaBinaryFormatter/write_char(1)", test3);
	g_test_add_func ("/CatalinaBinaryFormatter/read_char(1)", test4);
	g_test_add_func ("/CatalinaBinaryFormatter/write_uchar(1)", test5);
	g_test_add_func ("/CatalinaBinaryFormatter/read_uchar(1)", test6);
	g_test_add_func ("/CatalinaBinaryFormatter/write_short(1)", test7);
	g_test_add_func ("/CatalinaBinaryFormatter/read_short(1)", test8);
	g_test_add_func ("/CatalinaBinaryFormatter/write_ushort(1)", test9);
	g_test_add_func ("/CatalinaBinaryFormatter/read_ushort(1)", test10);
	g_test_add_func ("/CatalinaBinaryFormatter/write_int(1)", test11);
	g_test_add_func ("/CatalinaBinaryFormatter/read_int(1)", test12);
	g_test_add_func ("/CatalinaBinaryFormatter/write_uint(1)", test13);
	g_test_add_func ("/CatalinaBinaryFormatter/read_uint(1)", test14);
	g_test_add_func ("/CatalinaBinaryFormatter/write_long(1)", test15);
	g_test_add_func ("/CatalinaBinaryFormatter/read_long(1)", test16);
	g_test_add_func ("/CatalinaBinaryFormatter/write_ulong(1)", test17);
	g_test_add_func ("/CatalinaBinaryFormatter/read_ulong(1)", test18);
	g_test_add_func ("/CatalinaBinaryFormatter/write_int64(1)", test19);
	g_test_add_func ("/CatalinaBinaryFormatter/read_int64(1)", test20);
	g_test_add_func ("/CatalinaBinaryFormatter/write_uint64(1)", test21);
	g_test_add_func ("/CatalinaBinaryFormatter/read_uint64(1)", test22);
	g_test_add_func ("/CatalinaBinaryFormatter/write_string(1)", test23);
	g_test_add_func ("/CatalinaBinaryFormatter/read_string(1)", test24);
	g_test_add_func ("/CatalinaBinaryFormatter/write_double(1)", test25);
	g_test_add_func ("/CatalinaBinaryFormatter/read_double(1)", test26);
	g_test_add_func ("/CatalinaBinaryFormatter/write_float(1)", test27);
	g_test_add_func ("/CatalinaBinaryFormatter/read_float(1)", test28);
	g_test_add_func ("/CatalinaBinaryFormatter/write_object(1)", test29);
	g_test_add_func ("/CatalinaBinaryFormatter/read_object(1)", test30);
	g_test_add_func ("/CatalinaBinaryFormatter/new(1)", test31);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int>(1)", test32);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int>(2)", test40);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int64>(1)", test3);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<int64>(2)", test41);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<long>(1)", test34);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<long>(2)", test32);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<double>(1)", test35);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<double>(2)", test43);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<uint>(1)", test36);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<uint64>(1)", test37);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<ulong>(1)", test38);
	g_test_add_func ("/CatalinaBinaryFormatter/serialization<string>(1)", test39);

	return g_test_run ();
}
