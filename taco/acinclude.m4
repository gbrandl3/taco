AC_DEFUN(TACO_PYTHON_SERVER,
[
	TACO_PROG_PYTHON(2.1, [yes])
	ac_save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $PYTHON_INCLUDES"
	AC_CHECK_HEADERS(Numeric/arrayobject.h, [python_array=yes], [python_array=no], [#include <Python.h>])
	CFLAGS="$ac_save_CFLAGS"
	AM_CONDITIONAL(PYTHON_SERVER_BUILD, test "x$python_array" != "xno")
])

AC_DEFUN(TACO_MYSQL_SUPPORT,
[
	AC_ARG_ENABLE(mysqldbm, AC_HELP_STRING(--enable-mysqldbm, [build the database server with mysql support default=yes]),
		[case "${enable_mysqldbm}" in
			yes)	mysql=yes;;
			*)	mysql=no;;
		esac], [mysql=yes])
	if test "x$mysql" = "xyes" ; then
		AC_CHECK_HEADERS([mysql/mysql.h], [], [mysql=no])
	fi
	AM_CONDITIONAL(MYSQLSUPPORT, test "x$mysql" = "xyes")
])

AC_DEFUN(TACO_DC_API,
[
	AC_ARG_ENABLE(dc, AC_HELP_STRING([--enable-dc], [build the data collector API default=yes]),
		[case "${enable_dc}" in
			yes)	dc=yes;;
			no)	dc=no;;
			*)	AC_MSG_ERROR([bad value ${enable_dc} for --enable-dc]);;
		esac], [dc=yes])
	AM_CONDITIONAL(DC_BUILD, test "x$dc" = "xyes") 
	if test "x$dc" = "xyes" ; then
		TACO_DC_LIBS="\$(top_builddir)/lib/dc/libdcapi.la"
	fi
	AC_SUBST(TACO_DC_LIBS)
])

AC_DEFUN(TACO_GRETA,
[
	AC_REQUIRE([TACO_DC_API])
	AC_ARG_ENABLE(greta, AC_HELP_STRING([--enable-greta], [build the graphical dbase tool default=yes]),
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
	AM_CONDITIONAL(GRETABUILD, test "x$greta" = "xyes") 
	AC_SUBST(GRETA)
])

AC_DEFUN(TACO_ASCII_API,
[
	AC_ARG_ENABLE(ascii, AC_HELP_STRING([--enable-ascii], [build the ASCII api default=yes]),
		[case "${enable_ascii}" in
			yes)	ascii=yes;;
			no)	ascii=no;;
			*)	AC_MSG_ERROR([bad value ${enable_ascii} for --enable-ascii]);;
		esac], [ascii=yes])
	if test "x$ascii" = "xyes" ; then
		TACO_ASCII_LIBS="\$(top_builddir)/lib/ascii/libascapi.la \$(top_builddir)/lib/tc/libtcapi.la"
	fi
	AC_SUBST(TACO_ASCII_LIBS)
	AM_CONDITIONAL(ASCII_BUILD, test "x$ascii" = "xyes")
])

AC_DEFUN(TACO_XDEVMENU,
[
	AC_REQUIRE([TACO_ASCII_API])
	AC_ARG_ENABLE(xdevmenu, AC_HELP_STRING([--enable-xdevmenu], [build the graphical ds tool default=yes]),
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
	AM_CONDITIONAL(XDEVMENUBUILD, test "x$xdevmenu" = "xyes") 
	AC_SUBST(XDEVMENU)
])

AC_DEFUN(X_AND_MOTIF,
[
	AC_REQUIRE([AC_PATH_XTRA])
	AC_ARG_WITH(motif, AC_HELP_STRING([--with-motif], [Motif default=yes]),[
		case  "$with_motif" in
			yes) 	MOTIF_LIBS="-lXm" ;;
			no)  	AC_MSG_WARN([You need Motif/Lesstif for greta/xdevmenu]);;
			*) 	MOTIF_LIBS="-L$withval/lib -lXm"
				MOTIF_INCLUDES="-I$withval/include" ;;
		esac      
		],[MOTIF_LIBS="-lXm"])

	appdefaultdir="/usr/X11/lib/X11/app-defaults"
	AC_SUBST(MOTIF_LIBS)
	AC_SUBST(MOTIF_INCLUDES)
])

AC_DEFUN(TACO_DEFINES,
[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	case "$target" in
            i[[3456]]86-*-linux-* | i[[3456]]86-*-cygwin*)
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -Dx86=1 -DNDBM" ;;
            m68k-*-linux-*) 
                        taco_CFLAGS="-Dunix=1 -D__unix=1 -Dlinux=1 -D68k=1 -DNDBM" ;;
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

AC_DEFUN(AC_FIND_GDBM,
[
	LIBS_ORIG="$LIBS"
	AC_CHECK_LIB(gdbm,gdbm_open)		 
	AM_CONDITIONAL(BUILD_GDBM, test x${ac_cv_lib_gdbm_gdbm_open} = xyes)
	if test x${ac_cv_lib_gdbm_gdbm_open} = xyes ; then
	          TACO_GDBM="-lgdbm"
dnl		this required for RedHat
		  TACO_GDBM_INC="-I/usr/include/gdbm"
		  MAKE_GDBM=""		  	
	else
		  TACO_GDBM="-L\$(top_srcdir)/gdbm-1.7.3 -lgdbm"
		  TACO_GDBM_INC="-I\$(top_srcdir)/gdbm-1.7.3"
		  MAKE_GDBM="gdbm-1.7.3"
	fi
	AC_SUBST(TACO_GDBM)
	AC_SUBST(TACO_GDBM_INC)
	AC_SUBST(MAKE_GDBM)	
	LIBS="$LIBS_ORIG"
]
)

AC_DEFUN(TACO_DATAPORT_SRC,
[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AC_ARG_ENABLE(dataport, AC_HELP_STRING([--enable-dataport], [enables the dataport support default=yes]),
		[
		case "$enable_dataport" in 
			yes)	DATAPORT="yes";;
			*)	DATAPORT="no";;
		esac], 
		[DATAPORT="yes"])
	if test "x$DATAPORT" = "xyes" ; then
		case "$target" in
        		i[[3456]]86-*-linux-* |\
			i[[3456]]86-*-cygwin*|\
        		m68k-*-linux-* |\
            		*-*-solar*-* | *-*-sun*-* |\
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
    [AC_CACHE_CHECK(whether dbm_fetch declaration needs parameters, 
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
