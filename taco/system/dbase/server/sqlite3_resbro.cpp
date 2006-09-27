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
#include <algorithm>
#include <Sqlite3Server.h>


/**
 * retrieve resource domain list for all the resources defined in the database
 *
 * @return The domain name list
 */
db_res *SQLite3Server::resdomainlist_1_svc()
{
    std::vector<std::string> 	dom_list;
	
#ifdef DEBUG
    std::cout << "In resdomainlist_1_svc function" << std::endl;
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
// Find each domain
//
    for (long i = 0;i < dbgen.TblNum;i++)
    {
	if ((dbgen.TblName[i] != "names") &&
	    (dbgen.TblName[i] != "ps_names"))
	    dom_list.push_back(dbgen.TblName[i]);
    }
//
// Sort domain name list
//
    sort(dom_list.begin(), dom_list.end());
//
// Bild the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = dom_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = dom_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        dom_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	std::cout << "Memory allocation error in resdomainlist" << std::endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);	
}


/**
 * retrieve all the families defined (in resources name) for a specific domain
 *
 * @param domain The domain name
 * 
 * @return The family name list
 */
db_res *SQLite3Server::resfamilylist_1_svc(nam* domain)
{
    std::vector<std::string> 	fam_list;
	
#ifdef DEBUG
    std::cout << "In resfamilylist_1_svc function for domain " << *domain << std::endl;
#endif

    std::string user_domain(*domain);
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
	std::cout << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources family name defined in this table
//
    std::string query;
    query = "SELECT DISTINCT FAMILY FROM property_device WHERE DOMAIN = '";
    query += (user_domain + "' ORDER BY FAMILY ASC");
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		browse_back.db_err = DbErr_DatabaseAccess;
		return (&browse_back);
	}
	int j = ncol; 
	for (int i = 0; i < nrow; ++i, j += ncol)
		fam_list.push_back(result[j]);
	sqlite3_free_table(result);                                                                            
//
// Sort family name list
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = fam_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = fam_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        fam_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                     
#if 0
    if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	std::cout << "Memory allocation error in resfamilylist" << std::endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);	
}


/**
 * retrieve all the members defined (in resources name) for a specific couple domain,family
 *
 * @param recev The domain name, the family name
 * 
 * @return The member name list
 */
db_res *SQLite3Server::resmemberlist_1_svc(db_res *recev)
{
    std::vector<std::string> 	memb_list;
    std::string 		user_domain(recev->res_val.arr1_val[0]),
  	   			user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
    std::cout << "In resmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << std::endl;
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
	std::cout << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources family name defined in this table
//
	std::string query;
	query = "SELECT DISTINCT MEMBER FROM property_device WHERE DOMAIN = '";
	query += (user_domain + "' AND FAMILY = '" + user_family + "' ORDER BY MEMBER ASC");
#ifdef DEBUG 
	std::cout << "SQLite3Server::resmemberlist_1_svc() : " << query << std::endl;
#endif
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		browse_back.db_err = DbErr_DatabaseAccess;
		return (&browse_back);
	}
#ifdef DEBUG 
	std::cout << "SQLite3Server::resmemberlist_1_svc() : " << nrow << " rows found" << std::endl;
#endif

	int j = ncol; 
	memb_list.clear();
	for (int i = 0; i < nrow; ++i, j += ncol)
	{
#ifdef DEBUG 
		std::cout << "SQLite3Server::resmemberlist_1_svc() : " << result[j] << std::endl;
#endif
		memb_list.push_back(result[j]);
	}
	sqlite3_free_table(result);                                                                            
//
// Sort member name list
//
//
// Build the structure returned to caller
//
	int length = browse_back.res_val.arr1_len = memb_list.size();
	browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
	for (int i = 0;i < length;i++)
	{
        	int     k = memb_list[i].length();
        	browse_back.res_val.arr1_val[i] = new char [k + 1];
        	memb_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        	(browse_back.res_val.arr1_val[i])[k] = '\0';
    	}                                                                                                     
