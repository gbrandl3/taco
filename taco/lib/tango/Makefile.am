SUBDIRS 		= 

include_HEADERS 	= 

lib_LTLIBRARIES 	= libtacotango++.la 

INCLUDES		= @TACO_INCLUDES@ @CORBA_INCLUDES@ @TANGO_INCLUDES@ -I../signal

AM_CPPFLAGS 		= -DTANGO

EXTRA_DIST 		= 

libtacotango___la_SOURCES = ../dev_api.c ../sec_api.c ../event_api.c \
			../svc_api.c ../DevServerSig.c  ../event_relisten.c \
			../gen_api.c ../dev_cmds.c ../dev_errors.c ../dev_states.c \
			../oic.c ../util_api.c ../asyn_api.c ../gettrans.c \
			../ds_prog_vers3.c ../DevServer.c \
			../Device.cpp ../OICDevice.cpp \
			../signal/DSSignal.c ../signal/MDSSignal.c \
			../signal/MDSSignal_config.c \
			tango_api.cpp attr_api.cpp attr_access.cpp
libtacotango___la_CFLAGS  	= -x c++ -DTANGO
libtacotango___la_LIBADD  	= ../xdr/libdsxdr.la  ../dbase/libdbapi.la \
				../dc/libdcapi.la -lstdc++ 
libtacotango___la_LDFLAGS 	= -version-info @LIB_VERSION@

include_HEADERS		+= ../BlcDsNumbers.h ../CntrlDsNumbers.h ../DasDsNumbers.h ../MachDsNumbers.h ../SysNumbers.h 

include_HEADERS		+= ../DeviceBase.h ../Device.h ../OICDevice.h 
include_HEADERS		+= ../DevServer.h  ../DevServerP.h  ../DevSignal.h  ../NT_debug.h  ../Starter.h
include_HEADERS		+= ../signal/MDSSignal.h 
include_HEADERS		+= attr_api.h

EXTRA_DIST		+= ../DeviceFRM.h ../OICDeviceFRM.h ../DeviceFRM.cpp ../OICDeviceFRM.cpp 
EXTRA_DIST		+= ../NT_debug.c  ../Doxyfile.in

doc:	Doxyfile $(include_HEADERS) $(libtacotango___la_SOURCES) 
	doxygen
