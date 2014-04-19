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
 *		mysql_cmd_query.cpp
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2008-04-06 09:07:41 $
 *
 */

#include <MySqlServer.h>


/**
 * retrieve (from database) the command code associated to a command name (the string)
 * 
 * @param pcmd_name A pointer to string which is the command name
 * 
 * @return a pointer to a structure of the cmd_que type
 */
cmd_que *MySQLServer::db_cmd_query_1_svc(DevString *pcmd_name)
{
    int 		i;
    std::string 	req_cmd(*pcmd_name);


    logStream->debugStream() << "Command name : " << req_cmd << log4cpp::eol;

//
// Initialize error code sended cmd_queue to client 
//
    cmd_queue.db_err = 0;
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
    {
	logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
	cmd_queue.db_err = DbErr_DatabaseNotConnected;
	cmd_queue.xcmd_code = 0;
	return(&cmd_queue);
    }
//
// Retrieve the right "table" in the table array 
//
    for (i = 0;i < dbgen.TblNum;i++)
	if (dbgen.TblName[i] == "cmds")		// DOMAIN = 'cmds'
	    break;
    if (i == dbgen.TblNum)
    {
	cmd_queue.db_err = DbErr_DomainDefinition;
	cmd_queue.xcmd_code = 0;
	return(&cmd_queue);
    }
//
// Try to retrieve a resource in the CMDS table with a resource value equal
// to the command name 
//
    std::string query = "SELECT FAMILY, MEMBER, NAME FROM property_device WHERE ";
    query += ("DOMAIN = 'cmds' AND VALUE = '" + req_cmd + "'");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
	cmd_queue.db_err = DbErr_DatabaseAccess;
	cmd_queue.xcmd_code = 0;
	return (&cmd_queue);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
	cmd_queue.xcmd_code = (atoi(row[0]) << DS_TEAM_SHIFT) + (atoi(row[1]) << DS_IDENT_SHIFT) + atoi(row[2]);
//
// If no command string founded 
//
    else
    {
	cmd_queue.xcmd_code = 0;
	cmd_queue.db_err = DbErr_ResourceNotDefined;
    }
    mysql_free_result(result);
//
// Leave server 
//
    return(&cmd_queue);
}
