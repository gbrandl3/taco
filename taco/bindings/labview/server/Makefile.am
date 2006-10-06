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

lib_LTLIBRARIES		= liblvdsclass.la liblvdsapi.la libtaco.la 
#noinst_LTLIBRARIES	= libtaco.la

LV_FLAGS = -DLV_DEBUG -DLV_DC -DLABVIEW -DNOMAIN

TACO_LDFLAGS_EXT = -L$(top_srcdir)/lib -L$(top_srcdir)/lib/dbase -L$(top_srcdir)/lib/dc
LIB_TACO_EXT = -ldsapi -ldcapi -ldbapi -ltacoapi
AM_CPPFLAGS = @TACO_INCLUDES@ $(LV_FLAGS)
#INCLUDES	= @TACO_INCLUDES@

include_HEADERS = LabViewGeneric.h LabViewGenericP.h

liblvdsapi_la_SOURCES	= lv_dsapi.c extcode.h
liblvdsapi_la_LDFLAGS = $(LV_FLAGS) -export-dynamic
liblvdsapi_la_LIBADD = libtaco.la

liblvdsclass_la_SOURCES	= lv_dsclass.c LabViewGeneric.h LabViewGenericP.h
liblvdsclass_la_LDFLAGS = $(LV_FLAGS) -export-dynamic
liblvdsclass_la_LIBADD = libtaco.la @LIB_TACO@

libtaco_la_SOURCES = lv_dsapi.c lv_dsclass.c 
libtaco_la_LDFLAGS = $(LV_FLAGS) $(TACO_LDFLAGS_EXT) -export-dynamic @TACO_LDFLAGS@  @QT_LDFLAGS@ 
libtaco_la_LIBADD = $(LIB_TACO_EXT) @LIB_TACO@
#libdsxdr_la_CFLAGS	=
#liblvdsclass_la_LDFLAGS	= -version-info @LIB_VERSION@


rescmdsdir 		= ${prefix}/dbase/res/CMDS
rescmds_DATA 	= LabviewGenericCmds.res

restestdir 		= ${prefix}/dbase/res/TEST
restest_DATA 	= LabviewGeneric.res

EXTRA_DIST 		= LabviewGeneric.res LabviewGenericCmds.res

