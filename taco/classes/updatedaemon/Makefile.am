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

noinst_HEADERS 		= daemon.h daemon_xdr.h ud_server.h ud_serverP.h
INCLUDES		= @TACO_INCLUDES@ -I$(top_srcdir)/lib/dataport/

LDADD			= @TACO_SERVER_LIBS@ ../../lib/dataport/libdataport.la 

bin_PROGRAMS 		= ud_daemon ud_poller ud_daemon_menu

ud_daemon_SOURCES  	= daemon.c startup.c daemon_xdr.c MakeDP.c

ud_daemon_menu_SOURCES = daemon_menu.c daemon_xdr.c

if TANGO_POLLER
override CC		= $(CXX)
ud_poller_SOURCES  	= poller.c MakeDP.c datac.c
ud_poller_LDADD		= @TANGO_CLIENT_CXX_LIBS@ ../../lib/dataport/libdataport.la 
else
ud_poller_SOURCES  	= poller.c MakeDP.c datac.c
ud_poller_LDADD		= @TACO_CLIENT_LIBS@ ../../lib/dataport/libdataport.la
#                          ../../lib/dc/libdcapi.la
endif
