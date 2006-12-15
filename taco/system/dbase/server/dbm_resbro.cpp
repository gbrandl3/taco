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
 * Version:	$Revision: 1.10 $
 *
 * Date:	$Date: 2006-12-15 12:43:54 $
 *
 */

#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// C++ include

#include <new>
#include <string>
#include <iostream>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <errno.h>

/**
 * To retrieve resource domain list for all the resources defined in the database
 * 
 * @return The domain name list
 */
db_res *NdbmServer::resdomainlist_1_svc()
{

	logStream->debugStream() << "In resdomainlist_1_svc function" << log4cpp::CategoryStream::ENDLINE;

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
// Find each domain
//
	NdbmNameList dom_list;
	for (long i = 0;i < dbgen.TblNum;i++)
		if ((dbgen.TblName[i] == "names") || (dbgen.TblName[i] == "ps_names"))
		    	continue;
		else
			dom_list.add_if_new(dbgen.TblName[i]);
		
//
// Sort domain name list
//
	dom_list.sort_name();

//
// Bild the structure returned to caller
//
	browse_back.res_val.arr1_len = dom_list.size();
	if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in resdomainlist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Return data
//
	return(&browse_back);	
}


/**
 * To retrieve all the family defined (in resources name) for a specific domain
 * 
 * @param domain The domain name
 *
 * @return The family name list
 */
db_res *NdbmServer::resfamilylist_1_svc(nam* domain)
{
	long 		i;
	std::string 	user_domain(*domain);

	logStream->debugStream() << "In resfamilylist_1_svc function for domain " << user_domain << log4cpp::CategoryStream::ENDLINE;
	
//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
		
//
// If the server is not connected to the database, throw exception
//
	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == user_domain)
			break;
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}

//
// Get all resources family name defined in this table
//
	NdbmNameList 	fam_list;
	try
	{
		datum 		key, 
				key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);
			fam_list.add_if_new(reskey.get_res_fam_name());
		}
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in resfamilylist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Sort family name list
//
	fam_list.sort_name();
	
//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = fam_list.size();
	if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in resfamilylist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Return data
//
	return(&browse_back);	
}


/**
 * To retrieve all the family defined (in resources name) for a specific couple domain,family
 * 
 * @param recev The domain name, the family name
 *
 * @return The member name list
 */
db_res *NdbmServer::resmemberlist_1_svc(db_res *recev)
{
	long 		i;
	
	std::string user_domain(recev->res_val.arr1_val[0]);
	std::string user_family(recev->res_val.arr1_val[1]);

	logStream->debugStream() << "In resmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << log4cpp::CategoryStream::ENDLINE;
	
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
// Find the db table for the specificated domain
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == user_domain)
			break;
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}
	
//
// Get all resources family name defined in this table
//
	NdbmNameList 	memb_list;
	try
	{
		datum 	key, 
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);
			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			memb_list.add_if_new(reskey.get_res_memb_name());
		}
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in resmemberlist" << log4cpp::CategoryStream::ENDLINE;
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
		logStream->errorStream() << "Memory allocation error in resmemberlist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//
	return(&browse_back);	
}


/**
 * To retrieve a list of resources for a given device
 *
 * @param recev The device domain name, the device family name, the device member name
 * 
 * @return The member name list
 */
db_res *NdbmServer::resresolist_1_svc(db_res *recev)
{
	long 		i;
	
	std::string 	user_domain(recev->res_val.arr1_val[0]),
			user_family(recev->res_val.arr1_val[1]),
			user_member(recev->res_val.arr1_val[2]);

	logStream->debugStream() << "In resresolist_1_svc function for " << user_domain 
			<< "/" << user_family << "/" << user_member << log4cpp::CategoryStream::ENDLINE;
	
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
// Find the db table for the specificated domain
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == user_domain)
			break;
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}
	
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
	NdbmNameList 	reso_list;
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			if (reskey.get_res_indi() != 1)
				continue;			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			if (user_member != "*")
				if (reskey.get_res_memb_name() != user_member)
					continue;
			reso_list.add_if_new(reskey.get_res_name());
		}
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			free(key.dptr);
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in resresolist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// If resources belongs to SEC domain, replace all occurances of ^ by |
//
	if (user_domain == "sec")
	{
		long l = reso_list.size();
		for (long i = 0; i < l; i++)
		{
			std::string::size_type	pos = 0;
			while ((pos = reso_list[i].find_first_of(SEC_SEP,pos)) != std::string::npos)
			{
				(reso_list[i])[pos] = '|';
				pos++;
			}
		}
	}
			
