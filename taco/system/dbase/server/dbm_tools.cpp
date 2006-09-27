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
 * Version:	$Revision: 1.13 $
 *
 * Date:	$Date: 2006-09-27 12:29:19 $
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
#include <vector>
#include <algorithm>

#include <errno.h>


/**
 * To return device information
 * 
 * @param dev The device name
 * 
 * @return This function returns a pointer to a structure with all device info 
 *    and an error code which is set if needed
 */
db_devinfo_svc *NdbmServer::devinfo_1_svc(nam *dev)
{
	datum 		key;
	std::string 	device;
	
#ifdef DEBUG
	std::cout << "In devinfo_1_svc function for device " << *dev << std::endl;
#endif
	std::string user_device(*dev);

//
// Initialize parameter sent back to client and allocate memory for string
// (bloody XDR)
//
	sent_back.db_err = 0;

	sent_back.device_class = NULL;
	sent_back.server_name = NULL;
	sent_back.personal_name = NULL;
	sent_back.process_name = NULL;
	sent_back.host_name = NULL;
	
	try
	{	
		sent_back.device_class = new char[DEV_CLASS_LENGTH];
		sent_back.device_class[0] = '\0';
	
		sent_back.server_name = new char[DS_NAME_LENGTH];
		sent_back.server_name[0] = '\0';
	
		sent_back.personal_name = new char[DSPERS_NAME_LENGTH];
		sent_back.personal_name[0] = '\0';
	
		sent_back.process_name = new char[PROC_NAME_LENGTH];
		sent_back.process_name[0] = '\0';

		sent_back.host_name = new char[HOST_NAME_LENGTH];	
		sent_back.host_name[0] = '\0';
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in devinfo" << std::endl;
		sent_back.db_err = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}
	
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		sent_back.db_err = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}
	
//
// Search for device name in the NAMES table
//
	long found = False;
	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			device = cont.get_device_name();

			if (device == user_device)
			{
				cont.get_devinfo(sent_back);
				
				NdbmNamesKey dev_key(key);				
				dev_key.get_devinfo(sent_back);

				sent_back.device_type = DB_Device;				
				found = True;
				free(key.dptr);
				break;
			}		
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[0]);
			sent_back.db_err = DbErr_DatabaseAccess;
			return(&sent_back);			
		}

//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//

		if (found == False)
		{
			datum	key2;
			for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]); 
		     	     key.dptr != NULL; 
		     	     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
			{
				std::string ps_dev_name(key.dptr,key.dsize);
				
				if (ps_dev_name == user_device)
				{
					NdbmPSNamesCont pscont(dbgen.tid[dbgen.ps_names_index],key);
					
					pscont.get_devinfo(sent_back);
					
					sent_back.device_type = DB_Pseudo_Device;
					found = True;
					free(key.dptr);
					break;
				}
		
			}
			if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
			{
				gdbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
				sent_back.db_err = DbErr_DatabaseAccess;
				return(&sent_back);		
			}
		}
	}
	catch (NdbmError &err)
	{		
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		sent_back.db_err = err.get_err_code();
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devinfo" << std::endl;
		sent_back.db_err = DbErr_ServerMemoryAllocation;
	}
	
//
// Return error if the device is not found
//
	if (found == False)
	{
		sent_back.db_err = DbErr_DeviceNotDefined;
	}
		
//
// Return data
//
	return(&sent_back);

}


/**
 * To retrieve all resources belonging to a device
 *
 * @param recev The device name list
 * 
 * @return This function returns the resource list and an error code which is set 
 *    if needed
 */
