SUBDIRS 		= demo.server doc lib

EXTRA_DIST 		= README Taco.pth h2py

nodist_pyexec_PYTHON	= DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py 
pyexec_PYTHON		= ResDatabase.py TacoDevice.py TacoServer.py

CLEANFILES 		= DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py *.pyc *.pyo

DEVCMDS.py:	$(top_srcdir)/lib/DevCmds.h
	python $(srcdir)/h2py $?

DEVERRORS.py:	$(top_srcdir)/lib/DevErrors.h 
	python $(srcdir)/h2py $?

DEVSTATES.py:	$(top_srcdir)/lib/DevStates.h 
	python $(srcdir)/h2py $?

DEV_XDR.py:	$(top_srcdir)/lib/xdr/dev_xdr.h
	python $(srcdir)/h2py $?
