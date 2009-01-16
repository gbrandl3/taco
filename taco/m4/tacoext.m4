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
	AS_IF([test $taco_try = failed], 
		AC_MSG_ERROR([it seems that the TACO extension header files are not installed]),
		CXXFLAGS="$CXXFLAGS -DTACO_EXT")

	# Check TACO extensions library
	LIB_TACO="$LIB_TACO -lTACOExtensions"
	LIBS="$taco_save_libs $LIB_TACO -ltacomain -ltaco++"
	AC_MSG_CHECKING([for TACO extensions library])
	AC_LINK_IFELSE(AC_LANG_PROGRAM(
	[[#include <TACOExtensions.h>]],[[TACO::errorString( 0)]]), [taco_try=ok], [taco_try=failed])
	AS_IF([test $taco_try = failed], [ 
		LIBS="$taco_save_libs $LIB_TACO -ltacoapig++"
		AC_LINK_IFELSE(AC_LANG_PROGRAM(
		[[#include <TACOExtensions.h>]],[[TACO::errorString( 0)]]), 
		[taco_try=ok], [AC_MSG_ERROR([it seems that the TACO extension library is not installed])])
	])
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
                i[[3456]]86-*-linux-* | i[[3456]]86-*-linux | x86_64-*-linux | x86_64-*-linux-*)
                   AS_IF([test -f /etc/lsb-release], [DISTRIBUTION=`(. /etc/lsb-release; echo $DISTRIB_DESCRIPTION)`])
		   AS_IF([test -z "$DISTRIBUTION"], 
			[LSB_FILE=`ls /etc/*-release | grep -v lsb`
                         DISTRIBUTION=`cat $LSB_FILE | head -1`
			])
                        ;;
        esac
        AC_SUBST([DISTRIBUTION])
])

