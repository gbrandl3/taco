/*+*******************************************************************

 File       :	asyn_api.c

 Project    :	Device Servers with SUN-RPC

 Description:	Asynchronous Application Programmer's Interface

		Source code to implement the asynchronous part
		of the Device Server C API (DSAPI). Includes all
		calls directly or indirectly related to the asynchronous
		DSAPI. 

		The asynchronous api can also be used to receive events.
		An event is simply an asynchronous call which can reoccur
		as opposed to a simple asynchronous call which has only
		one reply. The code has been modified to support events 
		as a type of asynchronism using the same mechanisms.
		Each asynchronous call will be identified by its type
		which can be D_ASYNCH_TYPE for simple asynchonous
		calls and D_EVENT_TYPE for asynchronous events.

 Author(s)  :	Andy Goetz
 		$Author: jkrueger1 $

 Original   :	January 1997

 Version:	$Revision: 1.10 $

 Date:		$Date: 2004-03-26 16:21:52 $

 Copyright (c) 1997-2000 by European Synchrotron Radiation Facility,
                            Grenoble, France

********************************************************************-*/

#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>
#include <API_xdr_vers3.h>
#include <errno.h>
#include <assert.h>

#if !defined (_NT)
#	if ( defined (OSK) || defined (_OSK))

#		include <inet/socket.h>
#		include <inet/netdb.h>
#	else
#		if defined (sun) || defined (irix)
#			include <sys/filio.h>
#		endif /* sun */
#		if HAVE_SYS_SOCKET_H
#			include <sys/socket.h>
#		else
#			include <socket.h>
#		endif
#		if !defined (vxworks)
#			include <netdb.h>
#		else
#			include <rpcGbl.h>
#		endif /* !vxworks */
#		ifdef lynx
#			include <ioctl.h>
#		endif /*lynx */
#		if HAVE_SYS_TYPES_H
#			include <sys/types.h>
#		endif
#		if HAVE_SYS_IOCTL_H
#			include <sys/ioctl.h>
#		endif
#		if defined (linux) || defined (FreeBSD) 
/* mutex locking for handling asyncronous request */
/* here the mutex is instantiated. */
#			ifdef _REENTRANT
#				include <pthread.h>
				pthread_mutex_t async_mutex = PTHREAD_MUTEX_INITIALIZER;
#			endif
#		endif /* linux */
#	endif /* OSK || _OSK */
#endif /* _NT */

#ifdef _UCC
#	include <rpc/rpc.h>
#	include <_os9_sockets.h>
/* 
 * findstr() was forgotten in the OS9 include files
 * therefore define prototype here
 *
 * #ifdef __cplusplus
 * extern "C" int findstr(int pos, char *string, char *pattern);
 * #else
 * int findstr(int pos, char *string, char *pattern);
 * #endif * __cplusplus *
 */
/*#include <os9time.h>*/
#endif /* _UCC */

#ifdef __cplusplus
extern "C" {
#endif
	extern configuration_flags config_flags;
	extern nethost_info *multi_nethost;
/*
 * global dynamic array of pending asynchronous requests used to store 
 * info needed to receive asynchronous replies
 */
	extern server_connections svr_conns[];
	extern int ds_rpc_svc_fd; /* global variable - client rpc file descriptor */
/* 
 * dynamic error variables
 */
	extern char *dev_error_stack;
	extern char *dev_error_string;
#ifdef __cplusplus
};
#endif

asynch_request client_asynch_request = {0, NULL};

/* DIRTY FIX - the following 2 lines should be in API_xdr.h */

#ifdef __cplusplus 
extern "C" {
#endif
bool_t _DLLFunc xdr__asynch_client_data PT_((XDR *xdrs, _asynch_client_data *objp));
bool_t _DLLFunc xdr__asynch_client_raw_data PT_((XDR *xdrs, _asynch_client_raw_data *objp));
#ifdef __cplusplus 
}
#endif


/*
 * global variables indicating the sockets used by the synchronous and
 * asynchronous rpc services (used by dev_synch())
 */
long synch_svc_udp_sock;
long synch_svc_tcp_sock;
long asynch_svc_tcp_sock;

/**
 * @defgroup asyncAPI Asynchronous Device Client API
 * @ingroup clientAPI
 * These functions are used by the DSAPI clients to send and receive aynchronously request to a
 * device server. The notion of client-server refers to sender and receiver of each DSAPI call.
 * This means a device server itself can become a DSAPI client if it accesses a device.
 */

/**@ingroup asyncAPI
 * This function executes a command asynchronously on the device associated with the passed
 * client handle. The device must be remote and compiled with V6. Input and output data types
 * must correspond to the types specified for this command in the device server's command list.
 * Otherwise an error code will be returned. All arguments have to be passed as pointers.
 *
 * Memory for outgoing arguments will be automatically allocated by XDR, if pointers are 
 * initialised to @b NULL. To free the memory allocated by XDR afterwards, the function @b
 * dev_xdrfree() must be used.
 * 
 * The client continues immediatly and does not wait for the server to execute the request.
 * The callback function has to be specified otherwise an error will be returned. The callback 
 * function is triggred by making a call to @ref dev_synch(). The client can pass data to the
 * callback function via userdata. The callback function receives the device server handle, 
 * user data, and a DevCallbackData structure as input. The function returns a (unique) id in
 * asynch_id for each call.
 * 
 * The command request will be dispatched to the server so that the client can continue.
 * the result will be sent back to the client once the command has finished executing. It is 
 * up to the client to resynchronise so that it can recuperate the result (c.f. using the 
 * dev_synch(), dev_wait() etc. calls).  This version is with callback i.e. the client has to
 * specify a callback.
 * 
 * @param ds       	handle to access the device.
 * @param cmd           command to be executed.
 * @param argin		pointer to input arguments.
 * @param argin_type	data type of input arguments.
 * @param callback	callback routine to be triggered  on completion
 * @param user_data	pointer to user data to be passed to callback function
 * @param argout	pointer for output arguments.
 * @param argout_type 	data type of output arguments.
 * @param asynch_id_ptr asynch call identifier
 * @param error         Will contain an appropriate error * code if the 
 *			corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_putget_asyn (devserver ds, long cmd, 
                               DevArgument argin, DevType argin_type, 
                               DevArgument argout, DevType argout_type, 
                               DevCallbackFunction *callback, void *user_data,
			       long *asynch_id_ptr, long *error)
{
	_server_data		server_data;
	DevVarArgument		vararg[10];
	enum clnt_stat		clnt_stat;
	_Int			local_flag;
	long			client_id = 0,
				i_nethost;
	nethost_info		*nethost;
	short			iarg;
	long			rstatus,
				asynch_id,
				asynch_index,
				asynch_type;
	DevString		name,host;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_putget_asyn() : entering routine\n");

#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
        {
		*error = DevErr_CommandNotImplemented;
                return(DS_NOTOK);
        }
#endif /* TANGO */
/*
 * make sure dynamic error is initialised to nothing
 */
	if (dev_error_string != NULL)
	{
		free(dev_error_string);
		dev_error_string = NULL;
	}
/*
 *  check data types
 */
	if (  argin_type < 0 || argout_type < 0)
	{
		*error = DevErr_DevArgTypeNotRecognised;
		return(DS_NOTOK);
	}

	if ( callback == NULL)
	{
		*error = DevErr_NoCallbackSpecified;
		return(DS_NOTOK);
	}

/*
 * save the device's nethost in an intermediate variable
 * to make it more accessible
 */
	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];
	}


/*
 * Identify a local device.
 * Device is local if local_flag == True.
 */
	local_flag =(_Int)( ds->ds_id >> LOCALACCESS_SHIFT);
	local_flag = local_flag & LOCALACCESS_MASK;

/*
 * Verify the RPC connection if the device is not local.
 */
	if ( local_flag != True && (dev_rpc_connection (ds, error)  == DS_NOTOK))
		return (DS_NOTOK);

/*
 * If the security system is configured, 
 * verify the security key
 */
	if (!ds->no_database && (nethost->config_flags.security == True)
		&& ( verify_sec_key (ds, &client_id, error) == DS_NOTOK))
			return (DS_NOTOK);

/*
 * in order to do an asynchronous call the client needs to register
 * an RPC service to receive the answer. Check if the client has done
 * this, if not then register the client.
 */
	if (config_flags.asynch_rpc != True)
	{
		rstatus = asynch_rpc_register(error);
		if (rstatus != DS_OK) 
			return(DS_NOTOK);
	}
	
/*
 *  make sure that the asynchronous version of the device
 *  server is imported.
 */
	if (asynch_server_import(ds,error) != DS_OK)
		return (DS_NOTOK);

/*
 * store the pointers to the return arguments so that when reply can
 * be passed back asynchronously to the client
 */
	asynch_type = D_ASYNCH_TYPE;
	if (asynch_add_request(ds, asynch_type, 0, argout, argout_type, callback, user_data, &asynch_id, &asynch_index, error) != DS_OK)
		return(DS_NOTOK);

/*
 *  fill in data transfer structures server_data
 *  and client_data.
 */

	server_data.ds_id	= ds->ds_id;
	server_data.client_id	= client_id;
	server_data.access_right= ds->dev_access;
	server_data.cmd		= cmd;
	server_data.argin_type	= argin_type;
	server_data.argout_type	= argout_type;
	server_data.argin	= (char *) argin;

	dev_printdebug (DBG_ASYNCH, "dev_putget_asyn() : server data -> \n");
	dev_printdebug (DBG_ASYNCH, "ds_id=%d  cmd=%d  intype=%d  outtype=%d\n",
	    	server_data.ds_id, server_data.cmd, server_data.argin_type, server_data.argout_type);

/*
 * additional arguments, in this case asynch_id and the return argument
 * pointers are passed to the device server via the variable argument 
 * array
 */
	server_data.var_argument.length = iarg = 0;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&asynch_id;
	server_data.var_argument.length++; 
	iarg++;

	name = config_flags.server_name;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&name;
	server_data.var_argument.length++; 
	iarg++;
	host = config_flags.server_host;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&host;
	server_data.var_argument.length++; 
	iarg++;
 
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.prog_number;
	server_data.var_argument.length++; 
	iarg++;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.vers_number;
	server_data.var_argument.length++;
	server_data.var_argument.sequence = vararg;

        dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_putget_asyn() : client data -> ");
        dev_printdebug (DBG_ASYNCH, "asynch_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
	    asynch_id,config_flags.server_name,config_flags.server_host, config_flags.prog_number,config_flags.vers_number);

/* 
 * the asynchronous call uses one-way RPC to dispatch the call which
 * means no return arguments i.e. client_data not used
 *
 *	memset ((char *)&client_data, 0, sizeof (client_data));
 *	client_data.argout	= (char *) argout;
 *
 * Check if the device is a local or a remote device.
 * If local, call the local putget function.
 */
	if ( local_flag == True )
	{
/*
 * local asynchronous calls NOT supported at present, it is anyway not sure
 * what this means if there is only one thread in the server - must think
 * about this one
 *
 * andy 12feb97
 *
 *		return ( dev_putget_local (&server_data, &client_data, error) );
 */
		*error = DevErr_AsynchronousCallsNotSupported;
		return(DS_NOTOK);
	}

