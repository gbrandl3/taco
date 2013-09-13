# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2013 Jens Krüger <jkrueger1@users.sf.net>
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

SUBDIRS = . dbase tools

if BUILD_MANAGER
SUBDIRS += manager
endif

if BUILD_MESSAGESERVER
SUBDIRS += msg
endif

if BUILD_EXT
if BUILD_SERVER
SUBDIRS += starter
endif
endif

if DC_BUILD
SUBDIRS += dc
endif

if XDEVMENUBUILD
SUBDIRS += xdevmenu
endif

if ALARMBUILD
SUBDIRS += alarm
endif

SUBDIRS += testcs

SUBDIRS += $(dbdev_dir)
