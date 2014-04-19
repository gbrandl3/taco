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
 *		mysql_clo_op.cpp
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2008-04-06 09:07:41 $
 *
 */

#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>


/**
 * close the database to be able to reload a new database.
 *
 * @return a pointer to an integer. This integer is simply an error code (0 if no error).
 */
DevLong *MySQLServer::db_clodb_1_svc(void)
{
    errcode = 0;

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
    mysql_close(mysql_conn);
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
DevLong *MySQLServer::db_reopendb_1_svc(void)
{
    errcode = 0;

    logStream->debugStream() << "db_reopendb" << log4cpp::eol;
//
// Open database tables according to the definition
//
    if ((mysql_conn = mysql_real_connect(&mysql, "localhost", mysql_user.c_str(), mysql_passwd.c_str(), mysql_db.c_str(), 0, NULL, 0)) == NULL)
    {
        logStream->errorStream() << mysql_error(&mysql) << log4cpp::eol;
	errcode = DbErr_DatabaseAccess;
        return (&errcode);
    }
//
// Mark the server as connected to the database 
//
    dbgen.connected = true;
//
// Leave server 
//
    return(&errcode);
}
