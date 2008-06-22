/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:
 *
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-06-22 19:04:05 $
 *
 */

#include <DevErrors.h>
#include <Sqlite3Server.h>
#include <algorithm>


/**
 * close the database to be able to reload a new database.
 *
 * @return a pointer to an integer. This integer is simply an error code (0 if no error).
 */
DevLong *SQLite3Server::db_clodb_1_svc(void)
{
    errcode = DS_OK;

    logStream->debugStream() << "db_clodb()" << log4cpp::eol;

//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Disconnect server from database files 
//
    sqlite3_close(db);
    dbgen.connected = false;
//
// Leave server 
//
    return(&errcode);
}

/**
 * Reopen the database after it has been updated by a dbm_update command in a 
 * single user mode or after the rebuilding from a backup file.
 * 
 * @returns a pointer to an integer. This integer is simply an error code (0 if no error).
 */
DevLong *SQLite3Server::db_reopendb_1_svc(void)
{
	logStream->debugStream() << "db_reopendb" << log4cpp::eol;

	errcode = DS_NOTOK;
//
// Open database tables according to the definition
//
	if(sqlite3_open(m_table.c_str(), &db))
	{
		logStream->errorStream() << "Can't open database: " <<  sqlite3_errmsg(db) << log4cpp::eol;
		errcode = DbErr_DatabaseAccess;
		sqlite3_close(db);
		dbgen.connected = false;
	}
	else
	{
		std::string query = "SELECT COUNT(*) FROM device";
		logStream->infoStream() << query << log4cpp::eol;
		if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			query = "CREATE TABLE attribute_class (class varchar(255) NOT NULL default '', ";
			query += "name varchar(255) NOT NULL default '', updated timestamp(14) NOT NULL, ";
			query += "accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;
		
			query = "INSERT INTO attribute_class VALUES('DevClass', 'DEVCLASS', '2006-06-12 15:00:00',";
			query += " '2006-06-12 15:00:00', 'Test device class')";
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;
		
			query = "CREATE TABLE device(name varchar(255) default NULL, alias varchar(255) default NULL,";
			query += " domain varchar(85) default NULL, family varchar(85) default NULL,";
			query += " member varchar(85) default NULL, exported int(11) default NULL, ior text,";
			query += " host varchar(255) default NULL, server varchar(255) default NULL,";
			query += " pid int(11) default NULL, class varchar(255) default NULL,";
			query += " version varchar(8) default NULL, started datetime default NULL,";
			query += " stopped datetime default NULL, comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;
#if 0
			query = "INSERT INTO device VALUES('dserver/DataBaseds/2', NULL, 'dserver', 'DataBaseds',";
			query += " '2', 0, 'nada', 'wow', 'DataBaseds/2', 0, 'DServer', '2', '2003-07-11 09:43:17',";
			query += " NULL, NULL)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "INSERT INTO device VALUES('sys/database/2', NULL, 'sys', 'database', '2', 0, 'nada', ";
			query += " 'wow', 'DataBaseds/2', 0, 'DataBase', '2', '2003-07-11 09:43:17', NULL, NULL)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;
#endif
			query = "CREATE TABLE property(object varchar(255) default NULL, name varchar(255) default NULL,";
			query += " count int(11) default NULL, value varchar(255) default NULL,";
			query += " updated timestamp(14) NOT NULL, accessed timestamp(14) NOT NULL, comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_attribute_class(class varchar(255) NOT NULL default '',";
			query += " attribute varchar(255) NOT NULL default '', name varchar(255) NOT NULL default '',";
			query += " count int(11) NOT NULL default '0', value varchar(255) default NULL,";
			query += " updated timestamp(14) NOT NULL, accessed timestamp(14) NOT NULL, comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_class(class varchar(255) NOT NULL default '',";
			query += " name varchar(255) NOT NULL default '', count int(11) NOT NULL default '0',";
			query += " value varchar(255) default NULL, updated timestamp(14) NOT NULL,";
			query += " accessed timestamp(14) NOT NULL, comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_device (";
			query += " device varchar(255) NOT NULL default '', name varchar(255) NOT NULL default '',";
			query += " domain varchar(255) NOT NULL default '', family varchar(255) NOT NULL default '',";
			query += " member varchar(255) NOT NULL default '', count int(11) NOT NULL default '0',";
			query += " value varchar(255) default NULL, updated timestamp(14) NOT NULL,";
			query += " accessed timestamp(14) NOT NULL, comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE server(name varchar(255) NOT NULL default '',";
			query += " host varchar(255) NOT NULL default '', mode int(11) default '0',";
			query += " level int(11) default '0')";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE event(name varchar(255), exported int(11), ior text, host varchar(255),";
			query += " server varchar(255), pid int(11), version varchar(8), started datetime,";
			query += " stopped datetime)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE attribute_alias(alias varchar(255) DEFAULT '' NOT NULL,";
			query += " name varchar(255) DEFAULT '' NOT NULL, device varchar(255) DEFAULT '' NOT NULL,";
			query += " attribute varchar(255) DEFAULT '' NOT NULL, updated timestamp(14),";
			query += " accessed timestamp(14), comment text)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_attribute_class on attribute_class(class, name)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_property_attribute_class on ";
			query += " property_attribute_class(attribute,name,count)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_resource on property_device(device, name, count)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_domain on property_device(domain)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_family on property_device(family)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_member on property_device(member)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_property on property_class(class, name, count)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_attribute_alias on attribute_alias(alias, name)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_name on device(name, alias)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_server on device(server)";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
				return &errcode;

		}
		dbgen.connected = true;
	}	
//
// Leave server 
//
	return(&errcode);
}
