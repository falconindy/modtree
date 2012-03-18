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

#AC_ARG_ENABLE([logging],
#        AS_HELP_STRING([--disable-nls], [disable system logging @<:@default=enabled@:>@]),
#        [], enable_logging=yes)
#AS_IF([test "x$enable_logging" = "xyes"], [
#        AC_DEFINE(ENABLE_LOGGING, [1], [System logging.])
#])

#AC_ARG_ENABLE([debug],
#        AS_HELP_STRING([--enable-debug], [enable debug messages @<:@default=disabled@:>@]),
#        [], [enable_debug=no])
#AS_IF([test "x$enable_debug" = "xyes"], [
#        AC_DEFINE(ENABLE_DEBUG, [1], [Debug messages.])
#])

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
	sysconfdir:             ${sysconfdir}
	libdir:                 ${libdir}
	includedir:             ${includedir}

	compiler:               ${CC}
	cflags:                 ${CFLAGS}
	ldflags:                ${LDFLAGS}

	logging:                ${enable_logging}
	debug:                  ${enable_debug}
])
