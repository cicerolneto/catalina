m4_define([catalina_major_version], [0])
m4_define([catalina_minor_version], [1])
m4_define([catalina_micro_version], [0])
m4_define([catalina_version], [catalina_major_version.catalina_minor_version.catalina_micro_version])
m4_define([catalina_interface_age], [0])
m4_define([catalina_binary_age], [m4_eval(100 * catalina_minor_version + catalina_micro_version)])
m4_define([lt_current], [m4_eval(100 * catalina_minor_version + catalina_micro_version - catalina_interface_age)])
m4_define([lt_revision], [catalina_interface_age])
m4_define([lt_age], [m4_eval(catalina_binary_age - catalina_interface_age)])

m4_define([glib_req_version], [2.16])
m4_define([iris_req_version], [0.1.1])

AC_PREREQ([2.59])

AC_INIT([catalina],
        [catalina_version],
        [],
        [catalina])

AC_CONFIG_SRCDIR([catalina/catalina.h])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE

AM_PROG_CC_C_O
AM_PATH_GLIB_2_0

CATALINA_MAJOR_VERSION=catalina_major_version
CATALINA_MINOR_VERSION=catalina_minor_version
CATALINA_MICRO_VERSION=catalina_micro_version
CATALINA_API_VERSION=1.0
AC_SUBST(CATALINA_MAJOR_VERSION)
AC_SUBST(CATALINA_MICRO_VERSION)
AC_SUBST(CATALINA_MINOR_VERSION)
AC_SUBST(CATALINA_API_VERSION)
AC_SUBST(CATALINA_VERSION)

CATALINA_LT_CURRENT=lt_current
CATALINA_LT_REVISION=lt_revision
CATALINA_LT_AGE=lt_age
CATALINA_LT_VERSION="$CATALINA_LT_CURRENT:$CATALINA_LT_REVISION:$CATALINA_LT_AGE"
CATALINA_LT_LDFLAGS="-version-info $CATALINA_LT_VERSION"
AC_SUBST(CATALINA_LT_LDFLAGS)

AC_PROG_LIBTOOL
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GTESTER], [gtester])
AC_PATH_PROG([GTESTER_REPORT], [gtester-report])

PKG_CHECK_MODULES(CATALINA, gobject-2.0 >= glib_req_version iris-1.0 >= iris_req_version)
AC_SUBST(CATALINA_CFLAGS)
AC_SUBST(CATALINA_LIBS)

AM_CONDITIONAL(ENABLE_GLIB_TEST, test "x$enable_glibtest" = "xyes")

AX_PATH_BDB([4.6],[
	LIBS="$BDB_LIBS"
	LDFLAGS="$BDB_LDFLAGS $LDFLAGS"
	CPPFLAGS="$CPPFLAGS $BDB_CPPFLAGS"
])

dnl = Enable strict compiler flags =========================================
# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], m4_if(m4_eval(catalina_minor_version % 2), [1], [yes], [no]))
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CATALINA_MAINTAINER_CFLAGS="-Werror -Wall -Wshadow -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
fi

AC_SUBST(CATALINA_MAINTAINER_CFLAGS)

dnl = Check for GObject Introspection ======================================

AC_MSG_CHECKING([whether GObject introspection is requested])
AC_ARG_ENABLE([introspection],
	      AS_HELP_STRING([--enable-introspection],[Enable GObject introspection]),
	      [],[enable_introspection=no])
AC_MSG_RESULT([$enable_introspection])

G_IR_SCANNER=
G_IR_COMPILER=
G_IR_GENERATE=
GIRDIR=
TYPELIBDIR=
if test "$enable_introspection" = "yes"; then
   G_IR_SCANNER=`pkg-config --variable=g_ir_scanner gobject-introspection-1.0`
   G_IR_COMPILER=`pkg-config --variable=g_ir_compiler gobject-introspection-1.0`
   G_IR_GENERATE=`pkg-config --variable=g_ir_generate gobject-introspection-1.0`
   GIRDIR=`pkg-config --variable=girdir gobject-introspection-1.0`
   TYPELIBDIR="$(pkg-config --variable=typelibdir gobject-introspection-1.0)"
fi

AC_SUBST(G_IR_SCANNER)
AC_SUBST(G_IR_COMPILER)
AC_SUBST(G_IR_GENERATE)
AC_SUBST(GIRDIR)
AC_SUBST(TYPELIBDIR)

AM_CONDITIONAL([ENABLE_INTROSPECTION],[test "$enable_introspection" = "yes"])

dnl = Misc =================================================================
GTK_DOC_CHECK([1.11])
SHAVE_INIT([.], [enable])

AC_CONFIG_FILES([
	shave-libtool
	shave
	Makefile
	catalina/Makefile
	catalina/catalina-version.h
	tests/Makefile
	doc/Makefile
	doc/reference/Makefile
	doc/reference/version.xml
	catalina.pc
])

AC_OUTPUT

echo ""
echo " Catalina $VERSION"
echo ""
echo "   Prefix...................:  ${prefix}"
echo "   Debug level..............:  ${enable_debug}"
echo "   Maintainer Compiler flags:  ${enable_maintainer_flags}"
echo "   Build API reference......:  ${enable_gtk_doc}"
echo "   Enable test suite........:  ${enable_glibtest}"
echo "   GObject Introspection....:  ${enable_introspection}"
echo ""

if test "x$enable_introspection" = "xyes"; then
echo "   GObject introspection may be broke for older GObject Introspection "
echo "   versions.  Ubuntu Jaunty Jackalope 9.10 is known to be broken!"
echo ""
fi