db_res *NdbmServer::devres_1_svc(db_res *recev)
{
	std::vector<NdbmDomDev> 	dom_list;
	std::vector<std::string> 	tmp_res_list;
	datum 				key;
	std::string 			fam,
					memb,
					resource,
					tmp_res,
					r_name;
	long 				i,
					j,
					k,
					ind,
					sec_res = False;
	std::string::size_type 		pos;
	
#ifdef DEBUG
	std::cout << "In devres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << std::endl;
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
// Build a vector of object from the NdbmDomDev class. Each object in this class
// has a domain name and a list of family/member for this domain
//
	for (i = 0;i < recev->res_val.arr1_len;i++)
	{
		std::string in_dev(recev->res_val.arr1_val[i]);

		if ((pos = in_dev.find(SEP_DEV)) == std::string::npos)
		{
			browse_back.db_err = DbErr_BadDevSyntax;
			return(&browse_back);
		}
		
		std::string in_domain(in_dev,0,pos);
		std::string fam_memb(in_dev,pos + 1,in_dev.size());
		
		NdbmDomDev tmp(in_domain,fam_memb);				
		std::vector<NdbmDomDev>::iterator p = std::find(dom_list.begin(),dom_list.end(),tmp);
				
		if (p == dom_list.end())
		{
			dom_list.push_back(tmp);
		}
		else
			(*p).add_dev(fam_memb);
	}
	res_list_dev.clear();
	try
	{
		for (i = 0;i < dom_list.size();i++)
		{
//
// Find the db table for the specified domain
//
			for (j = 0;j < dbgen.TblNum;j++)
			{
				if (dbgen.TblName[j] == dom_list[i].get_domain())
					break;
			}
			if (j == dbgen.TblNum)
			{
				browse_back.db_err = DbErr_DomainDefinition;
				return(&browse_back);
			}
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
			tmp_res_list.clear();
			datum	key2;
			for (key = gdbm_firstkey(dbgen.tid[j]);
				key.dptr != NULL;
				key2 = key, key = gdbm_nextkey(dbgen.tid[j], key), free(key2.dptr))
			{
				long ind;
				long found;
				NdbmResKey reskey(key);

				ind = reskey.get_res_indi();
				if (ind != 1)
					continue;			
				fam = reskey.get_res_fam_name();
				memb = reskey.get_res_memb_name();

				std::string fm(fam);
				fm = fm + "/" + memb;
				
				found = dom_list[i].find_in_list(fm);
				
				if (found == True)
				{
//
// A resource for the device has been found. Store its name in the resource name list. 
//
					std::string t(key.dptr,key.dsize);
					tmp_res_list.push_back(t);
				}
			}
			if (gdbm_error(dbgen.tid[j]) != 0)
			{			
				gdbm_clearerr(dbgen.tid[j]);
				browse_back.db_err = DbErr_DatabaseAccess;
				return(&browse_back);			
			}
			
//
// Get resource value for each element in the tmp_res_list list 
//
			for (k = 0;k < tmp_res_list.size();k++)
			{
				try
				{
					ind = 1;
				
					NdbmResKey key(tmp_res_list[k]);
					fam = key.get_res_fam_name();
					memb = key.get_res_memb_name();
					r_name = key.get_res_name();
					resource = dom_list[i].get_domain() + "/" + fam + "/" + memb + "/" + r_name;			
					while(1)
					{
						if (ind != 1)
							key.upd_indi(ind); 
	
						NdbmResCont rescont(dbgen.tid[j],key.get_key());
			
						tmp_res = rescont.get_res_value();
						if (ind == 1)
							resource += ": " + tmp_res;
						else
							resource += "," + tmp_res;
						ind++;
					}
				}
				catch (NdbmError &err)
				{
					if (ind == 1)
					{		
						std::cerr << err.get_err_message() << std::endl;
						browse_back.db_err = err.get_err_code();
						return(&browse_back);
					}
					else
					{
						if (gdbm_error(dbgen.tid[j]) != 0)
						{
							gdbm_clearerr(dbgen.tid[j]);
							browse_back.db_err = DbErr_DatabaseAccess;
							return(&browse_back);
						}

//					
// If the resource belongs to the sec domain, replace ^ character by the original
// | character
//
						if (sec_res == True)
						{
							pos = 0;
							while ((pos = resource.find_first_of(SEC_SEP,pos)) != std::string::npos)
							{
								resource[pos] = '|';
								pos++;
							}	
						}
						res_list_dev.push_back(resource);
					}
				}
			}
		}
	}
	catch (NdbmError &err)
	{		
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devres_svc" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Sort resource name list
//
	std::sort(res_list_dev.begin(),res_list_dev.end());
		
// 
// Build the structure returned to caller
//
	long res_nb = res_list_dev.size();
	try
	{		
		browse_back.res_val.arr1_val = new nam[res_nb];
		for (i = 0;i < res_list_dev.size();i++)
		{
			browse_back.res_val.arr1_val[i] = new char[res_list_dev[i].size() + 1];
			strcpy(browse_back.res_val.arr1_val[i],res_list_dev[i].c_str());
		}
		browse_back.res_val.arr1_len = res_list_dev.size();
	}
	catch (std::bad_alloc)
	{
		if (browse_back.res_val.arr1_val != NULL)
		{
			for (j = 0;j < i;j++)
				delete [] browse_back.res_val.arr1_val;
			delete [] browse_back.res_val.arr1_val;
			
			std::cerr << "Memory allocation error in devres_svc" << std::endl;
			browse_back.db_err = DbErr_ServerMemoryAllocation;
			return(&browse_back);
		}
	}
				
//
// Return data
//
	return(&browse_back);	
}


/**
 * To delete a device from the database
 * 
 * @param dev The device name
 * 
 * @return This function returns a pointer to an error code
 */
long *NdbmServer::devdel_1_svc(nam *dev)
{
	datum 		key;
	std::string 	device;
	static long 	sent_back;
	
#ifdef DEBUG
	std::cout << "In devdel_1_svc function for device " << *dev << std::endl;
#endif
	std::string user_device(*dev);
//
// Initialize parameter sent back to client
//
	sent_back = 0;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		sent_back = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}
//
// Search for device name in the NAMES table
//
	long found = False;
	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			device = cont.get_device_name();

			if (device == user_device)
			{
				std::string ds_name;
				std::string ds_pers_name;
				long ind;
				
				NdbmNamesKey k(key);
//
// Memorize key members
//				
				ds_name = k.get_ds_name();
				ds_pers_name = k.get_ds_pers_name();
				ind = k.get_dev_indi();
//
// Delete device from table
//
				gdbm_delete(dbgen.tid[0],key);
				found = True;
//
// Update device server device list (decrement all device index in device list
// for all devices above the deleted one)
//
				try
				{
					while(1)
					{
						ind++;
						NdbmNamesKey new_key(ds_name,ds_pers_name,ind);
						NdbmNamesCont dbco(dbgen.tid[0],new_key.get_key());
					
						gdbm_delete(dbgen.tid[0],new_key.get_key());
					
						new_key.upd_indi(ind - 1);
						
						if (gdbm_store(dbgen.tid[0],new_key.get_key(),dbco.get_datum(),GDBM_INSERT) != 0)
						{
							sent_back = DbErr_DatabaseAccess;
							free(key.dptr);
							return(&sent_back);
						}
					}
				}
				catch (NdbmError &err)
				{
					if (gdbm_error(dbgen.tid[0]) != 0)
					{
						free(key.dptr);
						gdbm_clearerr(dbgen.tid[0]);
						sent_back = DbErr_DatabaseAccess;
						return(&sent_back);
					}
				}
				break;
			}		
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[0]);
			sent_back = DbErr_DatabaseAccess;
			return(&sent_back);			
		}
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
		if (found == False)
		{
			for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]); 
		     	     key.dptr != NULL; 
		     	     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
			{
				std::string ps_dev_name(key.dptr,key.dsize);
				
				if (ps_dev_name == user_device)
				{			
					gdbm_delete(dbgen.tid[dbgen.ps_names_index],key);
					found = True;
					free(key.dptr);
					break;
				}
		
			}
			if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
			{
				gdbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
				sent_back = DbErr_DatabaseAccess;
				return(&sent_back);		
			}
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		sent_back = err.get_err_code();
		return(&sent_back);
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in devdel" << std::endl;
		sent_back = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}
