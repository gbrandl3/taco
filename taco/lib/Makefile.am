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
else
 dc_dir=
endif

SUBDIRS 		= dbase xdr $(dc_dir) $(ascii_dir) $(datap_dir) private .

include_HEADERS 	= API.h DevSec.h hdb.h hdb_out.h Admin.h DevCmds.h DevErrors.h DevStates.h DserverTeams.h macros.h

lib_LTLIBRARIES 	= libtaco.la libtaco++.la libtacomain.la

noinst_LTLIBRARIES 	= libtacoc.la 

INCLUDES		= @TACO_INCLUDES@

## AM_CPPFLAGS 		= -DEBUG

libtacoc_la_SOURCES 	= gen_api.c dev_errors.c dev_states.c oic.c util_api.c asyn_api.c gettrans.c \
			ds_prog_vers3.c DevServer.c 

libtacoc_la_LDFLAGS 	= -version-info @LIB_VERSION@
libtacoc_la_LIBADD  	= dbase/libdbapi.la xdr/libdsxdr.la -lc

EXTRA_DIST 		= 

SRC_TACO_SPECIAL 	= dev_api.c sec_api.c event_api.c \
			svc_api.c DevServerSig.c  event_relisten.c 

libtaco_la_SOURCES 	= $(SRC_TACO_SPECIAL) 
libtaco_la_LIBADD  	= libtacoc.la 
libtaco_la_LDFLAGS 	= -version-info @LIB_VERSION@

libtaco___la_SOURCES 	= $(SRC_TACO_SPECIAL) Device.C OICDevice.C 
libtaco___la_CFLAGS  	= -x c++
libtaco___la_LIBADD  	= libtacoc.la -lstdc++ 
libtaco___la_LDFLAGS 	= -version-info @LIB_VERSION@

libtacomain_la_SOURCES 	= DevServerMain.c 
if BUILTIN_STARTUP
libtacomain_la_SOURCES 	+= startup.c
endif
libtacomain_la_LDFLAGS 	= -version-info @LIB_VERSION@

include_HEADERS		+= BlcDsNumbers.h CntrlDsNumbers.h DasDsNumbers.h MachDsNumbers.h SysNumbers.h 

include_HEADERS		+= DeviceBase.h Device.H OICDevice.H 
include_HEADERS		+= DevServer.h  DevServerP.h  DevSignal.h  NT_debug.h  Starter.h

EXTRA_DIST		+= DeviceFRM.H OICDeviceFRM.H DeviceFRM.C OICDeviceFRM.C 
EXTRA_DIST		+= NT_debug.c  Doxyfile.in

doc:	Doxyfile $(include_HEADERS) $(libtaco_la_SOURCES) $(libtaco___la_SOURCES) 
	doxygen
