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
 * Version:	$Revision: 1.1 $
 *
 * Date:	$Date: 2006-09-27 12:21:35 $
 *
 */

#include <DevErrors.h>
#include <Sqlite3Server.h>
#include <algorithm>


/**
 * retrieve the  device information from the database for a given device
 *
 * @param dev The device name
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_devinfo_svc *SQLite3Server::devinfo_1_svc(nam *dev)
{
	std::string user_device(*dev);
#ifdef DEBUG
	std::cout << "In devinfo_1_svc function for device " << user_device << std::endl;
#endif
//
// Initialize parameter sent back to client and allocate memory for string (bloody XDR)
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
		std::cout << "Memory allocation error in devinfo" << std::endl;
		sent_back.db_err = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		std::cout << "I'm not connected to database" << std::endl;
		sent_back.db_err = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}
//
// Search for device name in the NAMES table
//
	long found = False;

	std::string query;
	query = "SELECT SERVER, HOST, IOR, VERSION, CLASS, PID, EXPORTED FROM device";
	query += " WHERE NAME = '" + user_device + "' AND IOR NOT LIKE 'ior:%'";
	query += " AND CLASS NOT LIKE 'PseudoDevice'";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{			
		sent_back.db_err = DbErr_DatabaseAccess;
		return(&sent_back);			
	}
	else
	{
#ifdef DEBUG
		std::cout << "SQLite3Server::devinfo_1_svc() : " << nrow << " devices found" << std::endl;
#endif
		if(nrow != 0)
		{
			sent_back.device_type = DB_Device;				
			if (result[ncol] != NULL) 
			{
				std::string tmp = result[ncol];
				std::string::size_type pos = tmp.find('/');
				strcpy(sent_back.server_name, tmp.substr(0, pos).c_str());
				strcpy(sent_back.process_name, tmp.substr(0, pos).c_str());
				strcpy(sent_back.personal_name, tmp.substr(pos + 1).c_str());
#ifdef DEBUG
				std::cout << "SQLite3Server::devinfo_1_svc() : " << tmp.substr(0, pos) << " "
					<< tmp.substr(pos + 1) << std::endl;
#endif
			}
			if (result[ncol + 1] != NULL) 
				strcpy(sent_back.host_name, result[ncol + 1]);
			if (result[ncol + 2] != NULL)
			{
				std::string ior(result[ncol + 2]);
				std::string pgm_no;
				pgm_no = ior.substr(ior.rfind(':')+1);
				sent_back.program_num = atoi(pgm_no.c_str());         
			}
			if (result[ncol + 3] != NULL) 
				sent_back.server_version = atoi(result[ncol + 3]);
			if (result[ncol + 4] != NULL) 
				strcpy(sent_back.device_class, result[ncol + 4]);
			if (result[ncol + 5] != NULL) 
				sent_back.pid = atoi(result[ncol + 5]);
			if (result[ncol + 6] != NULL) 
				sent_back.device_exported = atoi(result[ncol + 6]);
		}
		else
		{
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
			sqlite3_free_table(result);
			query = "SELECT HOST, PID FROM device";
			query += (" WHERE NAME = '" + user_device + "'");
			query += (" AND IOR NOT LIKE 'ior:%'");
			query += (" AND CLASS LIKE 'PseudoDevice'");
			if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
			{
				if(nrow != 0)
				{
					sent_back.device_type = DB_Pseudo_Device;
					*sent_back.server_name = '\0';
					*sent_back.personal_name = '\0';
					if (result[ncol] != NULL) 
						strcpy(sent_back.host_name, result[ncol]);
					else
						*sent_back.host_name = '\0';
					if (result[ncol + 1] != NULL) 
						sent_back.pid = atoi(result[ncol + 1]);
					else
						sent_back.pid = 0;
					sent_back.program_num = 0;
					sent_back.server_version = 0;
					*sent_back.device_class = '\0';
					*sent_back.process_name = '\0';
					sent_back.device_exported = False;
				}
//
// Return error if the device is not found
//
				else 
				{
					sqlite3_free_table(result);
					sent_back.db_err = DbErr_DeviceNotDefined;
					return(&sent_back);
				}
			}
			else
			{		
				sent_back.db_err = DbErr_DeviceNotDefined;
				return(&sent_back);
			}
		}
		sqlite3_free_table(result);
	}
//
// Return data
//
	return(&sent_back);
}




/**
 * retrieve all resources belonging to a device
 * 
 * @param recev The device name
 *
 * @return the resource list and an error code which is set if needed
 */