//
// Return error if the device is not found
//
	if (found == False)
	{
		sent_back = DbErr_DeviceNotDefined;
		return(&sent_back);
	}
//
// Return data
//
	return(&sent_back);

}


/**
 * To delete all the resources belonging to a device
 * 
 * @param recev The device name list
 *
 * @return This function returns a pointer to an error code
 */
db_psdev_error *NdbmServer::devdelres_1_svc(db_res *recev)
{
	datum 		key;
	std::string 	fam,
			memb,
			resource;
	static db_psdev_error sent_back;
	long 		i,
			j,
			l,
			m,
			ind;
	std::vector<NdbmDomDev> dom_list;
	std::vector<std::string> res_list;
	std::string tmp_dev_name;
	std::string::size_type pos;
	long found;
	
#ifdef DEBUG
	std::cout << "In devdelres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << std::endl;
#endif


//
// Initialize parameter sent back to client
//

	sent_back.error_code = 0;
	sent_back.psdev_err = 0;
	
//
// If the server is not connected to the database, return error
//

	if (dbgen.connected == False)
	{
		sent_back.error_code = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}
	
//
// Build a vector of object from the NdbmDomDev class. Each object in this class
// has a domain name and a list of family/member for this domain
//

	for (i = 0;i < recev->res_val.arr1_len;i++)
	{
		std::string in_dev(recev->res_val.arr1_val[i]);
		
		if ((pos = in_dev.find(SEP_DEV)) == std::string::npos)
		{
			sent_back.error_code = DbErr_BadDevSyntax;
			sent_back.psdev_err = i;
			return(&sent_back);
		}
		
		std::string in_domain(in_dev,0,pos);
		std::string fam_memb(in_dev,pos + 1,in_dev.size());
		
		NdbmDomDev tmp(in_domain,fam_memb);				
		std::vector<NdbmDomDev>::iterator p = std::find(dom_list.begin(),dom_list.end(),tmp);
				
		if (p == dom_list.end())
		{
			dom_list.push_back(tmp);
		}
		else
			(*p).add_dev(fam_memb);
	}

	try
	{
		for (i = 0;i < dom_list.size();i++)
		{
				
//
// Find the db table for the specified domain
//

			for (j = 0;j < dbgen.TblNum;j++)
			{
				if (dbgen.TblName[j] == dom_list[i].get_domain())
			 		break;
			}
			if (j == dbgen.TblNum)
			{
				sent_back.error_code = DbErr_DomainDefinition;
				tmp_dev_name = dom_list[i].get_domain() + '/' + dom_list[i].get_fm(0);
				for (l = 0;l < recev->res_val.arr1_len;l++)
				{
					if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
						sent_back.psdev_err = l;
				} 
				return(&sent_back);
			}
	
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
			datum	key2;
			for (key = gdbm_firstkey(dbgen.tid[j]);
				key.dptr != NULL;
				key2 = key, key = gdbm_nextkey(dbgen.tid[j], key), free(key2.dptr))
			{
				NdbmResKey reskey(key);

				ind = reskey.get_res_indi();
				if (ind != 1)
					continue;			
				fam = reskey.get_res_fam_name();
				memb = reskey.get_res_memb_name();
				
				std::string fm(fam);
				fm = fm + "/" + memb;
				
				found = dom_list[i].find_in_list(fm);
				
				if (found == True)
				{				
					resource = reskey.get_res_name();
//
// Push this resource into resource array (already built as key)
//
					std::string tmp_res_key(fam);
					tmp_res_key = tmp_res_key + '|' + memb + '|' + resource + "|1|";
					res_list.push_back(tmp_res_key);
				}
			}
//
// Now, remove all resources for this domain
//
			for (m = 0;m < res_list.size();m++)
			{
				try
				{
					int res;
					ind = 1;
					NdbmResKey k(res_list[m]);			
					while(1)
					{
						fam = k.get_res_fam_name();
						memb = k.get_res_memb_name();
				
						if (ind != 1)
							k.upd_indi(ind); 
	
						res = gdbm_delete(dbgen.tid[j],k.get_key());
						if (res == 0)
							ind++;
						else
						{
							if (ind == 1)
							{
								sent_back.error_code = DbErr_DatabaseAccess;
								fam = k.get_res_fam_name();
								memb = k.get_res_memb_name();
								tmp_dev_name = dom_list[i].get_domain() + '/' + fam + '/' + memb;
								for (l = 0;l < recev->res_val.arr1_len;l++)
								{
									if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
										sent_back.psdev_err = l;
								}
								return(&sent_back);
							}
							else
							{
								gdbm_clearerr(dbgen.tid[j]);
								break;
							}
						}					
					}
				}
				catch (NdbmError &err)
				{
					if (ind == 1)
					{		
						std::cerr << err.get_err_message() << std::endl;
						tmp_dev_name = dom_list[i].get_domain() + '/' + fam + '/' + memb;
						for (l = 0;l < recev->res_val.arr1_len;l++)
						{
							if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
								sent_back.psdev_err = l;
						}
						sent_back.error_code = err.get_err_code();
						return(&sent_back);
					}
					else
					{
						if (gdbm_error(dbgen.tid[j]) != 0)
						{
							gdbm_clearerr(dbgen.tid[j]);
							sent_back.error_code = DbErr_DatabaseAccess;
							tmp_dev_name = dom_list[i].get_domain() + '/' + fam + '/' + memb;
							for (l = 0;l < recev->res_val.arr1_len;l++)
							{
								if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
									sent_back.psdev_err = l;
							}
							return(&sent_back);
						}
					}
				}
			}

//
// Clear resource list
//
			res_list.clear();
		}
	}
	catch (NdbmError &err)
	{		
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;
		sent_back.error_code = err.get_err_code();
		tmp_dev_name = dom_list[i].get_domain() + '/' + fam + '/' + memb;
		for (l = 0;l < recev->res_val.arr1_len;l++)
		{
			if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
				sent_back.psdev_err = l;
		}
		return(&sent_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in devdelres_svc" << std::endl;
		sent_back.error_code = DbErr_ServerMemoryAllocation;
		tmp_dev_name = dom_list[i].get_domain() + '/' + fam + '/' + memb;
		for (l = 0;l < recev->res_val.arr1_len;l++)
		{
			if (strcmp(recev->res_val.arr1_val[l],tmp_dev_name.c_str()) == 0)
				sent_back.psdev_err = l;
		}
		return(&sent_back);
	}
//
// leave fucntion
//
	return(&sent_back);
}


/**
 * To get global information on the database
 * 
 * @return This function returns a pointer to a structure with all the database info
 */
db_info_svc *NdbmServer::info_1_svc()
{
	long 		dev_defined,
			dev_exported,
			psdev_defined,
			res_num;
	unsigned long 	pg;
	std::string 	domain_name;
	long 		length,
			i,
			tmp_res;
	datum 		key;

#ifdef DEBUG
	std::cout << "In info_1_svc function" << std::endl;
#endif /* DEBUG */

//
// Initialize parameter sent back to client
//

	info_back.db_err = 0;
	info_back.dev_defined = 0;
	info_back.dev_exported = 0;
	info_back.psdev_defined = 0;
	info_back.res_number = 0;
	info_back.dev.dom_len = 0;
	info_back.res.dom_len = 0;

//
// Miscellaneous init.
//

	dev_defined = dev_exported = psdev_defined = res_num = 0;
		
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		info_back.db_err = DbErr_DatabaseNotConnected;
		return(&info_back);
	}

	dom_list.clear();
	res_list.clear();
//
// First, count devices
//
	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			dev_defined++;
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			domain_name = cont.get_dev_domain_name();
			NdbmDomain tmp(domain_name, 0);				
			std::vector<NdbmDomain>::iterator p = std::find(dom_list.begin(),dom_list.end(),tmp);
				
			if (p == dom_list.end())
				dom_list.push_back(tmp);
			if (cont.get_p_num())			
			{
				dev_exported++;
				(*p).inc_nb();
			}
		}
		
