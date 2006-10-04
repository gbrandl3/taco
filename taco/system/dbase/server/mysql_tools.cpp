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
 * Version:	$Revision: 1.18 $
 *
 * Date:	$Date: 2006-10-04 19:24:43 $
 *
 */

#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>


/**
 * retrieve the  device information from the database for a given device
 *
 * @param dev The device name
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_devinfo_svc *MySQLServer::devinfo_1_svc(nam *dev)
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
	std::cerr << "Memory allocation error in devinfo" << std::endl;
	sent_back.db_err = DbErr_ServerMemoryAllocation;
	return(&sent_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	std::cerr << "I'm not connected to database" << std::endl;
	sent_back.db_err = DbErr_DatabaseNotConnected;
	return(&sent_back);
    }
//
// Search for device name in the NAMES table
//
    long found = False;

    std::string query;
    query = "SELECT SUBSTRING_INDEX(SERVER,'/',1), SUBSTRING_INDEX(SERVER,'/',-1), HOST, IOR,";
    query += ("VERSION, CLASS, PID, SUBSTRING_INDEX(SERVER,'/',1), EXPORTED FROM device");
    query += (" WHERE NAME = '" + user_device + "'");
    query += (" AND IOR NOT LIKE 'ior:%'");
    query += (" AND CLASS NOT LIKE 'PseudoDevice'");
    if (mysql_query(mysql_conn, query.c_str()) == 0)
    {
	MYSQL_RES *result = mysql_store_result(mysql_conn);
	MYSQL_ROW row;
	if((row = mysql_fetch_row(result)) != NULL)
	{
	    sent_back.device_type = DB_Device;				
	    if (row[0] != NULL) strcpy(sent_back.server_name, row[0]);
	    if (row[1] != NULL) strcpy(sent_back.personal_name, row[1]);
	    if (row[2] != NULL) strcpy(sent_back.host_name, row[2]);
	    if (row[3] != NULL)
	    {
                    std::string ior(row[3]);
                    std::string pgm_no;
		    pgm_no = ior.substr(ior.rfind(':')+1);
                    sent_back.program_num = atoi(pgm_no.c_str());         
	    }
	    if (row[4] != NULL) sent_back.server_version = atoi(row[4]);
	    if (row[5] != NULL) strcpy(sent_back.device_class, row[5]);
	    if (row[6] != NULL) sent_back.pid = atoi(row[6]);
	    if (row[7] != NULL) strcpy(sent_back.process_name, row[7]);
	    if (row[8] != NULL) sent_back.device_exported = atoi(row[8]);
	}
	else
	{
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
		mysql_free_result(result);
		query = "SELECT HOST, PID FROM device";
		query += (" WHERE NAME = '" + user_device + "'");
		query += (" AND IOR NOT LIKE 'ior:%'");
                query += (" AND CLASS LIKE 'PseudoDevice'");
    	        if (mysql_query(mysql_conn, query.c_str()) == 0)
    	        {
		    result = mysql_store_result(mysql_conn);
		    if((row = mysql_fetch_row(result)) != NULL)
		    {

    		        sent_back.device_type = DB_Pseudo_Device;
	    	        *sent_back.server_name = '\0';
	    	        *sent_back.personal_name = '\0';
			if (row[0] != NULL) 
				strcpy(sent_back.host_name, row[0]);
			else
	    	        	*sent_back.host_name = '\0';
			if (row[1] != NULL) 
				sent_back.pid = atoi(row[1]);
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
	mysql_free_result(result);
    }
    else
    {			
	sent_back.db_err = DbErr_DatabaseAccess;
	return(&sent_back);			
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
db_res *MySQLServer::devres_1_svc(db_res *recev)
{
    std::string 		fam,
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
	std::cerr << "I'm not connected to database" << std::endl;
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
//
// Get resource value for each element in the tmp_res_list list 
//
	if (mysql_query(mysql_conn, query.c_str()) == 0)
	{
	    MYSQL_RES *result = mysql_store_result(mysql_conn);
	    MYSQL_ROW row;

	    std::string resource("");
	    while ((row = mysql_fetch_row(result)) != NULL)
	    {
		if (atoi(row[4]) == 1)
		{
		    if (!resource.empty())
		   	res_list_dev.push_back(resource);
		    resource = std::string(row[0]) + "/" + row[1] + "/" + row[2] + "/" + row[3] + ": " + row[5];
		}
		else
		    resource += ("," + std::string(row[5]));
	    }
	    if (!resource.empty())
		res_list_dev.push_back(resource);
	    mysql_free_result(result);
	}
	else
	{
	    std::cerr << mysql_error(mysql_conn) << std::endl;
	    browse_back.db_err = DbErr_DatabaseAccess;
	    return (&browse_back);
	}
    }
//
// Sort resource name list
//
    std::sort(res_list_dev.begin(),res_list_dev.end());
// 
// Build the structure returned to caller
//
    long res_nb = res_list_dev.size(),
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
 * delete a device from the database
 *
 * @param dev The device name
 *
 * @return a pointer to an error code
 */
