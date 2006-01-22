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

INCLUDES=@TACO_INCLUDES@ -DDEBUG

LDADD = @TACO_CLIENT_LIBS@ ../library/libdcmem.la 

bin_PROGRAMS = 	dc_del \
		dc_dels \
		dc_devinfo \
		dc_hash \
		dc_info \
		dc_init \
		dc_inits \
		dc_mem \
		dc_mfree \
		dc_mfrees \
		dc_devall \
		dc_patch \
		dc_cmd

##		dc_test \

dc_del_SOURCES = dc_del.c

dc_dels_SOURCES = dc_dels.c

dc_devall_SOURCES = dc_devall.c

dc_devinfo_SOURCES = dc_devinfo.c

dc_hash_SOURCES = dc_hash.c

dc_info_SOURCES = dc_info.c

dc_init_SOURCES = dc_init.c

dc_inits_SOURCES = dc_inits.c

dc_mem_SOURCES = dc_mem.c

dc_mfree_SOURCES = dc_mfree.c

dc_mfrees_SOURCES = dc_mfrees.c

dc_patch_SOURCES = dc_patch.c

dc_cmd_SOURCES = dc_cmd.c

## dc_test_SOURCES = dc_test.c
