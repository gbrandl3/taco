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
	taco_python_binding="yes"
	swig_tmp="no"
	AC_ARG_ENABLE(python, AC_HELP_STRING(--enable-python, [build the binding for Python @<:@default=${taco_python_binding}@:>@]),
                [AS_CASE(["${enable_python}"],
                        [yes], [taco_python_binding=yes],
                        [no], [taco_python_binding=no],
                        [AC_MSG_ERROR([bad value ${enable_python} for --enable-python])])
                ], [taco_python_binding=yes])
	AS_IF([test x"${taco_python_binding}" = x"yes"],
	[
		AC_MSG_NOTICE([Hallo])
		AM_PATH_PYTHON([2.1],[],[taco_python_binding=no])
		AC_MSG_NOTICE([Python])
		dnl AX_PYTHON(2.0, [yes])
		AS_IF([test x"${ax_python_bin}" = x"no" -o x"${ax_python_header}" = x"no" -o x"${ax_python_lib}" = x"no"],
			[taco_python_binding=no], [taco_python_binding=yes])
		AX_PYTHON_DEVEL
		AS_IF([test -z "${PYTHON_VERSION}"], [taco_python_binding=no])
		AS_IF([test "x${taco_python_binding}" = "xyes"],
		[
			ac_save_CPPFLAGS="${CPPFLAGS}"
			AS_IF([test "x${PYTHON}" != "x"],
			[
				NUMPY_INCLUDES=`${PYTHON} -c "import numpy; print numpy.lib.utils.get_include();" 2>/dev/null`
				AS_IF([test "x${NUMPY_INCLUDES}" != "x"],
				[
					PYTHON_CPPFLAGS="${PYTHON_CPPFLAGS} -I${NUMPY_INCLUDES}"
					AC_SUBST([PYTHON_CPPFLAGS])
				])
			])
			CPPFLAGS="${CFLAGS} ${PYTHON_CPPFLAGS}"
			AC_CHECK_HEADERS([numarray/arrayobject.h Numeric/arrayobject.h numpy/arrayobject.h],
				[taco_python_binding=yes; break;],
				[taco_python_binding=no], [#include <Python.h>])
			CPPFLAGS="${ac_save_CPPFLAGS}"
			AX_PKG_SWIG([1.3.29], [swig_tmp="yes"], [swig_tmp="no"])
			AS_IF([test x"${swig_tmp}" = x"yes" -a x"${taco_python_binding}" = x"yes"],
			[
				AX_SWIG_ENABLE_CXX
				AX_SWIG_MULTI_MODULE_SUPPORT
				AX_SWIG_PYTHON([yes])
				ac_save_CXXFLAGS="${CXXFLAGS}"
				CXXFLAGS="${PYTHON_CPPFLAGS} ${CXXFLAGS}"
				AC_CHECK_TYPES(Py_ssize_t, [], [
					AC_CHECK_TYPES(ssize_t, [], [])], [#include <Python.h>])
				CXXFLAGS="${ac_save_CXXFLAGS}"
			])
		])
	])
	AS_IF([test x"${enable_server}" != x"yes"], [taco_python_binding=no])
	AM_CONDITIONAL(BUILD_PYTHON, [test x"${swig_tmp}" = x"yes" -a x"${taco_python_binding}" = x"yes"])
	AC_MSG_NOTICE([Build Python bindings ... ${taco_python_binding}])
	AM_CONDITIONAL(PYTHON_BINDING, [test x"${taco_python_binding}" = x"yes"])
])

AC_DEFUN([TACO_TCL_BINDING],
[
	AC_REQUIRE([TCL_DEVEL])
	AS_IF([test -n "${TCLINCLUDE}" -a -n "${TCLLIB}" -a "${TCLPACKAGE}" != "no"], [taco_tcl_binding=yes], [taco_tcl_binding=no])
	AM_CONDITIONAL(TCL_BINDING, test ${taco_tcl_binding} = yes)
])

AC_DEFUN([TACO_LABVIEW_BINDING],
[
	taco_labview_binding="yes"
	AC_ARG_ENABLE(labview, AC_HELP_STRING(--enable-labview, [build the binding for LabView @<:@default=${taco_labview_binding}@:>@]),
                [AS_CASE(["${enable_labview}"],
                        [yes], [taco_labview_binding=yes],
                        [no], [taco_labview_binding=no],
                        [AC_MSG_ERROR([bad value ${enable_labview} for --enable-labview])])
                ], [taco_labview=yes])
        AC_ARG_WITH(labview, AS_HELP_STRING([--with-labview=PFX], [Prefix where labview is installed, @<:@default=/usr/local/lv71@:>@]),
                [labview_prefix="${withval}"], [labview_prefix="/usr/local/lv71"])
	ac_save_CPPFLAGS="${CPPFLAGS}"
	CPPFLAGS="${CPPFLAGS} -I${labview_prefix}/cintools"
	AS_IF([test x"${taco_labview_binding}" = x"yes"],
	      [
		AC_CHECK_HEADERS([extcode.h], [taco_labview_binding=yes
			AC_SUBST([LABVIEW_INCLUDES], ["-I${labview_prefix}/cintools"])
			AC_SUBST([LABVIEW_LIBS], ["-L${labview_prefix}/cintools -llv"])
			],[taco_labview_binding=no])
	      ])
	CPPFLAGS="${ac_save_CPPFLAGS}"
	AM_CONDITIONAL(LABVIEW_BINDING, [test "${taco_labview_binding}" = "yes"])
])

AC_DEFUN([TACO_DBM_SERVER],
[
	TACO_MYSQL_SUPPORT
	TACO_GDBM_SUPPORT
	TACO_SQLITE3_SUPPORT
	AM_CONDITIONAL(MYSQLSUPPORT, test "x${enable_mysql}" = "xyes")
dnl disable gdbm support of the dbm server if mysql support of database server is disabled
	AS_IF([test x"${enable_mysql}" != x"yes" -a x"${enable_sqlite3}" != x"yes"], [enable_gdbm=yes])
dnl disable the build of gdbm if gdbm support of database server is disabled
	AS_IF([test "x${enable_gdbm}" = "xno" -a "x${taco_build_gdbm}" = "xyes"], [taco_build_gdbm=no])
	AM_CONDITIONAL(GDBMSUPPORT, test "x${enable_gdbm}" = "xyes")
	AM_CONDITIONAL(BUILD_GDBM, test "x${taco_build_gdbm}" = "xyes")
	AC_SUBST(GDBM_CFLAGS)
	AC_SUBST(GDBM_LIBS)
	AC_SUBST(GDBM_COMPAT_LIBS)
])

AC_DEFUN([TACO_SQLITE3_SUPPORT],
[
	AC_ARG_ENABLE(sqlite3, AC_HELP_STRING(--enable-sqlite3, [build the database server with SQLite3 support @<:@default=yes@:>@]),
		[], [enable_sqlite3=yes])	
	AS_IF([test x"${enable_sqlite3}" = x"yes"],
	      [
		PKG_CHECK_MODULES(SQLITE3, sqlite3 >= 3.0.0,
		[
		SQLITE3_CLFAGS=`${PKG_CONFIG} --cflags sqlite3`
		SQLITE3_LIBS=`${PKG_CONFIG} --libs-only-l sqlite3`
		SQLITE3_LDFLAGS=`${PKG_CONFIG} --libs-only-L sqlite3`
		save_CFLAGS="${CFLAGS}"
		save_CPPFLAGS="${CPPFLAGS}"
		CFLAGS="${CFLAGS} ${SQLITE3_CFLAGS}"
		CPPFLAGS="${CPPFLAGS} ${SQLITE3_CFLAGS}"
		AC_CHECK_HEADERS([sqlite3.h])
		CFLAGS="${save_CFLAGS}"
		CPPFLAGS="${save_CPPFLAGS}"
		], [enable_sqlite3=no])
	      ])
	AC_SUBST([SQLITE3_CFLAGS], [${SQLITE3_CFLAGS}])
	AC_SUBST([SQLITE3_LIBS], [${SQLITE3_LIBS}])
	AC_SUBST([SQLITE3_LDFLAGS], [${SQLITE3_LDFLAGS}])
	AM_CONDITIONAL(SQLITE3SUPPORT, test x"${enable_sqlite3}" = x"yes")
])

AC_DEFUN([TACO_MYSQL_SUPPORT],
[
dnl
dnl Get the cflags and libraries
dnl
	AC_PATH_TOOL(MYSQL_CONFIG, mysql_config)
	AC_ARG_ENABLE(mysql, AC_HELP_STRING(--enable-mysql, [build the database server with mysql support @<:@default=yes@:>@]),
		[], [enable_mysql=yes])
	AC_ARG_WITH(mysql, AS_HELP_STRING([--with-mysql=PFX], [Prefix where mysql is installed, e.g. '/usr/local/mysql']),
		[mysql_prefix="${withval}"], [mysql_prefix=""])
	AC_ARG_WITH(mysql-libraries, AS_HELP_STRING([--with-mysql-libraries=DIR], [Directory where mysql library is installed (optional)]),
		[mysql_libraries="${withval}"], [mysql_libraries=""])
	AC_ARG_WITH(mysql-includes, AS_HELP_STRING([--with-mysql-includes=DIR], [Directory where mysql header files are installed (optional)]),
		[mysql_includes="${withval}"], [mysql_includes=""])

	AS_IF([test "x${enable_mysql}" = "xyes"],
              [
		AS_IF([test "x${mysql_libraries}" != "x"], [MYSQL_LIBS="-L${mysql_libraries} -lmysqlclient -lm -lz"],
		      [test "x${mysql_prefix}" != "x"], [MYSQL_LIBS="-L${mysql_prefix}/lib/mysql -lmysqlclient -lm -lz"],
		      [test -n "${MYSQL_CONFIG}"], [MYSQL_LIBS=`${MYSQL_CONFIG} --libs`],
		      [test "x${prefix}" != "xNONE" -a "x${prefix}" != "x/usr"], [MYSQL_LIBS="-L${prefix}/lib -lmysqlclient -lm -lz"])

		AS_IF([test "x${mysql_includes}" != "x"], [MYSQL_CFLAGS="-I${mysql_includes}"],
		      [test "x${mysql_prefix}" != "x"], [MYSQL_CFLAGS="-I${mysql_prefix}/include/mysql"],
		      [test -n "${MYSQL_CONFIG}"], [MYSQL_CFLAGS=`${MYSQL_CONFIG} --cflags`],
		      [test "x${prefix}" != "xNONE" -a "x${prefix}" != "x/usr"], [MYSQL_CFLAGS="-I${prefix}/include"])

		save_LIBS="${LIBS}"
		save_CPPFLAGS="${CPPFLAGS}"
		CPPFLAGS="${CPPFLAGS} ${MYSQL_CFLAGS}"
		LIBS="${LIBS} ${MYSQL_LIBS}"
		enable_mysql=no
		AC_CHECK_HEADERS([mysql/mysql.h mysql.h], [enable_mysql=yes])
		AS_IF([test "x${enable_mysql}" = "xyes"], [
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

])], [], [enable_mysql=no])
		])
		LIBS="${save_LIBS}"
		CPPFLAGS="${save_CPPPLAGS}"
	      ])
	AC_SUBST(MYSQL_CFLAGS)
	AC_SUBST(MYSQL_LIBS)
])

