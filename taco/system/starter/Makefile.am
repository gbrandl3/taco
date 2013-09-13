# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2013 Jens Krüger <jkrueger1@users.sf.net>
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

sbin_PROGRAMS 	= StartServer

noinst_HEADERS 	= StarterDevice.h StarterCmds.h

include_HEADERS	= StarterCommon.h

AM_CXXFLAGS 	= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@ -I$(top_srcdir)/ext/include
AM_CFLAGS	= @TACO_INCLUDES@

StartServer_SOURCES 	= StarterDevice.cpp startup.cpp device_server.c
#if SOLARIS_CXX
#StartServer_LDADD 	= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@
#else
StartServer_LDADD 	= @TACO_SERVER_CXX_LIBS@ 
#endif

StartServer_LDADD	+= $(top_builddir)/lib/log4taco/liblog4taco.la @LOG4CPP_LIBS@

SUBDIRS		= .

if BUILD_EXT
SUBDIRS		+= client
endif
