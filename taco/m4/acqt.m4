dnl
dnl Module:
dnl		$Id: acqt.m4,v 1.3 2004-02-17 12:27:19 jkrueger1 Exp $
dnl Description:
dnl		autotool macros to get the Qt installation pathes
dnl		adapted form the KDE2 acinclude.m4
dnl Author:
dnl		$Author: jkrueger1 $
dnl		




dnl
dnl AC_MOC_ERROR_MESSAGE
dnl puts a message to user about the missing moc
dnl
AC_DEFUN([QT_MOC_ERROR_MESSAGE],
    [
	AC_MSG_ERROR([No Qt meta object compiler (moc) found!
Please check whether you installed Qt correctly.
You need to have a running moc binary.
configure tried to run $ac_cv_path_moc and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable MOC to the right one before running
configure.]
	)
    ]
)
 
dnl
dnl AC_UIC_ERROR_MESSAGE
dnl puts a message to user about the missing uic
dnl
AC_DEFUN([QT_UIC_ERROR_MESSAGE],
    [
	AC_MSG_WARN([No Qt ui compiler (uic) found!
Please check whether you installed Qt correctly.
You need to have a running uic binary.
configure tried to run $ac_cv_path_uic and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable UIC to the right one before running
configure.]
	)
   ]
)

dnl
dnl AC_PATH_QT_MOC_UIC
dnl gets the pathes to moc and uic, and returns it in the
dnl variables MOC resp. UIC
dnl if failed, the variables contains only a echo statement
dnl
AC_DEFUN([AC_PATH_QT_MOC_UIC],
    [
	qt_bindirs=""
	for dir in $qt_dirs; do
      	    qt_bindirs="$qt_bindirs:$dir/bin:$dir/src/moc"
   	done
   	qt_bindirs="$qt_bindirs:/usr/bin:/usr/X11R6/bin:/usr/local/qt/bin"
   	if test ! "$ac_qt_bindir" = "NO"; then
      	    qt_bindirs="$ac_qt_bindir:$qt_bindirs"
   	fi
 
	AC_PATH_PROG(MOC, moc, [NO], [$qt_bindirs])
	if test "$MOC" = "NO" ; then
	    QT_MOC_ERROR_MESSAGE
       	    MOC="echo moc not available: "
	fi
	AC_PATH_PROG(UIC, uic, [NO], [$qt_bindirs])
   	if test "$UIC" = "NO" ; then
     	    if test -z "$UIC_NOT_NEEDED" ; then
		QT_UIC_ERROR_MESSAGE
     	    else
       		UIC="echo uic not available: "
     	    fi
   	fi

   	AC_SUBST(MOC)
   	AC_SUBST(UIC)
    ]
)

dnl
dnl AC_CHECK_BOOL
dnl checks supporting bool type by c++ compiler and makes an item in 
dnl config.h 
dnl 
AC_DEFUN([AC_CHECK_BOOL],
    [
        AC_MSG_CHECKING([for bool])
        AC_CACHE_VAL(ac_cv_have_bool,
            [
                AC_LANG_CPLUSPLUS
                AC_TRY_COMPILE([],
                 [bool aBool = true;],
                 [ac_cv_have_bool="yes"],
                 [ac_cv_have_bool="no"])
            ]
	) dnl end AC_CHECK_VAL
        AC_MSG_RESULT($ac_cv_have_bool)
        if test "$ac_cv_have_bool" = "yes"; then
            AC_DEFINE(HAVE_BOOL, 1, [Define if the C++ compiler supports BOOL])
        fi
    ]
)

