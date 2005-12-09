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

if BUILD_DATAPORT
 datap_dir=dataport
else
 datap_dir=
endif

if ASCII_BUILD
 ascii_dir=ascii tc
else
 ascii_dir=
endif

if DC_BUILD
 dc_dir=dc
 dc_lib=dc/libdcapi.la
else
 dc_dir=
 dc_lib=
endif

if TANGO_BUILD
 tango_dir=tango
else
 tango_dir=
endif

SUBDIRS 		= dbase xdr signal $(dc_dir) $(ascii_dir) $(datap_dir) \
			  $(tango_dir) private .

include_HEADERS 	= API.h DevSec.h hdb.h hdb_out.h Admin.h DevCmds.h DevErrors.h DevStates.h DserverTeams.h macros.h

lib_LTLIBRARIES 	= libtaco.la libtaco++.la libtacomain.la

noinst_LTLIBRARIES 	= libtacoc.la 

INCLUDES		= @TACO_INCLUDES@ 

## AM_CPPFLAGS 		= -DEBUG

libtacoc_la_SOURCES 	= gen_api.c dev_cmds.c dev_errors.c dev_states.c oic.c \
			util_api.c asyn_api.c gettrans.c ds_prog_vers3.c DevServer.c 

libtacoc_la_LDFLAGS 	= -version-info @LIB_VERSION@
#libtacoc_la_LIBADD  	= xdr/libdsxdr.la dbase/libdbapi.la signal/libsignal.la -lc

EXTRA_DIST 		= 

SRC_TACO_SPECIAL 	= dev_api.c sec_api.c event_api.c \
			svc_api.c DevServerSig.c  event_relisten.c startup.c

libtaco_la_SOURCES 	= $(SRC_TACO_SPECIAL) 
libtaco_la_LIBADD  	= xdr/libdsxdr_all.la dbase/libdbapi.la signal/libsignal.la $(dc_lib) libtacoc.la -lc
libtaco_la_LDFLAGS 	= -version-info @LIB_VERSION@

libtaco___la_SOURCES 	= $(SRC_TACO_SPECIAL) Device.cpp OICDevice.cpp 
libtaco___la_CFLAGS  	= -x c++
libtaco___la_LIBADD  	= xdr/libdsxdr_all.la dbase/libdbapi.la signal/libsignal.la $(dc_lib) libtacoc.la -lstdc++
libtaco___la_LDFLAGS 	= -version-info @LIB_VERSION@

libtacomain_la_SOURCES 	= DevServerMain.c 
if BUILTIN_STARTUP
#libtacomain_la_SOURCES 	+= startup.c
endif
libtacomain_la_LDFLAGS 	= -version-info @LIB_VERSION@

include_HEADERS		+= BlcDsNumbers.h CntrlDsNumbers.h DasDsNumbers.h MachDsNumbers.h SysNumbers.h 

include_HEADERS		+= Device.h OICDevice.h 
include_HEADERS		+= DevServer.h  DevServerP.h  DevSignal.h  NT_debug.h  Starter.h

EXTRA_DIST		+= DeviceFRM.h OICDeviceFRM.h DeviceFRM.cpp OICDeviceFRM.cpp 
EXTRA_DIST		+= NT_debug.c  Doxyfile.in

doc:	Doxyfile $(include_HEADERS) $(libtaco_la_SOURCES) $(libtaco___la_SOURCES) 
	doxygen
