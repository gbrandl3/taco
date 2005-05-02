pyexec_LTLIBRARIES	= Server.la Taco.la

Server_la_LDFLAGS 	= -module 

Taco_la_LDFLAGS 	= -module 

INCLUDES 		= @TACO_INCLUDES@ @PYTHON_CPPFLAGS@ @PYTHON_CPPFLAGS@

Server_la_LIBADD 	= $(top_builddir)/lib/libtaco.la $(top_builddir)/lib/xdr/libdsxdr_all.la $(top_builddir)/lib/dc/libdcapi.la @PYTHON_LDFLAGS@

Taco_la_LIBADD 		= $(top_builddir)/lib/libtaco.la $(top_builddir)/lib/xdr/libdsxdr_all.la $(top_builddir)/lib/dc/libdcapi.la @PYTHON_LDFLAGS@

AM_CPPFLAGS 		= -DMYDEB -DNUMPY 

noinst_HEADERS		= taco.h python.h pythonP.h

Server_la_SOURCES	= taco_types.c server.c python.c

Taco_la_SOURCES		= taco.c taco_types.c

