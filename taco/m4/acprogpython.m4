AC_DEFUN(TACO_PROG_PYTHON,
[
	AM_PATH_PYTHON([$1]) 
	AC_MSG_CHECKING([Python ])
 
	AC_CACHE_VAL(ac_cv_pythondir,
	[
  		if test -z "$PYTHONDIR"; then
    			ac_cv_pythondir=/usr/local
		else
			ac_cv_pythondir="$PYTHONDIR"
		fi
	])
 
	AC_ARG_WITH(pythondir, AC_HELP_STRING([--with-pythondir=pythondir], [use python installed in pythondir]),
		[ac_python_dir=$withval], [ac_python_dir=$ac_cv_pythondir])
 
	AC_MSG_RESULT($ac_python_dir)
 
  	version="$PYTHON_VERSION"
 
	AC_MSG_CHECKING([Python${version}])
 
	python_incdirs="$ac_python_dir/include /usr/include /usr/local/include/"
	AC_FIND_FILE(Python.h, $python_incdirs, python_incdir)
	if test ! -r $python_incdir/Python.h; then
		AC_FIND_FILE(python${version}/Python.h, $python_incdirs, python_incdir)
		python_incdir=$python_incdir/python${version}
	fi
  	if test -r $python_incdir/Python.h; then
		python_includes=-I$python_incdir
		PYTHON_INCLUDES=-I$python_incdir
 
		python_libdirs="$ac_python_dir/lib /usr/lib /usr/local /usr/lib"
		AC_FIND_FILE(libpython${version}.a, $python_libdirs, python_libdir)
		if test ! -r $python_libdir/libpython${version}.a; then
  			AC_FIND_FILE(python${version}/config/libpython${version}.a, $python_libdirs, python_libdir)
  	    		python_libdir=$python_libdir/python${version}/config
		fi
  		if test -r $python_libdir/libpython${version}.a; then
			LIB_PYTHON=-L$python_libdir
			if test -z "$python_libraries"; then
				python_libraries=-lpython${version}
			fi
			LIBPYTHON="$LIBPYTHON $LIBDL $LIBSOCKET"
		else
			if test -z "$2" ; then
				AC_MSG_ERROR(libpython${version}.a not found.)
			else
    	    			AC_MSG_RESULT(libpython${version}.a not found.)
			fi
  	    	fi
	else
		if test -z "$2" ; then
    	    		AC_MSG_ERROR(Python.h not found.)
		else
    	    		AC_MSG_RESULT(Python.h not found.)
		fi
  	fi
 
	AC_SUBST(PYTHON_INCLUDES)
	AC_SUBST(LIB_PYTHON)
	AC_SUBST(python_libraries)
	AC_SUBST(python_includes)
])