dnl 
dnl
dnl AC_USE_QT[(version,[ subversion, [version string]])]
dnl sets some variables to the required Qt version:
dnl	qtver		= release of Qt
dnl	qtsubver	= revision of Qt
dnl	qt_minversion	= string with the minimum required version of Qt
dnl	qt_verstring	= string with the version of Qt
dnl
AC_DEFUN([AC_USE_QT],
    [
	if test -z "$1"; then
  	    qtver=2
  	    qtsubver=3
	else
  	    qtsubver=`echo "$1" | sed -e 's#[0-9]\+\.\([0-9]\+\).*#\1#'`
# following is the check if subversion isn´t found in passed argument
  	    if test "$qtsubver" = "$1"; then
    		qtsubver=0
  	    fi
  	    qtver=`echo "$1" | sed -e 's#^\([0-9]\+\)\..*#\1#'`
  	    if test "$qtver" = "1"; then
    		qtsubver=42
  	    elif test "$qtver" = "2"; then
    		    qtsubver=3
  	    elif test "$qtver" != "3"; then
		qtver=2
		qtsubver=3	
	    fi
	fi
 
	if test -z "$2"; then
  	    if test $qtver = 2; then
    		case $qtsubver in
      		    1)	qt_minversion=">= 2.1.0";;
      		    2)	qt_minversion=">= 2.2.0";;
      		    3)	qt_minversion=">= 2.3.0";;
      		    *)	qt_minversion=">= 2.0.0";;
    		esac	
	    elif test $qtver = 3; then
		case $qtsubver in
		   1)	qt_minversion=">=3.1.0";;
		   2)	qt_minversion=">=3.2.0";;
		   *)	qt_minversion=">=3.0.0";;
		esac
  	    else
   	 	qt_minversion=">= 1.42 and < 2.0"
  	    fi
	else
	    qtsubver=$2
   	    qt_minversion=">=${qtver}.${qtsubver}.0"
        fi
 
	if test -z "$3"; then
   	    if test $qtver = 2; then
    		case $qtsubver in
      		   1) 	qt_verstring="QT_VERSION >= 210";;
      		   2) 	qt_verstring="QT_VERSION >= 220";;
      		   3) 	qt_verstring="QT_VERSION >= 230";;
      		   *) 	qt_verstring="QT_VERSION >= 200";;
    		esac
	    elif test $qtver = 3; then
		case $qtsubver in
		   1) qt_verstring="QT_VERSION >= 310";;
		   2) qt_verstring="QT_VERSION >= 320";;
		   3) qt_verstring="QT_VERSION >= 300";;
		esac
   	    else
    		qt_verstring="QT_VERSION >= 142 && QT_VERSION < 200"
  	    fi
	else
   	    qt_verstring=$3
	fi
 
	if test $qtver = 3 ; then
	    qt_dirs="$QTDIR /usr/lib/qt3 /usr/lib/qt"
	elif test $qtver = 2; then
   	    qt_dirs="$QTDIR /usr/lib/qt2 /usr/lib/qt"
	else
   	    qt_dirs="$QTDIR /usr/lib/qt"
	fi
    ]
)

