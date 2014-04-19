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
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
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
	AS_IF([test x"${with_tclconfig}" != x], 
              [
   		AS_IF([test -f "${with_tclconfig}/tclConfig.sh"], [TCLCONFIG=`(cd ${with_tclconfig}; pwd)`],
      		      [AC_MSG_ERROR([${with_tcl} directory doesn't contain tclConfig.sh])])
	      ])
dnl
dnl check in a few common install locations
dnl	
	AS_IF([test x"${TCLCONFIG}" = x],
	      [
                AC_CHECK_PROGS([TCLSH], [tclsh])
		AS_IF([test x"${TCLSH}" != x],
                      [tcl_version=`echo 'puts $tcl_version; exit 0' | tclsh`
                       tcl_lib_search="/usr/lib/tcl${tcl_version}"
                      ]
		     )
	        AS_CASE(["$target"],
                        [x86_64-*-linux* | ia64-*-linux-* | ia64-*-freebsd* | x86_64-*-freebsd*],
                        [taco_tcl_lib_search="/usr/lib64: "`ls -a /usr/lib64/* /usr/local/lib64/* 2>/dev/null | grep ':$'`],
                        [taco_tcl_lib_search="/usr/lib: "`ls -a /usr/lib/* /usr/local/lib/* 2>/dev/null | grep ':$'`]
		       )

                for i in $tcl_lib_search $taco_tcl_lib_search ; do
                        dir="`dirname $i`/`basename $i ':'`"
                        if test -f "$dir/tclConfig.sh" ; then
                                TCLCONFIG=$dir
	    			break
			fi
		done
	      ])

	AS_IF([test x"${TCLCONFIG}" = x], [AC_MSG_RESULT(no)],
	      [
		AC_MSG_RESULT(found $TCLCONFIG/tclConfig.sh)
		. $TCLCONFIG/tclConfig.sh
		AS_IF([test -z "$TCL_INCLUDE_SPEC"], 
                      [
			TCL_PREFIX=`eval "echo $TCL_PREFIX"`
			TCL_H=`find $TCL_PREFIX/include -type f -name tcl.h | grep $TCL_VERSION`
			AS_IF([test -z "$TCL_H"], [TCL_H=`find $TCL_PREFIX/include -type f -name tcl.h`])
			TCLINCLUDE="-I`dirname $TCL_H`"
		      ], [TCLINCLUDE=`eval "echo $TCL_INCLUDE_SPEC"`])
		TCLINCLUDE="$TCL_CFLAGS_WARNING $TCL_EXTRA_CFLAGS $TCLINCLUDE"
		TCLLIB=`eval "echo $TCL_LIB_SPEC $TCL_LIBS"`
	      ])

	AS_IF([test -z "$TCLINCLUDE" -a -n "$TCLPACKAGE"], [TCLINCLUDE="-I$TCLPACKAGE/include"])
	AS_IF([test -z "$TCLLIB" -a -n "$TCLPACKAGE"], [TCLLIB="-L$TCLPACKAGE/lib -ltcl"])

	AC_MSG_CHECKING(for Tcl header files)
	save_CPPFLAGS="$CPPFLAGS"
	AS_IF([test -z "$TCLINCLUDE"], 
              [
		AS_IF([test -z "$TCLINCLUDE"],
                      [
			dirs="/usr/local/include /usr/include /opt/local/include"
			for i in $dirs ; do
				if test -r $i/tcl.h; then
					AC_MSG_RESULT($i)
					TCLINCLUDE="-I$i"
					break
				fi
			done
		      ])
	      ])
	AS_IF([test -n "$TCLINCLUDE"], 
	      [
		CPPFLAGS="$CPPFLAGS -I$TCLINCLUDE"
		AC_CHECK_HEADERS([tcl.h], , TCLINCLUDE="")
	      ])
	AS_IF([test -n "$TCLINCLUDE"], AC_MSG_RESULT($TCLINCLUDE),
	      [
		CPPFLAGS="$save_CPPFLAGS"
    		AC_MSG_RESULT(not found)
	      ])

	AC_MSG_CHECKING(for Tcl library)
	AS_IF([test -z "$TCLLIB"], 
              [
	        case "$target" in
                        x86_64-*-linux* | ia64-*-linux-* | ia64-*-freebsd* )
				dirs="/usr/local/lib64 /usr/lib64 /opt/local/lib64"
                                ;;
                        *)
				dirs="/usr/local/lib /usr/lib /opt/local/lib"
                                ;;
                esac
		for i in $dirs ; do
			if test -r $i/libtcl.a; then
				AC_MSG_RESULT($i)
				TCLLIB="-L$i -ltcl"
				break
			fi
		done
		AS_IF([test -z "$TCLLIB"], [AC_MSG_RESULT(not found)])
	      ], [AC_MSG_RESULT($TCLLIB)])

# Cygwin (Windows) needs the library for dynamic linking
	case $host in
		*-*-cygwin* | *-*-mingw*) TCLDYNAMICLINKING="$TCLLIB";;
		*)TCLDYNAMICLINKING="";;
	esac

	AC_SUBST([TCL_CPPFLAGS], [$TCLINCLUDE])
	AC_SUBST([TCL_LDFLAGS], [$TCLLIB])
	AC_SUBST(TCLDYNAMICLINKING)
])
