noinst_HEADERS 		= Serial.h
INCLUDES		= @TACO_INCLUDES@

Serialds_LDADD		= $(top_builddir)/lib/dbase/libdbapi.la $(top_builddir)/lib/xdr/libdsxdr.la $(top_builddir)/lib/libdsapig++.la

bin_PROGRAMS 		= Serialds

Serialds_SOURCES 	= Serial.cpp startup.cpp

rescmdsdir 		= ${prefix}/dbase/res/CMDS
rescmds_DATA 		= SerialCmnds.res

errordir 		= ${prefix}/dbase/res/ERROR
error_DATA 		= SerialErrors.res

templatedir 		= ${prefix}/templates
template_DATA 		= Serial.res

EXTRA_DIST 		= Serial.res SerialCmnds.res SerialErrors.res
