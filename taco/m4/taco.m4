dnl
dnl TACO_BASE_PATH
dnl 
AC_DEFUN([TACO_BASE_PATH],
    [
dnl    	AC_MSG_CHECKING([for TACO base])
    	AC_REQUIRE([TACO_CHECK_TACO_SYSTEM]) 
 
    	if test "${prefix}" != NONE; then
	    ac_taco_path=${prefix}
            taco_includes=${prefix}/include
            ac_taco_includes=$prefix/include
            if test "${exec_prefix}" != NONE; then
            	taco_libraries=${exec_prefix}/lib
            	ac_taco_libraries=$exec_prefix/lib
            else
            	taco_libraries=${prefix}/lib
            	ac_taco_libraries=$prefix/lib
            fi
    	else
	    ac_taco_path=""
            ac_taco_includes=""
            ac_taco_libraries=""
            taco_libraries=""
            taco_includes=""
    	fi

    	AC_CACHE_VAL(ac_cv_have_taco,
    	    [#try to guess taco locations
    	    if test -z "$1"; then
        	taco_incdirs="$ac_taco_includes"
        	test -n "$DSHOME" && taco_incdirs="$taco_incdirs $DSHOME/include "
		taco_incdirs="$taco_incdirs /usr/lib/taco/include /usr/local/taco/include \
                /usr/taco/include /usr/include/taco /usr/include /opt/taco/include"
        	AC_FIND_FILE(API.h, $taco_incdirs, taco_incdir)
        	ac_taco_includes="$taco_incdir"
 
        	if test -n "$ac_taco_includes" -a ! -r "$ac_taco_includes/API.h"; then
            	    AC_MSG_ERROR([
in the prefix, you've chosen, are no taco headers installed. This will fail.
So, check this please and use another prefix!])
        	fi
 
        	taco_libdirs="$ac_taco_libraries"
		test -n "$DSHOME" && taco_libdirs="$taco_libdirs $DSHOME/lib/"
        	taco_libdirs="$taco_libdirs /usr/lib/taco/lib \
                /usr/local/taco/lib /usr/taco/lib /usr/lib/taco /usr/lib /opt/taco/lib "
        	AC_FIND_FILE(libdsapi.so, $taco_libdirs, taco_libdir)
 
		if test -n "$taco_libdir" && test ! -r "$taco_libdir/libdsapi.so"; then
            	    AC_MSG_ERROR([
in the prefix, you've chosen, are no taco libraries installed. This will fail.
So, check this please and use another prefix!])
        	fi
        	ac_taco_libraries="$taco_libdir"
 
        	if test "$ac_taco_includes" = NO || test "$ac_taco_libraries" = NO; then
            	    ac_cv_have_taco="have_taco=no"
        	else
            	    ac_cv_have_taco="have_taco=yes \
                	ac_taco_includes=$ac_taco_includes ac_taco_libraries=$ac_taco_libraries"
        	fi
    	    else                                dnl test -z $1
        	ac_cv_have_taco="have_taco=no"
    	    fi
    	])dnl
 
    	eval "$ac_cv_have_taco"
    	if test "$have_taco" != "yes"; then
		echo yes
            if test "${prefix}" = NONE; then
            	ac_taco_prefix="$ac_default_prefix"
            else
            	ac_taco_prefix="$prefix"
            fi
            if test "$exec_prefix" = NONE; then
            	ac_taco_exec_prefix="$ac_taco_prefix"
            	AC_MSG_RESULT([will be installed in $ac_taco_prefix])
            else
            	ac_taco_exec_prefix="$exec_prefix"
            	AC_MSG_RESULT([will be installed in $ac_taco_prefix and $ac_taco_exec_prefix])
            fi
 
            taco_libraries="${ac_taco_exec_prefix}/lib/"
            taco_includes="${ac_taco_prefix}/include" 
   	else
            ac_cv_have_taco="have_taco=yes \
            ac_taco_includes=$ac_taco_includes ac_taco_libraries=$ac_taco_libraries"
            AC_MSG_RESULT([libraries $ac_taco_libraries, headers $ac_taco_includes])
            taco_libraries="$ac_taco_libraries"
            taco_includes="${ac_taco_includes}"
	fi

    	TACO_CHECK_EXTRA_LIBS

    	AC_SUBST(taco_libraries)
    	AC_SUBST(taco_includes)
 
        TACO_INCLUDES="-I${taco_includes}  -I${taco_includes}++"
        all_includes="$TACO_INCLUDES $all_includes"
 
        TACO_LDFLAGS="-L$taco_libraries"
        all_libraries="$TACO_LDFLAGS $all_libraries"
 
    	AC_SUBST(TACO_LDFLAGS)
    	AC_SUBST(TACO_INCLUDES)

	LIB_TACO="-ldbapi -ldsxdr -lm" 
	AC_SUBST(LIB_TACO)
 
    	AC_SUBST(all_includes)
      	AC_SUBST(all_libraries)
])

