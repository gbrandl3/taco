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

bin_PROGRAMS = TacoSilvia lv_menu

TACO_LDFLAGS_EXT = -L$(top_builddir)/lib -L$(top_builddir)/lib/dbase 

TacoSilvia_SOURCES = TacoSilvia.cpp LabviewClient.cpp main.cpp
TacoSilvia_LDFLAGS = 
TacoSilvia_LDADD = $(TACO_LDFLAGS_EXT) @QT_LDFLAGS@ @X_LDFLAGS@ $(LIB_TACO_EXT) @TACO_CLIENT_LIBS@  @LIB_QT@

lv_menu_SOURCES = LabviewClient.cpp lv_menu.cpp
lv_menu_LDFLAGS = 
lv_menu_LDADD = $(TACO_LDFLAGS_EXT) $(LIB_TACO_EXT) @TACO_CLIENT_LIBS@

EXTRA_DIST = Doxyfile.in
 
AM_CPPFLAGS = @TACO_INCLUDES@ @QT_INCLUDES@ -I$(srcdir)/../server

METASOURCES = AUTO

%.moc.cpp:	$(srcdir)/%.h
	@MOC@ $< -o $@

moc_%.cpp: $(srcdir)/%.h
	@MOC@ $< -o $@

nodist_TacoSilvia_SOURCES	= moc_TacoSilvia.cpp 

CLEANFILES = moc_* *.moc.cpp

noinst_HEADERS = TacoSilvia.h LabviewClient.h Pixmaps.h

doc:	Doxyfile.in
	doxygen
