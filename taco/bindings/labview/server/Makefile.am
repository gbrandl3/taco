lib_LTLIBRARIES		= liblvdsclass.la liblvdsapi.la libtaco.la 
#noinst_LTLIBRARIES	= libtaco.la

LV_FLAGS = -DLV_DEBUG -DLV_DC -DLABVIEW -DNOMAIN

TACO_LDFLAGS_EXT = -L$(top_srcdir)/lib -L$(top_srcdir)/lib/dbase -L$(top_srcdir)/lib/dc
LIB_TACO_EXT = -ldsapi -ldcapi -ldbapi -ltacoapi
AM_CPPFLAGS = @TACO_INCLUDES@ $(LV_FLAGS)
#INCLUDES	= @TACO_INCLUDES@

include_HEADERS = LabViewGeneric.h LabViewGenericP.h

liblvdsapi_la_SOURCES	= lv_dsapi.c extcode.h
liblvdsapi_la_LDFLAGS = $(LV_FLAGS) -export-dynamic
liblvdsapi_la_LIBADD = libtaco.la

liblvdsclass_la_SOURCES	= lv_dsclass.c LabViewGeneric.h LabViewGenericP.h
liblvdsclass_la_LDFLAGS = $(LV_FLAGS) -export-dynamic
liblvdsclass_la_LIBADD = libtaco.la @LIB_TACO@

libtaco_la_SOURCES = lv_dsapi.c lv_dsclass.c 
libtaco_la_LDFLAGS = $(LV_FLAGS) $(TACO_LDFLAGS_EXT) -export-dynamic @TACO_LDFLAGS@  @QT_LDFLAGS@ 
libtaco_la_LIBADD = $(LIB_TACO_EXT) @LIB_TACO@
#libdsxdr_la_CFLAGS	=
#liblvdsclass_la_LDFLAGS	= -version-info @LIB_VERSION@


rescmdsdir 		= ${prefix}/dbase/res/CMDS
rescmds_DATA 	= LabviewGenericCmds.res

restestdir 		= ${prefix}/dbase/res/TEST
restest_DATA 	= LabviewGeneric.res

EXTRA_DIST 		= LabviewGeneric.res LabviewGenericCmds.res

