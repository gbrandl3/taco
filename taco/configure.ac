AC_INIT(taco, 3.0.0, [goetz@esrf.fr])
AC_PREREQ(2.53)
AC_REVISION($Revision: 1.1 $)dnl
AC_COPYRIGHT([(C) 1990-2003 ESRF])

AC_PREFIX_DEFAULT(${DSHOME:-/usr/local/dshome})
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(lib/gen_api.c)

dnl AC_DISABLE_STATIC
AC_DISABLE_FAST_INSTALL

dnl AM_MAINTAINER_MODE

AC_TACO_DEFINES
AM_INIT_AUTOMAKE([1.6])
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_CXX
AC_PROG_INSTALL
AC_CONFIG_HEADERS(config.h)

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)

AC_CHECK_HEADERS(ctype.h, [], [])
AC_CHECK_HEADERS(errno.h, [], [])

AC_CHECK_HEADERS(netdb.h, [], [])
AC_CHECK_HEADERS(inet/netdb.h, [], [])
AC_CHECK_HEADERS(inet/socket.h, [], [])

AC_CHECK_HEADERS(linux/posix_types.h)
AC_CHECK_HEADERS(linux/types.h)

AC_CHECK_HEADERS(malloc.h, [], [])
AC_CHECK_HEADERS(netinet/in.h, [], [])
AC_CHECK_HEADERS(pwd.h, [], [])
AC_CHECK_HEADERS(rpc/rpc.h, [], [])

AC_CHECK_HEADERS(pingLib.h [], [])
AC_CHECK_HEADERS(grp.h [], [])
AC_CHECK_HEADERS(hostLib.h [], [])
AC_CHECK_HEADERS(_os9_sockets.h [], [])
AC_CHECK_HEADERS(arpa/inet.h [], [])
AC_CHECK_HEADERS(rpcGbl.h [], [])
AC_CHECK_HEADERS(taskLib.h [], [])
AC_CHECK_HEADERS(sys/filio.h, [], [])

AC_CHECK_HEADERS(stdarg.h, [], [])
AC_CHECK_HEADERS(stddef.h, [], [])
AC_CHECK_HEADERS(stdlib.h, [], [])

AC_CHECK_HEADERS(ioctl.h, [], [])
AC_CHECK_HEADERS(sys/ioctl.h, [], [])

AC_CHECK_HEADERS(signal.h, [], [])
AC_CHECK_HEADERS(sys/signal.h, [], [])

AC_CHECK_HEADERS(sys/socket.h, [], [])

AC_CHECK_HEADERS(time.h, [], [])
AC_CHECK_HEADERS(varargs.h, [], [])

dnl Windows specific
AC_CHECK_HEADERS(process.h, [], [])
AC_CHECK_HEADERS(winsock.h, [], [])

dnl AC_LANG_PUSH(C++)
dnl AC_CHECK_HEADERS(sstream, [], [])
dnl AC_CHECK_HEADERS(strstream, [], [])
dnl AC_LANG_POP(C++)

dnl AC_ARG_ENABLE(greta, AC_HELP_STRING([--enable-greta], [build the graphical dbase tool default=no]),
dnl 	[case "${enableval}" in
dnl 		yes)	greta=true;;
dnl 		no)	greta=false;;
dnl 		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-greta);;
dnl 	esac], [greta=false])
dnl AM_CONDITIONAL(GRETABUILD, test "x$greta" = "xtrue") 
dnl if test "x$greta" = "xtrue" ; then
dnl 	X_AND_MOTIF
dnl 	GRETA=greta_ndbm
dnl fi
dnl AC_SUBST(GRETA)

dnl AC_ARG_ENABLE(xdevmenu, AC_HELP_STRING([--enable-xdevmenu], [build the graphical dbase tool default=no]),
dnl 	[case "${enableval}" in
dnl 		yes)	xdevmenu=true;;
dnl 		no)	xdevmenu=false;;
dnl 		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-xdevmenu);;
dnl 	esac], [xdevmenu=false])
dnl AM_CONDITIONAL(XDEVMENUBUILD, test x$xdevmenu = xtrue) 
dnl if test "x$xdevmenu" = "xtrue" ; then
dnl 	X_AND_MOTIF
dnl 	XDEVMENU=xdevmenu
dnl fi
AC_SUBST(XDEVMENU)


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

dnl TACO_INCLUDES="-I\$(top_srcdir)/dserver/classes/main/include \
dnl -I\$(top_srcdir)/dserver/classes++/device/include \
dnl -I\$(top_srcdir)/dserver/classes++/device/oic/include \
dnl -I\$(top_srcdir)/dserver/system/api/admin/include \
dnl -I\$(top_srcdir)/dserver/system/api/apilib/include \
dnl -I\$(top_srcdir)/dserver/system/api/cmds_err/include \
dnl -I\$(top_srcdir)/dserver/system/dataport/include \
dnl -I\$(top_srcdir)/dserver/system/dc/include \
dnl -I\$(top_srcdir)/dserver/system/manager/include \
dnl -I\$(top_srcdir)/dserver/system/msg/include \
dnl -I\$(top_srcdir)/dserver/system/xdr/include \
dnl -I\$(top_srcdir)/dserver/system/dbapi/include \
dnl -I\$(top_srcdir)/dbase/server/include"

dnl AC_SUBST(TACO_INCLUDES) 
dnl AC_CHECK_HEADERS([ndbm.h gdbm/ndbm.h])
dnl AC_FIND_GDBM
dnl AC_FUNC_DBM_FETCH

dnl if test -n "${MAKE_GDBM}" ; then
dnl      AC_CONFIG_SUBDIRS(gdbm-1.7.3)
dnl fi

dnl TACO_LIBSBUILD="\$(top_builddir)/dserver/system/api/apilib/src/libdsapi.la \
dnl 	       \$(top_builddir)/dserver/system/dbapi/src/libdbapi.la \
dnl 	       \$(top_builddir)/dserver/system/xdr/src/libdsxdr.la"
dnl AC_SUBST(TACO_LIBSBUILD)
dnl TACO_LIBS2BUILD="\$(top_builddir)/dserver/system/api/apilib/src/libtacoapi.la \
dnl 	       \$(top_builddir)/dserver/system/dbapi/src/libdbapi.la \
dnl 	       \$(top_builddir)/dserver/system/xdr/src/libdsxdr.la"
dnl AC_SUBST(TACO_LIBSBUILD)
dnl AC_TACO_DEFINES
dnl AC_DATAPORT_SRC

dnl TACO_LIBDCBUILD="\$(top_builddir)/dserver/system/dc/src/libdcapi.la"
dnl AC_SUBST(TACO_LIBDCBUILD)
dnl TACO_LIBXDEVBUILD="\$(top_builddir)/dserver/system/xdevmenu/ascapi/src/libascapi.la \
dnl \$(top_builddir)/dserver/system/xdevmenu/tcapi/src/libtcapi.la"
dnl AC_SUBST(TACO_LIBXDEVBUILD)


dnl RPM_RELEASE=3
dnl AC_SUBST(RPM_RELEASE)
dnl AM_RPM_INIT
dnl AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

AC_CONFIG_FILES(Makefile lib/Makefile)
AC_OUTPUT
