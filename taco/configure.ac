AC_INIT(TACO, 3.0-rc2, [taco-devel@lists.sourceforge.net])
AC_PREREQ(2.57)
AC_REVISION($Revision: 1.19 $)dnl
AC_COPYRIGHT([(C) 1990-2003 ESRF])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([config.h.in])

TACO_INIT([no])

LIB_VERSION="10:0:0"
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

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/socket.h termios.h unistd.h])

AC_LANG_PUSH(C++)
AC_CHECK_HEADERS(sstream, [], [])
AC_CHECK_HEADERS(strstream, [], [])
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
#	include	<sys/socket.h>
#elif HAVE_SOCKET_H
#	include <socket.h>
#elif HAVE_NETDB_H
#	include <netdb.h>
#elif HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif])
AC_CHECK_TYPES(time_t)


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
AC_CHECK_FUNCS([alarm dup2 gethostbyname gethostname getpass gettimeofday isascii mblen memset putenv select socket strcasecmp strchr strcspn strdup strerror strncasecmp strrchr strspn strstr strtoul])

AC_SEARCH_LIBS(dlopen, [dl])

TACO_CHECK_RPC
TACO_CHECK_RPC_AND_THREADS

AC_SUBST(QT_INCLUDES, ['-I/usr/lib/qt3/include'])
AC_SUBST(UIC, ['/usr/lib/qt3/bin/uic'])
AC_SUBST(MOC, ['/usr/lib/qt3/bin/moc'])
AC_SUBST(QT_LDFLAGS, ['-L/usr/lib/qt3/lib'])
AC_SUBST(LIBSM, ['-lSM -lICE'])
AC_SUBST(LIB_X11, ['-lX11 $(LIBSOCKET)'])
AC_SUBST(LIB_QT, ['-lqt $(LIBPNG) $(LIBJPEG) -lXext $(LIB_X11) $(LIBSM)'])
AC_SUBST(X_LDFLAGS, ['-L/usr/X11R6/lib'])

AC_SUBST(appdefaultdir)

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
AC_SUBST(pythondir, ['${prefix}/python'])

dnl aclocaldir=`aclocal --print-ac-dir`
AC_SUBST([aclocaldir], ['${datadir}/m4'])

AC_SUBST(TACO_INCLUDES, ["-I\$(top_srcdir)/lib/xdr -I\$(top_srcdir)/lib -I\$(top_srcdir)/lib/dc -I\$(top_srcdir)/lib/dbase -I\$(top_srcdir)/lib/tc -I\$(top_srcdir)/lib/ascii] -I\$(top_srcdir)/lib/private") 

AC_FIND_GDBM
if test "${MAKE_GDBM}" = "yes" ; then
     AC_CONFIG_SUBDIRS(gdbm)
fi

AC_SUBST(TACO_SERVER_LIBS, ["\$(top_builddir)/lib/libdsapi.la \$(top_builddir)/lib/dbase/libdbapi.la \$(top_builddir)/lib/xdr/libdsxdr.la"])
AC_SUBST(TACO_SERVER_CXX_LIBS, ["\$(top_builddir)/lib/libdsapig++.la \$(top_builddir)/lib/dbase/libdbapi.la \$(top_builddir)/lib/xdr/libdsxdr.la"])
AC_SUBST(TACO_CLIENT_LIBS, ["\$(top_builddir)/lib/libtacoapi.la \$(top_builddir)/lib/dbase/libdbapi.la \$(top_builddir)/lib/xdr/libdsxdr.la"])
AC_SUBST(TACO_CLIENT_CXX_LIBS, ["\$(top_builddir)/lib/libtacoapig++.la \$(top_builddir)/lib/dbase/libdbapi.la \$(top_builddir)/lib/xdr/libdsxdr.la"])

TACO_DEFINES
TACO_DATAPORT_SRC
TACO_PYTHON_BINDING
TACO_TCL_BINDING
TACO_MYSQL_SUPPORT
TACO_GRETA
TACO_XDEVMENU
TACO_ALARM

AC_SUBST([RPM_RELEASE], 0)
AM_RPM_INIT
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

AC_CONFIG_FILES(Makefile 
	  misc/Makefile
	  misc/taco-config
	  misc/taco
	  misc/tacoenv.sh
	  misc/taco.startup
	  misc/taco.startup.sec
	  misc/.taco.csh
	  misc/.taco_env
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
	  system/xdevmenu/doc/Makefile
	  system/starter/Makefile
	  system/alarm/Makefile
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

AC_CONFIG_FILES(m4/Makefile)

AC_CONFIG_FILES(bindings/python/Makefile bindings/python/lib/Makefile bindings/python/doc/Makefile bindings/python/demo.server/Makefile)

AC_CONFIG_FILES(bindings/Makefile bindings/tcl/Makefile)

AC_CONFIG_FILES(bindings/labview/Makefile
					bindings/labview/client/Makefile
					bindings/labview/server/Makefile)

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

	Motif:			${motif_found}
	
build:
	gdbm:			${MAKE_GDBM}
	dataport API:		${DATAPORT}
	data collector API:	${taco_dc}
	ASCII API:		${ascii}
	database server with:
		DBM support:	yes
		mySQL support:	${taco_mysql}
	greta:			${greta}
	xdevmenu:		${xdevmenu}
	S_Alarm:		${alarm}
	bindings:
		python :	${taco_python_binding}
		TCL:		${taco_tcl_binding}

Please check whether the configuration I detected matches what you
would like to have. 
"
