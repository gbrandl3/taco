# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2014 Jens Krüger <jkrueger1@users.sf.net>
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

DISTCHECK_CONFIGURE_FLAGS = --enable-ext --enable-gdbm --enable-sqlite3
DISTCHECK_CONFIGURE_FLAGS += --with-appdefaultdir=\$${DESTDIR}/\$${datadir}/X11/app-defaults

ACLOCAL_AMFLAGS = -I m4 -I log4cpp -I log4cpp/m4

SUBDIRS = m4 

if BUILD_GDBM
SUBDIRS += gdbm
endif

if BUILD_LOG4CPP
SUBDIRS += log4cpp
endif

SUBDIRS += lib

if BUILD_EXT
SUBDIRS += ext
endif

SUBDIRS += system

if BUILD_SERVER
SUBDIRS += classes
endif

SUBDIRS += doc misc bindings tests examples

install-data-hook:
	@if test ! -f $(DESTDIR)/$(includedir)/private/config.h ; then \
		echo $(INSTALL_HEADER) config.h $(DESTDIR)$(includedir)/private ; \
		$(INSTALL_HEADER) config.h $(DESTDIR)$(includedir)/private ; \
	fi

uninstall-hook:
	@if test -f $(DESTDIR)/$(includedir)/private/config.h ; then \
		$(RM) $(DESTDIR)/$(includedir)/private/config.h ; \
	fi

EXTRA_DIST	= autogen.sh taco.spec.in README-RPM

EXTRA_DIST	+= debian/changelog debian/compat debian/control debian/copyright \
		   debian/docs debian/README debian/rules debian/taco-dev.install \
		   debian/taco.dirs debian/taco.install debian/source/format

include $(top_srcdir)/rpm_rules.mh

DISTCLEANFILES = pathes.h
