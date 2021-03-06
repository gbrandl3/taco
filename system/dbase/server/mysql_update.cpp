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
 *		mysql_update.cpp
 * Description:
 *
 * Authors:
 *		$Author: andy_gotz $
 *
 * Version:	$Revision: 1.16 $
 *
 * Date:	$Date: 2010-01-06 17:36:34 $
 *
 */

#include <DevErrors.h>
#include <algorithm>
#include <MySqlServer.h>


/**
 * update device lists for device servers
 *
 * @param dev_list The device name lists
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_psdev_error *MySQLServer::upddev_1_svc(db_res *dev_list)
{
	long	list_nb = dev_list->res_val.arr1_len;
		

	logStream->debugStream() << "In upddev_1_svc function for " << list_nb << " device list(s)" << log4cpp::eol;
//
// Initialize parameter sent back to client 
//
	psdev_back.error_code = 0;
	psdev_back.psdev_err = 0;

	if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
	{
		logStream->errorStream() << "I'm not connected to database" << log4cpp::eol;
		psdev_back.error_code = DbErr_DatabaseNotConnected;
		return(&psdev_back);
	}
//
// A loop on each device list 
//
	for (long i = 0;i < list_nb;i++)
	{
//		
// Allocate memory for strtok pointers 
//
		std::string lin = dev_list->res_val.arr1_val[i];

		logStream->debugStream() << "Device list = " << lin << log4cpp::eol;
//
// Find the last device in the list. If there is no , character in the line,
// this means that there is only one device in the list 
//
		std::string	ds_class,
				ds_name,
				tmp;
		std::string::size_type pos =  lin.rfind(':');
		if (pos == std::string::npos)
		{
			psdev_back.error_code = DbErr_BadResourceType;
			psdev_back.psdev_err = i + 1;
			return (&psdev_back);
		}
		tmp = lin.substr(0, pos);
		if (count(tmp.begin(), tmp.end(), '/') != 2)
		{
			psdev_back.error_code = DbErr_BadResourceType;
			psdev_back.psdev_err = i + 1;
			return (&psdev_back);
		}
		lin.erase(0, pos + 1);
		pos = tmp.find('/');
		ds_class = tmp.substr(0, pos);
		tmp.erase(0, pos + 1);
		pos = tmp.find('/');
		ds_name = tmp.substr(0,pos);

		std::vector<std::string>	dev_list;
		dev_list.clear();
		while((pos = lin.find(',')) != std::string::npos)
		{
			logStream->debugStream() << "Line = " << lin << log4cpp::eol;
			dev_list.push_back(lin.substr(0, pos));
			lin.erase(0, pos + 1);
		}
		dev_list.push_back(lin);

		std::string query = "SELECT NAME FROM device WHERE ";
		query += (" SERVER LIKE '" + escape_wildcards(ds_class + "/" + ds_name) + "'");

		logStream->debugStream() << "MySQLServer::upddev_1_svc(): query = " << query << log4cpp::eol;

		if (mysql_query(mysql_conn, query.c_str()) != 0)
		{
			logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
			psdev_back.error_code = DbErr_DatabaseAccess;
			psdev_back.psdev_err = i + 1;
			return (&psdev_back);
		}
		MYSQL_RES *result = mysql_store_result(mysql_conn);
		MYSQL_ROW row;

		std::vector<std::string>	db_dev_list;
		db_dev_list.clear();
		while ((row = mysql_fetch_row(result)) != NULL)
			db_dev_list.push_back(row[0]);	
		mysql_free_result(result);

		int 	ind = 1;

		logStream->debugStream() << "Some devices deleted " << log4cpp::eol;

		for (std::vector<std::string>::iterator it = db_dev_list.begin(); it != db_dev_list.end(); ++it)
		{
			logStream->debugStream() << " Device = " << *it; 
			std::vector<std::string>::iterator	it2;  
			if ((it2 = find(dev_list.begin(), dev_list.end(), *it)) != dev_list.end())
			{
				logStream->debugStream() << " found." << log4cpp::eol;	
				switch(psdev_back.error_code = db_update_names(ds_class, ds_name, ind, *it))
				{
					case DbErr_BadResourceType: 	
					case DbErr_DatabaseAccess :	
						psdev_back.error_code = DbErr_DatabaseAccess;
						psdev_back.psdev_err = i + 1;
						return (&psdev_back);
					default	:	
						dev_list.erase(it2);
					      	ind++;
						break;
				}
			}
			else
			{
				logStream->debugStream() << " not found." << log4cpp::eol;
				switch(psdev_back.error_code = db_delete_names(ds_class, ds_name, ind, *it))
				{
					case DbErr_BadResourceType: 	
					case DbErr_DatabaseAccess :	
						psdev_back.error_code = DbErr_DatabaseAccess;
						psdev_back.psdev_err = i + 1;
						return (&psdev_back);
				}
			}
			logStream->debugStream() << " Return = " << psdev_back.error_code << log4cpp::eol;
		}
//
// Delete devices which the same name but registered for other servers
//
		for (std::vector<std::string>::iterator it = dev_list.begin(); it != dev_list.end(); ++it)
		{
			logStream->debugStream() << "Checking device : "<< *it << log4cpp::eol;
// if device was not exported by the same server
			if ( ( find(db_dev_list.begin(), db_dev_list.end(), *it)) == db_dev_list.end() )
			{
				logStream->debugStream() << "Not found -> delete" << log4cpp::eol;
// clean it from the database if an entry exists
				query = "DELETE FROM device WHERE NAME = '" + *it + "'";
				if (mysql_query(mysql_conn, query.c_str()) != 0)
				{
					logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
					psdev_back.error_code = DbErr_DatabaseAccess;
					psdev_back.psdev_err = i + 1;
					return (&psdev_back);
				}
				if (mysql_affected_rows(mysql_conn) == 1)
					logStream->debugStream() << "Deleted existing device : "<< *it << log4cpp::eol;
			}
		}

		for (std::vector<std::string>::iterator it = dev_list.begin(); it != dev_list.end(); ++it)
			if ((psdev_back.error_code = db_insert_names(ds_class, ds_name, ind, *it)) != 0)
			{
				psdev_back.psdev_err = i + 1;
				return (&psdev_back);
			}
	}
//
// return data
//
	return(&psdev_back);
}

/**
 * update the index of devices in the database 
 *
 * @param ds_class The device class name contains the DS 
 * @param ds_name The personal name
 * @param ind The device index list in the list (starting with 1)
 * @param dev_name the device name
 *
 * @returns 0 if no errors occurs or the error code when there is a problem.
 */
