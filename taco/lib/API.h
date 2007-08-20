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
 * File:		API.h
 *
 * Project:	Device Servers with sun-rpc
 *
 * Description:	Public include for applications using
 *		the applications programmers interface.
 *
 * Author(s):	Jens Meyer
 * 		$Author: bourtemb $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.20 $
 *
 * Date:	$Date: 2007-08-20 15:51:07 $
 *
 *********************************************************************-*/

#ifndef _api_h
#define _api_h

/**
 * @defgroup dsAPI Device Server API
 * @ingroup API
 * Functions to access to a device server.
 *
 * Implement the device server (aka TACO) and the pplication programmer's interface (aka DSAPI).
 *
 * @li version of DSAPI e.g. dev_import(), dev_putget(), dev_put()
 * @li the list of DS error messages
 * @li the list of device states (in ascii)
 * @li general purpose DSAPI routines for printing error, debug and diagnostic messages
 * @li implementing DSAPI security
 * @li some more DSAPI related routines e.g. dev_put_asyn()
 */

/**@defgroup dsAPIserver Devicer server calls
 * @ingroup dsAPI
 */

/**@defgroup oicAPI OIC device server calls
 * @ingroup dsAPI
 */

/**
 * @defgroup dsAPIintern Internal functions to the Device Server API
 * @ingroup dsAPI
 * These functions will be called by the Device Server API
 */

/**@defgroup clientAPI Client API
 * @ingroup API
 */

/**@defgroup clientAPIintern Internal funtions of the Device Server Client API
 * @ingroup clientAPI
 */

/**
 * @defgroup syncAPI Synchronous Client API
 * @ingroup clientAPI
 * These functions are used by Device Server API (DSAPI) clients to send a synchronous request
 * to a device server. The notation of client-server refers to sender and receiver of each DSAPI
 * call. This means a device server itself can become a DSAPI client it accesses a device.
 */

/**
 * @defgroup secAPI Interface for the security system
 * @ingroup clientAPI
 *
 */

/**
 * @defgroup secAPIintern Internal functions to the security system
 * @ingroup secAPI
 */
 

/*
 *  standard header string to use "what" or "ident".
 */
#ifdef _IDENT
static char APIh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/API.h,v 1.20 2007-08-20 15:51:07 bourtemb Exp $";
#endif /* _IDENT */

/*
 * Definitions to code and decode the error and command numbers.
 */

#define DS_TEAM_SHIFT	26
#define DS_IDENT_SHIFT	18
#define DS_CAT_SHIFT	12

#define DS_TEAM_MASK	63      /* value = 0x3f */
#define DS_IDENT_MASK	255     /* value = 0xff */
#define DS_CAT_MASK	63      /* value = 0x3f */

/*
 * RedHat 5.0 of Linux needs this otherwise it has missing types
 * when it comes to the RPC part e.g. caddr_t
#ifdef linux
#include <linux/types.h>
#include <assert.h>
#endif
 */

/*
 * Include system commands and states.
 */
#ifndef HAVE_CONFIG_H
#include <private/config.h>
#endif
#include <macros.h>
#include <DevCmds.h>
#include <DevStates.h>

/*
 * xdr library include file
 */
#include <DevXdrKernel.h>

/*
 * database library include file
 */
#include <db_setup.h>

/*
 * Definition of some constants
 */
#define MAX_NAME_LENGTH LONG_NAME_SIZE 	/* maximum length of name fields */
#define MIN_NETHOST 	10		/* minimum number of NETHOSTs */

/*
 * Definitions for the states of the startup configuration
 * flag.
 */
#define SERVER_STARTUP  (-1)

/*
 * Definitions for available protocols
 */
#define D_TCP	888
#define D_UDP	999
#define D_IIOP	1111

/*
 * definitions needed by asynchronous call (should be in macros.h)
 */
#define DS_PENDING 3

/**
 *  Structure for the administration of open RPC connections.
 */
