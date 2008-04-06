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
 * File:	Sqlite3Server.h
 * 
 * Description:
 *
 * Author(s):
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:40 $
 *
 */

#ifndef __SQLITE3SERVER_H__
#define __SQLITE3SERVER_H__

#ifdef sun
#define PORTMAP
#endif
#include "config.h"

#include <API.h>
#include <cstdlib>
#include <private/ApiP.h>
#include <db_xdr.h>
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#	error Could not find signal.h
#endif
#include <errno.h>
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#	include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif
#if HAVE_NETDB_H
#	include <netdb.h>
#endif
#include <rpc/pmap_clnt.h>

#ifdef HAVE_SQLITE3_H
#	include <sqlite3.h>
#endif

// C++ include
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef sun
#	include <new>
#endif

// Special for database 
#if HAVE_FCNTL_H
#	include <fcntl.h>
#endif

#include "dbClass.h"

/**@ingroup dbServerClasses
 */
class SQLite3Server : public DBServer
{
private:
	char 		*zErrMsg;
	char 		**result;
	int 		rc;
	int 		nrow,
			ncol;
	sqlite3 	*db;
	std::string	m_table;

/*
 * to fix broken std::count() on Solaris CC
 */
#ifdef _solaris
        template <class Iterator, class T>
        //typename std::iterator_traits<Iterator>::difference_type
        int
        count (Iterator first, Iterator last, T const & value)
        {
                //std::iterator_traits<Iterator>::difference_type n = 0;
                int n = 0;
                while (first != last)
                        if (*first++ == value) ++n;
                return n;
	}
#endif

private:
	int	db_find(std::string, std::string, char **, int *);
	int	db_devlist(std::string, int *, db_res *);
	int	db_del(std::string);
	int	db_insert(std::string, std::string, std::string);
	int 	db_store(db_devinfo_3 &);
	int 	db_store(db_devinfo_2 &);
	int 	db_store(db_devinfo &);
	long	reg_ps(std::string, long, std::string, long, DevLong *);
	long	unreg_ps(std::string, long *);
	long	db_update_names(const std::string, const std::string, const int, const std::string); 
	long	db_insert_names(const std::string, const std::string, const int, const std::string); 
	long	db_delete_names(const std::string, const std::string, const int, const std::string); 
	long	upd_res(std::string, long, char, DevLong *);
public:
    			SQLite3Server(std::string);
	virtual		~SQLite3Server();
	db_res 		*db_getres_1_svc(arr1 *, struct svc_req *);
	db_res 		*db_getdev_1_svc(DevString *);
	DevLong		*db_devexp_1_svc(tab_dbdev *);
	DevLong	   	*db_devexp_2_svc(tab_dbdev_2 *);
	DevLong	   	*db_devexp_3_svc(tab_dbdev_3 *);
	db_resimp	*db_devimp_1_svc(arr1 *);
	DevLong		*db_svcunr_1_svc(DevString *);
	svc_inf		*db_svcchk_1_svc(DevString *);
	db_res		*db_getdevexp_1_svc(DevString *, struct svc_req *);
	DevLong		*db_clodb_1_svc(void);
	DevLong		*db_reopendb_1_svc(void);
	DevLong		*db_putres_1_svc(tab_putres *);
	DevLong		*db_delres_1_svc(arr1 */*, struct svc_req **/);
	cmd_que		*db_cmd_query_1_svc(DevString *);
	event_que	*db_event_query_1_svc(DevString *);
	db_psdev_error	*db_psdev_reg_1_svc(psdev_reg_x *);
	db_psdev_error	*db_psdev_unreg_1_svc(arr1 *);
	db_res		*devdomainlist_1_svc(void);
	db_res		*devfamilylist_1_svc(DevString *);
	db_res		*devmemberlist_1_svc(db_res *);
	db_res		*resdomainlist_1_svc(void);
	db_res		*resfamilylist_1_svc(DevString *);
	db_res		*resmemberlist_1_svc(db_res *);
	db_res		*resresolist_1_svc(db_res *);
	db_res		*resresoval_1_svc(db_res *);
	db_res		*devserverlist_1_svc(void);
	db_res		*devpersnamelist_1_svc(DevString *);
	db_res		*hostlist_1_svc(void);
	db_devinfo_svc	*devinfo_1_svc(DevString *);
	db_res		*devres_1_svc(db_res *);
	DevLong		*devdel_1_svc(DevString *);
	db_psdev_error	*devdelres_1_svc(db_res *);
	db_info_svc	*info_1_svc();
	DevLong		*unreg_1_svc(db_res *);
	svcinfo_svc	*svcinfo_1_svc(db_res *);
	DevLong		*svcdelete_1_svc(db_res *);
	db_psdev_error	*upddev_1_svc(db_res *);
	db_psdev_error	*updres_1_svc (db_res *);
	db_poller_svc	*getpoller_1_svc(DevString *);
};

inline SQLite3Server::SQLite3Server(const std::string tablename)
	: DBServer(), zErrMsg(0), rc(0)
{
//
// Open database tables according to the definition 
//
	char *ptr = (char *)getenv("DBM_DIR");
        if (!ptr) 
        {
                std::cerr << "dbm_server: Can't find environment variable DBM_DIR" << std::endl;
                dbgen.connected = false;
		return;
        }

        std::string dir_name(ptr);
        if (dir_name[dir_name.size() - 1] != '/')
                dir_name.append(1,'/');
	m_table = dir_name + tablename; 
	if (*this->db_reopendb_1_svc() != 0)
		return;
	dbgen.connected = true;
}

inline SQLite3Server::~SQLite3Server()
{
	sqlite3_close(db);
	dbgen.connected = false;
}

#endif
