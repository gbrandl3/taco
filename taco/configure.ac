AC_INIT(TACO, 3.0-beta, [taco-devel@lists.sourceforge.net])
AC_PREREQ(2.53)
AC_REVISION($Revision: 1.5 $)dnl
AC_COPYRIGHT([(C) 1990-2003 ESRF])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(lib/dev_api.c)

TACO_INIT([no])

LIB_VERSION="10.0.0"
AC_SUBST(LIB_VERSION)

AM_INIT_AUTOMAKE([1.7])

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)

AC_LANG_PUSH(C++)
AC_CHECK_HEADERS(sstream, [], [])
AC_CHECK_HEADERS(strstream, [], [])
AC_LANG_POP(C++)

AC_SUBST(appdefaultdir)

sbindir='${exec_prefix}/system/bin'
sbindir="$sbindir/$taco_install_path"
datadir='${prefix}/etc'
docdir='${prefix}/doc'
pythondir='${prefix}/python'
templatedir='${prefix}/templates/taco'
resdatadir='${prefix}/dbase/res'
dbmdir='${prefix}/dbase/dbm'
CMDS_resdir='${resdatadir}/CMDS'
ERROR_resdir='${resdatadir}/ERROR'
CLASS_resdir='${resdatadir}/CLASS'
NAMES_resdir='${resdatadir}/NAMES'
PS_NAMES_resdir='${resdatadir}/PS_NAMES'
EVENTS_resdir='${resdatadir}/EVENTS'
SYS_resdir='${resdatadir}/SYS'
SEC_resdir='${resdatadir}/SEC'
TEST_resdir='${resdatadir}/TEST'
aclocaldir=`aclocal --print-ac-dir`
AC_SUBST(aclocaldir)
AC_SUBST(sbindir)
AC_SUBST(datadir)
AC_SUBST(docdir)
AC_SUBST(etcdir)
AC_SUBST(templatedir)
AC_SUBST(resdatadir)
AC_SUBST(dbmdir)
AC_SUBST(initdir)
AC_SUBST(CMDS_resdir)
AC_SUBST(ERROR_resdir)
AC_SUBST(CLASS_resdir)
AC_SUBST(EVENTS_resdir)
AC_SUBST(NAMES_resdir)
AC_SUBST(PS_NAMES_resdir)
AC_SUBST(SYS_resdir)
AC_SUBST(SEC_resdir)
AC_SUBST(TEST_resdir)

TACO_INCLUDES="-I\$(top_srcdir)/lib/xdr -I\$(top_srcdir)/lib -I\$(top_srcdir)/lib/dc -I\$(top_srcdir)/lib/dbase\
		-I\$(top_srcdir)/lib/tc -I\$(top_srcdir)/lib/ascii" 

AC_SUBST(TACO_INCLUDES) 
AC_CHECK_HEADERS([ndbm.h gdbm/ndbm.h])
AC_FIND_GDBM
AC_FUNC_DBM_FETCH

if test -n "${MAKE_GDBM}" ; then
     AC_CONFIG_SUBDIRS(gdbm-1.7.3)
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

AC_CONFIG_FILES(classes/instverify/Makefile)
AC_CONFIG_FILES(classes/powersupply/Makefile)
AC_CONFIG_FILES(classes/serialline/Makefile)

AC_CONFIG_FILES(classes/signal/Makefile 
		classes/signal/doc/Makefile
		classes/signal/multiple/Makefile 
		classes/signal/multiple/doc/Makefile
		classes/signal/single/Makefile
		classes/signal/single/doc/Makefile)

AC_CONFIG_FILES(system/dbase/newserver/Makefile)

AC_CONFIG_FILES(m4/Makefile)

AC_CONFIG_FILES(bindings/python/Makefile bindings/python/lib/Makefile bindings/python/doc/Makefile bindings/python/demo.server/Makefile)

AC_CONFIG_FILES(bindings/Makefile bindings/tcl/Makefile)

dnl Initialize the test suite
AC_CONFIG_TESTDIR(tests)
AC_CONFIG_FILES(tests/Makefile tests/atlocal)
AC_CONFIG_FILES(tests/taco, [chmod +x tests/taco])
AM_MISSING_PROG([AUTOM4TE], [autom4te])

AC_OUTPUT

echo "

Configuration (taco):

	Source code location:	${srcdir}
	Version:		${VERSION}
	Compiler:		${CC},${CXX}
	
build:
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
