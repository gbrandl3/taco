# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2014 Jens Krüger <jkrueger1@users.sf.net>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

EXTRA_DIST		= $(bin_SCRIPTS)

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
			db_hostlist \
			db_devicelist \
			db_devicetree \
			sec_objinfo \
			sec_passwd \
			sec_userinfo \
			db_backup \
			db_clear

bin_SCRIPTS		= db_fillup2

if GDBMSUPPORT
bin_PROGRAMS		+= db_fillup \
		 	db_read db_dump
endif

AM_CPPFLAGS		= @TACO_INCLUDES@ @GDBM_CFLAGS@

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

db_dump_SOURCES		= db_dump.cpp
db_dump_CPPFLAGS	= -I$(top_srcdir)/gdbm @TACO_INCLUDES@ @GDBM_CFLAGS@
db_dump_LDADD		= @TACO_CLIENT_LIBS@ @GDBM_LIBS@ @GDBM_COMPAT_LIBS@

db_hostlist_SOURCES	= db_hostlist.cpp
db_hostlist_LDADD	= @TACO_CLIENT_LIBS@

db_devicelist_SOURCES	= db_devicelist.cpp
db_devicelist_LDADD	= @TACO_CLIENT_LIBS@

sec_objinfo_SOURCES 	= dbm_sec_objinfo.cpp 
sec_objinfo_LDADD 	= @TACO_CLIENT_LIBS@ 

sec_passwd_SOURCES  	= dbm_sec_passwd.cpp
sec_passwd_LDADD	= @TACO_CLIENT_LIBS@ 

sec_userinfo_SOURCES	= dbm_sec_userinfo.cpp
sec_userinfo_LDADD	= @TACO_CLIENT_LIBS@

db_backup_SOURCES	= db_backup.cpp
db_backup_LDADD		= @TACO_CLIENT_LIBS@

db_clear_SOURCES	= db_clear.cpp
db_clear_LDADD		= @TACO_CLIENT_LIBS@
