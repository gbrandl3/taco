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
 * File:	ApiP.h
 *
 * Project:	Device Servers with sun-rpc
 *
 * Description:	Private include for the applications 
 *		programmers interface.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jensmeyer $
 *
 * Original:	June 1992
 *
 * Version:	$Revision: 1.20 $
 *
 * Date:	$Date: 2009-11-17 11:49:03 $
 *
 ********************************************************************-*/

#ifndef APIP_H
#define APIP_H


/*
 *  standard header string to use "what" or "ident".
 */
#ifdef _IDENT
static char ApiPh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/private/ApiP.h,v 1.20 2009-11-17 11:49:03 jensmeyer Exp $";
#endif /* _IDENT */


/*
 * dsxdr library include files
 */
#include <API_xdr.h>
#ifndef __cplusplus
#	include <DevServer.h>
#else
#	include <Device.h>
#endif /* __cplusplus */

/*
 * Define the number of array fields to allocate as a
 * data block.
 */

#define BLOCK_SIZE      50

/*
 * Defines to handle the export of devices and the
 * import of devices.
 */

#define NOT_EXPORTED            -1
#define EXPORTED                0

#define MAX_DEVICES             2047
#define MAX_CONNECTIONS         4095
#define MAX_COUNT                255
#define MAX_ASYNCH_CALLS	 300

/*
 * Masks to decode the device identification variable.
 */

#define DEVICES_MASK            0x7FF
#define CONNECTIONS_MASK        0xFFF
#define COUNT_MASK              0xFF
#define LOCALACCESS_MASK        0x1

#define CONNECTIONS_SHIFT       11
#define COUNT_SHIFT             23
#define LOCALACCESS_SHIFT       31

/*
 * defines to mark rpc connections
 */

#ifndef _UCC
#define NFILE 512 /* maximum number of server connections per process */
#else
#define NFILE 100 /* maximum number of server connections per process */
#endif

#define RPC_ERROR	1
#define GOOD_SVC_CONN	0
#define BAD_SVC_CONN	3

#define DELAY_TIME	10

/*
 * asynchronous + event symbols
 */

#define D_ASYNCH_TYPE 0x10000000
#define D_EVENT_TYPE  0x20000000

#define EVENT_MAX_CLIENTS 100

/* 
 * timeouts for rpc calls
 */
#ifdef __cplusplus
extern "C" {
#endif

extern struct timeval zero_timeout;

/*  timeout for a client server connection  */
extern struct timeval timeout;
extern struct timeval retry_timeout;

/*  timeout for a server server connection  */
extern struct timeval inner_timeout;
extern struct timeval inner_retry_timeout;

/*  internal timeout for api library functions  */
extern struct timeval api_timeout;
extern struct timeval api_retry_timeout;

/*  timeout for a message server connection  */
extern struct timeval msg_timeout;
extern struct timeval msg_retry_timeout;

/*  timeout for database server connection  */
extern struct timeval dbase_timeout;
extern struct timeval dbase_retry_timeout;

/* timeout for asynchronous calls i.e. maximum time to wait for reply after 
 * sending request, after this time declare the request as failed due to TIMEOUT
 */
extern struct timeval asynch_timeout;

/*  default timeout for import */
extern struct timeval import_timeout;
extern struct timeval import_retry_timeout;

/*  timeout for the select in rpc_check_host   */
extern struct timeval check_host_timeout;

#ifdef __cplusplus
}
#endif

/*
 *  status flags for device server configuration
 */

typedef struct {
		DevBoolean 	configuration;
		DevBoolean 	message_server;
		DevBoolean 	database_server;
		DevBoolean 	startup;
		DevBoolean 	security;
		char		server_name[PROC_NAME_LENGTH];
		char		server_host[HOST_NAME_LENGTH];
		long		prog_number;
		long		vers_number;
		DevBoolean	device_server;
		DevBoolean	asynch_rpc;
		DevBoolean	no_database;
		int		device_no;
		char		**device_list;
	       } configuration_flags; 
	      
/*
 * configuration structure for the database server
 */

typedef struct {
		devserver	conf;
		} dbserver_info;

/*
 * configuration structure for the message server
 */

typedef struct {
		devserver 	conf;
		char  		DS_name [256];
		char  		DS_host [32];
		long		DS_prog_number;
		char  		DS_display [32];
		} msgserver_info;

/*
 * the following structure groups all the information needed per
 * nethost (i.e. control system) in a multi-nethost environment
 *
 * config_flags - indicate what has been imported and if security
 *                has been switched on
 * nethost - connection to a nethost
 * db_info - connection to the corresponding static database
 * msg_info - connection to the corresponding message server
 */