/*
 *  call RPC_DEV_PUTGET_ASYN using the client handle which has
 *  been opened to the asynchronous service of the server and
 *  ONE-WAY rpc i.e. xdr routine = xdr_void && timeout = zero
 */
	clnt_stat = clnt_call (ds->asynch_clnt, RPC_DEV_PUTGET_ASYN,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
	 dev_printdebug (DBG_ASYNCH, "\ndev_putget_asyn() : clnt_stat %d\n",clnt_stat);

/*
 * Check for errors on the RPC connection.
 */
	if ( (clnt_stat != RPC_TIMEDOUT) && (dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK))
	{
/*
 * remove pending request from list
 */
		client_asynch_request.args[asynch_index].flag = DS_FALSE;
		client_asynch_request.pending--;
		ds->pending--;
		return (DS_NOTOK);
	}

/*
 * return the asynch_id to the client for identification purposes
 */
	*asynch_id_ptr = asynch_id;
	return (DS_OK);
}

/**@ingroup asyncAPI
 * Application interface to execute commands on a device asynchronously with the possibility 
 * to pass input data and to receive output data in raw format. raw format means that the XDR 
 * data are not decoded by the client instead they are returned as a string of bytes whichthe 
 * command request will be dispatched to the server so that the client can continue.
 *
 * The result will be sent back to the client once the command has finished executing. it is up 
 * to the client to resynchronise so that it can recuperate the result (c.f. using the dev_synch(), 
 * dev_wait() etc. calls).
 *
 * This version is with callback i.e. the client has to specify a callback.
 * 
 * @param ds       	handle to access the device.
 * @param cmd           command to be executed.
 * @param argin  	pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 * @param callback  	callback routine to be triggered on completion
 * @param user_data	pointer to user data to be passed to callback function
 * 
 * @param argout  	pointer for raw output arguments.
 * @param argout_type   data type of output arguments.
 * @param asynch_id_ptr	asynch call identifier
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_putget_raw_asyn (devserver ds, long cmd, 
                                DevArgument argin, DevType argin_type, 
                                DevArgument argout, DevType argout_type, 
                                DevCallbackFunction *callback, void *user_data,
			        long *asynch_id_ptr, long *error)
{
	_server_data		server_data;
	DevVarArgument		vararg[10];
	enum clnt_stat		clnt_stat;
	_Int			local_flag;
	long			client_id = 0;
	long			i_nethost;
	nethost_info		*nethost;
	short			iarg;
	long			rstatus;
	long			asynch_id;
	long			asynch_index;
	long			asynch_type;
	DevString		name,host;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_putget_asyn() : entering routine\n");

#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
        {
		*error = DevErr_CommandNotImplemented;
                return(DS_NOTOK);
        } 
#endif /* TANGO */
/*
 *  check data types
 */
	if (  argin_type < 0 || argout_type < 0)
	{
		*error = DevErr_DevArgTypeNotRecognised;
		return(DS_NOTOK);
	}

	if ( callback == NULL)
	{
		*error = DevErr_NoCallbackSpecified;
		return(DS_NOTOK);
	}

/*
 * save the device's nethost in an intermediate variable
 * to make it more accessible
 */
	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];
	}


/*
 * Identify a local device.
 * Device is local if local_flag == True.
 */
	local_flag =(_Int)( ds->ds_id >> LOCALACCESS_SHIFT);
	local_flag = local_flag & LOCALACCESS_MASK;

/*
 * Verify the RPC connection if the device is not local.
 */
	if ( (local_flag != True ) && ( dev_rpc_connection (ds, error)  == DS_NOTOK))
		return (DS_NOTOK);

/*
 * If the security system is configured, 
 * verify the security key
 */
	if (!ds->no_database && ( nethost->config_flags.security == True)
		&& ( verify_sec_key (ds, &client_id, error) == DS_NOTOK ))
		return (DS_NOTOK);

/*
 * in order to do an asynchronous call the client needs to register
 * an RPC service to receive the answer. Check if the client has done
 * this, if not then register the client.
 */
	if (config_flags.asynch_rpc != True)
	{
		rstatus = asynch_rpc_register(error);
		if (rstatus != DS_OK) return(DS_NOTOK);
	}
	
/*
 *  make sure that the asynchronous version of the device
 *  server is imported.
 */
	if (asynch_server_import(ds,error) != DS_OK)
		return (DS_NOTOK);

/*
 * store the pointers to the return arguments so that when reply can
 * be passed back asynchronously to the client
 */
	asynch_type = D_ASYNCH_TYPE;
	if (asynch_add_request(ds, asynch_type, 0, argout, argout_type, callback, user_data, &asynch_id, &asynch_index, error) != DS_OK)
		return(DS_NOTOK);

/*
 *  fill in data transfer structures server_data
 *  and client_data.
 */
	server_data.ds_id	= ds->ds_id;
	server_data.client_id	= client_id;
	server_data.access_right= ds->dev_access;
	server_data.cmd		= cmd;
	server_data.argin_type	= argin_type;
	server_data.argout_type	= argout_type;
	server_data.argin	= (char *) argin;

	dev_printdebug (DBG_ASYNCH, "dev_putget_asyn() : server data -> \n");
	dev_printdebug (DBG_ASYNCH, "ds_id=%d  cmd=%d  intype=%d  outtype=%d\n",
	    server_data.ds_id, server_data.cmd, server_data.argin_type, server_data.argout_type);

/*
 * additional arguments, in this case asynch_id and the return argument
 * pointers are passed to the device server via the variable argument 
 * array
 */
	server_data.var_argument.length = iarg = 0;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&asynch_id;
	server_data.var_argument.length++; 
	iarg++;

	name = config_flags.server_name;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&name;
	server_data.var_argument.length++; 
	iarg++;

	host = config_flags.server_host;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&host;
	server_data.var_argument.length++; 
	iarg++;
 
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.prog_number;
	server_data.var_argument.length++; 
	iarg++;

	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.vers_number;
	server_data.var_argument.length++;
	server_data.var_argument.sequence = vararg;

        dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_putget_asyn() : client data -> ");
        dev_printdebug (DBG_ASYNCH, "asynch_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
	    asynch_id,config_flags.server_name,config_flags.server_host,
	    config_flags.prog_number,config_flags.vers_number);

/* 
 * the asynchronous call uses one-way RPC to dispatch the call which
 * means no return arguments i.e. client_data not used
 *
 *	memset ((char *)&client_data, 0, sizeof (client_data));
 *	client_data.argout	= (char *) argout;
 *
 * Check if the device is a local or a remote device.
 * If local, call the local putget function.
 */

	if ( local_flag == True )
	{
/*
 * local asynchronous calls NOT supported at present, it is anyway not sure
 * what this means if there is only one thread in the server - must think
 * about this one
 *
 * andy 12feb97
 *
 *		return ( dev_putget_local (&server_data, &client_data, error) );
 */
		*error = DevErr_AsynchronousCallsNotSupported;
		return(DS_NOTOK);
	}

/*
 *  call RPC_DEV_PUTGET_RAW_ASYN using the client handle which has
 *  been opened to the asynchronous service of the server and
 *  ONE-WAY rpc i.e. xdr routine = xdr_void && timeout = zero
 */

	clnt_stat = clnt_call (ds->asynch_clnt, RPC_DEV_PUTGET_RAW_ASYN,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
/*
 * DO NOT use BATCH rpc as default, the majority opinion is that
 * the client request should be dispatched asap; BATCHED can be offered
 * as an option in the future
 *
 * andy 11jul97
 *
 *	clnt_stat = clnt_call (ds->asynch_clnt, RPC_DEV_PUTGET_RAW_ASYN,
 *		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
 *		    (xdrproc_t)NULL, (caddr_t) NULL, TIMEVAL(zero_timeout));
 */
	 dev_printdebug (DBG_ASYNCH, "\ndev_putget_asyn() : clnt_stat %d\n",clnt_stat);

/*
 * Check for errors on the RPC connection.
 */
	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
	{
/*
 * remove pending request from list
 */
		client_asynch_request.args[asynch_index].flag = DS_FALSE;
		client_asynch_request.pending--;
		ds->pending--;
		return (DS_NOTOK);
	}

/*
 * return the asynch_id to the client for identification purposes
 */
	*asynch_id_ptr = asynch_id;
	return (DS_OK);
}

static fd_set asynch_readfds;

/**@ingroup dsAPI
 * Entry point for asynchronous RPCs requests sent to
 * servers by clients and their asynchronous replies
 * sent to clients by servers.
 * 
 * Will switch to the requested remote procedure.
 * 
 * @param rqstp  RPC request handle
 * @param transp Service transport handle
 */
static void _WINAPI devserver_prog_5 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	struct _devserver 	client;
	DevVarArgument 		vararg[10];
	long 			asynch_id,
				iarg,
				error;
	static struct timeval 	timenow;
#ifndef WIN32
	static struct timezone 	tz;
#endif /* !WIN32 */
#if defined (vxworks) || (WIN32)
	time_t 			tea_time;
#endif /* vxworks */
	enum clnt_stat 		clnt_stat;

	union {
		_server_data 		server_data;
		_client_data 		client_data;
		_asynch_client_data 	asynch_client_data;
		_asynch_client_raw_data asynch_client_raw_data;
	} argument;

	_client_data 		*client_data;
	_client_raw_data 	*client_raw_data;
	_asynch_client_data 	asynch_client_data;
	_asynch_client_raw_data asynch_client_raw_data;
	char 			*result;
	xdrproc_t 		xdr_argument;
#ifdef __cplusplus
	DevRpcLocalFunc 	local;
#else
	char 			*(*local)();
#endif

        dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : rqstp->rq_proc=%d\n",rqstp->rq_proc);

/*
 *  call the right server routine
 */
	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
			return;
/*
 * note : the asynchronous import and free are called automatically
 *        once for every client-server connection, this differs 
 *        from the device import and free which are called explicitly
 *        once for every device. the asynchronous import and free
 *        are synchronous in their execution i.e. the client blocks
 *        waiting for a reply
 */
		case RPC_ASYN_IMPORT:
			xdr_argument = (xdrproc_t)xdr__dev_import_in;

#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_asynch_import_5;
#else
			local = (char *(*)()) rpc_asynch_import_5;
#endif
			break;
		case RPC_ASYN_FREE:
			xdr_argument = (xdrproc_t)xdr__dev_free_in;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_asynch_free_5;
#else
			local = (char *(*)()) rpc_asynch_free_5;
#endif
			break;
		case RPC_DEV_PUTGET_ASYN:
			xdr_argument = (xdrproc_t)xdr__server_data;
