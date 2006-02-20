dnl Toolkit for building distributed control systems or any other distributed system.
dnl
dnl Copyright (C) 2003-2005  Jens Krueger <jkrueger1@users.sf.net>
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AC_INIT([TACO], [3.94.0], [taco-devel@lists.sourceforge.net])
AC_PREREQ([2.57])
AC_REVISION($Revision: 1.77 $)dnl
AC_COPYRIGHT([(C) 1990-2006 ESRF])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([config.h.in])

TACO_INIT([no])

AC_SUBST(LIB_VERSION, [11:8:0])

AM_INIT_AUTOMAKE([dist-bzip2 1.7])

dnl AM_PROG_LIBTOOL

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_AWK
AC_CHECK_PROGS(HOSTNAME, hostname)
if test "$HOSTNAME" ; then
	hostname=`hostname`;
fi
if test "$hostname" ; then
	HOSTNAME='hostname'
else 
	hostname=`hostname`
	if test -z "$hostname" ; then
		HOSTNAME='uname -n'
	fi
fi
AC_SUBST([HOSTNAME])

GCC_LIB_PATH="" 
if test x"$CC" = x"gcc" ; then
	AC_MSG_CHECKING([for the path to the gcc library])
	for i in `gcc -print-libgcc-file-name | tr '/' ' '` ; do 
		if test x"$i" = x"gcc-lib" ; then 
			break ; 
		else 
			GCC_LIB_PATH="${GCC_LIB_PATH}/${i}" 
		fi  
	done  
	AC_MSG_RESULT([$GCC_LIB_PATH])
fi
AC_SUBST([GCC_LIBRARY_PATH], [$GCC_LIB_PATH])

AC_ARG_ENABLE(longhostname, AC_HELP_STRING([--disable-longhostname], [disable the use of long hostnames [default=enabled]]),
[
	case ${enableval} in
		no)
			taco_use_longhostnames=no
			;;
		*)
			taco_use_longhostnames=yes
			;;
       	esac
],
[
	taco_use_longhostnames=yes
])

AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug=ARG], [enables debug symbols (yes|no|full) [default=no]]),
[
	case ${enableval} in
		yes)
			taco_debug_code="yes"
			;;
		full)
			taco_debug_code="full"
			;;
		*)
			taco_debug_code="no"
			;;
	esac
],
[
	taco_debug_code=no
])

if test x"$CC" = x"gcc"; then
       if test "$taco_debug_code" != "no"; then
               if test $taco_debug_code = "full"; then
                       CFLAGS=`echo $CFLAGS | sed -e"s/-O2//g"`
                       CFLAGS="-g3 -fno-inline $CFLAGS"
               else
                       CFLAGS="-g -O2 $CFLAGS"
               fi
       else
               CFLAGS="-O2 $CFLAGS"
       fi
fi
if test -z "$LDFLAGS" && test "$taco_debug_code" = "no" && test x"$CC" = x"gcc"; then
       LDFLAGS=""
fi

if test x"$CXX" = x"g++" || test x"$CXX" = x"KCC"; then
       if test "$taco_debug_code" != "no"; then
               if test x"$CXX" = x"KCC"; then
                       CXXFLAGS="+K0 -Wall -pedantic -W -Wpointer-arith -Wwrite-strings $CXXFLAGS"
               else
                       if test "$taco_debug_code" = "full"; then
                               CXXFLAGS=`echo $CXXFLAGS | sed -e"s/-O2//g"`
                               CXXFLAGS="-g3 -fno-inline $CXXFLAGS"
                       else
                               CXXFLAGS="-g -O2 $CXXFLAGS"
                       fi
               fi
       else
               if test x"$CXX" = "xKCC"; then
                       CXXFLAGS="+K3 $CXXFLAGS"
               else
                       CXXFLAGS="-O2 $CXXFLAGS"
               fi
       fi
fi

