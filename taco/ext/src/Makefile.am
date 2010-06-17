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

AM_CPPFLAGS 	= -I$(top_srcdir)/ext/include @TACO_INCLUDES@ @LOG4CPP_CFLAGS@

AM_CXXFLAGS 	= -Wall -DTACO_CLIENT_RUNTIME_TYPE_CHECK

lib_LTLIBRARIES = libTACOExtensions.la

libTACOExtensions_la_SOURCES = TACOServer.cpp \
	TACOException.cpp \
	TACOExtensions.cpp \
	TACOResources.cpp \
	TACOStates.cpp \
	TACOStringConverters.cpp \
	TACOClient.cpp \
	TACOAdmin.cpp

libTACOExtensions_la_LDFLAGS = -version-info @LIB_VERSION@
libTACOExtensions_la_LIBADD = @TACO_CLIENT_CXX_LIBS@ @LIB_STDCXX@ $(top_builddir)/lib/log4taco/liblog4taco.la
