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

EXTRA_DIST	= README 

## TACO.pdf
## dist_doc_DATA = index.html 

# doc: language FORCE
doc:	FORCE index.doc Doxyfile taco_manual.pdf started.doc
	@DOXYGEN@
	@rm -f latex/refman.tex
	@cp @srcdir@/taco_logo.gif html
	@sed -e "s/\$$VERSION/$(VERSION)/g" taco_manual.tex >latex/taco_manual.tex
	@sed -e "s/\$$VERSION/$(VERSION)/g" taco.sty >latex/taco.sty 
	@epstopdf @srcdir@/taco_logo.eps --outfile=latex/taco_logo.pdf

latex/index.tex:
	@DOXYGEN@

taco_manual.pdf: taco_manual.tex taco.sty latex/index.tex taco_logo.pdf tacoarch.pdf \
		tacodsm.pdf dsm_model.pdf cmd_exe.pdf class_hierarchy.pdf \
		devwindow.pdf filewindow.pdf servwindow.pdf reswindow.pdf newserver.pdf
	@cp latex/procedure.pdf .
	@cp latex/agps_states.pdf .
	@echo "Running latex..."
	@pdflatex taco_manual.tex
	@echo "Running makeindex..."
	@makeindex taco_manual.idx
	@echo "Rerunning latex...."
	@pdflatex taco_manual.tex

taco_logo.pdf:	$(srcdir)/taco_logo.eps
	epstopdf $(srcdir)/taco_logo.eps --outfile=taco_logo.pdf

tacoarch.pdf:	$(srcdir)/tacoarch.eps
	epstopdf $(srcdir)/tacoarch.eps --outfile=tacoarch.pdf

tacodsm.pdf:	$(srcdir)/tacodsm.eps
	epstopdf $(srcdir)/tacodsm.eps --outfile=tacodsm.pdf

dsm_model.pdf:	$(srcdir)/dsm_model.eps
	epstopdf $(srcdir)/dsm_model.eps --outfile=dsm_model.pdf

cmd_exe.pdf:	$(srcdir)/cmd_exe.eps
	epstopdf $(srcdir)/cmd_exe.eps --outfile=cmd_exe.pdf

class_hierarchy.pdf:	$(srcdir)/class_hierarchy.eps
	epstopdf $(srcdir)/class_hierarchy.eps --outfile=class_hierarchy.pdf

procedure.pdf:	$(srcdir)/procedure.eps
	epstopdf $(srcdir)/procedure.eps --outfile=procedure.pdf

devwindow.pdf:	$(srcdir)/devwindow.eps
	epstopdf $(srcdir)/devwindow.eps --outfile=devwindow.pdf

filewindow.pdf:	$(srcdir)/filewindow.eps
	epstopdf $(srcdir)/filewindow.eps --outfile=filewindow.pdf

servwindow.pdf:	$(srcdir)/servwindow.eps
	epstopdf $(srcdir)/servwindow.eps --outfile=servwindow.pdf

reswindow.pdf:	$(srcdir)/reswindow.eps
	epstopdf $(srcdir)/reswindow.eps --outfile=reswindow.pdf

newserver.pdf:	$(srcdir)/newserver.eps
	epstopdf $(srcdir)/newserver.eps --outfile=newserver.pdf

# clean: 
# 	rm -rf html latex *.bak 
# 
# language: language.doc 
# 
# language.doc: $(wildcard ../src/translator*.h) maintainers.txt language.tpl translator.py
# 	python translator.py
# 
FORCE:
