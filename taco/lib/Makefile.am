lib_LTLIBRARIES = libtaco.la 

libtaco_la_SOURCES = gen_api.c dev_api.c util_api.c sec_api.c asyn_api.c event_api.c svc_api.c \
		api_xdr.c api_xdr_vers3.c dev_xdr.c DevXdrKernel.c \
		browse_cli.c setacc_cli.c reconnect_cli.c \
		setacc_clnt.c \
		setacc_xdr.c \
	 	gettrans.c oic.c DevServerSig.c \
		dev_errors.c dev_states.c \
		tools_cli.c update_cli.c 

libtaco_la_SOURCES += dc_sdev.c \
                dchist_cli.c \
                dcrd_xdr.c dcwr_xdr.c \
                dcrd_cli.c dcwr_cli.c \
                dcrd_clnt.c dcwr_clnt.c \
                utils_cli.c
 

noinst_HEADERS = API.h API_xdr.h API_xdr_vers3.h Admin.h ApiP.h \
		DevCmds.h DevErrors.h DevSec.h \
		DevServer.h DevServerP.h \
		DevSignal.h DevStates.h DevXdrKernel.h db_setup.h \
		db_xdr.h dev_xdr.h macros.h

noinst_HEADERS += dc.h dcP.h dc_xdr.h dcrd_xdr.h dcwr_xdr.h

EXTRA_DIST = NT_debug.h NT_debug.c

