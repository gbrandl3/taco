ACLOCAL_AMFLAGS = -I m4

if BUILD_GDBM
gdbm=gdbm
else
gdbm=
endif

SUBDIRS = m4 $(gdbm) lib system classes doc misc bindings tests 

EXTRA_DIST=Makefile.cvs taco.spec config/depcomp

include $(top_srcdir)/rpm_rules.mh


