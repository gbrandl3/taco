# Extensions for the convenient access to TACO
# Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

SUBDIRS = lib common server client doc

ACLOCAL_AMFLAGS = -I m4 -I @DSHOME@/share/aclocal

DISTCHECK_CONFIGURE_FLAGS= DSHOME=@DSHOME@

# This is necessary because 'doc' is a directory
empty:

doc: 
	make -C doc doc

install-doc:
	make -C doc install-doc

uninstall-doc:
	make -C doc uninstall-doc

rpm: dist
	$(RPM_PROG) $(RPM_ARGS) $(RPM_TARBALL)

.PHONY:	doc
