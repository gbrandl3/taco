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

include_HEADERS 	= API.h DevSec.h hdb.h hdb_out.h Admin.h DevCmds.h DevErrors.h DevStates.h DserverTeams.h 

lib_LTLIBRARIES 	= libtacoapi.la libdsapi.la libdsapinomain.la libtacoapig++.la libdsapig++.la 

noinst_LTLIBRARIES 	= libtacoc.la libdsapic.la

INCLUDES		= -I$(top_srcdir)/dbase/include @TACO_INCLUDES@

AM_CPPFLAGS 		= -DEBUG

libtacoc_la_SOURCES 	= gen_api.c util_api.c dev_errors.c dev_states.c event_relisten.c oic.c 
libtacoc_la_LDFLAGS 	= -version-info $(subst .,:,@LIB_VERSION@)
libtacoc_la_LIBADD  	= xdr/libdsxdr.la

EXTRA_DIST 		= 

SRC_TACO_SPECIAL 	= asyn_api.c dev_api.c sec_api.c event_api.c \
			svc_api.c DevServerSig.c  gettrans.c 

libtacoapi_la_SOURCES 	= $(SRC_TACO_SPECIAL) 
libtacoapi_la_CFLAGS  	= -DCLIENTONLY 
libtacoapi_la_LIBADD  	= ./libtacoc.la
libtacoapi_la_LDFLAGS 	= -version-info $(subst .,:,@LIB_VERSION@)

libtacoapig___la_SOURCES = $(SRC_TACO_SPECIAL)
libtacoapig___la_CFLAGS  = -DCLIENTONLY -x c++
libtacoapig___la_LIBADD  = ./libtacoc.la
libtacoapig___la_LDFLAGS = -version-info $(subst .,:,@LIB_VERSION@)

libdsapic_la_SOURCES	= ds_prog_vers3.c oic.c \
			DevServer.c DevServerSig.c\
			gettrans.c startup.c  


libdsapinomain_la_CPPFLAGS	= -DNOMAIN
libdsapinomain_la_SOURCES 	= DevServerMain.c 
libdsapinomain_la_LDFLAGS 	= -version-info $(subst .,:,@LIB_VERSION@)
libdsapinomain_la_LIBADD  	= ./libdsapic.la ./libtacoapi.la 

libdsapi_la_SOURCES 	= DevServerMain.c 
libdsapi_la_CPPFLAGS  	= -UCLIENTONLY 
libdsapi_la_LDFLAGS 	= -version-info $(subst .,:,@LIB_VERSION@)
libdsapi_la_LIBADD  	= ./libdsapic.la ./libtacoapi.la 

libdsapig___la_SOURCES 	= Device.C OICDevice.C 

libdsapig___la_CFLAGS  	= -UCLIENTONLY  -x c++
libdsapig___la_LDFLAGS 	= -version-info $(subst .,:,@LIB_VERSION@)
libdsapig___la_LIBADD  	= ./libdsapi.la ./libtacoapig++.la 
include_HEADERS		+= BlcDsNumbers.h CntrlDsNumbers.h DasDsNumbers.h MachDsNumbers.h SysNumbers.h 

include_HEADERS		+= Device.H OICDevice.H 
include_HEADERS		+= DevServer.h  DevServerP.h  DevSignal.h  NT_debug.h  Starter.h

EXTRA_DIST		+= DeviceFRM.H OICDeviceFRM.H DeviceFRM.C OICDeviceFRM.C startup.cpp 
EXTRA_DIST		+= NT_debug.c 


