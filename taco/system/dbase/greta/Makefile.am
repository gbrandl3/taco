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

noinst_HEADERS		= greta.h  greta_cstes.h

SRC_GRETA		= greta.c greta_creation.c bxutil-c.c greta_cb.c greta_xs.c \
		greta_misc.c greta_sdd.c greta_win.c servutil.c devutil.c \
		greta_update.c

EXTRA_PROGRAMS		= greta greta_ndbm
bin_PROGRAMS		= @GRETA@

appdefaultdir		= $(datadir)/taco/app-defaults
appdefault_DATA		= GReta

EXTRA_DIST		= GReta greta.uil
INCLUDES		= @MOTIF_INCLUDES@ @TACO_INCLUDES@ -I$(srcdir)/../include -I$(srcdir)/../../include

greta_SOURCES		= $(SRC_GRETA)
greta_CPPFLAGS		= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@
greta_LDADD		= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ -lX11 -lXt  @X_EXTRA_LIBS@ 

greta_ndbm_SOURCES 	= $(SRC_GRETA)
greta_ndbm_CPPFLAGS  	= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@ -DNDBM_USED 
greta_ndbm_LDADD   	= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ -lX11 -lXt  @X_EXTRA_LIBS@
