noinst_HEADERS 		= Serial.h
INCLUDES		= @TACO_INCLUDES@

Serialds_LDADD		= $(top_builddir)/lib/dbase/libdbapi.la $(top_builddir)/lib/xdr/libdsxdr.la $(top_builddir)/lib/libdsapig++.la

bin_PROGRAMS 		= Serialds

Serialds_SOURCES 	= Serial.cpp startup.cpp

CMDS_res_DATA 		= SerialCmnds.res

ERROR_res_DATA 		= SerialErrors.res

template_DATA 		= Serial.res

EXTRA_DIST 		= Serial.res SerialCmnds.res SerialErrors.res
