# 
# TACO_INIT
# ---------------
# 
AC_DEFUN([TACO_INIT],
[
        AC_REQUIRE([AC_CANONICAL_SYSTEM])dnl
	AC_REQUIRE([AC_PROG_CPP])dnl
	AC_REQUIRE([AC_PROG_CC])dnl
        AC_REQUIRE([AC_PROG_CXX])dnl
	AC_REQUIRE([AC_PROG_CXX_C_O])dnl

	AC_REQUIRE([AM_PROG_CC_C_O])dnl
	AC_REQUIRE([AC_PROG_CC_STDC])dnl
	AC_REQUIRE([AC_PROG_CC_C99])dnl

# Check for Solaris CC compiler (added by Andy)
	AC_MSG_CHECKING([for Solaris CC compiler])
	AM_CONDITIONAL(SOLARIS_CXX, [$CXX -V 2>&1 |grep Sun])
	AC_MSG_RESULT([$SOLARIS_CXX])
	AC_SUBST([SOLARIS_CXX], [$SOLARIS_CXX])
	AS_IF([test x"$SOLARIS_CXX" != x], AC_SUBST([LIB_STDCXX], [-lCstd]), AC_SUBST([LIB_STDCXX], [-lstdc++]))

dnl
dnl     don't use static libraries
dnl
        AC_REQUIRE([AC_DISABLE_STATIC])
        AC_REQUIRE([AC_ENABLE_FAST_INSTALL])
        AC_REQUIRE([AM_PROG_LIBTOOL])dnl
dnl
dnl     essential for automatic make in development
dnl
        AM_CONFIG_HEADER([config.h])
	AH_TOP([#ifndef TACO_CONFIG_H
#define TACO_CONFIG_H])

	AH_BOTTOM([#endif /* TACO_CONFIG_H */])

        AM_INIT_AUTOMAKE([dist-bzip2 1.7])

        AC_PREFIX_DEFAULT(${DSHOME:-/usr/local/taco})

        AC_REQUIRE([AC_PROG_INSTALL])dnl
        AC_REQUIRE([AC_PROG_LN_S])dnl
	AC_REQUIRE([AC_PROG_MAKE_SET])dnl
	AC_REQUIRE([AC_PROG_AWK])dnl
	AC_REQUIRE([AC_PROG_YACC])dnl
	AC_REQUIRE([AC_PROG_LEX])dnl
	AC_REQUIRE([AC_PROG_RM])dnl

        AM_MAINTAINER_MODE
])

#
# TACO_DEFINES
#
# uses $target, $target_alias, $target_cpu, $target_vendor, $target_os
#
# produces $taco_CFLAGS
#
AC_DEFUN([TACO_DEFINES],
[
        AC_REQUIRE([TACO_INIT])dnl
        case "$target" in
                i[[3456]]86-*-linux-* | i[[3456]]86-*-linux | i[[3456]]86-*-cygwin* | \
                x86_64-*-linux* | ia64-*-linux-* | arm-*-linux-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -DNDBM" ;;
                ia64-*-freebsd* | i386-*-freebsd* | powerpc-apple-darwin*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -DFreeBSD -DNDBM" ;;
		i[[3456]]86-apple-darwin*)
			taco_CFLAGS="-Dunix=1 -D__unix=1 -DFreeBSD -Dx86=1 -DNDBM -DDARWIN";;
                m68k-*-linux-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -Dm68k=1 -DNDBM" ;;
                *-*-solaris* | *-*-sun*-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -D_solaris=1 -D__solaris__=1 -DNDBM" ;;
                *-*-hp*-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -D__hpux=1 -D__hpux10=1 -D__hp9000s700=1 -D__hpux9000s700=1 -DNDBM" ;;
                *-*-OS?-*)
                        taco_CFLAGS="-D_UCC=1" ;;
                *)      AC_MSG_ERROR([unknown target : $target])
                        taco_CFLAGS="unknown $target" ;;
        esac
        AC_ARG_ENABLE(multithreading, AC_HELP_STRING([--enable-multithreading], [allow multithreading @<:@default=yes@:>@]),
        [
                AS_IF([test x$enableval = xno], [extraflags=""], [extraflags="-D_REENTRANT"])
        ], [extraflags="-D_REENTRANT"])
        taco_CFLAGS="$taco_CFLAGS $extraflags"
        CFLAGS="$CFLAGS $taco_CFLAGS"
        CXXFLAGS="$CXXFLAGS $taco_CFLAGS"
        AC_SUBST(taco_CFLAGS)
])

#
# TACO_CHECK_HEADERS_AND_FUNCTIONS
# --------------------------------
#
AC_DEFUN([TACO_CHECK_HEADERS_AND_FUNCTIONS],
[
	TACO_DEFINES
dnl
dnl Checks for compiler characteristics.
dnl
        AC_C_CONST
        AC_C_INLINE
        AC_C_VOLATILE
dnl
dnl Checks for header files.
dnl
        AC_HEADER_STDC
dnl Done by AC_HEADER_STDC
dnl     AC_CHECK_HEADERS([sys/types.h sys/socket.h sys/param.h])
dnl	AC_CHECK_HEADERS([memory.h unistd.h stdlib.h string.h strings.h])
        AC_HEADER_SYS_WAIT
        AC_HEADER_STAT
        AC_HEADER_STDBOOL

	AC_CHECK_HEADERS([sys/sem.h sys/shm.h sys/resource.h sys/time.h])
        AC_CHECK_HEADERS([sys/file.h sys/ioctl.h sys/filio.h paths.h sys/param.h])
        AC_CHECK_HEADERS([termios.h])
        AC_CHECK_HEADERS([wait.h sys/wait.h])
        AC_CHECK_HEADERS([pthread.h siggen.h])
        AC_CHECK_HEADERS([float.h])
        AC_CHECK_HEADERS([fcntl.h malloc.h netdb.h stddef.h]) 
        AC_CHECK_HEADERS([arpa/inet.h netinet/in.h])

        AC_CHECK_HEADERS([signal.h sys/signal.h])
        AC_TYPE_SIGNAL
        AC_CHECK_DECLS([signal], [], [], [
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#endif
        ])

        AC_CHECK_FUNCS([signal], [], [], [
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#endif
        ])

        AC_CHECK_TYPES([sighandler_t, __sighandler_t], [], [], [
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#endif
        ])

# Checks for typedefs, structures, and compiler characteristics.
        AC_TYPE_MODE_T
        AC_TYPE_OFF_T
        AC_TYPE_PID_T
        AC_TYPE_SIZE_T
        AC_TYPE_UID_T

        AC_HEADER_TIME
        AC_STRUCT_TM
        AC_CHECK_TYPES(time_t)

        AC_CHECK_TYPES(socklen_t, [],[],
        [#if HAVE_SYS_TYPES_H
#       include <sys/types.h>
#endif
#if HAVE_SYS_SOCKET_H
#       include <sys/socket.h>
#elif HAVE_SOCKET_H
#       include <socket.h>
#elif HAVE_NETDB_H
#       include <netdb.h>
#elif HAVE_ARPA_INET_H
#       include <arpa/inet.h>
#endif])

        AC_CHECK_HEADERS([varargs.h stdarg.h])
        AC_MSG_CHECKING([for number of parameters in va_start])
        AH_TEMPLATE([HAVE_VA_START_P1], [define whether the va_start macro expect only one parameter])
        AH_TEMPLATE([HAVE_VA_START_P1_P2], [define whether the va_start macro expect two parameters])
        AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#if HAVE_STDARG_H
#       include <stdarg.h>
#elif HAVE_VARARGS_H
#       include <varargs.h>
#endif

void dev_printerror (int mode, char *fmt, ...)
{
        va_list         args;
        va_start(args, fmt);
}
]], [[dev_printerror(0, "%s", "Hello World\n");]])],
                [
                AC_DEFINE(HAVE_VA_START_P1_P2)
                AC_MSG_RESULT([2])
                ],
                [
                AC_DEFINE(HAVE_VA_START_P1)
                AC_MSG_RESULT([1])
                ]
        )

        AC_MSG_CHECKING([for WAIT_ANY in waitpid])
        AH_TEMPLATE([HAVE_WAIT_ANY], [defines whether the WAIT_ANY macro is defined or not])
        AH_VERBATIM([_HAVE_WAIT_ANY], [
#if !HAVE_WAIT_ANY
#       define WAIT_ANY         (-1)
#endif
        ])
        AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[
#if HAVE_SYS_TYPES_H
#       include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#       include <sys/wait.h>
#endif
                ]],
                [[
int     status;
int     pid = waitpid (WAIT_ANY, &status, WNOHANG);]]
                )],
                [
                AC_MSG_RESULT([yes])
                AC_DEFINE(HAVE_WAIT_ANY)
                ],
                [
                AC_MSG_RESULT([no])
                ]
        )

        AH_VERBATIM([_FIONREAD], [
#if HAVE_SYS_IOCTL_H
#       include <sys/ioctl.h>
#endif
#ifndef FIONREAD
#       define  FIONREAD        TIOCISIZE
#endif
        ])

	AC_CHECK_DECLS([gethostname], [], [],
[
#if HAVE_UNISTD_H
#       include <unistd.h>
#endif 
])

