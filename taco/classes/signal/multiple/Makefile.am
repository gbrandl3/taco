SUBDIRS = doc

include_HEADERS = MDSSignal.h
noinst_HEADERS = MDSSignalP.h

INCLUDES=-I$(srcdir)/../single -I$(top_srcdir)/dbase/include @TACO_INCLUDES@

LDADD= @TACO_SERVER_LIBS@ ../single/libdssig.la

bin_PROGRAMS = MDSSignal 

MDSSignal_SOURCES = MDSSignal.c  MDSSignal_config.c

if !BUILTIN_STARTUP
MDSSignal_SOURCES += $(top_srcdir)/lib/startup.c
endif
