lib_LTLIBRARIES=libdcmem.la

libdcmem_la_SOURCES=	dc_hash.c \
		dcmem_alloc.c \
		dcmem_free.c \
		dcmem_info.c

libdcmem_la_LDFLAGS=-version-info @LIB_VERSION@

INCLUDES=-I$(top_srcdir)/dbase/include @TACO_INCLUDES@
