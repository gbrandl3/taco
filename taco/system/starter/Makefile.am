sbin_PROGRAMS 	= StartServer

noinst_HEADERS 	= starter.h

AM_CXXFLAGS 	= @TACO_INCLUDES@

StartServer_SOURCES 	= starter.cpp startup.cpp
if SOLARIS_CXX
StartServer_LDADD 	= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@
else
StartServer_LDADD 	= @TACO_SERVER_CXX_LIBS@ 
endif

