lib_LTLIBRARIES = libpythontacoapi.la

AM_CPPFLAGS =  	-I$(top_srcdir)/lib  @PYTHON_INCLUDES@ @PYTHON_INCLUDES@/Numeric

noinst_HEADERS = taco.h python.h pythonP.h

libpythontacoapi_la_SOURCES =	taco.c taco_types.c server.c python.c

libpythontacoapi_la_LIBADD = $(top_builddir)/lib/libtaco.la	

noinst_PYTHON =	TacoDevice.py TacoServer.py ResDatabase.py

nodist_python_PYTHON = DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py

CLEANFILES = DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py


DEVCMDS.py:	$(top_srcdir)/lib/DevCmds.h
	./h2py $<

DEVERRORS.py:	$(top_srcdir)/lib/DevErrors.h 
	./h2py $<

DEVSTATES.py:	$(top_srcdir)/lib/DevStates.h 
	./h2py $<

DEV_XDR.py:	$(top_srcdir)/lib/dev_xdr.h
	./h2py $<
