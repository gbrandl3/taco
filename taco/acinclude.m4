AC_DEFUN(X_AND_MOTIF,[
AC_REQUIRE([AC_PATH_XTRA])
AC_ARG_WITH(motif,[  --with-motif            Motif default=yes],[
	case  "$withval" in
	yes)
		MOTIF_LIBS="-lXm" ;;
	no)
		AC_MSG_ERROR([You need Motif/Lesstif for Greta]) ;;
	*)
		MOTIF_LIBS="-L$withval/lib -lXm"
		MOTIF_INCLUDES="-I$withval/include" ;;
	esac      
	],[ MOTIF_LIBS="-lXm"])

appdefaultdir="/usr/X11/lib/X11/app-defaults"
AC_SUBST(MOTIF_LIBS)
AC_SUBST(MOTIF_INCLUDES)
])

AC_DEFUN(AC_TACO_DEFINES,
	[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	case "$target" in
            i[[3456]]86-*-linux* | i[[3456]]86-*-cygwin*)
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

AC_DEFUN(AC_DATAPORT_SRC,
	[
 	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	case "$target" in
            i[[3456]]86-*-linux-* |\
	    i[[3456]]86-*-cygwin*|\
            m68k-*-linux-* |\
            *-*-solar*-* | *-*-sun*-* |\
            *-*-hp*-*)
			DATAPORT="dataport" 
			DATAPORTUNIX="yes" ;;
            *-*-OS?-*)      
			DATAPORT="dataport" 
			DATAPORTUNIX="no" ;;
            *)              
			DATAPORT="";;			
	esac

	AM_CONDITIONAL(BUILD_DATAPORT, test x$DATAPORT = xdataport)
	AM_CONDITIONAL(DATAPORT_UNIX, test x$DATAPORTUNIX = xyes)
	]
)

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
