dnl Toolkit for building distributed control systems or any other distributed system.
dnl
dnl Copyright (C) 2003-2014  Jens Krüger <jkrueger1@users.sf.net>
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

AC_INIT([TACO], [3.99.179], [taco-devel@lists.sourceforge.net], [], [http://sf.net/projects/taco])
AC_PREREQ([2.62])
AC_REVISION($Revision: 1.134 $)dnl
AC_COPYRIGHT([(C) 1990-2014 ESRF])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([config.h.in])

AC_CONFIG_MACRO_DIR([m4])

TACO_CHECK_SYSTEM
AM_PROG_LIBTOOL
LT_INIT
dnl Debian resets this to no, but this break both Spot and the libtool
dnl test suite itself.  Instead of requiring developer to install a
dnl non-patched version of Libtool on any Debian they use, we just
dnl cancel the effect of Debian's patch here.
link_all_deplibs=yes
link_all_deplibs_CXX=yes

AC_SUBST(LIB_VERSION, [17:0:0])

TACO_CHECK_SIZEOF_TYPES

dnl AC_CHECK_TYPES([int8_t], AC_CHECK_SIZEOF(int8_t))
dnl AC_CHECK_TYPES([uint8_t], AC_CHECK_SIZEOF(uint8_t))
dnl AC_CHECK_TYPES([int16_t], AC_CHECK_SIZEOF(int16_t))
dnl AC_CHECK_TYPES([uint16_t], AC_CHECK_SIZEOF(uint16_t))
dnl AC_CHECK_TYPES([int32_t], AC_CHECK_SIZEOF(int32_t))
dnl AC_CHECK_TYPES([uint32_t], AC_CHECK_SIZEOF(uint32_t))
dnl AC_CHECK_TYPES([int64_t], AC_CHECK_SIZEOF(int64_t))
dnl AC_CHECK_TYPES([uint64_t], AC_CHECK_SIZEOF(uint64_t))

dnl wanted by: classes/binem/code/rebin.c:124
dnl AC_CHECK_FUNCS([floor sqrt]) 
dnl wanted by: lib/config/rsync.h:110
dnl AC_CHECK_HEADERS([limits.h syslog.h utime.h]) 
dnl AC_HEADER_DIRENT 
dnl AC_HEADER_MAJOR 

TACO_CHECK_HOSTNAME
TACO_CHECK_GCC_LIB_PATH
TACO_ENABLE_LONGHOSTNAMES
TACO_ENABLE_DEBUG

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

TACO_CHECK_RPC_AND_THREADS

AC_PATH_QT([3], [0], no)
AM_CONDITIONAL([BUILD_QT], [test x"$have_qt" = x"yes"])

dnl AC_SUBST(QT_INCLUDES, ['-I/usr/lib/qt3/include'])
dnl AC_SUBST(UIC, ['/usr/lib/qt3/bin/uic'])
dnl AC_SUBST(MOC, ['/usr/lib/qt3/bin/moc'])
dnl AC_SUBST(QT_LDFLAGS, ['-L/usr/lib/qt3/lib'])
dnl AC_SUBST(LIBSM, ['-lSM -lICE'])
dnl AC_SUBST(LIB_X11, ['-lX11 $(LIBSM) $(LIBSOCKET)'])
dnl AC_SUBST(LIB_QT, ['-lqt-mt $(LIBPNG) $(LIBJPEG) -lXext $(LIB_X11) $(LIBSM)'])
dnl AC_SUBST(X_LDFLAGS, ['-L/usr/X11R6/lib'])

AC_SUBST(appdefaultdir)

AC_SUBST(docdir,        ['${datadir}/doc/packages/taco'])
AC_SUBST(templatedir,   ['${datadir}/taco/templates'])
AC_SUBST(resdatadir,    ['${datadir}/taco/dbase/res'])
AC_SUBST(dbmdir,        ['${localstatedir}/cache/taco'])
AC_SUBST(logdir,	['${localstatedir}/log/taco'])
AC_SUBST(rundir,        ['${localstatedir}/run'])
AC_SUBST(pkgconfigdir,  ['${libdir}/pkgconfig'])

AC_SUBST(CMDS_resdir,     ['${resdatadir}/CMDS'])
AC_SUBST(ERROR_resdir,    ['${resdatadir}/ERROR'])
AC_SUBST(CLASS_resdir,    ['${resdatadir}/CLASS'])
AC_SUBST(EVENTS_resdir,   ['${resdatadir}/EVENTS'])
AC_SUBST(NAMES_resdir,    ['${resdatadir}/NAMES'])
AC_SUBST(PS_NAMES_resdir, ['${resdatadir}/PS_NAMES'])
AC_SUBST(SYS_resdir,      ['${resdatadir}/SYS'])
AC_SUBST(SEC_resdir,      ['${resdatadir}/SEC'])
AC_SUBST(TEST_resdir,     ['${resdatadir}/TEST'])

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

TACO_SERVER_T
TACO_DATAPORT_SRC
TACO_TANGO
TACO_PYTHON_BINDING
TACO_TCL_BINDING
TACO_LABVIEW_BINDING
TACO_GRETA
TACO_XDEVMENU
TACO_ALARM
TACO_MANAGER
TACO_DATABASE_SERVER
TACO_CLIENT_TCP
TACO_FRM_EXT
AS_IF([test x"${enable_ext}" = x"yes"],
	[enable_python_ext=$swig_tmp],
	[enable_python_ext="no"])
AC_SUBST([PYTHON_EXT_BUILD], [test x"${enable_python_ext}" = x"yes"])

AM_PATH_LOG4CPP([1.0.0], [taco_build_log4cpp=no], [
taco_build_log4cpp=yes
AC_SUBST(LOG4CPP_CFLAGS, ["-I\$(top_srcdir)/log4cpp/src -I\$(top_srcdir)/log4cpp/include -I\$(top_builddir)/log4cpp/include"])
AC_SUBST(LOG4CPP_LIBS, ["\$(top_builddir)/log4cpp/src/liblog4cpp.la"])
])
AM_CONDITIONAL(BUILD_LOG4CPP, test "x$taco_build_log4cpp" = "xyes")
AC_SUBST(LOG4TACO_CFLAGS, ["-I\$(top_srcdir)/lib/log4taco"])

AC_CONFIG_SUBDIRS([gdbm log4cpp])

TACO_MESSAGE_SERVER

dnl the startup routine should not be built in the dsapi library on Mac OS X systems
AM_CONDITIONAL(BUILTIN_STARTUP, ! echo $target|grep darwin)

AC_SUBST([RPM_RELEASE], 0)
AM_RPM_INIT
LINUX_DISTRIBUTION
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

BB_ENABLE_DOXYGEN

AC_CONFIG_FILES(Makefile 
		misc/Makefile
		misc/taco-config
		misc/taco.debian
		misc/tacoenv.sh
		misc/tacoenv.csh
		misc/taco.startup
		misc/taco.shutdown
		misc/taco.status
		misc/taco_log.cfg
		misc/taco.SuSE
		misc/dc.SuSE
		misc/taco-manager.sh
		misc/taco-dc.sh
		doc/Makefile
		doc/Doxyfile
		doc/taco_manual.tex
		doc/taco.sty
		doc/index.doc
		doc/started.doc
		lib/Makefile
		lib/Doxyfile
		lib/dbase/Makefile
		lib/dataport/Makefile
		lib/xdr/Makefile
		lib/dc/Makefile
		lib/ascii/Makefile
		lib/ascii/doc/Makefile
		lib/tc/Makefile
		lib/tango/Makefile
		lib/signal/Makefile
		lib/log4taco/Makefile
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
		system/starter/client/Makefile
		system/alarm/Makefile
		system/testcs/Makefile
		taco.spec
)

AC_CONFIG_FILES([system/tools/Makefile])

dnl AC_CONFIG_FILES(system/dbdev/Makefile
dnl 		system/dbdev/src/Makefile)
dnl		system/dbdev/doc/Makefile)

AC_CONFIG_FILES([classes/instverify/Makefile])
AC_CONFIG_FILES([classes/powersupply/Makefile])
AC_CONFIG_FILES([classes/serialline/Makefile])
AC_CONFIG_FILES([classes/updatedaemon/Makefile])
AC_CONFIG_FILES([classes/agps/Makefile])
dnl AC_CONFIG_FILES([classes/steppermotor/Makefile])
dnl AC_CONFIG_FILES([classes/steppermotor/include/Makefile])
dnl AC_CONFIG_FILES([classes/steppermotor/src/Makefile])
dnl AC_CONFIG_FILES([classes/steppermotor/vpap/Makefile])
dnl AC_CONFIG_FILES([classes/fbus/Makefile])
dnl AC_CONFIG_FILES([classes/fbus/include/Makefile])
dnl AC_CONFIG_FILES([classes/fbus/src/Makefile])
dnl AC_CONFIG_FILES([classes/fbus/utils/Makefile])
dnl AC_CONFIG_FILES([classes/fbus/gescio1/Makefile])
dnl AC_CONFIG_FILES([classes/xbpm/Makefile])
AC_CONFIG_FILES([classes/motor/Makefile])
AC_CONFIG_FILES([
		classes/signal/Makefile 
		classes/signal/single/Makefile 
		classes/signal/multiple/Makefile
		classes/signal/doc/Makefile
		classes/signal/single/doc/Makefile
		classes/signal/multiple/doc/Makefile
		])

AC_CONFIG_FILES([
		examples/Makefile
		examples/helloworld/Makefile
		])

AC_CONFIG_FILES([m4/Makefile])

AC_CONFIG_FILES([
		bindings/python/Makefile 
		bindings/python/lib/Makefile 
		bindings/python/doc/Makefile 
		bindings/python/demo.server/Makefile
		])

AC_CONFIG_FILES(bindings/Makefile bindings/tcl/Makefile)

AC_CONFIG_FILES([
		bindings/labview/Makefile
 		bindings/labview/client/Makefile
 		bindings/labview/server/Makefile
		])

dnl Initialize the test suite
AC_CONFIG_TESTDIR(tests)
AC_CONFIG_FILES(tests/Makefile tests/atlocal)
dnl AC_CONFIG_FILES(tests/testserver/Makefile)
AC_CONFIG_FILES(tests/taco, [
mv tests/taco tests/t && \
sed \
-e's%^exec_prefix=/%exec_prefix=${DESTDIR}/%' \
-e's%^bindir=/%bindir=${DESTDIR}/%' \
-e's%^sbindir=/%sbindir=${DESTDIR}/%' \
-e's%^datadir=/%datadir=${DESTDIR}/%' \
-e's%^templatedir=/%templatedir=${DESTDIR}/%' \
-e's%^appdefaultdir=/%appdefaultdir=${DESTDIR}/%' \
-e's%^localstatedir=/%localstatedir=${DESTDIR}/%' \
-e's%^sysconfdir=/%sysconfdir=${DESTDIR}/%' \
-e's%^libdir=/%libdir=${DESTDIR}/%' \
tests/t > tests/taco \
&& rm tests/t && chmod +x tests/taco
])

AC_CONFIG_FILES([
		ext/Makefile 
		ext/include/Makefile 
		ext/src/Makefile 
		ext/doc/Makefile
		ext/python/Makefile 
		ext/res/Makefile
		])

AC_CONFIG_FILES([
		misc/taco-server.pc 
		misc/taco-server++.pc 
		misc/taco-client.pc 
		misc/taco-client++.pc
		])

AC_CHECK_PROGS([AUTOM4TE], [autom4te autom4te259 autom4te253])
AM_MISSING_PROG([AUTOM4TE], [$AUTOM4TE])

AC_OUTPUT

AS_IF([test x"${enable_dbserver}" = x"yes"],
	[taco_dbserver_support="
	 	with:
		DBM support:	${enable_gdbm}
		mySQL support:	${enable_mysql}
		SQLite3 support:${enable_sqlite3}"
	])

AC_MSG_RESULT([

Configuration (taco):

	Source code location:	${srcdir}
	Version:		${VERSION}
	Compiler:		${ac_ct_CC},${ac_ct_CXX}

	Motif:			${motif_found}
	CORBA:			${with_corba}
	TANGO:			${with_tango}
	
build:
	log4cpp:		${taco_build_log4cpp}
	gdbm:			${taco_build_gdbm}
	manager:		${enable_manager}
	database server:	${enable_dbserver} ${taco_dbserver_support}
	message server:		${enable_message}

	greta:			${enable_greta}
	xdevmenu:		${enable_xdevmenu}
	S_Alarm:		${enable_alarm}

	client tcp :		${enable_client_tcp}
	server API:		${enable_server}
	dataport API:		${enable_dataport}
	data collector API:	${enable_dc}
	TANGO poller:		${enable_tango_poller}
	ASCII API:		${taco_ascii}
	bindings:
		python :	${taco_python_binding} ${using_python_version}
		TCL:		${taco_tcl_binding}
		LabVIEW:	${taco_labview_binding}

	TANGO support:		${enable_tango}
	Extensions :		${enable_ext}
		python :	${enable_python_ext}

install:
	Fast install:           ${enable_fast_install}

Please check whether the configuration I detected matches what you
would like to have. 
])
