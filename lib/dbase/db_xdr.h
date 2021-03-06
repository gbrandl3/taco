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
 * File:	db_xdr.h
 *
 * Description:
 *		
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Version:	$Version$
 *
 * Date:		$Date: 2008-04-06 09:07:06 $
 *  
 ******************************************************************************/

#ifndef _setacc
#define _setacc

#include <macros.h>
#include <db_setup.h>
#include <dev_xdr.h>

#ifdef __STDCPP__
#include <vector>
#include <string>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef char *nam;
#endif

bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_nam(XDR *, DevString *);
#else
 xdr_nam();
#endif


struct arr1 {
	u_int arr1_len;	/**! The number of strings */
	DevString *arr1_val;	/**! A pointer to the array of strings */
};
typedef struct arr1 arr1;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_arr1(XDR *, arr1 *);
#else
 xdr_arr1();
#endif	/* __STDC__ */

/* For db_dev_export version 1 */

struct db_devinfo {
/** The device name */
	DevString 	dev_name;
/** The host name */
	DevString 	host_name;
/** The device type */
	DevString 	dev_type;
/** The device class */
	DevString 	dev_class;
/** The program number */
	u_int 		p_num;
/** The version number */
	u_int 		v_num;
};
typedef struct db_devinfo db_devinfo;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo(XDR *, db_devinfo *);
#else
 xdr_db_devinfo();
#endif	/* __STDC__ */


typedef struct {
/** The number of structures db_devinfo */
	u_int 		tab_dbdev_len;
/** A pointer to the array of structure db_devinfo */
	db_devinfo 	*tab_dbdev_val;
} tab_dbdev;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev(XDR *, tab_dbdev *);
#else
 xdr_tab_dbdev();
#endif	/* __STDC__ */

/* For db_dev_export version 2 */

struct db_devinfo_2 {
/** The device name */
	DevString	dev_name;
/** The host name */
	DevString	host_name;
/** The device type */
	DevString	dev_type;
/** The device class */
	DevString	dev_class;
/** The program number */
	u_int 	p_num;
/** The version number */
	u_int 	v_num;
/** The process ID */
	u_int 	pid;
};
typedef struct db_devinfo_2 db_devinfo_2;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo_2(XDR *, db_devinfo_2 *);
#else
 xdr_db_devinfo_2();
#endif	/* __STDC__ */


typedef struct {
/** The number of structures db_devinfo_2 */
	u_int 		tab_dbdev_len;
/** A pointer to the array of structure db_devinfo_2 */
	db_devinfo_2 	*tab_dbdev_val;
} tab_dbdev_2;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev_2(XDR *, tab_dbdev_2 *);
#else
 xdr_tab_dbdev_2();
#endif	/* __STDC__ */

/* For db_dev_export version 3 */

struct db_devinfo_3 {
/** The host name */
	DevString 	dev_name;
/** The device type */
	DevString 	host_name;
/** The device type */
	DevString 	dev_type;
/** The device class */
	DevString 	dev_class;
/** The program number */
	u_int 	p_num;
/** The version number */
	u_int 	v_num;
/** The process ID */
	u_int 	pid;
/** The process name */
	DevString 	proc_name;
};
typedef struct db_devinfo_3 db_devinfo_3;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo_3(XDR *, db_devinfo_3 *);
#else
 xdr_db_devinfo_3();
#endif	/* __STDC__ */


typedef struct {
/** The number of structures db_devinfo_3 */
	u_int 		tab_dbdev_len;
/** A pointer to the array of structure db_devinfo_3 */
	db_devinfo_3 	*tab_dbdev_val;
} tab_dbdev_3;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev_3(XDR *, tab_dbdev_3 *);
#else
 xdr_tab_dbdev_3();
#endif	/* __STDC__ */




struct db_res {
	arr1 res_val;	/**  A structure of the arr1 type (see above) with the 
			resources values information transferred as strings */
	int db_err;	/** The database error code 0 if no error */
};
typedef struct db_res db_res;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_res(XDR *, db_res *);
#else
 xdr_db_res();
#endif	/* __STDC__ */


struct db_resimp {
/** A structure of the tab_dbdev type with the information needed 
 (host_name, program number and version number) */
	tab_dbdev 	imp_dev;
/** The database error code (0 if no error occured) */
	int 		db_imperr;
};
typedef struct db_resimp db_resimp;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_resimp(XDR *, db_resimp *);
#else
 xdr_db_resimp();
#endif	/* __STDC__ */


