AM_CPPFLAGS 		= -D_REENTRANT -I$(top_srcdir)/ext/common/include -I$(top_srcdir)/ext/client/include $(TACO_INCLUDES) $(PYTHON_CPPFLAGS)

AM_CXXFLAGS 		= -Wall

noinst_PYTHON		= test.py
 
pyexec_LTLIBRARIES 	= TACOClient.la

noinst_HEADERS 		= TACOPythonClient.h TACOPythonClientConverters.h

TACOClient_la_SOURCES 	= TACOPythonClient.cpp TACOPythonClientConverters.cpp
TACOClient_la_LDFLAGS 	= -pthread -avoid-version -module
TACOClient_la_LIBADD 	= -lstdc++ $(top_builddir)/ext/lib/libTACOExtensions.la @TACO_CLIENT_CXX_LIBS@

CLEANFILES		= *.pyc

install-data-hook:
	rm -f $(DESTDIR)$(pythondir)/TACOClient.py*

