/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * Date:	$Date: 2008-06-22 19:02:40 $
 *
 */

#include "config.h"
#include <API.h>

#include <DevErrors.h>
#if HAVE_MALLOC_H
#	include <malloc.h>
#else
#	include <stdlib.h>
#endif
#ifdef HAVE_SSTREAM
#	include <sstream>
#else
#	include <strstream>
#	define  stringstream	strstream
#endif

#include <sys/socket.h>
#include <db_xdr.h>
#include <fcntl.h>

#include <iostream>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <algorithm>
#include <cctype>
#include <string>


NdbmServer::NdbmServer(const std::string user, const std::string password, const std::string db)
        : DBServer()
{
//
// Open database tables according to the definition
//
	if (*this->db_reopendb_1_svc() == 0)
		this->dbgen.connected = true;
	return;
}

/**
 * To retrieve from the database (built from resources files) a resource value 
 * 
 * @param rece A pointer to a structure of the arr1 type, containing the names of the requested 
 *		resources
 * @param rqstp
 * 
 * @return a pointer to a structure db_res type, containing the resources and the 
 * 	error code. 
 */
db_res *NdbmServer::db_getres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
	int 		k = 0;
	u_int 		num_res = rece->arr1_len;
	struct sockaddr_in 	so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
	socklen_t 		so_size;		// from POSIX draft - already used by GLIBC 
#else
	int 		so_size;
#endif
	u_short 	prot;
//
// Return error code if the server is not connected to the database */
//
	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		browse_back.res_val.arr1_len = 0;
		browse_back.res_val.arr1_val = NULL;
		return (&browse_back);
	}
//
// Retrieve the protocol used to send this request to server 
//
	so_size = sizeof(so);

#ifdef sun
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

	for (int i = 0; i < num_res; i++)
		logStream->debugStream() << "Resource name : " << rece->arr1_val[i] << log4cpp::eol;
//
// Initialize browse_back structure error code 
//
	browse_back.db_err = 0;
//
// Allocate memory for the array of string sended back to client 
//
	try
	{
		browse_back.res_val.arr1_val = new DevString[num_res];
	} 
	catch (const std::bad_alloc &)
	{
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		browse_back.res_val.arr1_len = 0;
		return (&browse_back);
	}
//
// A loop on the resource's number to be looked for 
//
	for (int i = 0; i < num_res; i++)
	{
		std::string ptrc(rece->arr1_val[i]);
//
// Find the table name (DOMAIN) 
//
		std::string::size_type 	pos = ptrc.find('/');
		std::string	  	tab_name(ptrc.substr(0, pos));
		std::string		rest(ptrc.substr(pos + 1));
//
// Try to find the resource value from database 
//
		try
		{
			db_find(tab_name, rest, &browse_back.res_val.arr1_val[i]);
		}
		catch (const int err_db)
		{
			for (int j = 0; j < i; j++)
				delete [] browse_back.res_val.arr1_val[j];
			delete [] browse_back.res_val.arr1_val;
			browse_back.db_err = err_db;
			browse_back.res_val.arr1_len = 0;
			return (&browse_back);
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
					delete [] (browse_back.res_val.arr1_val[j]);
				delete [] browse_back.res_val.arr1_val;
				browse_back.db_err = DbErr_TooManyInfoForUDP;
				browse_back.res_val.arr1_len = 0;
				return (&browse_back);
			}
		}
	}
//
// Initialize the structure sent back to client 
//
	browse_back.res_val.arr1_len = num_res;
//
// Exit server 
//
	return (&browse_back);
}

/**
 * To retrieve all the names of the devices driven by a device server. 
 * 
 * @param dev_name The name of the device server 
 * 
 * @return a pointer to a structure of the db_res type, containing the device names
 *	and the error code.
 */
db_res *NdbmServer::db_getdev_1_svc(DevString *dev_name)
{
	logStream->debugStream() << "Device server name (getdevlist) : " << *dev_name << log4cpp::eol;
//
// Initialize error code sended back to client 
//
	browse_back.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
	if (!dbgen.connected)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		browse_back.res_val.arr1_len = 0;
		browse_back.res_val.arr1_val = NULL;
		return (&browse_back);
	}
//
// Call database function 
//
	try
	{
		int dev_num = db_devlist(*dev_name, &browse_back);

		for (int i = 0; i < dev_num; i++)
			logStream->debugStream() << "Device name : " << browse_back.res_val.arr1_val[i] << log4cpp::eol;

	}
	catch (const int err_db)
	{
		browse_back.db_err = err_db;
//		for (int i = 0;;);
		browse_back.res_val.arr1_len = 0;
    	}
