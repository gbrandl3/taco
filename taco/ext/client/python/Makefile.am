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

AM_CPPFLAGS 		= -D_REENTRANT -I$(top_srcdir)/ext/common/include -I$(top_srcdir)/ext/client/include @TACO_INCLUDES@ @PYTHON_CPPFLAGS@

AM_CXXFLAGS 		= -Wall

noinst_PYTHON		= test.py
 
pyexec_LTLIBRARIES 	= TACOClient.la

noinst_HEADERS 		= TACOPythonClient.h TACOPythonClientConverters.h

TACOClient_la_SOURCES 	= TACOPythonClient.cpp TACOPythonClientConverters.cpp
TACOClient_la_LDFLAGS 	= -pthread -avoid-version -module
TACOClient_la_LIBADD 	= @LIB_STDCXX@ $(top_builddir)/ext/lib/libTACOExtensions.la @TACO_CLIENT_CXX_LIBS@

CLEANFILES		= *.pyc

install-data-hook:
	rm -f $(DESTDIR)$(pythondir)/TACOClient.py*

