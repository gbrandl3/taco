/*+*******************************************************************

 File       :	event_api.c

 Project    :	Device Servers with SUN-RPC

 Description:	Event Application Programmer's Interface

		Source code to implement the event api of the Device 
		Server C API (DSAPI). Includes all calls specific to 
		events DSAPI. Because events use the same mechanisms
		as the asynchronous calls additional code can be found in
		asyn_api.c. The semantics for events consists of
		a client registering interest in an event (by calling
		dev_event_listen()). This is transmitted to the 
		device server where the client is registered. When
		a device server has an event it wants to distribute
		to registered clients it has to dispatch it (by calling
		dev_event_fire()). If a client is not interested
		in an event anymore it (or wants to exit) it must
		unregister (by calling dev_event_unlisten()).

		Although it is possible to support different types of events 
		in the first implementation only user events are supported. 
		User events are events which are specific to a device server 
		and are totally managed by the device class. This means the 
		device class generates the events which it then passes on to 
		the DSAPI to dispatch.  The period and timing of user events 
		is totally under control of the device class.
	
 Author(s)  :	Andy Goetz
 		$Author: jkrueger1 $

 Original   :	April 1999

 Version    :	$Revision: 1.7 $

 Date       :	$Date: 2004-03-09 09:35:50 $

 Copyleft (c) 1999 by European Synchrotron Radiation Facility,
                      Grenoble, France

********************************************************************-*/
#include <config.h>
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>
#include <API_xdr_vers3.h>

#if !defined _NT
#	include <errno.h>
#	if ( (defined OSK) || (defined _OSK))
#		include <inet/socket.h>
#		include <inet/netdb.h>
#	else
#		if HAVE_SYS_FILIO_H
#			include <sys/filio.h>
#		endif 
#		if HAVE_SYS_SOCKET_H
#			include <sys/socket.h>
#		else
#			include <socket.h>
#		endif
#		if HAVE_NETDB_H
#			include <netdb.h>
#		else
#			include <rpcGbl.h>
#		endif /* !vxworks */
#		if HAVE_SYS_IOCTL_H
#			include <sys/ioctl.h>
#		else
#			include <ioctl.h>
#		endif 
#		if HAVE_SYS_TYPES_H
#			include <sys/types.h>
#		endif
#	endif /* OSK || _OSK */
#endif /* _NT */
#ifdef _UCC
#include <rpc/rpc.h>
#include <_os9_sockets.h>
#endif /* _UCC */

#ifdef __cplusplus
extern "C" {
#endif
	extern configuration_flags 	config_flags;
	extern nethost_info 		*multi_nethost;
	extern server_connections 	svr_conns[];
	extern DevServerDevices 	*devices;
/*
 * global dynamic array of pending asynchronous requests used to store 
 * info needed to receive events (shared with asynchronous calls)
 */
	extern asynch_request client_asynch_request;
#ifdef __cplusplus
};
#endif

/*
 * use malloc() to allocate space (to avoid problems with OS-9 and 64k limit)
 */
static event_client *event_client_list=NULL;

/* following line should be in API_xdr.h */
bool_t _DLLFunc xdr__asynch_client_data PT_((XDR *xdrs, _asynch_client_data *objp));

/**
 * application interface to register a client as a listener
 * for events of a specified type from a device server.
 * The clients specifies a callback routine which will
 * be called every time an event of this type occurs.
 * The client has to pass pointers to output arguments of
 * a type appropriate to the event type. The client remains
 * registered until such time as a dev_event_unlisten()
 * call is issued.
 * 
 * @param ds		handle to access the device.
 * @param event_type	event to listen for
 * @param callback	callback routine to be triggered on completion
 * @param user_data	pointer to user data to be passed to callback function
 * @param argout	pointer for output arguments.
 * @param argout_type	data type of output arguments.
 * @param event_id_ptr	client event identifier
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_event_listen_x (devserver ds, long event_type,
	DevArgument argout, DevType argout_type, 
	DevCallbackFunction *callback, void *user_data,
	long *event_id_ptr, long *error)
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
	long			event_id;
	long			event_index;
	DevString		name,host;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_event_listen_x() : entering routine\n");

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

	if (  argout_type < 0)
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
 * Events are not supported for local devices.
 */

	if ( local_flag != True )
	{
		if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
		{
			return (DS_NOTOK);
		}
	}
	else
	{
		*error = DevErr_AsynchronousCallsNotSupported;
		return(DS_NOTOK);
	}
/*
 * If the security system is configured, 
 * verify the security key
 */

	if (!ds->no_database)
	{
		if ( nethost->config_flags.security == True )
		{
			if ( verify_sec_key (ds, &client_id, error) == DS_NOTOK )
			{
				return (DS_NOTOK);
			}
		}
	}

