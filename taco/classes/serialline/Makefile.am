noinst_HEADERS 		= Serial.h
INCLUDES		= @TACO_INCLUDES@

Serialds_LDADD		= @TACO_SERVER_CXX_LIBS@

bin_PROGRAMS 		= Serialds

Serialds_SOURCES 	= Serial.cpp startup.cpp

CMDS_res_DATA 		= SerialCmnds.res

ERROR_res_DATA 		= SerialErrors.res

template_DATA 		= Serial.res

EXTRA_DIST 		= Serial.res SerialCmnds.res SerialErrors.res