struct svc_inf {
/** the host name*/
	DevString	ho_name;
/** the program number */
	u_int 	p_num;
/** the version number */
	u_int 	v_num;
/** database access error code (if 0 no error occured) */
	int 	db_err;
};
typedef struct svc_inf svc_inf;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_svc_inf(XDR *, svc_inf *);
#else
 xdr_svc_inf();
#endif	/* __STDC__ */


struct putres {
	DevString res_name;
	DevString res_val;
};
typedef struct putres putres;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_putres(XDR *, putres *);
#else
 xdr_putres();
#endif	/* __STDC__ */


typedef struct {
/** The number of resources to be updated or inserted */
	u_int 	tab_putres_len;
/** A pointer to an array of putres structure. Each putres structure 
    contains the resource name and the resource value */
	putres 	*tab_putres_val;
}tab_putres;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_putres(XDR *, tab_putres *);
#else
 xdr_tab_putres();
#endif	/* __STDC__ */

struct cmd_que{
/** The database error code 0 if no error */
	int db_err;
/** The command code. 0 if the database query fails */
	u_int xcmd_code;
};
typedef struct cmd_que cmd_que;
/* for event querys */
struct event_que{
/** The database error code 0 if no error */
	int db_err;
/** The event code. 0 if the database query fails */
	u_int xevent_code;
};
typedef struct event_que event_que;

bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_cmd_que(XDR *, cmd_que *);
#else
 xdr_cmd_que();
#endif	/* __STDC__ */

/* for event querys*/
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_event_que(XDR *, event_que *);
#else
 xdr_event_que();
#endif	/* __STDC__ */

/* Added definition for the db_freedevexp function */

typedef struct {
	db_res res;
	DevLong tcp;
	DevLong tcp_so;
	CLIENT  *cl;
	}devexp_res;

/* Added definition for db_psdev_register function */

struct psdev_elt {
	DevString 	psdev_name;
	DevLong 	poll;
};
typedef struct psdev_elt psdev_elt;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_psdev_elt(XDR *, psdev_elt *);
#else
 xdr_psdev_elt();
#endif	/* __STDC__ */


struct psdev_reg_x {
	u_int pid;
	DevString h_name;
	struct {
		u_int psdev_arr_len;
		struct psdev_elt *psdev_arr_val;
	} psdev_arr;
};
typedef struct psdev_reg_x psdev_reg_x;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_psdev_reg_x(XDR *, psdev_reg_x *);
#else
 xdr_psdev_reg_x();
#endif	/* __STDC__ */

struct db_psdev_error {
	DevLong error_code;
	DevLong psdev_err;
};
typedef struct db_psdev_error db_psdev_error;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_db_psdev_error(XDR *, db_psdev_error *);
#else
 xdr_db_psdev_error();
#endif	/* __STDC__ */


struct db_devinfo_svc {
	DevLong 	device_type;
	DevLong 	device_exported;
	DevString	device_class;
	DevString	server_name;
	DevString 	personal_name;
	DevString 	process_name;
	DevULong 	server_version;
	DevString 	host_name;
	DevULong 	pid;
	DevULong 	program_num;
	DevLong 	db_err;
};
typedef struct db_devinfo_svc db_devinfo_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo_svc(XDR *, db_devinfo_svc *);
#else
xdr_db_devinfo_svc();
#endif /* __STDC__ */


struct db_info_dom_svc {
	DevString	dom_name;
	DevLong		dom_elt;
};
typedef struct db_info_dom_svc db_info_dom_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_info_dom_svc(XDR *, db_info_dom_svc *);
#else
xdr_db_info_dom_svc();
#endif /* __STDC__ */



struct var_dom {
	u_int		dom_len;
	db_info_dom_svc	*dom_val;
};
typedef struct var_dom var_dom;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_var_dom(XDR *, var_dom *);
#else
xdr_var_dom();
#endif /* __STDC__ */



struct db_info_svc {
	DevLong		dev_defined;
	DevLong		dev_exported;
	DevLong		psdev_defined;
	DevLong		res_number;
	var_dom		dev;
	var_dom		res;
	DevLong		db_err;
};
typedef struct db_info_svc db_info_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_info_svc(XDR *, db_info_svc *);
#else
xdr_db_info_svc();
#endif /* __STDC__ */


struct svcinfo_dev {
	DevLong		exported_flag;
	DevString	name;
};
typedef struct svcinfo_dev svcinfo_dev;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_dev(XDR *, svcinfo_dev *);
#else
xdr_svcinfo_dev();
#endif /* __STDC__ */


