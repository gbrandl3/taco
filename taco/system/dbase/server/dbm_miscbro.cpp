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
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2008-04-06 09:07:40 $
 *
 */

#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// C++ include

#include <new>
#include <iostream>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <string>

#include <errno.h>



/**
 * To retrieve device server list for all the devices defined in the NAMES table
 * 
 * @return The domain name list
 */
db_res *NdbmServer::devserverlist_1_svc()
{
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
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

//
// Get the server name list from the NAMES table
//
	NdbmNameList 	serv_list;
	try 
	{
		datum 		key,
				key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesKey nameskey(key);

			if (nameskey.get_dev_indi() != 1)
				continue;
			serv_list.add_if_new(nameskey.get_ds_name());		
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);			
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::eol;	
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in devserverlist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Sort device server name list
//
	serv_list.sort_name();

//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = serv_list.size();
	if (serv_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in devserverlist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Return data
//
	return(&browse_back);
}



/**
 * To list the personal names for a server defined in the NAMES and PS_NAMES tables
 * 
 * @param server The server name
 * 
 * @return The family name list
 */
db_res *NdbmServer::devpersnamelist_1_svc(DevString *server)
{
	std::string user_server(*server);
	
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
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

//
// Get the family name list for the wanted domain in the NAMES table
//
	NdbmNameList 	pers_list;
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesKey nameskey(key);

			if (nameskey.get_dev_indi() != 1)
				continue;
			if (nameskey.get_ds_name() != user_server)
				continue;
			pers_list.add_if_new(nameskey.get_ds_pers_name());
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{
		logStream->errorStream() << err.get_err_message() << log4cpp::eol;		
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in devpersnamelist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);		
	}
	
//
// Sort device server personal name list
//
	pers_list.sort_name();
	
//
// Build the sequence returned to caller
//
	browse_back.res_val.arr1_len = pers_list.size();
	if (pers_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in devpersnamelist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Return data
//
	return(&browse_back);	
}




/**
 * To list the hosts where device server should run
 * 
 * @return The host name list
 */
db_res *NdbmServer::hostlist_1_svc()
{

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
// Get the server name list from the NAMES table
//
	NdbmNameList 	host_list;
	try 
	{
		datum	key,
			key2;

		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
//
// Skip record if the device indice is different than 1
//
			NdbmNamesKey nameskey(key);
			
			if (nameskey.get_dev_indi() != 1)
				continue;
			NdbmNamesCont cont(dbgen.tid[0],key);
			std::string host = cont.get_host_name();
//
// Skip record if the device is not exported (for DS with DB library release 4)
//
			if (host == "not_exp")
				continue;
			host_list.add_if_new(host);		
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);			
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::eol;	
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in devserverlist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Sort device server host list
//
	host_list.sort_name();

//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = host_list.size();
	if (host_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in hostlist" << log4cpp::eol;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Return data
//
	return(&browse_back);
}
