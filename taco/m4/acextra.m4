dnl
dnl AC_CHECK_LIB_FUN[lib-name,includes,body,found-action,not-found-action]
dnl
AC_DEFUN([AC_CHECK_LIB_FUN],
	[AC_MSG_CHECKING(checking for lib$1)
	LIBS_ORG_LIBFUN="$LIBS"	
	LIBS="$LIBS -l$1 "
	ac_safe=`echo "$1" | sed 'y%./+-%__p_%'`
	AC_CACHE_VAL(ac_cv_lib_$ac_safe,
		[AC_TRY_LINK([$2],
		 [$3],[eval "ac_cv_lib_$ac_safe=yes"],
		 [eval "ac_cv_lib_$ac_safe=no"])
		]
	)		
	eval "res="'$ac_cv_lib_'$ac_safe""
	AC_MSG_RESULT($res)
	if eval "test \"`echo $res`\" = yes"; then
		 LIBS="$LIBS_ORG_LIBFUN"
		 $4
	else
	  LIBS="$LIBS_ORG_LIBFUN" 
	  $5
	fi
	]
)

dnl
dnl find header in list dirs
dnl add dir to include file search path, if not already there
dnl AC_FIND_HEADER( HEADER, PATH )
dnl
AC_DEFUN([AC_FIND_HEADER],
[ found=no
 ac_shead=`echo "$1" | sed 'y%./+-%__p_%'`
 AC_CACHE_VAL([ac_cv_header_path_$ac_shead],[
	AC_CHECKING([in current search header path])
	AC_CHECK_HEADER($1,found=yes,[unset ac_cv_header_$ac_safe;unset ac_Header])
	  if  eval "test \"`echo $found`\" = no"; then
		for i in $2; do
			CPPFLAGS_ORIG="$CPPFLAGS"
			CPPFLAGS="$CPPFLAGS_ORIG $i"
			AC_CHECKING([searching in $i])
			AC_CHECK_HEADER($1,found=yes,[unset ac_cv_header_$ac_safe;unset ac_Header])
			if   eval "test \"`echo $found`\" = yes"; then
				CPPFLAGS="$CPPFLAGS_ORIG"
				eval ac_cv_header_path_$ac_shead="\"$i\""
				break;
			fi
			CPPFLAGS="$CPPFLAGS_ORIG"
		done
		if   eval "test \"`echo $found`\" = no"; then
			AC_MSG_ERROR(" could not find $1 in $2 or standard path")
		fi
	else	
		eval ac_cv_header_path_$ac_shead="\"\""
	fi
 ])
eval echo '$ac_cv_header_path_'$ac_shead
eval  CPPFLAGS=\"${CPPFLAGS} '$ac_cv_header_path_'$ac_shead\"
	echo $CPPFLAGS	
])

dnl
dnl find lib in list dirs, checking if code stub compiles an links
dnl add dir to library file search path, if not already there
dnl if src-dir is given, it is preferred
dnl make sure the ordering is correct for linking!
dnl AC_FIND_LIB( lib-name,PATH,includes,body,[ , [src_dir]])
dnl
AC_DEFUN([AC_FIND_LIB],
[ found=no 
ac_slib=`echo "$1" | sed 'y%./+-%__p_%'`
AC_CACHE_VAL([ac_cv_lib_path_$ac_slib],
[
  if [[ ! -z "$6" ]]; then 
     if [[ -d $6 ]]; then
	    eval ac_cv_lib_path_$ac_slib="\"$6/lib$1.la\""
	dnl  use libtool library 
     fi
  else 
	AC_CHECKING([in current search library path])
	LIBS_ORG_FINDLIB="$LIBS"
	if [[ ! -z "$5" ]] ; then
	    LIBS="$LIBS $$5"
	fi
	AC_CHECK_LIB_FUN([$1],[$3],[$4],[found=yes],[unset ac_cv_lib_$ac_safe])
	if  eval "test \"`echo $found`\" = no"; then
		for i in $2 ; do
			AC_CHECKING([searching in $i])
			LIBS="$LIBS $i"
			AC_CHECK_LIB_FUN([$1],[$3],[$4],[found=yes],[unset ac_cv_lib_$ac_safe])
			if  eval "test \"`echo $found`\" = yes"; then
				eval ac_cv_lib_path_$ac_slib="\"$i -l$1\""
				break;
			fi
		done
		if  eval "test \"`echo $found`\" = no"; then
			if [[ -z "$6" ]]; then 
			AC_MSG_ERROR(" could not find $1 in $2 or standard path")
			else
				eval ac_cv_lib_path_$ac_slib="\"$6/lib$1.la\""
				dnl  libtool  creates the library in a subdir .libs
			fi
		fi
	else
		eval ac_cv_lib_path_$ac_slib="\"-l$1\""
	fi
	LIBS="$LIBS_ORG_FINDLIB"
  fi
])
 eval echo '$ac_cv_lib_path_'$ac_slib
 if [[ ! -z "$5" ]] ; then
 	 eval $5=\"'$ac_cv_lib_path_'$ac_slib ${$5}\" 	
	 echo "$5: $$5"
 else
	 eval LIBS=\"'$ac_cv_lib_path_'$ac_slib ${LIBS}\"
 fi	 
])

				