struct svcinfo_server {
	DevString	server_name;
	DevLong		dev_len;
	svcinfo_dev	*dev_val;
};
typedef struct svcinfo_server svcinfo_server;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_server(XDR *, svcinfo_server *);
#else
xdr_svcinfo_server();
#endif /* __STDC__ */



struct svcinfo_svc {
	DevString	process_name;
	DevULong	pid;
	DevULong	program_num;
	DevString	host_name;
	DevLong		embedded_len;
	svcinfo_server	*embedded_val;
	DevLong 	db_err;
};
typedef struct svcinfo_svc svcinfo_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_svc(XDR *, svcinfo_svc *);
#else
xdr_svcinfo_svc();
#endif /* __STDC__ */


struct db_poller_svc {
	DevString	server_name;
	DevString	personal_name;
	DevString	host_name;
	DevString 	process_name;
	DevULong	pid;
	DevLong 	db_err;
};
typedef struct db_poller_svc db_poller_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_poller_svc(XDR *, db_poller_svc *);
#else
xdr_db_poller_svc();
#endif /* _STDC__ */


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_delupd_error(XDR *, db_delupd_error *);
#else
xdr_db_delupd_error();
#endif /* _STDC__ */


struct db_arr1_array {
	arr1   	*arr;
	int    	arr_length;
	int 	arr_type;
};
typedef struct db_arr1_array db_arr1_array;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_arr1_array(XDR *, db_arr1_array *);
#else
 xdr_db_arr1_array();
#endif	/* __STDC__ */



struct db_svc_net {
	DevString	server_name;
	DevString	personal_name;
	DevString	host_name;
	DevULong	pid;
	DevULong	program_num;
};
typedef struct db_svc_net db_svc_net;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_svc_net(XDR *, db_svc_net *);
#else
 xdr_db_svc_net();
#endif	/* __STDC__ */



struct db_svcarray_net {
	u_int		length;
	db_svc_net	*sequence;
	DevLong		db_err;
};
typedef struct db_svcarray_net db_svcarray_net;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_svcarray_net(XDR *, db_svcarray_net *);
#else
 xdr_db_svcarray_net();
#endif	/* __STDC__ */


struct ana_input{
	DevLong		in_type;
	DevString	*buf;
	FILE		*f;
};
typedef struct ana_input ana_input;


/* Miscellaneous function */

int to_reconnection(void *,void **,CLIENT **,int,long,long, DevLong *);
void kern_sort(char **,int);
#ifdef __STDCPP__
long get_tango_exp_devices(char *,vector<string> &, DevLong *);
#endif

/* Define some constantes */

#ifdef ALONE
#define DB_SETUPPROG ((DevULong)21000023)
#endif

#define DB_GETRES ((DevULong)1)
#define DB_GETDEV ((DevULong)2)
#define DB_DEVEXP ((DevULong)3)
#define DB_DEVIMP ((DevULong)4)
#define DB_SVCUNR ((DevULong)5)
#define DB_SVCCHK ((DevULong)6)
#define DB_GETDEVEXP ((DevULong)7)
#define DB_PUTRES ((DevULong)10)
#define DB_DELRES ((DevULong)11)
#define DB_CMDQUERY ((DevULong)12)
#define DB_PSDEV_REG ((DevULong)13)
#define DB_PSDEV_UNREG ((DevULong)14)
#define DB_GETDEVDOMAIN ((DevULong)15)
#define DB_GETDEVFAMILY ((DevULong)16)
#define DB_GETDEVMEMBER ((DevULong)17)
#define DB_GETRESDOMAIN ((DevULong)19)
#define DB_GETRESFAMILY ((DevULong)20)
#define DB_GETRESMEMBER ((DevULong)21)
#define DB_GETRESRESO ((DevULong)22)
#define DB_GETRESRESOVAL ((DevULong)23)
#define DB_GETSERVER ((DevULong)24)
#define DB_GETPERS ((DevULong)25)
#define DB_GETHOST ((DevULong)26)
#define DB_DEVINFO ((DevULong)27)
#define DB_DEVRES ((DevULong)28)
#define DB_DEVDEL ((DevULong)29)
#define DB_DEVDELALLRES ((DevULong)30)
#define DB_INFO ((DevULong)31)
#define DB_SVCUNREG ((DevULong)32)
#define DB_SVCINFO ((DevULong)33)
#define DB_SVCDELETE ((DevULong)34)
#define DB_UPDDEV ((DevULong)35)
#define DB_UPDRES ((DevULong)36)
#define DB_SECPASS ((DevULong)37)
#define DB_GETPOLLER ((DevULong)38)
#define DB_DEL_UPDATE ((DevULong)39)
#define DB_INITCACHE ((DevULong)40)
#define DB_GETDSHOST ((DevULong)41)
#define DB_EVENTQUERY ((DevULong)42)