DevLong *MySQLServer::devdel_1_svc(nam *dev)
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
	std::cerr << "I'm not connected to database." << std::endl;
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
    where = "WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + user_device + "'";   
    
    if (mysql_query(mysql_conn, (query + where).c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row = mysql_fetch_row(result);

    if(row != NULL)
    {
    	std::string	ds_name(row[0]),
		ds_pers_name(row[1]),
    		ind(row[2]);
		
    	mysql_free_result(result);
    	query = "DELETE FROM device "; 
    	if (mysql_query(mysql_conn, (query + where).c_str()) != 0)
    	{
	    std::cerr << mysql_error(mysql_conn) << std::endl;
	    errcode = DbErr_DatabaseAccess;
	    return (&errcode);
    	}
//
// Update device server device list (decrement all device index in device list
// for all devices above the deleted one)
//
    }
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
    else
    {
    	mysql_free_result(result);
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
db_psdev_error *MySQLServer::devdelres_1_svc(db_res *recev)
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
	std::cerr << "I'm not connected to database." << std::endl;
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
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
	    std::cerr << mysql_error(mysql_conn) << std::endl;
	    psdev_back.error_code = DbErr_DatabaseAccess;
	    psdev_back.psdev_err = i;
	    return (&psdev_back);
	}
	if (mysql_affected_rows(mysql_conn) == 0)
	{
	    psdev_back.error_code = DbErr_DeviceNotDefined;
	    psdev_back.psdev_err = i;
	    return(&psdev_back);
	}
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
db_info_svc *MySQLServer::info_1_svc()
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
	std::cerr << "I'm not connected to database." << std::endl;
	info_back.db_err = DbErr_DatabaseNotConnected;
	return(&info_back);
    }
//
// First, count exported devices in the domains
//
    std::string query;
    query = "SELECT DOMAIN, COUNT(*) FROM device WHERE EXPORTED != 0 GROUP BY DOMAIN";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = atoi(row[1]);
	dev_defined += nc.count;
	dev_exported += nc.count;
	dom_list.push_back(nc);
    }
    mysql_free_result(result); 
//
// And now,  count not exported devices in the domains
//
    query = "SELECT DOMAIN, COUNT(*) FROM device WHERE EXPORTED = 0 GROUP BY domain";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = 0;
	dev_defined += atoi(row[1]);
	std::vector<NameCount>::iterator p;
	for (p = dom_list.begin(); p != dom_list.end(); ++p)
	    if ((*p).name == nc.name)
		break;
	if (p == dom_list.end())
	    dom_list.push_back(nc);
//	else
//	    (*p).count += nc.count;
    }
    mysql_free_result(result); 