//
// Exit server 
//
	return (&browse_back);
}

/**
 * To retrieve a resource value in the database 
 *
 * @param tab_name The table name where the ressource can be retrieved
 * @param res_name A part of the resource name (FAMILY/MEMBER/RES.) 
 * @param out The adress where to put the resource value (as a string)
 *
 * @return 0 if no errors occurs or the error code when there is a problem.  
 */
int NdbmServer::db_find(const std::string tab_name, const std::string res_name, DevString *out) throw (int)
{
	std::string	p_res_name(res_name),
    			adr_tmp1;
	int 		k,
			ctr = 0,
			res_numb = 1,
	 		i;
	bool		sec_res = (tab_name == "sec");
	GDBM_FILE 	tab;
	datum 		key;

	logStream->debugStream() << "Table name : " << tab_name << log4cpp::eol;

//
// Get family name 
//
 	std::string::size_type	pos = p_res_name.find("/");
	std::string		family = p_res_name.substr(0, pos);
	p_res_name.erase(0, pos + 1);
//
// Get member name 
//
	pos = p_res_name.find("/");
	std::string	member = p_res_name.substr(0, pos);
//
// Get resource name 
//
	std::string	r_name = p_res_name.substr(pos + 1);
//
// For security domain, change all occurances of | by ^ (| is the field separator in NDBM !) 
//
	if (sec_res) 
		std::transform(r_name.begin(), r_name.end(), r_name.begin(), DBServer::make_sec);

	logStream->debugStream() << "Family name : " << family << log4cpp::eol;
	logStream->debugStream() << "Member name : " << member << log4cpp::eol;
	logStream->debugStream() << "Resource name : " << r_name << log4cpp::eol;
//
// Select the right resource table in the right database 
//
	for (i = 0; i < dbgen.TblNum; i++)
		if (tab_name == dbgen.TblName[i])
		{
			tab = dbgen.tid[i];
			break;
		}
	if (i == dbgen.TblNum)
		throw int(DbErr_DomainDefinition);
//
// Try to retrieve the right tuple in table and loop in the case of an array of resources 
//
	try 
	{
		key.dptr = new char[MAX_KEY];
	}
	catch(const std::bad_alloc &)
	{
		logStream->errorStream() << "Error in malloc for key" << log4cpp::eol;
		throw int (DbErr_ServerMemoryAllocation);
	}

	std::string	outstring;
	do
	{
		std::stringstream	s;
#if !HAVE_SSTREAM
		s.seekp(0, std::ios::beg);
#endif
		s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
		strcpy(key.dptr, s.str());
		s.freeze(false);
#else
		strcpy(key.dptr, s.str().c_str());
#endif
		key.dsize = strlen(key.dptr);

		try
		{
			NdbmResCont resu(tab, key);
	    		
			if (ctr)
			{
//
// Copy the new array element in the result buffer. If the temporary buffer
// is full, realloc memory for it. */
//
				adr_tmp1 += SEP_ELT;
				adr_tmp1 += resu.get_res_value();
			}
			else
				adr_tmp1 = resu.get_res_value();
			ctr++;
			res_numb++;
		}
		catch ( ... )
		{
	    		break;
		}
	}
	while (true);
//
// Reset the temporary buffer 
//
	delete [] key.dptr;

	try
	{
		switch(ctr)
		{
//
// If it is a normal resource,so copy the resource value to the result buffer 
//
			case 1 : 	
				*out = new char[adr_tmp1.length() + 1];
	    			strcpy(*out, adr_tmp1.c_str());
				break;
//
// Initialize resource value to N_DEF if the resource is not defined in the database 
//
			case 0 : 	
			    	*out = new char[10];
			    	strcpy(*out, "N_DEF");
				break;
			default : 	
				*out = new char[adr_tmp1.length() + 10];
	    			(*out)[0] = INIT_ARRAY;
	    			sprintf(&((*out)[1]), "%d", ctr);
	    			k = strlen(*out);
	    			(*out)[k] = SEP_ELT;
	    			(*out)[k + 1] = '\0';
	    			strcat(*out, adr_tmp1.c_str());
				break;
		}
	}
	catch(const std::bad_alloc &)
	{
		delete [] key.dptr;
		logStream->errorStream() << "Error in malloc for out" << log4cpp::eol;
		throw int (DbErr_ServerMemoryAllocation);
	}
//
// For resource of the SEC domain, change all occurences of the ^ character to the | character 
//
	if (sec_res)
		std::transform(*out, *out + strlen(*out), *out, &DBServer::make_unsec);
	return (0);
}

