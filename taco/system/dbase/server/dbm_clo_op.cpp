/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.15 $
 *
 * Date:	$Date: 2008-04-06 09:07:40 $
 *
 */

#include <API.h>

#include <DevErrors.h>

#include <db_xdr.h>

#include <fcntl.h>

/* Some C++ include files */

#include <string>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <iostream>
#include <fstream>
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif


/**
 * To close the database to be able to reload a new database.
 *
 * @return  This function returns a pointer to an integer. This integer is simply
 *    an error code (0 if no error).
 */
DevLong *NdbmServer::db_clodb_1_svc()
{
	static DevLong errcode;

	logStream->debugStream() << "db_clodb()" << log4cpp::eol;
//
// Return error code if the server is not connected to the database files
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Disconnect server from database files
//
	for (int i = 0;i < dbgen.TblNum;i++)
		gdbm_close(dbgen.tid[i]);
	dbgen.connected = False;
//
// Leave server 
//
	errcode = 0;
	return(&errcode);
}



/**
 * Reopen the database after it has been updated by a dbm_update command in 
 * a single user mode or after the rebuilding from a backup file.
 *
 * @return This function returns a pointer to an integer. This integer is simply
 *    an error code (0 if no error).
 */
DevLong *NdbmServer::db_reopendb_1_svc()
{
	static DevLong 	errcode;
	int 		flags = O_RDWR;
	char 		*ptr;

	logStream->debugStream() << "db_reopendb" << log4cpp::eol;
//
// Find the dbm_database files
//
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		logStream->errorStream() << "dbm_server: Can't find environment variable DBM_DIR" << log4cpp::eol;
		errcode = DbErr_DatabaseAccess;
		return(&errcode);
	}

	std::string dir_name(ptr);
	if (dir_name[dir_name.size() - 1] != '/')
		dir_name.append(1,'/');
//
// Open database tables according to the definitions
//
	for (int i = 0;i < dbgen.TblNum;i++)
	{
		std::string dbm_file(dir_name);
		dbm_file.append(dbgen.TblName[i]);
		
		std::ifstream fi(dbm_file.c_str());
		if (!fi)
		{
			mode_t	old_mask = umask(0);
			GDBM_FILE t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, GDBM_WRCREAT, 0666, NULL);
			umask(old_mask);
                	if (t == NULL && gdbm_errno == GDBM_CANT_BE_WRITER)
				t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, GDBM_NOLOCK | GDBM_WRCREAT, 0666, NULL);
			if (t == NULL)
                	{
				logStream->errorStream() << "dbm_clo_op (" << gdbm_errno << "): Can't create file : " << dbm_file << log4cpp::eol;
				errcode = DbErr_DatabaseAccess;
				return(&errcode);
                	}
			gdbm_close(t);
		}

		dbgen.tid[i] = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, 0666, NULL);
		if (dbgen.tid[i] == NULL || gdbm_errno == GDBM_CANT_BE_WRITER)
		{
			flags |= GDBM_NOLOCK; 
			dbgen.tid[i] = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, 0666, NULL);
		}
		if (dbgen.tid[i] == NULL)
		{
			logStream->errorStream() <<"dbm_server (" << gdbm_errno << "): Can't open table : " << dbgen.TblName[i] << log4cpp::eol;
			errcode = DbErr_DatabaseAccess;
			return(&errcode);
		}
	} 
//
// Mark the server as connected to the database
//
	dbgen.connected = True;
//
// Leave server
//
	errcode = 0;
	return(&errcode);
}
