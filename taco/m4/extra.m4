dnl
dnl Description:
dnl
dnl Author(s):	B. Pedersen
dnl		$Author: jkrueger1 $
dnl
dnl Version:	$Revision: 1.2 $
dnl
dnl Date:	$Date: 2004-08-02 12:54:25 $
dnl

dnl
dnl
dnl AC_CHECK_LIB_FUN[lib-name, includes, body, found-action, not-found-action]
dnl
AC_DEFUN([AC_CHECK_LIB_FUN],
[
	AC_MSG_CHECKING(checking for lib$1)
	LIBS_ORG_LIBFUN="$LIBS"	
	LIBS="$LIBS -l$1 "
	ac_safe=`echo "$1" | sed 'y%./+-%__p_%'`
	AC_CACHE_VAL(ac_cv_lib_$ac_safe, AC_TRY_LINK([$2], [$3],[eval "ac_cv_lib_$ac_safe=yes"], [eval "ac_cv_lib_$ac_safe=no"]))		
	eval "res="'$ac_cv_lib_'$ac_safe""
	AC_MSG_RESULT($res)
	if eval "test \"`echo $res`\" = yes"; then
		LIBS="$LIBS_ORG_LIBFUN"
		$4
	else
		LIBS="$LIBS_ORG_LIBFUN" 
		$5
	fi
])

dnl
dnl find header in list dirs
dnl add dir to include file search path, if not already there
dnl AC_FIND_HEADER( HEADER, PATH, [action-if-found], [action-if-not-found], [includes=default-includes] )
dnl
AC_DEFUN([AC_FIND_HEADER],
[
	found=no
	ac_shead=`echo "$1" | sed 'y%./+-%__p_%'`
	AC_CACHE_VAL([ac_cv_header_path_$ac_shead],[
		for i in $2; do
			save_CPPFLAGS="$CPPFLAGS"
			CPPFLAGS="$CPPFLAGS -I$i"
			AC_CHECK_HEADER([$1], [found=yes], [unset ac_cv_header_$ac_safe; unset ac_Header])
			CPPFLAGS="$save_CPPFLAGS"
			if   eval "test \"`echo $found`\" = yes"; then
				if test -f ${i}/$1 ; then 
					eval ac_cv_header_path_$ac_shead="\"$i\""
					$3
					break;
				fi
			fi
		done
		if   eval "test \"`echo $found`\" = no"; then
			$4
		else
			eval  CPPFLAGS=\"${CPPFLAGS} -I'$ac_cv_header_path_'$ac_shead\"
		fi
	])
])

dnl
dnl find lib in list dirs, checking if code stub compiles an links
dnl add dir to library file search path, if not already there
dnl if src-dir is given, it is preferred
dnl make sure the ordering is correct for linking!
dnl AC_FIND_LIB(lib-name, function, PATH, [action-if-found], [action-if-not-found], [other-libraries])
dnl
AC_DEFUN([AC_FIND_LIB],
[	found=no 
	ac_slib=`echo "$1" | sed 'y%./+-%__p_%'`
	AC_CACHE_VAL([ac_cv_lib_path_$ac_slib], [
		save_LIBS="$LIBS"
		for i in $3 ; do
			LIBS="$LIBS -L$i"
			AC_CHECK_LIB([$1], [$2], [found=yes], [unset ac_cv_lib_${ac_slib}_$2;], [$6])
			if  eval "test \"`echo $found`\" = yes"; then
				eval ac_cv_lib_path_$ac_slib="\"$i -l$1\""
				$4
				break;
			fi
		done
		if  eval "test \"`echo $found`\" = no"; then
			$5
		else
			eval ac_cv_lib_path_$ac_slib="\"-l$1\""
		fi
		LIBS="$save_LIBS"
	])
])

