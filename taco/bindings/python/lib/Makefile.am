pyexec_LTLIBRARIES	= Server.la 

Server_la_LDFLAGS 	= -module 

INCLUDES 		= @TACO_INCLUDES@ @PYTHON_CPPFLAGS@ @PYTHON_CPPFLAGS@

Server_la_LIBADD 	= $(top_builddir)/lib/libtaco.la $(top_builddir)/lib/xdr/libdsxdr_all.la $(top_builddir)/lib/dc/libdcapi.la @PYTHON_LDFLAGS@

AM_CPPFLAGS 		= -DMYDEB -DNUMPY 

noinst_HEADERS		= taco.h python.h pythonP.h

Server_la_SOURCES	= taco.c taco_types.c server.c python.c

install-exec-hook:	
	SHL_EXT="`(. $(DESTDIR)$(pyexecdir)/Server.la ; echo $${library_names} | cut -f3 -d' ' | cut -f2 -d'.')`" ; \
	rm -f "$(DESTDIR)$(pyexecdir)/Taco.$${SHL_EXT}" && $(LN_S) "$(DESTDIR)$(pyexecdir)/Server.$${SHL_EXT}" "$(DESTDIR)$(pyexecdir)/Taco.$${SHL_EXT}"

uninstall-local:
	SHL_EXT="`(. $(DESTDIR)$(pyexecdir)/Server.la ; echo $${library_names} | cut -f3 -d' ' | cut -f2 -d'.')`" ; \
	rm -f "$(DESTDIR)$(pyexecdir)/Taco.$${SHL_EXT}"
