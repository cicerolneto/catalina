dnl Make automake/libtool output more friendly to humans
dnl
dnl SHAVE_INIT([shavedir],[default_mode])
dnl
dnl shavedir: the directory where the shave scripts are, it defaults to
dnl           $(top_builddir)
dnl default_mode: (enable|disable) default shave mode.  This parameter
dnl               controls shave's behaviour when no option has been
dnl               given to configure.  It defaults to disable.
dnl
dnl * SHAVE_INIT should be called late in your configure.(ac|in) file (just
dnl   before AC_CONFIG_FILE/AC_OUTPUT is perfect.  This macro rewrites CC and
dnl   LIBTOOL, you don't want the configure tests to have these variables
dnl   re-defined.
dnl * This macro requires GNU make's -s option.

AC_DEFUN([_SHAVE_ARG_ENABLE],
[
  AC_ARG_ENABLE([shave],
    AS_HELP_STRING(
      [--enable-shave],
      [use shave to make the build pretty [[default=$1]]]),,
      [enable_shave=$1]
    )
])

AC_DEFUN([SHAVE_INIT],
[
  dnl you can tweak the default value of enable_shave
  m4_if([$2], [enable], [_SHAVE_ARG_ENABLE(yes)], [_SHAVE_ARG_ENABLE(no)])

  if test x"$enable_shave" = xyes; then
    dnl where can we find the shave scripts?
    m4_if([$1],,
      [shavedir="$ac_pwd"],
      [shavedir="$ac_pwd/$1"])
    AC_SUBST(shavedir)

    dnl make is now quiet
    AC_SUBST([MAKEFLAGS], [-s])
    AC_SUBST([AM_MAKEFLAGS], ['`test -z $V && echo -s`'])

    dnl we need sed
    AC_CHECK_PROG(SED,sed,sed,false)

    dnl substitute libtool
    SHAVE_SAVED_LIBTOOL=$LIBTOOL
    LIBTOOL="${SHELL} ${shavedir}/shave-libtool '${SHAVE_SAVED_LIBTOOL}'"
    AC_SUBST(LIBTOOL)

    dnl substitute cc/cxx
    SHAVE_SAVED_CC=$CC
    SHAVE_SAVED_CXX=$CXX
    SHAVE_SAVED_FC=$FC
    SHAVE_SAVED_F77=$F77
    CC="${SHELL} ${shavedir}/shave cc ${SHAVE_SAVED_CC}"
    CXX="${SHELL} ${shavedir}/shave cxx ${SHAVE_SAVED_CXX}"
    FC="${SHELL} ${shavedir}/shave fc ${SHAVE_SAVED_FC}"
    F77="${SHELL} ${shavedir}/shave f77 ${SHAVE_SAVED_F77}"
    AC_SUBST(CC)
    AC_SUBST(CXX)
    AC_SUBST(FC)
    AC_SUBST(F77)

    V=@
  else
    V=1
  fi
  Q='$(V:1=)'
  AC_SUBST(V)
  AC_SUBST(Q)
])

## this one is commonly used with AM_PATH_PYTHONDIR ...
dnl AM_CHECK_PYMOD(MODNAME [,SYMBOL [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]]])
dnl Check if a module containing a given symbol is visible to python.
AC_DEFUN([AM_CHECK_PYMOD],
[AC_REQUIRE([AM_PATH_PYTHON])
py_mod_var=`echo $1['_']$2 | sed 'y%./+-%__p_%'`
AC_MSG_CHECKING(for ifelse([$2],[],,[$2 in ])python module $1)
AC_CACHE_VAL(py_cv_mod_$py_mod_var, [
ifelse([$2],[], [prog="
import sys
try:
        import $1
except ImportError:
        sys.exit(1)
except:
        sys.exit(0)
sys.exit(0)"], [prog="
import $1
$1.$2"])
if $PYTHON -c "$prog" 1>&AC_FD_CC 2>&AC_FD_CC
  then
    eval "py_cv_mod_$py_mod_var=yes"
  else
    eval "py_cv_mod_$py_mod_var=no"
  fi
])
py_val=`eval "echo \`echo '$py_cv_mod_'$py_mod_var\`"`
if test "x$py_val" != xno; then
  AC_MSG_RESULT(yes)
  ifelse([$3], [],, [$3
])dnl
else
  AC_MSG_RESULT(no)
  ifelse([$4], [],, [$4
])dnl
fi
])

dnl a macro to check for ability to create python extensions
dnl  AM_CHECK_PYTHON_HEADERS([ACTION-IF-POSSIBLE], [ACTION-IF-NOT-POSSIBLE])
dnl function also defines PYTHON_INCLUDES
AC_DEFUN([AM_CHECK_PYTHON_HEADERS],
[AC_REQUIRE([AM_PATH_PYTHON])
AC_MSG_CHECKING(for headers required to compile python extensions)
dnl deduce PYTHON_INCLUDES
py_prefix=`$PYTHON -c "import sys; print sys.prefix"`
py_exec_prefix=`$PYTHON -c "import sys; print sys.exec_prefix"`
PYTHON_INCLUDES="-I${py_prefix}/include/python${PYTHON_VERSION}"
if test "$py_prefix" != "$py_exec_prefix"; then
  PYTHON_INCLUDES="$PYTHON_INCLUDES -I${py_exec_prefix}/include/python${PYTHON_VERSION}"
fi
AC_SUBST(PYTHON_INCLUDES)
dnl check if the headers exist:
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"
AC_TRY_CPP([#include <Python.h>],dnl
[AC_MSG_RESULT(found)
$1],dnl
[AC_MSG_RESULT(not found)
$2])
CPPFLAGS="$save_CPPFLAGS"
])

#############################################################################

# ===========================================================================
#           http://www.nongnu.org/autoconf-archive/check_zlib.html
# ===========================================================================
#
# SYNOPSIS
#
#   CHECK_ZLIB()
#
# DESCRIPTION
#
#   This macro searches for an installed zlib library. If nothing was
#   specified when calling configure, it searches first in /usr/local and
#   then in /usr. If the --with-zlib=DIR is specified, it will try to find
#   it in DIR/include/zlib.h and DIR/lib/libz.a. If --without-zlib is
#   specified, the library is not searched at all.
#
#   If either the header file (zlib.h) or the library (libz) is not found,
#   the configuration exits on error, asking for a valid zlib installation
#   directory or --without-zlib.
#
#   The macro defines the symbol HAVE_LIBZ if the library is found. You
#   should use autoheader to include a definition for this symbol in a
#   config.h file. Sample usage in a C/C++ source is as follows:
#
#     #ifdef HAVE_LIBZ
#     #include <zlib.h>
#     #endif /* HAVE_LIBZ */
#
# LICENSE
#
#   Copyright (c) 2008 Loic Dachary <loic@senga.org>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

AC_DEFUN([CHECK_ZLIB],
#
# Handle user hints
#
[AC_MSG_CHECKING(if zlib is wanted)
AC_ARG_WITH(zlib,
[  --with-zlib=DIR root directory path of zlib installation [defaults to
                    /usr/local or /usr if not found in /usr/local]
  --without-zlib to disable zlib usage completely],
[if test "$withval" != no ; then
  AC_MSG_RESULT(yes)
  if test -d "$withval"
  then
    ZLIB_HOME="$withval"
  else
    AC_MSG_WARN([Sorry, $withval does not exist, checking usual places])
  fi
else
  AC_MSG_RESULT(no)
fi])

ZLIB_HOME=/usr/local
if test ! -f "${ZLIB_HOME}/include/zlib.h"
then
        ZLIB_HOME=/usr
fi

#
# Locate zlib, if wanted
#
if test -n "${ZLIB_HOME}"
then
        ZLIB_OLD_LDFLAGS=$LDFLAGS
        ZLIB_OLD_CPPFLAGS=$LDFLAGS
        LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
        CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
        AC_LANG_SAVE
        AC_LANG_C
        AC_CHECK_LIB(z, inflateEnd, [zlib_cv_libz=yes], [zlib_cv_libz=no])
        AC_CHECK_HEADER(zlib.h, [zlib_cv_zlib_h=yes], [zlib_cv_zlib_h=no])
        AC_LANG_RESTORE
        if test "$zlib_cv_libz" = "yes" -a "$zlib_cv_zlib_h" = "yes"
        then
                #
                # If both library and header were found, use them
                #
                AC_CHECK_LIB(z, inflateEnd)
                AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
                AC_MSG_RESULT(ok)
        else
                #
                # If either header or library was not found, revert and bomb
                #
                AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
                LDFLAGS="$ZLIB_OLD_LDFLAGS"
                CPPFLAGS="$ZLIB_OLD_CPPFLAGS"
                AC_MSG_RESULT(failed)
                AC_MSG_ERROR(either specify a valid zlib installation with --with-zlib=DIR or disable zlib usage with --without-zlib)
        fi
fi

])