AC_DEFUN([TACO_SERVER_T],
[
	AC_ARG_ENABLE(server, AC_HELP_STRING([--enable-server], [build the libraries for the TACO servers @<:@default=yes@:>@]),
		[], [enable_server=yes])
	AM_CONDITIONAL(BUILD_SERVER, [test x"${enable_server}" = x"yes"])
])

AC_DEFUN([TACO_DATABASE_SERVER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(dbserver, AC_HELP_STRING([--enable-dbserver], [build the TACO database server @<:@default=yes@:>@]),
		[], [enable_dbserver=yes])
	AS_IF([test x"${enable_server}" != x"yes"], [enable_dbserver=no])
	AS_IF([test x"${enable_dbserver}" = x"yes"], [TACO_DBM_SERVER],
	      [
		AM_CONDITIONAL(MYSQLSUPPORT, [false])
		AM_CONDITIONAL(GDBMSUPPORT, [false])
		AM_CONDITIONAL(SQLITE3SUPPORT, [false])
		AM_CONDITIONAL(BUILD_GDBM, [false])
		taco_build_gdbm=no
	      ])
	AM_CONDITIONAL(BUILD_DATABASESERVER, [test x"${enable_dbserver}" = x"yes"])
	AS_IF([test x"${enable_sqlite3}" = x"yes"], [AC_SUBST([TACO_DATABASE], [sqlite3])],
	      [test x"${enable_mysql}" = x"yes"], [AC_SUBST([TACO_DATABASE], [mysql])],
	      [test x"${enable_gdbm}" = x"yes"], [AC_SUBST([TACO_DATABASE], [dbm])],
	      [AC_SUBST([TACO_DATABASE], [no])])
])

