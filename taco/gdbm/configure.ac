dnl Process this file with autoconf to produce a configure script.
AC_INIT(gdbm, 1.8.3a, [gdbm-bug@gnu.org])
AC_PREREQ(2.53)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(gdbmdefs.h)
AC_CONFIG_HEADERS(autoconf.h)

AM_INIT_AUTOMAKE(1.7)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_ARG_ENABLE(compat, AC_HELP_STRING([--disable-compat], [disable compatibility mode to ndbm and dbm @<:@default=enabled@:>@]),
	[], [enable_compat=yes])
AM_CONDITIONAL(COMPAT, test "$enable_compat" = "yes")

AC_C_CONST
AC_CHECK_HEADERS([stdlib.h string.h sys/file.h unistd.h fcntl.h sys/types.h memory.y])
AC_CHECK_LIB(dbm, main)
AC_CHECK_LIB(ndbm, main)
AC_CHECK_FUNCS([rename ftruncate flock bcopy fsync])
AC_REPLACE_FUNCS(getopt)
AC_TYPE_OFF_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