/*
 * in order to received events the client needs to register
 * an RPC service to receive the answer. Check if the client has done
 * this, if not then register the client.
 */
	if (config_flags.asynch_rpc != True)
	{
		rstatus = asynch_rpc_register(error);
		if (rstatus != DS_OK) return(DS_NOTOK);
	}
	
/*
 * events use the asynchronous service of the device server - 
 * make sure it is imported. 
 */

	if (asynch_server_import(ds,error) != DS_OK)
	{
		return (DS_NOTOK);
	}

/*
 * store the pointers to the return arguments so that event can
 * be passed back asynchronously to the client
 */
	event_type = event_type;
	if (asynch_add_request(ds, D_EVENT_TYPE, event_type, argout, argout_type, callback, user_data, &event_id, &event_index, error) != DS_OK)
	{
		return(DS_NOTOK);
	}


	/*
	 *  fill in data transfer structures server_data
	 *  and client_data.
	 */

	server_data.ds_id	= ds->ds_id;
	server_data.client_id	= client_id;
	server_data.access_right= ds->dev_access;
	server_data.cmd		= 0;
	server_data.argin_type	= D_VOID_TYPE;
	server_data.argout_type	= argout_type;
	server_data.argin	= (char *) NULL;

	dev_printdebug (DBG_ASYNCH, "dev_event_listen_x() : server data -> \n");
	dev_printdebug (DBG_ASYNCH, "ds_id=%d  cmd=%d  outtype=%d\n",
	    server_data.ds_id, server_data.cmd,
	    server_data.argout_type);

/*
 * additional arguments, in this case asynch_id and the return argument
 * pointers are passed to the device server via the variable argument 
 * array
 */
	server_data.var_argument.length = iarg = 0;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&event_id;
	server_data.var_argument.length++; iarg++;

	name = config_flags.server_name;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&name;
	server_data.var_argument.length++; iarg++;
	host = config_flags.server_host;
	vararg[iarg].argument_type	= D_STRING_TYPE;
	vararg[iarg].argument		= (DevArgument)&host;
	server_data.var_argument.length++; iarg++;
 
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.prog_number;
	server_data.var_argument.length++; iarg++;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&config_flags.vers_number;
	server_data.var_argument.length++; iarg++;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&event_type;
	server_data.var_argument.length++;
	server_data.var_argument.sequence = vararg;

        dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_event_listen_x() : client data -> ");
        dev_printdebug (DBG_ASYNCH, "event_type=%d asynch_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
	    event_type,event_id,
	    config_flags.server_name,config_flags.server_host,
	    config_flags.prog_number,config_flags.vers_number);

/*
 *  call RPC_DEV_PUTGET_ASYN using the client handle which has
 *  been opened to the asynchronous service of the server and
 *  ONE-WAY rpc i.e. xdr routine = xdr_void && timeout = zero
 */

	clnt_stat = clnt_call (ds->asynch_clnt, RPC_EVENT_LISTEN,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
	dev_printdebug (DBG_ASYNCH, "\ndev_event_listen_x() : clnt_stat %d\n",clnt_stat);

/*
 * Check for errors on the RPC connection.
 */

	if ( clnt_stat != RPC_TIMEDOUT)
	{
		if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
		{
/*
 * remove pending request from list
 */
			client_asynch_request.args[event_index].flag = DS_FALSE;
			client_asynch_request.pending--;
			ds->pending--;
			return (DS_NOTOK);
		}
	}

	/*
	 * return the asynch_id to the client for identification purposes
	 */

	*event_id_ptr = event_id;

	return (DS_OK);
}

/**@ingroup dsAPI
 * application interface for device servers to fire events.
 * It will dispatch the event to all clients who have
 * registered interest in the specified event type.
 * Clients are passed the arguments specified in argout.
 * 
 * @param ds
 * @param event    	event type to fire
 * @param argout  	pointer for output arguments.
 * @param argout_type	data type of output arguments.
 * @param event_status 
 * @param event_error
 */ 
#ifndef __cplusplus 
/*
 * OIC version
 */
void _DLLFunc dev_event_fire (DevServer ds, long event,
	DevArgument argout, DevType argout_type, 
	long event_status, long event_error)
#else
/*
 * C++ version
 */
void _DLLFunc dev_event_fire (DeviceBase *device, long event,
	DevArgument argout, DevType argout_type, 
	long event_status, long event_error)
