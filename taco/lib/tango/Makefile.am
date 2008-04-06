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

lib_LTLIBRARIES 	= libtacotango++.la 

INCLUDES		= @TACO_INCLUDES@ @CORBA_CFLAGS@ @TANGO_INCLUDES@

AM_CPPFLAGS 		= -DTANGO

EXTRA_DIST 		= 

libtacotango___la_SOURCES = ../dev_api.c ../sec_api.c ../event_api.c \
			../svc_api.c ../DevServerSig.c  ../event_relisten.c \
			../gen_api.c ../dev_cmds.c ../dev_errors.c ../dev_states.c \
			../oic.c ../util_api.c ../asyn_api.c ../gettrans.c \
			../ds_prog_vers3.c ../DevServer.c \
			../Device.cpp ../OICDevice.cpp \
			../signal/DSSignal.c ../signal/MDSSignal.c \
			../signal/MDSSignal_config.c \
			../dc/dc_sdev.c ../dc/dcrd_clnt.c ../dc/dcwr_clnt.c \
			../dc/dchist_cli.c ../dc/dcrd_xdr.c ../dc/dcwr_xdr.c \
			../dc/dcrd_cli.c ../dc/dcwr_cli.c ../dc/utils_cli.c \
			tango_api.cpp attr_api.cpp attr_access.cpp \
			db_exp.cpp ../startup.c ../taco_utils.c ../timeouts.c

libtacotango___la_CFLAGS  = -x c++ -DTANGO
if CLIENT_TCP
libtacotango___la_CFLAGS += -DCLIENT_TCP
endif
libtacotango___la_LIBADD  = ../xdr/libdsxdr_all.la  ../dbase/libdbapi.la \
				@TANGO_LIBS@ @CORBA_LDFLAGS@ @CORBA_LIBS@ 
if SOLARIS_CXX
libtacotango___la_LIBADD += -lCstd
else
libtacotango___la_LIBADD += -lstdc++ 
endif
libtacotango___la_LDFLAGS = -version-info @LIB_VERSION@

include_HEADERS		  = attr_api.h