/* For ndbm only */
#define DB_CLODB ((DevULong)8)
#define DB_REOPENDB ((DevULong)9)
/* For to_reconnection */
#define DB_DEVEXP_2 ((DevULong)1002)
#define DB_DEVEXP_3 ((DevULong)1003)

#if defined __STDC__ | defined __STDCPP__

/* Client call to server */

extern db_res  * db_getres_1(arr1 *, CLIENT *, DevLong *);
extern db_res  * db_getdev_1(DevString *, CLIENT *, DevLong *);
extern int * db_devexp_1(tab_dbdev *, CLIENT *, DevLong *);
extern int * db_devexp_2(tab_dbdev_2 *, CLIENT *, DevLong *);
extern int * db_devexp_3(tab_dbdev_3 *, CLIENT *, DevLong *);
extern db_resimp * db_devimp_1(arr1 *, CLIENT *, DevLong *);
extern int * db_svcunr_1(DevString *, CLIENT *, DevLong *);
extern svc_inf * db_svcchk_1(DevString *, CLIENT *, DevLong *);
extern db_res * db_getdevexp_1(DevString *, CLIENT *, DevLong *);
extern int * db_putres_1(tab_putres *, CLIENT *, DevLong *);
extern int * db_delres_1(arr1 *, CLIENT *, DevLong *);
extern cmd_que * db_cmd_query_1(DevString *, CLIENT *, DevLong *);
/*event query*/
extern event_que * db_event_query_1(DevString *, CLIENT *, DevLong *);
extern db_psdev_error * db_psdev_reg_1(psdev_reg_x *, CLIENT *, DevLong *);
extern db_psdev_error * db_psdev_unreg_1(arr1 *, CLIENT *, DevLong *);
extern long db_null_proc_1(CLIENT *clnt, DevLong *perr);

extern db_res * db_getdevdomain_1(CLIENT *,DevLong *);
extern db_res * db_getdevfamily_1(DevString *,CLIENT *,DevLong *);
extern db_res * db_getdevmember_1(db_res *,CLIENT *,DevLong *);
extern db_res * db_getresdomain_1(CLIENT *,DevLong *);
extern db_res * db_getresfamily_1(DevString *,CLIENT *,DevLong *);
extern db_res * db_getresmember_1(db_res *,CLIENT *,DevLong *);
extern db_res * db_getresreso_1(db_res *,CLIENT *,DevLong *);
extern db_res * db_getresresoval_1(db_res *,CLIENT *,DevLong *);
extern db_res * db_getdsserver_1(CLIENT *,DevLong *);
extern db_res * db_getdspers_1(DevString *,CLIENT *,DevLong *);
extern db_res * db_gethost_1(CLIENT *,DevLong *);
extern db_svcarray_net * db_getdsonhost_1(DevString *,CLIENT *,DevLong *);

extern db_devinfo_svc * db_deviceinfo_1(DevString *,CLIENT *,DevLong *);
extern db_res * db_deviceres_1(db_res *,CLIENT *,DevLong *);
extern long *db_devicedelete_1(DevString *,CLIENT *,DevLong *);
extern db_psdev_error *db_devicedeleteres_1(db_res *,CLIENT *,DevLong *);
extern db_info_svc * db_stat_1(CLIENT *,DevLong *);
extern long *db_servunreg_1(db_res *,CLIENT *,DevLong *);
extern svcinfo_svc *db_servinfo_1(db_res *,CLIENT *,DevLong *);
extern long *db_servdelete_1(db_res *,CLIENT *,DevLong *);
extern db_poller_svc *db_getpoll_1(DevString *,CLIENT *,DevLong *);
extern long *db_initcache_1(DevString *,CLIENT *,DevLong *);

extern db_psdev_error *db_upddev_1(db_res *,CLIENT *,DevLong *);
extern db_psdev_error *db_updres_1(db_res *,CLIENT *,DevLong *);
extern db_res *db_secpass_1(CLIENT *,DevLong *);
extern db_delupd_error *db_delete_update_1(db_arr1_array *,CLIENT *,DevLong *);

extern int * db_clodb_1(CLIENT *, DevLong *);
extern int * db_reopendb_1(CLIENT *, DevLong *);

/* Server functions */

