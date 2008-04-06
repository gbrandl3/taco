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
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2008-04-06 09:07:43 $
 *
 */

#include <DevErrors.h>
#include <Sqlite3Server.h>


/**
 * retrieve from the database (built from resources files) a resource value
 *
 * @param rece A pointer to a structure of the arr1 type
 * @param rqstp
 *
 * @return a pointer to a structure of the db_res type.
 */
db_res *SQLite3Server::db_getres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
    int 		j,
    			k = 0;
    u_int 		num_res,
			err_db;
    std::string		tab_name,
    			rest;
    struct sockaddr_in 	so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
    socklen_t  		so_size;   /* from POSIX draft - already used by GLIBC */
#else
    int 		so_size;
#endif
    u_short 		prot;
    int 		k1 = 1;

    logStream->debugStream() << "In db_getres_1_svc " << log4cpp::eol;
//
// Return error code if the server is not connected to the database 
//
    if (dbgen.connected == False)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return(&browse_back);
    }
//
// Retrieve the protocol used to send this request to server 
//
    so_size = sizeof(so);
#ifdef unix
    if (rqstp->rq_xprt->xp_port == getUDPPort())
        prot = IPPROTO_UDP;
    else
        prot = IPPROTO_TCP;
#else
    so_size = sizeof(so);
    if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so, (socklen_t *)&so_size) == -1)
    {
	browse_back.db_err = DbErr_TooManyInfoForUDP;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

    if (so.sin_port == getUDPPort())
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#endif 
    num_res = rece->arr1_len;

    for(int i = 0; i < num_res; i++)
	logStream->debugStream() << "Resource name : " << rece->arr1_val[i] << log4cpp::eol;

//
// Initialize browse_back structure error code 
//
    browse_back.db_err = 0;
//
// Allocate memory for the array of string sended back to client 
//
    int i = 0;
    try
    {
    	browse_back.res_val.arr1_val = new DevString[num_res];
	for (int j = 0; j < num_res; ++j)
		browse_back.res_val.arr1_val[j] = NULL;
//
// A loop on the resource's number to be looked for
//
    	for(i = 0; i < num_res; i++)
    	{
	    std::string ptrc = rece->arr1_val[i];
//
// Find the table name (DOMAIN) 
//
	    std::string::size_type pos = ptrc.find('/');
	    tab_name = ptrc.substr(0, pos);
	    rest = ptrc.substr(pos + 1);
//
// Try to find the resource value from database 
//
	    if((err_db = db_find(tab_name, rest, &browse_back.res_val.arr1_val[i], &k1)) != 0)
	    {
	    	for (int j = 0; j <= i; j++)
		    delete [] browse_back.res_val.arr1_val[j];
		delete [] browse_back.res_val.arr1_val;
	    	browse_back.db_err = err_db;
	    	browse_back.res_val.arr1_len = 0;
	    	return(&browse_back);
	    }
//
// Compute an estimation of the network packet size (Only if the UDP protocol
// has been used to send this request to the server) 
//
	    if (prot == IPPROTO_UDP)
	    {
	    	if ((k = strlen(browse_back.res_val.arr1_val[i]) + k) > SIZE - 1000)
	    	{
		    for (int j = 0; j <= i; j++)
		    	delete [] browse_back.res_val.arr1_val[j];
		    delete [] browse_back.res_val.arr1_val;
		    browse_back.db_err = DbErr_TooManyInfoForUDP;
		    browse_back.res_val.arr1_len = 0;
		    return(&browse_back);
	    	}
	    }
    	}
//
// Initialize the structure sended back to client 
//
    	browse_back.res_val.arr1_len = num_res;
    }
    catch(std::bad_alloc)
    {
	for (int j = 0; j <= i; j++)
	    delete [] browse_back.res_val.arr1_val[j];
	delete [] browse_back.res_val.arr1_val;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	browse_back.res_val.arr1_len = 0;
    }
//
// Exit server 
//
    logStream->debugStream() << "Exit db_getres_1_svc " << log4cpp::eol;
    return(&browse_back);
}


/**
 * retrieve all the names of the devices driven by a device server. 
 * 
 * @param dev_name The name of the device server
 *
 * @returns a pointer to a structure of the db_res type.
 */