#endif /* __cplusplus */
{
	struct _devserver 	client;
	DevVarArgument 		vararg[10];
	long 			iarg;
	long 			error;
	static struct timeval 	timenow;
#ifndef WIN32
	static struct timezone 	tz;
#endif /*!WIN32 */
#if defined (vxworks) || (WIN32)
	time_t tea_time;
#endif /* vxworks */
	enum clnt_stat clnt_stat;

	_asynch_client_data 	asynch_client_data;
	long			i;

        dev_printdebug (DBG_ASYNCH, "\ndev_event_fire() : entering routine\n");
	LOCK(async_mutex);


/*
 * for all registered clients if event type corresponds then notify
 * them by sending them the output arguments
 */
	for (i=0; i<EVENT_MAX_CLIENTS; i++)
		if (event_client_list != NULL)
		{
			if ((event == event_client_list[i].event) &&
#ifndef __cplusplus
				(ds == event_client_list[i].ds) 
#else
				(device == event_client_list[i].device) 
#endif /* __cplusplus */
		    		&& (event_client_list[i].flag == DS_PENDING))
			{
				strncpy(client.server_name,event_client_list[i].server_name, sizeof(client.server_name));
				strncpy(client.server_host,event_client_list[i].server_host, sizeof(client.server_host));
				client.prog_number =  event_client_list[i].prog_number;
				client.vers_number =  event_client_list[i].vers_number;

        			dev_printdebug (DBG_ASYNCH, "\ndev_event_fire() : send event to client -> ");
        			dev_printdebug (DBG_ASYNCH, "event_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
		    			event_client_list[i].id, client.server_name, client.server_host, client.prog_number, client.vers_number);

/*
 * to send an event to the client asynchronously the client rpc service
 * must be imported. make sure it is imported.
 */
        			UNLOCK(async_mutex);
				if (asynch_client_check(&client, &error) != DS_OK)
				{
					dev_printerror (SEND,"%s", "dev_event_fire : server couldn't import client to fire event");
                                	return;
				}
        			LOCK(async_mutex);

/*
 * tag asynchronous information onto client_data so that client
 * can identify the reply
 *
 * return time command was executed by server to client tagged on
 * (simulate gettimeofday() with time() on VxWorks)
 */

#if defined (vxworks) || (WIN32)
				time(&tea_time);
				timenow.tv_sec = tea_time;
				timenow.tv_usec = 0;
#else
				gettimeofday(&timenow,&tz);

#endif /* !vxworks */

                        	dev_printdebug (DBG_ASYNCH, "\ndev_event_fire() : send event to client (time={%d,%d})\n",
				   	timenow.tv_sec,timenow.tv_usec);

				iarg = 0;
				vararg[iarg].argument_type	= D_ULONG_TYPE;
				vararg[iarg].argument = (DevArgument)&timenow.tv_sec;
				iarg++;

				vararg[iarg].argument_type	= D_ULONG_TYPE;
				vararg[iarg].argument = (DevArgument)&timenow.tv_usec;
				iarg++;

				asynch_client_data.asynch_id = event_client_list[i].id;
				asynch_client_data.status = event_status;
				asynch_client_data.error = event_error;
				asynch_client_data.argout = argout;
				asynch_client_data.argout_type = argout_type;

#ifdef NEVER 
/* no need for this management - simply point to argout ? 
 *
 * allocate space for client data and copy argout to this area
 */
        			if (asynch_client_data.argout_type != D_VOID_TYPE)
        			{
/*
 * Get the XDR data type from the loaded type list
 */
                			if ( xdr_get_type( asynch_client_data.argout_type, &data_type, &asynch_client_data.error) == DS_NOTOK)
                			{
	                        		dev_printdebug (DBG_ERROR | DBG_ASYNCH, "\ndev_event_fire() : xdr_get_type(%d) returned error %d\n",
		                            		asynch_client_data.argout_type, asynch_client_data.error);
                        			asynch_client_data.status = DS_NOTOK;
                			}

                			asynch_client_data.argout = (char *) malloc ((unsigned int)data_type.size);
                			if ( asynch_client_data.argout == NULL )
                			{
                        			asynch_client_data.status = DS_NOTOK;
                        			asynch_client_data.error  = DevErr_InsufficientMemory;
                			}
					else
                				memcpy (asynch_client_data.argout, argout, (size_t)data_type.size);
        			}
        			else
                			asynch_client_data.argout = NULL;
#endif /* NEVER */
				asynch_client_data.var_argument.length = iarg; 
				asynch_client_data.var_argument.sequence = vararg;
		
/*
 * send event using ONE-WAY rpc i.e. timeout=0 , this way the server
 * gets rid of the replies immediately.
 */
        			clnt_stat = clnt_call (client.asynch_clnt, RPC_PUTGET_ASYN_REPLY,
	                    	   	(xdrproc_t)xdr__asynch_client_data, (caddr_t) &asynch_client_data,
				   	(xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));

                        	dev_printdebug (DBG_ASYNCH, "\ndev_event_fire() : send event to client (clnt_stat %d)\n",clnt_stat);

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
 * shutdown the client tcp socket and call the event_client_cleanup()
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
  					dev_printerror (SEND,
                          		"dev_event_fire() : server couldn't send event to client (clnt_stat=%d) calling cleanup !", (char*)clnt_stat); 
					UNLOCK(async_mutex);
					event_client_cleanup(&error);
					LOCK(async_mutex);
									
  				}			
			}
		}

	dev_printdebug (DBG_ASYNCH, "\ndev_event_fire() : returning\n");
	UNLOCK(async_mutex);

	return;
}