# Check for Solaris CC compiler (added by Andy)
AC_MSG_CHECKING([for Solaris CC compiler])
AM_CONDITIONAL(SOLARIS_CXX, [$CXX -V 2>&1 |grep Sun])
AC_MSG_RESULT([$SOLARIS_CXX])
AC_SUBST([SOLARIS_CXX], [$SOLARIS_CXX])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STAT
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h sys/socket.h]) 
AC_CHECK_HEADERS([pthread.h fcntl.h malloc.h memory.h netdb.h stddef.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h termios.h unistd.h sys/filio.h paths.h siggen.h])
AC_CHECK_HEADERS([varargs.h stdarg.h])
AC_CHECK_HEADERS([svc.h rpc/svc.h rpc/pmap_prot.h rpc/pmap_prot.h])
AC_CHECK_HEADERS([signal.h sys/signal.h sys/param.h])
AC_TYPE_SIGNAL
AC_CHECK_DECLS([signal], [], [],
[
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#endif
])
AC_CHECK_FUNCS([signal], [], [], 
[
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#endif
])
AC_CHECK_TYPES([sighandler_t, __sighandler_t], [], [],
[
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#endif
])

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
AC_TYPE_SIGNAL
AC_TYPE_UID_T
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES(socklen_t, [],[],
[#if HAVE_SYS_SOCKET_H
#	include	<sys/socket.h>
#elif HAVE_SOCKET_H
#	include <socket.h>
#elif HAVE_NETDB_H
#	include <netdb.h>
#elif HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif])
AC_CHECK_TYPES(time_t)

AC_MSG_CHECKING([for number of parameters in va_start])
AH_TEMPLATE([HAVE_VA_START_P1], [define whether the va_start macro expect only one parameter])
AH_TEMPLATE([HAVE_VA_START_P1_P2], [define whether the va_start macro expect two parameters])
AC_COMPILE_IFELSE([
	AC_LANG_PROGRAM([[
#if HAVE_STDARG_H
#	include <stdarg.h>
#elif HAVE_VARARGS_H
#	include <varargs.h>
#endif

void dev_printerror (int mode, char *fmt, ...)
{
        va_list         args;
        va_start(args, fmt);
}
]],
[[dev_printerror(0, "%s", "Hello World\n");]])], [
AC_DEFINE(HAVE_VA_START_P1_P2)
AC_MSG_RESULT([2])], [AC_DEFINE(HAVE_VA_START_P1)
AC_MSG_RESULT([1])])
	
AC_MSG_CHECKING([for WAIT_ANY in waitpid])
AH_TEMPLATE([HAVE_WAIT_ANY], [defines whether the WAIT_ANY macro is defined or not])
AH_VERBATIM([_HAVE_WAIT_ANY],
[#if !HAVE_WAIT_ANY
#	define WAIT_ANY 	(-1)
#endif])
AC_COMPILE_IFELSE([
	AC_LANG_PROGRAM([[
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif
]],
[[
int	status;
int 	pid = waitpid (WAIT_ANY, &status, WNOHANG);]])], [
AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_WAIT_ANY)],
[AC_MSG_RESULT([no])])


AH_VERBATIM([_FIONREAD], [
#if HAVE_SYS_IOCTL_H
#	include <sys/ioctl.h>
#endif
#ifndef FIONREAD
#	define	FIONREAD	TIOCISIZE
#endif])

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm dup2 gethostbyname gethostname getpass gettimeofday isascii mblen memset putenv select socket strcasecmp strchr strcspn strdup strerror strncasecmp strrchr strspn strstr strtoul get_myaddress])
AC_CHECK_FUNCS([setsid])

AC_SEARCH_LIBS(dlopen, [dl])
AC_SEARCH_LIBS(svc_run, [c nsl])
AC_SEARCH_LIBS(socket, [c socket], [], [], [-lnsl])
AC_SEARCH_LIBS(connect, [c socket], [], [], [-lnsl])
AC_SEARCH_LIBS(nanosleep, [c rt c_r])
AC_SEARCH_LIBS(inet_pton, [c socket nsl])

sbin_dir=`eval "echo $sbindir"`
if test x"$prefix" = x"NONE" -a x"$sbin_dir" = x"NONE/sbin"; then
	sbin_dir=`echo $sbin_dir| sed -e s%NONE/%%`
	sbin_dir="${ac_default_prefix}/${sbin_dir}"
else
	sbin_dir="$sbindir"
fi
AC_SUBST(sbin_dir)
AC_CONFIG_COMMANDS([pathes.h], [echo '#define sbindir "'${sbin_dir}'"' > pathes.h], [sbin_dir=$sbin_dir]) 

TACO_CHECK_RPC
TACO_CHECK_RPC_AND_THREADS
AC_CHECK_DECLS([get_myaddress], [], [], [
#if HAVE_RPC_RPC_H
#       include <rpc/rpc.h>
#elif HAVE_RPC_H
#       include <rpc.h>
#else
#       error "Can't find rpc.h"
#endif
]) 
AC_CHECK_FUNCS([get_myaddress])
AC_CHECK_DECLS([gethostname])

AC_SUBST(QT_INCLUDES, ['-I/usr/lib/qt3/include'])
AC_SUBST(UIC, ['/usr/lib/qt3/bin/uic'])
AC_SUBST(MOC, ['/usr/lib/qt3/bin/moc'])
AC_SUBST(QT_LDFLAGS, ['-L/usr/lib/qt3/lib'])
AC_SUBST(LIBSM, ['-lSM -lICE'])
AC_SUBST(LIB_X11, ['-lX11 $(LIBSM) $(LIBSOCKET)'])
AC_SUBST(LIB_QT, ['-lqt $(LIBPNG) $(LIBJPEG) -lXext $(LIB_X11) $(LIBSM)'])
AC_SUBST(X_LDFLAGS, ['-L/usr/X11R6/lib'])

AC_SUBST(appdefaultdir)

AC_SUBST(docdir, ['${datadir}/doc/packages/taco'])
AC_SUBST(templatedir, ['${datadir}/taco/templates'])
AC_SUBST(resdatadir, ['${datadir}/taco/dbase/res'])
AC_SUBST(dbmdir, ['${localstatedir}/cache/taco'])
AC_SUBST(logdir, ['${localstatedir}/log/taco'])
AC_SUBST(CMDS_resdir, ['${resdatadir}/CMDS'])
AC_SUBST(ERROR_resdir, ['${resdatadir}/ERROR'])
AC_SUBST(CLASS_resdir, ['${resdatadir}/CLASS'])
AC_SUBST(EVENTS_resdir, ['${resdatadir}/EVENTS'])
AC_SUBST(NAMES_resdir, ['${resdatadir}/NAMES'])
AC_SUBST(PS_NAMES_resdir, ['${resdatadir}/PS_NAMES'])
AC_SUBST(SYS_resdir, ['${resdatadir}/SYS'])
AC_SUBST(SEC_resdir, ['${resdatadir}/SEC'])
AC_SUBST(TEST_resdir, ['${resdatadir}/TEST'])

dnl aclocaldir=`aclocal --print-ac-dir`
AC_SUBST([aclocaldir], ['${datadir}/aclocal'])

AC_SUBST(TACO_INCLUDES, ["-I\$(top_srcdir)/lib/xdr -I\$(top_srcdir)/lib -I\$(top_srcdir)/lib/dc -I\$(top_srcdir)/lib/dbase -I\$(top_srcdir)/lib/tc -I\$(top_srcdir)/lib/ascii -I\$(top_srcdir)/lib/signal] -I\$(top_srcdir)/lib/private") 

AC_SUBST(TACO_SERVER_LIBS, ["\$(top_builddir)/lib/libtacomain.la \$(top_builddir)/lib/libtaco.la"])
AC_SUBST(TACO_SERVER_CXX_LIBS, ["\$(top_builddir)/lib/libtacomain.la \$(top_builddir)/lib/libtaco++.la"])
AC_SUBST(TACO_SERVER_NOMAIN_CXX_STATIC_LIBS, ["\$(top_builddir)/lib/.libs/libtaco++.a"])
AC_SUBST(TACO_CLIENT_LIBS, ["\$(top_builddir)/lib/libtaco.la"])
AC_SUBST(TACO_CLIENT_CXX_LIBS, ["\$(top_builddir)/lib/libtaco++.la"])
AC_SUBST(TACO_DEVSERVER_MAIN, ["\$(top_builddir)/lib/DevServerMain.o"])
AC_SUBST(TANGO_CLIENT_CXX_LIBS, ["\$(top_builddir)/lib/tango/libtacotango++.la"])

TACO_DEFINES
TACO_SERVER_T
TACO_DATAPORT_SRC
TACO_TANGO
TACO_PYTHON_BINDING
TACO_TCL_BINDING
TACO_GRETA
TACO_XDEVMENU
TACO_ALARM
TACO_MANAGER
TACO_DATABASE_SERVER
TACO_CLIENT_TCP

if test "x${taco_build_gdbm}" = "xyes" ; then
     AC_CONFIG_SUBDIRS(gdbm)
fi
TACO_MESSAGE_SERVER

dnl the startup routine should not be built in the dsapi library on Mac OS X systems
AM_CONDITIONAL(BUILTIN_STARTUP, ! echo $target|grep darwin)

AC_SUBST([RPM_RELEASE], 0)
AM_RPM_INIT
LINUX_DISTRIBUTION
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

AC_CONFIG_FILES(Makefile 
		misc/Makefile
		misc/taco-config
		misc/taco.SuSE
		misc/taco.debian
		misc/taco-manager.sh
		misc/tacoenv.sh
		misc/tacoenv.csh
		misc/taco.startup
		misc/taco.shutdown
		misc/taco.status
		doc/Makefile
		lib/Makefile
		lib/Doxyfile
		lib/private/Makefile
		lib/dbase/Makefile
		lib/dataport/Makefile
		lib/xdr/Makefile
		lib/dc/Makefile
		lib/ascii/Makefile
		lib/ascii/doc/Makefile
		lib/tc/Makefile
		lib/tango/Makefile
		lib/signal/Makefile
		system/dbase/Makefile
		system/dbase/server/Makefile
		system/dbase/tools/Makefile
		system/dbase/res/Makefile
		system/Makefile 
		system/manager/Makefile 
		system/msg/Makefile 
		system/dc/Makefile 
		system/dc/doc/Makefile 
		system/dc/server/Makefile 
		system/dc/library/Makefile 
		system/dc/tools/Makefile
		classes/Makefile
		system/dbase/greta/Makefile
		system/xdevmenu/Makefile
		system/xdevmenu/doc/Makefile
		system/starter/Makefile
		system/alarm/Makefile
		taco.spec
)

dnl AC_CONFIG_FILES(system/dbdev/Makefile
dnl 		system/dbdev/src/Makefile)
dnl		system/dbdev/doc/Makefile)

AC_CONFIG_FILES(classes/instverify/Makefile)
AC_CONFIG_FILES(classes/powersupply/Makefile)
AC_CONFIG_FILES(classes/serialline/Makefile)
AC_CONFIG_FILES(classes/updatedaemon/Makefile)

AC_CONFIG_FILES(m4/Makefile)

AC_CONFIG_FILES(bindings/python/Makefile 
		bindings/python/lib/Makefile 
		bindings/python/doc/Makefile 
		bindings/python/demo.server/Makefile)

AC_CONFIG_FILES(bindings/Makefile bindings/tcl/Makefile)

dnl AC_CONFIG_FILES(bindings/labview/Makefile
dnl 		bindings/labview/client/Makefile
dnl 		bindings/labview/server/Makefile)

dnl Initialize the test suite
AC_CONFIG_TESTDIR(tests)
AC_CONFIG_FILES(tests/Makefile tests/atlocal)
dnl AC_CONFIG_FILES(tests/testserver/Makefile)
AC_CONFIG_FILES(tests/taco, [chmod +x tests/taco])
AC_CHECK_PROGS([AUTOM4TE], [autom4te autom4te259 autom4te253])
AM_MISSING_PROG([AUTOM4TE], [$AUTOM4TE])

AC_OUTPUT

if test x"${taco_dbserver}" = x"yes" ; then
	taco_dbserver_support="
	 	with:
		DBM support:	${taco_gdbm}
		mySQL support:	${taco_mysql}"
fi

AC_MSG_RESULT([

Configuration (taco):

	Source code location:	${srcdir}
	Version:		${VERSION}
	Compiler:		${CC},${CXX}

	Motif:			${motif_found}
	CORBA:			${corba_found}
	TANGO:			${tango_found}
	
build:
	gdbm:			${taco_build_gdbm}
	manager:		${taco_manager}
	database server:	${taco_dbserver} ${taco_dbserver_support}
	message server:		${taco_message}

	greta:			${greta}
	xdevmenu:		${xdevmenu}
	S_Alarm:		${alarm}

	client tcp :		${taco_client_tcp}
	server API:		${taco_server_libs}
	dataport API:		${DATAPORT}
	data collector API:	${taco_dc}
	TANGO poller :		${tango_poller}
	ASCII API:		${taco_ascii}
	bindings:
		python :	${taco_python_binding}
		TCL:		${taco_tcl_binding}

	TANGO support:		${taco_tango}

Please check whether the configuration I detected matches what you
would like to have. 
])
