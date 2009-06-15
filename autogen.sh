#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PROJECT=Catalina
TEST_TYPE=-d
FILE=catalina

test ${TEST_TYPE} ${FILE} || {
        echo "You must run this script in the top-level ${PROJECT} directory"
        exit 1
}

GTKDOCIZE=`which gtkdocize`
if test -z $GTKDOCIZE; then
        echo "*** No gtk-doc support ***"
        echo "EXTRA_DIST =" > gtk-doc.make
else
        gtkdocize || exit $?
fi

autoreconf -v --install || exit $?

./configure --enable-maintainer-mode "$@" && echo "Now type 'make' to compile ${PROJECT}"