/**@ingroup dsAPI
 * application interface for a client to unlisten to
 * an event. Client will be unregistered in the server
 * and will not receive anymore events of this type.
 * 
 * @param ds       	handle to access the device.
 * @param event_type    event to listen for
 * @param event_id  	client event identifier
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 * 
 * @return	DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_event_unlisten_x (devserver ds, long event_type,
                                  long event_id, long *error)
{
	_server_data		server_data;
	DevVarArgument		vararg[10];
	enum clnt_stat		clnt_stat;
	_Int			local_flag;
	long			client_id = 0;
	long			i_nethost;
	nethost_info		*nethost;
	short			iarg;
	long			event_index;
	DevString		name,host;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_ASYNCH,
	    "\ndev_event_unlisten_x() : entering routine\n");

#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
        {
		*error = DevErr_CommandNotImplemented;
                return(DS_NOTOK);
        }
#endif /* TANGO */
	LOCK(async_mutex);

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
 * Events are not supported for local devices.
 */
	if ( local_flag != True )
	{
		if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
		{
			UNLOCK(async_mutex);
			return (DS_NOTOK);
		}
	}
	else
	{
		*error = DevErr_AsynchronousCallsNotSupported;
		UNLOCK(async_mutex);
		return(DS_NOTOK);
	}
/*
 * If the security system is configured, 
 * verify the security key
 */
	if (!ds->no_database && (nethost->config_flags.security == True)
		&& (verify_sec_key (ds, &client_id, error) == DS_NOTOK))
		{
			UNLOCK(async_mutex);
			return (DS_NOTOK);
		}

/*
 * events use the asynchronous service of the device server - 
 * make sure it is imported. 
 */
	UNLOCK(async_mutex);
	if (asynch_server_import(ds,error) != DS_OK)
		return (DS_NOTOK);

	LOCK(async_mutex);
/*
 * store the pointers to the return arguments so that event can
 * be passed back asynchronously to the client
 *
 *  fill in data transfer structures server_data and client_data.
 */
	server_data.ds_id	= ds->ds_id;
	server_data.client_id	= client_id;
	server_data.access_right= ds->dev_access;
	server_data.cmd		= 0;
	server_data.argin_type	= D_VOID_TYPE;
	server_data.argout_type	= D_VOID_TYPE;
	server_data.argin	= (char *) NULL;
/*
 * additional arguments, in this case asynch_id and the return argument
 * pointers are passed to the device server via the variable argument 
 * array
 */
	server_data.var_argument.length = iarg = 0;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&event_id;
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
	server_data.var_argument.length++; iarg++;
	vararg[iarg].argument_type	= D_ULONG_TYPE;
	vararg[iarg].argument		= (DevArgument)&event_type;
	server_data.var_argument.length++;
	server_data.var_argument.sequence = vararg;

        dev_printdebug (DBG_TRACE | DBG_ASYNCH, "\ndev_event_unlisten_x() : client data -> ");
        dev_printdebug (DBG_ASYNCH, "event_type=%d asynch_id=%d name=%s host=%s prog_no=%d vers_no=%d\n",
	    event_type,event_id, config_flags.server_name,config_flags.server_host, config_flags.prog_number,config_flags.vers_number);

/*
 *  call RPC_EVENT_LISTEN using the client handle which has
 *  been opened to the asynchronous service of the server and
 *  ONE-WAY rpc i.e. xdr routine = xdr_void && timeout = zero
 */

	clnt_stat = clnt_call (ds->asynch_clnt, RPC_EVENT_UNLISTEN,
			    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
			    (xdrproc_t)xdr_void, (caddr_t) NULL, TIMEVAL(zero_timeout));
	dev_printdebug (DBG_ASYNCH, "\ndev_event_unlisten_x() : clnt_stat %d\n",clnt_stat);

/*
 * Check for errors on the RPC connection.
 */
	if ((clnt_stat != RPC_TIMEDOUT) && (dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK))
	{
		UNLOCK(async_mutex);
		return (DS_NOTOK);
	}
/*
 * remove event from list of asynchronous calls
 */
	if ((event_index = asynch_get_index(event_id)) >= 0)
	{
		client_asynch_request.args[event_index].flag = DS_FALSE;
		client_asynch_request.pending--;
		ds->pending--;
	}

	UNLOCK(async_mutex);
	return (DS_OK);
}

/**@ingroup dsAPI
 * internal housekeeping function used on the server
 * side to cleanup event clients which are not
 * responding anymore. They are removed from the
 * list and will not receive events.
 * 
 * @param error	Will contain an appropriate error code if the 
 *		corresponding call returns a non-zero value.
 */
