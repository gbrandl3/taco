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

noinst_HEADERS 		= Serial.h

AM_CXXFLAGS     	= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@

AM_CFLAGS       	= @TACO_INCLUDES@

if SOLARIS_CXX
Serialds_LDADD		= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@
else
Serialds_LDADD		= @TACO_SERVER_CXX_LIBS@
endif

bin_PROGRAMS 		= Serialds

Serialds_SOURCES 	= Serial.cpp startup.cpp

CMDS_res_DATA 		= SerialCmnds.res

ERROR_res_DATA 		= SerialErrors.res

template_DATA 		= Serial.res

EXTRA_DIST 		= Serial.res SerialCmnds.res SerialErrors.res