#if 0
    if (memb_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
		std::cout << "Memory allocation error in resmemberlist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
    }
#endif
#ifdef DEBUG 
	std::cout << "SQLite3Server::resmemberlist_1_svc() : finish" << std::endl;
#endif
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
db_res *SQLite3Server::resresolist_1_svc(db_res *recev)
{
    std::vector<std::string> 	reso_list;
    std::string 		user_domain(recev->res_val.arr1_val[0]),
    			user_family(recev->res_val.arr1_val[1]),
    			user_member(recev->res_val.arr1_val[2]);
	
#ifdef DEBUG
    std::cout << "In resresolist_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << std::endl;
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
	std::cout << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
    std::string query;

	query = "SELECT DISTINCT NAME FROM property_device WHERE DOMAIN = '";
    	query += (user_domain + "' AND FAMILY = '" + user_family);
	if (user_member != "*")
	{
		query += ("' AND MEMBER = '" + user_member + "' ORDER BY NAME ASC");
	}
        else
	{
                query += ("' ORDER BY NAME ASC");
	}

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		browse_back.db_err = DbErr_DatabaseAccess;
		return (&browse_back);
	}
	
	int j = ncol; 
	for (int i = 0; i < nrow; ++i, j += ncol) 
		reso_list.push_back(result[j]);
	sqlite3_free_table(result);                                                                            
//
// Sort resource name list
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = reso_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = reso_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        reso_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (reso_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	std::cout << "Memory allocation error in resresolist" << std::endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);	
}

/**
 * To retrieve a device resource value (as strings)
 *
 * @param recev The device domain name, the device family name, the device member name,
 *	the resource name
 * 
 * return the resource value list
 */
db_res *SQLite3Server::resresoval_1_svc(db_res *recev)
{
    std::string 		res_val;
    std::vector<std::string> 	reso_list;
    std::string 		user_domain(recev->res_val.arr1_val[0]),
    			user_family(recev->res_val.arr1_val[1]),
    			user_member(recev->res_val.arr1_val[2]),
    			user_reso(recev->res_val.arr1_val[3]);
		
#ifdef DEBUG
    std::cout << "In resresoval_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << "/" << user_reso << std::endl;
#endif
	
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
	std::cout << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get a list of all members and resource name
//
    std::string query;

    query = "SELECT MEMBER, NAME, COUNT, VALUE FROM property_device WHERE ";
    query += (" DOMAIN = '" + user_domain + "' AND FAMILY = '" + user_family + "'");
    if (user_member != "*")
    	    query += (" AND MEMBER = '" + user_member + "'");
    if (user_reso != "*")
	    query += (" AND NAME = '" + user_reso + "'");
    query += (" ORDER BY MEMBER ASC, NAME ASC, COUNT ASC");
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << std::endl;
		browse_back.db_err = DbErr_DatabaseAccess;
		return (&browse_back);
	}
	if (nrow == 0)
	{ 
		if (user_member != "*" && user_reso != "*")
		{
			sqlite3_free_table(result);
			browse_back.db_err = DbErr_ResourceNotDefined;
			return(&browse_back);
		}
	}
	res_val = "";
	int j = ncol;
	for (int i = 0; i < nrow; ++i, j += ncol)
    	{
//	MEMBER, NAME, INDEX_RES, RESVAL
//
// Build the first field of the complete resource name
//
	if (atoi(result[j + 2]) == 1)
	{
	    if (res_val.length() != 0)
		reso_list.push_back(res_val);
	    res_val = user_domain + "/" + user_family + "/" + result[j] + "/" + result[j + 1] + ": " + result[j + 3];
	}
	else
	    res_val += ("," + std::string(result[j + 3]));
    }
    reso_list.push_back(res_val);
    sqlite3_free_table(result);                                                                            
//
// Sort the resource array
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = reso_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = reso_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        reso_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (reso_val.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	std::cout << "Memory allocation error in resresoval" << std::endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);
}