void _DLLFunc event_client_cleanup (long *error)
{
	long	i, 
		i_client;
	dev_printdebug (DBG_ASYNCH, "\nevent_client_cleanup() : entering\n");
	LOCK(async_mutex);

/*
 * first call asynch_client_cleanup() to detect any dead clients
 */
	asynch_client_cleanup(error);
/*
 * now loop through all clients who have registered for an event and
 * remove those which have been detected as dead
 */
	if (event_client_list != NULL)
		for (i=0; i<EVENT_MAX_CLIENTS; i++)
			if (event_client_list[i].flag == DS_PENDING)
			{
				i_client = event_client_list[i].no_svc_conn;
				if (svr_conns[i_client].asynch_clnt == NULL)
				{
					event_client_list[i].flag = DS_FALSE;
        				dev_printdebug (DBG_ASYNCH,
				                "\nevent_client_cleanup() : removed client %d from list of clients registered for events\n",i);
				}
			}
	dev_printdebug (DBG_ASYNCH, "\nevent_client_cleanup() : returning\n");
	UNLOCK(async_mutex);
	return;
}

/**@ingroup dsAPI
 * rpc function used on the server side to register new clients. 
 * They are added to the global list of registered clients and 
 * will be sent events by the dev_event_fire() call.
 * 
 * @param server_data client data
 * 
 * @return DS_OK or DS_NOTOK
 */ 
_dev_import_out* _DLLFunc rpc_event_listen_5 (_server_data *server_data)
{
	static struct _dev_import_out 	dev_import_out;
	long				i, 
					i_client;
	struct _devserver 		client;
	long				status, 
					error;
	static long			first=1;

/*
 * first time round initialise event_client_list[] 
 */
	LOCK(async_mutex);

	if (first || event_client_list == NULL)
	{
		event_client_list = (event_client*)malloc(sizeof(event_client)*EVENT_MAX_CLIENTS);
		if(!event_client_list)
		{
			UNLOCK(async_mutex);
			return NULL;
		}
		for (i=0; i<EVENT_MAX_CLIENTS; i++)
			event_client_list[i].flag = DS_FALSE;
		first = 0;
	}
/*
 * find next free slot
 */
	i_client = -1;
	for (i=0; i<EVENT_MAX_CLIENTS; i++)
		if (event_client_list[i].flag == DS_FALSE)
		{
			i_client = i;
			break;
		}
	
	if ((i < 0) || (i > EVENT_MAX_CLIENTS))
	{
		fprintf(stderr, "rpc_event_listen_5(): no more free event client slots !\n");
		UNLOCK(async_mutex);
		return(&dev_import_out);
	}

/*
 * initialise all event client fields so that events can be sent
 */
	event_client_list[i_client].ds_id = server_data->ds_id;
#ifndef __cplusplus
	event_client_list[i_client].ds = devices[server_data->ds_id&DEVICES_MASK].ds;
#else
	event_client_list[i_client].device = devices[server_data->ds_id&DEVICES_MASK].device;
#endif /* __cplusplus */
	event_client_list[i_client].id = *(long*)server_data->var_argument.sequence[0].argument;
	event_client_list[i_client].server_name = (char*)malloc(strlen(*(char**)server_data->var_argument.sequence[1].argument)+1);
	if(!event_client_list[i_client].server_name)
	{
		error=DevErr_InsufficientMemory;
		UNLOCK(async_mutex);
		return NULL;
	}
	strcpy(event_client_list[i_client].server_name, *(char**)server_data->var_argument.sequence[1].argument);
	event_client_list[i_client].server_host = (char*)malloc(strlen(*(char**)server_data->var_argument.sequence[2].argument)+1);
	if(!event_client_list[i_client].server_host)
	{
		free(event_client_list[i_client].server_name);
		error=DevErr_InsufficientMemory;
		UNLOCK(async_mutex);
		return NULL;
	}

	strcpy(event_client_list[i_client].server_host,*(char**)server_data->var_argument.sequence[2].argument);
	event_client_list[i_client].prog_number = *(long*)server_data->var_argument.sequence[3].argument;
	event_client_list[i_client].vers_number = *(long*)server_data->var_argument.sequence[4].argument;
	event_client_list[i_client].event = *(long*)server_data->var_argument.sequence[5].argument;
	event_client_list[i_client].argout_type = server_data->argout_type;

        dev_printdebug (DBG_ASYNCH, "\nrpc_event_listen_5() : event client data -> ");
        dev_printdebug (DBG_ASYNCH, "event=%d id=%d server_name=%s server_host=%s argout_type=%d\n",
	     event_client_list[i_client].event,event_client_list[i_client].id, 
	     event_client_list[i_client].server_name, event_client_list[i_client].server_host, event_client_list[i_client].argout_type);

/*
 * import asynchronous service of client which wants to receive events
 */
	strncpy(client.server_name,event_client_list[i_client].server_name, sizeof(client.server_name));
	strncpy(client.server_host,event_client_list[i_client].server_host, sizeof(client.server_host));
	client.prog_number = event_client_list[i_client].prog_number;
	client.vers_number = event_client_list[i_client].vers_number;

	UNLOCK(async_mutex);
	status = asynch_client_check(&client, &error);
	LOCK(async_mutex);

	if (status == DS_OK)
	{
		event_client_list[i_client].no_svc_conn = client.no_svr_conn;
		svr_conns[client.no_svr_conn].no_conns++; /* BP: needed to avoid premature freeing of async service ? */
		event_client_list[i_client].flag = DS_PENDING;
	}
	else 
	{
		free(event_client_list[i_client].server_name);
		free(event_client_list[i_client].server_host);
		event_client_list[i_client].flag = DS_FALSE;
	}
	UNLOCK(async_mutex);
	return(&dev_import_out);
}