typedef struct {
		char			nethost[SHORT_NAME_SIZE];
		configuration_flags 	config_flags;
		devserver		nh_info;
		devserver		db_info;
		devserver		msg_info;
		} nethost_info;

/*
 * the following structure contains all the information needed by
 * the client to receive replies from asynchronous dev_putget() calls 
 *
 * asynch_id - asynchronous call identifier
 * argout_type - argout type requested by client
 * argout - pointer to argout argument of client
 */

typedef struct {
		short flag;
		long type;
		long event;
		struct timeval timesent;
		devserver ds;
                long asynch_id;
		DevArgument argout;
		DevType argout_type;
		DevCallbackFunction *callback;
		void *user_data;
		} asynch_request_arg;

typedef struct { 
		long pending;
		asynch_request_arg *args;
		} asynch_request;

struct _asynch_client_data {
	DevLong asynch_id;
        DevLong status;
        DevLong error;
        DevLong argout_type;
        DevArgument argout;
        DevVarArgumentArray var_argument;
};
typedef struct _asynch_client_data _asynch_client_data;

struct _asynch_client_raw_data {
	DevLong asynch_id;
        DevLong status;
        DevLong error;
        DevLong ser_argout_type;
        DevLong deser_argout_type;
        DevLong xdr_length;
        DevArgument argout;
        DevVarArgumentArray var_argument;
};
typedef struct _asynch_client_raw_data _asynch_client_raw_data;

typedef struct {
		short flag;
		long event;
		long ds_id;
#ifndef __cplusplus
		DevServer ds;
#else
		Device *device;
#endif /* __cplusplus */
		long id;
		char *server_name;
		char *server_host;
		long prog_number;
		long vers_number;
		long no_svc_conn;
		long argout_type;
		} event_client;
/*
 * definitions for message transfer to message server
 */

#define NUMBER_OF_MSG_TYPES  3
#define ERROR_TYPE  	0
#define DEBUG_TYPE  	1
#define DIAG_TYPE   	2

typedef struct {
                 short 	init_flg;
		 int 	nbytes;
		 char  *messages;
               } _message_buffer;

