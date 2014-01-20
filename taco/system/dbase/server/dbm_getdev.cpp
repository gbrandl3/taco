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
 *
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.16 $
 *
 * Date:	$Date: 2008-04-06 09:07:40 $
 *
 */

#include "config.h"

#include <API.h>
#include <DevErrors.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <db_xdr.h>
#include <fcntl.h>

/* Some C++ include files */

#include <iostream>
#include <algorithm>
#include <string>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <errno.h>


/**
 * To retrieve the names of the exported devices device server. 
 * 
 * @param fil_name A string to filter special device names in the whole list of 
 *	     the exported device
 * @param rqstp
 *
 * @return a pointer to a structure of the db_res type.
 */
db_res *NdbmServer::db_getdevexp_1_svc(DevString *fil_name,struct svc_req *rqstp)
{
	std::string     device_name(*fil_name);
	int 		i,
			err,
			ret,
	 		k1 = 1,
			flags=O_RDWR;
	bool		exit = false;
	device 		dev;
	struct sockaddr_in so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
	socklen_t  	so_size;   /* from POSIX draft - already used by GLIBC */
#else
	int 		so_size;
#endif
	datum 		key, 
			key2, 
			content;
	std::string	ret_host_name,
			ret_pn,
			prgnr;

/* If the server is not connected to the database, return an error */
	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		browse_back.res_val.arr1_len = 0;
		return(&browse_back);
	}

#ifdef sun
	if (rqstp->rq_xprt->xp_port == getUDPPort())
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#else
/* Retrieve the protocol used to send this request to the server */
	so_size = sizeof(so);
	if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so, (socklen_t *)&so_size) == -1)
	{
		browse_back.db_err = DbErr_MaxDeviceForUDP;
		browse_back.res_val.arr1_len = 0;
		return(&browse_back);
	}

	if (so.sin_port == getUDPPort())
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#endif
//
// Extract from filter string each part of the filter (domain, family and 
// member). If two / characters can be retrieved in the filter string, this
// means that the domain, family and member part of the filter are initialized.
// If only one / can be retrieved, only the domain and family part are
// initialized and if there is no / in the filter string, just the domain
// is initialized. 
//
        std::string::size_type       pos;
        std::string             member,
                                family,
                                domain;
#ifndef _solaris
        switch(std::count(device_name.begin(), device_name.end(), '/'))
#else
        switch(NdbmServer::count(device_name.begin(), device_name.end(), '/'))
#endif /* _solaris */
        {
                case 2 : 
			pos =  device_name.find('/');
                        domain = device_name.substr(0, pos);    // potential error
                        device_name.erase(0, pos + 1);
                        pos =  device_name.find('/');
                        family  = device_name.substr(0, pos);   // potential error
                        member = device_name.substr(pos + 1);
                        break;
                case 1 : 
			pos =  device_name.find('/');
                        domain = device_name.substr(0, pos);    // potential error
                        family  = device_name.substr(pos + 1);  // potential error
                        member = "*";
                        break;
                case 0 : 
			domain = device_name;                  // potential error
                        family = "*";
                        member = "*";
                        break;
        }

	logStream->debugStream() << "filter domain : " << domain << log4cpp::eol;
	logStream->debugStream() << "filter family : " << family << log4cpp::eol;
	logStream->debugStream() << "filter member : " << member << log4cpp::eol;

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;

/* Allocate memory for the pointer's array */
	dev_num = 0;

	std::vector<DevString>        ptra;

