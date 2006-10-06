# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2005 Jens Krüger <jkrueger1@users.sf.net>
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

DISTCHECK_CONFIGURE_FLAGS = --enable-ext
ACLOCAL_AMFLAGS = -I m4

if BUILD_GDBM
gdbm=gdbm
else
gdbm=
endif

if BUILD_SERVER
classes=classes
else
classes=
endif

if BUILD_EXT
ext=ext
else
ext=
endif

SUBDIRS = m4 $(gdbm) lib $(ext) system $(classes) doc misc bindings tests 

EXTRA_DIST=autogen.sh taco.spec.in README-RPM

include $(top_srcdir)/rpm_rules.mh

DISTCLEANFILES = pathes.h