AC_DEFUN([TACO_MANAGER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(manager, AC_HELP_STRING([--enable-manager], [build the TACO manager @<:@default=yes@:>@]),
		[], [enable_manager=yes])
	AS_IF([test x"${enable_server}" != x"yes"], [enable_manager=no])
	AM_CONDITIONAL(BUILD_MANAGER, [test x"${enable_manager}" = x"yes"])
])

AC_DEFUN([TACO_MESSAGE_SERVER],
[
	AC_REQUIRE([TACO_SERVER_T])
	AC_ARG_ENABLE(message, AC_HELP_STRING([--enable-message], [build the TACO message server @<:@default=yes@:>@]),
		[], [enable_message=yes])
	AS_IF([test x"${enable_server}" != x"yes"], [enable_message=no])
	AM_CONDITIONAL(BUILD_MESSAGESERVER, [test x"${enable_message}" = x"yes"])
])

AC_DEFUN([TACO_DC_API],
[
	AC_ARG_ENABLE(dc, AC_HELP_STRING([--enable-dc], [build the data collector API @<:@default=yes@:>@]),
		[], [enable_dc=yes])
	AM_CONDITIONAL(DC_BUILD, test "x${enable_dc}" = "xyes")
#	AS_IF([test "x${enable_dc}" = "xyes"], [
#		TACO_DC_LIBS="\$(top_builddir)/lib/dc/libdcapi.la"
#		TACO_DC_LIBS=
#	      ])
#	AC_SUBST(TACO_DC_LIBS)
	AC_ARG_ENABLE(tango_poller, AC_HELP_STRING([--enable-tango_poller], [enable the data collector to poll tango devices @<:@default=no@:>@]),
		[], [enable_tango_poller=no])
	AM_CONDITIONAL(TANGO_POLLER, test "x${enable_tango_poller}" = "xyes")
])