typedef struct _server_connections {
	char            server_host[SHORT_NAME_SIZE];
	long            prog_number;
	long            vers_number;
	CLIENT          *clnt;
	_Int            no_conns;
	/** indicates already detected RPC problems */
	long            rpc_error_flag;
	/** number of times the connection was reestablished */
	_Int            rpc_conn_counter;
	/** indicates the RPC connection status */
	long            rpc_conn_status;
	/** indicates the RPC protocol */
	long            rpc_protocol;
	int             tcp_socket;
	/** timeout for RPC connection */
	struct timeval  rpc_timeout;
	struct timeval  rpc_retry_timeout;
	/** time stamp for delay time of VME access after a reboot */
	time_t          first_access_time;
	/** for the security system with single user connections! */
	long		open_si_connections;
	long		rpc_protocol_before_si;
	/** copy of UDP client handle for synchronous calls */
	CLIENT         	*udp_clnt;
	/** copy of TCP client handle for synchronous calls */
	CLIENT         	*tcp_clnt;
	/** client handle for asynchronous calls */
	CLIENT		*asynch_clnt; 
	/** flag for asynchonous queue flushing */
	DevBoolean     	flushed; 
	/** number of asynchronous requests with reply still pending */
	long		pending; 
	/** timeout for asynchronous requests */
	struct timeval 	asynch_timeout;
	int		asynch_listen_tcp_socket;
	int		asynch_callback_tcp_socket;
} server_connections;

/**
 * type returned by a successful dev_import call, is used in all
 * subsequent calls to the api_lib for this device
 */
typedef struct _devserver {
	char        	device_name[LONG_NAME_SIZE];
	char        	device_class[SHORT_NAME_SIZE];
	char        	device_type[SHORT_NAME_SIZE];
	char        	server_name[LONG_NAME_SIZE];
	char        	server_host[SHORT_NAME_SIZE];
	long        	prog_number;
	long        	vers_number;
	CLIENT      	*clnt;
	long        	ds_id;
	_Int        	no_svr_conn;
	_Int	    	rpc_conn_counter;
	long	    	dev_access;
	long        	i_nethost;
	long        	rpc_protocol;
	struct timeval 	rpc_timeout;
	CLIENT      	*asynch_clnt;
	struct timeval 	asynch_timeout;
	long        	pending;
	int         	no_database;
} *devserver;


/*
 * structures for a dev_cmd_query() call
 */
typedef struct {
	long    cmd;
	char    cmd_name [SHORT_NAME_SIZE];
	char	*in_name;
	long    in_type;
	char	*out_name;
	long    out_type;
} DevCmdInfo;

typedef struct {
	u_int          	length;
	DevCmdInfo 	*sequence;
} DevVarCmdArray;

/*
 * structures for a dev_event_query() call
 */
typedef struct {
	long    event;
	char    event_name [SHORT_NAME_SIZE];
	char	*out_name;
	long    out_type;
} DevEventInfo;

typedef struct {
	u_int          	length;
	DevEventInfo 	*sequence;
} DevVarEventArray;


/*
 * structures for a dev_inform() call
 */
typedef struct {
	char    device_name[LONG_NAME_SIZE];
	char    device_class[SHORT_NAME_SIZE];
	char    device_type[SHORT_NAME_SIZE];
	char    server_name[LONG_NAME_SIZE];
	char    server_host[SHORT_NAME_SIZE];
} DevInfo;

/**
 * structures for dev_putget_asyn() call
 */
typedef struct _DevCallbackData { 	
	long 		asynch_id;
	DevArgument 	argout;
	DevType 	argout_type;
	long 		status;
	long 		error;
	struct timeval 	time; 
} DevCallbackData;

typedef void (*DevCallbackFunction(devserver, void*, DevCallbackData));


/**
 * modes for dev_printerror, dev_printerror_no, dev_printdiag !
 */
#define CLEAR		0
#define WRITE		1
#define SEND      	2

/*
 * data collector library include file
 */
#include <dc.h>


#if !defined(WIN32)
/*
 * HDB library include file
 */
#include <hdb_out.h>
#endif

