pyexec_LTLIBRARIES	= Taco.la

Taco_la_LDFLAGS 	= -module 

## -avoid-version

INCLUDES 		= @TACO_INCLUDES@ @PYTHON_INCLUDES@ @PYTHON_INCLUDES@/Numeric

Taco_la_LIBADD 		= $(top_builddir)/lib/libdsapinomain.la $(top_builddir)/lib/dbase/libdbapi.la \
		$(top_builddir)/lib/xdr/libdsxdr_all.la $(top_builddir)/lib/dc/libdcapi.la @PYTHON_LIBS@

AM_CPPFLAGS 		= -DMYDEB -DNUMPY 

noinst_HEADERS		= taco.h python.h pythonP.h

Taco_la_SOURCES		= taco.c taco_types.c server.c python.c

#		Module library
SH_LIB   = 	Taco.so
MOD_LIB  =      Server.so ## ln -s $(SH_LIB) $(MOD_LIB)