AC_DEFUN([TACO_TANGO],
[
	AC_ARG_ENABLE(tango, AC_HELP_STRING([--enable-tango], [build the TANGO access @<:@default=no@:>@]),
		[], [enable_tango=no])
	AM_CONDITIONAL(TANGO_BUILD, test "x${enable_tango}" = "xyes")
	AS_IF([test "x${enable_tango}" = "xyes"], [
		AC_REQUIRE([WITH_CORBA])
		AC_REQUIRE([WITH_TANGO])
		TACO_TANGO_LIBS="\$(top_builddir)/lib/tango/libtacotango++.la"
	      ])
	AC_SUBST(TACO_TANGO_LIBS)
])


AC_DEFUN([TACO_GRETA],
[
	AC_REQUIRE([TACO_DC_API])
	AC_REQUIRE([X_AND_MOTIF])
	AC_ARG_ENABLE(greta, AC_HELP_STRING([--enable-greta], [build the graphical dbase tool @<:@default=yes@:>@]),
		[], [enable_greta=yes])
	AS_IF([test "x${motif_found}" != "xyes" -o "x${enable_dc}" != "xyes"], [enable_greta=no])
	AM_CONDITIONAL(GRETABUILD, test "x${enable_greta}" = "xyes")
	AC_SUBST([GRETA], ["greta greta_ndbm"])
])

AC_DEFUN([TACO_ASCII_API],
[
	AC_ARG_ENABLE(ascii, AC_HELP_STRING([--enable-ascii], [build the ASCII api @<:@default=yes@:>@]),
		[AS_CASE(["${enable_ascii}"],
			 [yes],	[taco_ascii=yes],
			 [no],	[taco_ascii=no],
			 [AC_MSG_ERROR([bad value ${enable_ascii} for --enable-ascii])])
		], [taco_ascii=yes])
	AC_CHECK_HEADERS([dlfcn.h], [], [
		AC_CHECK_HEADERS([dl.h], [], [taco_ascii=no])])
	AS_IF([test "x${taco_ascii}" = "xyes"], [TACO_ASCII_LIBS="\$(top_builddir)/lib/ascii/libascapi.la \$(top_builddir)/lib/tc/libtcapi.la"])
	AC_SUBST(TACO_ASCII_LIBS)
	AM_CONDITIONAL(ASCII_BUILD, test "x${taco_ascii}" = "xyes")
])