db_res *SQLite3Server::db_getdev_1_svc(DevString *dev_name)
{
    int 	dev_num,
		err_db;

    logStream->debugStream() << "Device server name (getdevlist) : " << *dev_name << log4cpp::eol;

//
// Initialize error code sended back to client 
//
    browse_back.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
    if (dbgen.connected == False)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return(&browse_back);
    }
//
// Call database function 
//
    if ((err_db = db_devlist(*dev_name, &dev_num, &browse_back)) != 0)
    {
	browse_back.db_err = err_db;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

    for (int i = 0; i < dev_num; i++)
	logStream->debugStream() << "Device name : " << browse_back.res_val.arr1_val[i] << log4cpp::eol;

//
// Exit server
//
    return(&browse_back);
}



/**
 * To retrieve a resource value in the database 
 *
 * @param tab_name The table name where the ressource can be retrieved
 * @param p_res_name A part of the resource name (FAMILY/MEMBER/RES.)
 * @param The adress where to put the resource value (as a string) 
 * @param The buffer's address used to store temporary results
 *
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
int SQLite3Server::db_find(std::string tab_name, std::string p_res_name, char **out, int *k1)
{
	std::string	family,
			member,
			r_name,
    			adr_tmp1;
	std::string::size_type	pos,
			last_pos;
	int 		k,
			sec_res,
    			i;

	logStream->debugStream() << "Table name : " << tab_name << log4cpp::eol;
//
// Set a flag if the resource belongs to security domain 
//
	sec_res = (tab_name == "sec"); 
//
// Get family name 
//
	pos = p_res_name.find('/');
	family = p_res_name.substr(0, pos);
//
// Get member name
//
	pos = p_res_name.find('/', (last_pos = pos + 1));
	member = p_res_name.substr(last_pos, pos - last_pos);
//
// Get resource name
//
	r_name = p_res_name.substr(pos + 1);
	if ((pos = r_name.find('/')) != std::string::npos)
		r_name.erase(pos,1);
//
// For security domain, change all occurances of | by ^ (| is the field
// separator in NDBM !) 
// 
	if (sec_res == True)
	{
		k = r_name.length();
		for (int i = 0; i < k; i++)
	    		if (r_name[i] == '|')
				r_name[i] = SEC_SEP;
	}
	
#ifdef NEVER
	logStream->errorStream() << "Family name : " << family << log4cpp::eol;
	logStream->errorStream() << "Member name : " << member << log4cpp::eol;
	logStream->errorStream() << "Resource name : " << r_name << log4cpp::eol;
#endif
//
// Select the right resource table in the right database
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (tab_name == dbgen.TblName[i])
			break;
	if (i >= dbgen.TblNum)
		return(DbErr_DomainDefinition);
//
// Try to retrieve the resource in table and loop in the case of an
// array of resources 
//
	std::string query;
	query = "SELECT COUNT, VALUE FROM property_device ";
	query += ("WHERE DEVICE = '" + tab_name + "/" + family + "/" + member + "' AND NAME = '" + r_name);
	query += "' ORDER BY COUNT ASC";

	logStream->debugStream() << "SQLite3Server::db_find(): query = " << query << log4cpp::eol;

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		return (DbErr_DatabaseAccess);
    	}
	*k1 = nrow;
	int j = ncol + 1;
	for (i = 0; i < nrow; i++, j += ncol)
		if (i)
		{
			adr_tmp1 += SEP_ELT;
			adr_tmp1 += result[j];
		}
		else
			adr_tmp1 = result[j];
	sqlite3_free_table(result);
	try
	{
    		switch (i)
    		{
//
// If it is a normal resource,so copy the resource value to the result buffer 
//
			case 1 : 
				*out = new char[adr_tmp1.length() + 1];
				strcpy(*out, adr_tmp1.c_str());
				break;
//
// Initialize resource value to N_DEF if the resource is not defined in the
// database 
//
			case 0 : 
				*out = new char[10];
				strcpy(*out,"N_DEF");
				break;
//
// For an array of resource 
//
			default: 
				*out = new char[adr_tmp1.length() + 10];
				(*out)[0] = INIT_ARRAY;
				sprintf(&((*out)[1]),"%d", i);
				k = strlen(*out);
				(*out)[k] = SEP_ELT;
				(*out)[k + 1] = 0;
				strcat(*out, adr_tmp1.c_str());
				break;
		}
	}
	catch(const std::bad_alloc &e)
	{
		logStream->errorStream() << "Error in malloc for out" << log4cpp::eol;
		throw e;
	}
//
// For resource of the SEC domain, change all occurences of the ^ character
// to the | character 
//
	if (sec_res)
	{
		k = strlen(*out);   
		for (int i = 0;i < k;i++)
			if ((*out)[i] == SEC_SEP)
		(*out)[i] = '|';
	}
	return DS_OK;
}


/**
 * retrieve all the devices name for a particular device server
 * 
 * @param dev_na The  device server name 
 * @param dev_num The number of devices managed by the devices server
 * @param back The address of the db_res structure to be initialized with 
 *              the devices names
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
int SQLite3Server::db_devlist(std::string dev_na, int *dev_num, db_res *back)
{
	char 		**ptra;
	int 		dev_numb = 1;
	unsigned int 	diff;
//
// Get device server type 
//
	std::string::size_type	pos = dev_na.find('/'); 
	std::string ds_class = dev_na.substr(0, pos);
//
// Get device type 
//
	std::string ds_name = dev_na.substr(pos + 1);
//

	logStream->debugStream() << "Device server class (getdevlist) : " << ds_class << log4cpp::eol;
	logStream->debugStream() << "Device server name (getdevlist) : " << ds_name << log4cpp::eol;

//
// Try to retrieve the right tuple in NAMES table 
//
//  
	std::string query;
	query = "SELECT NAME, CLASS FROM device WHERE SERVER = '" + ds_class + "/" + ds_name + "' ORDER BY CLASS";

	logStream->debugStream() << "SQLite3Server::db_devlist(): query " << query << log4cpp::eol;

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		return (DbErr_DatabaseAccess);
	}


	logStream->debugStream() << "SQLite3Server::db_devlist(): " << nrow << " devices found " << log4cpp::eol;

	if (nrow == 0)
		return DbErr_DeviceServerNotDefined;
//
// Allocate memory for the pointer's array 
//
	int i = 0;
	int j = ncol;
	try
	{
		ptra = new DevString[nrow];
		for (i = 0; i < nrow; i++, j += ncol)
		{
//
// Allocate memory for device name 
//
			ptra[i] = new char[strlen(result[j]) + 1];
//
// Copy the device name 
//
			strcpy(ptra[i], result[j]);
		}
//
// Initialize the structure 
//
		back->res_val.arr1_val = ptra;
		back->res_val.arr1_len = *dev_num = nrow;
		sqlite3_free_table(result);
	}
	catch(std::bad_alloc)
	{
		sqlite3_free_table(result);
        	for (int j = 0; j < i; j++)
			delete [] ptra[j];
		delete [] ptra;
		back->res_val.arr1_val = NULL;
    		back->res_val.arr1_len = *dev_num = 0;
		return DbErr_ServerMemoryAllocation;
	}
	return 0;
}

/**
 * To insert or update resources
 * 
 * @param rece A pointer to a structure of the tab_putres type
 * 
 * @return 0 if no error occurs. Otherwise an error code is returned
 */