long MySQLServer::db_update_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
	if (count(dev_name.begin(), dev_name.end(), '/') != 2)
		return DbErr_BadResourceType;
	std::stringstream query;
// the TANGO database does not have an index number in the device table therefore do nothing
	return 0;
}

/**
 * delete (in the database) a device for a specific device server. 
 *
 * @param ds_class The device class name contains the DS
 * @param ds_name The personal name
 * @param ind The device indice list in the list (starting with 1)
 * @param dev_name the device name
 *
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
long MySQLServer::db_delete_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
	if (count(dev_name.begin(), dev_name.end(), '/') != 2)
		return DbErr_BadResourceType;

	std::string query = "DELETE FROM device WHERE SERVER LIKE '" + escape_wildcards(ds_class + "/" + ds_name)
                         + "' AND NAME = '" + dev_name + "'";
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
		logStream->errorStream() << __LINE__ << mysql_error(mysql_conn) << log4cpp::eol;
		return DbErr_DatabaseAccess;
	}
	if (mysql_affected_rows(mysql_conn) == 1)
		return 0;
	else
		return DbErr_DeviceNotDefined;
}

/**
 * insert a new device in the database
 *
 * @param ds_class The device class name contains the DS
 * @param ds_name The personal name
 * @param ind The device indice list in the list (starting with 1)
 * @param dev_name the device name
 *
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
long MySQLServer::db_insert_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
	std::stringstream query;

	if (count(dev_name.begin(), dev_name.end(), '/') != 2)
		return DbErr_BadResourceType;

	std::string	domain,
			family,
			member;
	std::string::size_type	pos,
			last_pos = 0;

	pos = dev_name.find('/');
	domain = dev_name.substr(0, pos);
	pos = dev_name.find('/', (last_pos = pos + 1));
	family = dev_name.substr(last_pos, (pos - last_pos));
	pos = dev_name.find('/', (last_pos = pos + 1));
	member = dev_name.substr(last_pos, (pos - last_pos));

	query << "INSERT INTO device(NAME, CLASS, SERVER, DOMAIN, FAMILY, MEMBER, IOR, PID, VERSION, EXPORTED)"
	      << " VALUES('" << domain << "/" << family << "/" << member << "','" 
	      << ds_class << "', '" << ds_class << "/" << ds_name << "', '" << domain << "', '" << family
	      << "', '" << member << "', 'nada', 0, 0, 0)" << std::ends;
//	      << "', '" << member << "', 'rpc::0', 0, 0, 0)" << std::ends;

	logStream->debugStream() << query.str() << log4cpp::eol;

#if !HAVE_SSTREAM
	if (mysql_query(mysql_conn, query.str()) != 0)
#else
	if (mysql_query(mysql_conn, query.str().c_str()) != 0)
#endif
	{
		logStream->errorStream() << __LINE__ << mysql_error(mysql_conn) << log4cpp::eol;
#if !HAVE_SSTREAM
		query.freeze(false);
#endif
		return DbErr_DatabaseAccess;
	}
#if !HAVE_SSTREAM
	query.freeze(false);
#endif
	if (mysql_affected_rows(mysql_conn) == 1)
		return 0;
	else
		return DbErr_DatabaseAccess;
}


/**
 * update resources in the database
 *
 * @param res_list the list of resources to update
 *
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_psdev_error *MySQLServer::updres_1_svc(db_res *res_list)
{
	long 			list_nb = res_list->res_val.arr1_len,
				last,
				ind;
	char 			*ptr,
				*ptr_cp,
				*tmp,
				last_dev[DEV_NAME_LENGTH],
				pat[2] = {SEP_ELT, '\0'};

	logStream->debugStream() << "In updres_1_svc function for " << list_nb << " resource(s)" << log4cpp::eol;

//
// Initialize parameter sent back to client 
//
	psdev_back.error_code = 0;
	psdev_back.psdev_err = 0;

	if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
	{
		logStream->errorStream() << "I'm not connected to database" << log4cpp::eol;
		psdev_back.error_code = DbErr_DatabaseNotConnected;
		return(&psdev_back);
	}
//
// A loop on each resources 
//
	for (long i = 0; i < list_nb; i++)
	{
//
// Allocate memory for strtok pointers 
//
		std::string lin(res_list->res_val.arr1_val[i]);

		logStream->debugStream() << "Resource list = " << lin << log4cpp::eol;

//
// Only one update if the resource is a simple one 
//
		if (lin.find(SEP_ELT) == std::string::npos)
		{
			if (upd_res(lin,1,False,&psdev_back.error_code) == -1)
			{
				psdev_back.psdev_err = i + 1;
				return(&psdev_back);
			}
		}
		else
		{		
			try
			{
				ptr_cp = new char[lin.length() + 1];
			}
			catch(std::bad_alloc)
			{
				psdev_back.psdev_err = i + 1;
				psdev_back.error_code = DbErr_ClientMemoryAllocation;
				return(&psdev_back);
			}
//		
// Extract each resource from the list and update table each time 
//
			strcpy(ptr_cp, lin.c_str());
			ind = 1;
			ptr = strtok(ptr_cp,pat);
			if (upd_res(ptr, ind, False, &psdev_back.error_code) == -1)
			{
				delete [] ptr_cp;
				psdev_back.psdev_err = i + 1;
				return(&psdev_back);
			}
			strcpy(ptr_cp, lin.c_str());
			ptr = strtok(ptr_cp,pat); /* Reinit. strtok internal pointer */
			while((ptr = strtok(NULL,pat)) != NULL)
			{
				ind++;	
				if (upd_res(ptr, ind, True, &psdev_back.error_code) == -1)
				{
					delete [] ptr_cp;
					psdev_back.psdev_err = i + 1;
					return(&psdev_back);
				}
			}
			delete [] ptr_cp;
		}
	}