dnl 
dnl QT_MISC_TESTS
dnl performs some miscellaneous test to Qt
dnl
AC_DEFUN([QT_MISC_TESTS],
    [
   	AC_LANG_C
dnl Checks for libraries.
   	AC_CHECK_LIB(compat, main, [LIBCOMPAT="-lcompat"]) dnl for FreeBSD
   	AC_SUBST(LIBCOMPAT)
dnl	AC_CHECK_KSIZE_T
dnl   	AC_LANG_C
   	AC_CHECK_LIB(dnet, dnet_ntoa, [X_EXTRA_LIBS="$X_EXTRA_LIBS -ldnet"])
   	if test $ac_cv_lib_dnet_dnet_ntoa = no; then
      	    AC_CHECK_LIB(dnet_stub, dnet_ntoa, [X_EXTRA_LIBS="$X_EXTRA_LIBS -ldnet_stub"])
   	fi
   	AC_CHECK_FUNC(inet_ntoa)
   	if test $ac_cv_func_inet_ntoa = no; then
     	    AC_CHECK_LIB(nsl, inet_ntoa, X_EXTRA_LIBS="$X_EXTRA_LIBS -lnsl")
   	fi
   	AC_CHECK_FUNC(connect)
   	if test $ac_cv_func_connect = no; then
      	    AC_CHECK_LIB(socket, connect, X_EXTRA_LIBS="-lsocket $X_EXTRA_LIBS", , $X_EXTRA_LIBS)
   	fi
 
   	AC_CHECK_FUNC(remove)
   	if test $ac_cv_func_remove = no; then
      	    AC_CHECK_LIB(posix, remove, X_EXTRA_LIBS="$X_EXTRA_LIBS -lposix")
   	fi
   	LIBSOCKET="$X_EXTRA_LIBS"
   	AC_SUBST(LIBSOCKET)
   	AC_SUBST(X_EXTRA_LIBS)
   	AC_CHECK_LIB(ucb, killpg, [LIBUCB="-lucb"]) dnl for Solaris2.4
   	AC_SUBST(LIBUCB)
 
  	case $host in  dnl this *is* LynxOS specific
   	    *-*-lynxos*)
        		AC_MSG_CHECKING([LynxOS header file wrappers])
        			[CFLAGS="$CFLAGS -D__NO_INCLUDE_WARN__"]
        		AC_MSG_RESULT(disabled)
        		AC_CHECK_LIB(bsd, gethostbyname, [LIBSOCKET="-lbsd"]) dnl for LynxOS
         		;;
    	esac
 
dnl   	KDE_CHECK_TYPES
dnl   	KDE_CHECK_LIBDL
	AC_CHECK_BOOL
    ]
)

