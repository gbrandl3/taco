/*+*******************************************************************

 File:		ApiP.h

 Project:	Device Servers with sun-rpc

 Description:	Private include for the applications 
		programmers interface.

 Author(s):	Jens Meyer
 		$Author: jkrueger1 $

 Original:	June 1992

 Version:	$Revision: 1.3 $

 Date:		$Date: 2004-03-26 16:21:52 $

 Copyright (c) 1990-1997 by European Synchrotron Radiation Facility, 
                            Grenoble, France

********************************************************************-*/

#ifndef _apiP_h
#define _apiP_h


/*
 *  standard header string to use "what" or "ident".
 */
#ifdef _IDENT
static char ApiPh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/private/ApiP.h,v 1.3 2004-03-26 16:21:52 jkrueger1 Exp $";
#endif /* _IDENT */


/*
 * dsxdr library include files
 */
#include <API_xdr.h>
#ifndef __cplusplus
#	include <DevServer.h>
#else
#	include <DeviceBase.h>
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
#define NFILE 256 /* maximum number of server connections per process */
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

static struct timeval zero_timeout = { 0 , 0 };        /* { sec , usec } */

/*  timeout for a client server connection  */
static struct timeval timeout = { 3 , 0 };        /* { sec , usec } */
#ifndef lynx
static struct timeval retry_timeout = { 3 , 0 };  /* { sec , usec } */
#else /* lynx */
static struct timeval retry_timeout = { 1 , 0 };  /* { sec , usec } */
#endif

/*  timeout for a server server connection  */
static struct timeval inner_timeout = { 2, 0 };        /* { sec , usec } */
#ifndef lynx
static struct timeval inner_retry_timeout = { 2 , 0 };  /* { sec , usec } */
#else /* lynx */
static struct timeval inner_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#endif /* lynx */

/*  internal timeout for api library functions  */
static struct timeval api_timeout = { 1 , 0 };        /* { sec , usec } */
#ifndef lynx
static struct timeval api_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#else /* lynx */
static struct timeval api_retry_timeout = { 0 , 500000 };  /* { sec , usec } */
#endif /* lynx */

/*  timeout for a message server connection  */
static struct timeval msg_timeout = { 1 , 0 };        /* { sec , usec } */
#ifndef lynx
static struct timeval msg_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#else /* lynx */
static struct timeval msg_retry_timeout = { 0 , 500000 };  /* { sec , usec } */
#endif /* lynx */

/*  timeout for database server connection  */
static struct timeval dbase_timeout = { 4, 0 };        /* { sec , usec } */
static struct timeval dbase_retry_timeout = { 4 , 0 };  /* { sec , usec } */

/* timeout for asynchronous calls i.e. maximum time to wait for reply after 
 * sending request, after this time declare the request as failed due to TIMEOUT
 */
static struct timeval asynch_timeout = { 25 , 0 };        /* { sec , usec } */

/*  default timeout for import */
static struct timeval import_timeout = { 2 , 0 };        /* { sec , usec } */
static struct timeval import_retry_timeout = { 2 , 0 };  /* { sec , usec } */

/*  timeout for the select in rpc_check_host   */
static struct timeval check_host_timeout = { 0 , 200000 }; /* { sec , usec } */

/*
 *  status flags for device server configuration
 */

typedef struct {
		DevBoolean 	configuration;
		DevBoolean 	message_server;
		DevBoolean 	database_server;
		DevBoolean 	startup;
		DevBoolean 	security;
		char		server_name[80];
		char		server_host[80];
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
	long asynch_id;
        long status;
        long error;
        long argout_type;
        DevArgument argout;
        DevVarArgumentArray var_argument;
};
typedef struct _asynch_client_data _asynch_client_data;

struct _asynch_client_raw_data {
	long asynch_id;
        long status;
        long error;
        long ser_argout_type;
        long deser_argout_type;
        long xdr_length;
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
		DeviceBase *device;
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
long  dev_rpc_connection PT_( (devserver ds, long *error) );
long  dev_rpc_error PT_( (devserver ds, enum clnt_stat clnt_stat, long *error) );
long  check_rpc_connection PT_( (devserver ds, long *error) );
long  reinstall_rpc_connection PT_( (devserver ds, long *error) );
long  rpc_check_host PT_( (char *host_name, long *error) );
long  dev_query_svr PT_( (char* host,long prog_number,long vers_number) );

/*
 * Calls to setup the connection to the manager, database and message server
 */
long setup_config_multi PT_( (char *nethost, long *error) );
long db_import_multi PT_( (char *nethost, long *error) );
long db_ChangeDefaultNethost PT_( (char* nethost, long *error));


/*
 * Utility functions for multi-nethost support
 */
long 	nethost_alloc PT_( (long *error) );
long 	get_i_nethost_by_device_name PT_( (char *device_name, long *error) );
long 	get_i_nethost_by_name PT_( (char *nethost, long *error) );
char* 	get_nethost_by_index PT_( (long i_nethost, long *error) );
char* 	extract_device_name PT_( (char *full_name, long *error) );
/*
 * Private functions used to implement the asynchronism
 */
long asynch_rpc_register PT_( (long *error) );
long asynch_server_import PT_( (devserver ds, long *error) );
long asynch_client_import PT_( (devserver ds, long *error) );
long asynch_client_check PT_( (devserver ds, long *error) );
long asynch_add_request PT_( (devserver ds, long asynch_type, long event_type,
	      DevArgument argout, DevType argout_type, 
	      DevCallbackFunction *callback, void *user_data,
	      long *asynch_id, long *asynch_index, long *error) );
long asynch_get_index PT_( (long asynch_id) );
void asynch_client_cleanup PT_( (long *error) );
void asynch_timed_out PT_( (devserver ds) );
long asynch_client_ping PT_( (long i, long *error) );
void event_client_cleanup PT_( (long *error) );

/* TANGO entry points */

long tango_db_import(char*, char*, long, long*);
long tango_dev_import(char*, long, devserver*, long*);
long tango_dev_free(devserver,long*); 
long tango_dev_putget(devserver,long,void*,long,void*,long,long*);
long tango_dev_putget_raw(devserver,long,void*,long,void*,long,long*);
long tango_dev_cmd_query(devserver,DevVarCmdArray*,long*); 
long tango_dev_ping(devserver,long*); 

/* used by TANGO */

long dev_notimported_init(char*, long, long, devserver*, long*);

/* dynamic error string handling */

long dev_error_push PT_( (char *error_string) );
long dev_error_clear PT_( () );
long dev_error_push_level PT_( (const char * message,int level) );

/*
 * used for relistening events on client reconnect 
 */
long 	relisten_events(devserver ds);

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
#if defined(solaris) || defined(_UCC)
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int 	gethostname(char *name, int namelen);
void 	get_myaddress(struct sockaddr_in *);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* solaris || _UCC */

#ifdef __cplusplus
extern "C" configuration_flags  config_flags;
extern "C" nethost_info 	*multi_nethost;
extern "C" dbserver_info        db_info;
#endif

/* mutex locking for handling asyncronous request */
#ifdef _REENTRANT
#	include <pthread.h>
       	extern pthread_mutex_t async_mutex;
#	define LOCK(mutex) 	(pthread_mutex_lock(&(mutex)))
#	define UNLOCK(mutex) 	(pthread_mutex_unlock(&(mutex)))
#else /*_REENTRANT*/
#	define LOCK(mutex) 	void
#	define UNLOCK(mutex) 	void
#endif

#endif /* _apiP_h */