/*
 *  Function definitions for API-library
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern _DLLFunc long  dev_import
				PT_( (char *dev_name, long access,
				devserver *ds_ptr, long *error) );
extern _DLLFunc long  taco_dev_import
				PT_( (char *dev_name, long access,
				devserver *ds_ptr, long *error) );
extern _DLLFunc long  dev_putget
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				DevArgument argout, DevType argout_type,
				long *error) );
extern _DLLFunc long  taco_dev_putget
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				DevArgument argout, DevType argout_type,
				long *error) );
extern _DLLFunc long  dev_put
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				long *error) );
extern _DLLFunc long  taco_dev_put
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				long *error) );
extern _DLLFunc long  dev_putget_raw
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				DevOpaque *argout,  DevType argout_type,
				long *error) );
extern _DLLFunc long  dev_put_asyn
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				long *error) );
extern _DLLFunc long  dev_free
				PT_( (devserver ds, long *error) );
extern _DLLFunc long  taco_dev_free
				PT_( (devserver ds, long *error) );
extern _DLLFunc long  dev_xdrfree
				PT_( (DevType type, DevArgument objptr,
				long *error) );
extern _DLLFunc long  dev_cmd_query
				PT_( (devserver ds, DevVarCmdArray *varcmdarr,
				long *error) );
extern _DLLFunc long  taco_dev_cmd_query
                                PT_( (devserver ds, DevVarCmdArray *varcmdarr,
                                long *error) );
extern _DLLFunc long  dev_event_query
				PT_( (devserver ds, DevVarEventArray *vareventarr,
				long *error) );
extern _DLLFunc long  dev_inform
				PT_( (devserver *clnt_handles, long num_devices,
				DevInfo * *dev_info, long *error) );
extern _DLLFunc long  dev_rpc_protocol
				PT_( (devserver ds, long protocol,
				long *error) );
extern _DLLFunc long  dev_rpc_timeout
				PT_( (devserver ds, long request,
				struct timeval *timeout, long *error) );
extern _DLLFunc long  dev_ping
				PT_( (devserver ds, long *error) );
extern _DLLFunc long  dev_import_timeout
				PT_( (long request, struct timeval *timeout, 
				long *error) );

extern _DLLFunc long   db_import
				PT_( (long *error) );
extern _DLLFunc long   msg_import
				PT_( (char *DS_name, char *DS_host,
				long DS_prog_number, char *DS_display,
				long *error) );
extern _DLLFunc char * dev_error_str
				PT_( (long dev_errno) );
extern _DLLFunc void  dev_printerror_no
				PT_( (short mode, char *comment,
				long dev_errno) );
extern _DLLFunc long  dev_putget_asyn
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				DevArgument argout, DevType argout_type,
				DevCallbackFunction *callback, void *user_data,
				long *asynch_id, long *error) );

extern _DLLFunc long  dev_putget_raw_asyn
				PT_( (devserver ds, long cmd,
				DevArgument argin,  DevType argin_type,
				DevArgument argout, DevType argout_type,
				DevCallbackFunction *callback, void *user_data,
				long *asynch_id, long *error) );

extern _DLLFunc long dev_asynch_timeout 
				PT_( (devserver ds, long request,
				struct timeval *dev_timeout, long *error) );

extern _DLLFunc long dev_event_listen 
				PT_( (devserver ds, long event_type,
				DevArgument argout, DevType argout_type,
				DevCallbackFunction *callback, void *user_data,
				long *event_id, long *error) );

extern _DLLFunc long dev_event_unlisten 
				PT_( (devserver ds, long event_type,
				long event_id, long *error) );

extern _DLLFunc long dev_synch PT_( (struct timeval *timeout, long *error) );

extern _DLLFunc long dev_pending PT_( (devserver) );

#ifdef __cplusplus
}
#endif /* __cplusplus */

extern _DLLFunc long debug_flag;	/* global that is used in gen_api.c */

/*
 * For these functions, ANSI portotyping is in todays form not possible, because their
 * functionality base on type conversion for input arguments.
 *
 * adding ANSI C support for function with variable argument lists
 *
 * - andy , 13sep94
 */
#if defined (__STDC__) || (__STDCPP__)
#	ifdef __cplusplus
		extern "C" {
#	endif
#	if (!defined _GNU_PP)
		extern _DLLFunc void  dev_printerror	PT_( (DevShort mode, char *var_args, ...) );
		extern _DLLFunc void  dev_printdebug	PT_( (long mode, char *var_args, ...) );
		extern _DLLFunc void  dev_printdiag	PT_( (DevShort mode, char *var_args, ...) );
#	else
		extern _DLLFunc void  dev_printerror	(DevShort mode,char *fmt, ... );
		extern _DLLFunc void  dev_printdebug	(DevLong mode, char *fmt,  ... );
		extern _DLLFunc void  dev_printdiag	(DevShort mode, char *fmt,  ... );
#	endif
#	ifdef __cplusplus
		}
#	endif /* __cplusplus */
#else /* STDC */
	extern _DLLFunc void  dev_printerror();
	extern _DLLFunc void  dev_printdebug();
	extern _DLLFunc void  dev_printdiag();
#endif /* __STDC__ */

#ifdef __cplusplus
extern "C" {
#endif
/* global variable containing dynamic error string */

extern char *dev_error_string;

int device_server (char *server_name, char *pers_name, int m_opt, int s_opt, int nodb, int pn, int n_device, char** device_list); 
#ifdef __cplusplus
}
#endif
#endif /* _api_h */