/**
 * To retrieve all the devices name for a particular device server 
 * 
 * @param dev_name The device server name
 * @param back The adress  of the db_res structure to be initialized with the devices names
 *              (see the definition of the db_res structure above)
 *    The number of devices managed by the devices server
 *
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
int NdbmServer::db_devlist(const std::string dev_name, db_res * back)
{
	std::string	dev_na(dev_name);
	std::vector<DevString> ptra;
	register int 	d_num = 0;
	int 		i,
			j,
	 		dev_numb = 1;
	datum 		key;
	char 		indnr[4];
//
// Get device server type 
//
	std::string::size_type	pos = dev_na.find("/");
	std::string		ds_class = dev_na.substr(0, pos);
//
// Get device type 
//
	std::string		ds_name = dev_na.substr(pos + 1);

	logStream->debugStream() << "Device server class (getdevlist) : " << ds_class << log4cpp::eol;
	logStream->debugStream() << "Device server name (getdevlist) : " << ds_name << log4cpp::eol;

//
// Try to retrieve the right tuple in NAMES table 
//
	try
	{
		key.dptr = new char[MAX_KEY];
	}
	catch (const std::bad_alloc &)
	{
		return (DbErr_ServerMemoryAllocation);
	}
    	do
    	{
		std::stringstream	s;
#if !HAVE_SSTREAM
		s.seekp(0, std::ios::beg);
#endif
		s << ds_class << "|" << ds_name << "|" << dev_numb << "|" << std::ends;
#if !HAVE_SSTREAM
		strcpy(key.dptr, s.str());
		s.freeze(false);
#else
		strcpy(key.dptr, s.str().c_str());
#endif
		key.dsize = strlen(key.dptr);

		try
		{
			NdbmResCont resu(dbgen.tid[0], key);
//
// Unpack the retrieved content 
//
			std::string	dev_name = resu.get_res_value();
			pos = dev_name.find('|');
			if (pos == std::string::npos)
			{
				logStream->errorStream() << "No separator in the content." << log4cpp::eol;
				delete [] key.dptr;
				throw int (ERR_DEVNAME);
			}
	    		std::string	d_name = dev_name.substr(0, pos);
//
// Allocate memory for device name 
//
			char *p = new char[d_name.length() + 1];
//
// Copy the device name 
//
			strcpy(p, d_name.c_str());
			dev_numb++;
			d_num++;
			ptra.push_back(p);
		}
		catch(const std::bad_alloc &)
		{
			for (std::vector<DevString>::iterator j = ptra.begin(); j != ptra.end(); ++j)
				delete [] *j;
			delete [] key.dptr;
			return (DbErr_ServerMemoryAllocation);
		}
		catch (const int err)
		{
			for (std::vector<DevString>::iterator j = ptra.begin(); j != ptra.end(); ++j)
				delete [] *j;
			delete [] key.dptr;
			return err;
		}
		catch ( ... )
		{
	    		break;
		}
	} while (true);
	delete [] key.dptr;
//
// Initialize the structure 
//
	try
	{
    		back->res_val.arr1_val = new DevString[ptra.size()];
    		back->res_val.arr1_len = ptra.size();
		for (int j = 0; j < back->res_val.arr1_len; ++j)
			back->res_val.arr1_val[j] = ptra[j];
		if (back->res_val.arr1_len == 0)
			throw int (DbErr_DeviceServerNotDefined);
		return (back->res_val.arr1_len);
	}
	catch(const std::bad_alloc &)
	{
		return (DbErr_ServerMemoryAllocation);
	}	
}

/**
 * To insert or update resources 
 *
 * @param rece A pointer to a structure of the tab_putres type, containing
 +	the resources and resource values
 * 
 * @return 0 if no error occurs. Otherwise an error code is returned
 */
