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

lib_LTLIBRARIES = libtcltacoapi.la

libtcltacoapi_la_SOURCES = convert.c \
			db_io_debug.c db_io_get.c db_io_put.c \
			debug.c \
			dev_free.c dev_io.c dev_io_protocol.c dev_io_timeout.c \
			tcltaco_ver.c tcltacoapi_init.c \
			dc_io_debug.c dc_io_get.c 

libtcltacoapi_la_LIBADD = $(top_builddir)/lib/libtaco.la

libtcltacoapi_la_LDFLAGS = -rdynamic

AM_CPPFLAGS = @TACO_INCLUDES@ @TCL_CPPFLAGS@

noinst_HEADERS = db_io.h dc_io.h debug.h dev_io.h convert.h
