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
 * File:        DevSec.h
 *
 * Project:     Device Server Security
 *
 * Description: Public include file for the device server security
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    September 1993
 *
 * Version:	$Revision: 1.8 $
 * 
 * Date:	$Date: 2008-04-06 09:06:58 $
 *
 ********************************************************************-*/

#ifndef _DevSec_h
#define _DevSec_h

/*
 * Definition of access rights
 */

#define 	NO_ACCESS	(-20)	/* No access */
#define 	READ_ACCESS	(-10)	/* Read only access */
#define 	WRITE_ACCESS   	   0	/* Read and write access */
#define     	SI_WRITE_ACCESS   10	/* Single user write access */
#define 	SU_ACCESS         20  	/* Read and write access and the 
					   rigth to execute super user 
					   commands */
#define 	SI_SU_ACCESS	  30	/* Single user mode with super user
					   rigths */
#define		ADMIN_ACCESS      99    /* Single user administration 
					   access */



typedef struct _DevSecListEntry {
			const char  *access_name;  /* string as the defined name */
			long  access_right;        /* a number defining the access
						      level. The higher the number,
						      the more rights */
			} DevSecListEntry;

static DevSecListEntry DevSec_List[] = {
	{"NO_ACCESS", 		NO_ACCESS},
	{"READ_ACCESS", 	READ_ACCESS},
	{"WRITE_ACCESS", 	WRITE_ACCESS},
	{"SI_WRITE_ACCESS", 	SI_WRITE_ACCESS},
	{"SU_ACCESS", 		SU_ACCESS},
	{"SI_SU_ACCESS", 	SI_SU_ACCESS},
	{"ADMIN_ACCESS", 	ADMIN_ACCESS},
};
#define SEC_LIST_LENGTH     (sizeof(DevSec_List)/sizeof(DevSecListEntry))

/*
 * Defines for OS9 user name and group name
 */
#define OS9_USER	"os9"
#define OS9_GROUP	"os9"

/*
 * User identification structure
 */
typedef struct	_SecUserAuth {
	char		*user_name;
	long		uid;
	char		*group_name;
	long 		gid;
	char		host_name[HOST_NAME_LENGTH];
	u_long  	ip;
	} SecUserAuth;

/*
 * Default access values structure
 */
typedef struct _SecDefaultAccess {
	long	access_right;
	long	ip_list_length;
	u_long  *ip_addr_list;
	} SecDefaultAccess;

/*
 * Structure for the security keys off all connections
 * to device servers in a client. Is used to check on modifications
 * in the client handle which is returned to the user.
 */

#define FREE_CONNECTION		(-1)
#define LOCAL_CONNECTION	(-2)

typedef struct _SecOpenConn {
	long	max_no_of_conn;
	long	*sec_keys;
	} SecOpenConn;


#ifdef __cplusplus
extern "C" {
#endif
long _DLLFunc dev_security PT_( (char *dev_name, long requested_access,
			long *ret_client_id, long *connection_id,
			DevLong *error) );
long _DLLFunc create_sec_key PT_( (devserver ds, DevLong *error) );
long _DLLFunc verify_sec_key PT_( (devserver ds, long *client_id, DevLong *error));
void _DLLFunc free_sec_key PT_( (devserver ds) );
long _DLLFunc sec_tcp_connection PT_( (long requested_access, CLIENT * *clnt,
			server_connections *svr_conn, DevLong *error) );
void _DLLFunc sec_free_tcp_connection PT_( (devserver ds,server_connections *svr_conn));
long _DLLFunc free_connection_id_vers3 PT_( (long connection_id, DevLong *error) );
#ifdef __cplusplus
}
#endif

#ifdef _DevServer_h
/* DevServerDevices is declared in DevServer.h
 * so hopefully these functions are needed only for servers
 */
#ifdef __cplusplus
extern "C" {
#endif
long _DLLFunc sec_svc_import PT_( (DevServerDevices *device, long connection_id,
			long client_id, long access_right,
			struct svc_req *rqstp, DevLong *error) );
long _DLLFunc sec_svc_free PT_( (DevServerDevices *device, long connection_id,
			long client_id, long access_right,
			DevLong *error) );
long _DLLFunc sec_svc_cmd PT_( (DevServerDevices *device, long connection_id,
			long client_id, long access_right,
			long cmd, DevLong *error) );
void _DLLFunc free_var_str_array PT_( (DevVarStringArray *str_array) );
#ifdef __cplusplus
}
#endif
#endif /* _DevServer_h */

long _DLLFunc free_connection_id_vers3
			PT_( (long connection_id, DevLong *error) );


#endif /* _DevSec_h */

