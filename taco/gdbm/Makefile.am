# GDBM 1.8.3 builds shared libraries version 3.0
SHLIB_VER = 3 0 0

lib_LTLIBRARIES = libgdbm.la 
check_PROGRAMS 		= testgdbm testdbm testndbm 

if COMPAT

bin_PROGRAMS 		= conv2gdbm
lib_LTLIBRARIES 	+= libgdbm_compat.la
include_HEADERS 	= dbm.h ndbm.h 
libgdbm_compat_la_SOURCES = dbminit.c delete.c fetch.c store.c seq.c close.c \
		dbmopen.c dbmdelete.c dbmfetch.c dbmstore.c dbmseq.c \
		dbmclose.c dbmdirfno.c dbmpagfno.c dbmrdonly.c
libgdbm_compat_la_LDFLAGS = -version-info $(SHLIB_VER) 
check_PROGRAMS 		+= testdbm testndbm 

testndbm_SOURCES 	= testndbm.c 
testndbm_LDADD		= libgdbm.la libgdbm_compat.la
testndbm_CPPFLAGS	= -DGNU 

testdbm_SOURCES 	= testdbm.c 
testdbm_LDADD		= libgdbm.la libgdbm_compat.la

conv2gdbm_SOURCES	= conv2gdbm.c
conv2gdbm_LDADD		= libgdbm.la libgdbm_compat.la

endif

libgdbm_la_SOURCES  	= gdbmopen.c gdbmdelete.c gdbmfetch.c  gdbmstore.c gdbmclose.c \
		gdbmreorg.c gdbmseq.c gdbmsync.c gdbmerrno.c gdbmexists.c gdbmfdesc.c \
		gdbmsetopt.c bucket.c falloc.c findkey.c global.c hash.c update.c \
		version.c

libgdbm_la_LDFLAGS	= -version-info $(SHLIB_VER) 

testgdbm_SOURCES 	= testgdbm.c
testgdbm_LDADD		= libgdbm.la

man3_MANS		= gdbm.3

info_TEXINFOS		= gdbm.texinfo

noinst_HEADERS 		= gdbmdefs.h extern.h systems.h gdbmconst.h proto.h gdbmerrno.h 

BUILT_SOURCES		= gdbm.h
CLEANFILES		= gdbm.h

DISTCLEANFILES		= autoconf.h conv2gdbm junk.gdbm junkdbm.dir junkdbm.pag junkndbm.dir junkndbm.pag \
			testdbm testgdbm testndbm

nodist_include_HEADERS	= gdbm.h 

EXTRA_DIST 		= gdbm.3 gdbm.texinfo \
			gdbm.info gdbm.proto gdbm.proto2 \
			getopt.c getopt.h 

gdbm.h:	gdbm.proto gdbmerrno.h gdbm.proto2
	rm -f gdbm.h
	cp $(srcdir)/gdbm.proto gdbm.h
	chmod +w gdbm.h
	grep _ $(srcdir)/gdbmerrno.h >> gdbm.h
	cat $(srcdir)/gdbm.proto2 >> gdbm.h
	chmod -w gdbm.h

