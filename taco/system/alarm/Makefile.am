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

sbin_PROGRAMS = S_Alarm 

S_Alarm_SOURCES =  getlastmessage.cpp

S_Alarm_SOURCES += motifalarm.cpp
S_Alarm_CPPFLAGS = @MOTIF_INCLUDES@ @X_CFLAGS@
S_Alarm_LDADD = @MOTIF_LIBS@ @X_LIBS@ -lXt -lX11

# S_Alarm_SOURCES = qtalarm.cpp 
# S_Alarm_CPPFLAGS = -I$(QTDIR)/include
# S_Alarm_LDADD = -L$(QTDIR)/lib -lqt-mt

