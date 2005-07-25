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

if XDEVMENUBUILD
 xdev_dir=xdevmenu
else
 xdev_dir=
endif

if ALARMBUILD
 alarm_dir=alarm
else
 alarm_dir=
endif

if DC_BUILD
  dc_dir=dc
else
  dc_dir=
endif

if BUILD_SERVER
starter=starter
else
starter=
endif

if BUILD_MANAGER
manager=manager
else
manager=
endif

if BUILD_MESSAGESERVER
messageserver=msg
else
messageserver=
endif

SUBDIRS = . dbase $(manager) $(messageserver) $(dc_dir) $(starter) $(xdev_dir) $(alarm_dir) $(dbdev_dir)

