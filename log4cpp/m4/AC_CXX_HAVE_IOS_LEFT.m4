dnl @synopsis AC_CXX_HAVE_IOS
dnl
dnl If the C++ library has a working stringstream, define HAVE_SSTREAM.
dnl
dnl @author Jens Krüger
dnl
AC_DEFUN([AC_CXX_HAVE_IOS],
[AC_LANG_PUSH(C++)
AC_CHECK_HEADERS([ios])
AC_LANG_POP(C++)])

dnl @synopsis AC_CXX_HAVE_IOS_LEFT
dnl
dnl If the C++ library has a working stringstream, define HAVE_SSTREAM.
dnl
dnl @author Jens Krüger
dnl @version $Id: AC_CXX_HAVE_IOS_LEFT.m4,v 1.1 2006-12-13 11:12:32 jkrueger1 Exp $
dnl
AC_DEFUN([AC_CXX_HAVE_IOS_LEFT],
[
AC_CACHE_CHECK([whether the compiler has std::ios::left], ac_cv_cxx_have_std_ios_left,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_REQUIRE([AC_CXX_HAVE_SSTREAM])
 AC_LANG_PUSH(C++)
 AC_REQUIRE([AC_CXX_HAVE_IOS])
 AC_TRY_COMPILE([
#include <sstream>
#ifdef HAVE_IOS
#include <ios>
#endif
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],
[
 ostringstream message;
 message.setf(std::ios::left);
 message << "Hello"; 
 return 0;
],
 ac_cv_cxx_have_std_ios_left=yes, ac_cv_cxx_have_std_ios_left=no)
])
if test "$ac_cv_cxx_have_std_ios_left" = yes; then
  AC_DEFINE(HAVE_STD_IOS_LEFT,,[define if the compiler has std::ios::left])
else
  AC_CACHE_CHECK([whether the compiler has ios::left], ac_cv_cxx_have_ios_left,
  AC_TRY_COMPILE([
#include <sstream>
#ifdef HAVE_IOS
#include <ios>
#endif
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],
[
 ostringstream message;
 message.setf(std::ios::left);
 message << "Hello"; 
 return 0;
],
  ac_cv_cxx_have_ios_left=yes, ac_cv_cxx_have_ios_left=no))
  if test "$ac_cv_cxx_have_ios_left" = yes; then
    AC_DEFINE(HAVE_IOS_LEFT,,[define if the compiler has ios::left])
  fi
fi
 AC_LANG_POP(C++)
])
