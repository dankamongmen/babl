## Source directory

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

SUBDIRS = base

c_sources =				\
	babl.c				\
	babl-component.c		\
	babl-conversion.c		\
	babl-core.c			\
	babl-db.c			\
	babl-extension.c		\
	babl-fish-path.c		\
	babl-fish-reference.c		\
	babl-fish-simple.c		\
	babl-fish-stats.c		\
	babl-fish.c			\
	babl-format.c			\
	babl-hash-table.c		\
	babl-image.c			\
	babl-internal.c			\
	babl-introspect.c		\
	babl-list.c			\
	babl-memory.c			\
	babl-model.c			\
	babl-mutex.c			\
	babl-palette.c    \
	babl-sampling.c			\
	babl-sanity.c			\
	babl-type.c			\
	babl-util.c			\
	babl-cpuaccel.c			\
	babl-version.c

h_sources  =				\
	babl.h				\
	babl-class.h			\
	babl-classes.h			\
	babl-component.h		\
	babl-conversion.h		\
	babl-cpuaccel.h			\
	babl-db.h			\
	babl-extension.h		\
	babl-fish.h			\
	babl-format.h			\
	babl-hash-table.h		\
	babl-ids.h			\
	babl-image.h			\
	babl-internal.h			\
	babl-introspect.h		\
	babl-list.h			\
	babl-macros.h			\
	babl-memory.h			\
	babl-model.h			\
	babl-mutex.h			\
	babl-sampling.h			\
	babl-type.h			\
	babl-types.h			\
	babl-util.h

library_includedir=$(includedir)/babl-$(BABL_API_VERSION)/babl
libinc_hdrs = \
	babl-macros.h           \
	babl-types.h            \
	babl.h
libinc_generated_hdrs = \
	babl-version.h
library_include_HEADERS = \
	$(libinc_hdrs) $(libinc_generated_hdrs)

AM_CPPFLAGS = \
	-DLIBDIR=\""$(libdir)"\"	\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/babl/base

lib_LTLIBRARIES= libbabl-@BABL_API_VERSION@.la
libbabl_@BABL_API_VERSION@_la_SOURCES= $(h_sources) $(c_sources)
libbabl_@BABL_API_VERSION@_la_LIBADD=\
	base/libbase.la \
	@LTLIBOBJS@

libbabl_@BABL_API_VERSION@_la_LDFLAGS= 		\
	${no_undefined} $(MATH_LIB)	\
	-version-info $(BABL_LIBRARY_VERSION)

# GObject Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

if HAVE_GI_RENAME_TO
Babl-$(BABL_API_VERSION).gir: $(G_IR_SCANNER) $(library_include_HEADERS) $(c_sources) $(srcdir)/Makefile.am libbabl-@BABL_API_VERSION@.la
	$(INTROSPECTION_SCANNER) -v --warn-all \
	    --namespace Babl --nsversion=$(BABL_API_VERSION) \
            --add-include-path=$(srcdir) --add-include-path=. \
            --library=$(builddir)/libbabl-$(BABL_API_VERSION).la \
            --libtool="$(LIBTOOL)" \
            --output $@ \
            -DBABL_IS_BEING_COMPILED \
	    -I$(top_srcdir) \
	    -I$(top_builddir) \
	    $(addprefix $(srcdir)/, $(libinc_hdrs)) \
	    $(addprefix $(builddir)/, $(libinc_generated_hdrs)) \
	    $(addprefix $(srcdir)/, $(c_sources))

else # HAVE_GI_RENAME_TO
# If the "rename to" gobject introspection annotation is not respected (GI <= 1.33.4)
# we have to use a pre-generated .gir file. See https://bugzilla.gnome.org/show_bug.cgi?id=673422
# and https://bugzilla.gnome.org/show_bug.cgi?id=675985
Babl-$(BABL_API_VERSION).gir:
	cp $@.static $@

endif # HAVE_GI_RENAME_TO

girdir = $(datadir)/gir-1.0
gir_DATA = Babl-$(BABL_API_VERSION).gir

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = Babl-$(BABL_API_VERSION).typelib

%.typelib: %.gir
	LD_LIBRARY_PATH=$${LD_LIBRARY_PATH:+$$LD_LIBRARY_PATH:}. $(INTROSPECTION_COMPILER) \
        --includedir=$(srcdir) \
        --includedir=$(top_builddir)/babl \
        $(G_IR_COMPILER_OPTS) \
        $< -o $@

if HAVE_VALA
babl-$(BABL_API_VERSION).vapi: Babl-$(BABL_API_VERSION).gir
	$(VAPIGEN) --library=babl-$(BABL_API_VERSION) Babl-$(BABL_API_VERSION).gir

vapidir=$(datadir)/vala/vapi
vapi_DATA = babl-$(BABL_API_VERSION).vapi

endif # HAVE_VALA

endif # HAVE_INTROSPECTION

EXTRA_DIST = $(gir_DATA)
CLEANFILES = $(gir_DATA) $(typelibs_DATA) $(vapi_DATA)