//
// Now, count pseudo_devices
//
		for (key = gdbm_firstkey(dbgen.tid[dbgen.ps_names_index]);
		     key.dptr != NULL;
		     key2 = key, key = gdbm_nextkey(dbgen.tid[dbgen.ps_names_index], key), free(key2.dptr))
		{
			psdev_defined++;
		}
//
// Then, count resources in each domain
//
		for (i = 1; i < dbgen.TblNum;i++)
		{
			if (i == dbgen.ps_names_index)
				continue;

			tmp_res = 0;				
			for (key = gdbm_firstkey(dbgen.tid[i]);
			     key.dptr != NULL;
			     key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
			{
				tmp_res++;
			}
			if (tmp_res != 0)
			{
				NdbmDomain tmp(dbgen.TblName[i],tmp_res);
				res_list.push_back(tmp);
				res_num += tmp_res;
			}
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		info_back.db_err = err.get_err_code();
		return(&info_back);
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in info" << std::endl;
		info_back.db_err = DbErr_ServerMemoryAllocation;
		return(&info_back);
	}
//
// Sort exported devices domain list and resource domain list
//
	std::sort(dom_list.begin(),dom_list.end());
	std::sort(res_list.begin(),res_list.end());
//
// Return data
//
	info_back.dev_defined = dev_defined;
	info_back.dev_exported = dev_exported;
	info_back.psdev_defined = psdev_defined;
	info_back.res_number = res_num;

	length = dom_list.size();
	info_back.dev.dom_val = new db_info_dom_svc [length];
	info_back.dev.dom_len = length;	
	for (i = 0;i < length;i++)
	{
		info_back.dev.dom_val[i].dom_elt = dom_list[i].get_nb();
		info_back.dev.dom_val[i].dom_name = (char *)dom_list[i].get_name();
	}
	
	length = res_list.size();
	info_back.res.dom_val = new db_info_dom_svc [length];
	info_back.res.dom_len = length;	
	for (i = 0;i < length;i++)
	{
		info_back.res.dom_val[i].dom_elt = res_list[i].get_nb();
		info_back.res.dom_val[i].dom_name = (char *)res_list[i].get_name();
	}
	
	return(&info_back);
}


