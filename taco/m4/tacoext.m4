dnl Extensions for the convenient access to TACO
dnl Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

AC_DEFUN([TACO_EXTENSIONS],[
	AC_REQUIRE([TACO_INIT])
	taco_save_cppflags="$CPPFLAGS"
	taco_save_libs="$LIBS"
	taco_save_ldflags="$LDFLAGS"
	CPPFLAGS="$CPPFLAGS $TACO_CPPFLAGS"
	LDFLAGS="$LDFLAGS $TACO_LDFLAGS"

	# Check TACO extensions header files
	AC_MSG_CHECKING([for TACO extensions header files])
	AC_LANG_SAVE
	AC_LANG_CPLUSPLUS
	AC_PREPROC_IFELSE(AC_LANG_PROGRAM([[#include <TACOExtensions.h>]], []), [taco_try=ok],[taco_try=failed])
	AC_MSG_RESULT($taco_try)
	if test $taco_try = failed ; then
		AC_MSG_ERROR([it seems that the TACO extension header files are not installed])
	fi

	# Check TACO extensions library
	LIB_TACO="$LIB_TACO -lTACOExtensions"
	LIBS="$taco_save_libs $LIB_TACO -ltacomain -ltaco++"
	AC_MSG_CHECKING([for TACO extensions library])
	AC_LINK_IFELSE(AC_LANG_PROGRAM(
	[[#include <TACOExtensions.h>]],[[TACO::errorString( 0)]]), [taco_try=ok], [taco_try=failed])
	if test $taco_try = failed ; then
		LIBS="$taco_save_libs $LIB_TACO -ltacoapig++"
		AC_LINK_IFELSE(AC_LANG_PROGRAM(
		[[#include <TACOExtensions.h>]],[[TACO::errorString( 0)]]), 
		[taco_try=ok], [AC_MSG_ERROR([it seems that the TACO extension library is not installed])])
	fi
	AC_MSG_RESULT($taco_try)
	AC_LANG_RESTORE
	LDFLAGS="$taco_save_ldflags"
	CPPFLAGS="$taco_save_cppflags"
	LIBS="$taco_save_libs"
])

AC_DEFUN([LINUX_DISTRIBUTION],
[
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
		case "$target" in
			i[[3456]]86-*-linux-* | i[[3456]]86-*-linux)
				if test -f /etc/lsb-release ; then
					DISTRIBUTION=`(. /etc/lsb-release; echo $DISTRIB_DESCRIPTION)`
				else
					DISTRIBUTION=`cat /etc/*-release | head -n 1`
				fi
				;;
		esac
        AC_SUBST([DISTRIBUTION])
])
