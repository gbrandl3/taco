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

sbin_PROGRAMS	=dc_wr_alo dc_rd_alo

AM_CPPFLAGS	= -DALONE
INCLUDES	=-I$(top_srcdir)/dbase/include @TACO_INCLUDES@ -I$(top_srcdir)/lib/dataport/
LIBDC		=../library/libdcmem.la
LIBDATAPORT	=../../../lib/dataport/libdataport.la 

dc_wr_alo_SOURCES=dc_devall.c \
		dc_devinf.c \
		dc_inf.c \
		dc_put.c \
		dc_serv.c \
		dcwr_svc.c \
		dcwr_xdr.c 

dc_wr_alo_LDADD	= @TACO_SERVER_LIBS@ $(LIBDC) $(LIBDATAPORT)

dc_rd_alo_SOURCES=dc_devall.c \
		dc_devinf.c \
		dc_inf.c \
		dc_read.c \
		dcrd_svc.c \
		dcrd_xdr.c 

dc_rd_alo_LDADD	= @TACO_SERVER_LIBS@ $(LIBDC) $(LIBDATAPORT)