//
// Now, count pseudo_devices
//
    query = "SELECT COUNT(*) FROM device WHERE IOR LIKE 'DC:%'";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    { 
	std::cerr << mysql_error(mysql_conn) << std::endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    if ((row = mysql_fetch_row(result)) != NULL)
    	psdev_defined = atoi(row[0]);
    mysql_free_result(result);
//
// Then, count resources in each domain
//
    query = "SELECT DOMAIN, COUNT(*) FROM property_device GROUP BY domain";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = atoi(row[1]);
	res_list.push_back(nc);
	res_num += nc.count;
    }
    mysql_free_result(result);
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
long *MySQLServer::unreg_1_svc(db_res *recev)
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
	std::cerr << "I'm not connected to database." << std::endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    std::string query;
    query = "SELECT DISTINCT CLASS FROM device";
    query += (" WHERE SERVER = '" + user_ds_name + "/" + user_pers_name + "'");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
   	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    std::string	class_list("");
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	if (class_list.empty())
	    class_list += ("'" + std::string(row[0]) + "'");
	else
	    class_list += (",'" + std::string(row[0]) + "'");
    }
    mysql_free_result(result);
//
// Unregister every devices for each ds name in the list
//
//
// If the ds_name was not a process name, init the class list with the user ds name
//
    if (class_list.empty())
    {
	class_list = "'" + user_ds_name + "'";
	query = "UPDATE device SET EXPORTED = 0 , IOR = 'rpc:not_exp:0',";
	query += " VERSION = 0, CLASS = 'unknown' WHERE CLASS IN (";
	query += (class_list + ") AND SERVER LIKE '%" + user_pers_name + "'");
    }
    else
    {
	query = "UPDATE device SET EXPORTED = 0 , IOR = 'rpc:not_exp:0',";
	query += " VERSION = 0, CLASS = 'unknown' WHERE class IN (";
	query += (class_list + ") AND SERVER = '"+ user_ds_name + "/" + user_pers_name + "'");
    }

    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
//
// Set error code if no device have been found
//
    if (mysql_affected_rows(mysql_conn) == 0)
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
svcinfo_svc *MySQLServer::svcinfo_1_svc(db_res *recev)
{
    long 		i,
			j,
			dev_length,
			name_length;
	
    std::string 	user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);

    std::string query;
    MYSQL_RES *result,*result2;
    MYSQL_ROW row,row2;
		
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
	strcpy(svcinfo_back.process_name , "unknown");
	svcinfo_back.host_name = new char [HOST_NAME_LENGTH];
	strcpy(svcinfo_back.host_name, "not_exp");
    }
    catch (std::bad_alloc)
    {
	std::cerr << "memory allocation error in svc_info" << std::endl;
	svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
	return(&svcinfo_back);
    }
//
// if the server is not connected to the database, returns error
//
    if (dbgen.connected == false)
    {
	std::cerr << "I'm not connected to database." << std::endl;
	svcinfo_back.db_err = DbErr_DatabaseNotConnected;
	return(&svcinfo_back);
    }
    
