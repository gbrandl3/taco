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
 * Version:	$Revision: 1.1 $
 *
 * Date:	$Date: 2006-09-27 12:21:35 $
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
    errcode = 0;
#ifdef DEBUG
    std::cout << "db_clodb()" << std::endl;
#endif 
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	std::cout << "I'm not connected to database." << std::endl;
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
#ifdef DEBUG
	std::cout << "db_reopendb" << std::endl;
#endif
	errcode = DS_NOTOK;
//
// Open database tables according to the definition
//
	if(sqlite3_open(m_table.c_str(), &db))
	{
		std::cout  << "Can't open database: " <<  sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		sqlite3_close(db);
		dbgen.connected = false;
	}
	else
	{
		std::string query = "SELECT COUNT(*) FROM device";
		if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			query = "CREATE TABLE attribute_class (class varchar(255) NOT NULL default '', ";
			query += "name varchar(255) NOT NULL default '', updated timestamp(14) NOT NULL, ";
			query += "accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;
		
			query = "INSERT INTO attribute_class VALUES('DevClass', 'DEVCLASS', '2006-06-12 15:00:00',";
			query += " '2006-06-12 15:00:00', 'Test device class')";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;
		
			query = "CREATE TABLE device(name varchar(255) default NULL, alias varchar(255) default NULL,";
			query += " domain varchar(85) default NULL, family varchar(85) default NULL,";
			query += " member varchar(85) default NULL, exported int(11) default NULL, ior text,";
			query += " host varchar(255) default NULL, server varchar(255) default NULL,";
			query += " pid int(11) default NULL, class varchar(255) default NULL,";
			query += " version varchar(8) default NULL, started datetime default NULL,";
			query += " stopped datetime default NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;
#if 0
			query = "INSERT INTO device VALUES('dserver/DataBaseds/2', NULL, 'dserver', 'DataBaseds',";
			query += " '2', 0, 'nada', 'wow', 'DataBaseds/2', 0, 'DServer', '2', '2003-07-11 09:43:17',";
			query += " NULL, NULL)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "INSERT INTO device VALUES('sys/database/2', NULL, 'sys', 'database', '2', 0, 'nada', ";
			query += " 'wow', 'DataBaseds/2', 0, 'DataBase', '2', '2003-07-11 09:43:17', NULL, NULL)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;
#endif
			query = "CREATE TABLE property(object varchar(255) default NULL, name varchar(255) default NULL,";
			query += " count int(11) default NULL, value varchar(255) default NULL,";
			query += " updated timestamp(14) NOT NULL, accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_attribute_class(class varchar(255) NOT NULL default '',";
			query += " attribute varchar(255) NOT NULL default '', name varchar(255) NOT NULL default '',";
			query += " count int(11) NOT NULL default '0', value varchar(255) default NULL,";
			query += " updated timestamp(14) NOT NULL, accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_class(class varchar(255) NOT NULL default '',";
			query += " name varchar(255) NOT NULL default '', count int(11) NOT NULL default '0',";
			query += " value varchar(255) default NULL, updated timestamp(14) NOT NULL,";
			query += " accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE property_device (";
			query += " device varchar(255) NOT NULL default '', name varchar(255) NOT NULL default '',";
			query += " domain varchar(255) NOT NULL default '', family varchar(255) NOT NULL default '',";
			query += " member varchar(255) NOT NULL default '', count int(11) NOT NULL default '0',";
			query += " value varchar(255) default NULL, updated timestamp(14) NOT NULL,";
			query += " accessed timestamp(14) NOT NULL, comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE server(name varchar(255) NOT NULL default '',";
			query += " host varchar(255) NOT NULL default '', mode int(11) default '0',";
			query += " level int(11) default '0')";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE event(name varchar(255), exported int(11), ior text, host varchar(255),";
			query += " server varchar(255), pid int(11), version varchar(8), started datetime,";
			query += " stopped datetime)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE TABLE attribute_alias(alias varchar(255) DEFAULT '' NOT NULL,";
			query += " name varchar(255) DEFAULT '' NOT NULL, device varchar(255) DEFAULT '' NOT NULL,";
			query += " attribute varchar(255) DEFAULT '' NOT NULL, updated timestamp(14),";
			query += " accessed timestamp(14), comment text)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_attribute_class on attribute_class(class, name)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_property_attribute_class on ";
			query += " property_attribute_class(attribute,name,count)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_resource on property_device(device, name, count)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_property on property_class(class, name, count)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

			query = "CREATE INDEX index_attribute_alias on attribute_alias(alias, name)";
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
				return &errcode;

		}
		dbgen.connected = true;
	}	
//
// Leave server 
//
	return(&errcode);
}
