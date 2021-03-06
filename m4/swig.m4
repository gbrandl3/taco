# SWIG_PROG([required-version = N[.N[.N]]], [require-swig = {yes, no}])
#
# Checks for the SWIG program.  If found you can (and should) call SWIG via $(SWIG).
#
# You can use the optional first argument to check if the version of the available SWIG
# is greater than or equal to the value of the argument.  It should have the format:
# N[.N[.N]] (N is an integer between 0 and 999.  Only the first N is mandatory.)
#
# If the optional second argument is 'yes', an error is generated if SWIG is not present.
AC_DEFUN([SWIG_PROG],[
	AC_PATH_PROG([SWIG],[swig])

	AS_IF([test -z "$SWIG"],
	      [ 
		swig_tmp="SWIG is not installed, you may have a look at http://www.swig.org"
		AS_IF([test "x$2" = "xyes"], [AC_MSG_ERROR([$swig_tmp])], [AC_MSG_WARN([$swig_tmp])])
		SWIG="echo \"error: $swig_tmp\" ; false"
              ],
	      [
		AS_IF([test -n "$1"],
                      [
			# Check the SWIG version
			AC_MSG_CHECKING([for SWIG version])
			[swig_version=`$SWIG -version 2>&1 | grep 'SWIG Version' | sed 's/.*\([0-9]\+\.[0-9]\+\.[0-9]\+\).*/\1/g'`]
			AC_MSG_RESULT([$swig_version])
			AS_IF([test -n "$swig_version"],
                              [
				# Calculate the required version number
				[swig_tmp=`(echo $1 | sed 's/[^0-9]/ /g')`]
				[swig_tmp0=`(echo $swig_tmp | cut -d' ' -f1)`]
				[swig_tmp1=`(echo $swig_tmp | cut -d' ' -f2)`]
				[swig_tmp2=`(echo $swig_tmp | cut -d' ' -f3)`]
				[swig_required_version=$(( 1000000 * ${swig_tmp0:-0} + 1000 * ${swig_tmp1:-0} + ${swig_tmp2:-0} ))]

				# Calculate the available version number
				[swig_tmp=`(echo $swig_version | sed 's/[^0-9]/ /g')`]
				[swig_tmp0=`(echo $swig_tmp | cut -d' ' -f1)`]
				[swig_tmp1=`(echo $swig_tmp | cut -d' ' -f2)`]
				[swig_tmp2=`(echo $swig_tmp | cut -d' ' -f3)`]
				[swig_tmp=$(( 1000000 * ${swig_tmp0:-0} + 1000 * ${swig_tmp1:-0} + ${swig_tmp2:-0} ))]
	
				AS_IF([test $swig_required_version -gt $swig_tmp], 
					[AC_MSG_WARN([SWIG version $1 or above is required, you have $swig_version])])
			      ], [AC_MSG_WARN([cannot determine SWIG version])])
		      ], [swig_tmp=0])
	
		AS_IF([test $swig_tmp -lt 1003020],
		      [
			# Check if a SWIG runtime library is available
			SWIG_RUNTIME_LIBS_DIR="${SWIG%/bin*}/lib"
			AC_MSG_CHECKING([for SWIG runtime libraries in $SWIG_RUNTIME_LIBS_DIR])
			swig_tmp=`find "$SWIG_RUNTIME_LIBS_DIR" -maxdepth 1 -name 'libswig*'`
			AS_IF([test -n "$swig_tmp"], [swig_tmp=yes], [swig_tmp=no])
			AC_MSG_RESULT([$swig_tmp])
		      ], [swig_tmp="yes"])
	      ])
	AC_SUBST([SWIG_RUNTIME_LIBS_DIR])
])

AC_DEFUN([SWIG_PROG_13],
[
	SWIG_PROG([1.3])
])

# SWIG_ENABLE_CXX()
#
# Enable SWIG C++ support.  This effects all invocations of $(SWIG).
AC_DEFUN([SWIG_ENABLE_CXX],[
	AC_REQUIRE([SWIG_PROG_13])
	AC_REQUIRE([AC_PROG_CXX])
	SWIG="$SWIG -c++"
])

# SWIG_MULTI_MODULE_SUPPORT()
#
# Enable support for multiple modules.  This effects all invocations of $(SWIG).
# You have to link all generated modules against the appropriate SWIG runtime library.
# If you want to build Python modules for example, use the SWIG_PYTHON() macro
# and link the modules against $(SWIG_PYTHON_LIBS).
AC_DEFUN([SWIG_MULTI_MODULE_SUPPORT],[
	AC_REQUIRE([SWIG_PROG_13])
	SWIG="$SWIG -c"
])

# SWIG_PYTHON([use-shadow-classes = {no, yes}])
#
# Checks for Python and provides the $(SWIG_PYTHON_CPPFLAGS), $(SWIG_PYTHON_LIBS) and
# $(SWIG_PYTHON_OPT) output variables.  $(SWIG_PYTHON_OPT) contains all necessary SWIG
# options to generate code for Python.  Shadow classes are enabled unless the
# value of the optional first argument is exactly 'no'.  If you need multi module
# support (provided by the SWIG_MULTI_MODULE_SUPPORT() macro) use $(SWIG_PYTHON_LIBS)
# to link against the appropriate library.  It contains the SWIG Python runtime library
# that is needed by the type check system for example.
AC_DEFUN([SWIG_PYTHON],[
	AC_REQUIRE([SWIG_PROG_13])
	AC_REQUIRE([PYTHON_DEVEL])
	AS_IF([test "x$1" != "xno"], [swig_shadow=" -shadow"])
	AC_SUBST([SWIG_PYTHON_OPT],[-python$swig_shadow])
	AS_IF([test "$SWIG"], [AC_SUBST([SWIG_LIB], [`$SWIG -swiglib`])])
	AM_CONDITIONAL(BUILD_PYTHON, [test x"$swig_tmp" = x"yes" -a x"$taco_python_binding" = x"yes"])
	AC_SUBST([SWIG_PYTHON_CPPFLAGS],[$PYTHON_CPPFLAGS])
	AC_SUBST([SWIG_PYTHON_LIBS],["-L$SWIG_RUNTIME_LIBS_DIR -lswigpy"])
])
