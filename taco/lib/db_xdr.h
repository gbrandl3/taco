 /*

 Author:	$Author: jkrueger1 $

 Version:	$Version$

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 *  */

#ifndef _setacc
#define _setacc

#include <macros.h>
#include <db_setup.h>

#ifdef __STDCPP__
#include <vector>
#include <string>
#endif

typedef char *nam;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_nam(XDR *, nam *);
#else
 xdr_nam();
#endif


struct arr1 {
	u_int arr1_len;
	nam *arr1_val;
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
	char *dev_name;
	char *host_name;
	char *dev_type;
	char *dev_class;
	u_int p_num;
	u_int v_num;
};
typedef struct db_devinfo db_devinfo;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo(XDR *, db_devinfo *);
#else
 xdr_db_devinfo();
#endif	/* __STDC__ */


typedef struct {
	u_int tab_dbdev_len;
	db_devinfo *tab_dbdev_val;
} tab_dbdev;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev(XDR *, tab_dbdev *);
#else
 xdr_tab_dbdev();
#endif	/* __STDC__ */

/* For db_dev_export version 2 */

struct db_devinfo_2 {
	char *dev_name;
	char *host_name;
	char *dev_type;
	char *dev_class;
	u_int p_num;
	u_int v_num;
	u_int pid;
};
typedef struct db_devinfo_2 db_devinfo_2;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo_2(XDR *, db_devinfo_2 *);
#else
 xdr_db_devinfo_2();
#endif	/* __STDC__ */


typedef struct {
	u_int tab_dbdev_len;
	db_devinfo_2 *tab_dbdev_val;
} tab_dbdev_2;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev_2(XDR *, tab_dbdev_2 *);
#else
 xdr_tab_dbdev_2();
#endif	/* __STDC__ */

/* For db_dev_export version 3 */

struct db_devinfo_3 {
	char *dev_name;
	char *host_name;
	char *dev_type;
	char *dev_class;
	u_int p_num;
	u_int v_num;
	u_int pid;
	char *proc_name;
};
typedef struct db_devinfo_3 db_devinfo_3;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_devinfo_3(XDR *, db_devinfo_3 *);
#else
 xdr_db_devinfo_3();
#endif	/* __STDC__ */


typedef struct {
	u_int tab_dbdev_len;
	db_devinfo_3 *tab_dbdev_val;
} tab_dbdev_3;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_dbdev_3(XDR *, tab_dbdev_3 *);
#else
 xdr_tab_dbdev_3();
#endif	/* __STDC__ */




struct db_res {
	arr1 res_val;
	int db_err;
};
typedef struct db_res db_res;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_res(XDR *, db_res *);
#else
 xdr_db_res();
#endif	/* __STDC__ */


struct db_resimp {
	tab_dbdev imp_dev;
	int db_imperr;
};
typedef struct db_resimp db_resimp;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_resimp(XDR *, db_resimp *);
#else
 xdr_db_resimp();
#endif	/* __STDC__ */


struct svc_inf {
	char *ho_name;
	u_int p_num;
	u_int v_num;
	int db_err;
};
typedef struct svc_inf svc_inf;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_svc_inf(XDR *, svc_inf *);
#else
 xdr_svc_inf();
#endif	/* __STDC__ */


struct putres {
	char *res_name;
	char *res_val;
};
typedef struct putres putres;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_putres(XDR *, putres *);
#else
 xdr_putres();
#endif	/* __STDC__ */


typedef struct {
	u_int tab_putres_len;
	putres *tab_putres_val;
}tab_putres;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_putres(XDR *, tab_putres *);
#else
 xdr_tab_putres();
#endif	/* __STDC__ */

struct cmd_que{
	int db_err;
	u_int xcmd_code;
};
typedef struct cmd_que cmd_que;
/* for event querys */
struct event_que{
	int db_err;
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
	int tcp;
	int tcp_so;
	CLIENT  *cl;
	}devexp_res;

/* Added definition for db_psdev_register function */

struct psdev_elt {
	char *psdev_name;
	long poll;
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
	char *h_name;
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
	long error_code;
	long psdev_err;
};
typedef struct db_psdev_error db_psdev_error;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__ 
 xdr_db_psdev_error(XDR *, db_psdev_error *);
#else
 xdr_db_psdev_error();
#endif	/* __STDC__ */


struct db_devinfo_svc {
	long 		device_type;
	long 		device_exported;
	char		*device_class;
	char		*server_name;
	char 		*personal_name;
	char 		*process_name;
	unsigned long	server_version;
	char 		*host_name;
	unsigned long	pid;
	unsigned long	program_num;
	int 		db_err;
};
typedef struct db_devinfo_svc db_devinfo_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo_svc(XDR *, db_devinfo_svc *);
#else
xdr_db_devinfo_svc();
#endif /* __STDC__ */


struct db_info_dom_svc {
	char		*dom_name;
	long		dom_elt;
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
	long		dev_defined;
	long		dev_exported;
	long		psdev_defined;
	long		res_number;
	var_dom		dev;
	var_dom		res;
	int		db_err;
};
typedef struct db_info_svc db_info_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_info_svc(XDR *, db_info_svc *);
#else
xdr_db_info_svc();
#endif /* __STDC__ */


struct svcinfo_dev {
	long		exported_flag;
	char		*name;
};
typedef struct svcinfo_dev svcinfo_dev;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_dev(XDR *, svcinfo_dev *);
#else
xdr_svcinfo_dev();
#endif /* __STDC__ */


struct svcinfo_server {
	char		*server_name;
	long		dev_len;
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
	char		*process_name;
	unsigned long	pid;
	unsigned long	program_num;
	char		*host_name;
	long		embedded_len;
	svcinfo_server	*embedded_val;
	int 		db_err;
};
typedef struct svcinfo_svc svcinfo_svc;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_svc(XDR *, svcinfo_svc *);
#else
xdr_svcinfo_svc();
#endif /* __STDC__ */


struct db_poller_svc {
	char		*server_name;
	char		*personal_name;
	char		*host_name;
	char 		*process_name;
	unsigned long	pid;
	int 		db_err;
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
	char		*server_name;
	char		*personal_name;
	char		*host_name;
	unsigned long	pid;
	unsigned long	program_num;
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
	long		db_err;
};
typedef struct db_svcarray_net db_svcarray_net;
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_svcarray_net(XDR *, db_svcarray_net *);
#else
 xdr_db_svcarray_net();
#endif	/* __STDC__ */


struct ana_input{
	long		in_type;
	char		**buf;
	FILE		*f;
};
typedef struct ana_input ana_input;


/* Miscellaneous function */

int to_reconnection(void *,void **,CLIENT **,int,long,long,long *);
void kern_sort(char **,int);
#ifdef __STDCPP__
long get_tango_exp_devices(char *,vector<string> &,long *);
#endif

/* Define some constantes */

#ifdef ALONE
#define DB_SETUPPROG ((u_long)21000023)
#endif

#define DB_GETRES ((u_long)1)
#define DB_GETDEV ((u_long)2)
#define DB_DEVEXP ((u_long)3)
#define DB_DEVIMP ((u_long)4)
#define DB_SVCUNR ((u_long)5)
#define DB_SVCCHK ((u_long)6)
#define DB_GETDEVEXP ((u_long)7)
#define DB_PUTRES ((u_long)10)
#define DB_DELRES ((u_long)11)
#define DB_CMDQUERY ((u_long)12)
#define DB_PSDEV_REG ((u_long)13)
#define DB_PSDEV_UNREG ((u_long)14)
#define DB_GETDEVDOMAIN ((u_long)15)
#define DB_GETDEVFAMILY ((u_long)16)
#define DB_GETDEVMEMBER ((u_long)17)
#define DB_GETRESDOMAIN ((u_long)19)
#define DB_GETRESFAMILY ((u_long)20)
#define DB_GETRESMEMBER ((u_long)21)
#define DB_GETRESRESO ((u_long)22)
#define DB_GETRESRESOVAL ((u_long)23)
#define DB_GETSERVER ((u_long)24)
#define DB_GETPERS ((u_long)25)
#define DB_GETHOST ((u_long)26)
#define DB_DEVINFO ((u_long)27)
#define DB_DEVRES ((u_long)28)
#define DB_DEVDEL ((u_long)29)
#define DB_DEVDELALLRES ((u_long)30)
#define DB_INFO ((u_long)31)
#define DB_SVCUNREG ((u_long)32)
#define DB_SVCINFO ((u_long)33)
#define DB_SVCDELETE ((u_long)34)
#define DB_UPDDEV ((u_long)35)
#define DB_UPDRES ((u_long)36)
#define DB_SECPASS ((u_long)37)
#define DB_GETPOLLER ((u_long)38)
#define DB_DEL_UPDATE ((u_long)39)
#define DB_INITCACHE ((u_long)40)
#define DB_GETDSHOST ((u_long)41)
#define DB_EVENTQUERY ((u_long)42)

/* For ndbm only */
#define DB_CLODB ((u_long)8)
#define DB_REOPENDB ((u_long)9)
/* For to_reconnection */
#define DB_DEVEXP_2 ((u_long)1002)
#define DB_DEVEXP_3 ((u_long)1003)

#if defined __STDC__ | defined __STDCPP__

/* Client call to server */

extern db_res  * db_getres_1(arr1 *, CLIENT *, long *);
extern db_res  * db_getdev_1(nam *, CLIENT *, long *);
extern int * db_devexp_1(tab_dbdev *, CLIENT *, long *);
extern int * db_devexp_2(tab_dbdev_2 *, CLIENT *, long *);
extern int * db_devexp_3(tab_dbdev_3 *, CLIENT *, long *);
extern db_resimp * db_devimp_1(arr1 *, CLIENT *, long *);
extern int * db_svcunr_1(nam *, CLIENT *, long *);
extern svc_inf * db_svcchk_1(nam *, CLIENT *, long *);
extern db_res * db_getdevexp_1(nam *, CLIENT *, long *);
extern int * db_putres_1(tab_putres *, CLIENT *, long *);
extern int * db_delres_1(arr1 *, CLIENT *, long *);
extern cmd_que * db_cmd_query_1(nam *, CLIENT *, long *);
/*event query*/
extern event_que * db_event_query_1(nam *, CLIENT *, long *);
extern db_psdev_error * db_psdev_reg_1(psdev_reg_x *, CLIENT *, long *);
extern db_psdev_error * db_psdev_unreg_1(arr1 *, CLIENT *, long *);
extern long db_null_proc_1(CLIENT *clnt, long *perr);

extern db_res * db_getdevdomain_1(CLIENT *,long *);
extern db_res * db_getdevfamily_1(nam *,CLIENT *,long *);
extern db_res * db_getdevmember_1(db_res *,CLIENT *,long *);
extern db_res * db_getresdomain_1(CLIENT *,long *);
extern db_res * db_getresfamily_1(nam *,CLIENT *,long *);
extern db_res * db_getresmember_1(db_res *,CLIENT *,long *);
extern db_res * db_getresreso_1(db_res *,CLIENT *,long *);
extern db_res * db_getresresoval_1(db_res *,CLIENT *,long *);
extern db_res * db_getdsserver_1(CLIENT *,long *);
extern db_res * db_getdspers_1(nam *,CLIENT *,long *);
extern db_res * db_gethost_1(CLIENT *,long *);
extern db_svcarray_net * db_getdsonhost_1(nam *,CLIENT *,long *);

extern db_devinfo_svc * db_deviceinfo_1(nam *,CLIENT *,long *);
extern db_res * db_deviceres_1(db_res *,CLIENT *,long *);
extern long *db_devicedelete_1(nam *,CLIENT *,long *);
extern db_psdev_error *db_devicedeleteres_1(db_res *,CLIENT *,long *);
extern db_info_svc * db_stat_1(CLIENT *,long *);
extern long *db_servunreg_1(db_res *,CLIENT *,long *);
extern svcinfo_svc *db_servinfo_1(db_res *,CLIENT *,long *);
extern long *db_servdelete_1(db_res *,CLIENT *,long *);
extern db_poller_svc *db_getpoll_1(nam *,CLIENT *,long *);
extern long *db_initcache_1(nam *,CLIENT *,long *);

extern db_psdev_error *db_upddev_1(db_res *,CLIENT *,long *);
extern db_psdev_error *db_updres_1(db_res *,CLIENT *,long *);
extern db_res *db_secpass_1(CLIENT *,long *);
extern db_delupd_error *db_delete_update_1(db_arr1_array *,CLIENT *,long *);

extern int * db_clodb_1(CLIENT *, long *);
extern int * db_reopendb_1(CLIENT *, long *);

/* Server functions */

db_res *db_getres_1_svc(arr1 *,struct svc_req *);
db_res *db_getdev_1_svc(nam *);
int *db_putres_1_svc(tab_putres *);
int *db_delres_1_svc(arr1 *,struct svc_req *);
cmd_que *db_cmd_query_1_svc(nam *);
event_que *db_event_query_1_svc(nam *);
db_res *db_getdevexp_1_svc(nam *,struct svc_req *);
int *db_devexp_1_svc(tab_dbdev *);
int *db_devexp_2_svc(tab_dbdev_2 *);
int *db_devexp_3_svc(tab_dbdev_3 *);
db_resimp *db_devimp_1_svc(arr1 *);
int *db_svcunr_1_svc(nam *);
svc_inf *db_svcchk_1_svc(nam *);
db_psdev_error *db_psdev_reg_1_svc(psdev_reg_x *);
db_psdev_error *db_psdev_unreg_1_svc(arr1 *);

db_res *devdomainlist_1_svc(void);
db_res *devfamilylist_1_svc(nam *);
db_res *devmemberlist_1_svc(db_res *);
db_res *resdomainlist_1_svc(void);
db_res *resfamilylist_1_svc(nam *);
db_res *resmemberlist_1_svc(db_res *);
db_res *resresolist_1_svc(db_res *);
db_res *resresoval_1_svc(db_res *);
db_res *devserverlist_1_svc(void);
db_res *devpersnamelist_1_svc(nam *);
db_res *hostlist_1_svc(void);
db_svcarray_net *getdsonhost_1_svc(nam *);

db_devinfo_svc *devinfo_1_svc(nam *);
db_res *devres_1_svc(db_res *);
long *devdel_1_svc(nam *);
db_psdev_error *devdelres_1_svc(db_res *);
db_info_svc *info_1_svc(void);
long *unreg_1_svc(db_res *);
svcinfo_svc *svcinfo_1_svc(db_res *);
long *svcdelete_1_svc(db_res *);
db_poller_svc *getpoller_1_svc(nam *);
long *initcache_1_svc(nam *);

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

#endif /* _setacc.h */
