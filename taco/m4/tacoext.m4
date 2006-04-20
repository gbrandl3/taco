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
	LIBS="$taco_save_libs $LIB_TACO -ltacoapig++"
	AC_MSG_CHECKING([for TACO extensions library])
	AC_LINK_IFELSE(AC_LANG_PROGRAM(
	[[#include <TACOExtensions.h>]],[[TACO::errorString( 0)]]), [taco_try=ok], [taco_try=failed])
	AC_MSG_RESULT($taco_try)
	if test $taco_try = failed ; then
		AC_MSG_ERROR([it seems that the TACO extension library is not installed])
	fi
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
