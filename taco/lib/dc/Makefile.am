include_HEADERS		= dc.h  dcP.h  dc_xdr.h  dcrd_xdr.h  dcwr_xdr.h
lib_LTLIBRARIES		= libdcapi.la

libdcapi_la_SOURCES	= dc_sdev.c dchist_cli.c dcrd_xdr.c dcrd_cli.c \
			dcrd_clnt.c dcwr_xdr.c dcwr_cli.c dcwr_clnt.c utils_cli.c

libdcapi_la_LDFLAGS	= -version-info $(subst .,:,@LIB_VERSION@)

INCLUDES		= @TACO_INCLUDES@
