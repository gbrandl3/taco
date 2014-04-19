# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2014 Jens Krüger <jkrueger1@users.sf.net>
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

include_HEADERS			= boolean.h  dataport.h  dataportP.h  sema.h  shared.h

EXTRA_libdataport_la_SOURCES	= dataport_ux.c dataport_os9.c shared.c sema.c

lib_LTLIBRARIES			= libdataport.la

if DATAPORT_UNIX
libdataport_la_SOURCES		= dataport_ux.c shared.c sema.c 
else
libdataport_la_SOURCES		= dataport_os9.c
endif

libdataport_la_LDFLAGS		= -version-info @LIB_VERSION@

AM_CPPFLAGS			= @TACO_INCLUDES@
