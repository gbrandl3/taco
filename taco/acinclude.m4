dnl Toolkit for building distributed control systems or any other distributed system.
dnl
dnl Copyright (C) 2003-2005  Jens Krüger <jkrueger1@users.sf.net>
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

AC_DEFUN([TACO_PYTHON_BINDING],
[
	AC_REQUIRE([TACO_SERVER_T])
	PYTHON_PROG(2.0, [yes])
	PYTHON_DEVEL
	if test "x$taco_python_binding" = "xyes" ; then
		ac_save_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS $PYTHON_CPPFLAGS"
		AC_CHECK_HEADERS([numarray/arrayobject.h], [taco_python_binding=yes], 
			AC_CHECK_HEADERS(Numeric/arrayobject.h, [taco_python_binding=yes], [taco_python_binding=no], [#include <Python.h>]),
			[#include <Python.h>])
		CFLAGS="$ac_save_CFLAGS"
	fi	
	if test x"${taco_server_libs}" != x"yes" ; then
		taco_python_binding=no
	fi
	AM_CONDITIONAL(PYTHON_BINDING, [test x"${taco_python_binding}" = x"yes"])
])

AC_DEFUN([TACO_TCL_BINDING],
[
	AC_REQUIRE([TCL_DEVEL])
	if test -n "$TCLINCLUDE" -a -n "$TCLLIB" -a "$TCLPACKAGE" != "no" ; then
		taco_tcl_binding=yes
	else
		taco_tcl_binding=no
	fi
	AM_CONDITIONAL(TCL_BINDING, test $taco_tcl_binding = yes)
])

AC_DEFUN([TACO_DBM_SERVER],
[
	TACO_MYSQL_SUPPORT
	TACO_GDBM_SUPPORT
	TACO_SQLITE3_SUPPORT
	AM_CONDITIONAL(MYSQLSUPPORT, test "x$taco_mysql" = "xyes")
dnl disable gdbm support of the dbm server if mysql support of database server is disabled
	if test "x$taco_mysql" != "xyes" ; then 	
		taco_gdbm=yes
	fi
dnl disable the build of gdbm if gdbm support of database server is disabled
	if test "x$taco_gdbm" = "xno" -a "x$taco_build_gdbm" = "xyes" ; then
		taco_build_gdbm=no
	fi 
	AM_CONDITIONAL(GDBMSUPPORT, test "x$taco_gdbm" = "xyes")
	AM_CONDITIONAL(BUILD_GDBM, test "x$taco_build_gdbm" = "xyes")
	AC_SUBST(GDBM_CFLAGS)
	AC_SUBST(GDBM_LIBS)
	AC_SUBST(GDBM_COMPAT_LIBS)
	AC_SUBST(MYSQL_CFLAGS)
	AC_SUBST(MYSQL_LIBS)
])

AC_DEFUN([TACO_SQLITE3_SUPPORT],
[
	AC_ARG_ENABLE(sqlite3, AC_HELP_STRING(--enable-sqlite3, [build the database server with SQLite3 support @<:@default=yes@:>@]),
		[case "${enable_sqlite3}" in
			yes)	taco_sqlite3=yes;;
			no)	taco_sqlite3=no;;
			*)	AC_MSG_ERROR([bad value ${enable_sqlite3} for --enable-sqlite3]);;
		esac], [taco_sqlite3=yes])	
	PKG_CHECK_MODULES(SQLITE3, sqlite3 >= 3.0.0, [taco_sqlite3=yes], [taco_sqlite3=no])
])

AC_DEFUN([TACO_MYSQL_SUPPORT],
[
dnl
dnl Get the cflags and libraries
dnl
	AC_PATH_TOOL(MYSQL_CONFIG, mysql_config)
	AC_ARG_ENABLE(mysql, AC_HELP_STRING(--enable-mysql, [build the database server with mysql support @<:@default=yes@:>@]),
		[case "${enable_mysql}" in
			yes)	taco_mysql=yes;;
			no)	taco_mysql=no;;
			*)	AC_MSG_ERROR([bad value ${enable_mysql} for --enable-mysql]);;
		esac], [taco_mysql=yes])
	AC_ARG_WITH(mysql, AS_HELP_STRING([--with-mysql=PFX], [Prefix where mysql is installed, e.g. '/usr/local/mysql']),
		[mysql_prefix="$withval"], [mysql_prefix=""])
	AC_ARG_WITH(mysql-libraries, AS_HELP_STRING([--with-mysql-libraries=DIR], [Directory where mysql library is installed (optional)]), 
		[mysql_libraries="$withval"], [mysql_libraries=""])
	AC_ARG_WITH(mysql-includes, AS_HELP_STRING([--with-mysql-includes=DIR], [Directory where mysql header files are installed (optional)]), 
		[mysql_includes="$withval"], [mysql_includes=""])

	if test "x$mysql_libraries" != "x" ; then
		MYSQL_LIBS="-L$mysql_libraries -lmysqlclient -lm -lz"
	elif test "x$mysql_prefix" != "x" ; then
		MYSQL_LIBS="-L$mysql_prefix/lib/mysql -lmysqlclient -lm -lz"
	elif test -n "$MYSQL_CONFIG" ; then
		MYSQL_LIBS=`$MYSQL_CONFIG --libs`
	elif test "x$prefix" != "xNONE" -a "x$prefix" != "x/usr"; then
		MYSQL_LIBS="-L$prefix/lib -lmysqlclient -lm -lz"
	fi

	if test "x$mysql_includes" != "x" ; then
		MYSQL_CFLAGS="-I$mysql_includes"
	elif test "x$mysql_prefix" != "x" ; then
		MYSQL_CFLAGS="-I$mysql_prefix/include/mysql"
	elif test -n "$MYSQL_CONFIG" ; then
		MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
	elif test "x$prefix" != "xNONE" -a "x$prefix" != "x/usr"; then
		MYSQL_CFLAGS="-I$prefix/include"
	fi

	if test "x$taco_mysql" = "xyes" ; then
		save_LIBS="$LIBS"
		save_CPPFLAGS="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $MYSQL_CFLAGS" 
		LIBS="$LIBS $MYSQL_LIBS"
		taco_mysql=no
		AC_CHECK_HEADERS([mysql/mysql.h mysql.h], [taco_mysql=yes])
		if test "x$taco_mysql" = "xyes" ; then
			AC_LINK_IFELSE(
				[AC_LANG_PROGRAM(
[#if HAVE_MYSQL_MYSQL_H
#	include <mysql/mysql.h>
#else
#	include <mysql.h> 
#endif
],
[
MYSQL       mysql,
            *mysql_conn = mysql_real_connect(&mysql, "localhost", "myuser", "mypasswd", "mydb", 0, 0, 0); 

])], [], [taco_mysql=no])
		fi
		LIBS="$save_LIBS"
		CPPFLAGS="$save_CPPPLAGS"
	fi
])