db_res *SQLite3Server::devres_1_svc(db_res *recev)
{
	std::string 	fam,
    			memb,
			resource,
			tmp_res,
			r_name;
	std::string::size_type 	pos;
	
	res_list_dev.clear();

#ifdef DEBUG
	std::cout << "In devres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << std::endl;
	for (long i = 0; i < recev->res_val.arr1_len; i++)
		std::cout << " Device = " << recev->res_val.arr1_val[i] << std::endl;
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
		std::cout << "I'm not connected to database" << std::endl;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

	for (long i = 0; i < recev->res_val.arr1_len; i++)
	{
		std::string in_dev(recev->res_val.arr1_val[i]);
#ifdef DEBUG
		std::cout << " Device = " << in_dev << std::endl;
#endif
		if (count(in_dev.begin(), in_dev.end(), SEP_DEV) != 2)
		{
			browse_back.db_err = DbErr_BadDevSyntax;
			return(&browse_back);
		}
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
		std::string query;

		query = "SELECT DOMAIN, FAMILY, MEMBER, NAME, COUNT, VALUE FROM property_device WHERE ";
		query += (" DEVICE = '" + in_dev);
		query += ( + "' ORDER BY DOMAIN ASC, FAMILY ASC, MEMBER ASC, NAME ASC, COUNT ASC");
#ifdef DEBUG
		std::cout << "QUERY = " << query << std::endl;
#endif
//
// Get resource value for each element in the tmp_res_list list 
//
		if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			std::cout << "ERROR SQLite3Server::devres_1_svc : " << sqlite3_errmsg(db) << std::endl;
			browse_back.db_err = DbErr_DatabaseAccess;
			return (&browse_back);
		}
		else
		{
			std::string resource("");
			int j = ncol;
	    		for (int i = 0; i < nrow; ++i, j += ncol) 
			{
				if (atoi(result[j + 4]) == 1)
				{
					if (!resource.empty())
						res_list_dev.push_back(resource);
		    			resource = std::string(result[j]) + "/" + result[j + 1] + "/" + result[j + 2] + "/" 
						+ result[j + 3] + ": " + result[j + 5];
				}
				else
					resource += ("," + std::string(result[j + 5]));
			}
			if (!resource.empty())
				res_list_dev.push_back(resource);
			sqlite3_free_table(result);
			std::cout << "SQLite3Server::devres_1_svc : " << resource << std::endl;
		}
	}
//
// Sort resource name list
//
	std::sort(res_list_dev.begin(),res_list_dev.end());
