dnl
dnl AC_PROG_SWIG_PYTHON
dnl	searches for SWIG in the PATH and adds the necessary options
dnl	returns both in SWIG
dnl

AC_DEFUN(AC_PROG_SWIG_PYTHON,
    [
dnl Check if the SWIG program is available
        AC_CHECK_PROG(ac_swig_prog, swig, swig)
        if test -z "$ac_swig_prog" ; then
            AC_MSG_ERROR([
Could not find SWIG. Please check your PATH!
])
        fi
	if test -z "$1" ; then
		ac_required_swig_version=1.3.14
	else
		ac_required_swig_version=$1 
	fi
dnl find out the version number of swig
        ac_swig_version=`swig -version 2>&1 | grep Version |cut -d' ' -f 3 |cut -d'-' -f 1| tr -d '[[:alpha:]]'`
	AC_MSG_RESULT([SWIG version is $ac_swig_version])
	dnl Check if the version is greater than or equal to 1.3.14 (10314)
	echo $ac_swig_version.$ac_required_swig_version| awk -F'.' '{ \
		found=(10000 * $[1] + 100 * $[2] + $[3]);\
		required=(10000 * $[4] + 100 * $[5] + $[6]);\
		if (found >= required) exit 0; else exit 1; \
	}'
	if test $? -ne 0 ; then
	    AC_MSG_ERROR([swig version >= to $ac_required_swig_version required, version $ac_swig_version found])
	fi
	
	ac_swig_lib_path=`swig -swiglib`	
        if `$ac_swig_prog -help 2>&1 | grep -q python` ; then
	    ac_swig_major_minor=`echo $ac_swig_version | cut -d'.' -f1-2`
            ac_swig_options="-c -c++ -python -shadow" 
            dnl ac_swig_options="" 
        else
            AC_MSG_ERROR([
This Version of SWIG, found $ac_prog_swig, does not support PYTHON.
Please get the right version.
])
        fi
	dnl Check if the SWIG Python library is available
	dnl AC_LANG_PUSH(C)
	dnl tmp=$LIBS
	LIB_SWIG_PYTHON=`swig -python -ldflags` -lswigpy
        dnl AC_FIND_LIB([swigpy],,,,)
	dnl AC_TRY_LINK(, LIB_SWIG_PYTHON=-lswigpy)
	dnl if test -z "$LIB_SWIG_PYTHON" ; then
	    dnl AC_MSG_ERROR([cannot find SWIG Python library])
	dnl fi
	AC_SUBST(LIB_SWIG_PYTHON)
	dnl AC_LANG_POP(C)

        SWIG="$ac_swig_prog $ac_swig_options -I$ac_swig_lib_path"
        AC_SUBST(SWIG)
	]
) 
 