/*
 * note : the asynchronus putget calls the same function i.e. rpc_dev_putget_4
 *        as the synchronous putget to execute the client request, it differs
 *        only in the reception and sending of arguments
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_4;
#else
			local = (char *(*)()) rpc_dev_putget_4;
#endif
			break;
		case RPC_DEV_PUTGET_RAW_ASYN:
			xdr_argument = (xdrproc_t)xdr__server_data;
/*
 * note : the asynchronus putget calls the same function i.e. rpc_dev_putget_4
 *        as the synchronous putget to execute the client request, it differs
 *        only in the reception and sending of arguments
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_raw_4;
#else
			local = (char *(*)()) rpc_dev_putget_raw_4;
#endif
			break;
		case RPC_PUTGET_ASYN_REPLY :
			xdr_argument = (xdrproc_t)xdr__asynch_client_data;
/*
 * this is the rpc service to receive replies asynchronously in the
 * client via an rpc call. it main work is to receive the reply, identify
 * the originating request and unpack the arguments correctly. if a 
 * callback has been registered it will be called.
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_asynch_reply_5;
#else
			local = (char *(*)()) rpc_asynch_reply_5;
#endif
			break;
		case RPC_PUTGET_RAW_ASYN_REPLY :
			xdr_argument = (xdrproc_t)xdr__asynch_client_raw_data;
/*
 * this is the rpc service to receive replies for dev_putget_raw_asyn()
 * asynchronously in the client via an rpc call. its main work is to 
 * receive the reply, identify the originating request and unpack the 
 * arguments correctly. if a callback has been registered it will be called.
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_raw_asynch_reply_5;
#else
			local = (char *(*)()) rpc_raw_asynch_reply_5;
#endif
			break;
		case RPC_EVENT_LISTEN :
			xdr_argument = (xdrproc_t)xdr__server_data;
/*
 * this is the rpc service to register a client's interest in an
 * event type. it will add the client to the list of registered
 * clients which will be notified when an event of this type
 * is signalled by the device server
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_event_listen_5;
#else
			local = (char *(*)()) rpc_event_listen_5;
#endif
			break;
		case RPC_EVENT_UNLISTEN :
			xdr_argument = (xdrproc_t)xdr__server_data;
/*
 * this is the rpc service to unregister a client's interest in an
 * event type. it will remove the client from the list of registered
 * clients
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_event_unlisten_5;
#else
			local = (char *(*)()) rpc_event_unlisten_5;
#endif
			break;
/*
 * a kind of null NULL procedure which does not send any reply.
 * it is used by asynchronous clients to flush the output buffer
 */
		case RPC_FLUSH:
			FD_SET(transp->xp_sock, &asynch_readfds);
			dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : returning\n");
			return;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));
	dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : going to svc_getargs()\n");
	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND,"%s", "svcerr_decode : server couldn't decode incoming arguments");
		dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : returning\n");
		return;
	}
/*
 * execute request asynchronously i.e. the client has continued 
 * to do something else in the meantime and is not waiting anymore
 */
	result = (*local)(&argument, rqstp);
	if (result == NULL ) 
	{
		dev_printerror (SEND,"%s", "devserver_prog_5 : server couldn't execute rpc service\n");
		svcerr_systemerr(transp);
	}
	else
	{
/*
 * the asynchronous import and free are asynchronous i.e. the client
 * does not wait for the server to reply.
 */
		if ((rqstp->rq_proc == RPC_ASYN_IMPORT) ||
		    (rqstp->rq_proc == RPC_ASYN_FREE))
		{
			if ( (rqstp->rq_proc == RPC_ASYN_FREE))
			{
/*
 * send a reply for free even if this is a asynchronous request otherwise
 * on OS9 (where else) the server blocks after returning from dserver_prog_5()
 *
 * andy - 27/4/98
 */
				svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
       				dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : free does send reply back to client\n");
			}
		        if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument))
        		{
                		dev_printerror (SEND,"%s", "devserver_prog_5() : server couldn't free arguments !!");
                		return;
        		}

		}
/*
 * the event listen call is synchronous i.e. the client
 * will block waiting for the server to reply.
 */
		if ((rqstp->rq_proc == RPC_EVENT_LISTEN) ||
		    (rqstp->rq_proc == RPC_EVENT_UNLISTEN))
		{
       			dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : event listen/unlisten do not send reply to client\n");
			if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument))
        		{
                		dev_printerror (SEND,"%s",
                		"devserver_prog_5() : server couldn't free arguments !!");
                		return;
        		}
		}

/*
 * if this is a request to the server then send the reply to
 * the client now
 */
		if ((rqstp->rq_proc == RPC_DEV_PUTGET_ASYN) ||
	    	(rqstp->rq_proc == RPC_DEV_PUTGET_RAW_ASYN))
		{
/*
 * first extract the necessary client information from the input arguments
 */
			iarg = 0;
			asynch_id = *(long*)argument.server_data.var_argument.sequence[iarg].argument;
			iarg++;

			strncpy(client.server_name,*(char**)argument.server_data.var_argument.sequence[iarg].argument, sizeof(client.server_name));
			iarg++;
			strncpy(client.server_host,*(char**)argument.server_data.var_argument.sequence[iarg].argument, sizeof(client.server_host));
			iarg++;

			client.prog_number = *(long*)argument.server_data.var_argument.sequence[iarg].argument;
			iarg++;
			client.vers_number = *(long*)argument.server_data.var_argument.sequence[iarg].argument;

        		dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : client data -> ");
        		dev_printdebug (DBG_ASYNCH, "asynch_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
		    		asynch_id,client.server_name,client.server_host, client.prog_number, client.vers_number);

/*
 * to send answer back to client asynchronously the client rpc service
 * must be imported. make sure it is imported.
 */
			if (asynch_client_check(&client, &error) != DS_OK)
			{
				dev_printerror (SEND,"%s", "devserver_prog_5 : server couldn't import client to send asynchronous reply");
/*
 * cannot import asynchronous client free the input arguments and return
 */
                        	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument))
                                	dev_printerror (SEND,"%s", "devserver_prog_5() : server couldn't free arguments !!");
                                return;
			}
/*
 * tag asynchronous information onto client_data so that client
 * can identify the reply
 *
 * return time command was executed by server to client tagged on
 * (simulate gettimeofday() with time() on Windows and VxWorks)
 */

#if defined (vxworks) || (WIN32)
			time(&tea_time);
			timenow.tv_sec = tea_time;
			timenow.tv_usec = 0;
#else

			gettimeofday(&timenow,&tz);
#endif /* !vxworks */
			iarg = 0;
			vararg[iarg].argument_type	= D_ULONG_TYPE;
			vararg[iarg].argument = (DevArgument)&timenow.tv_sec;
			iarg++;

			vararg[iarg].argument_type	= D_ULONG_TYPE;
			vararg[iarg].argument = (DevArgument)&timenow.tv_usec;
			iarg++;

			if (rqstp->rq_proc == RPC_DEV_PUTGET_ASYN)
			{
				client_data = (_client_data*)result;
				asynch_client_data.asynch_id = asynch_id;
				asynch_client_data.status = client_data->status;
				asynch_client_data.error = client_data->error;
				asynch_client_data.argout_type = client_data->argout_type;
				asynch_client_data.argout = client_data->argout;
/*
 * if dynamic error then tag it onto the end of the client_data
 */
				if (dev_error_stack != NULL)
				{
					vararg[iarg].argument_type      = D_STRING_TYPE;
					vararg[iarg].argument = (DevArgument)&dev_error_stack;
					iarg++;
				}
				asynch_client_data.var_argument.length = iarg; 
				asynch_client_data.var_argument.sequence = vararg;
		
        			dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : asynchronous client data -> ");
        			dev_printdebug (DBG_ASYNCH, "asynch_id=%d status=%d error=%d argout_type=%d\n",
	    				asynch_id, asynch_client_data.status, asynch_client_data.error, asynch_client_data.argout_type);

/*
 * DO send reply using ONE-WAY rpc i.e. timeout=0 , this way the server
 * gets rid of the replies immediately.
 */
				clnt_stat = clnt_call (client.asynch_clnt, RPC_PUTGET_ASYN_REPLY,
					(xdrproc_t)xdr__asynch_client_data, (caddr_t) &asynch_client_data,
					(xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
			}
			else
			{
				client_raw_data = (_client_raw_data*)result;
				asynch_client_raw_data.asynch_id = asynch_id;
/*
 * return time command was executed to client
 */
				asynch_client_raw_data.status = client_raw_data->status;
				asynch_client_raw_data.error = client_raw_data->error;
				asynch_client_raw_data.ser_argout_type = client_raw_data->ser_argout_type;
				asynch_client_raw_data.deser_argout_type = client_raw_data->deser_argout_type;
				asynch_client_raw_data.xdr_length = client_raw_data->xdr_length;
				asynch_client_raw_data.argout = client_raw_data->argout;
				asynch_client_raw_data.var_argument.length = iarg; 
				asynch_client_raw_data.var_argument.sequence = vararg;
		
				dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : asynchronous client raw data -> \n");
				dev_printdebug (DBG_ASYNCH, "\nasynch_id=%d status=%d error=%d ser_argout_type=%d\n",
					asynch_id, asynch_client_raw_data.status, asynch_client_raw_data.error, asynch_client_raw_data.ser_argout_type);

/*
 * DO send reply using ONE-WAY rpc i.e. timeout=0 , this way the server
 * gets rid of the replies immediately.
 */
				clnt_stat = clnt_call (client.asynch_clnt, RPC_PUTGET_RAW_ASYN_REPLY,
					(xdrproc_t)xdr__asynch_client_raw_data, (caddr_t) &asynch_client_raw_data,
					(xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
			}

                        dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : send reply to client (clnt_stat %d)\n",clnt_stat);

/*
 * because we are using "one-way rpc" to send the reply to the client
 * the normal status is RPC_TIMEDOUT, any other status is an error
 * what to do with the error ? how to inform the client ? maybe I could
 * try sending the error with clnt_call() and what if that fails ?
 * have to think out something here. maybe interpret the error and
 * then send it back via a special error channel ?
 *
 * for the moment ignore all errors except for RPC_CANTSEND. this
 * (normally) occurs when the server tries to send an answer to a
 * stale client handle (e.g. client has died). in this case call
 * shutdown the client tcp socket and call the asynch_client_cleanup()
 * routine to destroy the client handle.
 *
 * note : there is no point calling dev_rpc_error() here because the
 *        client devserver structure does not refer to a real device
 *        but to a pseudo device server. any attempt to recreate the
 *        connection will result in a crash (because not all fields
 *        are correctly initialised).
 *
 * andy 25/6/97
 */
			if (clnt_stat == RPC_CANTSEND)
			{
  				dev_printerror (SEND, "devserver_prog_5() : server couldn't send asynchronous reply to client"
					" (clnt_stat=%d) calling cleanup !", (char*)clnt_stat); 
				asynch_client_cleanup(&error);
  			}			
 

/*
 * only free arguments if this is the server side, on the client side
 * it is the client's responsibility to free memory allocated by xdr
 * using xdr_free()
 */
			if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) 
			{
				dev_printerror (SEND,"%s", "devserver_prog_5() : server couldn't free arguments !!");
				return;
			}
		}
/* 
 * free arguments on client side
 */

		if (rqstp->rq_proc == RPC_PUTGET_ASYN_REPLY)
       			xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(argument.asynch_client_data.var_argument));
		if (rqstp->rq_proc == RPC_PUTGET_RAW_ASYN_REPLY)
        		xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(argument.asynch_client_raw_data.var_argument));
	}

	dev_printdebug (DBG_ASYNCH, "\ndevserver_prog_5() : returning\n");
	return;
}

static SVCXPRT 	*asynch_trans_tcp, 
		*asynch_transp_udp;