AC_DEFUN([TACO_XDEVMENU],
[
	AC_ARG_ENABLE(xdevmenu, AC_HELP_STRING([--enable-xdevmenu], [build the graphical ds tool @<:@default=yes@:>@]),
		[], [enable_xdevmenu=yes])
	AS_IF([test "x${enable_xdevmenu}" = "xyes"], [
		AC_REQUIRE([TACO_ASCII_API])
		AC_REQUIRE([X_AND_MOTIF])
		taco_ascii=yes
		XDEVMENU=xdevmenu
	      ])
	AM_CONDITIONAL(ASCII_BUILD, test "x${taco_ascii}" = "xyes")
	AS_IF([test "x${motif_found}" != "xyes" -o "x${taco_ascii}" != "xyes"], [enable_xdevmenu=no])
	AM_CONDITIONAL(XDEVMENUBUILD, test "x${enable_xdevmenu}" = "xyes")
	AC_SUBST(XDEVMENU)
])

AC_DEFUN([TACO_ALARM],
[
	AC_REQUIRE([X_AND_MOTIF])
	AC_ARG_ENABLE(alarm, AC_HELP_STRING([--enable-alarm], [build the graphical alarm tool @<:@default=yes@:>@]),
		[], [enable_alarm=yes])
	AS_IF([test "x${enable_alarm}" = "xyes"], [
		AC_REQUIRE([X_AND_MOTIF])
		ALARM=alarm
	      ])
	AS_IF([test "x${motif_found}" != "xyes"], [enable_alarm=no])
	AM_CONDITIONAL(ALARMBUILD, test "x${enable_alarm}" = "xyes")
	AC_SUBST(ALARM)
])

AC_DEFUN([X_AND_MOTIF],
[
	AC_REQUIRE([AC_PATH_XTRA])

	PKG_CHECK_MODULES(APPDEFS, xt, [], [true])
	PKG_CHECK_EXISTS([xt],
		[xt_appdefaultdir=`pkg-config --variable=appdefaultdir xt`],
		AS_IF([test -z "${X_LDFLAGS}" -o x"${X_LDFLAGS}" = xNO], [xt_appdefaultdir="/usr/share/X11/app-defaults"],
	      	[
			tmp='for i in ${X_LDFLAGS} ; do if test `echo $i | cut -c1-2` = "-L" ; then echo $i; break; fi; done | cut -c3- '
			tmp=`eval ${tmp}`
			AS_IF([test -z "${tmp}" -o x"${tmp}" = xNO], [tmp=/usr/share])
			xt_appdefaultdir=${tmp}/X11/app-defaults
	      	]))
	AC_ARG_WITH(appdefaultdir,
		AC_HELP_STRING([--with-appdefaultdir=<pathname>],
		  	[specify directory for app-defaults files (default is autodetected)]),
			[appdefaultdir="${withval}"], [appdefaultdir="${xt_appdefaultdir}"])
 	AC_SUBST(appdefaultdir)
	motif_found=yes;
	AC_ARG_WITH(motif, AC_HELP_STRING([--with-motif@<:@=ARG@:>@], [Motif @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the Motif installation, e.g. '/usr/local/myMotif']), [
		AS_CASE(["${with_motif}"],
			[yes], [MOTIF_LIBS="-lXm"
				MOTIF_INCLUDES="${X_CFLAGS}"],
			[no],  [AC_MSG_WARN([Disable build of greta/xdevmenu])
				motif_found=no],
			[MOTIF_LIBS="-L${withval}/lib -lXm"
			 MOTIF_INCLUDES="-I${withval}/include"])
		],[MOTIF_LIBS="-lXm"; MOTIF_INCLUDES="${X_CFLAGS}"])
	CPPFLAGS_SAVE="${CPPFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${MOTIF_INCLUDES}"
	LIBS_SAVE="${LIBS}"
dnl	X_CFLAGS="${CPPFLAGS}"
dnl	X_LDFLAGS="${LIBS} ${X_LDFLAGS} ${MOTIF_LIBS} ${X_LIBS}"
	AC_CHECK_HEADERS([Xm/Xm.h Xm/XmAll.h], [
		AC_CHECK_LIB(Xm, XmStringCreateLocalized, [], [motif_found=no], [${X_LIBS} -lXt -lX11 -lSM -lICE])
		break;
	], [motif_found=no])
	CPPFLAGS="${CPPFLAGS_SAVE}"
	LIBS="${LIBS_SAVE}"

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
		[], [enable_gdbm=yes])
	AC_ARG_WITH(gdbm, AS_HELP_STRING([--with-gdbm=PFX], [Prefix where gdbm is installed, e.g. '/usr/local/gdbm']),
		[gdbm_prefix="${withval}"], [gdbm_prefix=""])
	AC_ARG_WITH(gdbm-libraries, AS_HELP_STRING([--with-gdbm-libraries=DIR], [Directory where gdbm library is installed (optional)]),
		[gdbm_libraries="${withval}"], [gdbm_libraries=""])
	AC_ARG_WITH(gdbm-includes, AS_HELP_STRING([--with-gdbm-includes=DIR], [Directory where gdbm header files are installed (optional)]),
		[gdbm_includes="${withval}"], [gdbm_includes=""])

	AS_IF([test "x${gdbm_libraries}" != "x"], [GDBM_LIBS="-L${gdbm_libraries}"],
	      [test "x${gdbm_prefix}" != "x"], [GDBM_LIBS="-L${gdbm_prefix}/lib"],
	      [test "x${prefix}" != "xNONE" -a "x${prefix}" != "x/usr"], [GDBM_LIBS="-L${prefix}/lib"])
	TACO_GDBM_LDFLAGS="${GDBM_LIBS}"

	GDBM_LIBS="${GDBM_LIBS} -lgdbm"
	GDBM_COMPAT_LIBS="-lgdbm_compat"

	AS_IF([test "x${gdbm_includes}" != "x"], [GDBM_CFLAGS="-I${gdbm_includes}"],
	      [test "x${gdbm_prefix}" != "x"], [GDBM_CFLAGS="-I${gdbm_prefix}/include"],
	      [test "x${prefix}" != "xNONE" -a "x${prefix}" != "x/usr"], [GDBM_CFLAGS="-I${prefix}/include"])
	TACO_GDBM_INC="${GDBM_CFLAGS}"

	save_LIBS="${LIBS}"
	save_CPPFLAGS="${CPPFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${GDBM_CFLAGS}"
	LIBS="${LIBS} ${GDBM_LIBS}"
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
	AS_IF([test "${taco_build_gdbm}" = "yes"], AC_MSG_RESULT([no]), AC_MSG_RESULT([yes]))
	AC_LANG_POP(C++)
	AS_IF([test x${ac_cv_lib_gdbm_gdbm_open} != xyes],
	      [
		GDBM_LIBS="\$(top_builddir)/gdbm/libgdbm.la"
		GDBM_CFLAGS="-I\$(top_srcdir)/gdbm -I\$(top_builddir)/gdbm"
		GDBM_COMPAT_LIBS="\$(top_builddir)/gdbm/libgdbm_compat.la"
		taco_build_gdbm="yes"
	      ])
	LIBS="${save_LIBS}"
	CPPFLAGS="${save_CPPPLAGS}"
]
)