// 
// Build the structure returned to caller
//
	long 	res_nb = res_list_dev.size(),
		i;
	try
	{		
		browse_back.res_val.arr1_val = new char * [res_nb];
		for (i = 0;i < res_list_dev.size();i++)
		{
	    		browse_back.res_val.arr1_val[i] = new char [res_list_dev[i].size() + 1];
	    		strcpy(browse_back.res_val.arr1_val[i], res_list_dev[i].c_str());
		}
		browse_back.res_val.arr1_len = res_list_dev.size();
	}
	catch (std::bad_alloc)
	{
		if (browse_back.res_val.arr1_val != NULL)
		{
			for (int j = 0; j < i; j++)
				delete [] browse_back.res_val.arr1_val;
			delete [] browse_back.res_val.arr1_val;
			
			std::cout << "Memory allocation error in devres_svc" << std::endl;
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
 * delete a device from the database
 *
 * @param dev The device name
 *
 * @return a pointer to an error code
 */
DevLong *SQLite3Server::devdel_1_svc(nam *dev)
{
	std::string 	user_device(*dev);
#ifdef DEBUG
	std::cout << "In devdel_1_svc function for device " << user_device << std::endl;
#endif

//
// Initialize parameter sent back to client
//
	errcode = 0;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		std::cout << "I'm not connected to database." << std::endl;
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
	if (count(user_device.begin(), user_device.end(), SEP_DEV) != 2)
	{
		errcode = DbErr_BadDevSyntax;
		return(&errcode);
	}
//
// Search for device name in the NAMES table
//
	std::string query, where;
	query = "SELECT CLASS, SERVER FROM device ",
	where = "WHERE NAME = '" + user_device + "'";   
#ifdef DEBUG
	std::cout << "SQLite3Server::devdel_1_svc() : " << query + where << std::endl;
#endif
	if (sqlite3_get_table(db, (query + where).c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
    	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
    	}

#ifdef DEBUG
	std::cout << "SQLite3Server::devdel_1_svc() : " << nrow << std::endl;
#endif
	if(nrow != 0)
	{
		std::string	ds_name(result[ncol]),
				ds_pers_name(result[ncol + 1]);
		
		sqlite3_free_table(result);
		query = "DELETE FROM device "; 
#ifdef DEBUG
		std::cout << "SQLite3Server::devdel_1_svc() : " << query + where << std::endl;
#endif
		if (sqlite3_get_table(db, (query + where).c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			std::cout << sqlite3_errmsg(db) << std::endl;
			errcode = DbErr_DatabaseAccess;
			return (&errcode);
		}
//
// Update device server device list (decrement all device index in device list
// for all devices above the deleted one)
//
	}
	sqlite3_free_table(result);
//
// Return data
//
	return(&errcode);
}



/**
 * delete all the resources belonging to a device
 * 
 * @param recev The device name
 * 
 * @return a pointer to an error code
 */
db_psdev_error *SQLite3Server::devdelres_1_svc(db_res *recev)
{
#ifdef DEBUG
    std::cout << "In devdelres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << std::endl;
#endif

//
// Initialize parameter sent back to client
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	std::cout << "I'm not connected to database." << std::endl;
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// Build a vector of object from the NdbmDomDev class. Each object in this class
// has a domain name and a list of family/member for this domain
//
    for (long i = 0;i < recev->res_val.arr1_len;i++)
    {
	std::string in_dev(recev->res_val.arr1_val[i]);
		
	if (count(in_dev.begin(), in_dev.end(), SEP_DEV) != 2)
	{
	    psdev_back.error_code = DbErr_BadDevSyntax;
	    psdev_back.psdev_err = i;
	    return(&psdev_back);
	}
	std::string query;
	query = "DELETE FROM property_device WHERE DEVICE = '" + in_dev + "'";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
	    std::cout << sqlite3_errmsg(db) << std::endl;
	    psdev_back.error_code = DbErr_DatabaseAccess;
	    psdev_back.psdev_err = i;
	    return (&psdev_back);
	}
	sqlite3_free_table(result);
    }
//
// leave fucntion
//
    return(&psdev_back);
}

class NameCount	
{
public:
    std::string	name;
    long	count;
    friend bool operator== (const NameCount &a, const NameCount &b){return (a.name == b.name);};
    friend bool operator< (const NameCount &a, const NameCount &b) {return (a.name < b.name);};
};


/**
 * get global information on the database
 * 
 * @return a pointer to a structure with all the database info
 */
db_info_svc *SQLite3Server::info_1_svc()
{
	long 			dev_defined = 0,
				dev_exported = 0,
				psdev_defined = 0,
				res_num = 0,
    				length;
	static std::vector<NameCount>	dom_list,
				res_list;

#ifdef DEBUG
	std::cout << "In info_1_svc function" << std::endl;
#endif 

	dom_list.clear();
	res_list.clear();
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
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		std::cout << "I'm not connected to database." << std::endl;
		info_back.db_err = DbErr_DatabaseNotConnected;
		return(&info_back);
	}
//
// First, count exported devices in the domains
//
	std::string query;
	query = "SELECT DOMAIN, COUNT(*) FROM device WHERE EXPORTED != 0 GROUP BY DOMAIN";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		info_back.db_err = DbErr_DatabaseAccess;
		return (&info_back);
	}
	int j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol) 
	{
		NameCount	nc;
		nc.name = result[j];
		nc.count = atoi(result[j + 1]);
		dev_defined += nc.count;
		dev_exported += nc.count;
		dom_list.push_back(nc);
	}
	sqlite3_free_table(result); 
//
// And now,  count not exported devices in the domains
//
	query = "SELECT DOMAIN, COUNT(*) FROM device WHERE EXPORTED = 0 GROUP BY domain";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		info_back.db_err = DbErr_DatabaseAccess;
		return (&info_back);
	}
	j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
		NameCount	nc;
		nc.name = result[j];
		nc.count = 0;
		dev_defined += atoi(result[j + 1]);
		std::vector<NameCount>::iterator p;
		for (p = dom_list.begin(); p != dom_list.end(); ++p)
			if ((*p).name == nc.name)
				break;
		if (p == dom_list.end())
			dom_list.push_back(nc);
	}
	sqlite3_free_table(result); 