/**@ingroup dsAPI
 * Function to register the asynchronous rpc service which will serve asynchronous requests and replies
 * for servers and clients respectively
 * 
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc asynch_rpc_register(long *error)
{
	long pid;
	char hostname[32];
	static int asynch_sock_tcp=RPC_ANYSOCK, asynch_sock_udp=RPC_ANYSOCK;

/*
 * first check if the asynchronous service has not already 
 * been registered. if so then do not register it.
 */
	LOCK(async_mutex);
	if (config_flags.asynch_rpc != True)
	{
/*
 * check if process is a bona fida device server if not then
 * generate a server name out of the host name and pid
 */
		if (config_flags.device_server != True)
		{
/*
 * the client's "server name" is the hostname/pid, it would be
 * better to use the process executable name but how to get it ?
 */
#if !defined (vxworks)
                        pid = getpid ();
#else  /* !vxworks */
                        pid = taskIdSelf ();
#endif /* !vxworks */
			gethostname(hostname,sizeof(hostname));
			snprintf(config_flags.server_name, sizeof(config_flags.server_name), "%s/%d",hostname,pid);
			strncpy(config_flags.server_host,hostname, sizeof(config_flags.server_host));

/* 
 * M Diehl, 15.11.99
 * Get a transient program number for the client only, if this has not
 * already been done - i.e. only if we are not a real device server.
 * If we aren't, use the new gettransient() interface!
 */
 
                        if( config_flags.prog_number == 0 )
				config_flags.prog_number = gettransient(config_flags.server_name);

/*
 * return error, if no number found
 */

                        if( config_flags.prog_number == 0 )
			{
				UNLOCK(async_mutex);
				return DS_NOTOK;    /* no error defined in  DB */
			}

/* 
 * M. Diehl, 15.11.99
 * Set version depending on whether we are a real device server or not!
 */
                        if( config_flags.device_server == True )
 				config_flags.vers_number = API_VERSION;
			else
 				config_flags.vers_number = ASYNCH_API_VERSION;

/*
 * setup signal handling to catch SIGPIPE and SIGHUP and return
 * to the client and to catch SIGINT, SIGQUIT and SIGTERM and unregister 
 * the asynchronous service from the portmapper - for UNIX and OS9
 */
#ifndef WIN32
			(void) signal(SIGINT, main_signal_handler);
			(void) signal(SIGQUIT, main_signal_handler);
			(void) signal(SIGTERM, main_signal_handler);
			(void) signal(SIGHUP, main_signal_handler);
			(void) signal(SIGPIPE, main_signal_handler);
#else
			(void) signal(SIGINT, main_signal_handler);
			(void) signal(SIGABRT, main_signal_handler);
			(void) signal(SIGTERM, main_signal_handler);
			(void) signal(SIGBREAK, main_signal_handler);
#endif /* !WIN32 */

		}

		asynch_sock_tcp = RPC_ANYSOCK;
		asynch_trans_tcp = svctcp_create(asynch_sock_tcp, 0, 0);
	
		if (asynch_trans_tcp == NULL)
		{
			fprintf(stderr, "asynch_rpc_register(): cannot create asynchronous tcp service\n");
			UNLOCK(async_mutex);
			return(DS_NOTOK);
		}

  
  		if (!svc_register(asynch_trans_tcp, config_flags.prog_number, ASYNCH_API_VERSION, devserver_prog_5, IPPROTO_TCP))
  		{
  			fprintf(stderr, "asynch_rpc_register(): cannot register asynchronous tcp service\n");
			UNLOCK(async_mutex);
  			return(DS_NOTOK);
  		}
  
		asynch_svc_tcp_sock = asynch_trans_tcp->xp_sock;
        	dev_printdebug (DBG_ASYNCH, "\nasynch_rpc_register() : registered asynchronous server (%s) at prog no=%d\n",
		config_flags.server_name,config_flags.prog_number);
		config_flags.asynch_rpc = True;
	}

	UNLOCK(async_mutex);
	return(DS_OK);
}

/**@ingroup asyncAPI
 * Function to check whether the asynchronous server has been imported, and if not to import it.
 * 
 * @param ds       	handle to access the device.
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long asynch_server_import(devserver ds, long *error)
{
	CLIENT 			*asynch_clnt_tcp;
	_dev_import_in 		dev_import_in;
	DevVarArgument 		vararg[10];
	char 			*name, 
				*host, 
				*hstring;
	long 			iarg, 
				clnt_stat;
        struct sockaddr_in 	serv_adr;
#if !defined (vxworks)
	struct hostent		*ht;
#else  /* !vxworks */
	int			host_addr;
#endif /* !vxworks */
        int 			tcp_socket=0;
	char 			*idot;

	LOCK(async_mutex);

/*
 * check to see whether the asynchronous client handle has
 * been created - if not then create it
 */
	if (svr_conns[ds->no_svr_conn].asynch_clnt == NULL)
	{
/*
 * remove any trailing Internet domain names - OS9 @ ESRF uses netdb
 * to resolve Internet host names and know nothing about DNS
 *
 * andy 10/7/97
 */
#ifdef _UCC
		if ((idot = strstr(svr_conns[ds->no_svr_conn].server_host,".")) != NULL)
		{
			svr_conns[ds->no_svr_conn].server_host[strlen(svr_conns[ds->no_svr_conn].server_host)-strlen(idot)-1] = '\0';
                	dev_printdebug (DBG_ASYNCH, "\nasynch_server_import() : new server host name %s\n", svr_conns[ds->no_svr_conn].server_host);
		}
#endif /* _UCC */

/*
 * use the clnttcp_create() call to create the client handle
 * this way specifies the tcp socket which we need later on
 * to close (because stupid ol' OS9 does not close it automatically
 * when calling clnt_destroy() !) - andy 2/7/97
 */

#if !defined (vxworks)
               	ht = gethostbyname((char*)svr_conns[ds->no_svr_conn].server_host);
		if (ht == NULL)
		{
			fprintf(stderr, "asynch_server_import(): could not resolve server host name %s\n", svr_conns[ds->no_svr_conn].server_host);
			*error = DevErr_AsynchronousServerNotImported;
			return(DS_NOTOK);
		}
		memcpy ( (char *)&serv_adr.sin_addr, ht->h_addr, (size_t) ht->h_length );
#else  /* !vxworks */
               	host_addr = hostGetByName((char*)svr_conns[ds->no_svr_conn].server_host);
		memcpy ( (char*)&serv_adr.sin_addr, (char*)&host_addr, 4);
#endif /* !vxworks */
                serv_adr.sin_family = AF_INET;
                serv_adr.sin_port = 0;
                tcp_socket = RPC_ANYSOCK;
                asynch_clnt_tcp = clnttcp_create ( &serv_adr, svr_conns[ds->no_svr_conn].prog_number, ASYNCH_API_VERSION, &tcp_socket, 0, 0);
		if (asynch_clnt_tcp == NULL)
		{
			fprintf(stderr, "asynch_server_import(): could not import asynchronous server\n");
			*error = DevErr_AsynchronousServerNotImported;
			UNLOCK(async_mutex);
			return(DS_NOTOK);
		}
/*
 * set timeout to asynchronous import timeout. this value can be changed
 * by the client using the dev_import_timeout() so that when the 
 * client imports the server it does not wait too long.
 */
		clnt_control(asynch_clnt_tcp, CLSET_RETRY_TIMEOUT, (char*)&import_retry_timeout);
		clnt_control(asynch_clnt_tcp, CLSET_TIMEOUT, (char*)&import_timeout);
/*
 * prepare arguments for dev_import
 */
		dev_import_in.device_name = extract_device_name(ds->device_name,error);
		dev_import_in.access_right = ds->dev_access;
		dev_import_in.client_id = ds->ds_id;
		dev_import_in.connection_id = 0;
/*
 * tag on additional arguments to dev_import_in needed by the server to identify 
 * client
 */
        	dev_import_in.var_argument.length = iarg = 0;
        	name = config_flags.server_name;
        	vararg[iarg].argument_type      = D_STRING_TYPE;
        	vararg[iarg].argument           = (DevArgument)&name;
        	dev_import_in.var_argument.length++; 
		iarg++;

        	host = config_flags.server_host;
        	vararg[iarg].argument_type      = D_STRING_TYPE;
        	vararg[iarg].argument           = (DevArgument)&host;
        	dev_import_in.var_argument.length++; 
		iarg++;

        	vararg[iarg].argument_type      = D_ULONG_TYPE;
        	vararg[iarg].argument           = (DevArgument)&config_flags.prog_number;
        	dev_import_in.var_argument.length++; 
		iarg++;

        	vararg[iarg].argument_type      = D_ULONG_TYPE;
        	vararg[iarg].argument           = (DevArgument)&config_flags.vers_number;
        	dev_import_in.var_argument.length++;
        	dev_import_in.var_argument.sequence = vararg;

/*
 * "import" the asynchronous service of the device server by calling
 * the RPC_ASYN_IMPORT service. this will test whether the server supports
 * asynchronous calls and will register the client with the server.
 * this is an asynchronous request i.e. the client does not wait for the
 * the server to reply.
 */
		clnt_stat = clnt_call(asynch_clnt_tcp, RPC_ASYN_IMPORT,
			            (xdrproc_t)xdr__dev_import_in, (caddr_t)&dev_import_in,
			            (xdrproc_t)xdr_void, (caddr_t)NULL, TIMEVAL(zero_timeout));

		if ((clnt_stat != RPC_SUCCESS) && (clnt_stat != RPC_TIMEDOUT))
		{
			if (clnt_stat == RPC_PROGVERSMISMATCH)
				*error = DevErr_AsynchronousCallsNotSupported;
			else
			{
				hstring = clnt_sperror(asynch_clnt_tcp,"asynch_server_import()");
				dev_printerror(SEND,"%s",hstring);
				*error = DevErr_RPCFailed;
			}
#if !defined (_NT)
                        close (tcp_socket);
#else
                        closesocket (tcp_socket);
#endif /* _NT */
			clnt_destroy(asynch_clnt_tcp);
			UNLOCK(async_mutex);
			return(DS_NOTOK);
		}
/*
 * set the timeout to zero 
 */
		clnt_control(asynch_clnt_tcp, CLSET_RETRY_TIMEOUT, (char*)&zero_timeout);
		clnt_control(asynch_clnt_tcp, CLSET_TIMEOUT, (char*)&zero_timeout);

		svr_conns[ds->no_svr_conn].asynch_clnt = asynch_clnt_tcp;
		svr_conns[ds->no_svr_conn].flushed = True;
		svr_conns[ds->no_svr_conn].pending = 0;
		svr_conns[ds->no_svr_conn].asynch_timeout = asynch_timeout;
#ifdef _UCC
                svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket = ds_rpc_svc_fd;
                svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket = tcp_socket;
#endif /* _UCC */

                dev_printdebug (DBG_ASYNCH,
                	"\nasynch_server_import() : imported asynchronous server (host=%s) at prog no=%d socket in=%d out=%d\n",
                	svr_conns[ds->no_svr_conn].server_host,svr_conns[ds->no_svr_conn].prog_number,
			svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket,
			svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket);
	}
	else
	{
/* 
 * asynch handle exists, ping the server to see if it is still alive
 */
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"calling asynch_client_ping\n");
		if (asynch_client_ping(ds->no_svr_conn, error) == DS_NOTOK)
		{
			ds->asynch_clnt = NULL;
			UNLOCK(async_mutex);
			return(DS_NOTOK);
		}
	}
/*
 * update ds' copy of client handle
 */
	ds->asynch_clnt = svr_conns[ds->no_svr_conn].asynch_clnt;
	UNLOCK(async_mutex);
	return(DS_OK);
}

/**@ingroup asyncAPI
 * function to check whether the asynchronous client has been imported, if not then import it.
 *
 * @param client       	handle to access the device.
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long asynch_client_import(devserver client, long *error)
{

	long 				no_svr_conn;
	char 				*idot;
	CLIENT 				*asynch_clnt;
        static struct sockaddr_in 	serv_adr;
#if !defined (vxworks)
	struct hostent			*ht;
#else  /* !vxworks */
	int				host_addr;
#endif /* !vxworks */
	int 				tcp_socket;

	LOCK(async_mutex);