dnl
dnl	TACO_DEFINES
dnl
dnl    	uses $target, $target_alias, $target_cpu, $target_vendor, $target_os
dnl
dnl	produces $taco_CFLAGS
dnl
AC_DEFUN([TACO_DEFINES],
[
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	case "$target" in
		i[[3456]]86-*-linux-* | i[[3456]]86-*-linux | i[[3456]]86-*-cygwin*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -Dx86=1 -DNDBM" ;;
		i386-*-freebsd* )
			taco_CFLAGS="-Dunix=1 -D__unix=1 -DFreeBSD -Dx86=1 -DNDBM" ;;
		m68k-*-linux-*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -Dm68k=1 -DNDBM" ;;
		powerpc-apple-darwin*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -DFreeBSD -DDARWIN -DNDBM" ;;
		*-*-solaris* | *-*-sun*-*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -D_solaris=1 -D__solaris__=1 -DNDBM" ;;
		*-*-hp*-*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -D__hpux=1 -D__hpux10=1 -D__hp9000s700=1 -D__hpux9000s700=1 -DNDBM" ;;
		*-*-OS?-*)
			taco_CFLAGS="-D_UCC=1" ;;
		*)	AC_MSG_ERROR([unknown target : $target])
			taco_CFLAGS="unknown $target" ;;
	esac
	AC_ARG_ENABLE(multithreading,
		AC_HELP_STRING([--enable-multithreading], [allow multithreading default=yes]),
		[
			if test x$enableval = xno ; then
				extraflags=""
			else
				extraflags="-D_REENTRANT"
			fi
		],
		[ extraflags="-D_REENTRANT"])
	taco_CFLAGS="$taco_CFLAGS $extraflags"
	CFLAGS="$CFLAGS $taco_CFLAGS"
	CXXFLAGS="$CXXFLAGS $taco_CFLAGS"
	AC_SUBST(taco_CFLAGS)
	AC_CHECK_HEADERS([fcntl.h malloc.h memory.h netdb.h stddef.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h termios.h unistd.h sys/filio.h paths.h])
	AC_CHECK_HEADERS([arpa/inet.h netinet/in.h sys/socket.h]) 

	AC_LANG_PUSH(C++)
	AC_CHECK_HEADERS([sstream strstream])
	AC_LANG_POP(C++)

# Checks for typedefs, structures, and compiler characteristics.
	AC_HEADER_STDBOOL
	AC_C_CONST
	AC_C_INLINE
	AC_TYPE_MODE_T
	AC_TYPE_OFF_T
	AC_TYPE_PID_T
	AC_TYPE_SIZE_T
	AC_HEADER_TIME
	AC_STRUCT_TM
	AC_C_VOLATILE
	AC_CHECK_TYPES(socklen_t, [],[],
	[#if HAVE_SYS_SOCKET_H
#       include <sys/socket.h>
#elif HAVE_SOCKET_H
#       include <socket.h>
#elif HAVE_NETDB_H
#       include <netdb.h>
#elif HAVE_ARPA_INET_H
#       include <arpa/inet.h>
#endif])
	AC_CHECK_TYPES(time_t)
])

dnl
dnl	TACO_CHECK_TACO_SYSTEM
dnl
dnl    	uses $target, $target_alias, $target_cpu, $target_vendor, $target_os
dnl
dnl	produces $taco_config 
dnl
AC_DEFUN([TACO_CHECK_TACO_SYSTEM], 
[
	AC_REQUIRE([TACO_DEFINES])
    	AC_SUBST([taco_config], ["$taco_CFLAGS"])
])

