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
 * Version:	$Revision: 1.11 $
 *
 * Date:	$Date: 2006-12-15 12:43:54 $
 *
 */

#include <API.h>

#include <DevErrors.h>

#include <ctype.h>
#include <db_xdr.h>

/* Some C++ include files */

#include <iostream>
#include <algorithm>
#include <NdbmClass.h>
#include <NdbmServer.h>


/**
 * To store in the database (built from resources files) the host_name, the 
 * program number and the version number of the device server for a specific device
 * 
 * @param rece A pointer to a structure of the tab_dbdev type 
 *            struct { 
 *              u_int tab_dbdev_len;     The number of structures db_devinfo 
 *              db_devinfo *tab_dbdev_val;    A pointer to the array of structures 
 *                  }
 *
 * @return This function returns an integer which is an error code Zero means no error.
 */
db_psdev_error *NdbmServer::db_psdev_reg_1_svc(psdev_reg_x *rece)
{
	static db_psdev_error 	err;
	int 			num_psdev = rece->psdev_arr.psdev_arr_len;;
//
// Miscellaneous init
//
	err.error_code = 0;
	err.psdev_err = 0;

	logStream->debugStream() << "Begin db_psdev_register" << log4cpp::CategoryStream::ENDLINE;
	logStream->debugStream() << "Host name : " << rece->h_name << log4cpp::CategoryStream::ENDLINE;
	logStream->debugStream() << "PID = " << rece->pid << log4cpp::CategoryStream::ENDLINE;
	for (int i = 0; i < num_psdev; i++)
	{
		logStream->debugStream() << "Pseudo device name : " << rece->psdev_arr.psdev_arr_val[i].psdev_name << log4cpp::CategoryStream::ENDLINE;
		logStream->debugStream() << "Refresh period : " << rece->psdev_arr.psdev_arr_val[i].poll << log4cpp::CategoryStream::ENDLINE;
	}
//
// Return error code if the server is not connected */
//
	if (dbgen.connected == False)
	{
		err.error_code = DbErr_DatabaseNotConnected;
		return(&err);
	}
//
// For each pseudo device, register it in the PS_NAMES table
//
	for (int i = 0;i < num_psdev;i++)
	{
		psdev_elt *tmp = &(rece->psdev_arr.psdev_arr_val[i]);
		long error;
		if (reg_ps(rece->h_name,rece->pid, tmp->psdev_name, tmp->poll, &error) == DS_NOTOK)
		{
			err.error_code =  error;
			err.psdev_err = i + 1;
			return(&err);
		}
	}
//
// Leave server 
//
	logStream->debugStream() << "End db_psdev_register" << log4cpp::CategoryStream::ENDLINE;
	return(&err);
}


/**
 * To register a pseudo device in the PS_NAMES database table
 *
 * @param h_name The host name
 * @param pid The process PID
 * @param ps_name The pseudo device name
 * @param poll The polling period
 *
 * @param p_error Pointer for an error code
 *
 * @return This function returns 0 is everything OK. Otherwise, the returned value
 *    is DS_NOTOK and the error code is set to the appropiate error.
 */
long NdbmServer::reg_ps(char *h_name, long pid, char *ps_name, long poll, long *p_error)
{
	std::string 	ps_name_low(ps_name);
//
// Make a copy of the pseudo device name in lowercase letter 
//
	std::transform(ps_name_low.begin(), ps_name_low.end(), ps_name_low.begin(), ::tolower);

//
// First, check that the name used for the pseudo device is not already used for a real device
//
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
			if (cont.get_device_name() == ps_name_low)
				break;
		}
		if (key.dptr == NULL)
		{
			if (gdbm_error(dbgen.tid[0]) != 0)
			{
				*p_error = DbErr_DatabaseAccess;
				return(DS_NOTOK);
			}
		}
		else
		{
			*p_error = DbErr_NameAlreadyUsed;
			return(DS_NOTOK);
		}