/*
 * check to see whether the asynchronous client has been
 * imported as a server - if not then import it
 */
	if ((no_svr_conn = dev_query_svr(client->server_host, client->prog_number, client->vers_number)) < 0)
	{
		fprintf(stderr, "asynch_client_import(): dev_query_svr failed, could not import client\n");
		*error = DevErr_ExceededMaximumNoOfServers;
		UNLOCK(async_mutex);
		return(DS_NOTOK);
	}

        dev_printdebug (DBG_ASYNCH, "\nasynch_client_import() : next free svr_conn = %d\n", no_svr_conn);
/*
 * before importing the asynchronous client first cleanup any stale 
 * asynchronous client connections lying around. this will ensure that
 * we don't run out of table space unnecessarily and (hopefully) prevent
 * the problem with OS9 crashing if there are too many TCP connections
 * in "CLOSED_WAIT" state.
 */
	asynch_client_cleanup(error);

	client->no_svr_conn = no_svr_conn;

	if ((svr_conns[no_svr_conn].no_conns == 0) && (svr_conns[no_svr_conn].asynch_clnt == NULL))
	{
/*
 * remove any trailing Internet domain names - OS9 @ ESRF uses netdb
 * to resolve Internet host names and know nothing about DNS
 *
 * andy 10/7/97
 */
#ifdef _UCC
		if ((idot = strstr(client->server_host,".")) != NULL)
		{
			client->server_host[strlen(client->server_host)-strlen(idot)-1] = 0;
                	dev_printdebug (DBG_ASYNCH, "\nasynch_client_import() : new client host name %s\n", client->server_host);
		}
#endif /* _UCC */
/*
 * client is not imported then import it (note: no need to verify host
 * because if the client could send an asynchronous request then there
 * must be a link between the server and client).
 *
 * use the clnttcp_create() call to create the client handle
 * this way specifies the tcp socket which we need later on
 * to close (because stupid ol' OS9 does not close it automatically
 * when calling clnt_destroy() !) - andy 2/7/97
 */
		tcp_socket = RPC_ANYSOCK;
#if !defined (vxworks)
                ht = gethostbyname(client->server_host);
		if (ht == NULL)
		{
			fprintf(stderr, "asynch_client_import(): could not resolve client host name %s!\n", client->server_host);
			*error = DevErr_AsynchronousServerNotImported;
			UNLOCK(async_mutex);
			return(DS_NOTOK);
		}
                memcpy ( (char *)&serv_adr.sin_addr, ht->h_addr, (size_t) ht->h_length );
#else  /* !vxworks */
                host_addr = hostGetByName(client->server_host);
		memcpy ( (char*)&serv_adr.sin_addr, (char*)&host_addr, 4);
#endif /* !vxworks */
                serv_adr.sin_family = AF_INET;
                serv_adr.sin_port = 0;
                asynch_clnt = clnttcp_create ( &serv_adr, client->prog_number, ASYNCH_API_VERSION, &tcp_socket, 0, 0);

		if (asynch_clnt == NULL)
		{
			fprintf(stderr, "asynch_rpc_server_import(): could not import asynchronous client\n");
			*error = DevErr_AsynchronousServerNotImported;
			return(DS_NOTOK);
		}
/*
 * set the timeout of the client to zero so that the server does not hang
 * when sending asynchronous replies
 */
		clnt_control(asynch_clnt, CLSET_RETRY_TIMEOUT, (char*)&zero_timeout);
		clnt_control(asynch_clnt, CLSET_TIMEOUT, (char*)&zero_timeout);

		svr_conns[no_svr_conn].asynch_clnt = asynch_clnt;
/*
 * on OS9 save the tcp socket numbers for incoming (listen) and returning 
 * (callback) * so that in the event of a client disappearing the server 
 * can cleanup properly
 */
#ifdef _UCC
                svr_conns[no_svr_conn].asynch_listen_tcp_socket = ds_rpc_svc_fd;
#endif /* _UCC */
                svr_conns[no_svr_conn].asynch_callback_tcp_socket = tcp_socket;
                dev_printdebug (DBG_ASYNCH,
	                "\nasynch_client_import() : imported asynchronous client (server=%s,prog no=%d,no_svr_conn=%d) socket in=%d out=%d\n",
	                client->server_name,client->prog_number,
			no_svr_conn,svr_conns[no_svr_conn].asynch_listen_tcp_socket,
			svr_conns[no_svr_conn].asynch_callback_tcp_socket);
	}
/*
 * increment the number of connections the client has open
 * on this client
 */
	svr_conns[no_svr_conn].no_conns++;
/*
 * update copy of client handle in devserver structure
 */
	client->asynch_clnt = svr_conns[client->no_svr_conn].asynch_clnt;
	UNLOCK(async_mutex);

	return(DS_OK);
}

/**@ingroup asyncAPI
 * function to check whether the asynchronous client has been importedand return an error if it has not
 *
 * @param client       	handle to access the device.
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long asynch_client_check(devserver client, long *error)
{

	long no_svr_conn;
/*
 * check to see whether the asynchronous client has been
 * imported as a server 
 */
	LOCK(async_mutex);
			
	if ((no_svr_conn = dev_query_svr(client->server_host, client->prog_number, client->vers_number)) < 0)
	{
		fprintf(stderr, "asynch_client_check(): dev_query_svr failed, could not import client (DevErr_ExceededMaximumNoOfServers)\n");
		*error = DevErr_ExceededMaximumNoOfServers;
		UNLOCK(async_mutex);

		return(DS_NOTOK);
	}

	client->no_svr_conn = no_svr_conn;
/*
 * update copy of client handle in devserver structure
 */
	client->asynch_clnt = svr_conns[client->no_svr_conn].asynch_clnt;
	if (client->asynch_clnt == NULL)
	{
		fprintf(stderr, "asynch_client_check(): asynch client not imported, try to import it !\n");
		UNLOCK(async_mutex);
		return (asynch_client_import(client,error));
	}

	UNLOCK(async_mutex);
	return(DS_OK);
}

#define getdtablesize() FD_SETSIZE

/**@ingroup asyncAPI
 * This function checks to see if any asynchronously replies are pending. If so it triggers the associated callback
 * routines. The call will wait for a maximum of timeout time before returning if no replies are received otherwise
 * it returns immediately after unpacking all received replies. A timeout of zero means check to see if any replies
 * are pending otherwise returning immediately.
 *
 * @param timeout       The time before returning in case of no answer of any device	
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_synch(struct timeval *timeout, long *error)
{
	fd_set 		zerofds, readfds;
	int 		dtbsz = getdtablesize();
#ifdef vxworks
	time_t 		tea_time;
#endif /* vxworks */
	short 		timedout=0;
	struct timeval 	my_timeout;

        dev_printdebug (DBG_ASYNCH, "\ndev_synch() : %d replies still pending\n",client_asynch_request.pending);

/*
 * only check if any asynchronous are pending
 */
	LOCK(async_mutex);

	if (client_asynch_request.pending > 0)
	{
		FD_ZERO(&readfds);

/*
 * make a copy of timeout so that if it gets modified (a.g. on Linux)
 * we don't touch the client's copy - andy 19nov98
 */
		my_timeout = *timeout;
	
		do
		{
#ifndef vxworks
			readfds = svc_fdset;
#else
			readfds = taskRpcStatics->svc.svc_fdset;
#endif /* !vxworks */
/*
 * dev_synch() only to listen for asynchronous calls i.e. requests made
 * to the asynchronous service port. It is not easy to detect only 
 * asynchronous ports. The compromise is to filter out the udp and
 * tcp synchronous services of the client. This way if it is a server
 * it will not receive any synchronous calls from udp clients or new
 * tcp clients while dev_synch'ing.
 *
 * - andy 17feb2000
 */
			FD_CLR(synch_svc_udp_sock, &readfds);
			FD_CLR(synch_svc_tcp_sock, &readfds);

#if defined (__hpux)
#if defined (__STDCPP__) && !defined (_GNU_PP)
			switch (select(dtbsz,&readfds,(fd_set*)0,(fd_set*)0,&my_timeout))
#else
			switch (select(dtbsz,(int*)&readfds,(int*)0,(int*)0,&my_timeout))
#endif /* __hpux10 */
#else /* hpux */
			switch (select(dtbsz,&readfds,(fd_set*)0,(fd_set*)0,&my_timeout))
#endif
			{
#ifndef _UCC
#ifndef WIN32
				case -1 : 
					if (errno == EINTR)
#else
				case SOCKET_ERROR : 
					if (WSAGetLastError() == WSAEINTR)
#endif /* WIN32 */

#else
                        	case -1 : 
					if ((errno == EINTR) || (errno == EOS_SIGNAL))
#endif
						continue;
				  	perror("dev_synch(): select() returns with error ");
					UNLOCK(async_mutex);
				        return(DS_NOTOK);
/*
 * returned from select() via timeout, return to client
 */
				case 0 : 
					timedout = 1;
				        break;
/*
 * decode request and call rpc service. this should normally be the 
 * RPC_PUTGET_ASYN_REPLY service because this is a client/server synchronising
 * with replies coming from device servers
 */
				default : 	
					UNLOCK(async_mutex);
                			svc_getreqset(&readfds);
					LOCK(async_mutex);
			}

		}
/*
 * if no more asynchronous requests pending or timed out then don't wait anymore
 */
		while ((timedout == 0) && (client_asynch_request.pending > 0));
	}
/*
 * if any calls still pending then check if any have timedout
 */
	asynch_timed_out(NULL);
	UNLOCK(async_mutex);

	return(DS_OK);
}

/*#ifdef _UCC*/
int ds_rpc_svc_fd;
/*#endif*/

/**@ingroup dsAPI
 * rpc service to import the asynchronous service of 
 * a server. this function is called by the client the
 * first time it (re)imports the asynchronous service
 * of the server. it will import the reply service of
 * the client and send a (synchronous) reply to the client.
 *
 * @param dev_import_in 
 *
 * @return 
 */
_dev_import_out* _DLLFunc rpc_asynch_import_5(_dev_import_in *dev_import_in)
{
	static struct _dev_import_out 	dev_import_out;
	struct _devserver 		client;
	long 				iarg, 
					error, 
					status;

	LOCK(async_mutex);

	dev_printdebug (DBG_ASYNCH, "rpc_asynch_import() : entered\n");

/*
 * unpack additional input arguments to identify the client 
 */
	iarg = 0;
	strncpy(client.server_name,*(char**)dev_import_in->var_argument.sequence[iarg].argument, sizeof(client.server_name));
	iarg++;
	strncpy(client.server_host,*(char**)dev_import_in->var_argument.sequence[iarg].argument, sizeof(client.server_host));
	iarg++;
	client.prog_number = *(long*)dev_import_in->var_argument.sequence[iarg].argument;
	iarg++;
	client.vers_number = *(long*)dev_import_in->var_argument.sequence[iarg].argument;
	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_import() : client data -> ");
        dev_printdebug (DBG_ASYNCH, "name=%s host=%s prog_no=%d vers_no=%d\n", client.server_name, client.server_host,
                        client.prog_number, client.vers_number);

/*
 * import asynchronous service of client so that server can send replies to it
 */
	UNLOCK(async_mutex);
	status = asynch_client_import(&client, &error);
	LOCK(async_mutex);

/*
 * initialise output arguments 
 */
        dev_import_out.ds_id  = 0;
        dev_import_out.status = status;
        dev_import_out.error  = error;
        strncpy (dev_import_out.server_name,  config_flags.server_name, sizeof(dev_import_out.server_name));
        dev_import_out.var_argument.length   = 0;
        dev_import_out.var_argument.sequence = NULL;

	UNLOCK(async_mutex);

	return(&dev_import_out);
}

