AC_DEFUN([TACO_PYTHON_BINDING],
[
	TACO_PROG_PYTHON(2.1, [yes])
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
	if test -n "$TCLINCLUDE" -a -n "$TCLLIB" -a "$TCLPACKAGE" != "no" ; then
		taco_tcl_binding=yes
	else
		taco_tcl_binding=no
	fi
	AM_CONDITIONAL(TCL_BINDING, test $taco_tcl_binding = yes)
])

AC_DEFUN([TACO_MYSQL_SUPPORT],
[
	AC_ARG_ENABLE(mysqldbm, AC_HELP_STRING(--enable-mysqldbm, [build the database server with mysql support @<:@default=yes@:>@]),
		[case "${enable_mysqldbm}" in
			yes)	taco_mysql=yes;;
			no)	taco_mysql=no;;
			*)	AC_MSG_ERROR([bad value ${enable_mysqldbm} for --enable-mysqldbm]);;
		esac], [taco_mysql=yes])
	if test "x$taco_mysql" = "xyes" ; then
		for i in /usr/include /usr/local/include ; do
			AC_CHECK_FILE($i/mysql/mysql.h, [
				taco_mysql=yes
				MYSQL_LDFLAGS="-L`dirname $i`/lib/mysql/"
				AC_SUBST(MYSQL_LDFLAGS)
				CPPFLAGS="$CPPFLAGS -I$i"
				break],[taco_mysql=no])
		done
		AC_CHECK_HEADERS([mysql/mysql.h], [taco_mysql=yes], [taco_mysql=no])
	fi
	AM_CONDITIONAL(MYSQLSUPPORT, test "x$taco_mysql" = "xyes")
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
		GRETA=greta_ndbm
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
			yes)	ascii=yes;;
			no)	ascii=no;;
			*)	AC_MSG_ERROR([bad value ${enable_ascii} for --enable-ascii]);;
		esac], [ascii=yes])
	AC_CHECK_HEADERS([dlfcn.h], [], [
		AC_CHECK_HEADERS([dl.h], [], [ascii=no])])
	if test "x$ascii" = "xyes" ; then
		TACO_ASCII_LIBS="\$(top_builddir)/lib/ascii/libascapi.la \$(top_builddir)/lib/tc/libtcapi.la"
	fi
	AC_SUBST(TACO_ASCII_LIBS)
	AM_CONDITIONAL(ASCII_BUILD, test "x$ascii" = "xyes")
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
	if test "x$xdevmenu" = "xno" ; then
		ascii=yes
		X_AND_MOTIF
		XDEVMENU=xdevmenu
	fi
	AM_CONDITIONAL(ASCII_BUILD, test "x$ascii" = "xyes")
	if test "x$motif_found" != "xyes" -o "x$ascii" != "xyes" ; then
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
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -DFreeBSD -DNDBM" ;;
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

AC_DEFUN([AC_FIND_GDBM],
[
	MAKE_GDBM=no
	AC_ARG_WITH(gdbm, AC_HELP_STRING([--with-gdbm=ARG], [ ARG is the path to the gdbm installation, e.g. '/usr/local/gdbm']),
		[
		TACO_GDBM_INC=-I${withval}/include
		TACO_GDBM_LDFLAGS=-L${withval}/lib,
		])
	LIBS_ORIG="$LIBS"
	CPPFLAGS_ORIG="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $TACO_GDBM_INC" 
	LIBS="$LIBS $TACO_GDBM_LIBS"
	gdbm_header=gdbm.h
	for i in /usr/include /usr/local/include /usr/include/gdbm /usr/local/include/gdbm ; do
		AC_CHECK_FILE($i/gdbm.h, [MAKE_GDBM=no;CPPFLAGS="$CPPFLAGS -I$i";break],[MAKE_GDBM=yes])
		if test "$MAKE_GDBM" = "no" ; then
			break
		fi
	done
	AC_CHECK_HEADER(gdbm.h, [gdbm_header="gdbm.h"], [MAKE_GDBM="yes"])
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
)], [AC_CHECK_LIB(gdbm, gdbm_open)], [], [MAKE_GDBM="yes"])
	if test "$MAKE_GDBM" = "yes" ;then
		AC_MSG_RESULT([no])
	else
		AC_MSG_RESULT([yes])
	fi
	AC_LANG_POP(C++)
	if test x${ac_cv_lib_gdbm_gdbm_open} != xyes ; then
		TACO_GDBM_LIBS="\$(top_builddir)/gdbm/libgdbm.la"
		TACO_GDBM_INC="-I\$(top_builddir)/gdbm"
		MAKE_GDBM="yes"
	else
		AC_SUBST(TACO_GDBM_LIBS, ["-lgdbm"])
	fi
	AM_CONDITIONAL(BUILD_GDBM, test x${MAKE_GDBM} = xyes)
	AC_SUBST(TACO_GDBM_INC)
	AC_SUBST(TACO_GDBM_LDFLAGS)
	AC_SUBST(MAKE_GDBM)	
	LIBS="$LIBS_ORIG"
	CXXFLAGS="$CXXFLAGS_ORIG"
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