AC_DEFUN([TACO_SERVER_T],
[
	AC_ARG_ENABLE(server, AC_HELP_STRING([--enable-server], [build the libraries for the TACO servers @<:@default=yes@:>@]),
		[case "${enable_server}" in
			yes)	taco_server_libs=yes;;
			no)	taco_server_libs=no;;
			*)	AC_MSG_ERROR([bad value ${enable_server} for --enable-server]);;
		esac], [taco_server_libs=yes])
	AM_CONDITIONAL(BUILD_SERVER, [test x"${taco_server_libs}" = x"yes"])
])

AC_DEFUN([TACO_DATABASE_SERVER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(dbserver, AC_HELP_STRING([--enable-dbserver], [build the TACO database server @<:@default=yes@:>@]),
		[case "${enable_dbserver}" in
			yes)	taco_dbserver=yes;;
			no)	taco_dbserver=no;;
			*)	AC_MSG_ERROR([bad value ${enable_dbserver} for --enable-dbserver]);;
		esac], [taco_dbserver=yes])
	if test x"${taco_server_libs}" != x"yes" ; then
		taco_dbserver=no
	fi
	if test x"${taco_dbserver}" = x"yes" ; then
		TACO_DBM_SERVER
	else
		AM_CONDITIONAL(MYSQLSUPPORT, [false])
		AM_CONDITIONAL(GDBMSUPPORT, [false])
		AM_CONDITIONAL(BUILD_GDBM, [false])
		taco_build_gdbm=no
	fi
	AM_CONDITIONAL(BUILD_DATABASESERVER, [test x"${taco_dbserver}" = x"yes"])
])