//
// Free memory and return data
//
	return(&psdev_back);
}


/**
 * update a resource in the appropriate table in database 
 *
 * @param lin the modified resource definition (without space and tab characters)
 * @param numb The number of the resource in the array (one if the resource 
 *              type is not an array)
 * @param array A flag to inform the function that this resource is a member of an array 
 * @param p_err pointer to the error code 
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
long MySQLServer::upd_res(std::string lin, long numb, char array, DevLong *p_err)
{
	unsigned int 		diff;
	static std::string	domain,
				family,
				member,
				name;
	std::string			val;

	if (numb == 1)
	{
    		std::string::size_type	pos,
					last_pos;
//
// Get table name 
//
		pos = lin.find('/');
		domain = lin.substr(0, pos).c_str();
		errcode = (domain == "sec");
//
// Get family name 
//
		pos = lin.find('/',  (last_pos = pos + 1));
		family = lin.substr(last_pos, pos - last_pos);
//
// Get member name 
//
		pos = lin.find('/', (last_pos = pos + 1));
		member = lin.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
		pos = lin.find(':', (last_pos = pos + 1));
		name = lin.substr(last_pos, pos - last_pos);

//
// Get resource value (resource values are stored in the database as 
// case dependent std::strings 
//
		val = lin.substr(pos + 1);
	}
	else
		val = lin;

//
// Initialise resource number 
//
	logStream->debugStream() << "Table name : " << domain << log4cpp::eol;
	logStream->debugStream() << "Family name : " << family << log4cpp::eol;
	logStream->debugStream() << "Number name : " << member << log4cpp::eol;
	logStream->debugStream() << "Resource name : " << name << log4cpp::eol;
	logStream->debugStream() << "Resource value : " << val << log4cpp::eol;
	logStream->debugStream() << "Sequence number : " << numb << log4cpp::eol;
//
// Select the right resource table in database 
//
	if (numb == 1)
	{
		int i;
		for (i = 0; i < dbgen.TblNum; i++)
			if (domain == dbgen.TblName[i])
				break;
		if (i == dbgen.TblNum)
		{
			*p_err = DbErr_DomainDefinition;
			return(-1);
		}
	}

	std::string full_res_name = domain + "/" + family + "/" + member + "/" + name;
    
//
// If the resource value is %, remove all the resources.
// If this function is called for a normal resource, I must also 
// remove all the old resources with the old name. This is necessary if there
// is an update of a resource which was previously an array 
//
	if (val == "%" || !array)
	{
		std::string query = "DELETE FROM property_device WHERE DEVICE = '" + domain + "/" + family + "/" + member + "'";
        	query += " AND NAME = '" + name + "'";
		if (mysql_query(mysql_conn, query.c_str()) != 0)
		{
			logStream->errorStream() << __LINE__ << mysql_error(mysql_conn) << log4cpp::eol;
			logStream->errorStream() << query.c_str() << log4cpp::eol;
			*p_err = DbErr_DatabaseAccess;
			return(-1);
		}
// Update the cache
		if (domain == "sec" || domain == "error") 
		{
			db_delete_from_cache(full_res_name);
		}
	}
//
// Insert a new tuple only if the value != %
//
	if (val != "%")
	{
    		std::stringstream query;
		query << "INSERT INTO property_device(DEVICE,NAME,DOMAIN,FAMILY,MEMBER,COUNT,VALUE) VALUES('"
			<< domain << "/" << family << "/" << member << "','" << name << "','"
			<< domain << "','" << family << "','" << member << "','" 
			<< numb << "',\"" << val << "\")" << std::ends;
		logStream->debugStream() << "MySQLServer::upd_res(): query = " << query.str() << log4cpp::eol;

#if !HAVE_SSTREAM
		if (mysql_query(mysql_conn, query.str()) != 0)
#else
		if (mysql_query(mysql_conn, query.str().c_str()) != 0)
#endif
		{
			logStream->errorStream() << __LINE__ << mysql_error(mysql_conn) << log4cpp::eol;
			logStream->errorStream() << query.str() << log4cpp::eol;
#if !HAVE_SSTREAM
			query.freeze(false);
#endif
			*p_err = DbErr_DatabaseAccess;
			return (-1);
		}
#if !HAVE_SSTREAM
		query.freeze(false);
#endif
		// Update the cache
		if( domain == "sec" || domain == "error" ) {
		  db_insert_into_cache(full_res_name,numb,val);
		}
	}
	return DS_OK;
}

