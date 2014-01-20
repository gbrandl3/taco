/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2010-07-07 08:26:02 $
 *
 */

#include <DevErrors.h>
#include <Sqlite3Server.h>
#include <algorithm>


/**
 * retrieve device server list for all the devices defined in the database
 *
 * @return The domain name list
 */
db_res *SQLite3Server::devserverlist_1_svc()
{
	std::string 		server;
	std::vector<std::string> serv_list;
	
	logStream->debugStream() << "In devserverlist_1_svc function" << log4cpp::eol;

//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
//
// Get the server name list from the NAMES table
// Sort device server name list
//
	std::string query;
	query = "SELECT DISTINCT SERVER FROM device ORDER BY SERVER ASC";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseAccess;
		return(&browse_back);			
	}
	int j = ncol;
	serv_list.clear();
	for (int i = 0; i < nrow; ++i, j += ncol) 
	{
		std::string tmp = result[j];
		std::string::size_type pos = tmp.find('/');
		tmp.erase(pos);
		if (std::find(serv_list.begin(), serv_list.end(), tmp) == serv_list.end())
		{
			serv_list.push_back(tmp);		

			logStream->debugStream() << "devserverlist_1_svc(): server found " << tmp << log4cpp::eol;
		}
	}
	sqlite3_free_table(result);
//
// Build the structure returned to caller
//
	int length = browse_back.res_val.arr1_len = serv_list.size();
	browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
	for (int i = 0;i < length;i++)
	{
        	int     k = serv_list[i].length();
        	browse_back.res_val.arr1_val[i] = new char [k + 1];
        	serv_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        	(browse_back.res_val.arr1_val[i])[k] = '\0';
	}                                                                                                                                        
#if 0
    if (serv_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	logStream->errorStream() << "Memory allocation error in devserverlist" << log4cpp::eol;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
	return(&browse_back);
}


/*
 * list the families for all the devices defined in the database for a given domain
 * 
 * @param domain The domain name
 * 
 * @param The family name list
 */
db_res *SQLite3Server::devpersnamelist_1_svc(DevString *server)
{
	std::string 		user_server(*server);
	std::vector<std::string> pers_list;

	logStream->debugStream() << "In devpersnamelist_1_svc function for server " << user_server << log4cpp::eol;

//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
//
// Get the family name list for the wanted domain in the NAMES table
//
//
// Sort device server personal name list
//
	std::string query;
	query = "SELECT DISTINCT SERVER FROM device WHERE SERVER LIKE '";
	query += (escape_wildcards(user_server) + "/%' ESCAPE '\\' ORDER BY SERVER ASC");
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseAccess;
		return(&browse_back);			
    	}
	int j = ncol;
	pers_list.clear();
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
		std::string tmp = result[j];
		std::string::size_type pos = tmp.find('/');
		tmp.erase(0, pos + 1);
		if (std::find(pers_list.begin(), pers_list.end(), tmp) == pers_list.end())
		{
			pers_list.push_back(tmp);		

			logStream->debugStream() << "SQLite3Server::devpersnamelist_1_svc(): server found " << tmp << log4cpp::eol;
		}
	}
	sqlite3_free_table(result);
//
// Build the sequence returned to caller
//
    int length = browse_back.res_val.arr1_len = pers_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = pers_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        pers_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                                                        
#if 0
    if (pers_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
		logStream->errorStream() << "Memory allocation error in devpersnamelist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);	
}


/**
 * list all hosts where device servers should run 
 *
 * @return The host name list
 */
db_res *SQLite3Server::hostlist_1_svc()
{
	std::vector<std::string>	host_list;
	
	logStream->debugStream() << "In hostlist_1_svc function" << log4cpp::eol;

//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

//
// Get the host name list from the NAMES table for all exported devices (for DS with DB library release 4)
// and sort device server host list
//
	std::string query;
	query = "SELECT DISTINCT HOST FROM device WHERE EXPORTED != 0 ORDER BY HOST ASC";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseAccess;
		return(&browse_back);			
	}
	int j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol) 
		host_list.push_back(result[j]);
	sqlite3_free_table(result);
//
// Build the structure returned to caller
//
	int length = browse_back.res_val.arr1_len = host_list.size();
	browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
	for (int i = 0;i < length;i++)
	{
		int     k = host_list[i].length();
        	browse_back.res_val.arr1_val[i] = new char [k + 1];
        	host_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        	(browse_back.res_val.arr1_val[i])[k] = '\0';
	}                                                                                                                                        
#if 0
	if (host_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in hostlist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
#endif
//
// Return data
//
	return(&browse_back);
}
