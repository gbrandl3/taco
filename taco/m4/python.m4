AC_DEFUN([PYTHON_PROG],
[
	AM_PATH_PYTHON([$1]) 
	AC_MSG_CHECKING([Python ])
 
	AC_CACHE_VAL(ac_cv_python,
	[
  		if test -z "$PYTHONDIR"; then
    			ac_cv_python=/usr/local
		else
			ac_cv_python="$PYTHONDIR"
		fi
	])
 
	AC_ARG_WITH(python, AC_HELP_STRING([--with-python=pythondir], [use python installed in pythondir]),
		[ac_python_dir=$withval], [ac_python_dir=$ac_cv_python])
 
	AC_MSG_RESULT($ac_python_dir)
 
  	AC_CACHE_VAL(ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
])

dnl PYTHON_DEVEL()
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) output variable.
AC_DEFUN([PYTHON_DEVEL],[
 	AC_REQUIRE([PYTHON_PROG])
	AC_CACHE_CHECK([python version], ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
	ac_python_version=$ac_cv_python_version
	AC_MSG_CHECKING([Python${ac_python_version}])
 
	python_incdirs="$ac_python_dir/include /usr/include /usr/local/include/"
	AC_FIND_FILE(Python.h, $python_incdirs, python_incdir)
	if test ! -r $python_incdir/Python.h; then
		AC_FIND_FILE(python${ac_python_version}/Python.h, $python_incdirs, python_incdir)
		python_incdir=$python_incdir/python${ac_python_version}
	fi
  	if test -r $python_incdir/Python.h; then
		python_includes=-I$python_incdir
		PYTHON_INCLUDES=-I$python_incdir
 
		python_libdirs="$ac_python_dir/lib /usr/lib /usr/local /usr/lib"
		AC_FIND_FILE(libpython${ac_python_version}.a, $python_libdirs, python_libdir)
		if test ! -r $python_libdir/libpython${ac_python_version}.a; then
  			AC_FIND_FILE(python${ac_python_version}/config/libpython${ac_python_version}.a, $python_libdirs, python_libdir)
  	    		python_libdir=$python_libdir/python${ac_python_version}/config
		fi
  		if test -r $python_libdir/libpython${ac_python_version}.a; then
			LIB_PYTHON=-L$python_libdir
			if test -z "$python_libraries"; then
				python_libraries=-lpython${ac_python_version}
			fi
			LIB_PYTHON="$LIB_PYTHON $LIBDL $LIBSOCKET"
		else
			if test -z "$2" ; then
				AC_MSG_ERROR(libpython${ac_python_version}.a not found.)
			else
    	    			AC_MSG_RESULT(libpython${ac_python_version}.a not found.)
			fi
  	    	fi
	else
		if test -z "$2" ; then
    	    		AC_MSG_ERROR(Python.h not found.)
		else
    	    		AC_MSG_RESULT(Python.h not found.)
		fi
  	fi
 
	AC_SUBST([PYTHON_CPPFLAGS], ["$PYTHON_INCLUDES"])
	AC_SUBST([PYTHON_LDFLAGS], ["$LIB_PYTHON"])
])

