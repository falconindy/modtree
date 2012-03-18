AC_PREREQ(2.60)
AC_INIT(modtree,
	[1],
	[dreisner@archlinux.org])

AC_CONFIG_SRCDIR([src/modtree.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([check-news foreign 1.11 -Wall -Wno-portability silent-rules tar-pax no-dist-gzip dist-xz subdir-objects])
AC_PROG_CC_STDC
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
AC_PREFIX_DEFAULT([/usr])

AC_PROG_SED
AC_PROG_MKDIR_P

AC_CHECK_HEADERS([ \
	err.h \
	langinfo.h \
	sys/ioctl.h
])

PKG_CHECK_MODULES(KMOD, [ libkmod >= 5 ])

AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <wchar.h>
#include <wctype.h>
#include <stdio.h>
]], [[
  wchar_t wc;
  wint_t w;
  w = fgetwc(stdin);
  if (w == WEOF) exit(1);
  wc = w;
  fputwc(wc,stdout);
]])],
[AC_DEFINE(HAVE_WIDECHAR, 1, Do we have wide character support?)
])

AC_ARG_WITH([rootprefix],
        AS_HELP_STRING([--with-rootprefix=DIR], [rootfs directory prefix for config files and kernel modules]),
        [], [with_rootprefix=""])
AC_SUBST([rootprefix], [$with_rootprefix])

my_CFLAGS="-Wall \
-Wmissing-declarations -Wmissing-prototypes \
-Wnested-externs -Wpointer-arith \
-Wpointer-arith -Wsign-compare -Wchar-subscripts \
-Wstrict-prototypes -Wshadow \
-Wformat-security -Wtype-limits"
AC_SUBST([my_CFLAGS])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
	Makefile
])

AC_OUTPUT
AC_MSG_RESULT([
	$PACKAGE $VERSION
	=====

	prefix:                 ${prefix}
	rootprefix              ${rootprefix}
	sysconfdir:             ${sysconfdir}
	libdir:                 ${libdir}
	includedir:             ${includedir}

	compiler:               ${CC}
	cflags:                 ${CFLAGS}
	ldflags:                ${LDFLAGS}

	logging:                ${enable_logging}
	debug:                  ${enable_debug}
])