AC_DEFUN([TACO_MANAGER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(manager, AC_HELP_STRING([--enable-manager], [build the TACO manager @<:@default=yes@:>@]),
		[case "${enable_manager}" in
			yes)	taco_manager=yes;;
			no)	taco_manager=no;;
			*)	AC_MSG_ERROR([bad value ${enable_manager} for --enable-manager]);;
		esac], [taco_manager=yes])
	if test x"${taco_server_libs}" != x"yes" ; then
		taco_manager=no
	fi
	AM_CONDITIONAL(BUILD_MANAGER, [test x"${taco_manager}" = x"yes"])
])

AC_DEFUN([TACO_MESSAGE_SERVER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(message, AC_HELP_STRING([--enable-message], [build the TACO message server @<:@default=yes@:>@]),
		[case "${enable_message}" in
			yes)	taco_message=yes;;
			no)	taco_message=no;;
			*)	AC_MSG_ERROR([bad value ${enable_message} for --enable-message]);;
		esac], [taco_message=yes])
	if test x"${taco_server_libs}" != x"yes" ; then
		taco_message=no
	fi
	AM_CONDITIONAL(BUILD_MESSAGESERVER, [test x"${taco_message}" = x"yes"])
])

AC_DEFUN([TACO_DC_API],
[
	AC_ARG_ENABLE(dc, AC_HELP_STRING([--enable-dc], [build the data collector API @<:@default=yes@:>@]),
		[case "${enable_dc}" in
			yes)	taco_dc=yes;;
			no)	taco_dc=no;;
			*)	AC_MSG_ERROR([bad value ${enable_dc} for --enable-dc]);;
		esac], [taco_dc=yes])
	AM_CONDITIONAL(DC_BUILD, test "x$taco_dc" = "xyes") 
#	if test "x$taco_dc" = "xyes" ; then
#		TACO_DC_LIBS="\$(top_builddir)/lib/dc/libdcapi.la"
#		TACO_DC_LIBS=
#	fi
#	AC_SUBST(TACO_DC_LIBS)
	AC_ARG_ENABLE(tango_poller, AC_HELP_STRING([--enable-tango_poller], [enable the data collector to poll tango devices @<:@default=no@:>@]),
		[case "${enable_tango_poller}" in
			yes)	tango_poller=yes;;
			no)	tango_poller=no;;
			*)	AC_MSG_ERROR([bad value ${enable_tango_poller} for --enable-tango_poller]);;
		esac], [tango_poller=no])
	AM_CONDITIONAL(TANGO_POLLER, test "x$tango_poller" = "xyes") 
])

AC_DEFUN([TACO_TANGO],
[
	AC_REQUIRE([WITH_CORBA])
	AC_REQUIRE([WITH_TANGO])
	AC_ARG_ENABLE(tango, AC_HELP_STRING([--enable-tango], [build the TANGO access @<:@default=no@:>@]),
		[case "${enable_tango}" in
			yes)    taco_tango=yes;;
			no)     taco_tango=no;;
			*)      AC_MSG_ERROR([bad value ${enable_tango} for --enable-tango]);;
		esac], [taco_tango=no])
	AM_CONDITIONAL(TANGO_BUILD, test "x$taco_tango" = "xyes")
	if test "x$taco_tango" = "xyes" ; then
		WITH_CORBA
		WITH_TANGO
		TACO_TANGO_LIBS="\$(top_builddir)/lib/tango/libtacotango++.la"

	fi
	AC_SUBST(TACO_TANGO_LIBS)
])


