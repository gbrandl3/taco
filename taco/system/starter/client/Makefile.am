# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2005 Jens Krüger <jkrueger1@users.sf.net>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

EXTRA_DIST		= StarterCommands.i Starter.py

include_HEADERS 	= Starter.h 

lib_LTLIBRARIES 	= libStarter.la

AM_CXXFLAGS 		= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@ -I$(srcdir)/.. -I$(top_srcdir)/ext/include -I$(top_srcdir)/lib/log4taco
AM_CFLAGS		= @TACO_INCLUDES@ -I$(srcdir)/.. -I$(top_srcdir)/ext/include -I$(top_srcdir)/lib/log4taco

libStarter_la_SOURCES 	= Starter.cpp
libStarter_la_LIBADD  	= @LIB_STDCXX@ $(top_builddir)/ext/src/libTACOExtensions.la

if BUILD_PYTHON
AM_CXXFLAGS			+= @AX_SWIG_PYTHON_CPPFLAGS@
nodist_python_PYTHON		= StarterCommands.py 

dist_python_PYTHON		= Starter.py 

pyexec_LTLIBRARIES 		= _StarterCommands.la 
nodist__StarterCommands_la_SOURCES = wrap_StarterCommands.cpp 
_StarterCommands_la_LDFLAGS 	= -avoid-version -module
_StarterCommands_la_LIBADD  	= @LIB_STDCXX@ libStarter.la

StarterCommands.py:		wrap_StarterCommands.cpp
	@true

wrap_StarterCommands.cpp:	$(srcdir)/StarterCommands.i $(srcdir)/../StarterCmds.h
	@SWIG@ @AX_SWIG_PYTHON_OPT@ -threads -c++ -I$(top_srcdir)/ext/include -I$(srcdir)/.. @TACO_INCLUDES@ -o $@ $(srcdir)/StarterCommands.i

DISTCLEANFILES			= wrap_StarterCommands.cpp StarterCommands.py

endif