//
// Now, count pseudo_devices
//
	query = "SELECT COUNT(*) FROM device WHERE IOR LIKE 'DC:%'";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{ 
		std::cout << sqlite3_errmsg(db) << std::endl;
		info_back.db_err = DbErr_DatabaseAccess;
		return (&info_back);
    	}
	j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
    		psdev_defined = atoi(result[j]);
	sqlite3_free_table(result);
//
// Then, count resources in each domain
//
	query = "SELECT DOMAIN, COUNT(*) FROM property_device GROUP BY domain";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		info_back.db_err = DbErr_DatabaseAccess;
		return (&info_back);
	}
	j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
		NameCount	nc;
		nc.name = result[j];
		nc.count = atoi(result[j + 1]);
		res_list.push_back(nc);
		res_num += nc.count;
	}
	sqlite3_free_table(result);
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
    for (int i = 0;i < length;i++)
    {
	info_back.dev.dom_val[i].dom_elt = dom_list[i].count;
	info_back.dev.dom_val[i].dom_name = (char *)dom_list[i].name.c_str();
    }
	
    length = res_list.size();
    info_back.res.dom_val = new db_info_dom_svc [length];
    info_back.res.dom_len = length;	
    for (int i = 0;i < length;i++)
    {
	info_back.res.dom_val[i].dom_elt = res_list[i].count;
	info_back.res.dom_val[i].dom_name = (char *)res_list[i].name.c_str();
    }
    return(&info_back);
}



/**
 * unregister a server from the database (to mark all its devices as not exported)
 * 
 * @param recev : A pointer to a structure where the first element is the device server 
 * 	name and the second element is the device server personal name.
 *
 * @return a pointer to a long which will be set in case of error
 */
long *SQLite3Server::unreg_1_svc(db_res *recev)
{
    std::string 		user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
    std::cout << "In unreg_1_svc function for " << user_ds_name << "/" << user_pers_name << std::endl;
#endif
	
//
// Initialize structure sent back to client
//
    errcode = 0;
//
// If the server is not connected to the database, returns error
//
    if (dbgen.connected == False)
    {
	std::cout << "I'm not connected to database." << std::endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    std::string query;
    query = "SELECT DISTINCT CLASS FROM device";
    query += (" WHERE SERVER = '" + user_ds_name + "/" + user_pers_name + "'");
#ifdef DEBUG
	std::cout << " SQLite3Server::unreg_1_svc() : " << query << std::endl;
#endif
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
   		errcode = DbErr_DatabaseAccess;
		return (&errcode);
    	}
	std::string	class_list("");
	int j = 0;
	for (int i = 0; i < nrow; ++i, j += ncol) 
	{
		if (class_list.empty())
			class_list += ("'" + std::string(result[j]) + "'");
		else
			class_list += (",'" + std::string(result[j]) + "'");
	}
	sqlite3_free_table(result);
//
// Unregister every devices for each ds name in the list
//
//
// If the ds_name was not a process name, init the class list with the user ds name
//
	query = "UPDATE device SET EXPORTED = 0 , IOR = 'rpc:not_exp:0',";
	query += " VERSION = 0, PID = 0, CLASS = 'unknown' WHERE CLASS IN (";
	if (class_list.empty())
    	{
		class_list = "'" + user_ds_name + "'";
		query += (class_list + ") AND SERVER LIKE '%" + user_pers_name + "'");
	}
	else
		query += (class_list + ") AND SERVER = '"+ user_ds_name + "/" + user_pers_name + "'");
#ifdef DEBUG
	std::cout << " SQLite3Server::unreg_1_svc() : " << query << std::endl;
#endif
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
	}
