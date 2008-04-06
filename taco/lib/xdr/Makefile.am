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

include_HEADERS		= API_xdr.h API_xdr_vers3.h DevServerXdr.h DevXdrKernel.h xdr_typelist.h \
			atte_xdr.h bpm_xdr.h bpss_xdr.h ct_xdr.h daemon_xdr.h dev_xdr.h \
			gpib_xdr.h grp_xdr.h haz_xdr.h icv101_xdr.h m4_xdr.h maxe_xdr.h \
			mclock_xdr.h mstat_xdr.h pin_xdr.h pss_xdr.h ram_xdr.h \
			rf_xdr.h seism_xdr.h slit_xdr.h thc_xdr.h union_xdr.h vgc_xdr.h \
			vrif_xdr.h ws_xdr.h 

noinst_LTLIBRARIES	= libdsxdr.la libdsxdr_all.la libdsxdrbase.la

INCLUDES		= @TACO_INCLUDES@

KERNEL_SOURCE		= DevXdrKernel.c 

libdsxdrbase_la_SOURCES	= api_xdr.c api_xdr_vers3.c dev_xdr.c 

libdsxdr_la_SOURCES	= $(KERNEL_SOURCE) 
libdsxdr_la_CFLAGS	=
libdsxdr_la_LIBADD  	= libdsxdrbase.la
# libdsxdr_la_LDFLAGS	= -version-info @LIB_VERSION@

libdsxdr_all_la_SOURCES	= $(KERNEL_SOURCE) atte_xdr.c bpm_xdr.c bpss_xdr.c ct_xdr.c gpib_xdr.c grp_xdr.c \
			haz_xdr.c icv101_xdr.c m4_xdr.c maxe_xdr.c mstat_xdr.c pin_xdr.c pss_xdr.c \
                	ram_xdr.c rf_xdr.c seism_xdr.c slit_xdr.c thc_xdr.c union_xdr.c vgc_xdr.c vrif_xdr.c ws_xdr.c \
			daemon_xdr.c

libdsxdr_all_la_CFLAGS	= -D_XDR_ALL
libdsxdr_all_la_LIBADD  = libdsxdrbase.la
# libdsxdr_all_la_LDFLAGS	= -version-info @LIB_VERSION@
