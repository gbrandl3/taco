EXTRA_DIST=NdbmClass.h 
sbin_PROGRAMS= dbm_server

dbm_server_SOURCES =NdbmClass.cpp \
		dbm_devbro.cpp \
		dbm_resbro.cpp \
		main_svc.cpp \
		dbm_miscbro.cpp \
		dbm_clo_op.cpp \
	 	dbm_getdev.cpp \
	 	dbm_impexp.cpp \
		dbm_serv.cpp \
		dbm_tools.cpp \
	 	cmd_query.cpp \
	 	dbm_xdr.cpp \
		dbm_psdev.cpp \
		dbm_update.cpp \
		dbm_event_query.cpp

INCLUDES= -DNDBM @TACO_GDBM_INC@ @TACO_INCLUDES@ 
dbm_server_LDADD= @TACO_SERVER_LIBS@ @TACO_GDBM@
