AC_DEFUN([TACO_PYTHON_BINDING],
[
	TACO_PROG_PYTHON(2.0, [yes])
	if test "$ac_python_dir" != "no" ; then
		ac_save_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS $PYTHON_INCLUDES"
		AC_CHECK_HEADERS(Numeric/arrayobject.h, [taco_python_binding=yes], [taco_python_binding=no], [#include <Python.h>])
		CFLAGS="$ac_save_CFLAGS"
	else
		taco_python_binding=no
	fi	
	AM_CONDITIONAL(PYTHON_BINDING, test $taco_python_binding = yes)
])

AC_DEFUN([TACO_TCL_BINDING],
[
	AC_REQUIRE([TACO_PROG_TCL])
	if { echo $target | grep darwin ; } then 
		taco_tcl_bindings=no
	elif test -n "$TCLINCLUDE" -a -n "$TCLLIB" -a "$TCLPACKAGE" != "no" ; then
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
	AC_SUBST(MYSQL_CFLAGS)
	AC_SUBST(MYSQL_LIBS)
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
		MYSQL_LIBS="-L$mysql_libraries -lmysqlclient"
	elif test "x$mysql_prefix" != "x" ; then
		MYSQL_LIBS="-L$mysql_prefix/lib -lmysqlclient"
	elif test -n "$MYSQL_CONFIG" ; then
		MYSQL_LIBS=`$MYSQL_CONFIG --libs`
	elif test "x$prefix" != "xNONE" -a "x$prefix" != "x/usr"; then
		MYSQL_LIBS="-L$prefix/lib -lmysqlclient"
	fi

	if test "x$mysql_includes" != "x" ; then
		MYSQL_CFLAGS="-I$mysql_includes"
	elif test "x$mysql_prefix" != "x" ; then
		MYSQL_CFLAGS="-I$mysql_prefix/include"
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

AC_DEFUN([TACO_DC_API],
[
	AC_ARG_ENABLE(dc, AC_HELP_STRING([--enable-dc], [build the data collector API @<:@default=yes@:>@]),
		[case "${enable_dc}" in
			yes)	taco_dc=yes;;
			no)	taco_dc=no;;
			*)	AC_MSG_ERROR([bad value ${enable_dc} for --enable-dc]);;
		esac], [taco_dc=yes])
	AM_CONDITIONAL(DC_BUILD, test "x$taco_dc" = "xyes") 
	if test "x$taco_dc" = "xyes" ; then
		TACO_DC_LIBS="\$(top_builddir)/lib/dc/libdcapi.la"
	fi
	AC_SUBST(TACO_DC_LIBS)
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
	if test "x$greta" = "xyes" ; then
		dc=yes
		X_AND_MOTIF
		GRETA=greta
	fi
	AM_CONDITIONAL(DC_BUILD, test "x$dc" = "xyes") 
	if test "x$dc" = "xyes" ; then
		TACO_DC_LIBS="\$(top_builddir)/lib/dc/libdcapi.la"
	fi
	AC_SUBST(TACO_DC_LIBS)
	if test "x$motif_found" != "xyes" -o "x$dc" != "xyes" ; then 
		greta=no
	fi
	AM_CONDITIONAL(GRETABUILD, test "x$greta" = "xyes") 
	AC_SUBST(GRETA)
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
	appdefaultdir="`echo $X_LDFLAGS | cut -c3- `/X11/app-defaults"
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
	AC_CHECK_HEADERS([Xm/Xm.h Xm/XmAll.h], [
		AC_CHECK_LIB(Xm, XmStringCreateLocalized, [], [motif_found=no], [$X_LDFLAGS -lXt -lX11])
		break;
	], [motif_found=no])
	CPPFLAGS="$CPPFLAGS_SAVE"
	LIBS="$LIBS_SAVE"

	AC_SUBST(MOTIF_LIBS)
	AC_SUBST(MOTIF_INCLUDES)
])

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
            *-*-solar*-* | *-*-sun*-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -D_solaris=1 -D__solaris__=1 -DNDBM" ;;
            *-*-hp*-*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -D__hpux=1 -D__hpux10=1 -D__hp9000s700=1 -D__hpux9000s700=1 -DNDBM" ;;
            *-*-OS?-*)      
                        taco_CFLAGS="-D_UCC=1" ;;
            *)		AC_MSG_ERROR([unknown target : $target])
                        taco_CFLAGS="unknown $target" ;;
        esac
        AC_ARG_ENABLE(multithreading,
	   [  --enable-multithreading allow multithreading default=yes],
 	   [
		if test x$enableval = xno ; then
			extraflags=""
		else
		    extraflags="-D_REENTRANT"
		fi
	   ],
	   [ extraflags="-D_REENTRANT"] 
       )
       taco_CFLAGS="$taco_CFLAGS $extraflags"
       CFLAGS="$CFLAGS $taco_CFLAGS"
       CXXFLAGS="$CXXFLAGS $taco_CFLAGS" 
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
		GDBM_CFLAGS="-I\$(top_builddir)/gdbm"
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
        		i[[3456]]86-*-linux-* |\
        		i[[3456]]86-*-linux |\
			i[[3456]]86-*-cygwin*|\
        		m68k-*-linux-* |\
            		*-*-solar*-* | *-*-sun*-* |\
			i386-*-freebsd* |\
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
AC_SEARCH_LIBS(pthread_cancel, [pthread c_r])
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

  inet_aton ("127.0.0.1", &sin.sin_addr);
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
