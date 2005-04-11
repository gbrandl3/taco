AC_DEFUN([PYTHON_PROG],
[
	AC_ARG_WITH(python, AC_HELP_STRING([--with-python=pythondir], [use python installed in pythondir]),
		[ac_python_dir=$withval], [ac_python_dir=$ac_cv_python])
	if test "x$ac_python_dir" != "xno"  ; then
		AM_PATH_PYTHON([$1],[],[taco_python_binding=no]) 
		AC_MSG_CHECKING([Python ])
 
		AC_CACHE_VAL(ac_cv_python, [ac_cv_python=${PYTHONDIR:-/usr/local}])
  		AC_CACHE_VAL(ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
dnl		AC_MSG_RESULT($ac_python_dir)
	fi
 
])

dnl PYTHON_DEVEL()
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) output variable.
AC_DEFUN([PYTHON_DEVEL],[
	taco_python_binding=no

	AC_ARG_WITH(python-libraries, AS_HELP_STRING([--with-python-libraries=DIR], [Directory where python library is installed (optional)]),
		[python_libraries="$withval"], [python_libraries=""])
	AC_ARG_WITH(python-includes, AS_HELP_STRING([--with-python-includes=DIR], [Directory where python header files are installed (optional)]),
                [python_includes="$withval"], [python_includes=""])

	if test "x$ac_python_dir" != "xno"  ; then
 		AC_REQUIRE([PYTHON_PROG])
		AC_REQUIRE([AC_CANONICAL_TARGET])
		AC_CACHE_CHECK([python version], ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
		ac_python_version=$ac_cv_python_version
		AC_MSG_NOTICE([checking for Python${ac_python_version} devel])
dnl
dnl Get the cflags and libraries
dnl
		if test "x$python_includes" != "x" ; then
	                PYTHON_CPPFLAGS="-I$python_includes"
			AC_FIND_FILE(Python.h, $python_includes, python_incdir)
	        elif test "x$python_prefix" != "x" ; then
	                PYTHON_CPPFLAGS="-I$python_prefix/include"
			AC_FIND_FILE(Python.h, $python_prefix/include, python_incdir)
	        else 
			python_incdirs="$ac_python_dir/include /usr/include /usr/local/include/"
			case $target in
		 		powerpc-apple-darwin*)	
					python_incdirs="$python_incdirs /System/Library/Frameworks/Python.framework/Versions/${ac_python_version}/include" ;;
			esac
			AC_FIND_FILE(Python.h, $python_incdirs, python_incdir)
	
			if test ! -r $python_incdir/Python.h; then
				AC_FIND_FILE(python${ac_python_version}/Python.h, $python_incdirs, python_incdir)
				python_incdir=$python_incdir/python${ac_python_version}
			fi
	        fi
	  	if test -r $python_incdir/Python.h ; then
			python_includes=-I$python_incdir
			PYTHON_CPPFLAGS=-I$python_incdir
			taco_python_binding=yes
		fi
		result=yes
		if test "x$taco_python_binding" = "xyes" ; then 
	      	  	if test "x$python_libraries" != "x" ; then
				AC_FIND_FILE(libpython${ac_python_version}.a, $python_libraries, python_libdir)
	        	elif test "x$python_prefix" != "x" ; then
				AC_FIND_FILE(libpython${ac_python_version}.a, $python_prefix/lib, python_libdir)
	        	else 
				python_libdirs="$ac_python_dir/lib /usr/lib /usr/local /usr/lib"
				AC_FIND_FILE(libpython${ac_python_version}.a, $python_libdirs, python_libdir)
				if test ! -r $python_libdir/libpython${ac_python_version}.a; then
	  				AC_FIND_FILE(python${ac_python_version}/config/libpython${ac_python_version}.a, $python_libdirs, python_libdir)
	  	    			python_libdir=$python_libdir/python${ac_python_version}/config
				fi
			fi
	  		if test -r $python_libdir/libpython${ac_python_version}.a; then
				PYTHON_LDFLAGS="-L$python_libdir -lpython${ac_python_version} $LIBDL $LIBSOCKET"
				taco_python_bindings=yes
				AC_MSG_RESULT([Python (devel and runtime) found.])
			else
				AC_MSG_RESULT([libpython${ac_python_version}.a not found.])
 	 	    	fi
		else
	    	    	AC_MSG_RESULT([Python.h not found.])
	  	fi
 
		AC_SUBST(PYTHON_CPPFLAGS)
		AC_SUBST(PYTHON_LDFLAGS)
	fi 
])

