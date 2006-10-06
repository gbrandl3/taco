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

bin_PROGRAMS = TacoSilvia lv_menu

TACO_LDFLAGS_EXT = -L$(top_srcdir)/lib -L$(top_srcdir)/lib/dbase 
LIB_TACO_EXT = -ldsapi -ldbapi -ltacoapi

TacoSilvia_SOURCES = TacoSilvia.cpp LabviewClient.cpp main.cpp
TacoSilvia_LDFLAGS = $(TACO_LDFLAGS_EXT) @TACO_LDFLAGS@  @QT_LDFLAGS@ @X_LDFLAGS@ 
TacoSilvia_LDADD = $(LIB_TACO_EXT) @LIB_TACO@  @LIB_QT@

lv_menu_SOURCES = LabviewClient.cpp lv_menu.cpp
lv_menu_LDFLAGS = $(TACO_LDFLAGS_EXT) @TACO_LDFLAGS@ 
lv_menu_LDADD = $(LIB_TACO_EXT) @LIB_TACO@  

#TacoSilvia_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS)
#TacoSilvia_LDADD = $(LIB_QT) $(LIB_X11)

EXTRA_DIST = am_edit Doxyfile.in
 
AM_CPPFLAGS = @TACO_INCLUDES@ @QT_INCLUDES@ -I$(srcdir)/../server
#INCLUDES = @TACO_INCLUDES@ @QT_INCLUDES@ -I$(srcdir)/../server 

METASOURCES = AUTO

%.moc.cpp:	$(srcdir)/%.h
	@MOC@ $< -o $@

moc_%.cpp: $(srcdir)/%.h
	@MOC@ $< -o $@

nodist_TacoSilvia_SOURCES	= moc_TacoSilvia.cpp 

CLEANFILES = moc_* *.moc.cpp

noinst_HEADERS = TacoSilvia.h LabviewClient.h 

doc:	Doxyfile.in
	doxygen