/**@ingroup dsAPI
 * rpc function used on the server side to unregister a clients. 
 * It is removed from the global list of registered clients and 
 * will be not be sent events anymore by the dev_event_fire() call.
 *
 * @param server_data client data
 *
 * @return DS_OK or DS_NOTOK
 */
_dev_free_out* _DLLFunc rpc_event_unlisten_5 (_server_data *server_data)
{
	static struct _dev_free_out dev_free_out;
	long		i, i_client,no_svc_conn;
	long		event_id, event_type, prog_number;
	char 		*server_name, *server_host;

/*
 * get event client fields so that client can be unregistered
 */
	LOCK(async_mutex);
	event_id = *(long*)server_data->var_argument.sequence[0].argument;
	server_name = *(char**)server_data->var_argument.sequence[1].argument;
	server_host = *(char**)server_data->var_argument.sequence[2].argument;
	prog_number = *(long*)server_data->var_argument.sequence[3].argument;
	event_type = *(long*)server_data->var_argument.sequence[5].argument;
        dev_printdebug (DBG_ASYNCH, "\nrpc_event_unlisten_5() : event client data -> ");
        dev_printdebug (DBG_ASYNCH, "type=%d id=%d name=%s host=%s program=%d\n",
	     event_type, event_id, server_name, server_host, prog_number);

/*
 * find event client
 */
	i_client = -1;
	if (event_client_list != NULL)
		for (i=0; i<EVENT_MAX_CLIENTS; i++)
			if (event_client_list[i].flag != DS_FALSE)
				if ((strcmp(server_name,event_client_list[i].server_name) == 0) 
					&& (strcmp(server_host,event_client_list[i].server_host) == 0) 
					&& (prog_number == event_client_list[i].prog_number))
				{
        				dev_printdebug (DBG_ASYNCH, "\nrpc_event_unlisten_5() : found client at i_client=%d\n",i_client);
					i_client = i;
					break;
				}
	if (i_client < 0)
	{
		fprintf(stderr, "rpc_event_unlisten_5(): event client not found !\n");
		UNLOCK(async_mutex);
		return(&dev_free_out);
	}
	else
	{
		free(event_client_list[i_client].server_name);
		free(event_client_list[i_client].server_host);
		event_client_list[i_client].flag = DS_FALSE;
		no_svc_conn = event_client_list[i_client].no_svc_conn;
		svr_conns[no_svc_conn].no_conns--;
		if ( svr_conns[no_svc_conn].no_conns == 0)
		{
                	clnt_destroy (svr_conns[no_svc_conn].asynch_clnt);
			svr_conns[no_svc_conn].asynch_clnt = NULL;
        		dev_printdebug (DBG_ASYNCH, "rpc_event_unlisten_5(): destroy asynch client handle\n");
		}
	}
	UNLOCK(async_mutex);

	return(&dev_free_out);
}

static db_resource   res_tab [] = {
	{(char *)"Out_Type", D_STRING_TYPE, NULL},
};

static long get_event_string PT_( (devserver ds, long event, char *event_str, long *error) );

/**@ingroup dsAPI
 * Returns a sequence of structures containing all
 * available events, their names, their input and
 * output data types, and type describtions for one
 * device.
 *
 * Events and data types are read from the event
 * list in the device server by calling 
 * RPC_DEV_EVENT_QUERY.
 *
 * Event names are read from the event name list,
 * defined in the database (EVENT/team/server/event): 
 * 
 * Data type describtions have to be specified as 
 * CLASS resources as: CLASS/class_name/event_name/OUT_TYPE:
 *
 * @param ds 		client handle for the associated device.
 * @param vareventarr 	sequence of DevEventInfo structures.
 * @param error     	Will contain an appropriate error code if the
 *			corresponding call returns a non-zero value.
 *
 * Return(s)  :	DS_OK or DS_NOTOK
 */
