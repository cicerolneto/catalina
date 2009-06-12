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




# ===========================================================================
#       http://www.nongnu.org/autoconf-archive/ax_compare_version.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_COMPARE_VERSION(VERSION_A, OP, VERSION_B, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   This macro compares two version strings. Due to the various number of
#   minor-version numbers that can exist, and the fact that string
#   comparisons are not compatible with numeric comparisons, this is not
#   necessarily trivial to do in a autoconf script. This macro makes doing
#   these comparisons easy.
#
#   The six basic comparisons are available, as well as checking equality
#   limited to a certain number of minor-version levels.
#
#   The operator OP determines what type of comparison to do, and can be one
#   of:
#
#    eq  - equal (test A == B)
#    ne  - not equal (test A != B)
#    le  - less than or equal (test A <= B)
#    ge  - greater than or equal (test A >= B)
#    lt  - less than (test A < B)
#    gt  - greater than (test A > B)
#
#   Additionally, the eq and ne operator can have a number after it to limit
#   the test to that number of minor versions.
#
#    eq0 - equal up to the length of the shorter version
#    ne0 - not equal up to the length of the shorter version
#    eqN - equal up to N sub-version levels
#    neN - not equal up to N sub-version levels
#
#   When the condition is true, shell commands ACTION-IF-TRUE are run,
#   otherwise shell commands ACTION-IF-FALSE are run. The environment
#   variable 'ax_compare_version' is always set to either 'true' or 'false'
#   as well.
#
#   Examples:
#
#     AX_COMPARE_VERSION([3.15.7],[lt],[3.15.8])
#     AX_COMPARE_VERSION([3.15],[lt],[3.15.8])
#
#   would both be true.
#
#     AX_COMPARE_VERSION([3.15.7],[eq],[3.15.8])
#     AX_COMPARE_VERSION([3.15],[gt],[3.15.8])
#
#   would both be false.
#
#     AX_COMPARE_VERSION([3.15.7],[eq2],[3.15.8])
#
#   would be true because it is only comparing two minor versions.
#
#     AX_COMPARE_VERSION([3.15.7],[eq0],[3.15])
#
#   would be true because it is only comparing the lesser number of minor
#   versions of the two values.
#
#   Note: The characters that separate the version numbers do not matter. An
#   empty string is the same as version 0. OP is evaluated by autoconf, not
#   configure, so must be a string, not a variable.
#
#   The author would like to acknowledge Guido Draheim whose advice about
#   the m4_case and m4_ifvaln functions make this macro only include the
#   portions necessary to perform the specific comparison specified by the
#   OP argument in the final configure script.
#
# LICENSE
#
#   Copyright (c) 2008 Tim Toolan <toolan@ele.uri.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

