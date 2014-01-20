# Extensions for the convenient access to TACO
# Copyright (C) 2002-2014 Sebastian Huber <sebastian-huber@web.de>

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

AM_CPPFLAGS 	= -I$(my_top_srcdir)/server/include -I$(my_top_srcdir)/common/include -I$(my_top_srcdir)/client/include @TACO_INCLUDES@

AM_CXXFLAGS 	= -Wall

lib_LTLIBRARIES = libTACOExtensions.la

# Bug or feature workaround
my_top_srcdir = $(top_srcdir)/ext

libTACOExtensions_la_SOURCES = $(my_top_srcdir)/server/src/TACOServer.cpp \
	$(my_top_srcdir)/common/src/TACOException.cpp \
	$(my_top_srcdir)/common/src/TACOExtensions.cpp \
	$(my_top_srcdir)/common/src/TACOResources.cpp \
	$(my_top_srcdir)/common/src/TACOStates.cpp \
	$(my_top_srcdir)/common/src/TACOStringConverters.cpp \
	$(my_top_srcdir)/client/src/TACOClient.cpp
libTACOExtensions_la_LDFLAGS = -version-info @LIB_VERSION@
libTACOExtensions_la_LIBADD = @TACO_SERVER_CXX_LIBS@ @LIB_STDCXX@