# Checks for library functions.
        AC_FUNC_FORK
        AC_PROG_GCC_TRADITIONAL
        AC_FUNC_MALLOC
        AC_FUNC_MEMCMP
        AC_FUNC_REALLOC
        AC_FUNC_SELECT_ARGTYPES
        AC_FUNC_ALLOCA
        AC_FUNC_STRFTIME
        AC_FUNC_STAT
        AC_FUNC_STRTOD
        AC_FUNC_VPRINTF

        AC_CHECK_FUNCS([alarm dup2 gethostbyname getpass gettimeofday isascii mblen memset putenv])
        AC_CHECK_FUNCS([select socket strcasecmp strchr strcspn strdup strerror strncasecmp strrchr strspn strstr])
        AC_CHECK_FUNCS([strtoul modf])
        AC_CHECK_FUNCS([waitpid wait4 getcwd chown chmod lchmod mknod mkfifo])
        AC_CHECK_FUNCS([fchmod fstat ftruncate readlink link utime utimes lutimes])
        AC_CHECK_FUNCS([memmove lchown vsnprintf snprintf vasprintf asprintf])
        AC_CHECK_FUNCS([setsid glob strpbrk strlcat strlcpy strtol mallinfo getgroups])
        AC_CHECK_FUNCS([setgroups geteuid getegid setlocale setmode open64 lseek64 mkstemp64])
        AC_CHECK_FUNCS([mtrace va_copy __va_copy iconv_open locale_charset])
        AC_CHECK_FUNCS([nl_langinfo sigaction sigprocmask])

        AC_LANG_PUSH(C++)
        AC_CHECK_HEADERS([sstream strstream])
        AC_LANG_POP(C++)
])