AC_DEFUN([TACO_DATAPORT_SRC],
[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AC_ARG_ENABLE(dataport, AC_HELP_STRING([--enable-dataport], [enables the dataport support @<:@default=yes@:>@]),
		[], [enable_dataport="yes"])
	AS_IF([test "x${enable_dataport}" = "xyes"],
	      [
		AS_CASE(["${target}"],
			[arm*-*-linux* | ia64-*-linux* | x86_64-*-linux* | i[[3456]]86-*-linux-* | i[[3456]]86-*-linux |\
			powerpc-*-linux-* | m68k-*-linux-* |\
			i[[3456]]86-*-cygwin* | i[[3456]]86-*-mingw* |\
            		*-*-solaris* | *-*-sun*-* |\
			*-*-darwin* |\
			i386-*-freebsd* | ia64-*-freebsd* | amd64-*-freebsd* | x86_64-*-freebsd* | *-*-hp*-*], [DATAPORTUNIX="yes"],
			[*-*-OS?-*], [DATAPORTUNIX="no"],
			[DATAPORT="no"])
	      ])
	AM_CONDITIONAL(BUILD_DATAPORT, test "x${enable_dataport}" = "xyes")
	AM_CONDITIONAL(DATAPORT_UNIX, test x${DATAPORTUNIX} = xyes)
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
    AS_IF([test x${ac_cv_func_dbm_fetch_void} = xno],
	AC_DEFINE(DBM_FETCH_VOID, 1,
		[Define if the C++ compiler supports K&R syntax.]))
)])

AC_DEFUN([DISABLE_TANGO],
	[enable_tango=no
	 AC_MSG_WARN([Disable build of TANGO])])