/**
 * To unregister a server from the database (to mark all its devices as not exported)
 *
 * @param recev A pointer to a structure where the first element is the device server 
 *		name and the second element is the device server personal name.
 *
 * @return This function returns a pointer to a long which will be set in case of error
 */
long *NdbmServer::unreg_1_svc(db_res *recev)
{
	static long 	sent_back;
	datum 		key;
	long 		indi,
			i;
	std::string 	tmp;
	long 		found = False;
	std::vector<std::string> ds_name_list;
	
	std::string user_ds_name(recev->res_val.arr1_val[0]);
	std::string user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
	std::cout << "In unreg_1_svc function for " << user_ds_name \
	<< "/" << user_pers_name << std::endl;
#endif
	
//
// Initialize structure sent back to client
//
	sent_back = 0;
		
//
// If the server is not connected to the database, returns error
//
	if (dbgen.connected == False)
	{
		sent_back = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}

//
// First, suppose that the ds_name is a PROCESS name
//
	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesKey k(key);
			
			indi = k.get_dev_indi();
			if (indi != 1)
				continue;
				
			tmp = k.get_ds_pers_name();
			if (tmp != user_pers_name)
				continue;

			k.build_datum();				
			NdbmNamesCont co(dbgen.tid[0],k.get_key());
			tmp = co.get_process_name();
			if (tmp != user_ds_name)
				continue;
				
			tmp = k.get_ds_name();
			ds_name_list.push_back(tmp);
			found = True;
		}

//
// If the ds_name was not a process name, init the class list with the user
// ds name
//		
		if (found == False)
			ds_name_list.push_back(user_ds_name);

//
// Unregister every devices for each ds name in the list
//
			
		for (i = 0;i < ds_name_list.size();i++)
		{
			indi = 1;
			NdbmNamesKey k(ds_name_list[i],user_pers_name,indi);

			try
			{
				while(1)
				{
					if (indi != 1)
						k.upd_indi(indi);			
					NdbmNamesCont co(dbgen.tid[0],k.get_key());
					found = True;
					co.unreg();
//					co.build_datum();
		
					gdbm_store(dbgen.tid[0],k.get_key(),co.get_datum(),GDBM_REPLACE);
					indi++;
				}
			}
			catch (NdbmError &err)
			{
				if (gdbm_error(dbgen.tid[i]) != 0)
				{
					gdbm_clearerr(dbgen.tid[i]);
					sent_back = DbErr_DatabaseAccess;
					return(&sent_back);
				}
			}
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		sent_back = err.get_err_code();
		return(&sent_back);
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in unreg_svc" << std::endl;
		sent_back = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}
//
// Set error code if no device have been found
//
	if (found == False)
		sent_back = DbErr_DeviceServerNotDefined;
//
// Return data
//
	return(&sent_back);
}


/**
 * To return device server info to client
 *
 * @param recev A pointer to a structure where the first element is the device 
 *		server name and the second element is the device server personal name.
 * 
 * @return This function returns a pointer to a structure with all the device
 *    	server information
 */
