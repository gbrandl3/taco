lib_LTLIBRARIES = libtcltacoapi.la

libtcltacoapi_la_SOURCES = convert.c \
			db_io_debug.c db_io_get.c db_io_put.c \
			debug.c \
			dev_free.c dev_io.c dev_io_protocol.c dev_io_timeout.c \
			tcltaco_ver.c tcltacoapi_init.c \
			dc_io_debug.c dc_io_get.c 

libtcltacoapi_la_LIBADD = $(top_builddir)/lib/libtaco.la

libtcltacoapi_la_LDFLAGS = -rdynamic

AM_CPPFLAGS = @TACO_INCLUDES@ @TCL_CPPFLAGS@

noinst_HEADERS = db_io.h dc_io.h debug.h dev_io.h