DevLong *NdbmServer::db_putres_1_svc(tab_putres * rece)
{
	int 		res_num = rece->tab_putres_len,
	 		i,
			ret_res = 0,
     			res_numb = 1;
	std::string 	domain,
    			family,
    			member,
    			r_name;
	unsigned int 	ctr;
	GDBM_FILE 	tab;
	datum 		key,
			content;

	for (i = 0; i < res_num; i++)
		logStream->debugStream() << "Resource name : " << rece->tab_putres_val[i].res_name << log4cpp::eol;
//
// Initialize sent back error code 
//
	errcode = 0;
//
// Return error code if the server is not connected to the database
//
	if (!dbgen.connected)
	{
		errcode = DbErr_DatabaseNotConnected;
		return (&errcode);
   	}
//
// Allocate memory for key and content pointers 
//
	try
	{
    		key.dptr = new char[MAX_KEY];
		content.dptr = new char[MAX_CONT];
	}
	catch (const std::bad_alloc &)
	{
		delete [] key.dptr;
		errcode = DbErr_ServerMemoryAllocation;
		return (&errcode);
	}
//
// A loop for every resource 
//
	try
	{
    		for (int ires = 0; ires < res_num; ires++)
    		{
			putres 	*tmp_ptr = &(rece->tab_putres_val[ires]);
			res_numb = 1;
//
// Extract domain, family, member and resource name from the full resource name 
//
			std::string		temp = tmp_ptr->res_name;
			std::string::size_type	pos = temp.find('/');
			domain = temp.substr(0, pos);
			temp.erase(0, pos + 1);

			pos = temp.find('/');
			family = temp.substr(0, pos);
			temp.erase(0, pos + 1);

			pos = temp.find('/');
			member = temp.substr(0, pos);
			r_name = temp.substr(pos + 1);

			logStream->debugStream() << "Domain name : " << domain << log4cpp::eol;
			logStream->debugStream() << "Family name : " << family << log4cpp::eol;
			logStream->debugStream() << "Member name : " << member << log4cpp::eol;
			logStream->debugStream() << "Resource name : " << r_name << log4cpp::eol;
//
// Select the right resource table in CS database 
//
			for (i = 0; i < dbgen.TblNum; i++)
			if (domain == dbgen.TblName[i])
			{
				tab = dbgen.tid[i];
				break;
			}
			if (i == dbgen.TblNum)
				throw long(DbErr_DomainDefinition);
//
// Delete the old information (single or array) if the array already exists
//
			for(res_numb = 1;;)
			{
				std::stringstream 	s;
#if !HAVE_SSTREAM
				s.seekp(0, std::ios::beg);
#endif
				s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
				strcpy(key.dptr, s.str());
				s.freeze(false);
#else
				strcpy(key.dptr, s.str().c_str());
#endif
				key.dsize = strlen(key.dptr);
				if (gdbm_delete(tab, key))
					break;  
				res_numb++;
				ret_res = 1;
			} 
//
// If the new update is for an array 
//
			if (tmp_ptr->res_val[0] == INIT_ARRAY)
			{
//
// Retrieve the number of element in the array.
// Initialize the loop counter "ctr" to number of element minus one because
// it is not necessary to look for the element separator to extract the last
// element value from the string. 
//
				std::string 	tmp = tmp_ptr->res_val;
				pos = tmp.find(SEP_ELT);
				int ctr = (unsigned int)atoi(tmp.substr(1, pos - 1).c_str()) - 1;
				tmp.erase(0, pos + 1);
				res_numb = 1;
				for (int l = 0; l < ctr; l++)
				{
//
// Initialize database information 
//
					std::stringstream 	s;
#if !HAVE_SSTREAM
					s.seekp(0, std::ios::beg);
#endif
					s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
					strcpy(key.dptr, s.str());
					s.freeze(false);
#else
					strcpy(key.dptr, s.str().c_str());
#endif
					key.dsize = strlen(key.dptr);
//
// Add one array element in the database 
//
					pos = tmp.find(SEP_ELT);
					strcpy(content.dptr, tmp.substr(0, pos).c_str());
					content.dsize = strlen(content.dptr);
					res_numb++;
					if (gdbm_store(tab, key, content, GDBM_INSERT))
						throw long(DbErr_DatabaseAccess);
					tmp.erase(0, pos + 1);
				}
//
// For the last element value 
//
				strcpy(content.dptr, tmp.c_str());
				content.dsize = strlen(content.dptr);
				std::stringstream	s;
#if !HAVE_SSTREAM
				s.seekp(0, std::ios::beg);
#endif
				s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
				strcpy(key.dptr, s.str());
				s.freeze(false);
#else
				strcpy(key.dptr, s.str().c_str());
#endif
				key.dsize = strlen(key.dptr);
				res_numb++;
				if (gdbm_store(tab, key, content, GDBM_INSERT))
					throw long(DbErr_DatabaseAccess);
			}
			else
			{
				res_numb = 1;
				std::stringstream	s;
#if !HAVE_SSTREAM
				s.seekp(0, std::ios::beg);
#endif
				s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
				strcpy(key.dptr, s.str());
				s.freeze(false);
#else
				strcpy(key.dptr, s.str().c_str());
#endif
				key.dsize = strlen(key.dptr);
				strcpy(content.dptr, tmp_ptr->res_val);
				content.dsize = strlen(content.dptr);
//
// If the resource is already defined in the database, just update the tuple or
// insert a new tuple 
//
				if (gdbm_store(tab, key, content, ret_res ? GDBM_REPLACE : GDBM_INSERT))
					throw long(DbErr_DatabaseAccess);
			}			
		}
	}				
	catch (const long err)
	{
		errcode = err;
	}
//
// Leave server 
//
	delete [] key.dptr;
	delete [] content.dptr;
	return (&errcode);
}

