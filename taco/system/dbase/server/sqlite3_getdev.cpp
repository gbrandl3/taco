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
 * retrieve the names of the exported devices of a device server. 
 *
 * @param fil_name A string to filter special device names in the whole list of 
 *	     the exported device
 * @param rqstp
 *
 * @return a pointer to a structure of the db_res type.
 */
db_res *SQLite3Server::db_getdevexp_1_svc(nam *fil_name,struct svc_req *rqstp)
{
	std::string 	domain,	
    			family("%"),			
    			member("%");
	std::string::size_type	pos,
			last_pos;
	int 		dev_num = 0;
	char 		**ptra;
	struct sockaddr_in 	so;
	u_short 		prot;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
	socklen_t  	so_size;   /* from POSIX draft - already used by GLIBC */
#else
	int 		so_size;
#endif
//
// If the server is not connected to the database, return an error 
//
	if (dbgen.connected == False)
	{
		std::cout << "I'm not connected to database" << std::endl;
		browse_back.db_err = DbErr_DatabaseNotConnected;
		browse_back.res_val.arr1_len = 0;
		return(&browse_back);
	}

#ifdef unix
	if (rqstp->rq_xprt->xp_port == getUDPPort())
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#else
//
// Retrieve the protocol used to send this request to the server/
//
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
	std::string tmpf(*fil_name);
	std::string query;
	query = "SELECT NAME FROM device WHERE ";
/*
 * replace * with mysql wildcard %
 */
#ifndef _solaris
	switch(count(tmpf.begin(), tmpf.end(), '/'))
#else
	switch(SQLite3Server::count(tmpf.begin(), tmpf.end(), '/'))
#endif /* !_solaris */
	{
		case 2 : 
			pos = tmpf.find('/');
			domain = tmpf.substr(0, pos);
			tmpf.erase(0, pos + 1);
			pos = tmpf.find('/');
			family = tmpf.substr(0, pos);
			member = tmpf.substr(pos + 1);
			if (domain != "*")
				query += " DOMAIN LIKE '" + domain + "' AND";
			if (family != "*")
				query += " FAMILY LIKE '" + family + "' AND";
			if (member != "*")
				query += " AND MEMBER LIKE '" + member + "' AND";
			break;
		case 1 : 
			pos = tmpf.find('/');
			domain = tmpf.substr(0, pos);	
			family = tmpf.substr(pos + 1);
			if (domain != "*")
				query += " DOMAIN LIKE '" + domain + "' AND";
			if (family != "*")
				query += " AND FAMILY LIKE '" + family + "' AND";
			break;
		case 0 : 
			domain = tmpf;		
			if (domain != "*")
				query += " DOMAIN LIKE '" + tmpf + "' AND";
			break;
		default: 
			std::cout << "To many '/' in device name." << std::endl;
			browse_back.db_err = 1;
			browse_back.res_val.arr1_len = 0;
			return (&browse_back);		 		 
	}
	query += (" EXPORTED != 0 AND IOR LIKE 'rpc:%'");
#ifdef DEBUG
	std::cout << "SQLite3Server::db_getdevexp_1_svc() : " << std::endl;
	std::cout << "filter domain : " << domain << std::endl;
	std::cout << "filter family : " << family << std::endl;
	std::cout << "filter member : " << member << std::endl;
	std::cout << "SQLite3Server::db_getdevexp_1_svc() : " << query << std::endl;
#endif 
//
// Try to retrieve all tuples in the database NAMES table with the PN column
// different than "not_exp" 
//

	browse_back.res_val.arr1_len = 0;
	browse_back.db_err = 0;
	if (sqlite3_get_table(db, query.c_str(), &result, &nrow, &ncol, &zErrMsg) != SQLITE_OK)
	{
//
// If a problem occurs during database function 
//
		std::cout << "SQLite3Server::db_getdevexp_1_svc() : " << sqlite3_errmsg(db) << std::endl;
    		browse_back.db_err = DbErr_DatabaseAccess;
    		return(&browse_back);
	}
//
// Store the key if it is needed later 
//
	dev_num = nrow;
	if (prot == IPPROTO_UDP && (dev_num > MAXDEV_UDP))
	{
		sqlite3_free_table(result);
		browse_back.db_err = DbErr_MaxDeviceForUDP;
		return (&browse_back);
	}
//
// Allocate memory for the pointer's array 
//
	int i = 0;
	int j = ncol;
	try
	{
		ptra = new nam[dev_num];
		for (i = 0; i < dev_num; i++, j += ncol)
		{
#ifdef DEBUG
			std::cout << "SQLite3Server::db_getdevexp_1_svc() : " << result[j] << std::endl;
#endif
			ptra[i] = new char[strlen(result[j]) + 1]; 
			strcpy(ptra[i], result[j]);
		}
	}
	catch(std::bad_alloc)
    	{
		sqlite3_free_table(result);
		for (int j = 0; j < i; j++)
			delete [] ptra[j];
		delete [] ptra;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
    	}
	sqlite3_free_table(result);
//
// Initialize the structure sended back to client and leave the server 
//
	browse_back.res_val.arr1_len = dev_num;
	browse_back.res_val.arr1_val = ptra;
	return(&browse_back);
}