dnl
dnl	TACO_CHECK_EXTRA_LIBS
dnl
dnl
AC_DEFUN([TACO_CHECK_EXTRA_LIBS],
    [
    	AC_MSG_CHECKING(for extra includes)
    	AC_ARG_WITH(extra-includes, [  --with-extra-includes=DIR
                          adds non standard include paths],
        	taco_use_extra_includes="$withval",
        	taco_use_extra_includes=NONE)
 
    	if test -n "$taco_use_extra_includes" && \
           test "$taco_use_extra_includes" != "NONE"; then
            ac_save_ifs=$IFS
            IFS=':'
            for dir in $taco_use_extra_includes; do
            	all_includes="$all_includes -I$dir"
            	USER_INCLUDES="$USER_INCLUDES -I$dir"
            done
	    AC_SUBST(USER_INCLUDES)
            IFS=$ac_save_ifs
            taco_use_extra_includes="added"
        else
            taco_use_extra_includes="no"
        fi
 
    	AC_MSG_RESULT($taco_use_extra_includes)
 
    	AC_MSG_CHECKING(for extra libs)
    	AC_ARG_WITH(extra-libs, [  --with-extra-libs=DIR   adds non standard library paths],
        	taco_use_extra_libs=$withval,
        	taco_use_extra_libs=NONE)
    	if test -n "$taco_use_extra_libs" && \
            test "$taco_use_extra_libs" != "NONE"; then
            ac_save_ifs=$IFS
            IFS=':'
            for dir in $taco_use_extra_libs; do
            	all_libraries="$all_libraries -L$dir"
            	TACO_EXTRA_RPATH="$TACO_EXTRA_RPATH -rpath $dir"
            	USER_LDFLAGS="$USER_LDFLAGS -L$dir"
            done
	    AC_SUBST(USER_LDFLAGS)
            IFS=$ac_save_ifs
            taco_use_extra_libs="added"
    	else
            taco_use_extra_libs="no"
    	fi
 
    	AC_MSG_RESULT($taco_use_extra_libs)
    ]
)

dnl
dnl	TACO_CHECK_TACO
dnl
AC_DEFUN([TACO_CHECK_TACO],
    [
    	TACO_CHECK_TACO_SYSTEM
        AC_MSG_CHECKING([for TACO])
	if [[ -z "$1" -o x"$1" = xcheck ]] ; then 
	    TACO_BASE_PATH
            AC_MSG_RESULT([found $taco_path libraries $taco_libraries])

	    ac_taco_save_LDFLAGS="$LDFLAGS"
	    AC_LANG_SAVE
	    AC_LANG_CPLUSPLUS
	    LDFLAGS="$LDFLAGS  -L${taco_libraries}"
	    AC_CHECK_LIB(dsapig++, startup,,, [-ldsapi -ldsxdr]) 
	    AC_CHECK_LIB(tacoapig++, startup,,, [-ldsapi -ldsxdr])
	    AC_LANG_RESTORE
	    LDFLAGS="$ac_taco_save_LDFLAGS"
	fi
    ]
)

dnl
dnl TACO_INIT([check])
dnl the parameter check indicates the check of a TACO installation
dnl blank or "check" indicate the check all other no check
dnl
AC_DEFUN([TACO_INIT],
    [
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AM_INIT_AUTOMAKE([1.7])
	
dnl	don't use static libraries
dnl
	AC_REQUIRE([AC_DISABLE_STATIC])
	AC_DISABLE_FAST_INSTALL
	
	AC_PROG_LIBTOOL

	AC_PREFIX_DEFAULT(${DSHOME:-/usr/local/dshome})
dnl
dnl	essential for automatic make in development
dnl

	AM_CONFIG_HEADER([config.h])

	AC_PROG_CXX
	AC_PROG_INSTALL
	AC_PROG_LN_S

	TACO_CHECK_TACO([$1])

	AM_MAINTAINER_MODE
    ]
)

AC_DEFUN([TACO_SERVER],
[       AC_REQUIRE([TACO_CHECK_TACO])
	AC_FIND_HEADER([API.h],[$TACO_INCLUDES])
        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS                                                                                                              
	AC_FIND_HEADER([Device.H],[$TACO_INCLUDES])
	AC_FIND_HEADER([ApiP.h],[$TACO_INCLUDES -I$taco_includes/private])
        AC_FIND_LIB([dsapig++],[$TACO_LDFLAGS],[#include "API.h"],,LIB_TACO)
	AC_LANG_RESTORE
	AC_CACHE_SAVE
])

AC_DEFUN([TACO_CLIENT],
[       AC_REQUIRE([TACO_CHECK_TACO])      
        AC_FIND_HEADER([API.h],[$TACO_INCLUDES])
        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS                                                                                                              
	AC_FIND_LIB([tacoapig++],[$TACO_LDFLAGS],[#include "API.h"],, LIB_TACO)
	AC_LANG_RESTORE
	AC_CACHE_SAVE
])

