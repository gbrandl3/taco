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
 * Author(s):
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.11 $
 *
 * Date:	$Date: 2006-12-15 12:43:53 $
 *
 */

#ifndef __MYSQLSERVER_H__
#define __MYSQLSERVER_H__

#include "dbClass.h"
#ifdef HAVE_MYSQL_MYSQL_H
#	include <mysql/mysql.h>
#else
#	include <mysql.h>
#endif

// logging variables

/**@ingroup dbServerClasses
 */
class MySQLServer : public DBServer
{
private:
    std::string	mysql_user,
		mysql_passwd,
		mysql_db;
    MYSQL	mysql,
		*mysql_conn;

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
    int		db_find(std::string, std::string, char **, int *);
    int		db_devlist(std::string, int *, db_res *);
    int		db_del(std::string);
    int		db_insert(std::string, std::string, std::string);                                                        
    int 	db_store(db_devinfo_3 &);
    int 	db_store(db_devinfo_2 &);
    int 	db_store(db_devinfo &);
    long	reg_ps(std::string, long, std::string, long, long *);
    long	unreg_ps(std::string, long *);
    long	db_update_names(const std::string, const std::string, const int, const std::string); 
    long	db_insert_names(const std::string, const std::string, const int, const std::string); 
    long	db_delete_names(const std::string, const std::string, const int, const std::string); 
    long	upd_res(std::string, long, char, long *);
public:
    				MySQLServer(std::string, std::string, std::string);
    virtual			~MySQLServer();
    virtual db_res 		*db_getres_1_svc(arr1 *, struct svc_req *);
    virtual db_res 		*db_getdev_1_svc(nam *);
    virtual DevLong		*db_devexp_1_svc(tab_dbdev *);
    virtual DevLong	   	*db_devexp_2_svc(tab_dbdev_2 *);
    virtual DevLong	   	*db_devexp_3_svc(tab_dbdev_3 *);
    virtual db_resimp		*db_devimp_1_svc(arr1 *);
    virtual DevLong		*db_svcunr_1_svc(nam *);
    virtual svc_inf		*db_svcchk_1_svc(nam *);
    virtual db_res		*db_getdevexp_1_svc(nam *, struct svc_req *);
    virtual DevLong		*db_clodb_1_svc(void);
    virtual DevLong		*db_reopendb_1_svc(void);
    virtual DevLong		*db_putres_1_svc(tab_putres *);
    virtual DevLong		*db_delres_1_svc(arr1 */*, struct svc_req **/);
    virtual cmd_que		*db_cmd_query_1_svc(nam *);
    virtual event_que		*db_event_query_1_svc(nam *);
    virtual db_psdev_error	*db_psdev_reg_1_svc(psdev_reg_x *);
    virtual db_psdev_error	*db_psdev_unreg_1_svc(arr1 *);
    virtual db_res		*devdomainlist_1_svc(void);
    virtual db_res		*devfamilylist_1_svc(nam *);
    virtual db_res		*devmemberlist_1_svc(db_res *);
    virtual db_res		*resdomainlist_1_svc(void);
    virtual db_res		*resfamilylist_1_svc(nam *);
    virtual db_res		*resmemberlist_1_svc(db_res *);
    virtual db_res		*resresolist_1_svc(db_res *);
    virtual db_res		*resresoval_1_svc(db_res *);
    virtual db_res		*devserverlist_1_svc(void);
    virtual db_res		*devpersnamelist_1_svc(nam *);
    virtual db_res		*hostlist_1_svc(void);
    virtual db_devinfo_svc	*devinfo_1_svc(nam *);
    virtual db_res		*devres_1_svc(db_res *);
    virtual DevLong		*devdel_1_svc(nam *);
    virtual db_psdev_error	*devdelres_1_svc(db_res *);
    virtual db_info_svc		*info_1_svc();
    virtual DevLong		*unreg_1_svc(db_res *);
    virtual svcinfo_svc		*svcinfo_1_svc(db_res *);
    virtual DevLong		*svcdelete_1_svc(db_res *);
    virtual db_psdev_error	*upddev_1_svc(db_res *);
    virtual db_psdev_error	*updres_1_svc (db_res *);
    virtual db_poller_svc	*getpoller_1_svc(nam *);
};

inline MySQLServer::MySQLServer(const std::string user, const std::string password, const std::string db)
	: DBServer(),
	  mysql_user(user),
	  mysql_passwd(password),
	  mysql_db(db),
	  mysql_conn(NULL)
{
//
// Open database tables according to the definition 
//
	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"client");
	if (*this->db_reopendb_1_svc() != 0)
		return;
	dbgen.connected = true;
}

inline MySQLServer::~MySQLServer()
{
	mysql_close(mysql_conn);
	dbgen.connected = false;
}

#endif