AC_DEFUN([TACO_GRETA],
[
	AC_REQUIRE([TACO_DC_API])
	AC_REQUIRE([X_AND_MOTIF])
	AC_ARG_ENABLE(greta, AC_HELP_STRING([--enable-greta], [build the graphical dbase tool @<:@default=yes@:>@]),
		[case "${enable_greta}" in
			yes)	greta=yes;;
			no)	greta=no;;
			*)	AC_MSG_ERROR(bad value ${enableval} for --enable-greta);;
		esac], [greta=yes])
	if test "x$motif_found" != "xyes" -o "x$taco_dc" != "xyes" ; then 
		greta=no
	fi
	AM_CONDITIONAL(GRETABUILD, test "x$greta" = "xyes") 
	AC_SUBST([GRETA], ["greta greta_ndbm"])
])

AC_DEFUN([TACO_ASCII_API],
[
	AC_ARG_ENABLE(ascii, AC_HELP_STRING([--enable-ascii], [build the ASCII api @<:@default=yes@:>@]),
		[case "${enable_ascii}" in
			yes)	taco_ascii=yes;;
			no)	taco_ascii=no;;
			*)	AC_MSG_ERROR([bad value ${enable_ascii} for --enable-ascii]);;
		esac], [taco_ascii=yes])
	AC_CHECK_HEADERS([dlfcn.h], [], [
		AC_CHECK_HEADERS([dl.h], [], [taco_ascii=no])])
	if test "x$taco_ascii" = "xyes" ; then
		TACO_ASCII_LIBS="\$(top_builddir)/lib/ascii/libascapi.la \$(top_builddir)/lib/tc/libtcapi.la"
	fi
	AC_SUBST(TACO_ASCII_LIBS)
	AM_CONDITIONAL(ASCII_BUILD, test "x$taco_ascii" = "xyes")
])

AC_DEFUN([TACO_XDEVMENU],
[
	AC_REQUIRE([TACO_ASCII_API])
	AC_REQUIRE([X_AND_MOTIF])
	AC_ARG_ENABLE(xdevmenu, AC_HELP_STRING([--enable-xdevmenu], [build the graphical ds tool @<:@default=yes@:>@]),
		[case "${enable_xdevmenu}" in
			yes)	xdevmenu=yes;;
			no)	xdevmenu=no;;
			*)	AC_MSG_ERROR(bad value ${enableval} for --enable-xdevmenu);;
		esac], [xdevmenu=yes])
	if test "x$xdevmenu" = "xyes" ; then
		taco_ascii=yes
		X_AND_MOTIF
		XDEVMENU=xdevmenu
	fi
	AM_CONDITIONAL(ASCII_BUILD, test "x$taco_ascii" = "xyes")
	if test "x$motif_found" != "xyes" -o "x$taco_ascii" != "xyes" ; then
		xdevmenu=no
	fi
	AM_CONDITIONAL(XDEVMENUBUILD, test "x$xdevmenu" = "xyes") 
	AC_SUBST(XDEVMENU)
])

AC_DEFUN([TACO_ALARM],
[
	AC_REQUIRE([X_AND_MOTIF])
	AC_ARG_ENABLE(alarm, AC_HELP_STRING([--enable-alarm], [build the graphical alarm tool @<:@default=yes@:>@]),
		[case "${enable_alarm}" in
			yes)	alarm=yes;;
			no)	alarm=no;;
			*)	AC_MSG_ERROR(bad value ${enableval} for --enable-alarm);;
		esac], [alarm=yes])
	if test "x$alarm" = "xno" ; then
		X_AND_MOTIF
		ALARM=alarm
	fi
	if test "x$motif_found" != "xyes" ; then
		alarm=no
	fi
	AM_CONDITIONAL(ALARMBUILD, test "x$alarm" = "xyes") 
	AC_SUBST(ALARM)
])

