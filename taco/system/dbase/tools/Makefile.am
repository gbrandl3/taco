bin_PROGRAMS		= db_update \
		 	db_info \
		 	db_devinfo \
	 		db_devdel \
		 	db_servdel \
		 	db_servinfo \
	 		db_servunreg \
		 	db_devres \
		 	db_resdel \
		 	db_initcache \
			db_devicetree

if GDBMSUPPORT
bin_PROGRAMS		+= db_fillup \
		 	db_read 
endif

INCLUDES		= @TACO_INCLUDES@ @GDBM_CFLAGS@

db_devicetree_SOURCES  = db_devicetree.cpp
db_devicetree_LDADD    = @TACO_CLIENT_LIBS@

db_fillup_SOURCES	= db_fillup.cpp
db_fillup_LDADD		= @GDBM_LIBS@	

db_update_SOURCES 	= db_update.cpp 
db_update_LDADD		= @TACO_CLIENT_LIBS@

db_read_SOURCES		= db_read.cpp
db_read_LDADD		= @TACO_CLIENT_LIBS@ @GDBM_LIBS@

db_info_SOURCES		= db_info.cpp 
db_info_LDADD		= @TACO_CLIENT_LIBS@

db_initcache_SOURCES	= db_initcache.cpp 
db_initcache_LDADD	= @TACO_CLIENT_LIBS@

db_devinfo_SOURCES	= db_devinfo.cpp
db_devinfo_LDADD	= @TACO_CLIENT_LIBS@

db_devdel_SOURCES	= db_devdel.cpp 
db_devdel_LDADD		= @TACO_CLIENT_LIBS@

db_servdel_SOURCES	= db_servdel.cpp
db_servdel_LDADD	= @TACO_CLIENT_LIBS@

db_servinfo_SOURCES	= db_servinfo.cpp 
db_servinfo_LDADD	= @TACO_CLIENT_LIBS@

db_servunreg_SOURCES	= db_servunreg.cpp 
db_servunreg_LDADD	= @TACO_CLIENT_LIBS@

db_devres_SOURCES	= db_devres.cpp 
db_devres_LDADD		= @TACO_CLIENT_LIBS@

db_resdel_SOURCES	= db_resdel.cpp 
db_resdel_LDADD		= @TACO_CLIENT_LIBS@