dnl #########################################################################
AC_DEFUN([AX_COMPARE_VERSION], [
  AC_PROG_AWK

  # Used to indicate true or false condition
  ax_compare_version=false

  # Convert the two version strings to be compared into a format that
  # allows a simple string comparison.  The end result is that a version
  # string of the form 1.12.5-r617 will be converted to the form
  # 0001001200050617.  In other words, each number is zero padded to four
  # digits, and non digits are removed.
  AS_VAR_PUSHDEF([A],[ax_compare_version_A])
  A=`echo "$1" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  AS_VAR_PUSHDEF([B],[ax_compare_version_B])
  B=`echo "$3" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  dnl # In the case of le, ge, lt, and gt, the strings are sorted as necessary
  dnl # then the first line is used to determine if the condition is true.
  dnl # The sed right after the echo is to remove any indented white space.
  m4_case(m4_tolower($2),
  [lt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [gt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [le],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],
  [ge],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],[
    dnl Split the operator from the subversion count if present.
    m4_bmatch(m4_substr($2,2),
    [0],[
      # A count of zero means use the length of the shorter version.
      # Determine the number of characters in A and B.
      ax_compare_version_len_A=`echo "$A" | $AWK '{print(length)}'`
      ax_compare_version_len_B=`echo "$B" | $AWK '{print(length)}'`

      # Set A to no more than B's length and B to no more than A's length.
      A=`echo "$A" | sed "s/\(.\{$ax_compare_version_len_B\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(.\{$ax_compare_version_len_A\}\).*/\1/"`
    ],
    [[0-9]+],[
      # A count greater than zero means use only that many subversions
      A=`echo "$A" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
    ],
    [.+],[
      AC_WARNING(
        [illegal OP numeric parameter: $2])
    ],[])

    # Pad zeros at end of numbers to make same length.
    ax_compare_version_tmp_A="$A`echo $B | sed 's/./0/g'`"
    B="$B`echo $A | sed 's/./0/g'`"
    A="$ax_compare_version_tmp_A"

    # Check for equality or inequality as necessary.
    m4_case(m4_tolower(m4_substr($2,0,2)),
    [eq],[
      test "x$A" = "x$B" && ax_compare_version=true
    ],
    [ne],[
      test "x$A" != "x$B" && ax_compare_version=true
    ],[
      AC_WARNING([illegal OP parameter: $2])
    ])
  ])

  AS_VAR_POPDEF([A])dnl
  AS_VAR_POPDEF([B])dnl

  dnl # Execute ACTION-IF-TRUE / ACTION-IF-FALSE.
  if test "$ax_compare_version" = "true" ; then
    m4_ifvaln([$4],[$4],[:])dnl
    m4_ifvaln([$5],[else $5])dnl
  fi
]) dnl AX_COMPARE_VERSION





# ===========================================================================
#           http://www.nongnu.org/autoconf-archive/ax_path_bdb.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PATH_BDB([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro finds the latest version of Berkeley DB on the system, and
#   ensures that the header file and library versions match. If
#   MINIMUM-VERSION is specified, it will ensure that the library found is
#   at least that version.
#
#   It determines the name of the library as well as the path to the header
#   file and library. It will check both the default environment as well as
#   the default Berkeley DB install location. When found, it sets BDB_LIBS,
#   BDB_CPPFLAGS, and BDB_LDFLAGS to the necessary values to add to LIBS,
#   CPPFLAGS, and LDFLAGS, as well as setting BDB_VERSION to the version
#   found. HAVE_DB_H is defined also.
#
#   The option --with-bdb-dir=DIR can be used to specify a specific Berkeley
#   DB installation to use.
#
#   An example of it's use is:
#
#      AX_PATH_BDB([3],[
#        LIBS="$BDB_LIBS $LIBS"
#        LDFLAGS="$BDB_LDFLAGS $LDFLAGS"
#        CPPFLAGS="$CPPFLAGS $BDB_CPPFLAGS"
#      ])
#
#   which will locate the latest version of Berkeley DB on the system, and
#   ensure that it is version 3.0 or higher.
#
#   Details: This macro does not use either AC_CHECK_HEADERS or AC_CHECK_LIB
#   because, first, the functions inside the library are sometimes renamed
#   to contain a version code that is only available from the db.h on the
#   system, and second, because it is common to have multiple db.h and libdb
#   files on a system it is important to make sure the ones being used
#   correspond to the same version. Additionally, there are many different
#   possible names for libdb when installed by an OS distribution, and these
#   need to be checked if db.h does not correspond to libdb.
#
#   When cross compiling, only header versions are verified since it would
#   be difficult to check the library version. Additionally the default
#   Berkeley DB installation locations /usr/local/BerkeleyDB* are not
#   searched for higher versions of the library.
#
#   The format for the list of library names to search came from the Cyrus
#   IMAP distribution, although they are generated dynamically here, and
#   only for the version found in db.h.
#
#   The macro AX_COMPARE_VERSION is required to use this macro, and should
#   be available from the Autoconf Macro Archive.
#
#   The author would like to acknowledge the generous and valuable feedback
#   from Guido Draheim, without which this macro would be far less robust,
#   and have poor and inconsistent cross compilation support.
#
#   Changes:
#
#    1/5/05 applied patch from Rafa Rzepecki to eliminate compiler
#           warning about unused variable, argv
#
# LICENSE
#
#   Copyright (c) 2008 Tim Toolan <toolan@ele.uri.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

dnl #########################################################################
AC_DEFUN([AX_PATH_BDB], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_ok=no

  # Add --with-bdb-dir option to configure.
  AC_ARG_WITH([bdb-dir],
    [AC_HELP_STRING([--with-bdb-dir=DIR],
                    [Berkeley DB installation directory])])

  # Check if --with-bdb-dir was specified.
  if test "x$with_bdb_dir" = "x" ; then
    # No option specified, so just search the system.
    AX_PATH_BDB_NO_OPTIONS([$1], [HIGHEST], [
      ax_path_bdb_ok=yes
    ])
   else
     # Set --with-bdb-dir option.
     ax_path_bdb_INC="$with_bdb_dir/include"
     ax_path_bdb_LIB="$with_bdb_dir/lib"

     dnl # Save previous environment, and modify with new stuff.
     ax_path_bdb_save_CPPFLAGS="$CPPFLAGS"
     CPPFLAGS="-I$ax_path_bdb_INC $CPPFLAGS"

     ax_path_bdb_save_LDFLAGS=$LDFLAGS
     LDFLAGS="-L$ax_path_bdb_LIB $LDFLAGS"

     # Check for specific header file db.h
     AC_MSG_CHECKING([db.h presence in $ax_path_bdb_INC])
     if test -f "$ax_path_bdb_INC/db.h" ; then
       AC_MSG_RESULT([yes])
       # Check for library
       AX_PATH_BDB_NO_OPTIONS([$1], [ENVONLY], [
         ax_path_bdb_ok=yes
         BDB_CPPFLAGS="-I$ax_path_bdb_INC"
         BDB_LDFLAGS="-L$ax_path_bdb_LIB"
       ])
     else
       AC_MSG_RESULT([no])
       AC_MSG_NOTICE([no usable Berkeley DB not found])
     fi

     dnl # Restore the environment.
     CPPFLAGS="$ax_path_bdb_save_CPPFLAGS"
     LDFLAGS="$ax_path_bdb_save_LDFLAGS"

  fi

  dnl # Execute ACTION-IF-FOUND / ACTION-IF-NOT-FOUND.
  if test "$ax_path_bdb_ok" = "yes" ; then
    m4_ifvaln([$2],[$2],[:])dnl
    m4_ifvaln([$3],[else $3])dnl
  fi

]) dnl AX_PATH_BDB

dnl #########################################################################
dnl Check for berkeley DB of at least MINIMUM-VERSION on system.
dnl
dnl The OPTION argument determines how the checks occur, and can be one of:
dnl
dnl   HIGHEST -  Check both the environment and the default installation
dnl              directories for Berkeley DB and choose the version that
dnl              is highest. (default)
dnl   ENVFIRST - Check the environment first, and if no satisfactory
dnl              library is found there check the default installation
dnl              directories for Berkeley DB which is /usr/local/BerkeleyDB*
dnl   ENVONLY -  Check the current environment only.
dnl
dnl Requires AX_PATH_BDB_PATH_GET_VERSION, AX_PATH_BDB_PATH_FIND_HIGHEST,
dnl          AX_PATH_BDB_ENV_CONFIRM_LIB, AX_PATH_BDB_ENV_GET_VERSION, and
dnl          AX_COMPARE_VERSION macros.
dnl
dnl Result: sets ax_path_bdb_no_options_ok to yes or no
dnl         sets BDB_LIBS, BDB_CPPFLAGS, BDB_LDFLAGS, BDB_VERSION
dnl
dnl AX_PATH_BDB_NO_OPTIONS([MINIMUM-VERSION], [OPTION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([AX_PATH_BDB_NO_OPTIONS], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_no_options_ok=no

  # Values to add to environment to use Berkeley DB.
  BDB_VERSION=''
  BDB_LIBS=''
  BDB_CPPFLAGS=''
  BDB_LDFLAGS=''

  # Check cross compilation here.
  if test "x$cross_compiling" = "xyes" ; then
    # If cross compiling, can't use AC_RUN_IFELSE so do these tests.
    # The AC_PREPROC_IFELSE confirms that db.h is preprocessable,
    # and extracts the version number from it.
    AC_MSG_CHECKING([for db.h])

    AS_VAR_PUSHDEF([HEADER_VERSION],[ax_path_bdb_no_options_HEADER_VERSION])dnl
    HEADER_VERSION=''
    AC_PREPROC_IFELSE([
      AC_LANG_SOURCE([[
#include <db.h>
AX_PATH_BDB_STUFF DB_VERSION_MAJOR,DB_VERSION_MINOR,DB_VERSION_PATCH
      ]])
    ],[
      # Extract version from preprocessor output.
      HEADER_VERSION=`eval "$ac_cpp conftest.$ac_ext" 2> /dev/null \
        | grep AX_PATH_BDB_STUFF | sed 's/[[^0-9,]]//g;s/,/./g;1q'`
    ],[])

    if test "x$HEADER_VERSION" = "x" ; then
      AC_MSG_RESULT([no])
    else
      AC_MSG_RESULT([$HEADER_VERSION])

      # Check that version is high enough.
      AX_COMPARE_VERSION([$HEADER_VERSION],[ge],[$1],[
        # get major and minor version numbers
        AS_VAR_PUSHDEF([MAJ],[ax_path_bdb_no_options_MAJOR])dnl
        MAJ=`echo $HEADER_VERSION | sed 's,\..*,,'`
        AS_VAR_PUSHDEF([MIN],[ax_path_bdb_no_options_MINOR])dnl
        MIN=`echo $HEADER_VERSION | sed 's,^[[0-9]]*\.,,;s,\.[[0-9]]*$,,'`

	dnl # Save LIBS.
	ax_path_bdb_no_options_save_LIBS="$LIBS"

        # Check that we can link with the library.
        AC_SEARCH_LIBS([db_version],
          [db db-$MAJ.$MIN db$MAJ.$MIN db$MAJ$MIN db-$MAJ db$MAJ],[
            # Sucessfully found library.
            ax_path_bdb_no_options_ok=yes
            BDB_VERSION=$HEADER_VERSION

	    # Extract library from LIBS
	    ax_path_bdb_no_options_LEN=` \
              echo "x$ax_path_bdb_no_options_save_LIBS" \
              | awk '{print(length)}'`
            BDB_LIBS=`echo "x$LIBS " \
              | sed "s/.\{$ax_path_bdb_no_options_LEN\}\$//;s/^x//;s/ //g"`
        ],[])

        dnl # Restore LIBS
	LIBS="$ax_path_bdb_no_options_save_LIBS"

        AS_VAR_POPDEF([MAJ])dnl
        AS_VAR_POPDEF([MIN])dnl
      ])
    fi

    AS_VAR_POPDEF([HEADER_VERSION])dnl
  else
    # Not cross compiling.
    # Check version of Berkeley DB in the current environment.
    AX_PATH_BDB_ENV_GET_VERSION([
      AX_COMPARE_VERSION([$ax_path_bdb_env_get_version_VERSION],[ge],[$1],[
        # Found acceptable version in current environment.
        ax_path_bdb_no_options_ok=yes
        BDB_VERSION="$ax_path_bdb_env_get_version_VERSION"
        BDB_LIBS="$ax_path_bdb_env_get_version_LIBS"
      ])
    ])

    # Determine if we need to search /usr/local/BerkeleyDB*
    ax_path_bdb_no_options_DONE=no
    if test "x$2" = "xENVONLY" ; then
      ax_path_bdb_no_options_DONE=yes
    elif test "x$2" = "xENVFIRST" ; then
      ax_path_bdb_no_options_DONE=$ax_path_bdb_no_options_ok
    fi

    if test "$ax_path_bdb_no_options_DONE" = "no" ; then
      # Check for highest in /usr/local/BerkeleyDB*
      AX_PATH_BDB_PATH_FIND_HIGHEST([
        if test "$ax_path_bdb_no_options_ok" = "yes" ; then
        # If we already have an acceptable version use this if higher.
          AX_COMPARE_VERSION(
             [$ax_path_bdb_path_find_highest_VERSION],[gt],[$BDB_VERSION])
        else
          # Since we didn't have an acceptable version check if this one is.
          AX_COMPARE_VERSION(
             [$ax_path_bdb_path_find_highest_VERSION],[ge],[$1])
        fi
      ])

      dnl # If result from _AX_COMPARE_VERSION is true we want this version.
      if test "$ax_compare_version" = "true" ; then
        ax_path_bdb_no_options_ok=yes
        BDB_LIBS="-ldb"
	if test "x$ax_path_bdb_path_find_highest_DIR" != x ; then
	  BDB_CPPFLAGS="-I$ax_path_bdb_path_find_highest_DIR/include"
	  BDB_LDFLAGS="-L$ax_path_bdb_path_find_highest_DIR/lib"
	fi
        BDB_VERSION="$ax_path_bdb_path_find_highest_VERSION"
      fi
    fi
  fi

  dnl # Execute ACTION-IF-FOUND / ACTION-IF-NOT-FOUND.
  if test "$ax_path_bdb_no_options_ok" = "yes" ; then
    AC_MSG_NOTICE([using Berkeley DB version $BDB_VERSION])
    AC_DEFINE([HAVE_DB_H],[1],
              [Define to 1 if you have the <db.h> header file.])
    m4_ifvaln([$3],[$3])dnl
  else
    AC_MSG_NOTICE([no Berkeley DB version $1 or higher found])
    m4_ifvaln([$4],[$4])dnl
  fi
]) dnl AX_PATH_BDB_NO_OPTIONS

dnl #########################################################################
dnl Check the default installation directory for Berkeley DB which is
dnl of the form /usr/local/BerkeleyDB* for the highest version.
dnl
dnl Result: sets ax_path_bdb_path_find_highest_ok to yes or no,
dnl         sets ax_path_bdb_path_find_highest_VERSION to version,
dnl         sets ax_path_bdb_path_find_highest_DIR to directory.
dnl
dnl AX_PATH_BDB_PATH_FIND_HIGHEST([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([AX_PATH_BDB_PATH_FIND_HIGHEST], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_path_find_highest_ok=no

  AS_VAR_PUSHDEF([VERSION],[ax_path_bdb_path_find_highest_VERSION])dnl
  VERSION=''

  ax_path_bdb_path_find_highest_DIR=''

  # find highest verison in default install directory for Berkeley DB
  AS_VAR_PUSHDEF([CURDIR],[ax_path_bdb_path_find_highest_CURDIR])dnl
  AS_VAR_PUSHDEF([CUR_VERSION],[ax_path_bdb_path_get_version_VERSION])dnl

  for CURDIR in `ls -d /usr/local/BerkeleyDB* 2> /dev/null`
  do
    AX_PATH_BDB_PATH_GET_VERSION([$CURDIR],[
      AX_COMPARE_VERSION([$CUR_VERSION],[gt],[$VERSION],[
        ax_path_bdb_path_find_highest_ok=yes
        ax_path_bdb_path_find_highest_DIR="$CURDIR"
        VERSION="$CUR_VERSION"
      ])
    ])
  done

  AS_VAR_POPDEF([VERSION])dnl
  AS_VAR_POPDEF([CUR_VERSION])dnl
  AS_VAR_POPDEF([CURDIR])dnl

  dnl # Execute ACTION-IF-FOUND / ACTION-IF-NOT-FOUND.
  if test "$ax_path_bdb_path_find_highest_ok" = "yes" ; then
    m4_ifvaln([$1],[$1],[:])dnl
    m4_ifvaln([$2],[else $2])dnl
  fi

]) dnl AX_PATH_BDB_PATH_FIND_HIGHEST

dnl #########################################################################
dnl Checks for Berkeley DB in specified directory's lib and include
dnl subdirectories.
dnl
dnl Result: sets ax_path_bdb_path_get_version_ok to yes or no,
dnl         sets ax_path_bdb_path_get_version_VERSION to version.
dnl
dnl AX_PATH_BDB_PATH_GET_VERSION(BDB-DIR, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([AX_PATH_BDB_PATH_GET_VERSION], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_path_get_version_ok=no

  # Indicate status of checking for Berkeley DB header.
  AC_MSG_CHECKING([in $1/include for db.h])
  ax_path_bdb_path_get_version_got_header=no
  test -f "$1/include/db.h" && ax_path_bdb_path_get_version_got_header=yes
  AC_MSG_RESULT([$ax_path_bdb_path_get_version_got_header])

  # Indicate status of checking for Berkeley DB library.
  AC_MSG_CHECKING([in $1/lib for library -ldb])

  ax_path_bdb_path_get_version_VERSION=''

  if test -d "$1/include" && test -d "$1/lib" &&
     test "$ax_path_bdb_path_get_version_got_header" = "yes" ; then
    dnl # save and modify environment
    ax_path_bdb_path_get_version_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="-I$1/include $CPPFLAGS"

    ax_path_bdb_path_get_version_save_LIBS="$LIBS"
    LIBS="$LIBS -ldb"

    ax_path_bdb_path_get_version_save_LDFLAGS="$LDFLAGS"
    LDFLAGS="-L$1/lib $LDFLAGS"

    # Compile and run a program that compares the version defined in
    # the header file with a version defined in the library function
    # db_version.
    AC_RUN_IFELSE([
      AC_LANG_SOURCE([[
#include <stdio.h>
#include <db.h>
int main(int argc,char **argv)
{
  int major,minor,patch;
  (void) argv;
  db_version(&major,&minor,&patch);
  if (argc > 1)
    printf("%d.%d.%d\n",DB_VERSION_MAJOR,DB_VERSION_MINOR,DB_VERSION_PATCH);
  if (DB_VERSION_MAJOR == major && DB_VERSION_MINOR == minor &&
      DB_VERSION_PATCH == patch)
    return 0;
  else
    return 1;
}
      ]])
    ],[
      # Program compiled and ran, so get version by adding argument.
      ax_path_bdb_path_get_version_VERSION=`./conftest$ac_exeext x`
      ax_path_bdb_path_get_version_ok=yes
    ],[],[])

    dnl # restore environment
    CPPFLAGS="$ax_path_bdb_path_get_version_save_CPPFLAGS"
    LIBS="$ax_path_bdb_path_get_version_save_LIBS"
    LDFLAGS="$ax_path_bdb_path_get_version_save_LDFLAGS"
  fi

  dnl # Finally, execute ACTION-IF-FOUND / ACTION-IF-NOT-FOUND.
  if test "$ax_path_bdb_path_get_version_ok" = "yes" ; then
    AC_MSG_RESULT([$ax_path_bdb_path_get_version_VERSION])
    m4_ifvaln([$2],[$2])dnl
  else
    AC_MSG_RESULT([no])
    m4_ifvaln([$3],[$3])dnl
  fi
]) dnl AX_PATH_BDB_PATH_GET_VERSION

#############################################################################
dnl Checks if version of library and header match specified version.
dnl Only meant to be used by AX_PATH_BDB_ENV_GET_VERSION macro.
dnl
dnl Requires AX_COMPARE_VERSION macro.
dnl
dnl Result: sets ax_path_bdb_env_confirm_lib_ok to yes or no.
dnl
dnl AX_PATH_BDB_ENV_CONFIRM_LIB(VERSION, [LIBNAME])
AC_DEFUN([AX_PATH_BDB_ENV_CONFIRM_LIB], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_env_confirm_lib_ok=no

  dnl # save and modify environment to link with library LIBNAME
  ax_path_bdb_env_confirm_lib_save_LIBS="$LIBS"
  LIBS="$LIBS $2"

  # Compile and run a program that compares the version defined in
  # the header file with a version defined in the library function
  # db_version.
  AC_RUN_IFELSE([
    AC_LANG_SOURCE([[
#include <stdio.h>
#include <db.h>
int main(int argc,char **argv)
{
  int major,minor,patch;
  (void) argv;
  db_version(&major,&minor,&patch);
  if (argc > 1)
    printf("%d.%d.%d\n",DB_VERSION_MAJOR,DB_VERSION_MINOR,DB_VERSION_PATCH);
  if (DB_VERSION_MAJOR == major && DB_VERSION_MINOR == minor &&
      DB_VERSION_PATCH == patch)
    return 0;
  else
    return 1;
}
    ]])
  ],[
    # Program compiled and ran, so get version by giving an argument,
    # which will tell the program to print the output.
    ax_path_bdb_env_confirm_lib_VERSION=`./conftest$ac_exeext x`

    # If the versions all match up, indicate success.
    AX_COMPARE_VERSION([$ax_path_bdb_env_confirm_lib_VERSION],[eq],[$1],[
      ax_path_bdb_env_confirm_lib_ok=yes
    ])
  ],[],[])

  dnl # restore environment
  LIBS="$ax_path_bdb_env_confirm_lib_save_LIBS"

]) dnl AX_PATH_BDB_ENV_CONFIRM_LIB

#############################################################################
dnl Finds the version and library name for Berkeley DB in the
dnl current environment.  Tries many different names for library.
dnl
dnl Requires AX_PATH_BDB_ENV_CONFIRM_LIB macro.
dnl
dnl Result: set ax_path_bdb_env_get_version_ok to yes or no,
dnl         set ax_path_bdb_env_get_version_VERSION to the version found,
dnl         and ax_path_bdb_env_get_version_LIBNAME to the library name.
dnl
dnl AX_PATH_BDB_ENV_GET_VERSION([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([AX_PATH_BDB_ENV_GET_VERSION], [
  dnl # Used to indicate success or failure of this function.
  ax_path_bdb_env_get_version_ok=no

  ax_path_bdb_env_get_version_VERSION=''
  ax_path_bdb_env_get_version_LIBS=''

  AS_VAR_PUSHDEF([HEADER_VERSION],[ax_path_bdb_env_get_version_HEADER_VERSION])dnl
  AS_VAR_PUSHDEF([TEST_LIBNAME],[ax_path_bdb_env_get_version_TEST_LIBNAME])dnl

  # Indicate status of checking for Berkeley DB library.
  AC_MSG_CHECKING([for db.h])

  # Compile and run a program that determines the Berkeley DB version
  # in the header file db.h.
  HEADER_VERSION=''
  AC_RUN_IFELSE([
    AC_LANG_SOURCE([[
#include <stdio.h>
#include <db.h>
int main(int argc,char **argv)
{
  (void) argv;
  if (argc > 1)
    printf("%d.%d.%d\n",DB_VERSION_MAJOR,DB_VERSION_MINOR,DB_VERSION_PATCH);
  return 0;
}
    ]])
  ],[
    # Program compiled and ran, so get version by adding an argument.
    HEADER_VERSION=`./conftest$ac_exeext x`
    AC_MSG_RESULT([$HEADER_VERSION])
  ],[AC_MSG_RESULT([no])],[AC_MSG_RESULT([no])])

  # Have header version, so try to find corresponding library.
  # Looks for library names in the order:
  #   nothing, db, db-X.Y, dbX.Y, dbXY, db-X, dbX
  # and stops when it finds the first one that matches the version
  # of the header file.
  if test "x$HEADER_VERSION" != "x" ; then
    AC_MSG_CHECKING([for library containing Berkeley DB $HEADER_VERSION])

    AS_VAR_PUSHDEF([MAJOR],[ax_path_bdb_env_get_version_MAJOR])dnl
    AS_VAR_PUSHDEF([MINOR],[ax_path_bdb_env_get_version_MINOR])dnl

    # get major and minor version numbers
    MAJOR=`echo $HEADER_VERSION | sed 's,\..*,,'`
    MINOR=`echo $HEADER_VERSION | sed 's,^[[0-9]]*\.,,;s,\.[[0-9]]*$,,'`

    # see if it is already specified in LIBS
    TEST_LIBNAME=''
    AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "db"
      TEST_LIBNAME='-ldb'
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "db-X.Y"
      TEST_LIBNAME="-ldb-${MAJOR}.$MINOR"
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "dbX.Y"
      TEST_LIBNAME="-ldb${MAJOR}.$MINOR"
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "dbXY"
      TEST_LIBNAME="-ldb$MAJOR$MINOR"
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "db-X"
      TEST_LIBNAME="-ldb-$MAJOR"
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    if test "$ax_path_bdb_env_confirm_lib_ok" = "no" ; then
      # try format "dbX"
      TEST_LIBNAME="-ldb$MAJOR"
      AX_PATH_BDB_ENV_CONFIRM_LIB([$HEADER_VERSION], [$TEST_LIBNAME])
    fi

    dnl # Found a valid library.
    if test "$ax_path_bdb_env_confirm_lib_ok" = "yes" ; then
      if test "x$TEST_LIBNAME" = "x" ; then
        AC_MSG_RESULT([none required])
      else
        AC_MSG_RESULT([$TEST_LIBNAME])
      fi
      ax_path_bdb_env_get_version_VERSION="$HEADER_VERSION"
      ax_path_bdb_env_get_version_LIBS="$TEST_LIBNAME"
      ax_path_bdb_env_get_version_ok=yes
    else
      AC_MSG_RESULT([no])
    fi

    AS_VAR_POPDEF([MAJOR])dnl
    AS_VAR_POPDEF([MINOR])dnl
  fi

  AS_VAR_POPDEF([HEADER_VERSION])dnl
  AS_VAR_POPDEF([TEST_LIBNAME])dnl

  dnl # Execute ACTION-IF-FOUND / ACTION-IF-NOT-FOUND.
  if test "$ax_path_bdb_env_confirm_lib_ok" = "yes" ; then
    m4_ifvaln([$1],[$1],[:])dnl
    m4_ifvaln([$2],[else $2])dnl
  fi

]) dnl BDB_ENV_GET_VERSION

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