AC_DEFUN([X_AND_MOTIF],
[
	AC_REQUIRE([AC_PATH_XTRA])
	tmp='for i in $X_LDFLAGS ; do if test `echo $i | cut -c1-2` = "-L" ; then echo $i; break; fi; done | cut -c3- '
	appdefaultdir=`eval $tmp`/X11/app-defaults
	motif_found=yes;
	AC_ARG_WITH(motif, AC_HELP_STRING([--with-motif@<:@=ARG@:>@], [Motif @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the Motif installation, e.g. '/usr/local/myMotif']), [
		case  "$with_motif" in
			yes) 	MOTIF_LIBS="-lXm" 
				MOTIF_INCLUDES="$X_CFLAGS" ;;
			no)  	AC_MSG_WARN([Disable build of greta/xdevmenu])
				motif_found=no;;
			*) 	MOTIF_LIBS="-L$withval/lib -lXm"
				MOTIF_INCLUDES="-I$withval/include" ;;
		esac      
		],[MOTIF_LIBS="-lXm"; MOTIF_INCLUDES="$X_CFLAGS"])
	CPPFLAGS_SAVE="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $MOTIF_INCLUDES"
	LIBS_SAVE="$LIBS"
dnl	X_CFLAGS="$CPPFLAGS"
	X_LDFLAGS="$LIBS $X_LDFLAGS $MOTIF_LIBS $X_LIBS"
	AC_CHECK_HEADERS([Xm/Xm.h Xm/XmAll.h], [
		AC_CHECK_LIB(Xm, XmStringCreateLocalized, [], [motif_found=no], [$X_LDFLAGS -lXt -lX11 -lSM -lICE])
		break;
	], [motif_found=no])
	CPPFLAGS="$CPPFLAGS_SAVE"
	LIBS="$LIBS_SAVE"

	AC_SUBST(MOTIF_LIBS)
	AC_SUBST(MOTIF_INCLUDES)
])

AC_DEFUN([TACO_GDBM_SUPPORT],
[
dnl
dnl Get the cflags and libraries
dnl
	taco_build_gdbm=no
	AC_ARG_ENABLE(gdbm, AC_HELP_STRING(--enable-gdbm, [build the database server with gdbm support @<:@default=yes@:>@]),
		[case "${enable_gdbm}" in
			yes)	taco_gdbm=yes;;
			no)	taco_gdbm=no;;
			*)	AC_MSG_ERROR([bad value ${enable_gdbm} for --enable-gdbm]);;
		esac], [taco_gdbm=yes])
	AC_ARG_WITH(gdbm, AS_HELP_STRING([--with-gdbm=PFX], [Prefix where gdbm is installed, e.g. '/usr/local/gdbm']),
		[gdbm_prefix="$withval"], [gdbm_prefix=""])
	AC_ARG_WITH(gdbm-libraries, AS_HELP_STRING([--with-gdbm-libraries=DIR], [Directory where gdbm library is installed (optional)]), 
		[gdbm_libraries="$withval"], [gdbm_libraries=""])
	AC_ARG_WITH(gdbm-includes, AS_HELP_STRING([--with-gdbm-includes=DIR], [Directory where gdbm header files are installed (optional)]), 
		[gdbm_includes="$withval"], [gdbm_includes=""])

	if test "x$gdbm_libraries" != "x" ; then
		GDBM_LIBS="-L$gdbm_libraries"
	elif test "x$gdbm_prefix" != "x" ; then
		GDBM_LIBS="-L$gdbm_prefix/lib"
	elif test "x$prefix" != "xNONE" -a "x$prefix" != "x/usr"; then
		GDBM_LIBS="-L$prefix/lib"
	fi
	TACO_GDBM_LDFLAGS="$GDBM_LIBS"

	GDBM_LIBS="$GDBM_LIBS -lgdbm"
	GDBM_COMPAT_LIBS="$GDBM_LIBS -lgdbm_compat"

	if test "x$gdbm_includes" != "x" ; then
		GDBM_CFLAGS="-I$gdbm_includes"
	elif test "x$gdbm_prefix" != "x" ; then
		GDBM_CFLAGS="-I$gdbm_prefix/include"
	elif test "x$prefix" != "xNONE" -a "x$prefix" != "x/usr"; then
		GDBM_CFLAGS="-I$prefix/include"
	fi
	TACO_GDBM_INC="$GDBM_CFLAGS"

	save_LIBS="$LIBS"
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $GDBM_CFLAGS" 
	LIBS="$LIBS $GDBM_LIBS"
	AC_CHECK_HEADER(gdbm.h, [gdbm_header="gdbm.h"], [taco_build_gdbm="yes"])
	AC_MSG_CHECKING([Compiles gdbm.h with C++])
	AC_LANG_PUSH(C++)
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
[#include <${gdbm_header}> 

class Test
{
public:
	Test(GDBM_FILE, datum);
};
],
[
	GDBM_FILE	db;
	datum		d;
	Test a(db, d);
// #error Broken
]
)], [AC_CHECK_LIB(gdbm, gdbm_open)], [], [taco_build_gdbm="yes"])
	if test "$taco_build_gdbm" = "yes" ;then
		AC_MSG_RESULT([no])
	else
		AC_MSG_RESULT([yes])
	fi
	AC_LANG_POP(C++)
	if test x${ac_cv_lib_gdbm_gdbm_open} != xyes ; then
		GDBM_LIBS="\$(top_builddir)/gdbm/libgdbm.la"
		GDBM_CFLAGS="-I\$(top_srcdir)/gdbm -I\$(top_builddir)/gdbm"
		taco_build_gdbm="yes"
	fi
	LIBS="$save_LIBS"
	CPPFLAGS="$save_CPPPLAGS"
]
)

