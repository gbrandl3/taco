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

bin_PROGRAMS		= devoff \
			devon \
			devstate \
			devstatus \
			devreset 

bin_PROGRAMS		+= devclose \
 			devopen \
 			devlocal \
 			devremote \
 			devrun \
 			devstandby \
 			devreadstate \
 			devreadvalue \
			devread

##  			devramp 

bin_PROGRAMS		+= dev_menu \
			dev_cmd

bin_PROGRAMS		+= devexp \
 			devupdate \
 			db_getpoller \
 			dc_signals \
 			poll_test \
 			readsignals 

if TANGO_BUILD
AM_CPPFLAGS		= @TACO_INCLUDES@
LDADD			= @TANGO_LIBS@ @CORBA_LDFLAGS@ @CORBA_LIBS@ @TANGO_CLIENT_CXX_LIBS@
else
AM_CPPFLAGS		= @TACO_INCLUDES@ 
LDADD			= @TACO_CLIENT_LIBS@ 
endif

# noinst_HEADERS		= xdrarg.h \
#			tacodevice.h

# dev_cmd_SOURCES		= dev_cmd.cpp \
# 			xdrarg.cpp \
# 			tacodevice.cpp

devoff_SOURCES		= devoff.c
devon_SOURCES		= devon.c
devstate_SOURCES	= devstate.c
devstatus_SOURCES	= devstatus.c
devreset_SOURCES	= devreset.c

dev_menu_SOURCES	= dev_menu.c
dev_cmd_SOURCES		= dev_cmd.c


devopen_SOURCES		= devopen.c
devclose_SOURCES	= devclose.c
devremote_SOURCES	= devremote.c
devlocal_SOURCES	= devlocal.c
devreadstate_SOURCES	= devreadstate.c
devreadvalue_SOURCES	= devreadvalue.c
devread_SOURCES		= devread.c
devrun_SOURCES		= devrun.c
devstandby_SOURCES	= devstandby.c
## devramp_SOURCES		= devramp.c


db_getpoller_SOURCES	= db_getpoller.c
dc_signals_SOURCES	= dc_signals.c
devexp_SOURCES		= devexp.c
devupdate_SOURCES	= devupdate.c
poll_test_SOURCES	= poll_test.c
readsignals_SOURCES	= readsignals.c