//
// Get server informations
//
    
    query = "SELECT pid,host,name,ior,exported FROM device WHERE server='" 
            + user_ds_name + "/" + user_pers_name + "' AND ior NOT LIKE 'ior:%'";
	    
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	svcinfo_back.db_err = DbErr_DatabaseAccess;
	return (&svcinfo_back);
    }
    
    result = mysql_store_result(mysql_conn);

    dev_length = mysql_num_rows(result);
    
    if (dev_length == 0)
    {
	mysql_free_result(result);
	svcinfo_back.db_err = DbErr_DeviceServerNotDefined;
	return(&svcinfo_back);
    }
    
    row = mysql_fetch_row(result);
    
    svcinfo_back.pid = atoi(row[0]);
    std::string ior(row[3]);
    std::string pgm_no;
    pgm_no = ior.substr(ior.rfind(':')+1);
    svcinfo_back.program_num = atoi(pgm_no.c_str());
    if(row[1]!=NULL) {
      strncpy(svcinfo_back.host_name, row[1], HOST_NAME_LENGTH - 1);
      svcinfo_back.host_name[HOST_NAME_LENGTH - 1] = '\0';
    }
    
    if( row[1]==NULL || svcinfo_back.pid == 0 ) {
    
       // We have here a server that has not been started
       // Do not search for embedded server
              
       svcinfo_back.embedded_len = 1;
       svcinfo_back.embedded_val = new svcinfo_server[1];
       svcinfo_back.embedded_val[0].server_name = new char [user_ds_name.length() + 1];
       strcpy(svcinfo_back.embedded_val[0].server_name,user_ds_name.c_str());
       svcinfo_back.embedded_val[0].dev_len = dev_length;
       svcinfo_back.embedded_val[0].dev_val = new svcinfo_dev [dev_length];
       for(i=0;i<dev_length;i++) {
         std::string dev_name(row[2]);
         svcinfo_back.embedded_val[0].dev_val[i].name = new char [dev_name.length()+1];
         strcpy(svcinfo_back.embedded_val[0].dev_val[i].name, dev_name.c_str());
         svcinfo_back.embedded_val[0].dev_val[i].exported_flag = atoi(row[4]);        
	 row = mysql_fetch_row(result);
       }
       
       mysql_free_result(result);
       return(&svcinfo_back);
           
    }
    
    mysql_free_result(result);
    
//
// Search for embedded server
//
// As we don't have the process name column within the 'tango' database scheme,
// we will use the couple host,pid instead. Note that with TACO the server
// name and the process name can be different, this happens when you have 
// several servers embedded whithin a process.
// Note that we CAN'T use the class column as the class name can be different
// from the server name.

    query = "SELECT DISTINCT server FROM device WHERE host='";
    query += svcinfo_back.host_name;
    query += "' and server like '%/" + user_pers_name + "' and pid=";
    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d", svcinfo_back.pid);
    query += pid_str;
	    
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	svcinfo_back.db_err = DbErr_DatabaseAccess;
	return (&svcinfo_back);
    }
    
    result = mysql_store_result(mysql_conn);
    
    name_length = mysql_num_rows(result);
       
    svcinfo_back.embedded_len = name_length;
    svcinfo_back.embedded_val = new svcinfo_server[name_length];
    for(i=0;i<name_length;i++) {
    
      row = mysql_fetch_row(result);
      std::string full_srv_name(row[0]);
      std::string::size_type pos = full_srv_name.find('/');
      std::string srv_name = full_srv_name.substr(0,pos);
      svcinfo_back.embedded_val[i].server_name = new char [srv_name.length() + 1];
      strcpy(svcinfo_back.embedded_val[i].server_name,srv_name.c_str());
      
      if(name_length==1) {
        // When there is only one server within a process, we can say
	// that the server name is also the process name.
        strncpy(svcinfo_back.process_name, srv_name.c_str(), PROC_NAME_LENGTH - 1);
        svcinfo_back.process_name[PROC_NAME_LENGTH - 1] = '\0';        
      }
      
      // Retrieve the device list
      query = "SELECT name,exported FROM device WHERE server='" + full_srv_name + "'";
	    
      if (mysql_query(mysql_conn, query.c_str()) != 0)
      {
        mysql_free_result(result);
	std::cerr << mysql_error(mysql_conn) << std::endl;
	svcinfo_back.db_err = DbErr_DatabaseAccess;
	return (&svcinfo_back);
      }
      
      result2 = mysql_store_result(mysql_conn);
      
      dev_length = mysql_num_rows(result2);
      svcinfo_back.embedded_val[i].dev_len = dev_length;
      svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
      
      for(j=0;j<dev_length;j++) {
        row2 = mysql_fetch_row(result2);
        std::string dev_name(row2[0]);
        svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_name.length()+1];
        strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_name.c_str());
        svcinfo_back.embedded_val[i].dev_val[j].exported_flag = atoi(row2[1]);        
      }
      
      mysql_free_result(result2);      
                  
    }
        
    mysql_free_result(result);      
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
long *MySQLServer::svcdelete_1_svc(db_res *recev)
{
    long 		i,
			dev_length;
	
    std::string 	user_ds_name(recev->res_val.arr1_val[0]),
			user_pers_name(recev->res_val.arr1_val[1]);
		
    std::string query;
    MYSQL_RES *result;
    MYSQL_ROW row;
    
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
	std::cerr << "I'm not connected to database." << std::endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
    
//
// Delete only the specified server (not the process) as we don't have the 
// process name column whithin the 'tango' scheme.
// Note that we CAN'T use the class column as the class name can be different
// from the server name.
//
    
    query = "SELECT name FROM device WHERE server='" 
            + user_ds_name + "/" + user_pers_name + "' AND ior NOT LIKE 'ior:%'";
	    
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    
    result = mysql_store_result(mysql_conn);

    dev_length = mysql_num_rows(result);
    
    if (dev_length == 0)
    {
	mysql_free_result(result);
	errcode = DbErr_DeviceServerNotDefined;
	return (&errcode);
    }
    
                      
    for(i=0;i<dev_length;i++) {
       
      row = mysql_fetch_row(result);
    
      std::string dev_name(row[0]);
         
      if (del_res == true) {
	 
        query = "DELETE FROM property_device WHERE device='" + dev_name + "'";
        if (mysql_query(mysql_conn, query.c_str()) != 0)
        {
          mysql_free_result(result);
          std::cerr << mysql_error(mysql_conn) << std::endl;
          errcode = DbErr_DatabaseAccess;
          return (&errcode);
        }
	   
      }
	 
      query = "DELETE FROM device WHERE name='" + dev_name + "'";
      if (mysql_query(mysql_conn, query.c_str()) != 0)
      {
        mysql_free_result(result);
        std::cerr << mysql_error(mysql_conn) << std::endl;
        errcode = DbErr_DatabaseAccess;
        return (&errcode);
      }
	 
	 
    }
       
    mysql_free_result(result);
    return(&errcode);
           
}