AC_DEFUN([TACO_DATAPORT_SRC],
[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AC_ARG_ENABLE(dataport, AC_HELP_STRING([--enable-dataport], [enables the dataport support @<:@default=yes@:>@]),
		[
		case "$enable_dataport" in 
			yes)	DATAPORT="yes";;
			*)	DATAPORT="no";;
		esac], 
		[DATAPORT="yes"])
	if test "x$DATAPORT" = "xyes" ; then
		case "$target" in
			ia64-*-linux-* | \
			x86_64-*-linux-* | \
        		i[[3456]]86-*-linux-* |\
        		i[[3456]]86-*-linux |\
			i[[3456]]86-*-cygwin*|\
        		m68k-*-linux-* |\
            		*-*-solaris* | *-*-sun*-* |\
			*-*-darwin* |\
			i386-*-freebsd* |\
			ia64-*-freebsd* |\
            		*-*-hp*-*)
				DATAPORTUNIX="yes" ;;
            		*-*-OS?-*)      
				DATAPORTUNIX="no" ;;
            		*)              
				DATAPORT="no";;			
		esac
	fi
	AM_CONDITIONAL(BUILD_DATAPORT, test x$DATAPORT = xyes)
	AM_CONDITIONAL(DATAPORT_UNIX, test x$DATAPORTUNIX = xyes)
])

AC_DEFUN([AC_FUNC_DBM_FETCH],
	[AC_CACHE_CHECK([whether dbm_fetch declaration needs parameters], 
		   ac_cv_func_dbm_fetch_void,
	[AC_TRY_COMPILE(
[
#include <ndbm.h>
],
[
   DBM *dbm;
   datum key; 
   dbm_fetch(dbm, key);
   exit 0;
], 
[ac_cv_func_dbm_fetch_void=no],
[ac_cv_func_dbm_fetch_void=yes])]
    if test $ac_cv_func_dbm_fetch_void = no ; then
	AC_DEFINE(DBM_FETCH_VOID, 1, 
		[Define if the C++ compiler supports K&R syntax.])
    fi
)])

AC_DEFUN([TACO_CHECK_RPC],
[
AC_CHECK_HEADERS([rpc.h rpc/rpc.h])
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
#	define xp_sock xp_fd
#else
#	error "Can't find xp_sock"
#endif
#endif])
])

