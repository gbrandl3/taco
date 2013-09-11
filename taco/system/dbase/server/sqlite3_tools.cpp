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
 * Date:	$Date: 2010-07-07 08:26:02 $
 *
 */

#include <DevErrors.h>
#include <Sqlite3Server.h>
#include <algorithm>


/**
 * add the escape sign '\' for all wildcards characters '_' for the 
 * MySQL database
 *
 * @param input string containing wildcards
 * @return string containing escaped wildcards
 */
std::string SQLite3Server::escape_wildcards(const std::string &input)
{
	std::string tmp(input);
	std::string::size_type pos(0);
	while ((pos = tmp.find('_', pos)) != std::string::npos)
	{
		tmp.insert(pos, 1, '\\');
		pos += 2;
	}
	return tmp;
}


/**
 * retrieve the  device information from the database for a given device
 *
 * @param dev The device name
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_devinfo_svc *SQLite3Server::devinfo_1_svc(DevString *dev)
{
	std::string user_device(*dev);

	logStream->debugStream() << "In devinfo_1_svc function for device " << user_device << log4cpp::eol;

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
		logStream->errorStream() << "Memory allocation error in devinfo" << log4cpp::eol;
		sent_back.db_err = DbErr_ServerMemoryAllocation;
		return(&sent_back);
	}
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database" << log4cpp::eol;
		sent_back.db_err = DbErr_DatabaseNotConnected;
		return(&sent_back);
	}
//
// Search for device name in the NAMES table
//
	long found = False;

	std::string query;
	query = "SELECT SERVER, HOST, IOR, VERSION, CLASS, PID, EXPORTED FROM device";
	query += " WHERE NAME = '" + user_device + "' AND IOR NOT LIKE 'ior:%' ESCAPE '\\'";
	query += " AND CLASS NOT LIKE 'PseudoDevice'";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{			
		sent_back.db_err = DbErr_DatabaseAccess;
		return(&sent_back);			
	}
	else
	{
		logStream->debugStream() << "SQLite3Server::devinfo_1_svc() : " << nrow << " devices found" << log4cpp::eol;
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

				logStream->debugStream() << "SQLite3Server::devinfo_1_svc() : " << tmp.substr(0, pos) << " "
					<< tmp.substr(pos + 1) << log4cpp::eol;

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
			query += (" AND IOR NOT LIKE 'ior:%' ESCAPE '\\'");
			query += (" AND CLASS LIKE 'PseudoDevice'");
			logStream->infoStream() << query << log4cpp::eol;
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

	logStream->debugStream() << "In devres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << log4cpp::eol;
	for (long i = 0; i < recev->res_val.arr1_len; i++)
		logStream->debugStream() << " Device = " << recev->res_val.arr1_val[i] << log4cpp::eol;

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
		logStream->errorStream() << "I'm not connected to database" << log4cpp::eol;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

	for (long i = 0; i < recev->res_val.arr1_len; i++)
	{
		std::string in_dev(recev->res_val.arr1_val[i]);

		logStream->debugStream() << " Device = " << in_dev << log4cpp::eol;

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

		logStream->infoStream() << "QUERY = " << query << log4cpp::eol;
//
// Get resource value for each element in the tmp_res_list list 
//
		if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
		{
			logStream->errorStream() << "ERROR SQLite3Server::devres_1_svc : " << sqlite3_errmsg(db) << log4cpp::eol;
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
			logStream->debugStream() << "SQLite3Server::devres_1_svc : " << resource << log4cpp::eol;
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
			
			logStream->errorStream() << "Memory allocation error in devres_svc" << log4cpp::eol;
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
DevLong *SQLite3Server::devdel_1_svc(DevString *dev)
{
	std::string 	user_device(*dev);

	logStream->debugStream() << "In devdel_1_svc function for device " << user_device << log4cpp::eol;

//
// Initialize parameter sent back to client
//
	errcode = 0;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
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
	logStream->infoStream() << "SQLite3Server::devdel_1_svc() : " << query + where << log4cpp::eol;
	if (sqlite3_get_table(db, (query + where).c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
    	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
    	}
	logStream->debugStream() << "SQLite3Server::devdel_1_svc() : " << nrow << log4cpp::eol;

	if(nrow != 0)
	{
		std::string	ds_name(result[ncol]),
				ds_pers_name(result[ncol + 1]);
		
		sqlite3_free_table(result);
		query = "DELETE FROM device "; 
		logStream->infoStream() << "SQLite3Server::devdel_1_svc() : " << query + where << log4cpp::eol;
		if (sqlite3_exec(db, (query + where).c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
		{
			logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
			errcode = DbErr_DatabaseAccess;
			return (&errcode);
		}
//
// Update device server device list (decrement all device index in device list
// for all devices above the deleted one)
//
	}
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
	logStream->debugStream() << "In devdelres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << log4cpp::eol;
//
// If the server is not connected to the database, return error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		psdev_back.error_code = DbErr_DatabaseNotConnected;
		return(&psdev_back);
	}
//
// Initialize parameter sent back to client
//
	psdev_back.error_code = 0;
	psdev_back.psdev_err = 0;
//
// Build a vector of object from the NdbmDomDev class. Each object in this class
// has a domain name and a list of family/member for this domain
//
	std::string query = "BEGIN TRANSACTION";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << __LINE__ << sqlite3_errmsg(db) << log4cpp::eol;
		logStream->errorStream() << query.c_str() << log4cpp::eol;
		psdev_back.error_code = DbErr_DatabaseAccess;
		return (&psdev_back);
	}
	for (long i = 0;i < recev->res_val.arr1_len;i++)
	{
		std::string in_dev(recev->res_val.arr1_val[i]);
		
		if (count(in_dev.begin(), in_dev.end(), SEP_DEV) != 2)
		{
			psdev_back.error_code = DbErr_BadDevSyntax;
			psdev_back.psdev_err = i;
			query = "ROLLBACK TRANSACTION";
			logStream->infoStream() << query << log4cpp::eol;
			sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg);
			return(&psdev_back);
		}
		std::string query;
		query = "DELETE FROM property_device WHERE DEVICE = '" + in_dev + "'";
		logStream->infoStream() << query << log4cpp::eol;
		if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
		{
			logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
			psdev_back.error_code = DbErr_DatabaseAccess;
			query = "ROLLBACK TRANSACTION";
			logStream->infoStream() << query << log4cpp::eol;
			sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg);
			psdev_back.psdev_err = i;
			return (&psdev_back);
		}
		logStream->infoStream() << query << log4cpp::eol;
	}
	query = "COMMIT TRANSACTION";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << __LINE__ << sqlite3_errmsg(db) << log4cpp::eol;
		logStream->errorStream() << query.c_str() << log4cpp::eol;
		psdev_back.error_code = DbErr_DatabaseAccess;
		return (&psdev_back);
	}
	logStream->infoStream() << query << log4cpp::eol;
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

	logStream->debugStream() << "In info_1_svc function" << log4cpp::eol;

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
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		info_back.db_err = DbErr_DatabaseNotConnected;
		return(&info_back);
	}
//
// First, count exported devices in the domains
//
	std::string query;
	query = "SELECT DOMAIN, COUNT(*) FROM device WHERE EXPORTED != 0 GROUP BY DOMAIN";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
	query = "SELECT COUNT(*) FROM device WHERE IOR LIKE 'DC:%' ESCAPE '\\'";
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{ 
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
DevLong *SQLite3Server::unreg_1_svc(db_res *recev)
{
	std::string 	user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);
		
	logStream->debugStream() << "In unreg_1_svc function for " << user_ds_name << "/" << user_pers_name << log4cpp::eol;
//
// If the server is not connected to the database, returns error
//
	if (dbgen.connected == False)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Initialize structure sent back to client
//
	errcode = 0;
//
// First, suppose that the ds_name is a PROCESS name
//
	std::string query;
	query = "SELECT DISTINCT CLASS FROM device";
	query += (" WHERE SERVER = '" + user_ds_name + "/" + user_pers_name + "'");
	logStream->infoStream() << " SQLite3Server::unreg_1_svc() : " << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
// If the ds_name was not a process name, init the class list with the user ds name
//
	// do no destroy useful information about device server
//	query = "UPDATE device SET EXPORTED = 0 , IOR = 'rpc:not_exp:0',";
//	query += " VERSION = 0, PID = 0, CLASS = 'unknown' WHERE CLASS IN (";
	query = "UPDATE device SET EXPORTED = 0 WHERE CLASS IN (";

	if (class_list.empty())
    	{
		class_list = "'" + user_ds_name + "'";
		query += (class_list + ") AND SERVER LIKE '%/" + escape_wildcards(user_pers_name) + "' ESCAPE '\\'");
	}
	else
		query += (class_list + ") AND SERVER = '"+ user_ds_name + "/" + user_pers_name + "'");
	logStream->infoStream() << " SQLite3Server::unreg_1_svc() : " << query << log4cpp::eol;

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
	long 		dev_length,
			name_length;

	std::string 	user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]),
			query;

	logStream->debugStream() << "in svcinfo_1_svc function for " << user_ds_name << "/" << user_pers_name << log4cpp::eol;
	
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
		strcpy(svcinfo_back.process_name , "unknown");
		svcinfo_back.host_name = new char [HOST_NAME_LENGTH];
		strcpy(svcinfo_back.host_name, "not_exp");
    	}
	catch (std::bad_alloc)
	{
		logStream->errorStream() << "memory allocation error in svc_info" << log4cpp::eol;
		svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
		return(&svcinfo_back);
	}
//
// if the server is not connected to the database, returns error
//
	if (dbgen.connected == false)
	{
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		svcinfo_back.db_err = DbErr_DatabaseNotConnected;
		return(&svcinfo_back);
	}
//
// Get server informations
//
	query = "SELECT PID, HOST, NAME, IOR, EXPORTED FROM device WHERE server='"
		+ user_ds_name + "/" + user_pers_name + "' AND IOR NOT LIKE 'ior:%' ESCAPE '\\'";


	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		svcinfo_back.db_err = DbErr_DatabaseAccess;
		return (&svcinfo_back);
	}
    
	std::string 	class_list("");
	int		nb_class = 0;

	dev_length = nrow;

	if (dev_length == 0)
	{
		sqlite3_free_table(result);
		svcinfo_back.db_err = DbErr_DeviceServerNotDefined;
 		return(&svcinfo_back);
	}

//
// First row contains the column names
//
	int		k = ncol;

	svcinfo_back.pid = atoi(result[k]);
	std::string ior(result[k + 3]);
	std::string pgm_no = ior.substr(ior.rfind(':') + 1);
	svcinfo_back.program_num = atoi(pgm_no.c_str());
	if(result[k + 1] != NULL) 
	{
		strncpy(svcinfo_back.host_name, result[k + 1], HOST_NAME_LENGTH - 1);
		svcinfo_back.host_name[HOST_NAME_LENGTH - 1] = '\0';
	}

	if (result[k + 1] == NULL || svcinfo_back.pid == 0)
	{
//
// We have here a server that has not been started
// Do not search for embedded server
//
		svcinfo_back.embedded_len = 1;
		svcinfo_back.embedded_val = new svcinfo_server[1];
		svcinfo_back.embedded_val[0].server_name = new char [user_ds_name.length() + 1];
		strcpy(svcinfo_back.embedded_val[0].server_name, user_ds_name.c_str());
		svcinfo_back.embedded_val[0].dev_len = dev_length;
		svcinfo_back.embedded_val[0].dev_val = new svcinfo_dev [dev_length];
		for(int i = 0; i < dev_length; i++, k += ncol) 
		{
			std::string dev_name(result[k + 2]);
			svcinfo_back.embedded_val[0].dev_val[i].name = new char [dev_name.length() + 1];
			strcpy(svcinfo_back.embedded_val[0].dev_val[i].name, dev_name.c_str());
			svcinfo_back.embedded_val[0].dev_val[i].exported_flag = atoi(result[k + 4]);
		}
		sqlite3_free_table(result);
//
// Leave function
//
		return(&svcinfo_back);
	}
//
// Search for embedded server
//
// As we don't have the process name column within the 'tango' database scheme,
// we will use the couple host,pid instead. Note that with TACO the server
// name and the process name can be different, this happens when you have
// several servers embedded whithin a process.
// Note that we CAN'T use the class column as the class name can be different
// from the server name.
//
	query = "SELECT DISTINCT SERVER FROM DEVICE WHERE HOST ='";
	query += std::string(svcinfo_back.host_name) + "' AND SERVER LIKE '%/" + user_pers_name + "' ESCAPE '\\' AND PID = ";
	char pid_str[32];
	snprintf(pid_str, sizeof(pid_str), "%d", svcinfo_back.pid);
	query += pid_str;
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		svcinfo_back.db_err = DbErr_DatabaseAccess;
		return (&svcinfo_back);
	}

	name_length = ncol;

	svcinfo_back.embedded_len = name_length;
	svcinfo_back.embedded_val = new svcinfo_server[name_length];
	k = ncol;
	for(int i = 0; i < name_length; i++, k += ncol) 
	{
		std::string full_srv_name(result[k]);
		std::string::size_type pos = full_srv_name.find('/');
		std::string srv_name = full_srv_name.substr(0,pos);
		svcinfo_back.embedded_val[i].server_name = new char [srv_name.length() + 1];
		strcpy(svcinfo_back.embedded_val[i].server_name, srv_name.c_str());

		if(name_length == 1) 
		{
//
// When there is only one server within a process, we can say
// that the server name is also the process name.
//
			strncpy(svcinfo_back.process_name, srv_name.c_str(), PROC_NAME_LENGTH - 1);
			svcinfo_back.process_name[PROC_NAME_LENGTH - 1] = '\0';
		}
//
// Retrieve the device list
//
		char 	**result2;
		char 	*zErrMsg2;
		int	nrow2,
			ncol2;

		query = "SELECT NAME, EXPORTED FROM DEVICE WHERE SERVER ='" + full_srv_name + "'";
		logStream->infoStream() << query << log4cpp::eol;
		if (sqlite3_get_table(db, query.c_str(), &result2, &nrow2, &ncol2, &zErrMsg2) != SQLITE_OK)
		{
			logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
			sqlite3_free_table(result);
			svcinfo_back.db_err = DbErr_DatabaseAccess;
			return (&svcinfo_back);
		}
		dev_length = nrow2;
		svcinfo_back.embedded_val[i].dev_len = dev_length;
		svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];

		long	l = ncol2;
		for(int j = 0; j < dev_length; ++j, l += ncol2) 
		{
		        std::string dev_name(result2[l]);
			svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_name.length()+1];
			strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_name.c_str());
			svcinfo_back.embedded_val[i].dev_val[j].exported_flag = atoi(result2[l + 1]);
		}
		sqlite3_free_table(result2);
	}
//
// Leave function
//
	sqlite3_free_table(result);
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
DevLong *SQLite3Server::svcdelete_1_svc(db_res *recev)
{
	std::string 	user_ds_name(recev->res_val.arr1_val[0]),
			user_pers_name(recev->res_val.arr1_val[1]);
		
	logStream->debugStream() << "In svcdelete_1_svc function for " << user_ds_name << "/" << user_pers_name << log4cpp::eol;
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
		logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Delete only the specified server (not the process) as we don't have the
// process name column whithin the 'tango' scheme.
// Note that we CAN'T use the class column as the class name can be different
// from the server name.
//
	std::string query;
	query = "SELECT NAME FROM DEVICE WHERE SERVER = '"
		+ user_ds_name + "/" + user_pers_name + "' AND IOR NOT LIKE 'ior:%' ESCAPE '\\'";
	logStream->infoStream() << query << log4cpp::eol;	
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		errcode = DbErr_DatabaseAccess;
		return (&errcode);
	}
	int dev_length = nrow;
	if (dev_length == 0)
	{
		sqlite3_free_table(result);
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		errcode = DbErr_DeviceServerNotDefined;
		return (&errcode);
	}

	int k = ncol;
	char	**result2,
		*zErrMsg2;
	int	ncol2,
		nrow2;
	for(int i = 0; i < dev_length; ++i) 
	{
		std::string dev_name(result[k + i]);
		if (del_res == true) 
		{
			query = "DELETE FROM PROPERTY_DEVICE WHERE DEVICE = '" + dev_name + "'";
			logStream->infoStream() << query << log4cpp::eol;
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg2) != SQLITE_OK)
			{
				logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
				errcode = DbErr_DatabaseAccess;
				return (&errcode);
			}
			logStream->infoStream() << query << log4cpp::eol;
		}

		query = "DELETE FROM DEVICE WHERE NAME = '" + dev_name + "'";
		logStream->infoStream() << query << log4cpp::eol;
		if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg2) != SQLITE_OK)
		{
			logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
			errcode = DbErr_DatabaseAccess;
			return (&errcode);
		}
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
db_poller_svc *SQLite3Server::getpoller_1_svc(DevString *dev)
{
    std::string 	poller_name,
    		user_device(*dev);

    logStream->debugStream() << "In getpoller_1_svc function for device " << user_device << log4cpp::eol;

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
	logStream->errorStream() << "Memory allocation error in devinfo" << log4cpp::eol;
	poll_back.db_err = DbErr_ServerMemoryAllocation;
	return(&poll_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	logStream->errorStream() << "I'm not connected to database." << log4cpp::eol;
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
	query += (" name = '" + std::string(POLL_RES) + "' AND UPPER(value) = UPPER('" + user_device + "')");
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
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
	query = "SELECT SERVER, HOST, PID FROM device";
	query += (" WHERE NAME = '" + poller_name + "'");
	logStream->infoStream() << query << log4cpp::eol;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		poll_back.db_err = DbErr_DatabaseAccess;
		return (&poll_back);
    	}
	if (nrow != 0)
	{
		std::string full_srv_name(result[ncol]);
		std::string::size_type pos = full_srv_name.find('/');
		std::string srv_name = full_srv_name.substr(0, pos);
		std::string pers_name = full_srv_name.substr(pos + 1);

		strcpy(poll_back.server_name, srv_name.c_str());
		strcpy(poll_back.personal_name, pers_name.c_str());
		strcpy(poll_back.host_name, result[ncol + 1]);
		strcpy(poll_back.process_name, srv_name.c_str()); // Here process = server name
		poll_back.pid = atoi(result[ncol + 2]);

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