DevLong *SQLite3Server::db_putres_1_svc(tab_putres *rece)
{
	int 			res_num = rece->tab_putres_len,
				res_numb = 1;
	std::string::size_type	pos,
				last_pos;
	std::string		res_name,
				res_val;
	register putres 	*tmp_ptr;
	char 			indnr[16];

	for (int i = 0; i < res_num; i++)
		logStream->debugStream() << "Resource name : " << rece->tab_putres_val[i].res_name << log4cpp::eol;
//
// Initialize sent back error code 
//
	errcode = 0;
//
// Return error code if the server is not connected to the database 
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode); 
	}
//
// A loop for every resource 
//
	for (int ires = 0; ires < res_num; ires++)
   	{
		std::string 	content;
		tmp_ptr = &(rece->tab_putres_val[ires]);
		res_numb = 1;
		res_name = tmp_ptr->res_name;

		if (db_del(res_name) != 0)
			logStream->errorStream() << "Could not delete resource" << res_name << log4cpp::eol;
		res_val = tmp_ptr->res_val;
//
// Try to retrieve this resource from the database 
//
// If the new update is for an array 
//

		logStream->debugStream() << "SQLite3Server::db_putres_1_svc() : Resource value : " << res_val << log4cpp::eol;

		if (res_val[0] == INIT_ARRAY)
		{
//
// Retrieve the number of element in the array.
// Initialize the loop counter "ctr" to number of element minus one because
// it is not necessary to look for the element separator to extract the last
// element value from the string. 
//
			if ((pos = res_val.find(SEP_ELT)) == std::string::npos)
			{
				logStream->errorStream() << "Missing '" << SEP_ELT <<"' in resource value." << log4cpp::eol;
				errcode = DbErr_BadResSyntax;
				return (&errcode);
			}
			int ctr = int(atoi(res_val.substr(1, pos - 1).c_str()) - 1);

			logStream->debugStream() << "SQLite3Server::db_putres_1_svc() : array " << ctr << " members" << log4cpp::eol;

			res_numb = 1;
			res_val.erase(0, pos + 1);
			for (int l = 0; l < ctr; l++)
			{
//
// Initialize database information 
//
				snprintf(indnr, sizeof(indnr), "%d", res_numb++);
//
// Add one array element in the database 
//
				pos = res_val.find(SEP_ELT);
				content = res_val.substr(0, pos);
				res_val.erase(0, pos + 1);

				logStream->debugStream() << "SQLite3Server::db_putres_1_svc() : " << l << ". member : " << content << log4cpp::eol;

				if (db_insert(res_name, indnr, content) != 0)
				{
					errcode = DbErr_DatabaseAccess;
					return(&errcode);
				}
			} 
//
// For the last element value 
			content = res_val;

			logStream->debugStream() << "SQLite3Server::db_putres_1_svc() : last member : " << content << log4cpp::eol;

			snprintf(indnr, sizeof(indnr), "%d",res_numb++);
			if (db_insert(res_name, indnr, content) != 0)
			{
				errcode = DbErr_DatabaseAccess;
				return(&errcode);
			}
		}
		else
		{
			snprintf(indnr, sizeof(indnr), "%d",res_numb++);
			if (db_insert(res_name, indnr, res_val) != 0)
			{ 
				errcode = DbErr_DatabaseAccess;
				return(&errcode);
			} 
		} 
	} 
