AC_DEFUN([PYTHON_PROG],
[
	AC_ARG_ENABLE(python, AC_HELP_STRING([--enable-python], [enable the pyhton bindings [[default=yes]]]),
		[ac_enable_python="$enableval"], [ac_enable_python="yes"]) 
	AC_ARG_WITH(python, AC_HELP_STRING([--with-python=pythondir], [use python installed in pythondir]),
		[ac_python_dir=$withval], [ac_python_dir=$ac_cv_python])
	AS_IF([test "x$ac_python_dir" != "xno" -a x"$ac_enable_python" = x"yes"],
	      [
		AM_PATH_PYTHON([$1],[],[taco_python_binding=no]) 
		AC_MSG_CHECKING([Python ])
 
		AC_CACHE_VAL(ac_cv_python, [ac_cv_python=${PYTHONDIR:-/usr/local}])
  		AC_CACHE_VAL(ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
dnl		AC_MSG_RESULT($ac_python_dir)
	      ])
])

dnl PYTHON_DEVEL()
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) output variable.
AC_DEFUN([PYTHON_DEVEL],[
	taco_python_binding=no

 	AC_REQUIRE([PYTHON_PROG])
	AC_ARG_WITH(python-libraries, AS_HELP_STRING([--with-python-libraries=DIR], [Directory where python library is installed (optional)]),
		[python_libraries="$withval"], [python_libraries=""])
	AC_ARG_WITH(python-includes, AS_HELP_STRING([--with-python-includes=DIR], [Directory where python header files are installed (optional)]),
                [python_includes="$withval"], [python_includes=""])
	
	AS_IF([test "x$ac_python_dir" != "xno" -a x"$ac_enable_python" = x"yes"],
	      [
		AC_REQUIRE([AC_CANONICAL_TARGET])
		AC_CACHE_CHECK([python version], ac_cv_python_version, [ac_cv_python_version="$PYTHON_VERSION"])
		ac_python_version=$ac_cv_python_version
		AC_MSG_NOTICE([checking for Python${ac_python_version} devel])
dnl
dnl Get the cflags and libraries
dnl
		PYTHON_LIBS=`$PYTHON -c "import distutils.sysconfig; print ('%s %s %s' % (distutils.sysconfig.get_config_var('LIBS'), distutils.sysconfig.get_config_var('SYSLIBS'), distutils.sysconfig.get_config_var('LDFLAGS')))"`
		AS_IF([test "x$python_includes" != "x"], [python_incdirs=${python_includes}],
	              [test "x$python_prefix" != "x"], [python_incdirs=${python_prefix}/include],
                      [
			python_incdirs=`$PYTHON -c "import distutils.sysconfig; print ('%s %s' % (distutils.sysconfig.get_config_var('INCLUDEDIR'), distutils.sysconfig.get_config_var('CONFINCLUDEPY')))"`
			case $target in
		 		*-apple-darwin*)	
					python_incdirs="$python_incdirs /System/Library/Frameworks/Python.framework/Versions/${ac_python_version}/include" ;;
			esac
	              ])
		AC_MSG_RESULT([$python_incdirs])
		AC_FIND_FILE(Python.h, $python_incdirs, python_incdir)
	
		AS_IF([test ! -r $python_incdir/Python.h],
		      [
			AC_FIND_FILE(python${ac_python_version}/Python.h, $python_incdirs, python_incdir)
			python_incdir=$python_incdir/python${ac_python_version}
		      ])
	  	AS_IF([test -r $python_incdir/Python.h],
		      [ 
			python_includes=-I$python_incdir
			PYTHON_CPPFLAGS=-I$python_incdir
			taco_python_binding=yes
			ac_save_CXXFLAGS="$CXXFLAGS"
			CXXFLAGS="$PYTHON_CPPFLAGS $CXXFLAGS"
			AC_CHECK_TYPES(Py_ssize_t, [], [
				AC_CHECK_TYPES(ssize_t, [], [])], [#include <Python.h>])
			CXXFLAGS="$ac_save_CXXFLAGS"
		      ])
		result=yes
		AS_IF([test "x$taco_python_binding" = "xyes"],
		      [
	        	AS_IF([test "x$python_prefix" != "x"], [python_libraries=${python_prefix}/lib],
	      	  	      [test "x$python_libraries" = "x"], 
			      [
				python_libraries=`$PYTHON -c "import distutils.sysconfig; print ('%s' % distutils.sysconfig.get_config_var('LIBDIR'))"`
			      ])
			ac_save_LIBS="$LIBS"
			ac_save_LDFLAGS="$LDFLAGS"
			for i in ${python_libraries} ; do
				LDFLAGS="$ac_save_LDFLAGS -L${i} ${PYTHON_LIBS}"
				PYTHON_LIB=`$PYTHON -c "import distutils.sysconfig; print ('%s' % distutils.sysconfig.get_config_var('BLDLIBRARY')[[2:]])"`
				AS_IF([test -z "$PYTHON_LIB"],
				      [
					PYTHON_LIB=`$PYTHON -c "import distutils.sysconfig; print ('%s' % distutils.sysconfig.get_config_var('LIBRARY')[[3:]])"`
				      ])
				AS_IF([test -z "$PYTHON_LIB"], [PYTHON_LIB="python"])
				AC_CHECK_LIB([$PYTHON_LIB], [Py_Initialize], [
					PYTHON_LDFLAGS="-L${i}"
					break], 
					AC_CHECK_LIB([python${ac_python_version}], [Py_Initialize], [
					PYTHON_LDFLAGS="-L${i} -lpython${ac_python_version}"
					break], []), [])
			done
			LIBS="${ac_save_LIBS}"
			LDFLAGS="$ac_save_LDFLAGS"
	  		AS_IF([test -n "$PYTHON_LDFLAGS"], 
				[taco_python_binding=yes; 
				 using_python_version=" : using Python version : $am_cv_python_version";
				 AC_MSG_RESULT([Python (devel and runtime) found.])],
			      	[taco_python_binding=no; 
				 AC_MSG_RESULT([Python library version ${ac_python_version} not found.])])
		      ],  
	    	      AC_MSG_RESULT([Python.h not found.]))
		AC_SUBST(PYTHON_CPPFLAGS)
		AC_SUBST(PYTHON_LDFLAGS)
	      ]) 

])

