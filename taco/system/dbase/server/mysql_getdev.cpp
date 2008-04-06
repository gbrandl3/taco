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
 *		mysql_getdev.cpp
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.15 $
 *
 * Date:	$Date: 2008-04-06 09:07:41 $
 *
 */

#include <DevErrors.h>
#include <MySqlServer.h>
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
db_res *MySQLServer::db_getdevexp_1_svc(DevString *fil_name,struct svc_req *rqstp)
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
    socklen_t  		so_size;   /* from POSIX draft - already used by GLIBC */
#else
    int 		so_size;
#endif
//
// If the server is not connected to the database, return an error 
//
    if (!dbgen.connected && (*db_reopendb_1_svc() != DS_OK))
    {
	logStream->errorStream() << "I'm not connected to database" << log4cpp::eol;
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
    query = "SELECT name FROM device WHERE ";
/*
 * replace * with mysql wildcard %
 */
    int index;

    while ((index = tmpf.find('*')) != std::string::npos)
    {
	    tmpf.replace(index, 1, 1, '%');
    }

#ifndef _solaris
    switch(count(tmpf.begin(), tmpf.end(), '/'))
#else
    switch(MySQLServer::count(tmpf.begin(), tmpf.end(), '/'))
#endif /* !_solaris */
    {
	case 2 : if( tmpf != "%/%/%" ) {	
		   pos = tmpf.find('/');
		   domain = tmpf.substr(0, pos);	
		   pos = tmpf.find('/', 1 + (last_pos = pos));
		   family = tmpf.substr(last_pos + 1, (pos - last_pos));
		   member = tmpf.substr(pos + 1);
		   query += (" name LIKE '" + tmpf + "' AND");
		 }
		 break;
	case 1 : pos = tmpf.find('/');
		 domain = tmpf.substr(0, pos);	
		 family = tmpf.substr(pos + 1);
		 query += (" CONCAT(DOMAIN, '/', FAMILY) LIKE '" + tmpf + "' AND");
		 break;
	case 0 : domain = tmpf;		
		 query += (" DOMAIN LIKE '" + tmpf + "' AND");
		 break;
	default: logStream->errorStream() << "To many '/' in device name." << log4cpp::eol;
		 browse_back.db_err = 1;
		 browse_back.res_val.arr1_len = 0;
		 return (&browse_back);		 		 
    }
    query += (" EXPORTED != 0 AND IOR LIKE 'rpc:%'");

    logStream->debugStream() << "filter domain : " << domain << log4cpp::eol;
    logStream->debugStream() << "filter family : " << family << log4cpp::eol;
    logStream->debugStream() << "filter member : " << member << log4cpp::eol;

//
// Try to retrieve all tuples in the database NAMES table with the PN column
// different than "not_exp" 
//

    browse_back.res_val.arr1_len = 0;
    browse_back.db_err = 0;
    if (mysql_query(mysql_conn, query.c_str()) == 0) 
    {
//
// Store the key if it is needed later 
//
	MYSQL_RES *result = mysql_store_result(mysql_conn);
	MYSQL_ROW row;
	dev_num = mysql_num_rows(result);
	if (prot == IPPROTO_UDP && (dev_num > MAXDEV_UDP))
	{
	    mysql_free_result(result);
	    browse_back.db_err = DbErr_MaxDeviceForUDP;
	    return (&browse_back);
	}
//
// Allocate memory for the pointer's array 
//
	int i = 0;
	try
	{
    	    ptra = new DevString[dev_num];
	    for (i = 0; i < dev_num && ((row = mysql_fetch_row(result)) != NULL); i++)
	    {
	    	ptra[i] = new char[strlen(row[0]) + 1]; 
	    	strcpy(ptra[i], row[0]);
	    }
	}
	catch(std::bad_alloc)
    	{
	    mysql_free_result(result);
	    for (int j = 0; j < i; j++)
		delete [] ptra[j];
	    delete [] ptra;
	    browse_back.db_err = DbErr_ServerMemoryAllocation;
	    return(&browse_back);
    	}
	mysql_free_result(result);
    } 
    else
    {
//
// If a problem occurs during database function 
//
    	browse_back.db_err = DbErr_DatabaseAccess;
    	return(&browse_back);
    }
//
// Initialize the structure sended back to client and leave the server 
//
    browse_back.res_val.arr1_len = dev_num;
    browse_back.res_val.arr1_val = ptra;
    return(&browse_back);
}
