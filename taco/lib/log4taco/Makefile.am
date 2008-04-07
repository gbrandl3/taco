lib_LTLIBRARIES 	= liblog4taco.la

INCLUDES 		= -I$(top_srcdir)/log4cpp/src -I$(top_srcdir)/log4cpp/include -I$(top_builddir)/log4cpp/include @TACO_INCLUDES@

include_HEADERS 	= MsgServerAppender.h

liblog4taco_la_SOURCES 	= MsgServerAppender.cpp 

liblog4taco_la_LDFLAGS 	= -version-info @LIB_VERSION@
liblog4taco_la_LIBADD 	= $(top_builddir)/log4cpp/src/liblog4cpp.la @TACO_CLIENT_LIBS@

noinst_PROGRAMS		= MsgServerTest

MsgServerTest_SOURCES 	= MsgServerTest.cpp
MsgServerTest_LDADD   	= liblog4taco.la

template_DATA		= taco_log.cfg