/**
 * To delete resources from the database (built from resource files)
 * 
 * @param rece  A pointer to a structure of the arr1 type, containing the names
 *	of the resources to be deleted 
 * 
 * @return  function returns a pointer to a int. This int is the error code
 *    It is set to 0 is everything is correct. Otherwise, it is initialised 
 *    with the error code.
 */
DevLong *NdbmServer::db_delres_1_svc(arr1 * rece /* , struct svc_req *rqstp */)
{
	u_int 	num_res = rece->arr1_len;
	DevString 	*old_res;

	for (int i = 0; i < num_res; i++)
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
		return (&errcode);
	}
//
// Mark the server as not connected. This will prevent dbm_update to
// add/modify resources during this call 
//
	dbgen.connected = false;
//
// Allocate array for pointers to store deleted resources value 
//
	try
	{
    		old_res = new DevString[num_res];
		memset(old_res, 0, num_res * sizeof(DevString));
		int	err_db;
//
// A loop on the resource's number to be deleted 
//
		for (int i = 0; i < num_res; i++)
		{
			std::string ptrc = rece->arr1_val[i];
//
// Try to delete the resource from database
//
			if ((err_db = db_del(ptrc, &(old_res[i]))) != 0)
			{
				if (i != 0)
					db_reinsert(rece, old_res, i);
				throw long(err_db);
			}
		}
	}
	catch(const std::bad_alloc &)
	{
		errcode = DbErr_ServerMemoryAllocation;
	}
	catch(const long err_db)
	{
		errcode = err_db;
	}
//
// Free memory and exit server 
//
	for (int i = 0; i < num_res; i++)
		if (old_res[i])
			delete [] old_res[i];
	delete [] old_res;
	dbgen.connected = true;
	return (&errcode);
}