/* Try to retrieve all tuples in the database NAMES table with the PN column different than "not_exp" */
	for (key = gdbm_firstkey(dbgen.tid[0]);
		key.dptr != NULL;
		key2 = key, key = gdbm_nextkey(dbgen.tid[0], key), free(key2.dptr))
	{
		content = gdbm_fetch(dbgen.tid[0], key);
		if (content.dptr != NULL)
		{
			std::string	temp(content.dptr, content.dsize);
			free(content.dptr);

                        std::string::size_type       pos = temp.find('|');
                        strcpy(dev.d_name, temp.substr(0, pos).c_str());	// length check !!
                        temp.erase(0, pos + 1);

                        pos = temp.find('|');
                        ret_host_name = temp.substr(0, pos);
                        temp.erase(0, pos + 1);

                        pos = temp.find('|');
                        ret_pn = temp.substr(0, pos);
			if (ret_pn != "0")
			{
//
// Move the the rest of the content and key values to the structure of dev
//
				strcpy(dev.h_name, ret_host_name.c_str());
				dev.pn = atoi(ret_pn.c_str());

				temp.erase(0, pos + 1);
				pos = temp.find('|');
				dev.vn = atoi(temp.substr(0, pos).c_str());
				temp.erase(0, pos + 1);

				pos = temp.find('|');
				strcpy(dev.d_class, temp.substr(0, pos).c_str());
				temp.erase(0, pos + 1);

				pos = temp.find('|');
				strcpy(dev.d_type, temp.substr(0, pos).c_str());
				temp.erase(0, pos + 1);

				pos = temp.find('|');
				dev.pid = atoi(temp.substr(0, pos).c_str());
//
// Extract device server class and name from the key
//
				temp = key.dptr;
				if ((pos = temp.find('|')) == std::string::npos)
				{
					logStream->errorStream() << "No separator in db tuple" << log4cpp::eol;
					browse_back.db_err = DbErr_DatabaseAccess;
					browse_back.res_val.arr1_len = 0;
					free(key.dptr);
					return(&browse_back);
				}
				strcpy(dev.ds_class, temp.substr(0, pos).c_str());
				temp.erase(0, pos + 1);

				pos = temp.find('|');
				strcpy(dev.ds_name, temp.substr(0, pos).c_str());
//
// Extract the domain part of the device name from the retrieved tuple */
//
                                temp = dev.d_name;
                                pos = temp.find('/');
                                std::string domain_tup =  temp.substr(0, pos);

				logStream->debugStream() << "Domain part from DB: " << domain_tup << log4cpp::eol;

//
// Call the stringOK function to verify that the retrieved devices device name is OK
// If the domain part of the filter is *, directly call the fam_fil  function
//
				if (!stringOK(domain, domain_tup))
				{
					try
					{
						ptra.push_back(fam_fil(&dev, family, member, prot));
					}
					catch(const int err)
					{
						if (err)
						{
							browse_back.res_val.arr1_val = &ptra.front();
							for (int i = 0; i < dev_num; i++)
								delete [] browse_back.res_val.arr1_val[i];
							browse_back.db_err = err;
							browse_back.res_val.arr1_len = 0;
							free(key.dptr);
							return(&browse_back);
						}
					}
				}
			}
		}
	} 
//
// Initialize the structure sended back to client and leave the server
//
	browse_back.res_val.arr1_val = new char *[dev_num];
	browse_back.res_val.arr1_len = dev_num;
	for (int i = 0; i < dev_num; ++i)
		browse_back.res_val.arr1_val[i] = ptra[i];
	browse_back.db_err = 0;
	return(&browse_back);
}

/**
 * check if the family part of the filter is the same as the family part of the 
 * device name retrieved from the database
 * 
 * @param dev1 
 * @param family
 * @param member 
 * @param prot
 *
 * @return 
 */
char* NdbmServer::fam_fil(device *dev1, const std::string &family, const std::string &member, int prot) throw (int)
{
	std::string 	tmp(dev1->d_name);
	unsigned int 	diff;
	int 		err,ret;
//
// If the family part of the filter is *, directly call the memb_fil
// function 
//

	logStream->debugStream() << "Arrived in fam_fil function" << log4cpp::eol;

//
// Extract the family part of the device name in the retrieved tuple 
//
#ifndef _solaris
	if (std::count(tmp.begin(), tmp.end(), '/') != 2)
#else
	if (NdbmServer::count(tmp.begin(), tmp.end(), '/') != 2)
#endif /* _solaris */
		throw int(1);
	std::string::size_type	first = tmp.find('/'),
				second = tmp.rfind('/');
	std::string 		family_tup = tmp.substr(first + 1, second - first);

	logStream->debugStream() << "Family part from DB : " << family_tup << log4cpp::eol;

//
// Special case for the data collector pseudo devices which are not real 
// devices. It is impossible to execute command on them. So, don't return
// them to the caller. 
//
	if ("dc_rd" == family_tup.substr(0, 5))
		throw int(0);
	if ("dc_wr" == family_tup.substr(0, 5))
		throw int(0);
//
// Call the stringOK to verify that the family name is OK 
// Is this family part the same than in the filter, call the memb_fil function 
//
	if (!stringOK(family, family_tup))
		return memb_fil(dev1, member, prot);
	throw int(0);
}