long _DLLFunc dev_event_query (devserver ds, DevVarEventArray *vareventarr, long *error)
{
	_dev_query_in		dev_query_in;
	_dev_queryevent_out	dev_query_out;
	enum clnt_stat  	clnt_stat;
	char			class_name[SHORT_NAME_SIZE],
				res_path [LONG_NAME_SIZE],
				event_name[SHORT_NAME_SIZE];
	long			length;
	long			ret_stat;
	int			i;
	u_int 			res_tab_size = sizeof(res_tab) / sizeof(db_resource);
	long			status;
	static char		**event_names=NULL;
	int			n_event_names;

	*error = 0;
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_event_query() : entering routine\n");

#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
		*error = DevErr_CommandNotImplemented;
		status = DS_NOTOK;
		status = tango_dev_event_query(ds, vareventarr, error);
		return(status);
	}
#endif /* TANGO */
/*
 * Verify the RPC connection.
 */
	if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
		return (DS_NOTOK);

/*
 *  fill in data transfer structures dev_query_in
 *  and dev_query_out.
 */
	dev_query_in.ds_id = ds->ds_id;
	dev_query_in.var_argument.length   = 0;
	dev_query_in.var_argument.sequence = NULL;

/*
 *  Call the rpc entry point RPC_DEV_EVENT_QUERY at the specified device server.
 */
	memset ((char *)&dev_query_out, 0, sizeof (dev_query_out));

/*
 * Query a device with the current version number >1.
 */
	if ( ds->vers_number > DEVSERVER_VERS)
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_EVENT_QUERY,
		    (xdrproc_t)xdr__dev_query_in,  (caddr_t) &dev_query_in,
		    (xdrproc_t)xdr__dev_queryevent_out, (caddr_t) &dev_query_out, TIMEVAL(timeout));
	else
/*
 * Query a device from an old version server. THIS is not supported for old servers
 */
		    return (DS_NOTOK);

/*
 * Check for errors on the RPC connection.
 */
	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
		return (DS_NOTOK);

/*
 * Free the variable arguments in the dev_query_out structure, coming from the server.
 */
        if (dev_query_out.var_argument.length > 0)
        {
                event_names = (char**)realloc(event_names,dev_query_out.var_argument.length*sizeof(char*));
		if (event_names == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return DS_NOTOK;
		}
                for (i=0; i<dev_query_out.var_argument.length; i++)
                {
                        event_names[i] = (char*)malloc(strlen(*(char**)dev_query_out.var_argument.sequence[i].argument)+1);
			if (event_names[i] == NULL)
			{
				int j;
				for (j = 0; j < i; ++i)
					free(event_names[j]);
				free(event_names);
				*error  = DevErr_InsufficientMemory;
				return DS_NOTOK;
			}
                        strcpy(event_names[i], *(char**)dev_query_out.var_argument.sequence[i].argument);
                }
        }
	n_event_names = dev_query_out.var_argument.length;

	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(dev_query_out.var_argument));
/*
 * Allocate memory for a sequence of DevEventInfo structures
 * returned with vareventarr.
 */
	vareventarr->length   = dev_query_out.length;
	vareventarr->sequence = (DevEventInfo *) malloc (vareventarr->length * sizeof (DevEventInfo));
	if ( vareventarr->sequence == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (-1);
	}
	memset ((char *)vareventarr->sequence, 0, (vareventarr->length * sizeof (DevEventInfo)));

