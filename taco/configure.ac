AC_INIT(TACO, 3.0-beta, [taco-devel@lists.sourceforge.net])
AC_PREREQ(2.53)
AC_REVISION($Revision: 1.8 $)dnl
AC_COPYRIGHT([(C) 1990-2003 ESRF])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(lib/dev_api.c)

TACO_INIT([no])

LIB_VERSION="10.0.0"
AC_SUBST(LIB_VERSION)

AM_INIT_AUTOMAKE([1.7])

AM_PROG_LIBTOOL

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)

LIBS_SAVE="$LIBS"
LIBS="$LIBS -lpthread"
AC_RUN_IFELSE(
	AC_LANG_PROGRAM([
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
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

AC_LANG_PUSH(C++)
AC_CHECK_HEADERS(sstream, [], [])
AC_CHECK_HEADERS(strstream, [], [])
AC_LANG_POP(C++)

AC_SUBST(QT_INCLUDES, ['-I/usr/lib/qt2/include'])
AC_SUBST(UIC, ['/usr/lib/qt2/bin/uic'])
AC_SUBST(MOC, ['/usr/lib/qt2/bin/moc'])
AC_SUBST(QT_LDFLAGS, ['-L/usr/lib/qt2/lib'])
AC_SUBST(LIBSM, ['-lSM -lICE'])
AC_SUBST(LIB_X11, ['-lX11 $(LIBSOCKET)'])
AC_SUBST(LIB_QT, ['-lqt $(LIBPNG) $(LIBJPEG) -lXext $(LIB_X11) $(LIBSM)'])
AC_SUBST(X_LDFLAGS, ['-L/usr/X11R6/lib'])

AC_SUBST(appdefaultdir)

pythondir='${prefix}/python'

aclocaldir=`aclocal --print-ac-dir`
AC_SUBST(aclocaldir)
sbindir='${exec_prefix}'/system/bin/${taco_install_path}
AC_SUBST(sbindir)
AC_SUBST(datadir, ['${prefix}/etc'])
AC_SUBST(docdir, ['${prefix}/doc'])
AC_SUBST(etcdir)
AC_SUBST(templatedir, ['${prefix}/templates/taco'])
AC_SUBST(resdatadir, ['${prefix}/dbase/res'])
AC_SUBST(dbmdir, ['${prefix}/dbase/dbm'])
AC_SUBST(initdir)
AC_SUBST(CMDS_resdir, ['${resdatadir}/CMDS'])
AC_SUBST(ERROR_resdir, ['${resdatadir}/ERROR'])
AC_SUBST(CLASS_resdir, ['${resdatadir}/CLASS'])
AC_SUBST(EVENTS_resdir, ['${resdatadir}/EVENTS'])
AC_SUBST(NAMES_resdir, ['${resdatadir}/NAMES'])
AC_SUBST(PS_NAMES_resdir, ['${resdatadir}/PS_NAMES'])
AC_SUBST(SYS_resdir, ['${resdatadir}/SYS'])
AC_SUBST(SEC_resdir, ['${resdatadir}/SEC'])
AC_SUBST(TEST_resdir, ['${resdatadir}/TEST'])

TACO_INCLUDES="-I\$(top_srcdir)/lib/xdr -I\$(top_srcdir)/lib -I\$(top_srcdir)/lib/dc -I\$(top_srcdir)/lib/dbase\
		-I\$(top_srcdir)/lib/tc -I\$(top_srcdir)/lib/ascii" 

AC_SEARCH_LIBS(dlopen, [dl])

AC_SUBST(TACO_INCLUDES) 

AC_FIND_GDBM
if test "${MAKE_GDBM}" = "yes" ; then
     AC_CONFIG_SUBDIRS(gdbm)
fi

TACO_SERVER_LIBS="\$(top_builddir)/lib/libdsapi.la \
	       \$(top_builddir)/lib/dbase/libdbapi.la \
	       \$(top_builddir)/lib/xdr/libdsxdr.la"
AC_SUBST(TACO_SERVER_LIBS)

TACO_SERVER_CXX_LIBS="\$(top_builddir)/lib/libdsapig++.la \
	       \$(top_builddir)/lib/dbase/libdbapi.la \
	       \$(top_builddir)/lib/xdr/libdsxdr.la"
AC_SUBST(TACO_SERVER_CXX_LIBS)

TACO_CLIENT_LIBS="\$(top_builddir)/lib/libtacoapi.la \
	       \$(top_builddir)/lib/dbase/libdbapi.la \
	       \$(top_builddir)/lib/xdr/libdsxdr.la"
AC_SUBST(TACO_CLIENT_LIBS)

TACO_CLIENT_CXX_LIBS="\$(top_builddir)/lib/libtacoapig++.la \
	       \$(top_builddir)/lib/dbase/libdbapi.la \
	       \$(top_builddir)/lib/xdr/libdsxdr.la"
AC_SUBST(TACO_CLIENT_CXX_LIBS)

AC_SUBST(TACO_CLIENT_LIBS)
TACO_DEFINES
TACO_DATAPORT_SRC
TACO_PYTHON_BINDING
TACO_TCL_BINDING
TACO_MYSQL_SUPPORT
TACO_GRETA
TACO_XDEVMENU

AC_SUBST([RPM_RELEASE], 0)
AM_RPM_INIT
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

AC_CONFIG_FILES(Makefile 
	  misc/Makefile
	  misc/taco-config
	  misc/tacoenv.sh
	  misc/taco.startup
	  misc/taco.startup.sec
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
	  system/starter/Makefile
	  taco.spec
)

dnl AC_CONFIG_FILES(system/dbdev/Makefile
dnl		system/dbdev/src/Makefile
dnl		system/dbdev/doc/Makefile)

AC_CONFIG_FILES(classes/instverify/Makefile)
AC_CONFIG_FILES(classes/powersupply/Makefile)
AC_CONFIG_FILES(classes/serialline/Makefile)

AC_CONFIG_FILES(classes/signal/Makefile 
		classes/signal/doc/Makefile
		classes/signal/multiple/Makefile 
		classes/signal/multiple/doc/Makefile
		classes/signal/single/Makefile
		classes/signal/single/doc/Makefile)

dnl AC_CONFIG_FILES(classes/profibusdp/Makefile
dnl 		classes/profibusdp/lib/Makefile
dnl 		classes/profibusdp/driver/Makefile)

AC_CONFIG_FILES(system/dbase/newserver/Makefile)

AC_CONFIG_FILES(m4/Makefile)

AC_CONFIG_FILES(bindings/python/Makefile bindings/python/lib/Makefile bindings/python/doc/Makefile bindings/python/demo.server/Makefile)

AC_CONFIG_FILES(bindings/Makefile bindings/tcl/Makefile)

dnl Initialize the test suite
AC_CONFIG_TESTDIR(tests)
AC_CONFIG_FILES(tests/Makefile tests/atlocal)
dnl AC_CONFIG_FILES(tests/testserver/Makefile)
AC_CONFIG_FILES(tests/taco, [chmod +x tests/taco])
AM_MISSING_PROG([AUTOM4TE], [autom4te])

AC_OUTPUT

echo "

Configuration (taco):

	Source code location:	${srcdir}
	Version:		${VERSION}
	Compiler:		${CC},${CXX}
	
build:
	gdbm:			${MAKE_GDBM}
	dataport API:		${DATAPORT}
	data collector API:	${dc}
	ASCII API:		${ascii}
	database server with:
		DBM support:	yes
		mySQL support:	${mysql}
	greta:			${greta}
	xdevmenu:		${xdevmenu}
	bindings:
		python :	${taco_python_binding}
		TCL:		${taco_tcl_binding}

Please check whether the configuration I detected matches what you
would like to have. 
"