//
// Leave server
//
	return(&errcode);
}


/**
 * delete resources from the database (built from resource files)
 * 
 * @param rece A pointer to a structure of the arr1 type
 *
 * @return a pointer to a int. This int is the error code It is set to 0 is everything 
 * is correct. Otherwise, it is initialised with the error code.
 */
DevLong *SQLite3Server::db_delres_1_svc(arr1 *rece/*, struct svc_req *rqstp*/)
{
    int 	j;
    u_int 	num_res = rece->arr1_len,
		err_db;
    register char *ptrc;

    for(int i = 0; i < num_res; i++)
	logStream->debugStream() << "Resource to delete : " << rece->arr1_val[i] << log4cpp::eol;

//
// Initialize error code 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database files
//
    if (dbgen.connected == False)
    {
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Mark the server as not connected. This will prevent dbm_update to
// add/modify resources during this call
//
    dbgen.connected = False;
//
// Allocate array for pointers to store deleted resources value
//
//
// A loop on the resource's number to be deleted

    for (int i = 0; i < num_res; i++)
    {
//
// Try to delete the resource from database 
//
	if((errcode = db_del(rece->arr1_val[i])) != 0 )
	{
	    dbgen.connected = True;
	    logStream->errorStream() << "Could not delete resource " << rece->arr1_val[i] << log4cpp::eol;
	    return(&errcode);
    	}
    }
//
// Free memory and exit server 
//
    dbgen.connected = True;
    return(&errcode);
}

/**
 * add a resource to the database
 * 
 * @param res_name The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)
 * @param number The address where to store the string to memorize the deleted resource value
 * @param content The content of the resource
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
int SQLite3Server::db_insert(std::string res_name, std::string number, std::string content)
{
	std::string	domain,
    			family,
    			member,
    			r_name;
	std::string::size_type 	pos, 
			last_pos;
	int 		i;
//
// Get table name 
//
	if ((pos = res_name.find('/')) == std::string::npos)
	{
		logStream->errorStream() << "db_del : Error in resource name " << res_name << log4cpp::eol;
		return(DbErr_BadResourceType);
 	}
	domain = res_name.substr(0, pos);
//
// Get family name 
//
	if ((pos = res_name.find('/', (last_pos = pos + 1))) == std::string::npos)
	{
		logStream->errorStream() << "db_insert : Error in resource name " << res_name << log4cpp::eol;
		return(DbErr_BadResourceType);
	}
	family = res_name.substr(last_pos, pos - last_pos);
//
// Get member name 
//
	if ((pos = res_name.find('/', (last_pos = pos + 1))) == std::string::npos)
	{
		logStream->errorStream() << "db_insert : Error in resource name " <<res_name << log4cpp::eol;
		return(DbErr_BadResourceType);
	}
	member = res_name.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
	r_name = res_name.substr(pos + 1);

#ifdef NEVER
	logStream->errorStream() << "Family name : " << family << log4cpp::eol;
	logStream->errorStream() << "Number name : " << member << log4cpp::eol;
	logStream->errorStream() << "Resource name : " << r_name << log4cpp::eol;
#endif
//
// Select the right resource table in database
//
	for (i = 0; i < dbgen.TblNum;i++)
		if (domain == dbgen.TblName[i])
			break;
	if (i == dbgen.TblNum)
		return(DbErr_DomainDefinition);
//
	std::string query;
	query = "INSERT INTO property_device(DEVICE,NAME,DOMAIN,FAMILY,MEMBER,COUNT,VALUE,UPDATED,ACCESSED)";
	query += " VALUES('" + domain + "/" + family + "/" + member + "','" + r_name + "','"; 
	query += (domain + "','" + family +"','" + member + "','");
	query += (number + "','" + content + "', DATETIME('now'), DATETIME('now'))");

	logStream->debugStream() << "SQLite3Server::db_insert(): query = " << query << log4cpp::eol;

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		return (DbErr_DatabaseAccess);
	}
	sqlite3_free_table(result);
	return DS_OK;
}


/**
 * delete a resource from the database
 * 
 * @param res_name The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
int SQLite3Server::db_del(std::string res_name)
{
    std::string		t_name,
    			family,
    			member,
    			r_name;
    std::string::size_type 	pos, 
			last_pos;
    int			i;
//
// Get table name 
//
    if ((pos = res_name.find('/')) == std::string::npos)
    {
	logStream->errorStream() << "db_del : Error in resource name " << res_name << log4cpp::eol;
	return(DbErr_BadResourceType);
    }
    t_name = res_name.substr(0, pos);
//
// Get family name 
//
    if ((pos = res_name.find('/', 1 + (last_pos = pos))) == std::string::npos)
    {
	logStream->errorStream() << "db_del : Error in resource name " << res_name << log4cpp::eol;
	return(DbErr_BadResourceType);
    }
    family = res_name.substr(last_pos + 1, pos - last_pos - 1);
//
// Get member name 
//
    if ((pos = res_name.find('/', 1 + (last_pos = pos))) == std::string::npos)
    {
	logStream->errorStream() << "db_del : Error in resource name " <<res_name << log4cpp::eol;
	return(DbErr_BadResourceType);
    }
    member = res_name.substr(last_pos + 1, pos - last_pos - 1);
//
// Get resource name 
//
   last_pos = pos;
    r_name = res_name.substr(last_pos + 1);

#ifdef NEVER
    logStream->errorStream() << "Family name : " << family << log4cpp::eol;
    logStream->errorStream() << "Number name : " << member << log4cpp::eol;
    logStream->errorStream() << "Resource name : " << r_name << log4cpp::eol;
#endif
//
// Select the right resource table in database
//
    for (i = 0; i < dbgen.TblNum;i++)
	if (t_name == dbgen.TblName[i])
	    break;
    if (i == dbgen.TblNum)
	return(DbErr_DomainDefinition);
//
// Try to retrieve the right tuple in table and loop for the case of an
// array of resource 
//
	std::string query;
	query = "DELETE FROM property_device WHERE DEVICE = '" + t_name + "/" + family + "/" + member + "'";
	query += " AND NAME = '" + r_name + "'";

	logStream->debugStream() << "db_del : query = " << query << log4cpp::eol;

	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
		logStream->errorStream() << sqlite3_errmsg(db) << log4cpp::eol;
		return (DbErr_DatabaseAccess);
	}
	return(0);
}