/**
 * retrieve poller info from a device name
 *
 * @param dev The device name
 *
 * @return a pointer to an error code
 */
db_poller_svc *MySQLServer::getpoller_1_svc(nam *dev)
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
	std::cerr << "Memory allocation error in devinfo" << std::endl;
	poll_back.db_err = DbErr_ServerMemoryAllocation;
	return(&poll_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	std::cerr << "I'm not connected to database." << std::endl;
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
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	poll_back.db_err = DbErr_DatabaseAccess;
	return (&poll_back);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
    {
//				
// A poller has been found, build its device name
//
	poller_name = row[0];
    }			
    else
    {
//
// Return error if no poller has been found
//
	mysql_free_result(result);
	poll_back.db_err = DbErr_NoPollerFound;
	return(&poll_back);
    }
    mysql_free_result(result);
	
//
// get poller device info from the NAMES table
//
    query = "SELECT server, host, pid FROM device";
    query += (" WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + poller_name + "'");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	poll_back.db_err = DbErr_DatabaseAccess;
	return (&poll_back);
    }
    result = mysql_store_result(mysql_conn);
    if ((row = mysql_fetch_row(result)) != NULL)
    {
      std::string full_srv_name(row[0]);
      std::string::size_type pos = full_srv_name.find('/');
      std::string srv_name = full_srv_name.substr(0,pos);
      std::string pers_name = full_srv_name.substr(pos+1);
      strcpy(poll_back.server_name, srv_name.c_str());
      strcpy(poll_back.personal_name, pers_name.c_str());
      strcpy(poll_back.host_name, row[1]);
      strcpy(poll_back.process_name, srv_name.c_str()); // Here process = server name
      poll_back.pid = atoi(row[2]);
    }
    else
    {
//
// Return error if the device is not found
//
	mysql_free_result(result);
	poll_back.db_err = DbErr_DeviceNotDefined;
	return(&poll_back);			
    }
//
// Return data
//
    mysql_free_result(result);
    return(&poll_back);
}
