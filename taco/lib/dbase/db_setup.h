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
 * File:	db_setup.h
 *
 * Description:	
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 * 
 * Version:	$Revision: 1.13 $
 *
 * Date:	$Date: 2008-10-23 09:21:32 $
 *
 ******************************************************************************/

/**@defgroup API TACO API
 */

/**@defgroup dbaseAPI	Database API
 * @ingroup API
 * Functions to allow a database client to:
 *	- retrieve, update, insert delete resources.
 *	- retrieve device list, mark device as exported, return device information
 *	- retrieve all or part of the exported devices
 *	- register and unregister pseudo devices
 *	- browse the database
 *	- retrieve command and event codes from command or event name
 *
 * These functions are briefly described here. Man pages are available for all of them to get
 * complete information. The library (client part of RPC calls) is available for HP-UX, Solaris,
 * OS-9, Linux, and FreeBSD.
 */

/**@defgroup dbaseAPIintern Internal functions of the database API
 * @ingroup dbaseAPI
 */

/**@defgroup dbaseAPIresource Resource oriented calls
 * @ingroup dbaseAPI
 * These functions are linked to resources.
 */

/**@defgroup dbaseAPIexport Exported device list oriented calls 
 * @ingroup dbaseAPI
 * These functions are used to get information on which devices are available for request in the control system.
 */

/**@defgroup dbaseAPIdevice Device oriented calls
 * @ingroup dbaseAPI
 * These functions are device oriented.
 */

/**@defgroup dbaseAPIserver Server oriented calls
 * @ingroup dbaseAPI
 * These functions deal with device server.
 */

/**@defgroup dbaseAPIbrowse Database browsing oriented calls
 * @ingroup dbaseAPI
 * All these functions allow database browsing.
 */

/**@defgroup dbaseAPIps Pseudo device oriented calls
 * @ingroup dbaseAPI
 */

/**@defgroup dbaseAPIupdate Database update calls
 * @ingroup dbaseAPI
 */

/**@defgroup dbaseAPImisc Miscellaneous calls
 * @ingroup dbaseAPI
 */

#ifndef _db_setup_h
#define _db_setup_h
#include <dev_xdr.h>

/* Constants definition */

#define	MAXDEV		16		/* Maximun number of device for a d 
					   server before memory reallocation */
#define ST_ALLOC	8		/* For allocation algorithm
					   DON'T CHANGE THIS VALUE OR THE SKY
					   WILL FALL ON YOUR HEAD ! */
#define	MAXDEV_UDP	201		/* The max. number of device names in
					   one UDP packet (8 K of data) */
#define STRING_SIZE	23		/* Maximun size for a resource defined
					   as a atring */
#define MAX_RES		256		/* Maximun size for a resource belonging
					   to the ERROR or CMDS domains */
#define	ERR_DEVNAME	0x1		/* Error in device name def. */
#define	ERR_RESVAL	0x2		/* Error in resource definition */
#define	ERR_DEV_NOT_FD	0x3		/* No device */

#define	SIZE		8192		/* The buffer size for dbset_update */
#define	SIZE_DEL	1024		/* The buffer size for db_delresource */
#define	LIM		256		/* For the RPC information */
#define	SEP_ELT		0x02		/* Separator between resource element
					   array */
#define	INIT_ARRAY	0x05		/* The first string character when the 
				   	   resource is an array */
#define	PAS_MAX_LENGTH	23		/* Max length for the sec. passwd */
#define	RETRY		2		/* Max number of retries after not
					   connected error (only for ndbm) */
#define	DB_SETUPVERS	((u_long)1)	/* The original version number */
#define	DB_VERS_2	((u_long)2)	/* Version number 2 ! */
#define	DB_VERS_3	((u_long)3)	/* Version number 3 ! */

#define SLEEP_TIME	0x80000006	/* 20 mS in 1/256 seconds */

