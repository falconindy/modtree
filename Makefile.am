EXTRA_DIST =
CLEANFILES =
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

AM_CFLAGS = $(WARNINGFLAGS)
AM_LDFLAGS = $(GCLDFLAGS)

AM_CPPFLAGS = \
	-imacros $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBEXECDIR=\""$(libexecdir)"\" \
	-DROOTPREFIX=\""$(rootprefix)"\" \
	-I${top_srcdir}/src

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

src_modtree_SOURCES = \
	src/c.h \
	src/langinfo.c \
	src/list.h \
	src/mbsalign.c src/mbsalign.h \
	src/modtree.c \
	src/nls.h \
	src/tt.c src/tt.h \
	src/widechar.h

src_modtree_CFLAGS = \
	$(AM_CFLAGS) \
	$(KMOD_CFLAGS)

src_modtree_LDFLAGS = \
	$(AM_LIBS) \
	$(KMOD_LIBS)

dist_man_MANS = \
	doc/modtree.1

bin_PROGRAMS = src/modtree
