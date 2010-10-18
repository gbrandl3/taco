lib_LTLIBRARIES 	= liblog4taco.la

AM_CPPFLAGS 		= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@

include_HEADERS 	= MsgServerAppender.h \
			  log4taco.h

liblog4taco_la_SOURCES 	= MsgServerAppender.cpp 

liblog4taco_la_LDFLAGS 	= -version-info @LIB_VERSION@
liblog4taco_la_LIBADD 	= @TACO_CLIENT_CXX_LIBS@ @LOG4CPP_LIBS@

noinst_PROGRAMS		= MsgServerTest

MsgServerTest_SOURCES 	= MsgServerTest.cpp
MsgServerTest_LDADD   	= liblog4taco.la
