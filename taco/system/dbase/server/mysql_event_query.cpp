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
 *		$Author: jlpons $
 *
 * Version:	$Revision: 1.8 $
 *
 * Date:	$Date: 2006-12-12 17:23:08 $
 *
 */

#include <MySqlServer.h>


/**
 * To retrieve (from database) the event code associated to a event name (the string)
 * 
 * @param pevent_name A pointer to string which is the event name
 * 
 * @return a pointer to a structure of the event_que type 
 */
event_que *MySQLServer::db_event_query_1_svc(nam *pevent_name)
{
    int 		i;
    std::string		fam,
			memb,
			r_name,
			event_str(*pevent_name);

#ifdef DEBUG
    std::cout << "Command name : " << event_str << std::endl;
#endif
//
// Initialize error code sended event_queue to client 
//
    event_queue.db_err = 0;
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	std::cout << "I'm not connected to database." << std::endl;
	event_queue.db_err = DbErr_DatabaseNotConnected;
	event_queue.xevent_code = 0;
	return(&event_queue);
    }
//
// Retrieve the right "table" in the table array 
//
    for (i = 0;i < dbgen.TblNum;i++)
	if (dbgen.TblName[i] == "events")
	    break;
    if (i == dbgen.TblNum)
    {
	event_queue.db_err = DbErr_DomainDefinition;
	event_queue.xevent_code = 0;
	return(&event_queue);
    }
//
// Try to retrieve a resource in the EVENTS table with a resource value equal
// to the command name 
//
    std::string query;

    query = "SELECT FAMILY, MEMBER, NAME FROM property_device WHERE ";
    query += ("DOMAIN = 'events' AND VALUE = '" + event_str + "'");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	event_queue.db_err = DbErr_DatabaseAccess;
	event_queue.xevent_code = 0;
	return(&event_queue);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
	event_queue.xevent_code = (atoi(row[0]) << DS_TEAM_SHIFT) + (atoi(row[1]) << DS_IDENT_SHIFT) + atoi(row[2]);
//
// If no command string founded 
//
    else
    {
	event_queue.xevent_code = 0;
	event_queue.db_err = DbErr_ResourceNotDefined;
    }
    mysql_free_result(result);
//
// Leave server 
//
    return(&event_queue);
}
