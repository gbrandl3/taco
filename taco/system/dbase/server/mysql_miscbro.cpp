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
 *		mysql_miscbro.cpp
 * Description:
 *
 * Authors:
 *		$Author: andy_gotz $
 *
 * Version:	$Revision: 1.12 $
 *
 * Date:	$Date: 2010-01-06 17:36:34 $
 *
 */

#include <DevErrors.h>
#include <MySqlServer.h>


/**
 * retrieve device server list for all the devices defined in the database
 *
 * @return The domain name list
 */
db_res *MySQLServer::devserverlist_1_svc()
{
    std::string 		server;
    std::vector<std::string> 	serv_list;
    int			i;
	
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
    if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
    {
	logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the server name list from the NAMES table
//
//
// Sort device server name list
//
    std::string query;
    query = "SELECT DISTINCT SUBSTRING_INDEX(SERVER,'/',1) FROM device ORDER BY SERVER ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
	browse_back.db_err = DbErr_DatabaseAccess;
	return(&browse_back);			
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	serv_list.push_back(row[0]);		

	logStream->debugStream() << "devserverlist_1_svc(): server found " << row[0] << log4cpp::eol;

    }
    mysql_free_result(result);
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
db_res *MySQLServer::devpersnamelist_1_svc(DevString *server)
{
    std::string 		user_server(*server);
    std::vector<std::string>	pers_list;
    int			i;

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
    if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
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

    query = "SELECT DISTINCT SUBSTRING_INDEX(SERVER,'/',-1) FROM device WHERE SERVER LIKE '";
    query += (escape_wildcards(user_server) + "/%' ORDER BY SERVER ASC");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
	browse_back.db_err = DbErr_DatabaseAccess;
	return(&browse_back);			
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	pers_list.push_back(row[0]);
	logStream->debugStream() << "devserverlist_1_svc(): server found " << row[0] << log4cpp::eol;
    }
    mysql_free_result(result);
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
db_res *MySQLServer::hostlist_1_svc()
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
	if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

//
// Get the host name list from the NAMES table for all exported devices (for DS with DB library release 4)
// and sort device server host list
//
	std::string query;
	query = "SELECT DISTINCT HOST FROM device WHERE EXPORTED != 0 ORDER BY HOST ASC";
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
		logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseAccess;
		return(&browse_back);			
	}
	MYSQL_RES *result = mysql_store_result(mysql_conn);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result)) != NULL)
	host_list.push_back(row[0]);
	mysql_free_result(result);
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
//
// Return data
//
	return(&browse_back);
}
