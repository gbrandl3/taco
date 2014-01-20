# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2014 Jens Krüger <jkrueger1@users.sf.net>
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

DISTCHECK_CONFIGURE_FLAGS = --with-appdefaultdir=\$${datadir}/X11/app-defaults

SUBDIRS		= doc

EXTRA_DIST	= bitmaps 

SRC 		= xdev_bxutil.c xdev_creation.c xdev_gui.c xdevmenu.uil\
  		xdev_cb.c xdev_dev.c xdev_gui.h\
  		XDevmenu xdev_const.h xdev_dev.h xdev_main.c	
		
bin_PROGRAMS	= xdevmenu

# appdefaultdir	= $(datadir)/taco/app-defaults
appdefault_DATA	= XDevmenu 

EXTRA_DIST	+= ${appdefault_DATA} xdevmenu.uil

AM_CPPFLAGS	= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@

AM_CPPFLAGS	+= @MOTIF_INCLUDES@ @TACO_INCLUDES@

LDADD		= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ @X_EXTRA_LIBS@ @LIB_X11@ -lXt @TACO_ASCII_LIBS@ # -ldl

xdevmenu_SOURCES= $(SRC)

