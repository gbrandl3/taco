noinst_HEADERS		= NdbmClass.h NdbmServer.h MySqlServer.h dbClass.h svc.h

sbin_PROGRAMS		= dbm_server

lib_LTLIBRARIES 	= libTACOndbm.la 

dbm_server_SOURCES 	= main_svc.cpp dbm.cpp dbm_xdr.cpp NdbmClass.cpp db_serv.cpp

libTACOndbm_la_SOURCES = dbm_serv.cpp \
			dbm_devbro.cpp \
			dbm_resbro.cpp \
			dbm_impexp.cpp \
			dbm_getdev.cpp \
			dbm_tools.cpp \
			dbm_cmd_query.cpp \
			dbm_event_query.cpp \
			dbm_psdev.cpp \
			dbm_clo_op.cpp \
			dbm_miscbro.cpp \
			dbm_update.cpp 
libTACOndbm_la_LIBADD	= @TACO_GDBM_LDFLAGS@ @TACO_GDBM_LIBS@ 

if MYSQLSUPPORT
lib_LTLIBRARIES		+= libTACOMySQL.la
libTACOMySQL_la_SOURCES = mysql_serv.cpp \
			mysql_devbro.cpp \
			mysql_resbro.cpp \
			mysql_impexp.cpp \
			mysql_getdev.cpp \
			mysql_tools.cpp \
			mysql_cmd_query.cpp \
			mysql_event_query.cpp \
			mysql_psdev.cpp \
			mysql_clo_op.cpp \
			mysql_miscbro.cpp \
			mysql_update.cpp 
libTACOMySQL_la_LIBADD	= @MYSQL_LDFLAGS@ -lmysqlclient 
endif

dbm_server_LDADD	= @TACO_SERVER_LIBS@ @TACO_GDBM_LDFLAGS@ @TACO_GDBM_LIBS@ $(lib_LTLIBRARIES)
INCLUDES		= -DNDBM @TACO_GDBM_INC@ @TACO_INCLUDES@ 
