#ifndef	__DB_CLASS__H__
#define	__DB_CLASS__H__
#ifdef sun
#define PORTMAP
#endif

#include <API.h>
#include <private/ApiP.h>
#include "db_xdr.h"
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error Could not find signal.h
#endif
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <rpc/pmap_clnt.h>

// C++ include
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#ifdef sun
#include <new>
#endif

// Special for database 
#include <fcntl.h>

#include <NdbmClass.h>

/**@ingroup dbServerClasses
 */
class DBServer
{
private:
	u_short 		udp_port;
	u_short 		tcp_port;
	u_long			pgnum;

protected:
	DevLong			errcode;
	NdbmInfo 		dbgen;
	long			ps_names_index;

	db_res 			browse_back;
	db_resimp 		back;
	db_devinfo_svc 		sent_back;
	db_info_svc 		info_back;
	db_poller_svc 		poll_back;
	svcinfo_svc 		svcinfo_back;
	db_psdev_error		psdev_back;
	cmd_que			cmd_queue;
	event_que		event_queue;
	svc_inf			svc_info;
	std::vector<NdbmDomain> 	dom_list;
	std::vector<NdbmDomain> 	res_list;
	std::vector<std::string> 	res_list_dev;

	std::ofstream		m_logStream;

public:
	void	setPgNum(const u_long pNum)  {pgnum = pNum;};
	u_long	getPgNum(void) {return pgnum;};
	void	setUDPPort(const u_short udp) {udp_port = udp;};
	u_short	getUDPPort(void) {return udp_port;}; 
	void	setTCPPort(const u_short tcp) {tcp_port = tcp;};
	u_short	getTCPPort(void) {return tcp_port;}; 

	void	setLogstream(const std::ofstream &log) 
	{
		std::cout.rdbuf(log.rdbuf());
		std::cerr.rdbuf(log.rdbuf());
	}

public:
	virtual db_res 		*db_getres_1_svc(arr1 *, struct svc_req *) = 0;
	virtual db_res 		*db_getdev_1_svc(nam *) = 0;
	virtual DevLong		*db_devexp_1_svc(tab_dbdev *) = 0;
	virtual DevLong	   	*db_devexp_2_svc(tab_dbdev_2 *) = 0;
	virtual DevLong	   	*db_devexp_3_svc(tab_dbdev_3 *) = 0;
	virtual db_resimp	*db_devimp_1_svc(arr1 *) = 0;
	virtual DevLong		*db_svcunr_1_svc(nam *) = 0;
	virtual svc_inf		*db_svcchk_1_svc(nam *) = 0;
	virtual db_res		*db_getdevexp_1_svc(nam *, struct svc_req *) = 0;
	virtual DevLong		*db_clodb_1_svc(void) = 0;
	virtual DevLong		*db_reopendb_1_svc(void) = 0;
	virtual DevLong		*db_putres_1_svc(tab_putres *) = 0;
	virtual DevLong		*db_delres_1_svc(arr1 */*, struct svc_req **/) = 0;
	virtual cmd_que		*db_cmd_query_1_svc(nam *) = 0;
	virtual event_que	*db_event_query_1_svc(nam *) = 0;
	virtual db_psdev_error	*db_psdev_reg_1_svc(psdev_reg_x *) = 0;
	virtual db_psdev_error	*db_psdev_unreg_1_svc(arr1 *) = 0;
	virtual db_res		*devdomainlist_1_svc(void) = 0;
	virtual db_res		*devfamilylist_1_svc(nam *) = 0;
	virtual db_res		*devmemberlist_1_svc(db_res *) = 0;
	virtual db_res		*resdomainlist_1_svc(void) = 0;
	virtual db_res		*resfamilylist_1_svc(nam *) = 0;
	virtual db_res		*resmemberlist_1_svc(db_res *) = 0;
	virtual db_res		*resresolist_1_svc(db_res *) = 0;
	virtual db_res		*resresoval_1_svc(db_res *) = 0;
	virtual db_res		*devserverlist_1_svc(void) = 0;
	virtual db_res		*devpersnamelist_1_svc(nam *) = 0;
	virtual db_res		*hostlist_1_svc(void) = 0;
	virtual db_devinfo_svc	*devinfo_1_svc(nam *) = 0;
	virtual db_res		*devres_1_svc(db_res *) = 0;
	virtual DevLong		*devdel_1_svc(nam *) = 0;
	virtual db_psdev_error	*devdelres_1_svc(db_res *) = 0;
	virtual db_info_svc	*info_1_svc() = 0;
	virtual DevLong		*unreg_1_svc(db_res *) = 0;
	virtual svcinfo_svc	*svcinfo_1_svc(db_res *) = 0;
	virtual long		*svcdelete_1_svc(db_res *) = 0;
	virtual db_psdev_error	*upddev_1_svc(db_res *) = 0;
	virtual db_psdev_error	*updres_1_svc (db_res *) = 0;
		db_res		*secpass_1_svc(void);
	virtual db_poller_svc	*getpoller_1_svc(nam *) = 0;

public:
	static  int 		make_sec(int c) {return (c == '|') ? SEC_SEP : c;}
	static  int 		make_unsec(int c) {return (c == SEC_SEP) ? '|' : c;}

public:
   			DBServer();
	virtual 	~DBServer() {};
};

#endif
