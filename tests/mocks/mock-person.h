/* mock-person.h */

#ifndef __MOCK_PERSON_H__
#define __MOCK_PERSON_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define MOCK_TYPE_PERSON            (mock_person_get_type())           
#define MOCK_PERSON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOCK_TYPE_PERSON, MockPerson))     
#define MOCK_PERSON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  MOCK_TYPE_PERSON, MockPersonClass))
#define MOCK_IS_PERSON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOCK_TYPE_PERSON))                 
#define MOCK_IS_PERSON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  MOCK_TYPE_PERSON))                 
#define MOCK_PERSON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  MOCK_TYPE_PERSON, MockPersonClass))

typedef struct _MockPerson        MockPerson;
typedef struct _MockPersonClass   MockPersonClass;
typedef struct _MockPersonPrivate MockPersonPrivate;

struct _MockPerson
{
	GObject parent;

	/*< private >*/
	MockPersonPrivate *priv;
};

struct _MockPersonClass
{
	GObjectClass parent_class;
};

GType       mock_person_get_type (void);
MockPerson* mock_person_new      (void);

G_CONST_RETURN gchar* mock_person_get_first_name (MockPerson *person);
void                  mock_person_set_first_name (MockPerson *person, const gchar* first_name);
G_CONST_RETURN gchar* mock_person_get_last_name  (MockPerson *person);
void                  mock_person_set_last_name  (MockPerson *person, const gchar* last_name);
G_CONST_RETURN gchar* mock_person_get_birth_date (MockPerson *person);
void                  mock_person_set_birth_date (MockPerson *person, const gchar* birth_date);

G_END_DECLS

#endif /* __MOCK_PERSON_H__ */

G_DEFINE_TYPE (MockPerson, mock_person, G_TYPE_OBJECT)

enum
{
	PROP_0,
	PROP_FIRST_NAME,
	PROP_LAST_NAME,
	PROP_BIRTH_DATE,
};

struct _MockPersonPrivate
{
	gchar* first_name;
	gchar* last_name;
	gchar* birth_date;
};

static void
mock_person_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
	switch (property_id) {
	case PROP_FIRST_NAME:
		g_value_set_string (value, MOCK_PERSON (object)->priv->first_name);
		break;
	case PROP_LAST_NAME:
		g_value_set_string (value, MOCK_PERSON (object)->priv->last_name);
		break;
	case PROP_BIRTH_DATE:
		g_value_set_string (value, MOCK_PERSON (object)->priv->birth_date);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
mock_person_set_property (GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
	switch (property_id) {
	case PROP_FIRST_NAME:
		g_free (MOCK_PERSON (object)->priv->first_name);
		MOCK_PERSON (object)->priv->first_name = g_value_dup_string (value);
		break;
	case PROP_LAST_NAME:
		g_free (MOCK_PERSON (object)->priv->last_name);
		MOCK_PERSON (object)->priv->last_name = g_value_dup_string (value);
		break;
	case PROP_BIRTH_DATE:
		g_free (MOCK_PERSON (object)->priv->birth_date);
		MOCK_PERSON (object)->priv->birth_date = g_value_dup_string (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
mock_person_finalize (GObject *object)
{
	G_OBJECT_CLASS (mock_person_parent_class)->finalize (object);
}

static void
mock_person_dispose (GObject *object)
{
}

static void
mock_person_class_init (MockPersonClass *klass)
{
	GObjectClass *object_class;

	g_type_class_add_private (klass, sizeof (MockPersonPrivate));

	object_class = G_OBJECT_CLASS (klass);
	object_class->set_property = mock_person_set_property;
	object_class->get_property = mock_person_get_property;
	object_class->finalize     = mock_person_finalize;
	object_class->dispose      = mock_person_dispose;

	/**
	 * MockPerson:first-name:
	 *
	 * The "first-name" property.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_FIRST_NAME,
	                                 g_param_spec_string ("first-name",
	                                                      "",
	                                                      "",
	                                                      NULL,
	                                                      G_PARAM_READWRITE));

	/**
	 * MockPerson:last-name:
	 *
	 * The "last-name" property.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_LAST_NAME,
	                                 g_param_spec_string ("last-name",
	                                                      "",
	                                                      "",
	                                                      NULL,
	                                                      G_PARAM_READWRITE));

	/**
	 * MockPerson:birth-date:
	 *
	 * The "birth-date" property.
	 */
	g_object_class_install_property (object_class,
	                                 PROP_BIRTH_DATE,
	                                 g_param_spec_string ("birth-date",
	                                                      "",
	                                                      "",
	                                                      NULL,
	                                                      G_PARAM_READWRITE));
}

static void
mock_person_init (MockPerson *person)
{
	person->priv = G_TYPE_INSTANCE_GET_PRIVATE (person,
	                                            MOCK_TYPE_PERSON,
	                                            MockPersonPrivate);
}

/**
 * mock_person_new:
 *
 * Return value: 
 */
MockPerson*
mock_person_new (void)
{
	return g_object_new (MOCK_TYPE_PERSON, NULL);
}

/**
 * mock_person_get_first_name:
 * @person: A #MockPerson
 *
 * Return value: 
 */
G_CONST_RETURN gchar*
mock_person_get_first_name (MockPerson *person)
{
	g_return_val_if_fail (MOCK_IS_PERSON (person), NULL);
	return MOCK_PERSON (person)->priv->first_name;
}

/**
 * mock_person_set_first_name:
 * @person: A #MockPerson
 * @first_name: A #const gchar
 */
void
mock_person_set_first_name (MockPerson   *person,
                            const gchar*  first_name)
{
	g_return_if_fail (MOCK_IS_PERSON (person));
	g_free (MOCK_PERSON (person)->priv->first_name);
	MOCK_PERSON (person)->priv->first_name = g_strdup (first_name);
	g_object_notify (G_OBJECT (person), "first_name");
}

/**
 * mock_person_get_last_name:
 * @person: A #MockPerson
 *
 * Return value: 
 */
G_CONST_RETURN gchar*
mock_person_get_last_name (MockPerson *person)
{
	g_return_val_if_fail (MOCK_IS_PERSON (person), NULL);
	return MOCK_PERSON (person)->priv->last_name;
}

/**
 * mock_person_set_last_name:
 * @person: A #MockPerson
 * @last_name: A #const gchar
 */
void
mock_person_set_last_name (MockPerson   *person,
                           const gchar*  last_name)
{
	g_return_if_fail (MOCK_IS_PERSON (person));
	g_free (MOCK_PERSON (person)->priv->last_name);
	MOCK_PERSON (person)->priv->last_name = g_strdup (last_name);
	g_object_notify (G_OBJECT (person), "last_name");
}

/**
 * mock_person_get_birth_date:
 * @person: A #MockPerson
 *
 * Return value: 
 */
G_CONST_RETURN gchar*
mock_person_get_birth_date (MockPerson *person)
{
	g_return_val_if_fail (MOCK_IS_PERSON (person), NULL);
	return MOCK_PERSON (person)->priv->birth_date;
}

/**
 * mock_person_set_birth_date:
 * @person: A #MockPerson
 * @birth_date: A #const gchar
 */
void
mock_person_set_birth_date (MockPerson   *person,
                            const gchar*  birth_date)
{
	g_return_if_fail (MOCK_IS_PERSON (person));
	g_free (MOCK_PERSON (person)->priv->birth_date);
	MOCK_PERSON (person)->priv->birth_date = g_strdup (birth_date);
	g_object_notify (G_OBJECT (person), "birth_date");
}