#ifdef __cplusplus
extern "C" {
#endif
extern _DLLFunc _dev_import_out *rpc_dev_import_4 PT_( (_dev_import_in *dev_import_in, struct svc_req *rqstp) );
extern _DLLFunc _dev_free_out * rpc_dev_free_4 PT_( (_dev_free_in *dev_free_in) );
extern _DLLFunc _client_data * rpc_dev_putget_4 PT_( (_server_data *server_data) );
/*event query */
extern _DLLFunc _dev_queryevent_out *  rpc_dev_event_query_4 PT_((_dev_query_in *dev_query_in));
/*end event query*/
extern _DLLFunc _client_data * rpc_dev_put_4 PT_( (_server_data *server_data) );
extern _DLLFunc _client_raw_data * rpc_dev_putget_raw_4 PT_( (_server_data *server_data));
extern _DLLFunc _client_data *  rpc_dev_put_asyn_4 PT_( (_server_data *server_data) );
extern _DLLFunc _dev_query_out *  rpc_dev_cmd_query_4 PT_((_dev_query_in *dev_query_in));
extern _DLLFunc _dev_import_out *  rpc_dev_ping_4 PT_( (_dev_import_in *dev_import_in, struct svc_req *rqstp) );
extern _DLLFunc _dev_import_out *  rpc_asynch_import_5 PT_( (_dev_import_in *dev_import_in));
extern _DLLFunc _dev_free_out *  rpc_asynch_free_5 PT_( (_dev_free_in *dev_free_in));
extern _DLLFunc _asynch_client_data *  rpc_asynch_reply_5 PT_( (_asynch_client_data *asynch_client_data) );
extern _DLLFunc _asynch_client_raw_data *  rpc_raw_asynch_reply_5 PT_( (_asynch_client_raw_data *asynch_client_raw_data) );
extern _DLLFunc _dev_import_out * rpc_event_listen_5 PT_( (_server_data *event_client_data) );
extern _DLLFunc _dev_free_out * rpc_event_unlisten_5 PT_( (_server_data *event_client_data) );
/*
 * RPC ADMIN service temporarily disabled, to be reimplemented later
 *
 * - andy 26nov96
 *
 *extern _DLLFunc long *  rpc_admin_import_4 PT_( (long *cmd) );
 *extern _DLLFunc long *  rpc_admin_4 PT_((_server_admin *server_admin));
 */
/*
 * Entry point for local dev_putget() calls!
 */
extern _DLLFunc _client_data *  rpc_dev_putget_local PT_( (_server_data *server_data));
/*
 * Entry point for asynchronous command
 */
extern _DLLFunc void rpc_dev_put_asyn_cmd PT_( (_server_data *server_data));
/*
 * Remote procedure stub for message server
 */
extern _DLLFunc _msg_out *  rpc_msg_send_1 PT_( (_msg_data *msg_data) );
/*
 * Remote procedure stub for the manager
 */
extern _DLLFunc _manager_data *  rpc_get_config_4 PT_( (_register_data *register_data) );
extern _DLLFunc _msg_manager_data *  rpc_msg_register_1 PT_( (_register_data *register_data) );
extern _DLLFunc int *  rpc_db_register_1 PT_( (_register_data *register_data) );

/*
 *	Functions used by library only, not for external use !!!
 */
long  dev_rpc_connection PT_( (devserver ds, DevLong *error) );
long  dev_rpc_error PT_( (devserver ds, enum clnt_stat clnt_stat, DevLong *error) );
long  check_rpc_connection PT_( (devserver ds, DevLong *error) );
long  reinstall_rpc_connection PT_( (devserver ds, DevLong *error) );
long  rpc_check_host PT_( (const char *host_name, DevLong *error) );
long  dev_query_svr PT_( (const char *host,long prog_number,long vers_number) );

/*
 * Calls to setup the connection to the manager, database and message server
 */
long setup_config_multi PT_( (const char *nethost, DevLong *error) );
long setup_config PT_( (DevLong *error) );
long db_import_multi PT_( (const char *nethost, DevLong *error) );
long db_ChangeDefaultNethost PT_( (const char* nethost, DevLong *error));


/*
 * Utility functions for multi-nethost support
 */
long 	nethost_alloc PT_( (DevLong *error) );
long 	get_i_nethost_by_device_name PT_( (const char *device_name, DevLong *error) );
long 	get_i_nethost_by_name PT_( (const char *nethost, DevLong *error) );
char* 	get_nethost_by_index PT_( (long i_nethost, DevLong *error) );
char* 	extract_device_name PT_( (const char *full_name, DevLong *error) );
char* 	extract_nethost PT_( (const char *full_name, DevLong *error) );
/*
 * Private functions used to implement the asynchronism
 */
long asynch_rpc_register PT_( (DevLong *error) );
long asynch_server_import PT_( (devserver ds, DevLong *error) );
long asynch_client_import PT_( (devserver ds, DevLong *error) );
long asynch_client_check PT_( (devserver ds, DevLong *error) );
long asynch_add_request PT_( (devserver ds, long asynch_type, long event_type,
	      DevArgument argout, DevType argout_type, 
	      DevCallbackFunction *callback, void *user_data,
	      long *asynch_id, long *asynch_index, DevLong *error) );
long asynch_get_index PT_( (long asynch_id) );
void asynch_client_cleanup PT_( (DevLong *error) );
void asynch_timed_out PT_( (devserver ds) );
long asynch_client_ping PT_( (long i, DevLong *error) );
void event_client_cleanup PT_( (DevLong *error) );

/* Attribute access entry points */

long attribute_import (const char *attribute_name, long access, devserver *ds_ptr, DevLong *error);
long attribute_free (devserver ds, DevLong *error); 
long attribute_putget (devserver ds, long cmd, DevArgument argin, DevType argin_type,DevArgument argout, DevType argout_type, DevLong *error);
long attribute_put (devserver ds, long cmd, DevArgument argin, DevType argin_type, DevLong *error);
long attribute_cmd_query (devserver ds, DevVarCmdArray *varcmdarr, DevLong *error);
long attribute_timeout(devserver ds, long request, 
                       struct timeval *dev_timeout, DevLong *error);
					   
/* TANGO entry points */

long tango_db_import(char*, char*, long, DevLong*);
long tango_dev_import(char*, long, devserver*, DevLong*);
long tango_dev_free(devserver,DevLong*); 
long tango_dev_putget(devserver,long,void*,long,void*,long,DevLong*);
long tango_dev_putget_raw(devserver,long,void*,long,void*,long,DevLong*);
long tango_dev_cmd_query(devserver,DevVarCmdArray*,DevLong*); 
long tango_dev_ping(devserver,DevLong*);
long tango_dev_timeout(devserver ds, long request, 
                       struct timeval *dev_timeout, DevLong *error); 

/* used by TANGO */

long dev_notimported_init(char*, long, long, devserver*, DevLong*);

/* dynamic error string handling */

long dev_error_push PT_( (const char *error_string) );
long dev_error_clear PT_( () );
long dev_error_push_level PT_( (const char * message,int level) );

/*
 * used for relistening events on client reconnect 
 */
long 	relisten_events(devserver ds);

/*
 * used internally for Taco timeout
 */
long taco_dev_timeout (devserver ds, long request, 
			           struct timeval *dev_timeout, DevLong *error);
					   
/*
 * used for determination of the hostname
 */
int taco_gethostname(char *host_name, size_t len);

void _WINAPI devserver_prog_4    PT_( (struct svc_req *rqstp,SVCXPRT *transp) );
#ifdef __cplusplus
}
#endif


