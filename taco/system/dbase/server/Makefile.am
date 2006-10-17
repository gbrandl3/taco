# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2005 Jens Krüger <jkrueger1@users.sf.net>
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

noinst_HEADERS		= dbClass.h svc.h

sbin_PROGRAMS		= dbm_server

lib_LTLIBRARIES 	= 

INCLUDES		= @TACO_INCLUDES@ 
AM_CPPFLAGS		= -DDEBUG

if GDBMSUPPORT
noinst_HEADERS		+= NdbmClass.h NdbmServer.h 
lib_LTLIBRARIES 	+= libTACOndbm.la 

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
			dbm_update.cpp \
			NdbmClass.cpp
libTACOndbm_la_LIBADD	= @GDBM_LIBS@ -lstdc++
INCLUDES		+= @GDBM_CFLAGS@ 
AM_CPPFLAGS		+= -DNDBM -DUSE_GDBM -DDEBUG
endif

if MYSQLSUPPORT
noinst_HEADERS		+= MySqlServer.h 
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
libTACOMySQL_la_LIBADD	= @MYSQL_LIBS@ -lstdc++
INCLUDES		+= @MYSQL_CFLAGS@ 
AM_CPPFLAGS		+= -DUSE_MYSQL -DDEBUG
endif

if SQLITE3SUPPORT
noinst_HEADERS		+= Sqlite3Server.h
lib_LTLIBRARIES		+= libTACOSQLite3.la
libTACOSQLite3_la_SOURCES = sqlite3_serv.cpp \
			sqlite3_devbro.cpp \
			sqlite3_resbro.cpp \
			sqlite3_impexp.cpp \
			sqlite3_getdev.cpp \
			sqlite3_tools.cpp \
			sqlite3_cmd_query.cpp \
			sqlite3_event_query.cpp \
			sqlite3_psdev.cpp \
			sqlite3_clo_op.cpp \
			sqlite3_miscbro.cpp \
			sqlite3_update.cpp 
libTACOSQLite3_la_LIBADD= @SQLITE3_LIBS@ -lstdc++
INCLUDES		+= @SQLITE3_CFLAGS@ 
AM_CPPFLAGS		+= -DUSE_SQLITE3 -DDEBUG
endif

dbm_server_CFLAGS	= -DDEBUG
dbm_server_SOURCES 	= main_svc.cpp dbm.cpp db_serv.cpp
dbm_server_LDADD	= @TACO_SERVER_LIBS@ $(lib_LTLIBRARIES) -lstdc++
dbm_server_LDFLAGS	= -static