//
// Sort resource name list
//
	reso_list.sort_name();
	
//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = reso_list.size();
	if (reso_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in resresolist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//
	return(&browse_back);	
}


/**
 * To retrieve a resource value (as strings)
 * 
 * @param recev The device domain name, the device family name, the device member name
 * 		the resource name
 * 
 * @return  The member name list
 */
db_res *NdbmServer::resresoval_1_svc(db_res *recev)
{
	long 		i;
		
	std::string 	user_domain(recev->res_val.arr1_val[0]),
			user_family(recev->res_val.arr1_val[1]),
			user_member(recev->res_val.arr1_val[2]),
			user_reso(recev->res_val.arr1_val[3]);

	logStream->debugStream() << "In resresoval_1_svc function for " << user_domain \
		<< "/" << user_family << "/" << user_member << "/" << user_reso << log4cpp::CategoryStream::ENDLINE;
	
//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
		
//
// If the server is not connected to the database, throw exception
//
	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == user_domain)
			break;
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}

//
// Get a list of all members and resource name
//
	NdbmDoubleNameList memb_res_list;
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			if (reskey.get_res_indi() != 1)
				continue;			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			if ((user_member != "*") && (reskey.get_res_memb_name() != user_member))
				continue;
			if ((user_reso != "*") && (reskey.get_res_name() != user_reso))
				continue;
			memb_res_list.add(reskey.get_res_memb_name(), reskey.get_res_name());
		}
		free(key.dptr);
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		logStream->errorStream() << err.get_err_message() << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		logStream->errorStream() << "Memory allocation error in resresolist" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

	NdbmNameList 	reso_val;
	long nb_memb = memb_res_list.first_name_length();
	for (long k = 0; k < nb_memb; k++)
	{
		long nb_res = memb_res_list.sec_name_length(k);
		for (long j = 0; j < nb_res; j++)
		{
			long		seq = 1;
			std::string	res_val;
			try
			{
				std::string	memb,
						resource;
				memb_res_list.get_record(k, j, memb, resource);
//
// Build the first field of the complete resource name
//
				res_val = user_domain + "/" + user_family + "/" + memb + "/" + resource + ": ";
//
// Get resource value
//
				NdbmResKey reskey(user_family, memb, resource, seq);
				while(1)
				{
					if (seq != 1)
						reskey.upd_indi(seq); 
	
					NdbmResCont rescont(dbgen.tid[i],reskey.get_key());
					if (seq == 1)
						res_val += rescont.get_res_value();
					else
						res_val += "," + rescont.get_res_value();
					seq++;
				}
			}
			catch (NdbmError)
			{
				if (seq == 1)
				{
					if (gdbm_error(dbgen.tid[i]) != 0)
					{
						gdbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					else if ((user_member != "*") && (user_reso != "*"))
					{
						browse_back.db_err = DbErr_ResourceNotDefined;
						return(&browse_back);
					}
				}
				else
				{
					if (gdbm_error(dbgen.tid[i]) != 0)
					{
						gdbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					reso_val.add_if_new(res_val);
				}
			}	
			catch (std::bad_alloc)
			{
				logStream->errorStream() << "Memory allocation error in resresoval" << log4cpp::CategoryStream::ENDLINE;
				browse_back.db_err = DbErr_ServerMemoryAllocation;
				return(&browse_back);
			}	
		}
	}
			
//
// If resources belongs to SEC domain, replace all occurances of ^ by |
//
	if (user_domain == "sec")
	{
		long j = reso_val.size();
		for (long i = 0; i < j; i++)
		{
			std::string::size_type	pos = 0;
			while ((pos = reso_val[i].find_first_of(SEC_SEP,pos)) != std::string::npos)
			{
				(reso_val[i])[pos] = '|';
				pos++;
			}
		}
	}	
//
// Sort the resource array
//
	reso_val.sort_name();		
	
//
// Build the structure returned to caller
//
	browse_back.res_val.arr1_len = reso_val.size();
	if (reso_val.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		logStream->errorStream() << "Memory allocation error in resresoval" << log4cpp::CategoryStream::ENDLINE;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//
	return(&browse_back);
}
