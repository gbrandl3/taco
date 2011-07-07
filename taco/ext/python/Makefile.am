# Extensions for the convenient access to TACO
# Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

EXTRA_DIST 		= TACOClient.i TACOStates.i TACO.i

AM_CPPFLAGS 		= -I$(top_srcdir)/ext/include \
			@TACO_INCLUDES@ \
			@LOG4CPP_CFLAGS@ \
			@LOG4TACO_CFLAGS@ \
			-DSWIG_PYTHON_THREADS

AM_CXXFLAGS 		= -Wall

nodist_python_PYTHON	= TACOStates.py TACOClient.py TACO.py

pyexec_LTLIBRARIES 	= _TACOStates.la _TACOClient.la _TACO.la
# TACOClient.la 

nodist__TACOStates_la_SOURCES = wrap_TACOStates.cpp 

nodist__TACOClient_la_SOURCES = wrap_TACOClient.cpp 

nodist__TACO_la_SOURCES	= wrap_TACO.cpp

_TACOClient_la_SOURCES	= TACOPythonClientConverters.cpp

_TACOClient_la_LDFLAGS 	= -avoid-version -module
_TACOClient_la_LIBADD  	= @LIB_STDCXX@ ../src/libTACOExtensions.la

_TACO_la_LDFLAGS	= -avoid-version -module
_TACO_la_LIBADD		= @LIB_STDCXX@ ../src/libTACOExtensions.la

TACO.py:		wrap_TACO.cpp
	@true

TACOClient.py:		wrap_TACOClient.cpp
	@true

wrap_TACO.cpp:		$(srcdir)/TACO.i $(top_srcdir)/ext/include/TACOExtensions.h
	@SWIG@ @AX_SWIG_PYTHON_OPT@ -threads -c++ -I$(top_srcdir)/ext/include @TACO_INCLUDES@ -o $@ $(srcdir)/TACO.i

wrap_TACOClient.cpp:	$(srcdir)/TACOClient.i $(top_srcdir)/ext/include/TACOClient.h
	@SWIG@ @AX_SWIG_PYTHON_OPT@ -threads -c++ -I$(top_srcdir)/ext/include @TACO_INCLUDES@ -o $@ $(srcdir)/TACOClient.i

_TACOStates_la_LDFLAGS 	= -avoid-version -module
_TACOStates_la_LIBADD 	= @LIB_STDCXX@ ../src/libTACOExtensions.la

TACOStates.py: 		wrap_TACOStates.cpp
	@true

wrap_TACOStates.cpp: 	$(srcdir)/TACOStates.i $(top_srcdir)/ext/include/TACOStates.h
	@SWIG@ @AX_SWIG_PYTHON_OPT@ -c++ -I$(top_srcdir)/ext/include -o $@ $(srcdir)/TACOStates.i

CLEANFILES		= *.pyc *.pyo wrap_*.cpp TACOStates.py TACOClient.py TACO.py


AM_CPPFLAGS 		+= -D_REENTRANT @PYTHON_CPPFLAGS@

noinst_PYTHON		= test.py
 
noinst_HEADERS 		= TACOPythonClientConverters.h

# TACOClient_la_SOURCES 	= TACOPythonClient.cpp TACOPythonClientConverters.cpp
# TACOClient_la_LDFLAGS 	= -pthread -avoid-version -module
# TACOClient_la_LIBADD 	= @LIB_STDCXX@ ../src/libTACOExtensions.la @TACO_CLIENT_CXX_LIBS@

# install-data-hook:
# 	rm -f $(DESTDIR)$(pythondir)/TACOClient.py*