#
# TACO_CHECK_SYSTEM
# -----------------
#
AC_DEFUN([TACO_CHECK_SYSTEM],
[
	TACO_CHECK_EXTRA_INCLUDES
	TACO_CHECK_EXTRA_LIBS
        TACO_CHECK_HEADERS_AND_FUNCTIONS
])

#
# TACO_CHECK_EXTRA_INCLUDES
# -------------------------
#
AC_DEFUN([TACO_CHECK_EXTRA_INCLUDES],
[
        AC_MSG_CHECKING(for extra includes)
        AC_ARG_WITH(extra-includes, AC_HELP_STRING([--with-extra-includes=DIR], [adds non standard include paths]),
                [with_extra_includes="$withval"], [with_extra_includes=NONE])

        AS_IF([test -n "$with_extra_includes" -a "$with_extra_includes" != "NONE"], 
              [
                ac_save_ifs=$IFS
                IFS=':'
                for dir in $with_extra_includes; do
                        all_includes="$all_includes -I$dir"
                        USER_INCLUDES="$USER_INCLUDES -I$dir"
                done
                AC_SUBST(USER_INCLUDES)
                IFS=$ac_save_ifs
                with_extra_includes="added"
	      ], [with_extra_includes="no"])

        AC_MSG_RESULT($with_extra_includes)
])