/**
 * check if the member part of the filter is the same as the member part of the device 
 * name retrieved from the database. If yes, this device name must be returned to the caller
 * 
 * @param dev2 
 * @param member
 * @param prot	
 *
 * @return 
 */
char *NdbmServer::memb_fil(device *dev2, const std::string &member, int prot) throw (int)
{
	std::string 		tmp(dev2->d_name);
	std::string::size_type 	pos;
	char			*p;
	int 			tp,
				ret;
//
// If the member part of the filter is *, the device name must be sent back to
// the user. This means copy the device name in the special array (allocate
// memory for the string) and increment the device name counter 
//

	logStream->debugStream() << "Arrived in memb_fil function" << log4cpp::eol;

//
// Extract the member part of the device name in the retrieved tuple */
//
#ifndef _solaris
	if (std::count(tmp.begin(), tmp.end(), '/') != 2)
#else
	if (NdbmServer::count(tmp.begin(), tmp.end(), '/') != 2)
#endif /* _solaris */
		throw int(1);
	pos = tmp.rfind('/');

	logStream->debugStream() << "member part from DB : " << tmp.substr(pos + 1) << log4cpp::eol;

//
// Call the stringOK function to verify that the member part of the
// retrieved device is OK 
// Is this member part the same than in the filter ? If yes, the device name
// must be sent back to the user: copy the device name in the special array
// (allocate memory for the string) and increment the device name counter. 
//
	if (!stringOK(member, tmp.substr(pos + 1)))
	{
		try
		{
			p = new char[strlen(dev2->d_name) + 1];
		}
		catch(const std::bad_alloc &)
		{
			throw int(DbErr_ServerMemoryAllocation);
		}
		strcpy(p, dev2->d_name);
		dev_num++;
		if (prot == IPPROTO_UDP && dev_num == MAXDEV_UDP)
			throw int(DbErr_MaxDeviceForUDP);

		logStream->debugStream() << "One more device name" << log4cpp::eol;

	}
	return p;
}

/**
 * check if a string is the same than a wanted string This function allow the 
 * caller to have ONE wildcard '*' in the wanted string.
 *
 * @param wanted the wanted string
 * @param retrieved the string to be compared
 * 
 * @return 0 if the string in the same than the wanted one Otherwise, this function returns 1
 */
bool NdbmServer::stringOK(const std::string wanted, const std::string retrieved)
{
//
// If the wanted string is only the wild card, return 0 
//
	if (wanted == "*")
		return(0);
//
// If the wild card is in the wanted string, compute the number of
// characters before and after it 
//
	std::string::size_type	pos;
	if ((pos = wanted.find('*')) != std::string::npos)
	{
		if (pos >= retrieved.length())
			return(1);
		if (wanted.length() - pos - 1 >= retrieved.length())
			return(1);
	}
//
// Test to see if the string is stricly the same than the wanted one 
//
	else
		return (wanted == retrieved); 
//
// Test characters before the wild card
// 
	if (wanted.substr(0, pos) != retrieved.substr(0, pos))
		return(1);
//
// Test characters after the wild card 
//
	long l = wanted.length() - (pos + 1);
	if (wanted.substr(pos + 1) != retrieved.substr(0, retrieved.length() - l))
		return(1);
//
// Leave function 
//
	return(0);
}