#define	DEV_CLASS_LENGTH	24
#define DEV_TYPE_LENGTH		24
#if HAVE_SYS_PARAM_H
#	include <sys/param.h>
#endif
#ifndef HOST_NAME_MAX
#	ifdef MAXHOSTNAMELEN
#		define HOST_NAME_LENGTH 	MAXHOSTNAMELEN
#	else
#		define  HOST_NAME_LENGTH        20
#	endif
#else
#	define  HOST_NAME_LENGTH        	HOST_NAME_MAX
#endif
#define	PROC_NAME_LENGTH	40
#define DSPERS_NAME_LENGTH	12
#if 0
#	define DS_NAME_LENGTH	24
#endif
#define DS_NAME_LENGTH		PROC_NAME_LENGTH + DSPERS_NAME_LENGTH
#define DOMAIN_NAME_LENGTH	9
#define FAMILY_NAME_LENGTH	20
#define MEMBER_NAME_LENGTH	20
#if 0
#	define DEV_NAME_LENGTH	24
#endif
#define DEV_NAME_LENGTH		DOMAIN_NAME_LENGTH + FAMILY_NAME_LENGTH + MEMBER_NAME_LENGTH 
#define RES_NAME_LENGTH		32
#define LONG_RES_NAME_LENGTH	80
#if 0
#	define LONG_RES_NAME_LENGTH	DEV_NAME_LENGTH + RES_NAME_LENGTH
#endif
#ifdef NDBM
#	define RES_VAL_LENGTH	2048
#else
#	define RES_VAL_LENGTH	24
#endif
#define LONG_RES_VAL_LENGTH	256

#define SPEC_NAME_LENGTH	60
#define SPEC_ACCESS_LENGTH	40

#define ORA_LOOP		50
#define ORA_MEDIUM_LOOP		20
#define ORA_SMALL_LOOP		10

#define	DB_Device		1
#define DB_Pseudo_Device	2

#define Db_File			1
#define Db_Buffer		2

#define SEC_SEP			'^'

#define UDP_MAX_SIZE		(8 * 1024) - 512

#define DB_GLOBAL_ERROR		1
#define DB_DELETE_ERROR		2
#define DB_UPD_DEV_ERROR	3
#define DB_UPD_RES_ERROR	4

#define DB_DEV_LIST		0
#define DB_RES_LIST		1

#define DB_UDP			0
#define DB_TCP			1

#ifdef sun
#define	ALONE_SERVER_HOST	"apus"
#else
#define ALONE_SERVER_HOST	"pollux"
#endif

/* Special definitions for ndbm software */

#define	MAX_CONT	2150		/* Max. length of the dbm database
					   content */
#define	MAXDOMAIN	20		/* Max. number of domain */
#define	MAX_KEY		100		/* Max. length of the dbm database 
					   key */
#define	MAX_RES_SERVDEL	500		/* Max res. number */


/* Structures definitions */

typedef struct _db_resource
{
	const char	*resource_name;
	short 		resource_type;
	void 		*resource_adr;
}db_resource,*Db_resource;

typedef struct _db_devinf_imp
{
	char 		device_name[DEV_NAME_LENGTH];
	char 		host_name[HOST_NAME_LENGTH];
	char 		device_type[DEV_TYPE_LENGTH];
	char 		device_class[DEV_CLASS_LENGTH];
	unsigned int 	pn;
	unsigned int 	vn;
}db_devinf_imp,*Db_devinf_imp;

typedef struct _db_devinf
{
	char 		*device_name;
	char 		*host_name;
	char 		*device_type;
	char 		*device_class;
	unsigned int 	pn;
	unsigned int 	vn;
	char 		*proc_name;
}db_devinf,*Db_devinf;

typedef struct _device
{
	char 		h_name[HOST_NAME_LENGTH];
	char 		ds_class[DS_NAME_LENGTH];
	char 		ds_name[DSPERS_NAME_LENGTH];
	char 		d_name[DEV_NAME_LENGTH];
	int 		pn;
	int 		vn;
	char 		d_type[DEV_TYPE_LENGTH];
	char 		d_class[DEV_CLASS_LENGTH];
	int 		indi;
	int 		pid;
	char 		proc_name[PROC_NAME_LENGTH];
}device;

typedef struct _ps_device
{
	char 		h_name[HOST_NAME_LENGTH];
	char 		ps_name[DS_NAME_LENGTH];
	int 		pid;
	int 		refresh;
}ps_device;

typedef struct _reso
{
	char 		fam[FAMILY_NAME_LENGTH];
	char 		member[MEMBER_NAME_LENGTH];
	char 		r_name[RES_NAME_LENGTH];
	char 		r_val[RES_VAL_LENGTH];
	int 		indi;
}reso;

typedef struct _reso_long
{
	char 		fam[FAMILY_NAME_LENGTH];
	char	 	member[MEMBER_NAME_LENGTH];
	char 		r_name[RES_NAME_LENGTH];
	char 		r_val[LONG_RES_VAL_LENGTH];
	int 		indi;
}reso_long;