#
# TACO_CHECK_EXTRA_LIBS
# -------------------------
#
AC_DEFUN([TACO_CHECK_EXTRA_LIBS],
[
        AC_MSG_CHECKING(for extra libs)
        AC_ARG_WITH(extra-libs, AC_HELP_STRING([--with-extra-libs=DIR], [adds non standard library paths]),
                [with_extra_libs=$withval], [with_extra_libs=NONE])
        AS_IF([test -n "$with_extra_libs" -a "$with_extra_libs" != "NONE"], 
	      [
                ac_save_ifs=$IFS
                IFS=':'
                for dir in $with_extra_libs; do
                        all_libraries="$all_libraries -L$dir"
                        TACO_EXTRA_RPATH="$TACO_EXTRA_RPATH -rpath $dir"
                        USER_LDFLAGS="$USER_LDFLAGS -L$dir"
                done
                AC_SUBST(USER_LDFLAGS)
                IFS=$ac_save_ifs
                with_extra_libs="added"
	      ], [with_extra_libs="no"])
        AC_MSG_RESULT($with_extra_libs)
])

#
# TACO_CHECK_HOSTNAME
# -------------------
# 
AC_DEFUN([TACO_CHECK_HOSTNAME],
[
	AC_CHECK_PROG(HOSTNAME_CMD, hostname, [hostname], [uname -n])
        hostname=`$HOSTNAME_CMD`;
])

#
# TACO_CHECK_GCC_LIB_PATH
# -----------------------
# 
AC_DEFUN([TACO_CHECK_GCC_LIB_PATH],
[
	AC_REQUIRE([AC_PROG_CC])dnl
	GCC_LIB_PATH=""
	AS_IF([test x"$ac_ct_CC" = x"gcc"],
              [
        	AC_MSG_CHECKING([for the path to the gcc library])
        	for i in `gcc -print-libgcc-file-name | tr '/' ' '` ; do
                	if test x"$i" = x"gcc-lib" ; then
                	        break ;
                	else
                        	GCC_LIB_PATH="${GCC_LIB_PATH}/${i}"
                	fi
        	done
        	AC_MSG_RESULT([$GCC_LIB_PATH])
	      ])
	AC_SUBST([GCC_LIBRARY_PATH], [$GCC_LIB_PATH])
])

#
# TACO_ENABLE_LONGHOSTNAMES
# ------------------------
#
AC_DEFUN([TACO_ENABLE_LONGHOSTNAMES],
[
	AC_ARG_ENABLE(longhostname, 
		AC_HELP_STRING([--disable-longhostname], [disable the use of long hostnames [default=enabled]]),
			[], [enable_longhostnames=yes])
])

#
# TACO_ENABLE_DEBUG
# -----------------
#
AC_DEFUN([TACO_ENABLE_DEBUG],
[
	AC_ARG_ENABLE(debug, 
		AC_HELP_STRING([--enable-debug=ARG], [enables debug symbols (yes|no|full) [default=no]]),
			[], [enable_debug=no])

	AC_REQUIRE([AC_PROG_CC])dnl
	AC_REQUIRE([AC_PROG_CXX])dnl
	case "$ac_ct_CC" in
		gcc)
		case "$enable_debug" in
			yes)
				;;
			full)
                       		CFLAGS=`echo $CFLAGS | sed -e"s/-O2/-O0/g"`
                       		CFLAGS=`echo $CFLAGS | sed -e"s/-g//g"`
                       		CFLAGS="-g3 -fno-inline $CFLAGS"
				;;
			*)
                       		CFLAGS=`echo $CFLAGS | sed -e"s/-g//g"`
				;;
       		esac
		;;
		*)
		;;
	esac
dnl
dnl Not really clear !!!
dnl
	AS_IF([test -z "$LDFLAGS" -a x"$enable_debug" = x"no" -a x"$ac_ct_CC" = x"gcc"], [LDFLAGS=""])
	case "$ac_ct_CXX" in 
		g++)
		case "$enable_debug" in
			yes)
				;;
                       	full)
                                CXXFLAGS=`echo $CXXFLAGS | sed -e"s/-O2/-O0/g"`
                       		CXXFLAGS=`echo $CXXFLAGS | sed -e"s/-g//g"`
                                CXXFLAGS="-g3 -fno-inline $CXXFLAGS"
				;;
			*)
                       		CXXFLAGS=`echo $CXXFLAGS | sed -e"s/-g//g"`
				;;
		esac
		;;
        	KCC)
		case "$enable_debug" in
			no)
                       		CXXFLAGS="+K3 $CXXFLAGS"
				;;
			*)
                       		CXXFLAGS="+K0 -Wall -pedantic -W -Wpointer-arith -Wwrite-strings $CXXFLAGS"
				;;
		esac
		;;
		*)
		;;
	esac
])