dnl                                                                                                      
dnl AC_X_PATH
dnl performs some tests to get the Installation of the X system
dnl 
AC_DEFUN([AC_X_PATH],
     [
	AC_REQUIRE([AC_PROG_CPP])dnl
	AC_REQUIRE([QT_MISC_TESTS])dnl

	AC_MSG_CHECKING(for X)
	AC_LANG_SAVE
	AC_LANG_C

	AC_CACHE_VAL(qt_cv_have_x,
	    [
# One or both of the vars are not set, and there is no cached value.
		if test "{$x_includes+set}" = set || test "$x_includes" = NONE; then
   		    qt_x_includes=NO
		else
   		    qt_x_includes=$x_includes
	  	fi
		if test "{$x_libraries+set}" = set || test "$x_libraries" = NONE; then
   		    qt_x_libraries=NO
		else
   		    qt_x_libraries=$x_libraries
		fi
 
# below we use the standard autoconf calls
		ac_x_libraries=$qt_x_libraries
		ac_x_includes=$qt_x_includes
		_AC_PATH_X_DIRECT
dnl AC_PATH_X_XMKMF picks /usr/lib as the path for the X libraries.
dnl Unfortunately, if compiling with the N32 ABI, this is not the correct
dnl location. The correct location is /usr/lib32 or an undefined value
dnl (the linker is smart enough to pick the correct default library).
dnl Things work just fine if you use just AC_PATH_X_DIRECT.
		case "$host" in
		    mips-sgi-irix6*) 
					;;
		    *)
  					_AC_PATH_X_XMKMF
  					if test -z "$ac_x_includes"; then
    					     ac_x_includes="."
  					fi
  					if test -z "$ac_x_libraries"; then
    					     ac_x_libraries="/usr/lib"
  					fi
		esac
#from now on we use our own again

# when the user already gave --x-includes, we ignore
# what the standard autoconf macros told us.
		if test "$qt_x_includes" = NO; then
  		    qt_x_includes=$ac_x_includes
		fi
 
# for --x-libraries too
		if test "$qt_x_libraries" = NO; then
  		    qt_x_libraries=$ac_x_libraries
		fi
 
		if test "$qt_x_includes" = NO; then
  		    AC_MSG_ERROR(
[Can't find X includes. Please check your installation and add the correct paths!])
		fi
 
		if test "$qt_x_libraries" = NO; then
  		    AC_MSG_ERROR(
[Can't find X libraries. Please check your installation and add the correct paths!])
		fi
 
# Record where we found X for the cache.
		qt_cv_have_x="have_x=yes \
         		qt_x_includes=$qt_x_includes qt_x_libraries=$qt_x_libraries"
	   ]
	)dnl

	eval "$qt_cv_have_x"
 
	if test "$have_x" != yes; then
  	    AC_MSG_RESULT($have_x)
  	    no_x=yes
	else
  	    AC_MSG_RESULT([libraries $qt_x_libraries, headers $qt_x_includes])
	fi
 
	if test -z "$qt_x_includes" || test "x$qt_x_includes" = xNONE; then
  	    X_INCLUDES=""
dnl better than nothing 
	    x_includes="."
 	else
  	    x_includes=$qt_x_includes
  	    X_INCLUDES="-I$x_includes"
	fi
 
	if test -z "$qt_x_libraries" || test "x$qt_x_libraries" = xNONE; then
  	    X_LDFLAGS=""
dnl better than nothing 
	    x_libraries="/usr/lib"
 	else
  	    x_libraries=$qt_x_libraries
  	    X_LDFLAGS="-L$x_libraries"
	fi
	all_includes="$X_INCLUDES"
	all_libraries="$X_LDFLAGS"
 
	AC_SUBST(X_INCLUDES)
	AC_SUBST(X_LDFLAGS)
	AC_SUBST(x_libraries)
	AC_SUBST(x_includes)                                                                                      
# Check for libraries that X11R6 Xt/Xaw programs need.
	ac_save_LDFLAGS="$LDFLAGS"
	LDFLAGS="$LDFLAGS $X_LDFLAGS"
# SM needs ICE to (dynamically) link under SunOS 4.x (so we have to
# check for ICE first), but we must link in the order -lSM -lICE or
# we get undefined symbols.  So assume we have SM if we have ICE.
# These have to be linked with before -lX11, unlike the other
# libraries we check for below, so use a different variable.
#  --interran@uluru.Stanford.EDU, kb@cs.umb.edu.
	AC_CHECK_LIB(ICE, IceConnectionNumber,
  		[LIBSM="-lSM -lICE"], , $X_EXTRA_LIBS)
	AC_SUBST(LIBSM)
	LDFLAGS="$ac_save_LDFLAGS"

	LIB_X11='-lX11 $(LIBSOCKET)'
	AC_SUBST(LIB_X11)
	AC_MSG_CHECKING(for libXext)
	AC_CACHE_VAL(qt_cv_have_libXext,
	    [
		qt_ldflags_safe="$LDFLAGS"
		qt_libs_safe="$LIBS"
 
		LDFLAGS="$LDFLAGS $X_LDFLAGS $USER_LDFLAGS"
		LIBS="-lXext -lX11 $LIBSOCKET"
 
		AC_TRY_LINK(
		    [
			#include <stdio.h>
		    ],
		    [
			printf("hello Xext\n");
		    ],
		    [qt_cv_have_libXext=yes],
		    [qt_cv_have_libXext=no]
   		)
 
		LDFLAGS=$qt_ldflags_safe
		LIBS=$qt_libs_safe
 	    ]
	) 
	AC_MSG_RESULT($qt_cv_have_libXext)
	if test "$qt_cv_have_libXext" = "no"; then
  	    AC_MSG_ERROR(
		[
We need a working libXext to proceed. Since configure
can't find it itself, we stop here assuming that make wouldn't find
them either.
	        ]
	    )
	fi

	AC_SUBST(X_PRE_LIBS)
	AC_LANG_RESTORE
    ]
)

dnl
dnl AC_PRINT_QT_PROGRAM
dnl creates a little Qt program dependend on the Qt version to test the Qt 
dnl installation
dnl
AC_DEFUN([AC_PRINT_QT_PROGRAM],
    [
	AC_REQUIRE([AC_USE_QT])
	cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include <qglobal.h>
#include <qapplication.h>
#include <qapp.h>
#include <qobjcoll.h>
EOF
	if test "$qtver" = "2"; then
	    cat >> conftest.$ac_ext <<EOF
#include <qevent.h>
#include <qstring.h>
#include <qstyle.h>
EOF
 
	    if test $qtsubver -gt 0; then
		cat >> conftest.$ac_ext <<EOF
#include <qiconview.h>
EOF
	    fi
 
	fi
 
	echo "#if ! ($qt_verstring)" >> conftest.$ac_ext
	cat >> conftest.$ac_ext <<EOF
#error 1
#endif
 
int main() {
EOF
	if test "$qtver" = "2"; then
	    cat >> conftest.$ac_ext <<EOF
    QStringList *t = new QStringList();
EOF
	    if test $qtsubver -gt 0; then
	    	cat >> conftest.$ac_ext <<EOF
    QIconView iv(0);
    iv.setWordWrapIconText(false);
    QString s;
    s.setLatin1("Elvis is alive", 14);
EOF
	    fi
	    if test $qtsubver -gt 1; then
	    	cat >> conftest.$ac_ext <<EOF
    int magnolia = QEvent::Speech; /* new in 2.2 beta2 */
EOF
	    fi
	fi
	cat >> conftest.$ac_ext <<EOF
    return 0;
}
EOF
    ]
)

dnl
dnl AC_CHECK_QT_DIRECT
dnl tests the generating a Qt program without setting some ld flags
dnl
AC_DEFUN([AC_CHECK_QT_DIRECT],
    [
	AC_REQUIRE([AC_USE_QT])
	AC_MSG_CHECKING([if Qt compiles without flags])
	AC_CACHE_VAL(qt_cv_qt_direct,
	    [
		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS
		ac_LD_LIBRARY_PATH_safe=$LD_LIBRARY_PATH
		ac_LIBRARY_PATH="$LIBRARY_PATH"
		ac_cxxflags_safe="$CXXFLAGS"
		ac_ldflags_safe="$LDFLAGS"
		ac_libs_safe="$LIBS"
 
		CXXFLAGS="$CXXFLAGS -I$qt_includes"
		LDFLAGS="$LDFLAGS $X_LDFLAGS"
		LIBS="$LIBQT -lXext -lX11 $LIBSOCKET"
		LD_LIBRARY_PATH=
		export LD_LIBRARY_PATH
		LIBRARY_PATH=
		export LIBRARY_PATH
 
		AC_PRINT_QT_PROGRAM
 
		if AC_TRY_EVAL(ac_link) && test -s conftest; then
  		    qt_cv_qt_direct="yes"
		else
  		    qt_cv_qt_direct="no"
  		    echo "configure: failed program was:" >&AC_FD_CC
  		    cat conftest.$ac_ext >&AC_FD_CC
		fi
 
		rm -f conftest*
		CXXFLAGS="$ac_cxxflags_safe"
		LDFLAGS="$ac_ldflags_safe"
		LIBS="$ac_libs_safe"
 
		LD_LIBRARY_PATH="$ac_LD_LIBRARY_PATH_safe"
		export LD_LIBRARY_PATH
		LIBRARY_PATH="$ac_LIBRARY_PATH"
		export LIBRARY_PATH
		AC_LANG_RESTORE
	    ]
	)
 
	if test "$qt_cv_qt_direct" = "yes"; then
  	    AC_MSG_RESULT(yes)
  	    $1
	else
  	    AC_MSG_RESULT(no)
  	    $2
	fi
    ]
)

dnl 
dnl AC_PATH_QT(current, revision, [abort=yes])
dnl tests and gets the installation of the required version of Qt
dnl exports following variables:
dnl	qt_libraries	= library path to the Qt libraries
dnl	qt_includes	= include path to the Qt headers
dnl	QT_INCLUDES	= include directives according to Qt
dnl	QT_LDFLAGS	= ld flags according to Qt
dnl	LIB_QT		= name of the Qt lib
dnl
AC_DEFUN([AC_PATH_QT],
    [
	AC_REQUIRE([AC_X_PATH])dnl
	AC_USE_QT([$1], [$2])dnl
	qt_was_given=yes
	if test -z "$LIBQT"; then
  	    LIBQT="-lqt"
  	    qt_was_given=no
  	    int_qt="-lqt"
	else
  	    int_qt="$LIBQT"
	fi
	if test $qtver = 2; then
dnl  	    AC_REQUIRE([AC_FIND_PNG])
dnl  	    AC_REQUIRE([AC_FIND_JPEG])
  	    LIBQT="$LIBQT $LIBPNG $LIBJPEG"
	fi
 
	AC_MSG_CHECKING([for Qt])
 
	LIBQT="$LIBQT $X_PRE_LIBS -lXext -lX11 $LIBSM $LIBSOCKET"
	ac_qt_includes=NO 
	ac_qt_libraries=NO 
	ac_qt_bindir=NO
	qt_libraries=""
	qt_includes=""
	AC_ARG_WITH(qt-dir,AC_HELP_STRING([--with-qt-dir=DIR],[where the root of Qt is installed]),
    	    [   ac_qt_includes="$withval"/include
       	    	ac_qt_libraries="$withval"/lib
       	    	ac_qt_bindir="$withval"/bin
    	    ]
	)
 
	AC_ARG_WITH(qt-includes, AC_HELP_STRING([--with-qt-includes=DIR], [where the Qt includes are.]),
    	    [ac_qt_includes="$withval"]
	)
 
	qt_libs_given=no
 
	AC_ARG_WITH(qt-libraries, AC_HELP_STRING([--with-qt-libraries=DIR], [where the Qt library is installed.]),
    	    [   ac_qt_libraries="$withval"
       		qt_libs_given=yes
    	    ]
	)
	AC_CACHE_VAL(ac_cv_have_qt,
	    [
#try to guess Qt locations
		qt_incdirs=""
		for dir in $qt_dirs; do
   		    qt_incdirs="$qt_incdirs $dir/include $dir"
		done
		qt_incdirs="$QTINC $qt_incdirs /usr/local/qt/include /usr/include/qt \
			/usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/qt $x_includes"
		if test ! "$ac_qt_includes" = "NO"; then
   		    qt_incdirs="$ac_qt_includes $qt_incdirs"
		fi
 
		if test "$qtver" = "2"; then
  		    qt_header=qstyle.h
		else
  		    qt_header=qglobal.h
		fi
 
		AC_FIND_FILE($qt_header, $qt_incdirs, qt_incdir)
		ac_qt_includes="$qt_incdir"
 
		qt_libdirs=""
		for dir in $qt_dirs; do
   		    qt_libdirs="$qt_libdirs $dir/lib $dir"
		done
		qt_libdirs="$QTLIB $qt_libdirs /usr/X11R6/lib /usr/lib /usr/local/qt/lib $x_libraries"
		if test ! "$ac_qt_libraries" = "NO"; then
  		    qt_libdir=$ac_qt_libraries
		else
  		    qt_libdirs="$ac_qt_libraries $qt_libdirs"
# if the Qt was given, the chance is too big that libqt.* doesn't exist
  		    qt_libdir=NONE
  		    for dir in $qt_libdirs; do
			for lib in qt qt-mt ; do
    				try="ls -1 ${dir}/lib${lib}.*"
	    			if test -n "`$try 2>/dev/null`"; then 
				    qt_libdir=$dir; 
				    int_qt="-l${lib}"
				    break 2; 
				else
				    echo "tried $dir" >&AC_FD_CC ; 
				fi
			done
  		    done
		fi
 
		ac_qt_libraries="$qt_libdir"
 
		AC_LANG_SAVE
		AC_LANG_CPLUSPLUS
 
		ac_cxxflags_safe="$CXXFLAGS"
		ac_ldflags_safe="$LDFLAGS"
		ac_libs_safe="$LIBS"
 
		CXXFLAGS="$CXXFLAGS -I$qt_incdir $all_includes"
		LDFLAGS="$LDFLAGS -L$qt_libdir $all_libraries $USER_LDFLAGS"

		for LIBQT in "-lqt" "-lqt-mt" ; do
			LIBS="$ac_libs_safe $LIBQT"
			AC_PRINT_QT_PROGRAM
			if AC_TRY_EVAL(ac_link) && test -s conftest; then
				rm -f conftest*
				break
			else
  		    		echo "configure: failed program was:" >&AC_FD_CC
  		    		cat conftest.$ac_ext >&AC_FD_CC
				if test "$LIBQT" = "-lqt-mt" ; then
  		    			ac_qt_libraries="NO"
				fi
			fi
			rm -f conftest*
		done
		CXXFLAGS="$ac_cxxflags_safe"
		LDFLAGS="$ac_ldflags_safe"
		LIBS="$ac_libs_safe"

		if test "$LIBQT" = "-lqt-mt" ; then
			CXXFLAGS="$CXXFLAGS -DQT_THREAD_SUPPORT"
		fi 
		AC_LANG_RESTORE
		if test "$ac_qt_includes" = NO || test "$ac_qt_libraries" = NO; then
  		    ac_cv_have_qt="have_qt=no"
  		    ac_qt_notfound=""
  		    if test "$ac_qt_includes" = NO; then
    			if test "$ac_qt_libraries" = NO; then
      			    ac_qt_notfound="(headers and libraries)";
    			else
      			    ac_qt_notfound="(headers)";
    			fi
  		    else
    			ac_qt_notfound="(libraries)";
  		    fi
		    qt_msg="Qt ($qt_minversion) $ac_qt_notfound not found. Please check your installation!
For more details about this problem, look at the end of config.log."
		    if test "$3" != "no" ; then
  		    	AC_MSG_FAILURE([$qt_msg])
		    else
  		    	AC_MSG_WARN([$qt_msg])
		    fi
		else
			have_qt="yes"
			int_qt=$LIBQT
		fi
	    ]
	)
	eval "$ac_cv_have_qt"
 
	if test "$have_qt" != yes; then
  	    AC_MSG_RESULT([$have_qt]);
	else
  	    ac_cv_have_qt="have_qt=yes \
    	    ac_qt_includes=$ac_qt_includes ac_qt_libraries=$ac_qt_libraries"
  	    AC_MSG_RESULT([libraries $ac_qt_libraries, headers $ac_qt_includes])
  	    qt_libraries="$ac_qt_libraries"
  	    qt_includes="$ac_qt_includes"
	fi
 
	if test ! "$qt_libs_given" = "yes"; then
	    AC_CHECK_QT_DIRECT(qt_libraries= ,[])
	fi
 
	AC_SUBST(qt_libraries)
	AC_SUBST(qt_includes)
 
	if test "$qt_includes" = "$x_includes" || test -z "$qt_includes"; then
 	    QT_INCLUDES="";
	else
 	    QT_INCLUDES="-I$qt_includes"
 	    all_includes="$QT_INCLUDES $all_includes"
	fi
 
	if test "$qt_libraries" = "$x_libraries" || test -z "$qt_libraries"; then
 	    QT_LDFLAGS=""
	else
 	    QT_LDFLAGS="-L$qt_libraries"
 	    all_libraries="$all_libraries $QT_LDFLAGS"
	fi
 
	AC_SUBST(QT_INCLUDES)
	AC_SUBST(QT_LDFLAGS)
	AC_PATH_QT_MOC_UIC
 
	LIB_QT="$int_qt "'$(LIBPNG) $(LIBJPEG) -lXext $(LIB_X11) $(LIBSM)'
	AC_SUBST(LIB_QT)
    ]
)