//
// Now, try to retrieve a tuple in the PS_NAMES table with the same pseudo device name 
//
		key.dptr = const_cast<char *>(ps_name_low.c_str());
		key.dsize = strlen(key.dptr);

		char	cont_buf[MAX_CONT];
		snprintf(cont_buf, sizeof(cont_buf), "%s|%d|%d|", h_name, pid, poll);

		datum content;
		content.dptr = cont_buf;
		content.dsize = strlen(cont_buf);

		try
		{
			NdbmPSNamesCont cont(dbgen.tid[dbgen.ps_names_index],key);
//
// Update database information 
//
			if (gdbm_store(dbgen.tid[dbgen.ps_names_index],key,content,GDBM_REPLACE) != 0)
			{
				*p_error = DbErr_DatabaseAccess;
				return(DS_NOTOK);
			}
		}
		catch ( ... )
		{
//
// In case of error 
//
			if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
			{
				*p_error = DbErr_DatabaseAccess;
				return(DS_NOTOK);
			}
//
// Insert a new record in database 
//
			else if (gdbm_store(dbgen.tid[dbgen.ps_names_index],key,content,GDBM_INSERT) != 0)
			{
				*p_error = DbErr_DatabaseAccess;
				return(DS_NOTOK);
			}
		}
        }
        catch (NdbmError &err)
        {
                logStream->errorStream() << err.get_err_message() << log4cpp::CategoryStream::ENDLINE;
                *p_error = err.get_err_code();
                return(DS_NOTOK);
        }
        catch (std::bad_alloc)
        {
                logStream->errorStream() << "Memory allocation error in devserverlist" << log4cpp::CategoryStream::ENDLINE;
                *p_error = DbErr_ServerMemoryAllocation;
                return(DS_NOTOK);
        }
//
// Free memory and leave function 
//
	return(DS_OK);
}


/**
 * To retrieve (from database) the host_name, the program number and the version 
 * number for specific devices
 * 
 * @param rece A pointer to a structure of the arr1 type
 *            struct {
 *               u_int arr1_len;      The number of strings
 *               char **arr1_val;     A pointer to the array of strings
 *                   }
 *
 * @return This function returns a pointer to a structure of the db_resimp type 
 *    struct {
 *      tab_dbdev imp_dev;   A structure of the tab_dbdev type (see above)
 *                           with the informations needed (host_name,
 *                           program number and version number)
 *      int db_imperr;    The database error code
 *                        0 if no error
 *            }
 */
db_psdev_error *NdbmServer::db_psdev_unreg_1_svc(arr1 *rece)
{
	static db_psdev_error 	err;
	u_int 			num_psdev = rece->arr1_len;;
//
// Miscellaneous init
//
	err.error_code = 0;
	err.psdev_err = 0;

	logStream->debugStream() << "Begin db_psdev_unregister" << log4cpp::CategoryStream::ENDLINE;
	for (int i = 0;i < num_psdev;i++)
		logStream->debugStream() << "Pseudo device name : " << rece->arr1_val[i] << log4cpp::CategoryStream::ENDLINE;
//
// Return error code if the server is not connected
//
	if (dbgen.connected == False)
	{
		err.error_code = DbErr_DatabaseNotConnected;
		return(&err);
	}
//
// For each pseudo device, unregister it in the PS_NAMES table
//
	u_int	i;
	long	error;
	for (i = 0; i < num_psdev; i++)
		if (unreg_ps(rece->arr1_val[i],&error) == DS_NOTOK)
		{
			err.error_code =  error;
			err.psdev_err = i + 1;
			return(&err);
		}
//
// Leave server 
//
	logStream->debugStream() << "End db_psdev_unregister" << log4cpp::CategoryStream::ENDLINE;
	return(&err);
}



/**
 * To unregister pseudo device from the database PS_NAMES table
 *
 * @param ps_name The pseudo device name
 *
 * @param p_error Pointer for an error code
 *
 * @return This function returns 0 is everything OK. Otherwise, the returned value
 *    is DS_NOTOK and the error code is set to the appropiate error.
 */
long NdbmServer::unreg_ps(char *ps_name,long *p_error)
{
	datum 		key;

	std::string 	ps_name_low(ps_name);
//
// Make a copy of the pseudo device name in lowercase letter 
//
	std::transform(ps_name_low.begin(), ps_name_low.end(), ps_name_low.begin(), ::tolower);
//
// Retrieve a tuple in the PS_NAMES table with the same pseudo device name */
//
	key.dptr = const_cast<char *>(ps_name_low.c_str());
	key.dsize = strlen(key.dptr);
	try
	{
		NdbmPSNamesCont cont(dbgen.tid[dbgen.ps_names_index], key);
//
// Remove pseudo device
//
		if (gdbm_delete(dbgen.tid[dbgen.ps_names_index],key) != 0)
		{
			*p_error = DbErr_DatabaseAccess;
			return(DS_NOTOK);
		}
	}
	catch ( ... )
	{
		if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
			*p_error = DbErr_DatabaseAccess;
		else
			*p_error = DbErr_DeviceNotDefined;
		return(DS_NOTOK);
	}
//
// Free memory and leave function
//
	return(DS_OK);
}