/*
 *  rpc service definitions used for to identify the rpc's
 */

/*
 * device related services
 */

#define DEVSERVER_VERS 		((u_long)1) /* The old version number !!!!! */
#define API_VERSION    		((u_long)4) /* The new version of the library !!! */
#define ASYNCH_API_VERSION 	((u_long)5) /* Asynchronous call version for API_VERSION */
#define RPC_DEV_EVENT_QUERY 	((u_long)20)
#define RPC_DEV_IMPORT 		((u_long)1)
#define RPC_DEV_FREE 		((u_long)2)
#define RPC_DEV_PUTGET 		((u_long)3)
#define RPC_DEV_PUT 		((u_long)4)
#define RPC_DEV_CMD_QUERY 	((u_long)5)
#define RPC_ADMIN_IMPORT 	((u_long)6)
#define RPC_ADMIN 		((u_long)7)
#define RPC_CHECK 		((u_long)8)
#define RPC_DEV_PUTGET_RAW 	((u_long)9)
#define RPC_DEV_PUT_ASYN 	((u_long)10)
#define RPC_DEV_PING 		((u_long)11)
#define RPC_DEV_PUTGET_ASYN 	((u_long)11)
#define RPC_DEV_PUTGET_RAW_ASYN ((u_long)12)
#define RPC_PUTGET_ASYN_REPLY 	((u_long)13)
#define RPC_PUTGET_RAW_ASYN_REPLY ((u_long)14)
#define RPC_ASYN_IMPORT 	((u_long)15)
#define RPC_ASYN_FREE 		((u_long)16)
#define RPC_FLUSH 		((u_long)17)
#define RPC_EVENT_LISTEN 	((u_long)18)
#define RPC_EVENT_UNLISTEN	 ((u_long)19)

/*
 *  message server related service
 */
#define MSGSERVER_VERS   	((u_long)1)

#define RPC_MSG_SEND     	((u_long)1)
#define RPC_STARTUP_MSG  	((u_long)98)
#define RPC_QUIT_SERVER  	((u_long)99)

/*
 *  network manager related service
 */

#define NMSERVER_PROG 		((u_long)100)
#define NMSERVER_VERS 		((u_long)4)
#define NMSERVER_VERS_1 	((u_long)1)

#define RPC_GET_CONFIG 	   	((u_long)1)
#define RPC_MSG_REGISTER   	((u_long)2)
#define RPC_DB_REGISTER    	((u_long)3)
#define RPC_GET_RDB_CONFIG 	((u_long)4)
#define RPC_RDB_REGISTER   	((u_long)5)

/*
 * special case for Solaris C++ and OS9 because no prototype defined !
 */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#if 	!HAVE_DECL_GETHOSTNAME && !WIN32
int 	gethostname(char *name, int namelen);
#endif
#if !HAVE_DECL_GET_MYADDRESS
void 	get_myaddress(struct sockaddr_in *);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif
extern configuration_flags  	*config_flags;
extern nethost_info 		*multi_nethost;
extern long			default_nethost;
extern dbserver_info        	db_info;
#ifdef __cplusplus
}
#endif

#ifdef TANGO_API
typedef struct _tango_device {
   public :
          long flag;
          short taco_state;
          //vector<string> cmd_name;
          char **cmd_name;
          long n_cmds;
          long *cmd_value;
          long *argin_type;
          long *argout_type;
          Tango::DeviceProxy *device_proxy;
} _tango_device;
#endif /* TANGO_API */

/* mutex locking for handling asyncronous request */
#ifdef _REENTRANT
#	include <pthread.h>
       	extern pthread_mutex_t async_mutex;
	extern pthread_mutex_t dev_api_mutex;

#	define LOCK(mutex) 	(pthread_mutex_lock(&(mutex)))
#	define UNLOCK(mutex) 	(pthread_mutex_unlock(&(mutex)))
#else /*_REENTRANT*/
#ifndef WIN32
#	define LOCK(mutex) 	void
#	define UNLOCK(mutex) 	void
#else /* !WIN32 */
#	define LOCK(mutex) 
#	define UNLOCK(mutex)
#endif /* !WIN32 */
#endif

#endif /* _apiP_h */

