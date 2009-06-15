include $(top_srcdir)/Makefile.decl
NULL = 
	
noinst_PROGRAMS =					\
	storage-tests					\
	zlib-transform-tests				\
	binary-formatter-tests				\
	$(NULL)

TEST_PROGS +=						\
	storage-tests					\
	zlib-transform-tests				\
	binary-formatter-tests				\
	$(NULL)

INCLUDES =						\
	-I$(top_srcdir)					\
	$(NULL)

LDADD = 						\
	$(top_builddir)/catalina/libcatalina-1.0.la	\
	$(NULL)

AM_CFLAGS = $(CATALINA_CFLAGS)
AM_LDFLAGS = $(CATALINA_LIBS)

storage_tests_sources = storage-tests.c
zlib_transform_tests_sources = zlib-transform-tests.c
binary_foramtter_tests_sources = binary-formatter-tests.c

EXTRA_DIST =						\
	$(srcdir)/mocks/long-test-data.h		\
	$(srcdir)/mocks/mock-person.h			\
	$(srcdir)/async-test.h				\
	$(NULL)

CLEANFILES = log.0000000001 storage-tests.db