/**
 * To delete a resource from the database
 *
 * @param res_name The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)
 * @param p_oldres The address where to store the string to memorize the deleted resource value
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
int NdbmServer::db_del(std::string res_name, DevString *p_oldres)
{
	std::string 	t_name,
			family,
			member,
			r_name;
	int 		i,
			k,
			k1,
			ctr = 0,
			res_numb = 1,
			err;
	datum 		key;
	GDBM_FILE 	tab;
	std::string::size_type 	pos, 
				last_pos;
	static std::string	tmp_buf;
//
// Get table name 
//
	if ((pos = res_name.find('/')) == std::string::npos)
	{
		logStream->errorStream() << "db_del : Error in resource name " << res_name << 	log4cpp::eol;
		return (DbErr_BadResourceType);
	}
	t_name = res_name.substr(0, pos);
//
// Get family name 
//
	if ((pos = res_name.find('/', (last_pos = pos + 1))) == std::string::npos)
	{
		logStream->errorStream() << "db_del : Error in resource name " << res_name << log4cpp::eol;
		return (DbErr_BadResourceType);
	}
	family = res_name.substr(last_pos, pos - last_pos);
//
// Get member name 
//
	if ((pos = res_name.find('/', (last_pos = pos + 1))) == std::string::npos)
	{
		logStream->errorStream() << "db_del : Error in resource name " << res_name << log4cpp::eol;
		return (DbErr_BadResourceType);
	}
	member = res_name.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
	r_name = res_name.substr(pos + 1);

	logStream->debugStream() << "Family name : " << family << log4cpp::eol;
	logStream->debugStream() << "Number name : " << member << log4cpp::eol;
	logStream->debugStream() << "Resource name : " << r_name << log4cpp::eol;
//
// Select the right resource table in database
//
	for (i = 0; i < dbgen.TblNum; i++)
		if (t_name == dbgen.TblName[i])
		{
			tab = dbgen.tid[i];
			break;
		}
	if (i == dbgen.TblNum)
		return (DbErr_DomainDefinition);
//
// Allocate memory to store the old resource value (to reinsert it in case
// of problem) and for the key
//
	try
	{
    		key.dptr = new char[MAX_KEY];
	}
	catch(const std::bad_alloc &)
	{
		return (DbErr_ServerMemoryAllocation);
	}
//
// Try to retrieve the right tuple in table and loop for the case of an array of resource 
//
	do
	{
		std::stringstream	s;
#if !HAVE_SSTREAM
		s.seekp(0, std::ios::beg);
#endif
		s << family <<"|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
		strcpy(key.dptr, s.str());
		s.freeze(false);
#else
		strcpy(key.dptr, s.str().c_str());
#endif
		key.dsize = strlen(key.dptr);

		try
		{
			NdbmResCont	resu(tab, key);
			if (ctr)
			{
//
// Copy the new element in the temporary buffer. If it is full, reallocate
// memory for it. 
//
				tmp_buf += SEP_ELT;
				tmp_buf += resu.get_res_value();
			}
			else
//
// It is the first element, just copy it in the temporary buffer/
//
				tmp_buf = resu.get_res_value();
//
// Remove the tuple from database 
//
			gdbm_delete(tab, key);
			ctr++;
			res_numb++;
		}
		catch ( ... )
		{
//
// Is it an error or simply the data does not exist in the database 
//
			if ((err = gdbm_error(tab)) != 0)
			{
				gdbm_clearerr(tab);
				return (DbErr_DatabaseAccess);
			}
			break;
		}
	} while (true);
	delete [] key.dptr;
//
// If it is a classical resource, copy the res. value in the real old res value buffer 
//
	try
	{
		switch (ctr)
    		{
			case 1: 	
				*p_oldres = new char[tmp_buf.length() + 1];
				strcpy(*p_oldres, tmp_buf.c_str());
				break;
			case 0: 	
				return (DbErr_ResourceNotDefined);
				break;
			default:
//
// For an array of resource, add the number of resources at the beginning of the string 
//
				*p_oldres = new char[tmp_buf.length() + 10];
				(*p_oldres)[0] = INIT_ARRAY;
				sprintf(&((*p_oldres)[1]), "%d", ctr);
				k = strlen(*p_oldres);
				(*p_oldres)[k] = SEP_ELT;
				(*p_oldres)[k + 1] = 0;
				strcat(*p_oldres, tmp_buf.c_str());
		}
	}
	catch (const std::bad_alloc &)
	{
		return (DbErr_ServerMemoryAllocation);
	}
//
// Return if the resource is not found 
//
	return (0);
}

/**
 * To reinsert a resource in the database. This function is called only if the db_del 
 * function returns a error.
 * 
 * @param rece The array passed to the db_delresource server part
 * @param res_value A array with the resource value
 * @param num_res The number of resource to be reinserted
 * 
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
int NdbmServer::db_reinsert(arr1 * rece, DevString *res_value, int num_res)
{
	tab_putres 	tmp;
	int 		j,
			num = 0;
//
// Find out how many resources are really to be reinserted
//
	for (int i = 0; i < num_res; i++)
		if (res_value[i] != NULL)
			num++;
//
// If the error happens after several try to delte only resources which don't
// exist, it  is not necessary to reinsert them ! 
//
	if (num == 0)
		return (0);
	tmp.tab_putres_len = num;
//
// Allocate a array of putres structure (one structure for each res.)
//
	try
	{
		tmp.tab_putres_val = new putres[num];
	}
	catch (const std::bad_alloc &)
	{
		return (DbErr_ServerMemoryAllocation);
	}
//
// Initialise the array of putres structure with the resource name and resource value 
//
	j = 0;
	for (int i = 0; i < num_res; i++)
		if (res_value[i] != NULL)
		{
			tmp.tab_putres_val[j].res_name = rece->arr1_val[i];
			tmp.tab_putres_val[j].res_val = res_value[i];
			j++;
		}
//
// Call the putresource function 
//
	DevLong *p = this->db_putres_1_svc(&tmp);
//
// Leave function 
//
	delete [] tmp.tab_putres_val;
	return (0);
}

void NdbmServer::leave(void)
{
//
// Close database 
//
	for (int i = 0; i < dbgen.TblNum; i++)
		gdbm_close(dbgen.tid[i]);
//
// Exit now 
//
	exit(-1);
}