/**@ingroup dsAPI
 * rpc service to free the asynchronous service of 
 * a server. this function is called by the client after
 * it has freed the last device imported from that server.
 * it will free the imported reply service of the client 
 * and send a (synchronous) reply to the client.
 *
 * @param dev_free_in 
 *
 * @return 
 */
_dev_free_out* _DLLFunc rpc_asynch_free_5(_dev_free_in *dev_free_in)
{
	static _dev_free_out dev_free_out;
	struct _devserver client;
	long iarg, status, no_svr_conn; 

/*
 * unpack additional input arguments to identify the client 
 */
	LOCK(async_mutex);

	iarg = 0;
	strncpy(client.server_name,*(char**)dev_free_in->var_argument.sequence[iarg].argument, sizeof(client.server_name));
	iarg++;
	strncpy(client.server_host,*(char**)dev_free_in->var_argument.sequence[iarg].argument, sizeof(client.server_host));
	iarg++;
	client.prog_number = *(long*)dev_free_in->var_argument.sequence[iarg].argument;
	iarg++;
	client.vers_number = *(long*)dev_free_in->var_argument.sequence[iarg].argument;
	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : client data -> ");
	dev_printdebug (DBG_ASYNCH, "name=%s host=%s prog_no=%d vers_no=%d\n",
                        client.server_name, client.server_host, client.prog_number, client.vers_number);

/*
 * decrement the number of open server --> client connections so that the 
 * the server can eventually free and reuse the svr_conns[] entry for
 * another client
 */
	no_svr_conn = dev_query_svr(client.server_host, client.prog_number, client.vers_number);
	svr_conns[no_svr_conn].no_conns--;
        dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : no of server (no_svr_conn=%d) connections open to client %d\n",
			no_svr_conn,svr_conns[no_svr_conn].no_conns);
/* 
 * destroy the client handle now, don't wait for the client to
 * exit. note on OS9 this can lead to a lot of stale tcp ports
 * waiting to timeout therefore do a special treatment for OS9
 *
 * andy 1/7/97
 */
	if (svr_conns[no_svr_conn].no_conns == 0)
	{
		errno = 0;
/*
 * destroy the imported client callback handle
 */
		clnt_destroy(svr_conns[no_svr_conn].asynch_clnt);
        	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : destroy asynchronous client handle (no_svr_conn=%d,errno=%d)\n", no_svr_conn, errno);
#ifdef _UCC
/*
 * OS9 has difficulty detecting TCP sockets closing which can lead to stale
 * sockets hanging around after clients have exited or core dumped
 * These have to be closed manually using close().
 */
		shutdown(svr_conns[no_svr_conn].asynch_listen_tcp_socket,2);
                status = close (svr_conns[no_svr_conn].asynch_listen_tcp_socket);
        	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : close (status=%d) listen socket %d (errno=%d)\n",
		       	status,svr_conns[no_svr_conn].asynch_listen_tcp_socket,errno);
/*
        	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : before calling FD_CLR() svc_fdset = 0x%04x, errno=%d\n", svc_fdset,errno);
 *
 * now that we have closed the socket manually we have to tell the RPC
 * not to listen on this socket anymore by modifying the global svc_fdset mask
 * This is not clean programming but a desperate soution to solve a
 * an OS9 problem !
 *
		FD_CLR(svr_conns[no_svr_conn].asynch_listen_tcp_socket,&svc_fdset);
        	dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_free() : after calling FD_CLR() svc_fdset = 0x%04x, errno=%d\n", svc_fdset,errno);
*/
#endif /* _UCC */
		svr_conns[no_svr_conn].asynch_clnt = NULL;

	}
/*
 * initialise output arguments 
 */
        dev_free_out.status = DS_OK;
        dev_free_out.error  = 0;
        dev_free_out.var_argument.length   = 0;
        dev_free_out.var_argument.sequence = NULL;

	UNLOCK(async_mutex);
	return(&dev_free_out);
}

/**@ingroup dsAPI
 * function to receive an asynchronous reply sent
 * by a device server to the client. it will identify
 * the originating request, unpack the arguments correctly
 * and trigger the callback (if registered). 
 *
 * @param asynch_client_data
 *
 * @return
 */
_asynch_client_data* _DLLFunc rpc_asynch_reply_5(_asynch_client_data *asynch_client_data)
{
	unsigned long 	asynch_id;
	long 		asynch_index, 
			iarg;
	DevCallbackData cb_data;
#ifdef vxworks
	time_t 		teatime;
#endif /* vxworks */


	asynch_id = asynch_client_data->asynch_id;
/*
 * find out which request the reply belongs to
 */ 

	if ((asynch_index = asynch_get_index(asynch_id)) < 0)
	{
		dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_reply_5(): problem - could not identify reply (id=%d)\n",asynch_id);
		return(asynch_client_data);
	}
	LOCK(async_mutex);


        dev_printdebug (DBG_ASYNCH, "\nrpc_asynch_reply() : received asynchronous reply id=%d index=%d status=%d error=%d\n",
		asynch_id,asynch_index,asynch_client_data->status,asynch_client_data->error);
	if (client_asynch_request.args[asynch_index].callback != NULL)
	{
		cb_data.asynch_id = client_asynch_request.args[asynch_index].asynch_id;
		cb_data.argout = client_asynch_request.args[asynch_index].argout;
		cb_data.argout_type = client_asynch_request.args[asynch_index].argout_type;
		cb_data.status = asynch_client_data->status;
		cb_data.error = asynch_client_data->error;
		iarg = 0;
		cb_data.time.tv_sec = *(long*)(asynch_client_data->var_argument.sequence[iarg].argument);
		iarg++;
		cb_data.time.tv_usec = *(long*)(asynch_client_data->var_argument.sequence[iarg].argument);
		iarg++;
/*
 * unpack dynamic error message if present 
 */
		if (asynch_client_data->var_argument.length == 3)
		{
			dev_error_string = (char*)malloc(strlen(*(char**)asynch_client_data->var_argument.sequence[iarg].argument)+1);
			assert(dev_error_string==NULL);
			if(dev_error_string)
				strcpy(dev_error_string,*(char**)asynch_client_data->var_argument.sequence[iarg].argument);
		}

		(*client_asynch_request.args[asynch_index].callback) (client_asynch_request.args[asynch_index].ds, 
			client_asynch_request.args[asynch_index].user_data, cb_data);
	}

/*
 * if asynchronous call type then free request entry for new request (event
 * type calls stay pending to accept new events)
 */
	if (client_asynch_request.args[asynch_index].type & D_ASYNCH_TYPE)
	{
		client_asynch_request.args[asynch_index].flag = DS_FALSE;
		client_asynch_request.pending--;
/*
 * decrement the pending calls counter for the server and device as well
 */
		svr_conns[client_asynch_request.args[asynch_index].ds->no_svr_conn].pending--;
		client_asynch_request.args[asynch_index].ds->pending--;
	}
	else
	{
/*
 * update time for event entry, event timeouts are calculated with respect
 * to the time the last event was received
 */
#ifndef vxworks
		gettimeofday(&client_asynch_request.args[asynch_index].timesent, NULL);
#else
		time(&tea_time);
		client_asynch_request.args[asynch_index].timesent.tv_sec = tea_time;
		client_asynch_request.args[asynch_index].timesent.tv_usec = 0;
#endif /* !vxworks */
	}
	UNLOCK(async_mutex);

	return(asynch_client_data);
}

/**@ingroup dsAPI
 * function to receive an asynchronous reply sent
 * by a device server to the client in raw (opaque)
 * format. it will identify the originating request, 
 * unpack the arguments correctly and trigger the 
 * callback (if registered). 
 *
 * @param asynch_client_raw_data
 *
 * @return
 */
_asynch_client_raw_data* _DLLFunc rpc_raw_asynch_reply_5(_asynch_client_raw_data *asynch_client_raw_data)
{
	unsigned long 	asynch_id;
	long 		asynch_index, 
			iarg;
	DevCallbackData cb_data;

	LOCK(async_mutex);
	asynch_id = asynch_client_raw_data->asynch_id;
/*
 * find out which request the reply belongs to
 */ 

	if ((asynch_index = asynch_get_index(asynch_id)) < 0)
	{
		fprintf(stderr, "rpc_raw_asynch_id(): problem - could not identify reply (id=%d)\n",asynch_id);
		UNLOCK(async_mutex);
		return(asynch_client_raw_data);
	}


        dev_printdebug (DBG_ASYNCH,
        "\nrpc_raw_asynch_reply() : received asynchronous reply id=%d index=%d status=%d error=%d\n",
        asynch_id,asynch_index,asynch_client_raw_data->status,asynch_client_raw_data->error);
	if (client_asynch_request.args[asynch_index].callback != NULL)
	{
		cb_data.asynch_id = client_asynch_request.args[asynch_index].asynch_id;
		cb_data.argout = client_asynch_request.args[asynch_index].argout;
		cb_data.argout_type = client_asynch_request.args[asynch_index].argout_type;
		cb_data.status = asynch_client_raw_data->status;
		cb_data.error = asynch_client_raw_data->error;
		iarg = 0;
		cb_data.time.tv_sec = *(long*)(asynch_client_raw_data->var_argument.sequence[iarg].argument);
		iarg++;
		cb_data.time.tv_usec = *(long*)(asynch_client_raw_data->var_argument.sequence[iarg].argument);

		(*client_asynch_request.args[asynch_index].callback) (client_asynch_request.args[asynch_index].ds, 
				client_asynch_request.args[asynch_index].user_data, cb_data);
	}

/*
 * free request for new request 
 */
	client_asynch_request.args[asynch_index].flag = DS_FALSE;
	client_asynch_request.pending--;
/*
 * decrement the pending calls counter for the server as well
 */
	svr_conns[client_asynch_request.args[asynch_index].ds->no_svr_conn].pending--;
	client_asynch_request.args[asynch_index].ds->pending--;

	UNLOCK(async_mutex);
	return(asynch_client_raw_data);
}

/**@ingroup asyncAPI
 * This function adds a request to the array of pending requests.  It will keep track of requests id 
 * and pointers to the status, error, argout and argout_type. Also manages events as recurring asynchronous replies.
 * 
 * @param ds       	handle to access the device.
 * @param asynch_type   
 * @param event_type    
 * @param argout	pointer to input arguments.
 * @param argout_type	data type of input arguments.
 * @param callback	callback routine to be triggered  on completion
 * @param user_data	pointer to user data to be passed to callback function
 * @param id_ptr 	asynch call identifier
 * @param asynch_index
 * @param error         Will contain an appropriate error * code if the 
 *			corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc asynch_add_request(devserver ds, long asynch_type, long event_type,
                                 DevArgument argout, DevType argout_type, 
				 DevCallbackFunction *callback, void *user_data, 
				 long *id_ptr, long *asynch_index,
				 long *error)
{
	static long asynch_id = 0;
#ifdef vxworks
	time_t tea_time;
#endif /* vxworks */
	long i;
	LOCK(async_mutex);