/*
 * Now get command and types name strings for the returned
 * command sequence. Command names are retrieved from the
 * global command-name-list and name strings for the data types
 * are searched in the resource CLASS table of the object class.
 * 
 * Undefined names will be initialised with NULL.
 */
	for ( i=0; (u_long)i<vareventarr->length; i++ )
	{
/*
 * initialise vareventarr->sequence[i] with command and
 * argument types, returned with dev_query_out from the
 * device servers command list.
 */
		vareventarr->sequence[i].event      = dev_query_out.sequence[i].event;
		vareventarr->sequence[i].out_type = dev_query_out.sequence[i].out_type;

/*
 * check to see if device server returned event names
 */
                if (i < n_event_names && n_event_names > 0)
                {
                        strncpy(vareventarr->sequence[i].event_name,event_names[i], sizeof(vareventarr->sequence[i].event_name));
                        free(event_names[i]);
                }
                else
                {
/*
 * get command name string from the resource database
 */
			if (!ds->no_database)
			{
				if ((ret_stat = get_event_string (ds, vareventarr->sequence[i].event, vareventarr->sequence[i].event_name, error)) == DS_NOTOK)
/*
 * An error will be only returned if the database access fails.
 */
					return (DS_NOTOK);
			}
			else
				snprintf(vareventarr->sequence[i].event_name, sizeof(vareventarr->sequence[i].event_name), "event%s",i);
		}

/*
 *  Check wether command name was found.
 *  If the name was not found, get_event_string() returns DS_WARNING.
 */
		if (!ds->no_database && (ret_stat != DS_WARNING))
		{
/*
 * Limit the class_name and the command_name
 * strings to 19 characters. This is the limit
 * of the static database name fields.
 */
			length = strlen (dev_query_out.class_name);
			if ( length > MAX_RESOURCE_FIELD_LENGTH )
				length = MAX_RESOURCE_FIELD_LENGTH;
			strncpy (class_name, dev_query_out.class_name, MAX_RESOURCE_FIELD_LENGTH);
			class_name[(_Int)length] = '\0';

			length = strlen (vareventarr->sequence[i].event_name);
			if ( length > MAX_RESOURCE_FIELD_LENGTH )
				length = MAX_RESOURCE_FIELD_LENGTH;
			strncpy (event_name, vareventarr->sequence[i].event_name, MAX_RESOURCE_FIELD_LENGTH);
			event_name[(_Int)length] = '\0';

/*
 * setup resource path to read information about
 * data types from the CLASS resource table.
 *
 * but first check to see whether the device belongs to another
 * nethost domain i.e. i_nethost != 0
 */
			if (ds->i_nethost > -1)
				snprintf(res_path, sizeof(res_path), "//%s/CLASS/%s/%s", get_nethost_by_index(ds->i_nethost, error), class_name, event_name);
/*
 * use default nethost
 */
			else
				snprintf (res_path, sizeof(res_path), "CLASS/%s/%s", class_name, event_name);

/*
 *  read CLASS resources from database
 */
			res_tab[0].resource_adr = &(vareventarr->sequence[i].out_name);
			if (db_getresource (res_path, res_tab, res_tab_size, error) < 0)
				return (DS_NOTOK);
		}
/*
 * no database, set out_name to NULL
 */
		else
			vareventarr->sequence[i].out_name = NULL;
	}

/*
 *  free dev_query_out 
 */
	xdr_free ((xdrproc_t)xdr__dev_queryevent_out, (char *)&dev_query_out);

/*
 * Return error code and status from device server.
 */
	*error = dev_query_out.error;
	return (dev_query_out.status);
}


/**@ingroup dsAPI
 * Read the event name as a string from the resource database.
 *
 * The resource name is: EVENT/team_no/server_no/event_ident
 *
 * DS_WARNING is returned, if the function was executed correctly, but no event 
 * name string was found in the database.
 *
 * @param ds		client device handle
 * @param event      	event number
 * @param event_str 	event name as a string.
 * @param error   	Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 * @return DS_OK or DS_NOTOK or DS_WARNING
 */
static long get_event_string (devserver ds, long event, char *event_str, long *error)
{
	char		res_path[LONG_NAME_SIZE],
			res_name[SHORT_NAME_SIZE],
			*ret_str = NULL;
	db_resource 	res_tab;
	unsigned long 	event_number_mask = 0x3ffff;
	unsigned short 	team,
			server,
			events_ident;

	dev_printdebug (DBG_TRACE | DBG_API, "\nget_event_string() : entering routine\n");

	*error = 0;

	/*
 * Decode the command nuber into the fields:
 * team, server and events_ident.
 */
	team   = (_Int)(event >> DS_TEAM_SHIFT);
	team   = team & DS_TEAM_MASK;
	server = (_Int)(event >> DS_IDENT_SHIFT);
	server = server & DS_IDENT_MASK;
	events_ident = (_Int)(event & event_number_mask);

/*
 * Create the resource path and the resource structure.
 *
 * first check to see whether the device belongs to another
 * nethost domain i.e. i_nethost != 0
 */
	if (ds->i_nethost > 0)
		snprintf(res_path, sizeof(res_path), "//%s/EVENTS/%d/%d", get_nethost_by_index(ds->i_nethost, error), team, server);
/*
 * use default nethost
 */
	else
		snprintf(res_path, sizeof(res_path),"EVENTS/%d/%d", team, server);

	snprintf (res_name, sizeof(res_name), "%d", events_ident);
	dev_printdebug (DBG_API, "get_events_string() : res_path = %s\n", res_path);
	dev_printdebug (DBG_API, "get_events_string() : res_name = %s\n", res_name);

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the command name string from the database.
 */

	if (db_getresource (res_path, &res_tab, 1, error) == DS_NOTOK)
	{
		dev_printdebug (DBG_API | DBG_ERROR, "get_event_string() : db_getresource failed with error %d\n", *error);

		return (DS_NOTOK);
	}

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
		return (DS_WARNING);

	snprintf (event_str, sizeof(event_str), "%s", ret_str);
	free (ret_str);
	return (DS_OK);
}

