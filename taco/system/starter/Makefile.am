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

sbin_PROGRAMS 	= StartServer

noinst_HEADERS 	= starter.h

AM_CXXFLAGS 	= @TACO_INCLUDES@ -I$(top_srcdir)/log4cpp/include -I$(top_builddir)/log4cpp/include
AM_CFLAGS	= @TACO_INCLUDES@

StartServer_SOURCES 	= starter.cpp startup.cpp device_server.c
#if SOLARIS_CXX
#StartServer_LDADD 	= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@
#else
StartServer_LDADD 	= @TACO_SERVER_CXX_LIBS@ 
#endif

StartServer_LDADD	+= $(top_builddir)/log4cpp/src/liblog4cpp.la
