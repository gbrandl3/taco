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
 * Author(s):
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.10 $
 *
 * Date:	$Date: 2008-04-06 09:07:40 $
 *
 */

#ifndef __NDBMSERVER_H__
#define __NDBMSERVER_H__

#include "dbClass.h"
#include "NdbmClass.h"

/**@ingroup dbServerClasses
 */
class NdbmServer : public DBServer
{
private:
	std::vector<GDBM_FILE>	tid;
	u_short			prot;
	int			dev_num;
	char			**ptra;

private:
	int 	db_reinsert(arr1 *, DevString *,int);

	long 	upd_name(std::string, std::string, int) throw (long);
	long	upd_res(const std::string &lin, const long numb, bool array) throw (long);
	long 	del_name(device&, int&, std::string, std::vector<dena>&) throw (long);
	long 	is_dev_in_db(db_dev_in_db *,long) throw (long);
	long 	update_dev_list(device&, long) throw (long);

	bool	stringOK(std::string, std::string);
	char 	*fam_fil(device*, const std::string&, const std::string&, int) throw (int);
	char 	*memb_fil(device *, const std::string&, int) throw (int);

	long 	reg_ps(char *,long,char *,long,long *);
	long 	unreg_ps(char *,long *);

	int	db_find(const std::string, const std::string, DevString *) throw (int);
	int	db_devlist(std::string, db_res *);
	int	db_insert(std::string, std::string, std::string);                                                        
	int 	db_del(std::string, DevString *);
	int 	db_store_3(db_devinfo_3 &);
	int 	db_store_2(db_devinfo_2 &);
	int 	db_store(db_devinfo &);
	void 	delete_res(const std::string &);
	long 	cmp_nocase(const std::string &, const std::string &);

	void	leave(void);

public:
    				NdbmServer(const std::string user, const std::string passwd, const std::string db);
	virtual			~NdbmServer() {this->dbgen.connected = false;}
	virtual db_res 		*db_getres_1_svc(arr1 *, struct svc_req *);
	virtual db_res 		*db_getdev_1_svc(DevString *);
	virtual DevLong		*db_devexp_1_svc(tab_dbdev *);
	virtual DevLong	   	*db_devexp_2_svc(tab_dbdev_2 *);
	virtual DevLong	   	*db_devexp_3_svc(tab_dbdev_3 *);
	virtual db_resimp	*db_devimp_1_svc(arr1 *);
	virtual DevLong		*db_svcunr_1_svc(DevString *);
	virtual svc_inf		*db_svcchk_1_svc(DevString *);
	virtual db_res		*db_getdevexp_1_svc(DevString *, struct svc_req *);
	virtual DevLong		*db_clodb_1_svc(void);
	virtual DevLong		*db_reopendb_1_svc(void);
	virtual DevLong		*db_putres_1_svc(tab_putres *);
	virtual DevLong		*db_delres_1_svc(arr1 */*, struct svc_req **/);
	virtual cmd_que		*db_cmd_query_1_svc(DevString *);
	virtual event_que	*db_event_query_1_svc(DevString *);
	virtual db_psdev_error	*db_psdev_reg_1_svc(psdev_reg_x *);
	virtual db_psdev_error	*db_psdev_unreg_1_svc(arr1 *);
	virtual db_res		*devdomainlist_1_svc(void);
	virtual db_res		*devfamilylist_1_svc(DevString *);
	virtual db_res		*devmemberlist_1_svc(db_res *);
	virtual db_res		*resdomainlist_1_svc(void);
	virtual db_res		*resfamilylist_1_svc(DevString *);
	virtual db_res		*resmemberlist_1_svc(db_res *);
	virtual db_res		*resresolist_1_svc(db_res *);
	virtual db_res		*resresoval_1_svc(db_res *);
	virtual db_res		*devserverlist_1_svc(void);
	virtual db_res		*devpersnamelist_1_svc(DevString *);
	virtual db_res		*hostlist_1_svc(void);
	virtual db_devinfo_svc	*devinfo_1_svc(DevString *);
	virtual db_res		*devres_1_svc(db_res *);
	virtual DevLong		*devdel_1_svc(DevString *);
	virtual db_psdev_error	*devdelres_1_svc(db_res *);
	virtual db_info_svc	*info_1_svc();
	virtual DevLong		*unreg_1_svc(db_res *);
	virtual svcinfo_svc	*svcinfo_1_svc(db_res *);
	virtual DevLong		*svcdelete_1_svc(db_res *);
	virtual db_psdev_error	*upddev_1_svc(db_res *);
	virtual db_psdev_error	*updres_1_svc (db_res *);
	virtual db_poller_svc	*getpoller_1_svc(DevString *);

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

};

#endif
