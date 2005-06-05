noinst_HEADERS 		= Serial.h
INCLUDES		= @TACO_INCLUDES@

LIBADD			= @TACO_SERVER_CXX_LIBS@ -lstdc++

if SOLARIS_CXX
Serialds_LDADD		= @TACO_DEVSERVER_MAIN@ @TACO_SERVER_NOMAIN_CXX_STATIC_LIBS@
else
Serialds_LDADD		= @TACO_SERVER_CXX_LIBS@
endif

bin_PROGRAMS 		= Serialds

Serialds_SOURCES 	= Serial.cpp startup.cpp

CMDS_res_DATA 		= SerialCmnds.res

ERROR_res_DATA 		= SerialErrors.res

template_DATA 		= Serial.res

EXTRA_DIST 		= Serial.res SerialCmnds.res SerialErrors.res