svcinfo_svc *NdbmServer::svcinfo_1_svc(db_res *recev)
{
	datum 		key;
	long 		indi,
			i,
			j,
			nb_class,
			length,
			dev_length,
			name_length;
	unsigned long 	pid = 0,
			pn = 0;
	std::string 	host,
			process,
			tmp;
	long 		found = False;
	std::vector<std::string> 	class_list;
	std::vector<NdbmSvcDev> 	*dev_list;
	
	std::string 	user_ds_name(recev->res_val.arr1_val[0]);
	std::string 	user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
	std::cout << "In svcinfo_1_svc function for " << user_ds_name 
		<< "/" << user_pers_name << std::endl;
#endif
	
//
// Initialize structure sent back to client
//

	svcinfo_back.db_err = 0;
	
	svcinfo_back.pid = 0;
	svcinfo_back.program_num = 0;
	svcinfo_back.embedded_len = 0;
	svcinfo_back.embedded_val = NULL;
	svcinfo_back.process_name = NULL;
	svcinfo_back.host_name = NULL;
	
	try
	{
		svcinfo_back.process_name = new char [PROC_NAME_LENGTH];
		svcinfo_back.process_name[0] = '\0';
		
		svcinfo_back.host_name = new char [HOST_NAME_LENGTH];
		svcinfo_back.host_name[0] = '\0';
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in svc_info" << std::endl;
		svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
		return(&svcinfo_back);
	}
		
//
// If the server is not connected to the database, returns error
//
	if (dbgen.connected == False)
	{
		svcinfo_back.db_err = DbErr_DatabaseNotConnected;
		return(&svcinfo_back);
	}

//
// First, suppose that the ds_name is a PROCESS name
//

	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesKey k(key);
			
			indi = k.get_dev_indi();
			if (indi != 1)
				continue;
				
			tmp = k.get_ds_pers_name();
			if (tmp != user_pers_name)
				continue;

			k.build_datum();				
			NdbmNamesCont co(dbgen.tid[0],k.get_key());
			tmp = co.get_process_name();
			if (tmp != user_ds_name)
				continue;

			if (found == False)
			{
				process = tmp;
				pid = co.get_pid();
				host = co.get_host_name();
				pn = co.get_p_num();
			}				
			tmp = k.get_ds_name();
			class_list.push_back(tmp);
			
			found = True;
		}
//
// If the user ds name was not a process name, init the class list with it
//
		if (found == False)
			class_list.push_back(user_ds_name);
//
// Allocate vector to store each class device list
//
		nb_class = class_list.size();
		dev_list = new std::vector<NdbmSvcDev> [nb_class];
#if DEBUG
		std::cerr << nb_class << std::endl;
#endif
//
// Get all device for each class in the list
//
		for (i = 0;i < nb_class;i++)
		{
			indi = 1;
			NdbmNamesKey k(class_list[i],user_pers_name,indi);

			try
			{
				while(1)
				{
					if (indi != 1)
						k.upd_indi(indi);
									
					NdbmNamesCont co(dbgen.tid[0],k.get_key());
					NdbmSvcDev tmp_dev(co);
					dev_list[i].push_back(tmp_dev);
					
					if (pid == 0)
					{
						process = co.get_process_name();
						pid = co.get_pid();
						host = co.get_host_name();
						pn = co.get_p_num();
					}
					found = True;
					indi++;
				}
			}
			catch (NdbmError &err)
			{
				if (gdbm_error(dbgen.tid[0]) != 0)
				{
					gdbm_clearerr(dbgen.tid[0]);
					delete [] dev_list;
					svcinfo_back.db_err = DbErr_DatabaseAccess;
					return(&svcinfo_back);
				}
			}
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;
		delete [] dev_list;	
		svcinfo_back.db_err = err.get_err_code();
		return(&svcinfo_back);
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in svc_info" << std::endl;
		delete [] dev_list;
		svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
		return(&svcinfo_back);
	}
	
//
// Set error code if no device have been found
//
	if (found == False)
	{
		delete [] dev_list;
		svcinfo_back.db_err = DbErr_DeviceServerNotDefined;
		return(&svcinfo_back);
	}
				
//
// Initialize structure sent-back to client
//

	strcpy(svcinfo_back.process_name,process.c_str());
	svcinfo_back.pid = pid;
	svcinfo_back.program_num = pn;
	strcpy(svcinfo_back.host_name,host.c_str());

	try
	{
		length = class_list.size();
		svcinfo_back.embedded_val = new svcinfo_server [length];
		for (i = 0;i < length;i++)
		{
			svcinfo_back.embedded_val[i].server_name = NULL;
			svcinfo_back.embedded_val[i].dev_val = NULL;
		}
		svcinfo_back.embedded_len = length;
		for (i = 0; i < length;i++)
		{
			name_length = class_list[i].size();
			svcinfo_back.embedded_val[i].server_name = new char [name_length + 1];
			strcpy(svcinfo_back.embedded_val[i].server_name,class_list[i].c_str());

			dev_length = dev_list[i].size();		
			svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
			for (j = 0;j < dev_length;j++)
				svcinfo_back.embedded_val[i].dev_val[j].name = NULL;
			for (j = 0;j < dev_length;j++)
			{
				svcinfo_back.embedded_val[i].dev_val[j].name = new char [(dev_list[i])[j].get_name_length()];
				strcpy(svcinfo_back.embedded_val[i].dev_val[j].name,(dev_list[i])[j].get_name());
				svcinfo_back.embedded_val[i].dev_val[j].exported_flag = (dev_list[i])[j].get_flag();
			}
			svcinfo_back.embedded_val[i].dev_len = dev_length;
		}
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in svc_info" << std::endl;
		delete [] dev_list;
		svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
		return(&svcinfo_back);
	}	
	
	delete [] dev_list;
	return(&svcinfo_back);
	
}


