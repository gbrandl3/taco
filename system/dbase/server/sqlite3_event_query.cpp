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
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-06-22 19:04:05 $
 *
 */

#include <Sqlite3Server.h>


/**
 * To retrieve (from database) the event code associated to a event name (the string)
 * 
 * @param pevent_name A pointer to string which is the event name
 * 
 * @return a pointer to a structure of the event_que type 
 */
event_que *SQLite3Server::db_event_query_1_svc(DevString *pevent_name)
{
	int 		i;
	std::string	fam,
			memb,
			r_name,
			event_str(*pevent_name);

	logStream->debugStream() << "Command name : " << event_str << log4cpp::eol;
//
// Initialize error code sended event_queue to client 
//
	event_queue.db_err = 0;
//
// Return error code if the server is not connected to the database files 
//
	if (!dbgen.connected)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
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
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
    	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		event_queue.db_err = DbErr_DatabaseAccess;
		event_queue.xevent_code = 0;
		return(&event_queue);
    	}
	if (nrow != 0)
		event_queue.xevent_code = (atoi(result[ncol]) << DS_TEAM_SHIFT) + (atoi(result[ncol + 1]) << DS_IDENT_SHIFT)
					 + atoi(result[ncol + 2]);
//
// If no command string founded 
//
	else
	{
		event_queue.xevent_code = 0;
		event_queue.db_err = DbErr_ResourceNotDefined;
	}
	sqlite3_free_table(result);
//
// Leave server 
//
	return(&event_queue);
}