//
// Set error code if no device have been found
//
	if (nrow == 0)
		errcode = DbErr_DeviceServerNotDefined;
//
// Return data
//
	return(&errcode);
}



typedef struct
{
    std::string	name;
    int		flag;
}SvcDev;
/**
 * retrieve device server info from the database
 *
 * @param recev A pointer to a structure where the first element is the device server 
 *		name and the second element is the device server personal name.
 *
 * @return a pointer to a structure with all the device server information
 */
svcinfo_svc *SQLite3Server::svcinfo_1_svc(db_res *recev)
{
    long 		indi,
			i,
			j,
			dev_length,
			name_length;
    std::vector<SvcDev> dev_list;
	
    std::string 	user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
	std::cout << "in svcinfo_1_svc function for " << user_ds_name << "/" << user_pers_name << std::endl;
#endif
	
//
// initialize structure sent back to client
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
		std::cout << "memory allocation error in svc_info" << std::endl;
		svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
		return(&svcinfo_back);
	}
//
// if the server is not connected to the database, returns error
//
	if (dbgen.connected == false)
	{
		std::cout << "I'm not connected to database." << std::endl;
		svcinfo_back.db_err = DbErr_DatabaseNotConnected;
		return(&svcinfo_back);
	}
//
// First, suppose that the ds_name is a PROCESS name
//
	std::string query;
	query = "SELECT DISTINCT CLASS FROM device";
	query += (" WHERE server = '" + user_ds_name + "/" + user_pers_name + "'");

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		svcinfo_back.db_err = DbErr_DatabaseAccess;
		return (&svcinfo_back);
	}
    
	std::string 	class_list("");
	int		nb_class = 0;
	int		k = ncol;
	
	for (int i = 0; i < nrow; ++i, k += ncol, ++nb_class)
	{
		if (class_list.empty())
			class_list = "'" + std::string(result[k]) + "'";
		else
			class_list += (", '" + std::string(result[k]) + "'");
	}
	sqlite3_free_table(result);

//
// Get all device for each class in the list
//
//
// If the ds_name was not a process name, init the class list with the user ds name
//
//	query = "SELECT SUBSTRING_INDEX(SERVER,'/',1), NAME, HOST, IOR, EXPORTED, ";
	query = "SELECT SERVER, NAME, HOST, IOR, EXPORTED, PID, SERVER FROM device WHERE ";
	if (nb_class == 0)
	{
		nb_class = 1;
		class_list = "'" + user_ds_name + "'";
		query += ("CLASS IN (" + class_list + ") AND SERVER LIKE '%/" + user_pers_name + "'");
	}
	else
	{
		query += ("SERVER = '"+ user_ds_name + "/" + user_pers_name + "'");
	}
	query += (" AND IOR NOT LIKE 'ior:%' ORDER BY CLASS");
#ifdef DEBUG
	std::cout << "SQLite3Server::svcinfo_1_svc() : query = " << query << std::endl;
#endif
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		svcinfo_back.db_err = DbErr_DatabaseAccess;
		return (&svcinfo_back);
	}
	if (nrow == 0)
	{
//
// Set error code if no device have been found
//
		sqlite3_free_table(result);
		svcinfo_back.db_err = DbErr_DeviceServerNotDefined;
		return(&svcinfo_back);
	} 
