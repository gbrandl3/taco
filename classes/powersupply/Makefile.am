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

noinst_HEADERS 		= PowerSupply.h AGPowerSupply.h

AM_CXXFLAGS     	= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@

AM_CFLAGS       	= @TACO_INCLUDES@

noinst_LTLIBRARIES 	= libPowersupply.la

libPowersupply_la_SOURCES = PowerSupply.cpp

if SOLARIS_CXX
AGPSpp_LDADD		= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@ \
			  libPowersupply.la
else
AGPSpp_LDADD		= @TACO_SERVER_CXX_LIBS@ \
			  libPowersupply.la 
endif

bin_PROGRAMS 		= AGPSpp ps_menu

AGPSpp_SOURCES 		= AGPowerSupply.cpp startup.cpp

ps_menu_SOURCES 	= ps_menu.cpp

ps_menu_LDADD		= @TACO_CLIENT_CXX_LIBS@ libPowersupply.la 
