#
# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2004-2013 Jens Krüger <jkrueger1@users.sf.net>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
SUBDIRS 		= demo.server doc lib

EXTRA_DIST 		= README Taco.pth h2py

nodist_pyexec_PYTHON	= DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py BLCDSNUMBERS.py
pyexec_PYTHON		= ResDatabase.py TacoDevice.py TacoServer.py

CLEANFILES 		= DEVCMDS.py DEVERRORS.py DEVSTATES.py DEV_XDR.py BLCDSNUMBERS.py *.pyc *.pyo

DEVCMDS.py:	$(top_srcdir)/lib/DevCmds.h
	python $(srcdir)/h2py $?

DEVERRORS.py:	$(top_srcdir)/lib/DevErrors.h 
	python $(srcdir)/h2py $?

DEVSTATES.py:	$(top_srcdir)/lib/DevStates.h 
	python $(srcdir)/h2py $?

DEV_XDR.py:	$(top_srcdir)/lib/xdr/dev_xdr.h
	python $(srcdir)/h2py $?

BLCDSNUMBERS.py:	$(top_srcdir)/lib/BlcDsNumbers.h
	python $(srcdir)/h2py $?