//
// Allocate vector to store each class device list
//
//
// Initialize structure sent-back to client
//
	std::string	ds_class("");
	svcinfo_back.embedded_len = nb_class;
	svcinfo_back.embedded_val = new svcinfo_server[nb_class];
	for (long i = 0; i < nb_class; ++i)
	{
	    svcinfo_back.embedded_val[i].server_name = NULL;
	    svcinfo_back.embedded_val[i].dev_val = NULL;
	}
   	if (result[ncol + 2] != NULL) 
		strncpy(svcinfo_back.host_name, result[ncol + 2], HOST_NAME_LENGTH - 1);
	svcinfo_back.host_name[HOST_NAME_LENGTH - 1] = '\0';
	if (result[ncol + 6] != NULL) 
		strncpy(svcinfo_back.process_name, result[ncol + 6], PROC_NAME_LENGTH - 1);
	svcinfo_back.process_name[PROC_NAME_LENGTH - 1] = '\0';
  	if (result[ncol + 5] != NULL) 
		svcinfo_back.pid = atoi(result[ncol + 5]);
	if (result[ncol + 3] != NULL)
	{
                std::string ior(result[ncol + 3]);
                std::string pgm_no;
                pgm_no = ior.substr(ior.rfind(':')+1);
                svcinfo_back.program_num = atoi(pgm_no.c_str());
        }
	i = 0;
        dev_list.clear();
	int l = ncol;
	for (int k = 0; k < nrow; ++k, l += ncol)
        {
	    if (ds_class != result[l])
	    {
	    	dev_length = dev_list.size();
		if (dev_length != 0)
		{
	    	    svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
	    	    for (long j = 0; j < dev_length; j++)
		    {
		    	svcinfo_back.embedded_val[i].dev_val[j].name = NULL;
		    	svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_list[j].name.length() + 1];
		    	strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_list[j].name.c_str());
		    	svcinfo_back.embedded_val[i].dev_val[j].exported_flag = dev_list[j].flag;
		    }
		    svcinfo_back.embedded_val[i].dev_len = dev_length;
		    i++;
		}
		dev_list.clear();

		ds_class = result[l];
	    	name_length = ds_class.length();
	    	svcinfo_back.embedded_val[i].server_name = new char [name_length + 1];
	    	strcpy(svcinfo_back.embedded_val[i].server_name, ds_class.c_str());
	    }
	    SvcDev	dev;
	    dev.name = result[l + 1];
	    if (result[l + 4] != NULL) 
		dev.flag = atoi(result[l + 4]);
	    dev_list.push_back(dev);	    
    	}
	dev_length = dev_list.size();
	svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
	for (long j = 0; j < dev_length; j++)
	{
	    svcinfo_back.embedded_val[i].dev_val[j].name = NULL;
	    svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_list[j].name.length() + 1];
	    strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_list[j].name.c_str());
	    svcinfo_back.embedded_val[i].dev_val[j].exported_flag = dev_list[j].flag;
	}
	svcinfo_back.embedded_val[i].dev_len = dev_length;
	sqlite3_free_table(result);
//
// Leave function
//
	return(&svcinfo_back);
}



/**
 * delete all the device belonging to a device server and if necessary also their resources
 * 
 * @param recev A pointer to a structure where the first element is the device server name and 
 * 	the second element is the device server personal name.
 *
 * @return a pointer to a structure with all the device server information
 */
long *SQLite3Server::svcdelete_1_svc(db_res *recev)
{
    std::string 		user_ds_name(recev->res_val.arr1_val[0]),
			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
    std::cout << "In svcdelete_1_svc function for " << user_ds_name << "/" << user_pers_name << std::endl;
#endif
//
// Get delete resource flag
//
	long del_res = recev->db_err;
	
//
// Initialize structure sent back to client
//
    errcode = 0;
//
// If the server is not connected to the database, returns error
//
    if (dbgen.connected == False)
    {
	std::cout << "I'm not connected to database." << std::endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    std::string query;
    query = "SELECT DISTINCT CLASS FROM device WHERE SERVER = '" + user_ds_name + "/" + user_pers_name + "'";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
	}
	std::string class_list("");
	int j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
		if (class_list.empty())
			class_list = "'" + std::string(result[j]) + "'";
		else
			class_list += (", '" + std::string(result[j]) + "'");
	}
	sqlite3_free_table(result);