/*
 * allocate space for the client_asynch_reply array, presently set
 * to a fixed size of MAX_ASYNCH_CALLS elements (this can be made
 * flexible once the basic mechanism is working
 *
 * andy 19feb97
 */

	if (client_asynch_request.args == NULL)
	{
       		dev_printdebug (DBG_ASYNCH, "\nasynch_add_request() : sizeof(asynch_request_arg)*MAX_ASYNCH_CALLS %d \n",
	       		sizeof(asynch_request_arg)*MAX_ASYNCH_CALLS);

		client_asynch_request.args = (asynch_request_arg*)malloc(sizeof(asynch_request_arg)*MAX_ASYNCH_CALLS);
		if(!client_asynch_request.args)
		{
			*error=DevErr_InsufficientMemory;
			UNLOCK(async_mutex);
			return DS_NOTOK;
		}
		memset((char*)client_asynch_request.args,0,sizeof(asynch_request_arg)*MAX_ASYNCH_CALLS);
		client_asynch_request.pending = 0;
		for (i=0; i<MAX_ASYNCH_CALLS; i++)
			client_asynch_request.args[i].flag = DS_FALSE;
	}
	for (i=0; i<MAX_ASYNCH_CALLS; i++)
	{
		if (client_asynch_request.args[i].flag == DS_FALSE)
		{
			asynch_id++;
			client_asynch_request.args[i].type = asynch_type;
			client_asynch_request.args[i].event = event_type;
			client_asynch_request.args[i].flag = DS_PENDING;
#ifndef vxworks
			gettimeofday(&client_asynch_request.args[i].timesent, NULL);
#else
			time(&tea_time);
			client_asynch_request.args[i].timesent.tv_sec = tea_time;
			client_asynch_request.args[i].timesent.tv_usec = 0;
#endif /* !vxworks */
			client_asynch_request.args[i].ds = ds;
			client_asynch_request.args[i].asynch_id = asynch_id;
			client_asynch_request.args[i].argout = argout;
			client_asynch_request.args[i].argout_type = argout_type;
			client_asynch_request.args[i].callback = callback;
			client_asynch_request.args[i].user_data = user_data;
			*id_ptr = asynch_id;
			client_asynch_request.pending++;
			svr_conns[ds->no_svr_conn].pending++;
			ds->pending++;
			
        		dev_printdebug (DBG_ASYNCH, "\nasynch_add_request() : added asynchronous request type=%08xd"
				" event=%d id=%d index=%d argout_type= %d argout=0x%08x callback=0x%08x pending total=%d device=%d\n",
        			asynch_type,event_type,asynch_id,i,argout_type,argout,callback,client_asynch_request.pending, ds->pending);
			break;
		}
	}

	if (i >= MAX_ASYNCH_CALLS)
	{
		fprintf(stderr, "asynch_add_request(): exceeded maximum number of pending asynchronous calls\n");
			
/*
 * calls are not BATCHed anymore therefore flushing wont help 
 *
 *		dev_flush(ds);
 */
		*error = DevErr_ExceededMaxNoOfPendingCalls;
		UNLOCK(async_mutex);
		return (DS_NOTOK);
	}

	*asynch_index = i;
	UNLOCK(async_mutex);
	return(DS_OK);
}

/**@ingroup asyncAPI
 * function to return the index corresponding to the asynchronous
 * request id in the list of client_asynch_request[] (only
 * for valid pending requests i.e. flag==DS_PENDING)
 * 
 * @param asynch_id
 *
 * @return index or DS_NOTOK if not found
 */
long _DLLFunc asynch_get_index( long asynch_id)
{
	long i;

	if (client_asynch_request.args != NULL)
		for (i=0; i<MAX_ASYNCH_CALLS; i++)
			if ((client_asynch_request.args[i].flag == DS_PENDING) &&
		    		(client_asynch_request.args[i].asynch_id == asynch_id))
				return(i);
	return(DS_NOTOK);
}

/**@ingroup asyncAPIxdr
 * XDR function to decode the client data sent by the server
 * its main difference to the standard (synchronous) version 
 * of client_data is that the asynch_id is sent before the
 * argout so that it can b used to identify the correct
 * asynchronous request and argout pointer to unpack the
 * argout data in.
 *
 * @param xdrs
 * @param objp
 *
 * @return TRUE or FALSE
 *********************************************************************-*/
bool_t xdr__asynch_client_data(XDR *xdrs, _asynch_client_data *objp)
{
	DevDataListEntry	data_type;
	long			error;	
	long			asynch_index;

	if (!xdr_long(xdrs, &objp->asynch_id)) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"cannot encode async_id: %ld\n",(long)objp->asynch_id);
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->status)) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"cannot encode status: %ld\n",(long)objp->status);
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->error)) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"cannot encode error: %ld\n",(long)objp->error);
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->argout_type)) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"cannot encode argout_type: %ld\n",(long)objp->argout_type);
		return (FALSE);
	}

/*
 * if this is an incoming call then identify the incoming asynchronous reply 
 * and the client argout so that the arguments can be decoded at the address
 * specified by the client
 */
	if (xdrs->x_op == XDR_DECODE)
	{
		asynch_index = asynch_get_index(objp->asynch_id);
		if (asynch_index >= 0)
		{
			objp->argout = client_asynch_request.args[asynch_index].argout;
        		dev_printdebug (DBG_ASYNCH,
        		"\nxdr__asynch_client_data() : identified asynchronous request id=%d index=%d argout_type=%d argout=0x%08x\n",
       			objp->asynch_id,asynch_index,objp->argout_type,objp->argout);
		}
		else
		{
        		dev_printdebug (DBG_ASYNCH, "\nxdr__asynch_client_data() : unidentified asynchronous request id=%d\n", objp->asynch_id);
			return(TRUE);
		}
	}

/*
 * Get the XDR data type from the loaded type list
 */
	if ( xdr_get_type(objp->argout_type, &data_type, &error) == DS_NOTOK)
	{
		fprintf (stderr, "xdr__client_data() : error = %d\n", error);

		objp->argout   = NULL;
		data_type.size = 0;
		data_type.xdr  = (DevDataFunction)xdr_void;
	}

	if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)data_type.xdr )) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"error decoding argout\n");
	  	return (FALSE);
	}

        if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
	{
		dev_printdebug (DBG_TRACE | DBG_ASYNCH,"error decoding vararg\n");
                return (FALSE);
        }
  	return (TRUE);
}

/** @ingroup asyncAPIxdr
 * XDR function to decode the raw client data sent by the server
 * its main difference to the standard (synchronous) version 
 * of client_raw_data is that the asynch_id is sent before the
 * argout so that it can b used to identify the correct
 * asynchronous request and argout pointer to unpack the
 * argout data in.
 *
 * @param xdrs
 * @param objp
 *
 * @return TRUE or FALSE
 */
bool_t xdr__asynch_client_raw_data(XDR *xdrs, _asynch_client_raw_data *objp)
{
	DevOpaque		*opaque;
	DevDataListEntry	data_type;
	long			error;	
	long			asynch_index;

	if (!xdr_long(xdrs, &objp->asynch_id)) 
		return (FALSE);
	if (!xdr_long(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_long(xdrs, &objp->error)) 
		return (FALSE);
        if (!xdr_long(xdrs, &objp->ser_argout_type)) 
                return (FALSE);
        if (!xdr_long(xdrs, &objp->deser_argout_type)) 
                return (FALSE);
        if (!xdr_long(xdrs, &objp->xdr_length)) 
                return (FALSE);

/*
 * if this is an incoming call then identify the incoming asynchronous reply 
 * and the client argout so that the arguments can be decoded at the address
 * specified by the client
 */
	if (xdrs->x_op == XDR_DECODE)
	{
		asynch_index = asynch_get_index(objp->asynch_id);

		if (asynch_index >= 0)
		{
			objp->argout = client_asynch_request.args[asynch_index].argout;

        		dev_printdebug (DBG_ASYNCH,
        		"\nxdr__asynch_client_data() : identified asynchronous request id=%d index=%d deser_argout_type=%d argout=0x%08x\n",
       			objp->asynch_id,asynch_index,objp->deser_argout_type,objp->argout);
		}
		else
		{
        		dev_printdebug (DBG_ASYNCH,
        		"\nxdr__asynch_client_data() : unidentified asynchronous request id=%d\n",
       			objp->asynch_id);
			return(TRUE);
		}

	/*
	 * Get the XDR data type from the loaded type list
	 */

		if ( xdr_get_type(objp->deser_argout_type, &data_type, &error) == DS_NOTOK)
	   	{
	   		fprintf (stderr, "xdr__client_raw_data() : error = %d\n", error);
	
		   	objp->argout   = NULL;
		   	data_type.size = 0;
		   	data_type.xdr  = (DevDataFunction)xdr_void;
	   	}

		opaque = (DevOpaque *)objp->argout;
		opaque->length = objp->xdr_length;

		if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)xdr_DevOpaqueRaw )) 
		  	return (FALSE);
	}
        if ( xdrs->x_op == XDR_ENCODE )
        {
/*
 * Get the XDR data type from the loaded type list
 */
        	if ( xdr_get_type(objp->ser_argout_type, &data_type, &error) == DS_NOTOK)
              	{
              		fprintf (stderr, "xdr__asynch_client_raw_data() : error = %d\n", error);
			return (FALSE);
		}

		if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)data_type.xdr )) 
			return (FALSE);
        }

        if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
                return (FALSE);
  	return (TRUE);
}

/**@ingroup asyncAPI
 * This function flushes the buffer of batched asynchronous calls sent by the client to the server.
 * 
 * @param ds       	handle to access the device.
 *
 * @return DS_OK 
 *********************************************************************-*/
long _DLLFunc dev_flush(devserver ds)
{
	enum clnt_stat clnt_stat;
	struct timeval timeout = {1, 0};
	long error;
	LOCK(async_mutex);

/*
 * check if the asynchronous client handle exists 
 */
	if (svr_conns[ds->no_svr_conn].asynch_clnt != NULL)
	{
/*
 * only flush if the server has calls pending which have not been
 * flushed already. this is to avoid sending too many flush calls
 * to the server
 */
		if (!svr_conns[ds->no_svr_conn].flushed)
		{
			dev_printdebug (DBG_ASYNCH, "\ndev_flush() : flush asynchronous request/reply buffer\n");
/*
 * flush the client's buffer by making a call to the null procedure
 * of the server. because the null procedure expects an (void) answer 
 * this will force the requests to be flushed. the timeout is set to
 * zero so the reply is ignored !
 */

			clnt_stat = clnt_call(svr_conns[ds->no_svr_conn].asynch_clnt, RPC_FLUSH, 
                   		(xdrproc_t)xdr_void, NULL, 
	           		(xdrproc_t)xdr_void, NULL,
                   		TIMEVAL(timeout));
			 dev_printdebug (DBG_ASYNCH, "\ndev_flush() : clnt_stat %d\n",clnt_stat);
/*
 * because this is a one-way RPC the normal status returned is RPC_TIMEDOUT
 * if any other error is received then pass it on to the dev_rpc_error()
 * function which will check the error and mark the connection accordingly
 */
			if ((clnt_stat != RPC_TIMEDOUT) && (clnt_stat != RPC_SUCCESS))
				dev_rpc_error(ds, clnt_stat, &error);
			svr_conns[ds->no_svr_conn].flushed = True;
		}
	}
	UNLOCK(async_mutex);
	return(DS_OK);
}

/**@ingroup asyncAPI
 * This functions returns the number of asynchronous calls sent by the client still pending an answer 
 * from the server ds. If ds == NULL the returns the total number of pending calls. 
 * 
 * @param ds       	handle to access the device.
 *
 * @return number of pending asynch calls or DS_NOTOK
 */
long _DLLFunc dev_pending(devserver ds)
{
#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
                return(DS_NOTOK);
#endif /* TANGO */
	LOCK(async_mutex);
/*
 * first check if any calls have timed out
 */
	asynch_timed_out(ds);
	if (ds == NULL)
	{
		UNLOCK(async_mutex);
		return(client_asynch_request.pending);
	}
	else
	{
		UNLOCK(async_mutex);
		return(ds->pending);
	}
}

