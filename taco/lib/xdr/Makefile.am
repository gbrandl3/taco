include_HEADERS		= API_xdr.h API_xdr_vers3.h DevServerXdr.h DevXdrKernel.h xdr_typelist.h \
			atte_xdr.h bpm_xdr.h bpss_xdr.h ct_xdr.h daemon_xdr.h dev_xdr.h \
			gpib_xdr.h grp_xdr.h haz_xdr.h icv101_xdr.h m4_xdr.h maxe_xdr.h \
			mclock_xdr.h mstat_xdr.h pin_xdr.h pss_xdr.h ram_xdr.h \
			rf_xdr.h seism_xdr.h slit_xdr.h thc_xdr.h union_xdr.h vgc_xdr.h \
			vrif_xdr.h ws_xdr.h 

lib_LTLIBRARIES		= libdsxdr.la libdsxdr_all.la
noinst_LTLIBRARIES	= libdsxdrbase.la

INCLUDES		= @TACO_INCLUDES@

KERNEL_SOURCE		= DevXdrKernel.c 

libdsxdrbase_la_SOURCES	= api_xdr.c api_xdr_vers3.c dev_xdr.c 

libdsxdr_la_SOURCES	= $(KERNEL_SOURCE) 
libdsxdr_la_CFLAGS	=
libdsxdr_la_LIBADD  	= libdsxdrbase.la
libdsxdr_la_LDFLAGS	= -version-info @LIB_VERSION@

libdsxdr_all_la_SOURCES	= $(KERNEL_SOURCE) atte_xdr.c bpm_xdr.c bpss_xdr.c ct_xdr.c gpib_xdr.c grp_xdr.c \
			haz_xdr.c icv101_xdr.c m4_xdr.c maxe_xdr.c mstat_xdr.c pin_xdr.c pss_xdr.c \
                	ram_xdr.c rf_xdr.c seism_xdr.c slit_xdr.c thc_xdr.c union_xdr.c vgc_xdr.c vrif_xdr.c ws_xdr.c 
libdsxdr_all_la_CFLAGS	= -D_XDR_ALL
libdsxdr_all_la_LIBADD  = libdsxdrbase.la
libdsxdr_all_la_LDFLAGS	= -version-info @LIB_VERSION@