AC_DEFUN([TACO_CHECK_RPC_AND_THREADS],
[
AC_CHECK_HEADERS([socket.h sys/socket.h rpc.h rpc/rpc.h])
LIBS_SAVE="$LIBS"
PTHREAD_LIBS=""
AC_SEARCH_LIBS(pthread_create, [pthread c_r nsl])
if test $ac_cv_search_pthread_create != "none required" ; then
	PTHREAD_LIBS="$ac_cv_search_pthread_create"
fi
AC_SEARCH_LIBS(pthread_mutex_unlock, [], [], [$PTHREAD_LIBS])
AC_SUBST([PTHREAD_LIBS])
AC_RUN_IFELSE(
	AC_LANG_PROGRAM([
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#if HAVE_SOCKET_H
#	include <socket.h>
#elif HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
#else
#	error "Can't find socket.h"
#endif
#if HAVE_RPC_RPC_H
#       include <rpc/rpc.h>
#elif HAVE_RPC_H
#       include <rpc.h>
#else
#	error "Can't find rpc.h"
#endif
#include <arpa/inet.h>
#include <netdb.h>

#define PROGNUM 1234
#define VERSNUM 1
#define PROCNUM 1
#define PROCQUIT 2

static int exitcode;

struct rpc_arg
{
  CLIENT *client;
  u_long proc;
};

static void
dispatch(struct svc_req *request, SVCXPRT *xprt)
{
  svc_sendreply(xprt, (xdrproc_t)xdr_void, 0);
  if (request->rq_proc == PROCQUIT)
    exit (0);
}

static void
test_one_call (struct rpc_arg *a)
{
  struct timeval tout = [{ 60, 0 }];
  enum clnt_stat result;

  printf ("test_one_call: ");
  result = clnt_call (a->client, a->proc,
		      (xdrproc_t) xdr_void, 0,
		      (xdrproc_t) xdr_void, 0, tout);
  if (result == RPC_SUCCESS)
    puts ("success");
  else
    {
      clnt_perrno (result);
      putchar ('\n');
      exitcode = 1;
    }
}

static void *
thread_wrapper (void *arg)
{
  struct rpc_arg a;

  a.client = (CLIENT *)arg;
  a.proc = PROCNUM;
  test_one_call (&a);
  a.client = (CLIENT *)arg;
  a.proc = PROCQUIT;
  test_one_call (&a);
  return 0;
}

const char hw[[[]]] = "Hello World!\n";],
[
  pthread_t tid;
  pid_t pid;
  int err;
  SVCXPRT *svx;
  CLIENT *clnt;
  struct sockaddr_in sin;
  struct timeval wait = [{ 5, 0 }];
  int sock = RPC_ANYSOCK;
  struct rpc_arg a;
  struct hostent *ht;

  printf(hw);
  svx = svcudp_create (RPC_ANYSOCK);
  svc_register (svx, PROGNUM, VERSNUM, dispatch, 0);

  pid = fork ();
  if (pid == -1)
    {
      perror ("fork");
      return 1;
    }
  if (pid == 0)
    svc_run ();

/*
 * inet_pton is not defined for solaris 7 ...
 * inet_pton (AF_INET, "127.0.0.1", &sin.sin_addr);
 */
  ht = gethostbyname("127.0.0.1");
  if (ht == NULL) return 1;
  memcpy ( (char *)&sin.sin_addr, ht->h_addr, (size_t) ht->h_length );


  sin.sin_port = htons (svx->xp_port);
  sin.sin_family = AF_INET;

  clnt = clntudp_create (&sin, PROGNUM, VERSNUM, wait, &sock);

  a.client = clnt;
  a.proc = PROCNUM;

  /* Test in this thread */
  test_one_call (&a);

  /* Test in a child thread */
  err = pthread_create (&tid, 0, thread_wrapper, (void *) clnt);
  if (err)
    fprintf (stderr, "pthread_create: %s\n", strerror (err));
  err = pthread_join (tid, 0);
  if (err)
    fprintf (stderr, "pthread_join: %s\n", strerror (err));

  return exitcode;
])
,
AC_MSG_NOTICE([Running]),
AC_MSG_FAILURE([Not Running]))
LIBS="$LIBS_SAVE"
])

