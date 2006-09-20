ACLOCAL_AMFLAGS = -I m4

if BUILD_GDBM
gdbm=gdbm
else
gdbm=
endif

if BUILD_SERVER
classes=classes
else
classes=
endif

if BUILD_EXT
ext=ext
else
ext=
endif

SUBDIRS = m4 $(gdbm) lib $(ext) system $(classes) doc misc bindings tests 

EXTRA_DIST=autogen.sh taco.spec.in README-RPM

include $(top_srcdir)/rpm_rules.mh

DISTCLEANFILES = pathes.h
