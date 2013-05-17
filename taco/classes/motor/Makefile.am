# Server for a (stepper/servo) motor
# Copyright (C) 2007 Jens Krüger

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

AM_CPPFLAGS		= @TACO_INCLUDES@ -I$(top_srcdir)/ext/include @LOG4CPP_CFLAGS@ @LOG4TACO_CFLAGS@


include_HEADERS          = TACOMotor.h TACOMotorCommon.h TACOMotorWorker.h

lib_LTLIBRARIES 	= libTACOMotor.la

libTACOMotor_la_SOURCES = TACOMotor.cpp TACOMotorWorker.cpp

libTACOMotor_la_LIBADD  = $(top_builddir)/ext/src/libTACOExtensions.la @TACO_SERVER_CXX_LIBS@ ## @LOG4CPP_LIBS@ ## -lstdc++

bin_PROGRAMS            = TACOMotor 

TACOMotor_SOURCES       = startup.cpp

TACOMotor_LDADD         = libTACOMotor.la