/**@ingroup asyncAPI
 * This function sets or reads the timeout for an asynchronous call to the device ds. 
 * A request to set the timeout has to be asked with CLSET_TIMEOUT. The timeout will be 
 * set without any retry.  A request to read the timeout has to be asked with CLGET_TIMEOUT.
 * 
 * @param ds           	handle to device.
 * @param request	indicates whether the timeout should be set or only read.
 * @param dev_timeout 	timeout structure.
 * @param error   	Will contain an appropriate error code if the
 *		        corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_asynch_timeout (devserver ds, long request,
				  struct timeval *dev_timeout, long *error)
{
	*error = 0;

#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
        {
		*error = DevErr_CommandNotImplemented;
                return(DS_NOTOK);
        }
#endif

/*
 * the asynchronous timeout is stored as part of the device server handle
 * getting or seting this timeout means simply accessing this variable
 */
	switch (request)
	{
		case (CLSET_TIMEOUT) : 
			ds->asynch_timeout = *dev_timeout;
			break;

		case (CLGET_TIMEOUT) :
			*dev_timeout = ds->asynch_timeout;
			break;
			
        	default:
               		*error = DevErr_UnknownInputParameter;
               		return (DS_NOTOK);

	}
	return(DS_OK);
}

/**@ingroup asyncAPI
 * This function checks whether there are any stale asynchronous 
 * client handles lying around (because the client has disappeared
 * without doing a dev_free() for example) if so it will close
 * the tcp sockets and free the entry in the svr_conn[] table.
 * 
 * @param error   	Will contain an appropriate error code if the
 *		        corresponding call returns a non-zero value.
 */
void asynch_client_cleanup(long *error)
{
	long 		i;
	enum clnt_stat 	clnt_stat;
	long 		status;
	char 		message[]="is there anybody there ?";
	static long 	n_stale_client = 0;
	dev_printdebug (DBG_ASYNCH, "asynch_client_cleanup() : entered\n");
	for (i=0; i<NFILE; i++)
	{
		if ((svr_conns[i].no_conns > 0) && (svr_conns[i].asynch_clnt != NULL))
		{
			dev_printdebug(DBG_ASYNCH,
				"asynch_client_cleanup(): found asynchronous client %d on host %s program no. %d no_conn %d\n",
			       	i, svr_conns[i].server_host, svr_conns[i].prog_number, svr_conns[i].no_conns);
/*
 * try to ping asynchronous client to see if it is alive
 */
			if (asynch_client_ping(i, error) == DS_NOTOK)
			{
				svr_conns[i].no_conns = 0;
				svr_conns[i].asynch_clnt = NULL;
			}
		}
	}
	dev_printdebug (DBG_ASYNCH, "asynch_client_cleanup() : left\n");
	return;
}

/**@ingroup asyncAPI
 * This function checks whether any asynchronous calls have timedout 
 * if so then trigger their callback with error=RPC_TIMEOUT
 * and remove them from the list of pending calls. If ds!=NULL
 * then treat only pending asynchronous calls for the specified
 * device server 
 *
 * @param ds_tout       handle to device.
 */
void asynch_timed_out(devserver ds_tout)
{
	long 		i, 
			timediff, 
			error;
	struct timeval 	timenow;
#ifdef vxworks
	time_t 		tea_time;
#endif /* vxworks */
	devserver 	ds;
	DevCallbackData cb_data;

/*
 * if any calls still pending then check if any have timedout
 */
	if (client_asynch_request.pending > 0)
	{
/*
 * before returning check if any calls have timed out
 * (only check if any asynchronous have been registered at all)
 */
		if (client_asynch_request.args != NULL)
		{
#ifndef vxworks
			gettimeofday(&timenow, NULL);
#else
			time(&tea_time);
			timenow.tv_sec = tea_time;
			timenow.tv_usec = 0;
#endif /* !vxworks */
	
			for (i=0; i<MAX_ASYNCH_CALLS; i++)
			{
				if (client_asynch_request.args[i].flag == DS_PENDING) 
				{
/*
 * check to see whether any asynchronous calls have timed out at the same time
 * (ignore event type asynchronous calls which are permanently active
 * until the client does an unlisten or until the server dies) 
 *
 * TODO : ping all servers where events are registered to see if they
 *        are still alive if not then time out the event request
 */

					timediff = timenow.tv_sec - client_asynch_request.args[i].timesent.tv_sec;
					ds = client_asynch_request.args[i].ds;
					if ((ds_tout == NULL ) || (ds_tout = ds))
					{
						if (timediff > ds->asynch_timeout.tv_sec)
						{
/*
 * if asynchronous callback to inform it that the request has timed out
 */
							if ((client_asynch_request.args[i].type & D_ASYNCH_TYPE) &&
							   (client_asynch_request.args[i].callback != NULL))
							{
								cb_data.asynch_id = client_asynch_request.args[i].asynch_id;
								cb_data.argout = client_asynch_request.args[i].argout;
								cb_data.argout_type = client_asynch_request.args[i].argout_type;
								cb_data.status = DS_NOTOK;
								cb_data.error = DevErr_RPCTimedOut;
/*
 * in case of timeout return local time as time of command execution 
 */
#ifndef vxworks
								gettimeofday(&cb_data.time,NULL);
#else
								time(&tea_time);
								cb_data.time.tv_sec = tea_time;
								cb_data.time.tv_usec = 0;
#endif /* !vxworks */
								(*client_asynch_request.args[i].callback) 
						 		(client_asynch_request.args[i].ds,
							 	client_asynch_request.args[i].user_data,
							 	cb_data);
								client_asynch_request.args[i].flag = DS_FALSE;
								client_asynch_request.pending--;
								svr_conns[ds->no_svr_conn].pending--;
								ds->pending--;
								dev_printdebug (DBG_ASYNCH,
								"\nasynch_timed_out(): asynchronous request %d TIMED OUT\n", 
								client_asynch_request.args[i].asynch_id);
							}
/*
 * if asynchronous callback to inform it that the request has timed out
 */
							if ((client_asynch_request.args[i].type & D_EVENT_TYPE) &&
							   (client_asynch_request.args[i].callback != NULL))
							{
/*
 * ping device server to see if it still alive, if not then call callback with timeout
 * to inform it
 */
							   if (asynch_client_ping(ds->no_svr_conn,&error) == DS_NOTOK)
							   {
								cb_data.asynch_id = client_asynch_request.args[i].asynch_id;
								cb_data.argout = client_asynch_request.args[i].argout;
								cb_data.argout_type = client_asynch_request.args[i].argout_type;
								cb_data.status = DS_NOTOK;
								cb_data.error = DevErr_RPCTimedOut;
/*
 * in case of timeout return local time as time of command execution 
 */
#ifndef vxworks
								gettimeofday(&cb_data.time,NULL);
#else
								time(&tea_time);
								cb_data.time.tv_sec = tea_time;
								cb_data.time.tv_usec = 0;
#endif /* !vxworks */
								(*client_asynch_request.args[i].callback) 
						 		(client_asynch_request.args[i].ds,
							 	client_asynch_request.args[i].user_data,
							 	cb_data);
								client_asynch_request.args[i].flag = DS_FALSE;
								client_asynch_request.pending--;
								svr_conns[ds->no_svr_conn].pending--;
								ds->pending--;
								dev_printdebug (DBG_ASYNCH,
								"\nasynch_timed_out(): event request %d TIMED OUT\n", 
								client_asynch_request.args[i].asynch_id);
							   }
							}
						}
					}
				}
			}
		}
	}
	return;
}

/**@ingroup asyncAPI
 * This function checks whether the an asynchronous client handle
 * is still valid. If not it will close the tcp sockets and 
 * free the entry in the svr_conn[] table. Function used by
 * asynch_client_cleanup() and asynch_timed_out().
 * 
 * @param i
 * @param error
 *
 * @return DS_OK or DS_NOTOK
 */
long asynch_client_ping(long i,long *error)
{
	enum clnt_stat 	clnt_stat;
	long 		status;
	char 		message[]="is there anybody there ?";
	static long 	n_stale_client = 0;
	dev_printdebug (DBG_ASYNCH, "asynch_client_ping() : entered\n");
/*
 * first check if there is an asynchronous client handle
 */
	if (svr_conns[i].asynch_clnt == NULL) 
		return(DS_NOTOK);
/*
 * test to see if the asynchronous client is still alive by executing
 * the RPC_FLUSH service of the client. if the client has died the tcp
 * socket should return an error immediately. in this case close the
 * tcp connection down gracefully and remove the client from the svr_conn[]
 * table
 *
 * use ONE-WAY rpc to send the flush request so that we do not wait
 */
	clnt_stat = clnt_call(svr_conns[i].asynch_clnt, RPC_FLUSH,
                              (xdrproc_t)xdr_void, NULL,
                              (xdrproc_t)xdr_void, NULL,
                              TIMEVAL(zero_timeout));
	dev_printdebug (DBG_ASYNCH, "\nasynch_client_ping() : clnt_stat %d\n",clnt_stat);
/*
 * if asynchronous client has died the rpc should return an RPC_CANTSEND error
 */
	if (clnt_stat == RPC_CANTSEND)
	{
		n_stale_client++;
             	dev_printdebug (DBG_ASYNCH, "\nasynch_client_ping() : detected %d th stale client handle - remove it !\n", n_stale_client);
       		clnt_destroy(svr_conns[i].asynch_clnt);
#ifdef _UCC
/*
 * OS9 has difficulty detecting TCP sockets closing which can lead to stale
 * sockets hanging around after clients have core dumped or exited gracelessly
 * These have to be detected (by trying to write to them) and closed manually
 * using close().
 */
		if (svr_conns[i].asynch_listen_tcp_socket != ds_rpc_svc_fd)
		{
              		status = send(svr_conns[i].asynch_listen_tcp_socket, message,sizeof(message),0);
       			dev_printdebug (DBG_ASYNCH,
               		"\nasynch_client_ping() : try to send to a dead client (status=%d) listen socket %d (errno=%d)\n",
	       		status,svr_conns[i].asynch_listen_tcp_socket,errno);
			if (status != -1)
			{
				shutdown(svr_conns[i].asynch_listen_tcp_socket,2);
               			close (svr_conns[i].asynch_listen_tcp_socket);
       				dev_printdebug (DBG_ASYNCH, "\nasynch_cleanup() : close (status=%d) listen socket %d (errno=%d)\n",
		       			status,svr_conns[i].asynch_listen_tcp_socket,errno);
        			dev_printdebug (DBG_ASYNCH, "\asynch_client_ping() : before calling FD_CLR() svc_fdset = 0x%04x, errno=%d\n",
		       			svc_fdset,errno);
/*
 * now that we have closed the socket manually we have to tell the RPC
 * not to listen on this socket anymore by modifying the global svc_fdset mask
 * This is not clean programming but a desperate soution to solve a
 * an OS9 problem !
 */
				FD_CLR(svr_conns[i].asynch_listen_tcp_socket,&svc_fdset);
        			dev_printdebug (DBG_ASYNCH, "\nasynch_client_ping() : after calling FD_CLR() svc_fdset = 0x%04x, errno=%d\n",
		       			svc_fdset,errno);
			}
		}
#endif /* _UCC */
		svr_conns[i].no_conns = 0;
		svr_conns[i].asynch_clnt = NULL;

		return(DS_NOTOK);
	}
	return(DS_OK);
}
