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
 * Version:	$Revision: 1.8 $
 *
 * Date:	$Date: 2005-07-25 08:28:41 $
 *
 */

#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// C++ include

#include <new>
#include <iostream>
#include <string>
#include <NdbmClass.h>
#include <NdbmServer.h>

#include <errno.h>



/**
 * To list the domains for all the device name defined in the NAMES and PS_NAMES tables
 * 
 * @return The domain name list
 */
db_res *NdbmServer::devdomainlist_1_svc(void)
{
#ifdef DEBUG
	std::cout << "In devdomainlist_1_svc function" << std::endl;
#endif

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

	NdbmNameList 	dom_list;
//
// Get the domain name list from the NAMES table
//
	try 
	{
		datum key, key2;
		for (key = gdbm_firstkey(dbgen.tid[0]); 
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
			dom_list.add_if_new(cont.get_dev_domain_name());
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);			
		}
//
// Add new domain from the PS_NAMES table
//
		for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]); \
		     key.dptr != NULL; \
		     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
		{
			NdbmPSNamesKey pskey(key);
			dom_list.add_if_new(pskey.get_psdev_domain_name());
		}
		if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
		{
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);		
		}
	}
	catch (NdbmError &err)
	{		
		std::cerr << err.get_err_message() << std::endl;	
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devdomainlist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// Sort domain name list
//
	dom_list.sort_name();

//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = dom_list.size();
	if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in devdomainlist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// Return data
//
	return(&browse_back);
}



/**
 * To list families for all the device defined in the NAMES and PS_NAMES tables 
 * for a given domain
 * 
 * @param domain The domain name
 *
 * @return The family name list
 */
db_res *NdbmServer::devfamilylist_1_svc(nam* domain)
{
	
	std::string 	user_domain(*domain);
#ifdef DEBUG
	std::cout << "In devfamilylist_1_svc function for domain " << user_domain << std::endl;
#endif
	
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
	NdbmNameList 	fam_list;
	try
	{
		datum 		key, 
				key2;
		for (key = gdbm_firstkey(dbgen.tid[0]); 
			key.dptr != NULL; 
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			if (cont.get_dev_domain_name() != user_domain)
				continue;
			fam_list.add_if_new(cont.get_dev_fam_name());
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	
//
// Add family name list for the wanted domain in from the PS_NAMES table
//

		for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]); \
		     key.dptr != NULL; \
		     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
		{
			NdbmPSNamesKey pskey(key);
		
			if (pskey.get_psdev_domain_name() != user_domain)
				continue;
			fam_list.add_if_new(pskey.get_psdev_fam_name());
		}
		if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
		{
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{
		std::cerr << err.get_err_message() << std::endl;		
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devfamilylist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);		
	}
	
//
// Sort family name list
//
	fam_list.sort_name();
	
//
// Build the sequence returned to caller
//
	browse_back.res_val.arr1_len = fam_list.size();
	if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in devfamilylist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Return data
//
	return(&browse_back);	
}


/**
 * To list members for all the device defined in the NAMES and PS_NAMES tables 
 * for a given domain and family
 *
 * @param recev The domain name, the family name
 *
 * @return The member name list
 */
db_res *NdbmServer::devmemberlist_1_svc(db_res *recev)
{
//
// Build strings from input names
//
	std::string user_domain(recev->res_val.arr1_val[0]);
	std::string user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
	std::cout << "In devmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << std::endl;
#endif
	
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
// Get the member name list for the wanted domain and family from NAMES table
//
	NdbmNameList 	memb_list;
	try
	{
		datum 		key, 
				key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			if (cont.get_dev_domain_name() != user_domain)
				continue;
			if (cont.get_dev_fam_name() != user_family)
				continue;
			memb_list.add_if_new(cont.get_dev_memb_name());
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[0]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
//
// Add member name for the wanted domain and family from PS_NAMES table
//
		for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]); \
		     key.dptr != NULL; \
		     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
		{
			NdbmPSNamesKey pskey(key);
		
			if (pskey.get_psdev_domain_name() != user_domain)
				continue;
			if (pskey.get_psdev_fam_name() != user_family)
				continue;
			memb_list.add_if_new(pskey.get_psdev_memb_name());
		}
		if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
		{
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devmemberlist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// Sort member name list
//
	memb_list.sort_name();
	
//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = memb_list.size();
	if (memb_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in devmemberlist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//
	return(&browse_back);	
}