typedef struct _dena
{
	char 		devina[DS_NAME_LENGTH];
	char 		oh_name[HOST_NAME_LENGTH];
	int 		opn;
	int 		ovn;
	char 		od_type[DEV_TYPE_LENGTH];
	char 		od_class[DEV_CLASS_LENGTH];
	int 		opid;
	char 		od_proc[PROC_NAME_LENGTH];
}dena;

typedef struct _spec_data
{
	char 		name[SPEC_NAME_LENGTH];
	char 		access[SPEC_ACCESS_LENGTH];
}spec_data;

typedef struct _servinfo
{
	unsigned int 	pn;
	char 		ds_name[DS_NAME_LENGTH];
	char 		pers_name[DSPERS_NAME_LENGTH];
	int 		pid;
	int 		bad_answer;
	int 		class_number;
	char 		class_name[10][PROC_NAME_LENGTH];
}servinfo;

typedef struct _db_psdev_info
{
	char 		*psdev_name;
	long 		poll_interval;
}db_psdev_info;

typedef struct _db_error
{
	long 		error_code;
	long 		psdev_err;
}db_error;

typedef struct _db_devinfo_call
{
	long		device_type;
	long		device_exported;
	char		device_class[DEV_CLASS_LENGTH];
	char		server_name[DS_NAME_LENGTH];
	char		personal_name[DSPERS_NAME_LENGTH];
	char		process_name[PROC_NAME_LENGTH];
	unsigned long	server_version;
	char		host_name[HOST_NAME_LENGTH];
	unsigned long	pid;
	unsigned long   program_num;
}db_devinfo_call;

typedef struct _db_info_dom
{
	char		dom_name[DOMAIN_NAME_LENGTH];
	long		dom_elt;
}db_info_dom;

typedef struct _db_stat_call
{
	long		dev_defined;
	long		dev_exported;
	long		psdev_defined;
	long		res_number;
	long		dev_domain_nb;
	db_info_dom	*dev_domain;
	long		res_domain_nb;
	db_info_dom	*res_domain;
}db_stat_call;


typedef struct _db_svcinfo_dev
{
	long		exported_flag;
	char		dev_name[DEV_NAME_LENGTH];
}db_svcinfo_dev;


typedef struct _db_svcinfo_server
{
	char		server_name[DS_NAME_LENGTH];
	long		device_nb;
	db_svcinfo_dev	*device;
}db_svcinfo_server;


typedef struct _db_svcinfo_call
{
	long			embedded_server_nb;
	db_svcinfo_server	*server;
	char			process_name[PROC_NAME_LENGTH];
	unsigned long		pid;
	char			host_name[HOST_NAME_LENGTH];
	unsigned long		program_num;
}db_svcinfo_call;		

typedef struct _db_dev_in_db
{
	char			dev_name[DEV_NAME_LENGTH];
	long			found;
	long			seq;
	char			key_buf[MAX_KEY];
	device			dev_info;
}db_dev_in_db;

typedef struct _db_poller
{
	char		server_name[DS_NAME_LENGTH];
	char 		personal_name[DSPERS_NAME_LENGTH];
	char		host_name[HOST_NAME_LENGTH];
	char		process_name[PROC_NAME_LENGTH];
	unsigned long	pid;
}db_poller;


typedef struct _db_svc
{
	char		server_name[DS_NAME_LENGTH];
	char 		personal_name[DSPERS_NAME_LENGTH];
	char		host_name[HOST_NAME_LENGTH];
	unsigned long	pid;
	unsigned long 	program_num;
}db_svc;


typedef struct _db_delupd_error 
{
	DevLong	type;
	DevLong number;
	DevLong error;
}db_delupd_error;


/* Macros definitions */

/* This macros initialize A with the number of C character in the string
   pointed to by B */

#define NB_CHAR(A,B,C) {\
	int im,lm;\
	lm = strlen(B);\
	for (im=0;im<lm;im++) {\
		if (B[im] == C)\
			A++;\
			}\
}\

/* Functions definitions */

