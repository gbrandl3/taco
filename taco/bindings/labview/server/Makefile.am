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

lib_LTLIBRARIES		= lv_dsclass.la lv_dsapi.la 

LV_FLAGS 		= -DLV_DEBUG -DLV_DC -DLABVIEW -DNOMAIN

AM_CPPFLAGS 		= @TACO_INCLUDES@ $(LV_FLAGS) @LABVIEW_INCLUDES@

noinst_HEADERS 		= LabViewGeneric.h LabViewGenericP.h

lv_dsapi_la_SOURCES 	= lv_dsapi.c 
lv_dsapi_la_LDFLAGS 	= $(LV_FLAGS) -module
lv_dsapi_la_LIBADD 	= $(top_builddir)/lib/libtaco.la

lv_dsclass_la_SOURCES	= lv_dsclass.c LabViewGeneric.h LabViewGenericP.h
lv_dsclass_la_LDFLAGS 	= $(LV_FLAGS) -module
lv_dsclass_la_LIBADD 	= $(top_builddir)/lib/libtaco.la 

rescmdsdir 		= ${prefix}/dbase/res/CMDS
rescmds_DATA 		= LabviewGenericCmds.res

restestdir 		= ${prefix}/dbase/res/TEST
restest_DATA 		= LabviewGeneric.res

EXTRA_DIST 		= LabviewGeneric.res LabviewGenericCmds.res

TESTS			= test_lv

check_PROGRAMS		= test_lv
test_lv_SOURCES		= test.c
test_lv_LDFLAGS		= -export-dynamic
test_lv_LDADD		= lv_dsapi.la @LABVIEW_LIBS@