/**
 * To return delete all the device belonging to a device server and if necessary 
 * also their resources
 * 
 * @param recev A pointer to a structure where the first element is the device server 
 * 		name and the second element is the device server personal name.
 * 
 * @return This function returns a pointer to a structure with all the device 
 *    	server information
 */
long *NdbmServer::svcdelete_1_svc(db_res *recev)
{
	static long 	error_back;
	datum 		key;
	std::string 	tmp,
			device;
	long 		indi,
			i,
			del_res,
			found = False;
	std::vector<std::string> 	ds_name_list;

	del_res = recev->db_err;	
	std::string 	user_ds_name(recev->res_val.arr1_val[0]);
	std::string 	user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
	std::cout << "In svcdelete_1_svc function for " << user_ds_name \
	<< "/" << user_pers_name << std::endl;
#endif
	
//
// Initialize structure sent back to client
//

	error_back = 0;
		
//
// If the server is not connected to the database, returns error
//

	if (dbgen.connected == False)
	{
		error_back = DbErr_DatabaseNotConnected;
		return(&error_back);
	}

//
// First, suppose that the ds_name is a PROCESS name
//

	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesKey k(key);
			
			indi = k.get_dev_indi();
			if (indi != 1)
				continue;
				
			tmp = k.get_ds_pers_name();
			if (tmp != user_pers_name)
				continue;
			
			k.build_datum();	
			NdbmNamesCont co(dbgen.tid[0],k.get_key());
			tmp = co.get_process_name();
			if (tmp != user_ds_name)
				continue;
				
			tmp = k.get_ds_name();
			ds_name_list.push_back(tmp);
			found = True;
		}
//
// If the ds_name was not a process name, init the class list with the user
// ds name
//
		if (found == False)
			ds_name_list.push_back(user_ds_name);

//
// Delete every devices for each ds name in the list
//
			
		for (i = 0;i < ds_name_list.size();i++)
		{
			indi = 1;
			NdbmNamesKey k(ds_name_list[i],user_pers_name,indi);

			try
			{
				while(1)
				{
					if (indi != 1)
						k.upd_indi(indi);			
					NdbmNamesCont co(dbgen.tid[0],k.get_key());
					found = True;
					device = co.get_device_name();
					
					if (del_res == True)
						delete_res(device);
			
					gdbm_delete(dbgen.tid[0],k.get_key());
					indi++;
				}
			}
			catch (NdbmError &err)
			{
				if (gdbm_error(dbgen.tid[i]) != 0)
				{
					gdbm_clearerr(dbgen.tid[i]);
					error_back = DbErr_DatabaseAccess;
					return(&error_back);
				}
			}
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		error_back = err.get_err_code();
		return(&error_back);
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in svcdelete_svc" << std::endl;
		error_back = DbErr_ServerMemoryAllocation;
		return(&error_back);
	}
//
// Set error code if no device have been found
//
	if (found == False)
		error_back = DbErr_DeviceServerNotDefined;
//
// Leave call
//				
	return(&error_back);
}



/**
 * To delete all the resources belonging to a device.
 *
 * @param dev_name The device name
 * 
 * @return This function does not return anything
 */
void  NdbmServer::delete_res(const std::string &dev_name)
{
	datum 		key;
	std::string 	fam,
			memb,
			resource;
	long 		i,
			j,
			ind;
	std::vector<std::string> 	res_list;
	
#ifdef DEBUG
	std::cout << "In delete_res function for device " << dev_name << std::endl;
#endif
	
//
// Extract domain, family and member name from device name
//

	std::string::size_type pos,start;

	if ((pos = dev_name.find(SEP_DEV)) == std::string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	
	std::string user_domain(dev_name,0,pos);
	
	pos++;
	start = pos;
	if ((pos = dev_name.find(SEP_DEV,pos)) == std::string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	
	std::string user_family(dev_name,start,pos - start);

	start = pos + 1;	
	std::string user_member(dev_name,start,dev_name.size() - start);
		
//
// Find the db table for the specified domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == user_domain)
			break;
	}
	if (i == dbgen.TblNum)
		throw NdbmError(DbErr_DomainDefinition,"");
	
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//

	try
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			ind = reskey.get_res_indi();
			if (ind != 1)
				continue;			
			fam = reskey.get_res_fam_name();
			if (fam != user_family)
				continue;
			memb = reskey.get_res_memb_name();
			if (memb != user_member)
				continue;
			resource = reskey.get_res_name();
			
//
// A resource has been founded, add it to the list
//

			res_list.push_back(resource);
		}
	}
	catch (NdbmError &err)
	{
		free(key.dptr);
		gdbm_clearerr(dbgen.tid[i]);		
		throw;
	}
	catch (std::bad_alloc)
	{
		throw;
	}
