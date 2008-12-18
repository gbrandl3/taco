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

SUBDIRS 		= dbase xdr signal
			  

if DC_BUILD
SUBDIRS			+= dc
endif

if ASCII_BUILD
SUBDIRS 		+= ascii tc
endif

if BUILD_DATAPORT
SUBDIRS			+= dataport
endif

SUBDIRS			+= .

if TANGO_BUILD
SUBDIRS			+= tango
endif

if BUILD_LOG4CPP
SUBDIRS			+= log4taco
endif

nobase_include_HEADERS  = private/ApiP.h

include_HEADERS 	= API.h DevSec.h hdb.h hdb_out.h Admin.h DevCmds.h DevErrors.h DevStates.h DserverTeams.h macros.h

noinst_HEADERS		= taco_utils.h _count.h

lib_LTLIBRARIES 	= libtaco.la libtaco++.la libtacomain.la

noinst_LTLIBRARIES 	= libtacoc.la 

INCLUDES		= @TACO_INCLUDES@ 

## AM_CPPFLAGS 		= -DEBUG

libtacoc_la_SOURCES 	= gen_api.c dev_cmds.c dev_errors.c dev_states.c oic.c \
			util_api.c asyn_api.c gettrans.c ds_prog_vers3.c DevServer.c taco_utils.c timeouts.c devserver_prog_4.c
libtacoc_la_LIBADD	= @PTHREAD_LIBS@

libtacoc_la_LDFLAGS 	= -version-info @LIB_VERSION@
if CLIENT_TCP
libtacoc_la_CFLAGS 	= -DCLIENT_TCP
endif

EXTRA_DIST 		= 

SRC_TACO_SPECIAL 	= dev_api.c sec_api.c event_api.c \
			svc_api.c DevServerSig.c  event_relisten.c startup.c

libtaco_la_SOURCES 	= $(SRC_TACO_SPECIAL) 
libtaco_la_LIBADD  	= xdr/libdsxdr_all.la dbase/libdbapi.la signal/libsignal.la libtacoc.la -lc
if DC_BUILD
libtaco_la_LIBADD	+= dc/libdcapi.la
endif
libtaco_la_LDFLAGS 	= -version-info @LIB_VERSION@ --with-pic

libtaco___la_SOURCES 	= $(SRC_TACO_SPECIAL) Device.cpp OICDevice.cpp 
libtaco___la_CFLAGS  	= -x c++
libtaco___la_LIBTOOLFLAGS = --tag=CXX
libtaco___la_LIBADD  	= xdr/libdsxdr_all.la dbase/libdbapi.la signal/libsignal.la libtacoc.la @LIB_STDCXX@
if DC_BUILD
libtaco___la_LIBADD	+= dc/libdcapi.la
endif
libtaco___la_LDFLAGS 	= -version-info @LIB_VERSION@

libtacomain_la_SOURCES 	= DevServerMain.c 
if BUILTIN_STARTUP
#libtacomain_la_SOURCES 	+= startup.c
endif
libtacomain_la_LDFLAGS 	= -version-info @LIB_VERSION@

include_HEADERS		+= BlcDsNumbers.h CntrlDsNumbers.h DasDsNumbers.h MachDsNumbers.h SysNumbers.h 

include_HEADERS		+= Device.h OICDevice.h 
include_HEADERS		+= DevServer.h  DevServerP.h  DevSignal.h  NT_debug.h  Starter.h

EXTRA_DIST		+= Frm2DsNumbers.h ExpDsNumbers.h DevCcdCmds.h
EXTRA_DIST		+= NT_debug.c  Doxyfile.in

doc:	Doxyfile $(include_HEADERS) $(libtaco_la_SOURCES) $(libtaco___la_SOURCES) 
	doxygen
