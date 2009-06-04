NULL =

BUILT_SOURCES = catalina-version.h

sources_public_h =						\
	$(top_srcdir)/catalina/catalina-storage.h		\
	$(top_srcdir)/catalina/catalina-formatter.h		\
	$(top_srcdir)/catalina/catalina-binary-formatter.h	\
	$(top_srcdir)/catalina/catalina-transform.h		\
	$(top_srcdir)/catalina/catalina-zlib-transform.h	\
	$(NULL)

sources_private_h =						\
	$(NULL)

sources_c = 							\
	catalina-storage.c					\
	catalina-formatter.c					\
	catalina-binary-formatter.c				\
	catalina-transform.c					\
	catalina-zlib-transform.c				\
	$(NULL)

catalinaincludedir = $(includedir)/catalina-$(CATALINA_API_VERSION)/catalina
catalinainclude_DATA =						\
	$(top_srcdir)/catalina/catalina.h			\
	$(top_srcdir)/catalina/catalina-version.h		\
	$(sources_public_h) 					\
	$(NULL)

lib_LTLIBRARIES = libcatalina-@CATALINA_API_VERSION@.la

libcatalina_@CATALINA_API_VERSION@_la_SOURCES = 		\
	$(sources_public_h) 					\
	$(sources_private_h) 					\
	$(sources_c) 						\
	$(BUILT_SOURCES) 					\
	$(NULL)

libcatalina_@CATALINA_API_VERSION@_la_CFLAGS =			\
	-I$(top_srcdir)						\
	-DPREFIX=\""$(prefix)"\"				\
	-DLIBDIR=\""$(libdir)"\"				\
	-DG_LOG_DOMAIN=\"Catalina\"				\
	-DG_DISABLE_DEPRECATED					\
	$(CATALINA_MAINTAINER_CFLAGS)				\
	$(CATALINA_DEBUG_CFLAGS)				\
	$(CATALINA_CFLAGS)					\
	$(NULL)

libcatalina_@CATALINA_API_VERSION@_la_LDFLAGS =		\
	$(CATALINA_GLIB_LT_LDFLAGS) 			\
	-export-dynamic 				\
	-export-symbols-regex "^catalina.*" 		\
	-rpath $(libdir) 				\
	$(NULL)

libcatalina_@CATALINA_API_VERSION@_la_LIBADD = $(CATALINA_LIBS)

DISTCLEANFILES = catalina-version.h
EXTRA_DIST = catalina-version.h.in

if ENABLE_INTROSPECTION
Catalina-1.0.gir: $(sources_c)
Catalina-1.0.gir: $(sources_public_h)
Catalina-1.0.gir: $(G_IR_SCANNER)
	$(G_IR_SCANNER) -v --namespace Catalina         \
		--nsversion=1.0                         \
		--add-include-path=$(srcdir)            \
		--add-include-path=.                    \
		--include=Gio-2.0                       \
		--library=catalina-1.0                  \
		--output $@                             \
		--pkg gobject-2.0                       \
		-I$(top_srcdir)                         \
		$(sources_c)                            \
		$(sources_public_h)

girdir = $(GIRDIR)
gir_DATA = Catalina-1.0.gir

EXTRA_DIST += Catalina-1.0.gir

typelibsdir = $(TYPELIBDIR)
typelibs_DATA = Catalina-1.0.typelib

Catalina-1.0.typelib: Catalina-1.0.gir
	$(G_IR_COMPILER)                                \
	--includedir=$(srcdir)                          \
	--includedir=.                                  \
	--shared-library=catalina-1.0                   \
	$(G_IR_COMPILER_OPTS)                           \
	$< -o $(top_builddir)/catalina/$(@F)
endif # ENABLE_INTROSPECTION

