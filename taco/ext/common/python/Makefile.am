EXTRA_DIST 		= TACOStates.i

AM_CPPFLAGS 		= -I$(top_srcdir)/ext/common/include @TACO_INCLUDES@ @SWIG_PYTHON_CPPFLAGS@

AM_CXXFLAGS 		= -Wall

nodist_python_PYTHON	= TACOStates.py

pyexec_LTLIBRARIES 	= _TACOStates.la

nodist__TACOStates_la_SOURCES = wrap_TACOStates.cpp
_TACOStates_la_LDFLAGS 	= -avoid-version -module
_TACOStates_la_LIBADD 	= -lstdc++ $(top_builddir)/ext/lib/libTACOExtensions.la

TACOStates.py: 		wrap_TACOStates.cpp
	@true

wrap_TACOStates.cpp: 	$(srcdir)/TACOStates.i $(top_srcdir)/ext/common/include/TACOStates.h
	@SWIG@ @SWIG_PYTHON_OPT@ -c++ -I$(top_srcdir)/ext/common/include -o $@ $(srcdir)/TACOStates.i

CLEANFILES		= *.pyc *.pyo wrap_*.cpp TACOStates.py

