#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2004-2005 Jens Krüger <jkrueger1@users.sf.net>
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

pyexec_LTLIBRARIES	= Server.la Taco.la

Server_la_LDFLAGS 	= -module 

Taco_la_LDFLAGS 	= -module 

INCLUDES 		= @TACO_INCLUDES@ @PYTHON_CPPFLAGS@ @PYTHON_CPPFLAGS@

Server_la_LIBADD 	= $(top_builddir)/lib/libtaco.la $(top_builddir)/lib/xdr/libdsxdr_all.la @PYTHON_LDFLAGS@

Taco_la_LIBADD 		= $(top_builddir)/lib/libtaco.la $(top_builddir)/lib/xdr/libdsxdr_all.la @PYTHON_LDFLAGS@

AM_CPPFLAGS 		= -DMYDEB -DNUMPY 

noinst_HEADERS		= taco.h tacopython.h pythonP.h

Server_la_SOURCES	= taco_types.c server.c tacopython.c

Taco_la_SOURCES		= taco.c taco_types.c