AC_DEFUN([WITH_CORBA],
[
	AC_REQUIRE([AC_PATH_XTRA])
	AC_ARG_WITH(corba, AC_HELP_STRING([--with-corba@<:@=ARG@:>@], [CORBA @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the omniORB CORBA installation, e.g. '/usr/local/omniORB']), [
		AS_CASE([${with_corba}],
			[yes], [],
			[no],  [],
			[with_corba=yes
			CORBA_LIBS="-lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread"
			CORBA_LDFLAGS="-L${withval}/lib "
			CORBA_CFLAGS="-I${withval}/include"])
		],[with_corba=yes])
	AS_IF([test x"${with_corba}" = x"yes"],
              [
		AS_IF([test -z "${CORBA_CFLAGS}"],
			[
			PKG_CHECK_MODULES(CORBA, [omniCOSDynamic4 >= 4.1.0], [],
			[
				PKG_CHECK_MODULES(CORBA, [omniDynamic4 >= 4.1.0], [],
				[
				CORBA_LIBS="-lomniORB4 -lomniDynamic4 -lCOS4 -lomnithread"
				])
			])])

		AC_MSG_NOTICE([CORBA : ${CORBA_LIBS} ${CORBA_LDFLAGS} ${CORBA_CFLAGS}])
		save_CFLAGS="${CFLAGS}"
		save_CPPFLAGS="${CPPFLAGS}"
		save_LDFLAGS="${LDFLAGS}"
		save_LIBS="${LIBS}"
		LDFLAGS="${LDFLAGS} ${CORBA_LDFLAGS}"
		CFLAGS="${CFLAGS} ${CORBA_CFLAGS}"
		CPPFLAGS="${CPPFLAGS} ${CORBA_CFLAGS}"
		LIBS="${LIBS} ${CORBA_LIBS}"
		AC_LANG_PUSH(C++)
		AC_CHECK_HEADERS([omniORB4/CORBA.h],
			AC_LINK_IFELSE(
				[AC_LANG_PROGRAM([[#include <omniORB4/CORBA.h>]], [[CORBA::ORB_var orb;]])],
				[],
				[with_corba=no
				AC_MSG_NOTICE([1])
				DISABLE_TANGO]),
			[with_corba=no
			AC_MSG_NOTICE([2])
			DISABLE_TANGO])
		CFLAGS="${save_CFLAGS}"
		CPPFLAGS="${save_CPPFLAGS}"
		LDFLAGS="${save_LDFLAGS}"
		LIBS="${save_LIBS}"
		AC_LANG_POP(C++)
	      ],
	      [ AC_MSG_NOTICE([3])
		DISABLE_TANGO
		]
	)
	AC_SUBST([CORBA_CFLAGS])
	AC_SUBST([CORBA_LIBS])
	AC_SUBST([CORBA_LDFLAGS])
])

AC_DEFUN([WITH_TANGO],
[
#	AC_REQUIRE([AC_PATH_XTRA])
	AC_REQUIRE([WITH_CORBA])
	AC_ARG_WITH(tango-libraries,
		AS_HELP_STRING([--with-tango-libraries=DIR], [Directory where TANGO libraries are installed (optional)]),
                [TANGO_LDFLAGS=-L${withval}; tango_libraries=-L${withval}], [tango_libraries=""])
	AC_ARG_WITH(tango-includes,
		AS_HELP_STRING([--with-tango-includes=DIR], [Directory where TANGO headers are installed (optional)]),
		[TANGO_CFLAGS=-I${withval}; tango_includes=-I${withval}], [tango_includes=""])
	AC_ARG_WITH(tango, AC_HELP_STRING([--with-tango@<:@=ARG@:>@], [TANGO @<:@ARG=yes@:>@ ARG may be 'yes', 'no', or the path to the TANGO installation, e.g. '/usr/local/tango']), [
		AS_CASE(["${with_tango}"],
			[yes],[],
			[no], [],
			[with_tango=yes
			TANGO_LIBS="-ltango -llog4tango"
			TANGO_LDFLAGS="-L${withval}/lib"
			TANGO_CFLAGS="-I${withval}/include"])
	])
	AC_MSG_NOTICE([with_tango : ${with_tango}])
	AS_IF([test x"${enable_tango}" = x"yes"], [
		AC_MSG_NOTICE([TANGO : ${TANGO_LIBS} ${TANGO_LDFLAGS} ${TANGO_CFLAGS}])
		AS_IF([test -z "${TANGO_CFLAGS}" -a -z "${TANGO_LDFLAGS}"],
			[
			PKG_CHECK_MODULES(TANGO, [tango >= 7.2.0],
			[
				TANGO_CFLAGS=`${PKG_CONFIG} --cflags tango`
				TANGO_LIBS=`${PKG_CONFIG} --libs-only-l tango`
				TANGO_LDFLAGS=`${PKG_CONFIG} --libs-only-L tango`
			],
			[
				TANGO_LIBS="-ltango -llog4tango"
			])])

		AC_MSG_NOTICE([TANGO : ${TANGO_LIBS} ${TANGO_LDFLAGS} ${TANGO_CFLAGS}])
		save_CFLAGS="${CFLAGS}"
		save_CPPFLAGS="${CPPFLAGS}"
		save_LDFLAGS="${LDFLAGS}"
		save_LIBS="${LIBS}"
		LDFLAGS="${LDFLAGS} ${CORBA_LDFLAGS} ${TANGO_LDFLAGS}"
		CFLAGS="${CFLAGS} ${CORBA_CFLAGS} ${TANGO_CFLAGS}"
		CPPFLAGS="${CPPFLAGS} ${CORBA_CFLAGS} ${TANGO_CFLAGS}"
		LIBS="${LIBS} ${CORBA_LIBS} ${TANGO_LIBS}"
		AC_CHECK_HEADERS([tango.h],
			AC_LINK_IFELSE(
				[AC_LANG_PROGRAM(
					[[#include <tango.h>]], [[Tango::Util *tg; tg->server_init(false);]])],
					[], [enable_tango=no],
					[${CORBA_LDFLAGS} ${CORBA_LIBS} ${TANGO_LDFLAGS} ${TANGO_LIBS}]),
					[enable_tango=no])
		CPPFLAGS="${CPPFLAGS_SAVE}"
		LIBS="${LIBS_SAVE}"
	      ],
	      [AC_MSG_NOTICE([4])
		DISABLE_TANGO]
	)
	AC_SUBST(TANGO_LIBS)
	AC_SUBST(TANGO_CFLAGS)
	AC_SUBST(TANGO_LDFLAGS)
])

AC_DEFUN([TACO_CLIENT_TCP],
[
        AC_ARG_ENABLE(client_tcp, AC_HELP_STRING([--enable-client_tcp], [client connections will be TCP by default (if disabled they will be UDP)@<:@default=yes@:>@]),
                [], [enable_client_tcp=yes])
	AH_TEMPLATE(CLIENT_TCP, [define to one if the clients should be run TCP mode as default])
	AS_IF(test "x${enable_client_tcp}" = "xyes", AC_DEFINE(CLIENT_TCP), [])
        AM_CONDITIONAL(CLIENT_TCP, test "x${enable_client_tcp}" = "xyes")
])

AC_DEFUN([TACO_FRM_EXT],
[
        AC_ARG_ENABLE(ext, AC_HELP_STRING([--enable-ext], [build the extensions libraries servers @<:@default=no@:>@]),
                [], [enable_ext=no])
	AS_CASE(["${enable_ext}"], ["yes"], [
		CFLAGS="${CFLAGS} -DTACO_EXT"
		CXXFLAGS="${CXXFLAGS} -DTACO_EXT"
		AC_CHECK_TYPES([struct timespec])
dnl		AS_CASE(["${enable_python}"], ["yes"], AX_SWIG_PYTHON)
		])
        AM_CONDITIONAL(BUILD_EXT, [test x"${enable_ext}" = x"yes"])
])

AC_DEFUN([TACO_CHECK_RPC_AND_THREADS],
[
	AC_REQUIRE([TACO_CHECK_RPC])
	AC_CHECK_HEADERS([socket.h sys/socket.h])
	LIBS_SAVE="${LIBS}"
	PTHREAD_LIBS=""
	AC_SEARCH_LIBS(pthread_create, [pthread c_r nsl])
	AS_IF([test x"${ac_cv_search_pthread_create}" != x"none required"], [PTHREAD_LIBS="${ac_cv_search_pthread_create}"])
	AC_SEARCH_LIBS(pthread_mutex_unlock, [${PTHREAD_LIBS}])
	AC_SUBST([PTHREAD_LIBS])
	AS_IF([test x"${cross_compiling}" != x"yes" -a x"${target}" != x"i686-pc-mingw32" ],
		[AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
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

const char hw[[]] = "Hello World!\n";],
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
	]), AC_MSG_NOTICE([Running]), AC_MSG_FAILURE([Not Running])])])
	LIBS="${LIBS_SAVE}"
])