#ifdef __cplusplus
extern "C" {
#endif

int _DLLFunc db_getresource PT_((const char *dev_name,register Db_resource resource,u_int num_resource,DevLong *error));
int _DLLFunc db_getdevlist PT_((char *ds_name,char ***device_tab,u_int *dev_num,DevLong *error));
int _DLLFunc db_dev_export PT_((register Db_devinf devinfo,u_int num_device,DevLong *error));
int _DLLFunc db_dev_import PT_((char **devname,Db_devinf_imp *devinf,unsigned int num_device,DevLong *error));
int _DLLFunc db_svc_unreg PT_((char *dsn_name,DevLong *error));
int _DLLFunc db_svc_check PT_((char *dsn_name,char **host_name,u_int *p_num,u_int *v_num,DevLong *error));
int _DLLFunc db_getdevexp  PT_((char *filter,char ***device_tab,u_int *dev_num,DevLong *error));
int _DLLFunc db_freedevexp  PT_((char **buffer));
int _DLLFunc db_svc_close PT_((DevLong *error));
int _DLLFunc db_svc_reopen PT_((DevLong *error));
int _DLLFunc db_putresource PT_((char *dev_name,register Db_resource resource,u_int num_resource,DevLong *error));
int _DLLFunc db_delresource PT_((char *dev_name,register char **res_name,u_int num_resource,DevLong *error));
int _DLLFunc db_cmd_query PT_((char *cmd_name,u_int *cmd_code,DevLong *error));
/*event query */
int _DLLFunc db_event_query PT_((char *event_name,u_int *event_code,DevLong *error));

int _DLLFunc db_psdev_register PT_((db_psdev_info *psdev,long num_psdev,db_error *error));
int _DLLFunc db_psdev_unregister PT_((char *psdev_list[],long num_psdev,db_error *error));

long _DLLFunc db_getdevdomainlist PT_((long *domain_nb,char ***domain_list,DevLong *error));
long _DLLFunc db_getdevfamilylist PT_((char *domain,long *family_nb,char ***family_list,DevLong *error));
long _DLLFunc db_getdevmemberlist PT_((char *domain,char *family,long *member_nb,char ***member_list,DevLong *error));
long _DLLFunc db_getresdomainlist PT_((long *domain_nb,char ***domain_list,DevLong *error));
long _DLLFunc db_getresfamilylist PT_((char *domain,long *family_nb,char ***family_list,DevLong *error));
long _DLLFunc db_getresmemberlist PT_((char *domain,char *family,long *member_nb,char ***member_list,DevLong *error));
long _DLLFunc db_getresresolist PT_((char *domain,char *family,char *member,long *resource_nb,char ***resource_list,DevLong *error));
long _DLLFunc db_getresresoval PT_((char *domain,char *family,char *member,char *resource,long *resval_nb,char ***resval_list,DevLong *error));
long _DLLFunc db_getdsserverlist PT_((long *server_nb,char ***server_list,DevLong *error));
long _DLLFunc db_getdspersnamelist PT_((char *server,long *persname_nb,char ***persname_list,DevLong *error));
long _DLLFunc db_gethostlist PT_((long *host_nb,char ***host_list,DevLong *error));
long _DLLFunc db_getdsonhost PT_((char *host,long *ds_nb,db_svc **ds_list,DevLong *error));

long _DLLFunc db_deviceinfo PT_((const char *dev_name,db_devinfo_call *devinfo,DevLong *error));
long _DLLFunc db_deviceres PT_((long dev_nb,char **dev_name_list,long *res_nb,char ***res_list,DevLong *error));
long _DLLFunc db_devicedelete PT_((const char *dev_name,DevLong *error));
long _DLLFunc db_devicedeleteres PT_((long dev_nb,char **dev_name_list,db_error *error));
long _DLLFunc db_stat PT_((db_stat_call *info,DevLong *error));
long _DLLFunc db_servunreg PT_((const char *ds_name,const char *pers_name,DevLong *error));
long _DLLFunc db_servinfo PT_((const char *ds_name,const char *pers_name,db_svcinfo_call *s_info,DevLong *error));
long _DLLFunc db_servdelete PT_((const char *ds_name,const char *pers_name,long delres_flag,DevLong *error));
long _DLLFunc db_getpoller PT_((const char *dev_name,db_poller *poll,DevLong *error));
long _DLLFunc db_delreslist PT_((char **res_list,long res_num,DevLong *error));
long _DLLFunc db_initcache PT_((const char *domain,DevLong *error));

long _DLLFunc db_analyze_data PT_((long in_type,const char *buffer,long *nb_devdef,char ***devdef,long *nb_resdef,char ***resdef,long *error_line,DevLong *error));
long _DLLFunc db_upddev PT_((long devdef_nb,char **devdef,long *deferr_nb,DevLong *error));
long _DLLFunc db_updres PT_((long resdef_nb,char **resdef,long *deferr_nb,DevLong *error));
long _DLLFunc db_secpass PT_((char **pass,DevLong *error));
long _DLLFunc db_delete_update PT_((long dev_nb,char **dev_name_list,long list_type,long devdef_nb,char **devdef,long resdef_nb,char **resdef,db_delupd_error *p_error));

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _db_setup_h */
