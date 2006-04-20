AM_CPPFLAGS 	= -I$(top_srcdir)/ext/server/include -I$(top_srcdir)/ext/common/include -I$(top_srcdir)/ext/client/include @TACO_INCLUDES@

AM_CXXFLAGS 	= -Wall -DTACO_EXT

lib_LTLIBRARIES = libTACOExtensions.la

# Bug or feature workaround
my_top_srcdir = $(top_srcdir)/ext

libTACOExtensions_la_SOURCES = \
	$(my_top_srcdir)/common/src/TACOException.cpp \
	$(my_top_srcdir)/common/src/TACOExtensions.cpp \
	$(my_top_srcdir)/common/src/TACOResources.cpp \
	$(my_top_srcdir)/common/src/TACOStates.cpp \
	$(my_top_srcdir)/common/src/TACOStringConverters.cpp \
	$(my_top_srcdir)/client/src/TACOClient.cpp \
	$(my_top_srcdir)/server/src/TACOServer.cpp 

libTACOExtensions_la_LDFLAGS = -version-info @LIB_VERSION@
libTACOExtensions_la_LIBADD = @TACO_SERVER_CXX_LIBS@ @TACO_CLIENT_CXX_LIBS@ -lstdc++ 