//
// Delete all resources in the list from db
//
	for (j = 0;j < res_list.size();j++)
	{
		try
		{
			int res;
			ind = 1;
			NdbmResKey k(user_family,user_member,res_list[j],ind);			
			while(1)
			{
				if (ind != 1)
					k.upd_indi(ind); 
	
				res = gdbm_delete(dbgen.tid[i],k.get_key());
				if (res == 0)
					ind++;
				else
				{
					if (ind == 1)
					{
						throw;
					}
					else
					{
						gdbm_clearerr(dbgen.tid[i]);
						break;
					}
				}					
			}
		}
		catch (NdbmError &err)
		{
			if (ind == 1)
			{		
				throw;
			}
			else
			{
				if (gdbm_error(dbgen.tid[i]) != 0)
				{
					gdbm_clearerr(dbgen.tid[i]);
					throw NdbmError(DbErr_DatabaseAccess,"");
				}
			}
		}
	}
}


/**
 * To retrieve poller info from a device name
 * 
 * @param dev The device name
 * 
 * @return This function returns a pointer to an error code
 */
db_poller_svc *NdbmServer::getpoller_1_svc(nam *dev)
{
	datum 		key;
	long 		i;
	std::string 	resource,
			poller_name,
			fam,
			memb,
			device,
			r_val;
	long 		found;
	
#ifdef DEBUG
	std::cout << "In getpoller_1_svc function for device " << *dev << std::endl;
#endif

	std::string user_device(*dev);

//
// Initialize parameter sent back to client
//
	poll_back.db_err = 0;
	poll_back.pid = 0;
	poll_back.server_name = NULL;
	poll_back.personal_name = NULL;
	poll_back.host_name = NULL;
	poll_back.process_name = NULL;
		
	try
	{	
		poll_back.server_name = new char[DS_NAME_LENGTH];
		poll_back.server_name[0] = '\0';
	
		poll_back.personal_name = new char[DSPERS_NAME_LENGTH];
		poll_back.personal_name[0] = '\0';
	
		poll_back.process_name = new char[PROC_NAME_LENGTH];
		poll_back.process_name[0] = '\0';

		poll_back.host_name = new char[HOST_NAME_LENGTH];	
		poll_back.host_name[0] = '\0';
	}
	catch (std::bad_alloc)
	{
		std::cerr << "Memory allocation error in devinfo" << std::endl;
		poll_back.db_err = DbErr_ServerMemoryAllocation;
		return(&poll_back);
	}
		
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		poll_back.db_err = DbErr_DatabaseNotConnected;
		return(&poll_back);
	}
	
//
// Find the db table for the sys domain
//
	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == "sys")
			break;
	}
	if (i == dbgen.TblNum)
	{
		poll_back.db_err = DbErr_DomainDefinition;
		return(&poll_back);
	}

//
// Search for a resource "ud_poll_list" with its value set to caller device name
//
	found = False;
	try
	{
		datum 	key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			resource = reskey.get_res_name();
			if (resource != POLL_RES)
				continue;

			reskey.build_datum();				
			NdbmResCont rescont(dbgen.tid[i],reskey.get_key());
			
			r_val = rescont.get_res_value();
			if (cmp_nocase(r_val,user_device) != 0)
				continue;

//				
// A poller has been found, build its device name
//
			else
			{
				found = True;
				fam = reskey.get_res_fam_name();
				memb = reskey.get_res_memb_name();
			
				poller_name = "sys/" + fam + '/' + memb;
			}			

		}
	}
	catch (NdbmError &err)
	{		
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;
		poll_back.db_err = err.get_err_code();
		return(&poll_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in getpoller_svc" << std::endl;
		poll_back.db_err = DbErr_ServerMemoryAllocation;
		return(&poll_back);
	}
	
//
// Return error if no poller has been found
//
	if (found == False)
	{
		poll_back.db_err = DbErr_NoPollerFound;
		return(&poll_back);
	}
	
//
// get poller device info from the NAMES table
//
	found = False;
	try 
	{
		datum	key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0],key);
		
			device = cont.get_device_name();

			if (device == poller_name)
			{
				cont.get_devinfo(poll_back);
				
				NdbmNamesKey dev_key(key);				
				dev_key.get_devinfo(poll_back);

				found = True;
				free(key.dptr);
				break;
			}		
		}
		if (gdbm_error(dbgen.tid[0]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[0]);
			poll_back.db_err = DbErr_DatabaseAccess;
			return(&poll_back);			
		}
	}
	catch (NdbmError &err)
	{		
		free(key.dptr);
		std::cerr << err.get_err_message() << std::endl;	
		poll_back.db_err = err.get_err_code();
		return(&poll_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in getpoller_svc" << std::endl;
		poll_back.db_err = DbErr_ServerMemoryAllocation;
		return(&poll_back);
	}
	
//
// Return error if the device is not found
//
	if (found == False)
	{
		poll_back.db_err = DbErr_DeviceNotDefined;
		return(&poll_back);
	}
		
//
// Return data
//
	return(&poll_back);
}



/**
 * To compare two string independently of uppercase or lowercase characters
 * 
 * @param s1 The first string
 * @param s2 the second string
 * 
 * @return This function returns 0 if the two strings are identical
 */
long NdbmServer::cmp_nocase(const std::string &s1, const std::string &s2)
{
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();
	
	while (p1 != s1.end() && p2 != s2.end())
	{
		if (::toupper(*p1) != ::toupper(*p2))
			return((::toupper(*p1) < ::toupper(*p2)) ? -1 : 1);
		++p1;
		++p2;
	}
	
	return (s2.size() - s1.size());
}