//
// If the ds_name was not a process name, init the class list with the user ds name
//
    query = "SELECT NAME FROM device WHERE ";
    if (class_list.empty())
    {
	class_list = "'" + user_ds_name + "'";
	query += ("class IN (" + class_list + ") AND server LIKE '%/" + user_pers_name + "'");
    }
    else
    {
	query += ("server = '"+ user_ds_name + "/" + user_pers_name + "'");
    }
    query += (" AND IOR NOT LIKE 'ior:%'");
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
	}
	std::string	dev_list("");
	j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
		if (dev_list.empty())
			dev_list = "'" + std::string(result[j]) + "'";
		else
			dev_list += ", '" + std::string(result[j]) + "'";
	}
	sqlite3_free_table(result);
	if (dev_list.empty())
	{
		errcode = DbErr_DeviceServerNotDefined;
		return (&errcode);
	}
//
// Delete every resource for all devices in the ds name list
//
	if (del_res == true)
	{
		query = "DELETE FROM property_device WHERE DEVICE IN (" + dev_list + ")";

		if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			std::cout << sqlite3_errmsg(db) << std::endl;
			errcode = DbErr_DatabaseAccess;
			return (&errcode);
		}
		sqlite3_free_table(result);
	}
//
// Delete every devices for each ds name in the list
//
	query = "DELETE FROM device WHERE name IN (" + dev_list + ")";
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
	}
	sqlite3_free_table(result);
//
// Leave call
//				
	return(&errcode);
}


/**
 * retrieve poller info from a device name
 *
 * @param dev The device name
 *
 * @return a pointer to an error code
 */
db_poller_svc *SQLite3Server::getpoller_1_svc(nam *dev)
{
    std::string 	poller_name,
    		user_device(*dev);
#ifdef DEBUG
    std::cout << "In getpoller_1_svc function for device " << user_device << std::endl;
#endif


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
	std::cout << "Memory allocation error in devinfo" << std::endl;
	poll_back.db_err = DbErr_ServerMemoryAllocation;
	return(&poll_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	std::cout << "I'm not connected to database." << std::endl;
	poll_back.db_err = DbErr_DatabaseNotConnected;
	return(&poll_back);
    }
//
// Find the db table for the sys domain
//
    long i;
    for (i = 0;i < dbgen.TblNum;i++)
	if (dbgen.TblName[i] == "sys")		// DOMAIN = 'sys'
	    break;
    if (i == dbgen.TblNum)
    {
	poll_back.db_err = DbErr_DomainDefinition;
	return(&poll_back);
    }
//
// Search for a resource "ud_poll_list" with its value set to caller device name
//
    std::string query;

    query = "SELECT DEVICE FROM property_device WHERE DOMAIN = 'sys' AND";
    query += ("name = '" + std::string(POLL_RES) + "' AND UPPER(value) = UPPER('" + user_device + "')");
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		poll_back.db_err = DbErr_DatabaseAccess;
		return (&poll_back);
    	}
	if (nrow != 0)
	{
//				
// A poller has been found, build its device name
//
		poller_name = result[ncol];
	}			
	else
	{
//
// Return error if no poller has been found
//
		sqlite3_free_table(result);
		poll_back.db_err = DbErr_NoPollerFound;
		return(&poll_back);
	}
	sqlite3_free_table(result);
	
//
// get poller device info from the NAMES table
//
    query = "SELECT DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, HOSTNAME, PROCESS_NAME, PROCESS_ID FROM NAMES";
    query += (" WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + poller_name + "'");
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		poll_back.db_err = DbErr_DatabaseAccess;
		return (&poll_back);
    	}
	if (nrow != 0)
	{
		strcpy(poll_back.server_name, result[ncol]);
		strcpy(poll_back.personal_name, result[ncol + 1]);
		strcpy(poll_back.host_name, result[ncol + 2]);
		strcpy(poll_back.process_name, result[ncol + 3]);
		poll_back.pid = atoi(result[ncol + 4]);
    	}
	else
	{
//
// Return error if the device is not found
//
		sqlite3_free_table(result);
		poll_back.db_err = DbErr_DeviceNotDefined;
		return(&poll_back);			
    	}
//
// Return data
//
	sqlite3_free_table(result);
	return(&poll_back);
}