db_res *db_getres_1_svc(arr1 *,struct svc_req *);
db_res *db_getdev_1_svc(DevString *);
int *db_putres_1_svc(tab_putres *);
#if 0
int *db_delres_1_svc(arr1 * /* ,struct svc_req * */);
#endif
int *db_delres_1_svc(arr1 *,struct svc_req *);

cmd_que *db_cmd_query_1_svc(DevString *);
event_que *db_event_query_1_svc(DevString *);
db_res *db_getdevexp_1_svc(DevString *,struct svc_req *);
int *db_devexp_1_svc(tab_dbdev *);
int *db_devexp_2_svc(tab_dbdev_2 *);
int *db_devexp_3_svc(tab_dbdev_3 *);
db_resimp *db_devimp_1_svc(arr1 *);
int *db_svcunr_1_svc(DevString *);
svc_inf *db_svcchk_1_svc(DevString *);
db_psdev_error *db_psdev_reg_1_svc(psdev_reg_x *);
db_psdev_error *db_psdev_unreg_1_svc(arr1 *);

db_res *devdomainlist_1_svc(void);
db_res *devfamilylist_1_svc(DevString *);
db_res *devmemberlist_1_svc(db_res *);
db_res *resdomainlist_1_svc(void);
db_res *resfamilylist_1_svc(DevString *);
db_res *resmemberlist_1_svc(db_res *);
db_res *resresolist_1_svc(db_res *);
db_res *resresoval_1_svc(db_res *);
db_res *devserverlist_1_svc(void);
db_res *devpersnamelist_1_svc(DevString *);
db_res *hostlist_1_svc(void);
db_svcarray_net *getdsonhost_1_svc(DevString *);

db_devinfo_svc *devinfo_1_svc(DevString *);
db_res *devres_1_svc(db_res *);
DevLong *devdel_1_svc(DevString *);
db_psdev_error *devdelres_1_svc(db_res *);
db_info_svc *info_1_svc(void);
DevLong *unreg_1_svc(db_res *);
svcinfo_svc *svcinfo_1_svc(db_res *);
DevLong *svcdelete_1_svc(db_res *);
db_poller_svc *getpoller_1_svc(DevString *);
DevLong *initcache_1_svc(DevString *);

db_psdev_error *upddev_1_svc(db_res *);
db_psdev_error *updres_1_svc(db_res *);
db_res *secpass_1_svc(void);
db_delupd_error *delete_update_1_svc(db_arr1_array *);


int *db_clodb_1_svc();
int *db_reopendb_1_svc();

#else
extern db_res *db_getres_1();
extern db_res *db_getdev_1();
extern int  *db_devexp_1();
extern int  *db_devexp_2();
extern int  *db_devexp_3();
extern db_resimp  *db_devimp_1();
extern int *db_svcunr_1();
extern svc_inf *db_svcchk_1();
extern db_res *db_getdevexp_1();
extern int *db_putres_1();
extern int *db_delres_1();
extern cmd_que  *db_cmd_query_1();
/* event query */
extern event_que  *db_event_query_1();
extern db_psdev_error  *db_psdev_reg_1();
extern db_psdev_error  *db_psdev_unreg_1();
extern long db_null_proc_1();

extern db_res * db_getdevdomain_1();
extern db_res * db_getdevfamily_1();
extern db_res * db_getdevmember_1();
extern db_res * db_getresdomain_1();
extern db_res * db_getresfamily_1();
extern db_res * db_getresmember_1();
extern db_res * db_getresreso_1();
extern db_res * db_getresresoval_1();
extern db_res * db_getdsserver_1();
extern db_res * db_getdspers_1();
extern db_res * db_gethost_1();
extern db_svcarray_net * db_getdsonhost_1();

extern db_devinfo_svc * db_deviceinfo_1();
extern db_res * db_deviceres_1();
extern long db_devicedelete_1();
extern db_psdev_error db_devicedeleteres_1();
extern db_info_svc *db_stat_1();
extern long *db_servunreg_1();
extern svcinfo_svc *db_servinfo_1();
extern long *db_servdelete_1();
extern db_poller_svc *db_getpoll_1();
extern long *db_initcache_1();

extern db_psdev_error *db_upddev_1();
extern db_psdev_error *db_updres_1();
extern db_res *db_secpass_1();
extern db_delupd_error *db_delete_update_1();


extern int  *db_clodb_1();
extern int  *db_reopendb_1();

#endif	/* __STDC__ */


#ifndef ALONE
bool_t _WINAPI
 xdr_register_data();
#endif

#ifdef __cplusplus
 }
#endif

#endif /* _setacc.h */
