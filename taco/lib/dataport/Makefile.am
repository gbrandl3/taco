include_HEADERS			= boolean.h  dataport.h  dataportP.h  sema.h  shared.h

EXTRA_libdataport_la_SOURCES	= dataport_ux.c dataport_os9.c shared.c sema.c

lib_LTLIBRARIES			= libdataport.la

if DATAPORT_UNIX
libdataport_la_SOURCES		= dataport_ux.c shared.c sema.c 
else
libdataport_la_SOURCES		= dataport_os9.c
endif

libdataport_la_LDFLAGS		= -version-info @LIB_VERSION@

INCLUDES			= @TACO_INCLUDES@
