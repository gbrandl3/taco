SUBDIRS = doc
include_HEADERS = DSSignalP.h
INCLUDES=-I$(srcdir)/../include -I$(top_srcdir)/dbase/include @TACO_INCLUDES@

bin_PROGRAMS = DSSignal

noinst_LTLIBRARIES = libdssig.la

DSSignal_SOURCES = 

DSSignal_LDADD = libdssig.la @TACO_SERVER_LIBS@

libdssig_la_SOURCES = DSSignal.c

if !BUILTIN_STARTUP
DSSignal_SOURCES += $(top_srcdir)/lib/startup.c
endif

libdssig_la_LIBADD = @TACO_SERVER_LIBS@

