#ifndef __NDBMSERVER_H__
#define __NDBMSERVER_H__

#ifdef sun
#define PORTMAP
#endif

#include "dbClass.h"

class NdbmServer : public DBServer
{
private:
    vector<DBM>	tid;
//    long        ps_names_index;
private:
    int		db_find(const string, const string, char **) throw (int);
    int		db_devlist(string, db_res *);
    int		db_del(string);
    int		db_insert(string, string, string);                                                        
    int 	db_del(string, char **);
    int 	db_reinsert(arr1 *, char **, int);
    int 	db_store_3(db_devinfo_3 &);
    int 	db_store_2(db_devinfo_2 &);
    int 	db_store(db_devinfo &);
    char 	*fam_fil(device *, const string &, const string &, int) throw (int);
    char 	*memb_fil(device *, const string &, int) throw (int);
    bool 	stringOK(const string, const string);
    void 	delete_res(const string);
    bool 	cmp_nocase(const string, const string);
    long 	reg_ps(const string &,long, const string &,long) throw (long);
    long 	unreg_ps(const string &) throw (long);
    long 	upd_name(string , string, int, long) throw (long);
    long 	del_name(device &,int &, string, vector<dena> &) throw (long);
    long 	is_dev_in_db(db_dev_in_db *,long) throw (long);
    long 	update_dev_list(device &,int) throw (long);
    long 	upd_res(const string &, long, bool) throw (long);
    void	leave(void);
public:
    				NdbmServer(const string user, const string passwd, const string db);
    virtual			~NdbmServer();
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
    virtual db_res		*secpass_1_svc(void);
    virtual db_poller_svc	*getpoller_1_svc(nam *);
};

inline NdbmServer::~NdbmServer()
{
    this->dbgen.connected = false;
}

#endif
