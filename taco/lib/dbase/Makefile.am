include_HEADERS = db_setup.h  db_xdr.h
noinst_HEADERS	= ndbm_ansi.h

API_SRC   = 	setacc_cli.c \
	 	setacc_clnt.c \
	 	setacc_xdr.c \
		browse_cli.c \
		reconnect_cli.c \
		tools_cli.c \
		update_cli.c
#--------------------------------------------------------------------
# What has to be made
#
lib_LTLIBRARIES = libdbapi.la

INCLUDES= @TACO_INCLUDES@

libdbapi_la_SOURCES=$(API_SRC)
libdbapi_la_LDFLAGS=-version-info $(subst .,:,@LIB_VERSION@)