AC_DEFUN([LINUX_DISTRIBUTION],
[
        AC_REQUIRE([AC_CANONICAL_SYSTEM])
        case "$target" in
                i[[3456]]86-*-linux-* | i[[3456]]86-*-linux)
			if test -f /etc/lsb-release ; then
				DISTRIBUTION=`(. /etc/lsb-release; echo $DISTRIB_DESCRIPTION)`
			else
				DISTRIBUTION=`cat /etc/*-release | head -1`
			fi
			;;
	esac
	AC_SUBST([DISTRIBUTION])
])

AC_DEFUN([WITH_CORBA],
[
#	AC_REQUIRE([AC_PATH_XTRA])
	corba_found=no;
	AC_ARG_WITH(corba, AC_HELP_STRING([--with-corba@<:@=ARG@:>@], [CORBA @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the omniORB CORBA installation, e.g. '/usr/local/omniORB']), [
		case  "$with_corba" in
			yes) 	CORBA_LIBS="-lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread" 
				CORBA_INCLUDES="-I$withval/include"
				corba_found=yes;;
			no)  	AC_MSG_WARN([Disable build of TANGO])
				corba_found=no;;
			*) 	CORBA_LIBS="-L$withval/lib -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread"
				CORBA_INCLUDES="-I$withval/include"
				corba_found=yes;;
		esac      
		],[CORBA_LIBS="-lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread"; CORBA_INCLUDES="-I$withval/include"])
	CPPFLAGS_SAVE="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $CORBA_INCLUDES"
	LIBS_SAVE="$LIBS"
#	AC_CHECK_HEADERS([omniORB4/CORBA.h], [
#	AC_CHECK_LIB(omniORB4, init_server, [], [corba_found=no], [$CORBA_LDFLAGS -lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread])
#		break;
#	], [corba_found=no])
	CPPFLAGS="$CPPFLAGS_SAVE"
	LIBS="$LIBS_SAVE"

	AC_SUBST(CORBA_LIBS)
	AC_SUBST(CORBA_INCLUDES)
])

AC_DEFUN([WITH_TANGO],
[
#	AC_REQUIRE([AC_PATH_XTRA])
	tango_found=no;
	AC_ARG_WITH(tango, AC_HELP_STRING([--with-tango@<:@=ARG@:>@], [TANGO @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the TANGO installation, e.g. '/usr/local/tango']), [
		case  "$with_tango" in
			yes) 	TANGO_LIBS="-ltango -llog4tango" 
				TANGO_INCLUDES="-I$withval/include" 
				tango_found=yes;;
			no)  	AC_MSG_WARN([Disable build of TANGO])
				tango_found=no;;
			*) 	TANGO_LIBS="-L$withval/lib -ltango -llog4tango"
				TANGO_INCLUDES="-I$withval/include"
				tango_found=yes;;
		esac      
		],[TANGO_LIBS="-ltango -llog4tango"; TANGO_INCLUDES="-I$withval/include"])
	CPPFLAGS_SAVE="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $TANGO_INCLUDES"
	LIBS_SAVE="$LIBS"
#	AC_CHECK_HEADERS([tango.h], [
#	AC_CHECK_LIB(omniORB4, init_server, [], [tango_found=no], [$TANGO_LDFLAGS -ltango])
#		break;
#	], [tango_found=no])
	CPPFLAGS="$CPPFLAGS_SAVE"
	LIBS="$LIBS_SAVE"

	AC_SUBST(TANGO_LIBS)
	AC_SUBST(TANGO_INCLUDES)
])

AC_DEFUN([TACO_CLIENT_TCP],
[
        AC_ARG_ENABLE(client_tcp, AC_HELP_STRING([--enable-client_tcp], [client connections will be TCP by default (if disabled they will be UDP)@<:@default=yes@:>@]),
                [case "${enable_client_tcp}" in
                        yes)    taco_client_tcp=yes;;
                        no)     taco_client_tcp=no;;
                        *)      AC_MSG_ERROR([bad value ${client_tcp} for --enable-client_tcp]);;
                esac], [taco_client_tcp=yes])
        AM_CONDITIONAL(CLIENT_TCP, test "x$taco_client_tcp" = "xyes")
])

