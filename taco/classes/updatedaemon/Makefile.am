noinst_HEADERS 		= daemon.h daemon_xdr.h ud_server.h ud_serverP.h
INCLUDES		= @TACO_INCLUDES@ -I$(top_srcdir)/lib/dataport/

LDADD			= @TACO_SERVER_LIBS@ ../../lib/dataport/libdataport.la \
			  ../../lib/dc/libdcapi.la

bin_PROGRAMS 		= ud_daemon ud_poller ud_daemon_menu

ud_daemon_SOURCES  	= daemon.c startup.c daemon_xdr.c MakeDP.c

ud_poller_SOURCES  	= poller.c MakeDP.c datac.c

ud_daemon_menu_SOURCES = daemon_menu.c daemon_xdr.c

