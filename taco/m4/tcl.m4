dnl 
dnl 	Try to locate the Tcl package
dnl 

AC_DEFUN([TCL_PROG],
[
	TCLINCLUDE=
	TCLLIB=
	TCLPACKAGE=

	AC_ARG_WITH(tcl, AC_HELP_STRING([--with-tcl=path], [Set location of Tcl package]),
		[TCLPACKAGE="$withval"], [TCLPACKAGE=])
])

AC_DEFUN([TCL_DEVEL],
[
	AC_REQUIRE([TCL_PROG])
	AC_ARG_WITH(tclconfig, AC_HELP_STRING([--with-tclconfig=path], [Set location of tclConfig.sh]),
        	[with_tclconfig="$withval"])
	AC_ARG_WITH(tclincl, AC_HELP_STRING([--with-tclincl=path], [Set location of Tcl include directory]),
		[TCLINCLUDE="-I$withval"], [TCLINCLUDE=])
	AC_ARG_WITH(tcllib, AC_HELP_STRING([--with-tcllib=path], [Set location of Tcl library directory]),
		[TCLLIB="-L$withval"], [TCLLIB=])

	AC_MSG_CHECKING([for Tcl configuration])
dnl
dnl First check to see if --with-tclconfig was specified.
dnl
	if test x"${with_tclconfig}" != x ; then
   		if test -f "${with_tclconfig}/tclConfig.sh" ; then
      			TCLCONFIG=`(cd ${with_tclconfig}; pwd)`
   		else
      			AC_MSG_ERROR([${with_tcl} directory doesn't contain tclConfig.sh])
   		fi
	fi
dnl
dnl check in a few common install locations
dnl	
	if test x"${TCLCONFIG}" = x ; then
               for i in "/usr/lib:" `ls -a /usr/lib/* /usr/local/lib/* 2>/dev/null | grep ':$'` ; do
                        dir="`dirname $i`/`basename $i ':'`"
                        if test -f "$dir/tclConfig.sh" ; then
                                TCLCONFIG=$dir
	    			break
			fi
		done
	fi
	if test x"${TCLCONFIG}" = x ; then
		AC_MSG_RESULT(no)
	else
		AC_MSG_RESULT(found $TCLCONFIG/tclConfig.sh)
		. $TCLCONFIG/tclConfig.sh
		TCL_PREFIX=`eval "echo $TCL_PREFIX"`
		TCL_H=`find $TCL_PREFIX/include -type f -name tcl.h | grep $TCL_VERSION`
		if test -z "$TCL_H" ; then
			TCL_H=`find $TCL_PREFIX/include -type f -name tcl.h`
		fi
		TCLINCLUDE="-I`dirname $TCL_H`"
		TCLLIB=`eval "echo $TCL_LIB_SPEC"`
	fi

	if test -z "$TCLINCLUDE"; then
		if test -n "$TCLPACKAGE"; then
			TCLINCLUDE="-I$TCLPACKAGE/include"
   		fi
	fi

	if test -z "$TCLLIB"; then
		if test -n "$TCLPACKAGE"; then
			TCLLIB="-L$TCLPACKAGE/lib -ltcl"
		fi
	fi

	AC_MSG_CHECKING(for Tcl header files)
	if test -z "$TCLINCLUDE"; then
		AC_TRY_CPP([#include <tcl.h>], , TCLINCLUDE="")
		if test -z "$TCLINCLUDE"; then
			dirs="/usr/local/include /usr/include /opt/local/include"
			for i in $dirs ; do
				if test -r $i/tcl.h; then
					AC_MSG_RESULT($i)
					TCLINCLUDE="-I$i"
					break
				fi
			done
		fi
		if test -z "$TCLINCLUDE"; then
#			TCLINCLUDE="-I/usr/local/include"
    			AC_MSG_RESULT(not found)
		fi
	else
        	AC_MSG_RESULT($TCLINCLUDE)
	fi

	AC_MSG_CHECKING(for Tcl library)
	if test -z "$TCLLIB"; then
	dirs="/usr/local/lib /usr/lib /opt/local/lib"
	for i in $dirs ; do
		if test -r $i/libtcl.a; then
			AC_MSG_RESULT($i)
			TCLLIB="-L$i -ltcl"
			break
		fi
	done
	if test -z "$TCLLIB"; then
		AC_MSG_RESULT(not found)
#		TCLLIB="-L/usr/local/lib"
	fi
	else
		AC_MSG_RESULT($TCLLIB)
	fi

# Cygwin (Windows) needs the library for dynamic linking
	case $host in
		*-*-cygwin* | *-*-mingw*) TCLDYNAMICLINKING="$TCLLIB";;
		*)TCLDYNAMICLINKING="";;
	esac

	AC_SUBST(TCL_CPPFLAGS)
	AC_SUBST(TCL_LDFLAGS)
	AC_SUBST(TCLDYNAMICLINKING)
])
