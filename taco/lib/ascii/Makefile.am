SUBDIRS = doc

include_HEADERS=asc_api.h
lib_LTLIBRARIES=libascapi.la

libascapi_la_SOURCES=asc_api.c
EXTRA_DIST=README
INCLUDES=-I$(top_srcdir)/dbase/include ${TACO_INCLUDES} -I$(srcdir)/../include -I$(srcdir)/../../tcapi/include

libascapi_la_DFLAGS=-module