AC_DEFUN([TACO_SERVER],
[
        AC_REQUIRE([TACO_CHECK_TACO])

        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS
        ac_save_CXXFLAGS="$CXXFLAGS"
        CXXFLAGS="$CXXFLAGS -I$taco_includes"
        AC_CHECK_HEADERS([Device.h], [], [AC_MSG_ERROR([In the following dirs: \"$taco_includes\", there are no Device.h header installed.
])], [
#include <API.h>
])
        AC_LANG_RESTORE
        AC_CACHE_SAVE
])

AC_DEFUN([TACO_CLIENT],
[
        AC_REQUIRE([TACO_CHECK_TACO])
        AC_CACHE_SAVE
])

#
# TACO_CHECK_TACO
# ---------------
#
AC_DEFUN([TACO_CHECK_TACO],
[
	AC_REQUIRE([TACO_INIT])dnl
	AC_REQUIRE([TACO_DEFINES])dnl
	AC_REQUIRE([TACO_CHECK_EXTRA_INCLUDES])
	AC_REQUIRE([TACO_CHECK_EXTRA_LIBS])
	AC_REQUIRE([TACO_ENABLE_DEBUG])
	AC_REQUIRE([TACO_CHECK_SIZEOF_TYPES])
        AC_REQUIRE([TACO_BASE_PATH])
	AC_REQUIRE([TACO_CHECK_RPC])

	save_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS -I$taco_includes"
	AC_CHECK_DECLS([gethostname], [], [],
[
#if HAVE_UNISTD_H
#       include <unistd.h>
#endif 
])

        AC_CHECK_HEADERS([ApiP.h private/ApiP.h], [], [], [#include <API.h>])
        CPPFLAGS="$save_CPPFLAGS"

        ac_taco_save_LDFLAGS="$LDFLAGS"
        LDFLAGS="$LDFLAGS  -L${taco_libraries}"
        ac_taco_save_LIBS="$LIBS"
        AC_CHECK_LIB([taco], [db_getresource], [LIB_TACO="-ltaco"], [], [])
        AC_CHECK_LIB([taco], [dev_ping], [LIB_TACO_CLIENT="-ltaco"], [], [])
        AC_CHECK_LIB([taco], [startup], [LIB_TACO_SERVER="$LIB_TACO -ltacomain"], [], [-ltacomain])
        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS
        AC_CHECK_LIB([taco++], [dev_ping], [LIB_TACO_CXX_CLIENT="-ltaco++"], [], [$LIB_TACO])
        AC_CHECK_LIB([taco++], [startup], [LIB_TACO_CXX_SERVER="-ltaco++ -ltacomain"], [], [-ltacomain])
        AC_LANG_RESTORE
        LIBS="$ac_taco_save_LIBS"
        LDFLAGS="$ac_taco_save_LDFLAGS"
        AC_SUBST([LIB_TACO_SERVER])
        AC_SUBST([LIB_TACO_CLIENT])
        AC_SUBST([LIB_TACO_CXX_SERVER])
        AC_SUBST([LIB_TACO_CXX_CLIENT])
dnl     AC_SUBST(docdir, ['${datadir}/doc/packages/taco'])
	AC_SUBST(pkgconfigdir,    ['${libdir}/pkgconfig'])
        AC_SUBST(templatedir,     ['${datadir}/taco/templates'])
        AC_SUBST(resdatadir,      ['${datadir}/taco/dbase/res'])
        AC_SUBST(rundir,          ['${localstatedir}/run'])
        AC_SUBST(dbmdir,          ['${datadir}/taco/dbase/dbm'])
        AC_SUBST(CMDS_resdir,     ['${resdatadir}/CMDS'])
        AC_SUBST(ERROR_resdir,    ['${resdatadir}/ERROR'])
        AC_SUBST(CLASS_resdir,    ['${resdatadir}/CLASS'])
        AC_SUBST(EVENTS_resdir,   ['${resdatadir}/EVENTS'])
        AC_SUBST(NAMES_resdir,    ['${resdatadir}/NAMES'])
        AC_SUBST(PS_NAMES_resdir, ['${resdatadir}/PS_NAMES'])
        AC_SUBST(SYS_resdir,      ['${resdatadir}/SYS'])
        AC_SUBST(SEC_resdir,      ['${resdatadir}/SEC'])
        AC_SUBST(TEST_resdir,     ['${resdatadir}/TEST'])
])

#
# TACO_BASE_PATH
# --------------
#
AC_DEFUN([TACO_BASE_PATH],
[
        AC_REQUIRE([AC_CANONICAL_SYSTEM])

        PKG_CHECK_MODULES(TACO, taco >= 3.0.0,
                [
                taco_includes=`$PKG_CONFIG --cflags-only-I taco-client`
                taco_libraries=`$PKG_CONFIG --libs-only-L taco-client`
                ],
                [
		AC_CHECK_PROGS(TACO_CONFIG, taco-config)
		AS_IF([test -n "$TACO_CONFIG"],
                      [
			taco_includes=`taco-config --includedir`
			taco_libraries=`taco-config --libdir`
		      ],
		      [
                	AS_IF([test "${prefix}" != NONE],
			      [
                      		taco_includes=${prefix}/include
                       		AS_IF([test "${exec_prefix}" != NONE], [taco_libraries=${exec_prefix}], [taco_libraries=${prefix}])
                       		case "$target" in
                        	       	x86_64-*-linux* | ia64-*-linux-* | ia64-*-freebsd* | x86_64-*-freebsd*)
                        	              	taco_libraries=${taco_libraries}/lib64
                        	               	;;
                        	       	*)
                        	               	taco_libraries=${taco_libraries}/lib
                        	               	;;
                       		esac
                	      ],
			      [
                       		taco_libraries=""
                       		taco_includes=""
                	      ])
		      ])
                ]
        )
        ac_taco_includes=$taco_includes
        ac_taco_libraries=$taco_libraries

	AC_CACHE_VAL(ac_cv_have_taco,
        [
dnl try to guess taco locations
                taco_incdirs="$ac_taco_includes"
                taco_standard_dirs="/usr/local/dshome /usr/lib/taco /usr/local/taco /usr/taco /opt/taco /usr /usr/local"
                test -n "$DSHOME" && taco_standard_dirs="$taco_standard_dirs $DSHOME"
                for i in $taco_standard_dirs ; do
                        taco_incdirs="$taco_incdirs $i/include"
                done

                AC_FIND_HEADER([API.h], [$taco_incdirs], [eval taco_incdir="\"$i\""],
                        [AC_MSG_ERROR([In the following dirs: \"$taco_incdirs\", there are no taco headers installed.])])
                ac_taco_includes="$taco_incdir"

                taco_libdirs="$ac_taco_libraries"
                for i in $taco_standard_dirs ; do
                        case "$target" in
                                x86_64-*-linux* | ia64-*-linux-* | ia64-*-freebsd* | x86_64-*-freebsd* )
                                        taco_libdirs="$taco_libdirs $i/lib64"
                                        ;;
                                *)
                                        taco_libdirs="$taco_libdirs $i/lib"
                                        ;;
                        esac
                done
                AC_FIND_LIB([taco], [xdr_length_DevChar], [$taco_libdirs], [eval taco_libdir="\"$i\""; LIB_TACO="-ltaco"],
                        [AC_MSG_ERROR([In the following dirs: $taco_libdirs, there are no taco libraries installed.])], [])
                ac_taco_libraries="$taco_libdir"

                AS_IF([test "$ac_taco_includes" = NO -o "$ac_taco_libraries" = NO], [ac_cv_have_taco="have_taco=no"],
                      [ac_cv_have_taco="have_taco=yes 
                       ac_taco_includes=$ac_taco_includes ac_taco_libraries=$ac_taco_libraries"])
        ])dnl

	eval "$ac_cv_have_taco"
        if test "$have_taco" != "yes"; then
                if test x"${prefix}" = xNONE; then
                        ac_taco_prefix="$ac_default_prefix"
                else
                        ac_taco_prefix="$prefix"
                fi
                if test x"${exec_prefix}" = xNONE; then
                        ac_taco_exec_prefix="$ac_taco_prefix"
                        AC_MSG_RESULT([will be installed in $ac_taco_prefix])
                else
                        ac_taco_exec_prefix="$exec_prefix"
                        AC_MSG_RESULT([will be installed in $ac_taco_prefix and $ac_taco_exec_prefix])
                fi
                case "$target" in
                        x86_64-*-linux* | ia64-*-linux-* | ia64-*-freebsd* )
                                taco_libraries="${ac_taco_exec_prefix}/lib64"
                                ;;
                        *)
                                taco_libraries="${ac_taco_exec_prefix}/lib"
                                ;;
                esac
                taco_includes="${ac_taco_prefix}/include"
        else
                ac_cv_have_taco="have_taco=yes \
                ac_taco_includes=$ac_taco_includes ac_taco_libraries=$ac_taco_libraries"
                AC_MSG_RESULT([libraries $ac_taco_libraries, headers $ac_taco_includes])
                taco_libraries="$ac_taco_libraries"
                taco_includes="${ac_taco_includes}"
        fi

        AC_MSG_CHECKING([for TACO])
        AC_MSG_RESULT([found $taco_path libraries $taco_libraries])

	AC_SUBST(taco_libraries)
        AC_SUBST(taco_includes)

        AC_SUBST([TACO_LDFLAGS], ["-L$taco_libraries"])
        AC_SUBST([TACO_INCLUDES], ["-I${taco_includes}  -I${taco_includes}++"])
        AC_SUBST([LIB_TACO])

        AC_SUBST([all_includes], ["$TACO_INCLUDES $all_includes"])
        AC_SUBST([all_libraries], ["$TACO_LDFLAGS $all_libraries"])

])

#
# TACO_CHECK_RPC
# --------------
#
AC_DEFUN([TACO_CHECK_RPC],
[
	AC_CHECK_HEADERS([rpc.h rpc/rpc.h])
        AC_CHECK_HEADERS([svc.h rpc/svc.h rpc/pmap_prot.h rpc/pmap_clnt.h])
        AC_CHECK_MEMBERS([SVCXPRT.xp_fd, SVCXPRT.xp_sock], [], [], [
#if HAVE_RPC_RPC_H
#       include <rpc/rpc.h>
#elif HAVE_RPC_H
#       include <rpc.h>
#else
#error rpc.h not found
#endif
])

        AH_BOTTOM([
#ifndef HAVE_SVCXPRT_XP_SOCK
#ifdef HAVE_SVCXPRT_XP_FD
#       define xp_sock xp_fd
#else
#       error "Can't find xp_sock"
#endif
#endif])

        AC_CHECK_DECLS([get_myaddress], [], [], [
#if HAVE_RPC_RPC_H
#       include <rpc/rpc.h>
#elif HAVE_RPC_H
#	include <rpc.h>
#else
#	error no rpc.h
#endif
])
])

#
# TACO_CHECK_SIZEOF_TYPES
# -----------------------
#
AC_DEFUN([TACO_CHECK_SIZEOF_TYPES],
[
	AC_CHECK_SIZEOF(char)
	AC_CHECK_SIZEOF(unsigned char)
	AC_CHECK_SIZEOF(short)
	AC_CHECK_SIZEOF(unsigned short)
	AC_CHECK_SIZEOF(int)
	AC_CHECK_SIZEOF(unsigned int)
	AC_CHECK_SIZEOF(long)
	AC_CHECK_SIZEOF(unsigned long)
	AC_CHECK_SIZEOF(float)
	AC_CHECK_SIZEOF(double)
	AC_TYPE_INT8_T
	AC_TYPE_UINT8_T
	AC_TYPE_INT16_T
	AC_TYPE_INT16_T
	AC_TYPE_UINT16_T
	AC_TYPE_INT32_T
	AC_TYPE_UINT32_T
	AC_TYPE_INT64_T
	AC_TYPE_UINT64_T
])
