/*+*******************************************************************

 File       :	dev_api.c

 Project    :	Device Servers with SUN-RPC

 Description:	Application Programmers Interface 
		
            :	Interface to access and handle remote devices.

 Author(s)  :	Andy Goetz
		Jens Meyer
 		$Author: jkrueger1 $

 Original   :	January 1991

 Version    :	$Revision: 1.18 $

 Date	    :	$Date: 2004-10-26 08:46:39 $

 Copyright (c) 1990-2000 by European Synchrotron Radiation Facility, 
                            Grenoble, France

********************************************************************-*/
#include <config.h>
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <Admin.h>
#include <DevErrors.h>
#include <API_xdr_vers3.h>

#if (!defined WIN32)
#	if ( (defined OSK) || (defined _OSK))
#		include <inet/socket.h>
#		include <inet/netdb.h>
#	else /* OSK || _OSK */
#		if HAVE_SYS_FILIO_H
#			include <sys/filio.h>
#		endif /* sun */
#		if HAVE_SYS_SOCKET_H
#			include <sys/socket.h>
#		else
#			include <socket.h>
#		endif
#		include <netinet/in.h>
#		if !defined vxworks
#			include <netdb.h>
#		else /* !vxworks */
#			include <hostLib.h>
#			include <pingLib.h>
#		endif  /* !vxworks */
#		ifdef lynx
#			include <ioctl.h>
#		endif /*lynx */
#		if HAVE_SYS_TYPES_H
#			include <sys/types.h>
#		endif
#		if HAVE_SYS_IOCTL_H
#			include <sys/ioctl.h>
#		endif
#		ifdef linux
#			include <linux/posix_types.h>
#		endif /* linux */
#	endif /* OSK || _OSK */
#endif /* WIN32 */

#include <errno.h>
#include <assert.h>

/*
 * Functions reused in modul util_api.c
 */

long _DLLFunc 	dev_query_svr PT_( (char* host,long prog_number,long vers_number) );
static long 	dev_import_local PT_( (_dev_import_in  *dev_import_in, devserver  *ds_ptr, long* error) );
static long 	dev_free_local PT_( (_dev_free_in  *dev_free_in, long* error) );
static long 	dev_put_local PT_( (_server_data  *server_data, long* error) );
static long 	dev_putget_local PT_( (_server_data  *server_data, _client_data  *client_data, long* error) );
long _DLLFunc	dev_notimported_init PT_( (char *device_name, long access, long i_nethost, devserver *ds_ptr, long *error) );

/*
 * local data used by each client is kept in external area
 *
 * the following variables keep track of the open connections
 * to servers, this data allows optimisation of the server
 * connections i.e. sockets. if the client wants to connect
 * to the same server  (identified by its host and program number) then
 * it is not necessary to reimport the service, the same connection
 * can be used.
 *
 * !!! note this depends on using non-forking servers - andy 05apr90
 *
 * as many connections are allowed as files open i.e. _NFILE
 * variable _NFILE replaced by NFILE in API.h because it is not
 * defined in sun unix.
 */

/****************************************
 *          Globals	                *
 ****************************************/

/*
 *  Types for global state flags for the current
 *  server or client status and for Database Server
 *  information are defined in API.h
 */

/*
 *  Configuration flags for the default control system
 */

#ifdef __cplusplus
extern "C" {
#endif
	extern configuration_flags      config_flags;
	extern nethost_info 		*multi_nethost;
/*
 * dynamic error string
 */
	extern char 			*dev_error_string;
#ifdef __cplusplus
};
#endif

/*
 * Global structure for multiple control systems, setup by
 * setup_config_multi() but used by all multi-nethost functions
 */

/*  
 *  Structure for the administration of open RPC connections.
 */

server_connections	 	svr_conns [NFILE];


/**@ingroup syncAPI
 * This function opens a connection to a device and returns a client handle for the connection.
 * This function can distinguish between local and remote devices.
 *
 * If the control system is running with security on then the axxess parameter determines what 
 * level of access permission the client wants on the device. The following levels are supported:
 * 	-# READ_ACCESS for read-only access
 * 	-# WRITE_ACCESS for read and write access (@b default)
 *	-# SI_WRITE_ACCESS for single user write access 
 *	-# SU_ACCESS for super-user access
 *	-# SI_SU_ACCESS for single super-user access
 *	-# ADMIN_ACCESS for administrator access
 *
 * The default access is WRITE_ACCESS and corresponds to access = 0. If the TACO control system
 * is running with security the client has to have the necessary permission in the security database
 * for the (UID,GID,HOST,NETWORK) quadrupole.
 *
 * For more information on security refer to "Access Control and Security for the ESRF Control System"
 * by J. Meyer (DSN/102)
 *
 * @param dev_name 	name of the device
 * @param access   	not used yet
 * @param ds_ptr 	a handle to access the device
 * @param error        	Will contain an appropriate error code if the corresponding call 
 *			returns DS_NOTOK
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_import (char *dev_name, long access, devserver *ds_ptr, long *error)
{
	static _dev_import_out	dev_import_out;
	_dev_import_in		dev_import_in;
	CLIENT			*clnt = NULL;
	enum clnt_stat		clnt_stat;

	Db_devinf_imp		devinfo;
	char			*device_name,
				device_class[SHORT_NAME_SIZE],
				device_type[SHORT_NAME_SIZE],
				host_name[SHORT_NAME_SIZE];
	long			prog_number,
				vers_number,
				i_nethost,
				client_id     = 0,
				connection_id = 0,
				status;
	int			n_svr_conn,
				len,
				i,
				no_database = False;
	char			name [256],
				*hstring,
				nethost[80],
				*prog_url;

	*error = 0;
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_import() : entering routine\n");

#ifdef TANGO
	if (strncasecmp(dev_name,"tango:",6) == 0)
	{
		status = tango_dev_import(dev_name+6,access, ds_ptr, error);
		printf("dev_import(): tango_dev_import(%s) returned %d\n",dev_name,status);
		return(status);
	}
#endif /* TANGO */
/*
 *  first convert the device name to lower case letters
 */
	strncpy (name, dev_name, sizeof(name));

	len = strlen (name);
	device_name = name;
	for (i=0; i<len; i++, device_name++)
		*device_name = tolower (*device_name);
	device_name = name;
/*
 * now check to see whether the nethost is specified
 * in the device name e.g. "//nethost/domain/family/member"
 * by calling the function get_i_nethost_from_device_name()
 * if nethost is not specified the default nethost (0)
 * will be returned
 */
	if ((i_nethost = get_i_nethost_by_device_name(device_name,error)) < 0)
	{
/*
 * nethost not imported, extract nethost and import it
 *
 * make a copy of the nethost name without the "//" and
 * removing the device name which follows
 */
		strncpy(nethost,device_name+2, sizeof(nethost));
		for (i=0; i<(int)strlen(nethost); i++)
			if (nethost[i] == '/')
			{
				nethost[i] = 0;
				break;
			}
/*
 * now check to see if this is a device without database
 * by checking for the "?number" in the device name
 */
		if (strchr(device_name,'?') != NULL)
		{
			prog_url = strchr(device_name,'?');
			sscanf(prog_url+1,"%d",&prog_number);
			no_database = True;
/*
 * load xdr kernel types (normally this is done in setup_config())
 */
        		if ( xdr_load_kernel (error) == DS_NOTOK )
        		{
				*error = DevErr_XdrDataTypeNotLoaded;
                		return (DS_NOTOK);
        		}
		}
		else
		{
/*
 * the specified nethost is not in the list of imported nethosts,
 * therefore call setup_config_multi() to add it
 */
			if (setup_config_multi(nethost,error) != DS_OK)
				return(DS_NOTOK);
			i_nethost = get_i_nethost_by_name(nethost,error);
		}
	}

/*
 * the specified nethost now exists in the list of imported nethosts
 * i_nethost is the index into the array of nethosts
 *
 * check wether a database server is already imported
 */
	if (!no_database)
	{
		if (i_nethost == 0)
		{
			if ( !config_flags.database_server )
			{
				if (db_import(error) != DS_OK)
				{
					return(DS_NOTOK);
				}
			}
		}
		else
		{
			if ( !multi_nethost[i_nethost].config_flags.database_server )
			{
				if ( db_import_multi (nethost,error) != DS_OK )
					return (DS_NOTOK);
			}
		}

		dev_printdebug (DBG_API, "dev_import() : try to import %s\n",device_name);

/*
 * if the security system is configured, check the user access on the control system 
 * and the requested access right.
 */
		if ((multi_nethost[i_nethost].config_flags.security == True )
			&& dev_security(device_name,access,&client_id, &connection_id, error) == DS_NOTOK )
				return (DS_NOTOK);

/*
 * get device server host name, programm number and version number from the devices 
 * database table. Information will be asked from the current network database server.
 */
		if ( db_dev_import(&device_name,&devinfo,1,error) < 0)
		{
/*
 * the following introduces "stateless-ness" to the device server api
 * by ignoring the error "DeviceNotExported" during the dev_import().
 * This error means the device * is defined in the NAMES table of the
 * database but is presently not served by a device server. In the new
 * version of the api (5.6) dev_import() will return OK and subsequent
 * calls to the dev_putget() will try to import the device. All other
 * errors returned by the database are still considered as serious
 * e.g. DeviceNotDefined, and will cause dev_import() to fail.
 */
                	if (*error == DbErr_DeviceNotExported)
                	{
        			dev_printdebug (DBG_API,"dev_import(): device %s not exported (yet)\n",device_name);
	                        return(dev_notimported_init(device_name,access,i_nethost,ds_ptr,error));
                	}
                	else
                        	return (DS_NOTOK);
		}
		dev_printdebug(DBG_API, "dev_import() : Info from database:\n");
		dev_printdebug(DBG_API, "class = %s   type = %s\n", devinfo[0].device_class, devinfo[0].device_type);
		dev_printdebug(DBG_API, "host = %s  pn = %d  vn = %d\n",devinfo[0].host_name, devinfo[0].pn, devinfo[0].vn);

		strncpy(device_class, devinfo[0].device_class, sizeof(device_class));
		strncpy(device_type, devinfo[0].device_type, sizeof(device_type));
		strncpy(host_name, devinfo[0].host_name, sizeof(host_name));
		prog_number = devinfo[0].pn;
		vers_number = API_VERSION;

/*
 * now free the devinfo structure allocated by db_dev_import().
 */
		free (devinfo);
	}
/*
 * device has no database, 
 */
	else
	{
		strncpy(device_class,"unknown", sizeof(device_class));
		strncpy(device_type,"unknown", sizeof(device_type));
		strncpy(host_name,nethost, sizeof(host_name));
		vers_number = API_VERSION;
	}

/*
 * Initialize the input arguments of the import call
 */
	dev_import_in.device_name   = extract_device_name(device_name,error);
	dev_import_in.access_right  = access;
	dev_import_in.client_id     = client_id;
	dev_import_in.connection_id = connection_id;

	dev_import_in.var_argument.length   = 0;
	dev_import_in.var_argument.sequence = NULL;

/*
 * Find out whether the process is a device server and the requested device is 
 * served locally in the process. 
 */
#if !defined __BORLANDC__
	if (config_flags.startup == SERVER_STARTUP || config_flags.startup == True)
#else
	if (config_flags.startup == True)
#endif
	{
		if ( strcmp (host_name, config_flags.server_host) == 0 &&
		    prog_number == config_flags.prog_number )
		{
/*
 * Call the import function for local devices.
 */
			if ( dev_import_local (&dev_import_in, ds_ptr, error) == DS_NOTOK )
				return (DS_NOTOK);

/*
 * Initialise all missing fields in the client handle.
 */
			strncpy((*ds_ptr)->device_class,device_class, sizeof((*ds_ptr)->device_class));
			strncpy((*ds_ptr)->device_type,device_type, sizeof((*ds_ptr)->device_type));
			strncpy((*ds_ptr)->server_host,host_name, sizeof((*ds_ptr)->server_host));
			(*ds_ptr)->prog_number      = prog_number;
			(*ds_ptr)->vers_number      = vers_number;

/*
 * If the device import has worked, create the security
 * key of the client handle.
 */
			if ((config_flags.security == True) && (create_sec_key ((*ds_ptr), error) == DS_NOTOK ))
					return (DS_NOTOK);
			return (DS_OK);
		}
	}

/*
 * If the device is not locally served, build up the RPC
 * connection.
 *
 * check if there is already a open connection to
 * this device server
 */
	if ((n_svr_conn=dev_query_svr (host_name,prog_number,vers_number)) < 0)
	{
		*error = DevErr_ExceededMaximumNoOfServers;
		return(-1);
	}

	if ( svr_conns[n_svr_conn].no_conns == 0)
	{
/*
 * Before a new handle can be created, verify whether it is possible to connect
 * to the remote host.
 */
		if (rpc_check_host(host_name, error) == DS_NOTOK)
		{
        		dev_printdebug (DBG_API,"dev_import(): host %s not answering, do stateless import\n",host_name);
                       	return(dev_notimported_init(device_name,access,i_nethost,ds_ptr,error));
		}

/*
 * No old connection exists to this server. Create new client handle the device server.
 */
		dev_printdebug (DBG_API, "dev_import() : open a new client handle \n");
		clnt = clnt_create (host_name, prog_number, vers_number, "tcp");
		if (clnt == NULL)
		{
/*
 * in order to make no database stateless return DS_OK here. I don't know what side 
 * effects this can have on the other (with database) device servers ... !
 */
                        dev_notimported_init(device_name,access,i_nethost,ds_ptr,error);
			strncpy((*ds_ptr)->server_host,host_name, sizeof((*ds_ptr)->server_host));
			(*ds_ptr)->prog_number      = prog_number;
			(*ds_ptr)->vers_number      = vers_number;
			return(DS_OK);
		}

/* 
 * This part was added for compatibility reasons with the old libray version 3.
 * If the server is not version 4, the version number must be set to 1.  Even if the 
 * last library version was 3, because 1 indicates the RPC service version.
 *
 * To make the version check the null procedure of the service is called. A version 
 * mismatch will be returned, if the service runs version 3 software. The RPC version 
 * number will be set to one in this case.
 *
 * USE A timeout of 2s to TRY TO CIRCUMVENT THE OS9 PROBLEM OF BLOCKING TCP CONNECTIONS 
 * ANDY 24OCT97
 *
 * changed to import_timeout which is a global which can be changed by clients 
 * andy 13mar2000
 */
 		clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &import_retry_timeout);
 		clnt_control (clnt, CLSET_TIMEOUT, (char *) &import_timeout);

		clnt_stat = clnt_call (clnt, NULLPROC,
				    (xdrproc_t)xdr_void,  NULL, (xdrproc_t)xdr_void, NULL, 
				    TIMEVAL(import_timeout));

		if (clnt_stat != RPC_SUCCESS)
		{
			if ( clnt_stat == RPC_PROGVERSMISMATCH )
			{
/*
 * Destroy version 4 handle.
 */
				clnt_destroy (clnt);

/*
 * Set version number to 1 and recreate the client handle.
 */
				vers_number = DEVSERVER_VERS;
				clnt = clnt_create (host_name,prog_number, vers_number,"tcp");
				if (clnt == NULL)
				{
					*error = DevErr_CannotCreateClientHandle;
					return (DS_NOTOK);
				}
			}

			else
			{
/*
 * treat all errors the same i.e. stateless import 
 * andy 25/6/02
 */
				clnt_destroy (clnt);
				clnt = NULL;
/*
 * add "stateless-ness" by ignoring RPC timeouts at import time this code assumes that 
 * because there was no version mismatch version 4 exists (i.e. version which supports 
 * security) and that the null procedure timed out because the device server was blocked.
 *
 * initialise the devserver struct so that the device can be properly imported next time round.
 */
                       		dev_notimported_init(device_name,access,i_nethost,ds_ptr,error);

				*error = 0;
				return (DS_OK);
			}
		}

		if (config_flags.startup == True)
		{
			clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &inner_retry_timeout);
			clnt_control (clnt, CLSET_TIMEOUT, (char *) &inner_timeout);
		}
		else
		{
			clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &retry_timeout);
			clnt_control (clnt, CLSET_TIMEOUT, (char *) &timeout);
		}

/*
 * initialise the administration structure for the RPC connection
 */
		svr_conns[n_svr_conn].clnt        = clnt;
		svr_conns[n_svr_conn].vers_number = vers_number;
		svr_conns[n_svr_conn].tcp_clnt = clnt;
		svr_conns[n_svr_conn].udp_clnt = NULL;
		svr_conns[n_svr_conn].asynch_clnt = NULL;
	}
	else
	{
/* 
 * a connection already exists to this server, reuse it
 */
		dev_printdebug (DBG_API, "dev_import() : reuse already open client handle\n");
		clnt        = svr_conns[n_svr_conn].clnt;
		vers_number = svr_conns[n_svr_conn].vers_number;
	}

/*
 * If the security system is used, switch to TCP protocol
 * if a single user or administration access is requested.
 */
	if (!no_database  && (multi_nethost[i_nethost].config_flags.security == True)
		&& (sec_tcp_connection (access, &clnt, &svr_conns[n_svr_conn], error) == DS_NOTOK))
		return (DS_NOTOK);

	dev_printdebug (DBG_API, "dev_import() : import device %s from device server\n", device_name);

/*
 *
 *  Store the current RPC timeout for the connection and
 *  change the timeout to a short api_timeout.
 */
 	clnt_control (clnt, CLGET_RETRY_TIMEOUT, (char *)&svr_conns[n_svr_conn].rpc_retry_timeout);
 	clnt_control (clnt, CLGET_TIMEOUT, (char *)&svr_conns[n_svr_conn].rpc_timeout);
 	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *)&import_retry_timeout);
 	clnt_control (clnt, CLSET_TIMEOUT, (char *)&import_timeout);

/*
 * import device from device server.
 * call RPC_DEV_IMPORT.
 *
 * Import the device with the current version number > 1.
 */
	if ( vers_number > DEVSERVER_VERS)
		clnt_stat = clnt_call (clnt, RPC_DEV_IMPORT,
		    (xdrproc_t)xdr__dev_import_in,  (caddr_t) &dev_import_in,
		    (xdrproc_t)xdr__dev_import_out, (caddr_t) &dev_import_out, TIMEVAL(timeout));
	else
	{
/*
 * If the security system is active, free the reserved
 * field for the security connection for old versions.
 */
		if (!no_database && (multi_nethost[i_nethost].config_flags.security == True)
			&& (free_connection_id_vers3(dev_import_in.connection_id, error) == DS_NOTOK))
			return (DS_NOTOK);

/*
 * Import a device from an old version server.
 */
		clnt_stat = clnt_call (clnt, RPC_DEV_IMPORT,
				    (xdrproc_t)xdr__dev_import_in_3,  (caddr_t) &dev_import_in,
				    (xdrproc_t)xdr__dev_import_out_3, (caddr_t) &dev_import_out, TIMEVAL(timeout));
	}
	if (clnt_stat != RPC_SUCCESS)
	{
/*
 * treat all errors the same i.e. as stateless import - andy 25jun02
 * add "stateless-ness" by ignoring RPC timeouts at import time
 * initialise the devserver struct so that the device can be properly
 * imported next time round.
 */
                dev_notimported_init(device_name,access,i_nethost,ds_ptr,error);
		clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *)&svr_conns[n_svr_conn].rpc_retry_timeout);
		clnt_control (clnt, CLSET_TIMEOUT, (char *)&svr_conns[n_svr_conn].rpc_timeout);

/*
 *  if the first connection failed, close
 *  the socket again.
 */
		if ( svr_conns[n_svr_conn].no_conns == 0)
			clnt_destroy (clnt);
/*
 * treat all rpc errors the same i.e. stateless import - andy 25jun02
 * check to see whether the import failed because of an RPC timeout
 * in which case "fool" the client into thinking the dev_import() was
 * successful.
 */
                *error = 0;
                return (DS_OK);
	}

/*
 *  reinstall current connnection timeout.
 */
	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &svr_conns[n_svr_conn].rpc_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &svr_conns[n_svr_conn].rpc_timeout);

/*
 * allocate space for client's copy of device server information
 */
	if (dev_import_out.status == 0)
	{
		if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
		{
			dev_import_out.status = DS_NOTOK;
			dev_import_out.error  = DevErr_InsufficientMemory;
		}
		else
		{
/*
 * initialise device server structure to zero to make sure there
 * are no strange values lying around - andy 7/5/97
 */
			memset ((char *)*ds_ptr, 0, sizeof (struct _devserver));

/*
 * initialise the administration structure for the RPC connection
 * if this is the first device connected.
 */
			if ( svr_conns[n_svr_conn].no_conns == 0)
			{
				svr_conns[n_svr_conn].clnt        = clnt;
				svr_conns[n_svr_conn].vers_number = vers_number;
				svr_conns[n_svr_conn].tcp_clnt    = clnt;
				svr_conns[n_svr_conn].udp_clnt    = NULL;
				svr_conns[n_svr_conn].asynch_clnt = NULL;
			}
			else
			{
/*
 * If the connection is marked as bad connection
 * and the import has worked, mark it now as a
 * good and working connection.
 */
				if (svr_conns[n_svr_conn].rpc_conn_status == BAD_SVC_CONN)
				{
					svr_conns[n_svr_conn].rpc_conn_status = GOOD_SVC_CONN;
					svr_conns[n_svr_conn].rpc_conn_counter++;
				}

				svr_conns[n_svr_conn].rpc_error_flag  = GOOD_SVC_CONN;
			}

/* 
 * pass the information back to the user
 */
			strncpy((*ds_ptr)->device_name,device_name, sizeof((*ds_ptr)->device_name));
			strncpy((*ds_ptr)->device_class,device_class, sizeof((*ds_ptr)->device_class));
			strncpy((*ds_ptr)->device_type,device_type, sizeof((*ds_ptr)->device_type));
			strncpy((*ds_ptr)->server_name,dev_import_out.server_name, sizeof((*ds_ptr)->server_name));
			strncpy((*ds_ptr)->server_host,host_name, sizeof((*ds_ptr)->server_host));
			(*ds_ptr)->prog_number      = prog_number;
			(*ds_ptr)->vers_number      = vers_number;
			(*ds_ptr)->clnt 	    = clnt;
			(*ds_ptr)->ds_id 	    = dev_import_out.ds_id;
			(*ds_ptr)->no_svr_conn      = n_svr_conn;
			(*ds_ptr)->rpc_conn_counter = svr_conns[n_svr_conn].rpc_conn_counter;
			(*ds_ptr)->dev_access       = access;
			(*ds_ptr)->i_nethost        = i_nethost;
			(*ds_ptr)->rpc_protocol     = D_TCP;
			(*ds_ptr)->asynch_clnt      = NULL;
			(*ds_ptr)->asynch_timeout   = asynch_timeout;
			(*ds_ptr)->pending          = 0;
			(*ds_ptr)->no_database      = no_database;

/*
 *  The connection to the device is correctly established
 *  now increase the number of associated devices to this
 *  RPC connection.
 */
			svr_conns[n_svr_conn].no_conns++;

/*
 * If the device import has worked, create the security
 * key of the client handle.
 */
			if (!no_database && (multi_nethost[i_nethost].config_flags.security == True)
				&& ( create_sec_key ((*ds_ptr), error) == DS_NOTOK ))
						return (DS_NOTOK);
			dev_printdebug (DBG_API, "dev_import() : client handle info:\n"); 
			dev_printdebug (DBG_API, "device_name = %s\n", device_name);
			dev_printdebug (DBG_API, "device_class = %s\n", device_class);
			dev_printdebug (DBG_API, "device_type = %s\n", device_type);
			dev_printdebug (DBG_API, "server_name = %s\n", (*ds_ptr)->server_name);
			dev_printdebug (DBG_API, "host_name   = %s\n", host_name);
			dev_printdebug (DBG_API, "prog_number = %d\n", prog_number);
			dev_printdebug (DBG_API, "vers_number = %d\n", vers_number);
			dev_printdebug (DBG_API, "ds_id       = %d\n", (*ds_ptr)->ds_id);
			dev_printdebug (DBG_API, "no_svr_conn = %d\n", (*ds_ptr)->no_svr_conn);
			dev_printdebug (DBG_API, "rpc_conn_counter = %d\n", (*ds_ptr)->rpc_conn_counter);
		}
	}

/*
 *  if the first connection failed, close
 *  the socket again.
 */
	if (dev_import_out.status == DS_NOTOK && svr_conns[n_svr_conn].no_conns == 0)
		clnt_destroy (clnt);

/*
 * Free the variable arguments in the dev_import_out
 * structure, coming from the server.
 */
	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(dev_import_out.var_argument));

/*
 * return error code and status from device server
 */
	*error = dev_import_out.error;
	return (dev_import_out.status);
}

/**@ingroup syncAPI
 * This function executes a command synchronously on the device associatied with the passed client handle.
 * The device might be remote or local. Input and output data types must correspond to the types specified
 * for this command in te device server's command list. Othewise an error code will be returned. All arguments
 * have to be passed as pointers.
 *
 * Memory for outgoing arguments will be automatically allocated by XDR, if pointers are initilised to @b 
 * NULL. To free the memory allocated by XDR afterwards, the function @ref dev_xdrfree() must be used.
 * 
 * @param ds       	handle to access the device.
 * @param cmd           command to be executed.
 * @param argin  	pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 * @param argout  	pointer for output arguments.
 * @param argout_type 	data type of output arguments.
 * @param error         Will contain an appropriate error code if the corresponding 
 *			call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_putget (devserver ds, long cmd,DevArgument argin,
			  DevType argin_type,DevArgument argout,
			  DevType argout_type, long *error)
{
	_server_data		server_data;
	_client_data		client_data;
	enum clnt_stat		clnt_stat;
	_Int			local_flag;
	long			client_id = 0;
	long			i_nethost;
	nethost_info		*nethost;
	long			status;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_putget() : entering routine\n");
	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
/*
 * make sure dynamic error string points to nothing
 */
	if (dev_error_string != NULL)
	{
		free(dev_error_string);
		dev_error_string = NULL;
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
		status = tango_dev_putget(ds, cmd, argin, argin_type, argout, argout_type, error);
		return(status);
	}
#endif /* TANGO */
/*
 *  check data types
 */
	if (  argin_type < 0 || argout_type < 0)
	{
		*error = DevErr_DevArgTypeNotRecognised;
		return(-1);
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
	if (( local_flag != True ) && (dev_rpc_connection (ds, error)  == DS_NOTOK))
		return (DS_NOTOK);

/*
 * If the security system is configured, 
 * verify the security key
 */
	if (!ds->no_database && (nethost->config_flags.security == True)
		&& ( verify_sec_key (ds, &client_id, error) == DS_NOTOK))
		return (DS_NOTOK);

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

	server_data.var_argument.length   = 0;
	server_data.var_argument.sequence = NULL;

	memset ((char *)&client_data, 0, sizeof (client_data));
	client_data.argout	= (char *) argout;

	dev_printdebug (DBG_API, "dev_putget() : server data -> \n");
	dev_printdebug (DBG_API, "ds_id=%d  cmd=%d  intype=%d  outtype=%d\n",
		    server_data.ds_id, server_data.cmd, server_data.argin_type, server_data.argout_type);

/*
 * Check if the device is a local or a remote device.
 * If local, call the local putget function.
 */
	if ( local_flag == True )
		return dev_putget_local (&server_data, &client_data, error);

/*
 * call RPC_DEV_PUTGET at the correct device server
 *
 * Call a device with the current version number >1.
 */
	if ( ds->vers_number > DEVSERVER_VERS)
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUTGET,
			    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
			    (xdrproc_t)xdr__client_data, (caddr_t) &client_data, TIMEVAL(timeout));
	else
/*
 * Call a device from an old version server.
 */
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUTGET,
			    (xdrproc_t)xdr__server_data_3, (caddr_t) &server_data,
			    (xdrproc_t)xdr__client_data_3,(caddr_t) &client_data, TIMEVAL(timeout));

/*
 * Check for errors on the RPC connection.
 */
	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
		return (DS_NOTOK);

/*
 * check if an  error message was returned if so make a copy of
 * it so th client can retrieve it with a call to dev_error_str()
 */
	if (client_data.var_argument.length == 1)
	{
		dev_error_string = (char*)malloc( strlen(*(char**)client_data.var_argument.sequence[0].argument)+1);
	    	assert(dev_error_string != NULL);
		if(dev_error_string)
			strcpy(dev_error_string,*(char**)client_data.var_argument.sequence[0].argument);
	}

/*
 * Free the variable arguments in the client_data
 * structure, coming from the server. 
 */
	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(client_data.var_argument));

/*
 * return error code and status from device server
 */
	*error = client_data.error;
	return (client_data.status);
}

/**@ingroup syncAPI
 * This function executes a command on the device associated with the passed client handle, without
 * returning any output data. The device might be remote or local. Input data types must correspond
 * to the types specified for this command in the device server's command list. Othewise an error 
 * code will be returned. The output data type in the device server's list must be set to D_VOID_TYPE.
 * All arguments have to be passed as pointers.
 * 
 * @param ds       	handle to access the device.
 * @param cmd           command to be executed.
 * @param argin   	pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 * @param error        	Will contain an appropriate error 
 * 			code if the corresponding call  returns a non-zero value.
 * 
 * Return(s)  :	DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_put (devserver ds, long cmd,DevArgument argin,
		       DevType argin_type, long *error )
{
	_server_data		server_data;
	_client_data		client_data;
	enum clnt_stat		clnt_stat;
	long			status;
	_Int			local_flag;
	long			client_id = 0;
	long			i_nethost;
	nethost_info		*nethost;

	*error = 0;
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_put() : entering routine\n");
	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
/*
 * make sure dynamic error string points to nothing
 */
        if (dev_error_string != NULL)
        {
                free(dev_error_string);
                dev_error_string = NULL;
        }

#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
		status = tango_dev_putget(ds, cmd, argin, argin_type, NULL, D_VOID_TYPE, error);
		return(status);
	}
#endif /* TANGO */
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
	local_flag =(_Int) ds->ds_id >> LOCALACCESS_SHIFT;
	local_flag = local_flag & LOCALACCESS_MASK;

/*
 * Verify the RPC connection if the device is not local.
 */
	if ( (local_flag != True ) && ( dev_rpc_connection (ds, error)  == DS_NOTOK))
		return (DS_NOTOK);

/*
 * If the security system is configured, 
 * verfiy the security key
 */
	if (!ds->no_database && (nethost->config_flags.security == True)
		&& (verify_sec_key (ds, &client_id, error) == DS_NOTOK ))
		return (DS_NOTOK);

/*
 *  fill in data transfer structures server_data
 *  and client_data.
 */
	server_data.ds_id	= ds->ds_id;
	server_data.client_id	= client_id;
	server_data.access_right= ds->dev_access;
	server_data.cmd		= cmd;
	server_data.argin_type	= argin_type;
	server_data.argout_type	= D_VOID_TYPE;
	server_data.argin	= (char *) argin;

	server_data.var_argument.length   = 0;
	server_data.var_argument.sequence = NULL;

	memset ((char *)&client_data, 0, sizeof (client_data));
	client_data.argout	= NULL;

	dev_printdebug (DBG_API, "dev_put() : server data -> \n");
	dev_printdebug (DBG_API, "ds_id=%d  cmd=%d  intype=%d\n", 
			server_data.ds_id, server_data.cmd, server_data.argin_type);

/*
 * Check if the device is a local or a remote device.
 * If local, call the local put function.
 */

	if ( local_flag == True )
		return dev_put_local (&server_data, error);

/*
 *  call RPC_DEV_PUT at the correct device server
 *
 * Call a device with the current version number >1.
 */
	if ( ds->vers_number > DEVSERVER_VERS)
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUT,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_data, (caddr_t) &client_data, TIMEVAL(timeout));
	else
/*
 * Call a device from an old version server.
 */
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUT,
		    (xdrproc_t)xdr__server_data_3, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_data_3, (caddr_t) &client_data, TIMEVAL(timeout));

/*
 * Check for errors on the RPC connection.
 */
	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
		return (DS_NOTOK);
/*
 * check if an  error message was returned if so make a copy of
 * it so th client can retrieve it with a call to dev_error_str()
 */
        if (client_data.var_argument.length == 1)
        {
                dev_error_string = (char*)malloc( strlen(*(char**)client_data.var_argument.sequence[0].argument)+1);
	    	assert(dev_error_string != NULL);
		if (dev_error_string)
                	strcpy(dev_error_string,*(char**)client_data.var_argument.sequence[0].argument);
        }

/*
 * Free the variable arguments in the client_data
 * structure, coming from the server. 
 */
	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(client_data.var_argument));

/*
 * return error code and status from device server
 */
	*error = client_data.error;
	return (client_data.status);
}

/**@ingroup syncAPI
 * This function closes the connection to a device associated with the passed client handle.
 *
 * @param ds	handle to device.
 * @param error Will contain an appropriate error code if the corresponding 
 *		call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_free (devserver ds, long *error)
{
	static _dev_free_out	dev_free_out;
	_dev_free_in		dev_free_in;
	enum clnt_stat		clnt_stat;
	char			*hstring;
	long			status;
	_Int			local_flag;
	long			client_id = 0;
	long			i_nethost;
	nethost_info		*nethost;
/*	struct timeval		zero_timeout = {0,0};*/
	long			iarg;
	DevVarArgument		vararg[10];
	char 			*name;
	char 			*host;

	status = DS_OK;
	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_free() : entering routine\n");
	dev_printdebug (DBG_API, "dev_free() :  ds_id = %d\n", ds->ds_id);

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
		tango_dev_free(ds, error);
		return(DS_OK);
	}
#endif /* TANGO */
/*
 * in a "stateless" import if the device was never imported
 * then simply free the _devserver structure and return
 */
	if (ds->clnt == NULL)
	{
		free (ds);
		return(DS_OK);
	}

/*
 * save the device's nethost in an intermediate variable
 * to make it more accessible
 */
	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];

/*
 * If the security system is configured, 
 * verify the security key
 */
		if ( (nethost->config_flags.security == True)
			&& (verify_sec_key (ds, &client_id, error) == DS_NOTOK))
				return (DS_NOTOK);
	}

/*
 * Fill the input structure.
 */
	dev_free_in.ds_id        = ds->ds_id;
	dev_free_in.client_id    = client_id;
	dev_free_in.access_right = ds->dev_access;

	dev_free_in.var_argument.length   = 0;
	dev_free_in.var_argument.sequence = NULL;

/*
 * Check if the device is a local or a remote device.
 * If local, call the local free function.
 */
	local_flag =(_Int) ds->ds_id >> LOCALACCESS_SHIFT;
	local_flag = local_flag & LOCALACCESS_MASK;
	if ( local_flag == True )
	{
		status = dev_free_local (&dev_free_in, error);

/*
 * Free the security key for the connection.
 */
		if ( nethost->config_flags.security == True )
			free_sec_key (ds);

		free (ds);
		return (status);
	}

/*
 * first check if the connection to the server is OK
 */
	if (svr_conns[ds->no_svr_conn].rpc_conn_status != BAD_SVC_CONN)
	{
/*
 * Make sure the correct client handle is used!
 */
		ds->clnt = svr_conns[ds->no_svr_conn].clnt;

/*
 *  Store the current RPC timeout for the connection and
 *  change the timeout to a short api_timeout.
 */
		clnt_control (ds->clnt, CLGET_RETRY_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_retry_timeout);
		clnt_control (ds->clnt, CLGET_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_timeout);

		clnt_control (ds->clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
		clnt_control (ds->clnt, CLSET_TIMEOUT, (char *) &api_timeout);

/*
 * call RPC_DEV_FREE at the correct device server
 *
 * Free a device with the current version number >1.
 */
		if ( ds->vers_number > DEVSERVER_VERS)
			clnt_stat = clnt_call (ds->clnt, RPC_DEV_FREE,
			    (xdrproc_t)xdr__dev_free_in, (caddr_t) &dev_free_in,
			    (xdrproc_t)xdr__dev_free_out,(caddr_t) &dev_free_out, TIMEVAL(timeout));
		else
/*
 * Free a device from an old version server.
 */
			clnt_stat = clnt_call (ds->clnt, RPC_DEV_FREE,
			    (xdrproc_t)xdr__dev_free_in_3, (caddr_t) &dev_free_in,
			    (xdrproc_t)xdr__dev_free_out_3,(caddr_t) &dev_free_out, TIMEVAL(timeout));

		if (clnt_stat != RPC_SUCCESS)
		{
			if (clnt_stat == RPC_TIMEDOUT)
				*error = DevErr_RPCTimedOut;
			else
				*error = DevErr_RPCFailed;
			status = DS_NOTOK;
		}
		else if (dev_free_out.status == DS_NOTOK)
		{
			*error = dev_free_out.error;
			status = DS_NOTOK;
		}
	}

/*
 * Decrease the counter of single user accesses
 * if a single user access was freed. If it was the last
 * single user access for the connection reinstall the initial
 * protocol.
 *
 * Free the security key for the connection.
 */
	if (!ds->no_database && (nethost->config_flags.security == True))
	{
		sec_free_tcp_connection (ds, &svr_conns[ds->no_svr_conn]);
		free_sec_key (ds);
	}

/*
 *  In any case close the RPC connection to the device.
 *  If the last device on a connection is freed,
 *  destroy the handle.
 */
	if (--(svr_conns[ds->no_svr_conn].no_conns) == 0)
	{
/*
 *  if tcp protocol is used, close the socket first.
 */
		if (svr_conns[ds->no_svr_conn].tcp_clnt != NULL)
		{
#if !defined (WIN32)
			close (svr_conns[ds->no_svr_conn].tcp_socket);
#else
			closesocket (svr_conns[ds->no_svr_conn].tcp_socket);
#endif /* WIN32 */
			clnt_destroy (svr_conns[ds->no_svr_conn].tcp_clnt);
		}
/*
 * if udp protocol has been used then close the UDP client handle
 */
		if (svr_conns[ds->no_svr_conn].udp_clnt != NULL)
			clnt_destroy (svr_conns[ds->no_svr_conn].udp_clnt);
/*
 *  if an asynchronous handle exists then close it (tcp connection)
 */
		if (svr_conns[ds->no_svr_conn].asynch_clnt != NULL)
		{
/*
 * first tag on additional arguments to dev_free_in needed by the server to identify
 * client
 */
                	dev_free_in.var_argument.length = iarg = 0;
                	name = config_flags.server_name;
                	vararg[iarg].argument_type      = D_STRING_TYPE;
                	vararg[iarg].argument           = (DevArgument)&name;
                	dev_free_in.var_argument.length++; iarg++;
                	host = config_flags.server_host;
                	vararg[iarg].argument_type      = D_STRING_TYPE;
                	vararg[iarg].argument           = (DevArgument)&host;
                	dev_free_in.var_argument.length++; iarg++;
                	vararg[iarg].argument_type      = D_ULONG_TYPE;
                	vararg[iarg].argument           = (DevArgument)&config_flags.prog_number;
                	dev_free_in.var_argument.length++; iarg++;
                	vararg[iarg].argument_type      = D_ULONG_TYPE;
                	vararg[iarg].argument           = (DevArgument)&config_flags.vers_number;
                	dev_free_in.var_argument.length++;
                	dev_free_in.var_argument.sequence = vararg;

/*
 * "free" the asynchronous service of the device server by calling
 * the RPC_ASYN_FREE service. this will close the client <--> server
 * connection in a clean manner and ensure that no stale handles
 * are left lying around. 
 *
 * this is a synchronous request i.e. wait for the return status
 * to give time to the server on OS9 to close the client socket connection.
 */
			clnt_control (svr_conns[ds->no_svr_conn].asynch_clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
			clnt_control (svr_conns[ds->no_svr_conn].asynch_clnt, CLSET_TIMEOUT, (char *) &api_timeout);
                	clnt_stat = clnt_call(svr_conns[ds->no_svr_conn].asynch_clnt, RPC_ASYN_FREE,
                           	(xdrproc_t)xdr__dev_free_in, (caddr_t)&dev_free_in,
                           	(xdrproc_t)xdr_void, (caddr_t)NULL,
			TIMEVAL(api_timeout));
			dev_printdebug ( DBG_ASYNCH, "dev_free(): RPC_ASYN_FREE returned clnt_stat=%d\n",clnt_stat);

/*
 * close the 2 asynch tcp sockets (one for client requests and one for server
 * callbacks), this should be done implicitly by clnt_destroy() 
 * but this is not the case for OS9
 */
			errno = 0;
/*
			shutdown(svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket,2);
#if !defined (WIN32)
                	close (svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket);
#else
                	close_socket (svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket);
#endif  * WIN32 * 

DON'T - 26/3/98
			shutdown(svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket,2);
#if !defined (WIN32)
                	close (svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket);
#else
                	close_socket (svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket);
#endif * WIN32 *
			dev_printdebug ( DBG_ASYNCH, "dev_free(): closed client socket out=%d (errno=%d)\n",
				*svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket,
				*svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket,errno);
*/
			clnt_destroy (svr_conns[ds->no_svr_conn].asynch_clnt);
		}
	}
	else
	{
/*
 * Reinstall the current connection timeout.
 */
		clnt_control (ds->clnt, CLSET_RETRY_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_retry_timeout);
		clnt_control (ds->clnt, CLSET_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_timeout);
	}

/*
 * Decrease the counter of single user accesses
 * if a single user access was freed. If it was the last
 * single user access for the connection reinstall the initial
 * protocol.
 *
 * Free the security key for the connection.
 */
	if ( nethost->config_flags.security == True )
	{
		sec_free_tcp_connection (ds, &svr_conns[ds->no_svr_conn]);
		free_sec_key (ds);
	}

/*
 * Free the variable arguments in the dev_free_out
 * structure, coming from the server. 
 */
	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray, (char *)&(dev_free_out.var_argument));

/*
 *  Free the allocated memory for the dserver structure.
 */
	free ( ds );

/*
 * return error code and status from device server
 */
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_free() : leaving routine\n");
	return (status);
}

/**@ingroup clientAPIintern
 * Query already open connections to servers.  If a connection to a server is already
 * established, it will be reused. Otherwise a new connection to a server will be
 * created.
 *
 * @param host       	host name of the device server
 * @param prog_number 	program number of the device server
 * @param vers_number	vers number of the device server
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_query_svr (char* host, long prog_number, long vers_number)
{
	static int no_svr_conn = TRUE;
	int 	   next_conn   = -1;
	int 	   i;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_query_svr() : entering routine\n");

/*
 * first time initialise all the possible server connections
 */
	if (no_svr_conn == TRUE)
	{
		for (i = 0; i < NFILE; i++)
			svr_conns[i].no_conns = 0;
		no_svr_conn = FALSE;
		next_conn   = 0;

		strncpy(svr_conns[next_conn].server_host,host, sizeof(svr_conns[next_conn].server_host));
		svr_conns[next_conn].prog_number 	   = prog_number;
		svr_conns[next_conn].vers_number 	   = vers_number;
		svr_conns[next_conn].rpc_conn_status    = GOOD_SVC_CONN;
		svr_conns[next_conn].rpc_error_flag     = GOOD_SVC_CONN;
		svr_conns[next_conn].rpc_conn_counter   = 0;
		svr_conns[next_conn].rpc_protocol       = D_TCP;
		svr_conns[next_conn].first_access_time  = False;
		svr_conns[next_conn].open_si_connections    = 0;
		svr_conns[next_conn].rpc_protocol_before_si = D_TCP;

		return(next_conn);
	}

/*
 * search amongst list of existing connections for server matching this host and port
 */
	for (i = 0; i < NFILE; i++)
	{
		if (svr_conns[i].no_conns == 0)
		{
			if (next_conn < 0) 
				next_conn = i;
		}
		else
		{
			if ((strcmp(svr_conns[i].server_host,host) == 0) &&
			    (svr_conns[i].prog_number == prog_number))
			{
/*
 * client already has a connection to this server
 */
				next_conn = i;
				return (next_conn);
			}
		}
	}

/*
 * server not found amongst already connected servers.
 * is there still place for another server connection ?
 */
	if (next_conn < 0)
		return (next_conn);

/*
 * yes, initialise the server table
 */
	strncpy(svr_conns[next_conn].server_host,host, sizeof(svr_conns[next_conn].server_host));
	svr_conns[next_conn].prog_number 	= prog_number;
	svr_conns[next_conn].vers_number 	= vers_number;
	svr_conns[next_conn].rpc_conn_status    = GOOD_SVC_CONN;
	svr_conns[next_conn].rpc_error_flag     = GOOD_SVC_CONN;
	svr_conns[next_conn].rpc_conn_counter   = 0;
	svr_conns[next_conn].rpc_protocol       = D_TCP;
	svr_conns[next_conn].first_access_time  = False;

	return(next_conn);
}


/**@ingroup syncAPI
 * This function frees the memory for device server data allocated by XDR. An example for the user
 * of this function is the freeing of a D_VAR_FLOATARR data type. Using this function you don't have
 * to care about the length of the internal sequence of float values. Just pass a pointer to a 
 * D_VAR_FLOATARR structure and the allocated memory for the sequence will be freed, according to the
 * length specified in the structure.
 *
 * This function offers you an interface to use xdr_free() for the implented device server types.
 *
 * Using free() might cause problems, because internal data of the rpc package will not be freed.
 *
 * @param type   device server data type to be freed.
 * @param objptr pointer to a structure of type.
 * @param error  Will contain an appropriate error code if the
 *		 corresponding call returns a non-zero value.
 *
 * @return  DS_OK or DS_NOTOK
 */
long _DLLFunc dev_xdrfree (DevType type, DevArgument objptr, long *error)
{
	DevDataListEntry        data_type;

	*error = 0;
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_xdrfree() : entering routine\n");

/*
 * Get the XDR data type from the loaded type list
 */
	if ( xdr_get_type (type, &data_type, error) == DS_NOTOK)
	{
		dev_printdebug (DBG_ERROR | DBG_API, "\ndev_xdrfree() : xdr_get_type(%d) returned error %d\n", type, *error);
		return (DS_NOTOK);
	}

/*
 *  Call xdr_free() to free all, by the rpc-package, allocated memory
 *  for the data type.
 */
	xdr_free ( (xdrproc_t)data_type.xdr, (char *)objptr );

	return (DS_OK);
}

/**@ingroup clientAPIintern 
 * Checks the RPC connection to a server.  If a tcp connection was detected as lost or 
 * a udp connection has shown three timeouts in a row, the function tries to recreate the
 * RPC handle and to reimport the current device.  If one of the two steps fails, the 
 * connection to the server is marked as a bad connection.
 * 
 * @param ds 	handle to device.
 * @param error Will contain an appropriate error code if the
 * 		corresponding call returns a non-zero value.
 * 
 * Return(s)  :	DS_OK or DS_NOTOK
 */
long _DLLFunc check_rpc_connection (devserver ds, long *error)
{
	CLIENT				*clnt = NULL;
	enum clnt_stat		clnt_stat;
	Db_devinf_imp		devinfo;
	devserver			new_ds = NULL;
	struct sockaddr_in	serv_adr;
#if !defined vxworks
	struct hostent		*ht;
#else  /* !vxworks */
	int			host_addr;
#endif /* !vxworks */
	int			tcp_socket;
	char			*device_name;
	char			host_name[SHORT_NAME_SIZE];
	char			*hstring;
	long			prog_number;
	long			vers_number;
	time_t			access_time;
	long			i_nethost;
	nethost_info		*nethost;
	long			no_conns;
	long			no_svr_conn;
	long			import_status;

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_API, "\ncheck_rpc_connection() : entering routine\n");

#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
		return(DS_OK);
#endif /* TANGO */

	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];
	}

/*
 * check whether the last three RPC calls on this connection
 * have shown consequetive errors. If yes try to reopen
 * the connection.
 */
	if ( svr_conns[ds->no_svr_conn].rpc_conn_status != BAD_SVC_CONN )
	{
		if ( svr_conns[ds->no_svr_conn].rpc_error_flag < BAD_SVC_CONN )
		{
			svr_conns[ds->no_svr_conn].rpc_error_flag = 
			    svr_conns[ds->no_svr_conn].rpc_error_flag + RPC_ERROR;
			return (DS_OK);
		}
		else
			svr_conns[ds->no_svr_conn].rpc_conn_status = BAD_SVC_CONN;
	}

/*
 * Check the delay time for VME access.
 */
	if ( svr_conns[ds->no_svr_conn].first_access_time != False )
	{
		time (&access_time);
		if ( (access_time - svr_conns[ds->no_svr_conn].first_access_time) < DELAY_TIME )
		{
			*error = DevErr_BadServerConnection;
			return (DS_NOTOK);
		}
	}

/* 
 *  Try to create a new rpc connection.
 */
	device_name = ds->device_name;

/*
 * check if device is being served by a database
 */
	if (!ds->no_database)
	{
		if ( db_dev_import (&device_name, &devinfo, 1, error) == DS_OK )
		{
			strncpy (host_name, devinfo[0].host_name, sizeof(host_name));
			prog_number  = devinfo[0].pn;
			vers_number  = API_VERSION;

/*
 * now free the devinfo structure allocated by db_dev_import().
 */
			free (devinfo);
		}
		else
		{
			free (devinfo);
			*error = DevErr_BadServerConnection;
			return (DS_NOTOK);
		}
	}
	else
	{
/*
 * if no database then assume it will be restarted on the same host and
 * with the same program number
 */
		strncpy(host_name, ds->server_host, sizeof(host_name));
		prog_number = ds->prog_number;
		vers_number = ds->vers_number;
	}

/*
 * Before a new handle can be created, verify whether it is possible to connect to the remote host.
 */
	if ( rpc_check_host ( host_name, error ) == DS_NOTOK )
		return (DS_NOTOK);

/*
 * Attention, Attention
 * Here comes a delay timing to let the VME reboot in peace when the network is alread answering.
 */
	if ( svr_conns[ds->no_svr_conn].first_access_time == False )
	{
		time ( &(svr_conns[ds->no_svr_conn].first_access_time) );
		*error = DevErr_BadServerConnection;
		return (DS_NOTOK);
	}

/*
 * Store the current RPC timeout off the old connection. 
 * reopen a connection with the same protocol than before.
 */
	if ( svr_conns[ds->no_svr_conn].rpc_protocol == D_UDP )
	{
		clnt = clnt_create ( host_name, prog_number, vers_number, "udp");

/*
 * This part was added for compatibility reasons with the old libray version 3.
 * If the server is not version 4, the version number must be set to 1.
 * Even if the last library version was 3, because 1 indicates the RPC service version.
 *
 * To make the version check the null procedure of the service is called. A version 
 * mismatch will be returned, if the service runs version 3 software.
 * The RPC version number will be set to one in this case.
 */
		if (clnt != NULL)
		{
			clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
			clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

			clnt_stat = clnt_call (clnt, NULLPROC,
				    (xdrproc_t)xdr_void,  NULL,
				    (xdrproc_t)xdr_void,  NULL, TIMEVAL(timeout));
			if (clnt_stat != RPC_SUCCESS)
			{
				if ( clnt_stat == RPC_PROGVERSMISMATCH )
				{
/*
 * Destroy version 4 handle.
 */
					clnt_destroy (clnt);

/*
 * Set version number to 1 and recreate the client handle.
 */
					vers_number = DEVSERVER_VERS;
					clnt = clnt_create (host_name,prog_number, vers_number,"udp");
				}
			}
		}
	}
	else
	{
#if !defined vxworks
		ht = gethostbyname(host_name);
		memcpy ( (char *)&serv_adr.sin_addr, ht->h_addr, (size_t) ht->h_length );
#else  /* !vxworks */
		host_addr = hostGetByName(host_name);
		memcpy ( (char*)&serv_adr.sin_addr, (char*)&host_addr, 4);
#endif /* !vxworks */
		serv_adr.sin_family = AF_INET;
		serv_adr.sin_port = 0;
		tcp_socket = RPC_ANYSOCK;

		clnt = clnttcp_create ( &serv_adr, prog_number, vers_number, &tcp_socket, 0, 0);
/*
 * This part was added for compatibility reasons with the old libray version 3.
 * If the server is not version 4, the version number must be set to 1.
 * Even if the last library version was 3, because 1 indicates the RPC service version.
 *
 * To make the version check the null procedure of the service is called. A version mismatch will
 * be returned, if the service runs version 3 software. The RPC version number will be set to one 
 * in this case.
 */
		if (clnt != NULL)
		{
			clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
			clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

			clnt_stat = clnt_call (clnt, NULLPROC, (xdrproc_t)xdr_void,  NULL,
				   (xdrproc_t)xdr_void,  NULL, TIMEVAL(timeout));
			if (clnt_stat != RPC_SUCCESS)
			{
				if ( clnt_stat == RPC_PROGVERSMISMATCH )
				{
/*
 * Destroy version 4 handle.
 */
					clnt_destroy (clnt);

/*
 * Set version number to 1 and recreate the client handle.
 */
#if !defined (WIN32)
                                        close (tcp_socket);
#else
                                        closesocket (tcp_socket);
#endif /* WIN32 */
					tcp_socket = RPC_ANYSOCK;

					vers_number = DEVSERVER_VERS;
					clnt = clnttcp_create ( &serv_adr, prog_number, 
						    vers_number, &tcp_socket, 0, 0);
				}
			}
		}
	}


	if (clnt != NULL) 
	{
/*
 * new client handle created to server - destroy old (stale) client handles
 */
		if (svr_conns[ds->no_svr_conn].udp_clnt != NULL)
		{
			clnt_destroy(svr_conns[ds->no_svr_conn].udp_clnt);
			svr_conns[ds->no_svr_conn].udp_clnt = NULL;
		}
		if (svr_conns[ds->no_svr_conn].tcp_clnt != NULL)
		{
/*
 *  if tcp protocol was used, close the old socket first.
 */
#if !defined (WIN32)
               		close (svr_conns[ds->no_svr_conn].tcp_socket);
#else
              		closesocket (svr_conns[ds->no_svr_conn].tcp_socket);
#endif /* WIN32 */

	 		clnt_destroy(svr_conns[ds->no_svr_conn].tcp_clnt);
			svr_conns[ds->no_svr_conn].tcp_clnt = NULL;
               		svr_conns[ds->no_svr_conn].tcp_socket = tcp_socket;
	  	}
/*
 * if an asynchronous handle exists then close it (tcp connection) it will be recreated at the 
 * next asynchronous request by the asynch_server_import() function
 */
		if (svr_conns[ds->no_svr_conn].asynch_clnt != NULL)
		{
/*
#if !defined (WIN32)
               		close (svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket);
               		close (svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket);
#else
              		closesocket (svr_conns[ds->no_svr_conn].asynch_listen_tcp_socket);
              		closesocket (svr_conns[ds->no_svr_conn].asynch_callback_tcp_socket);
#endif * WIN32 *
 */
			clnt_destroy (svr_conns[ds->no_svr_conn].asynch_clnt);
			svr_conns[ds->no_svr_conn].asynch_clnt = NULL;
		}
		svr_conns[ds->no_svr_conn].clnt = clnt;
		if (svr_conns[ds->no_svr_conn].rpc_protocol == D_UDP)
		  	svr_conns[ds->no_svr_conn].udp_clnt = clnt;
		else
		    	svr_conns[ds->no_svr_conn].tcp_clnt = clnt;
		strncpy (svr_conns[ds->no_svr_conn].server_host, host_name, sizeof(svr_conns[ds->no_svr_conn].server_host));
/*
 * update program and version number because they could have changed and increment the connection 
 * counter to force a reimport of the device (in dev_rpc_connection())
 */
		svr_conns[ds->no_svr_conn].prog_number = prog_number;
		svr_conns[ds->no_svr_conn].vers_number = vers_number;

/*
 * only return DS_OK if NULL procedure answered with RPC_SUCCESS this means it is OK to reimport the device
 */
		if (clnt_stat != RPC_SUCCESS)
		{
			*error = DevErr_BadServerConnection;
			return(DS_NOTOK);
		}
/*
 * mark this connection as god again and increment the counter of no. of reconnects
 */
		svr_conns[ds->no_svr_conn].rpc_conn_status = GOOD_SVC_CONN;
		svr_conns[ds->no_svr_conn].rpc_error_flag = GOOD_SVC_CONN;
		svr_conns[ds->no_svr_conn].rpc_conn_counter++;
/*
 *  Initialise the current RPC timeout to the new connection. 
 */
		clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_retry_timeout);
		clnt_control (clnt, CLSET_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_timeout);

/*
 * return without error, a call to dev_import() will be forced in dev_rpc_connection() 
 * andy 10feb99
 */
		return(DS_OK);
/*
 * try to import the device to see whether the server is realy ready and waiting and not 
 * only registered to the portmapper.
 *
 * keep track of the no. of the connection and the no. of connections we will need this to 
 * test if this is a new connection to a server or the old one 
 * andy 29jan99
 */
		no_conns = svr_conns[ds->no_svr_conn].no_conns;
		no_svr_conn = ds->no_svr_conn;
/*
 * if this is a no database device then tag the program number on so that the dev_import() can detect this
 */
		if (( import_status = dev_import ( ds->device_name,  ds->dev_access, &new_ds, error ) == DS_OK ))
		{
/*
 * dev_import returns DS_OK in three cases, these are :
 *
 * case (1) stateless import i.e. device didn't really respond therefore
 *          destroy client handle, free new_ds and return an error
 */
			if (new_ds->clnt == NULL)
			{
				if (svr_conns[ds->no_svr_conn].rpc_protocol == D_TCP)
				{
#if !defined (WIN32)
					close(tcp_socket);
#else
					closesocket(tcp_socket);
#endif /* WIN32 */
				}
				clnt_destroy(clnt);
				free(new_ds);
				*error = DevErr_BadServerConnection;
				return(DS_NOTOK);
			}
/*
 * case (2) import worked using client handle created by check_rpc_connection() i.e this routine
 */
			if (no_svr_conn == new_ds->no_svr_conn)
			{
/*
 * only substract old connection from the list. if dev_import really worked using old  client handle
 */
				svr_conns[ds->no_svr_conn].no_conns--;
/*
 * make a copy of the client handle for future use e.g. when changing protocol
 */
			 	if (svr_conns[ds->no_svr_conn].rpc_protocol == D_UDP)
			 	{
			    		svr_conns[ds->no_svr_conn].udp_clnt = clnt;
			     		svr_conns[ds->no_svr_conn].tcp_clnt = NULL;
			 	}
			 	else
			 	{
			    		svr_conns[ds->no_svr_conn].udp_clnt = NULL;
			     		svr_conns[ds->no_svr_conn].tcp_clnt = clnt;
		  		}

			}
/*
 * case (3) import worked using a new client handle it created, destroy the
 *          handle created by check_rpc_connection() therefore
 */
			if (no_svr_conn != new_ds->no_svr_conn)
			{
				if (svr_conns[no_svr_conn].rpc_protocol == D_TCP)
				{
#if !defined (WIN32)
					close(tcp_socket);
#else
					closesocket(tcp_socket);
#endif /* WIN32 */
				}
				clnt_destroy(clnt);
				
				svr_conns[no_svr_conn].no_conns--;
/*
 * set protocol of new device server to protocol of old ds handle
 */
				dev_rpc_protocol (new_ds,svr_conns[no_svr_conn].rpc_protocol,error);
/*
 * set timeout of new device server to timeout of old ds handle
 */
				if ((ds->rpc_timeout.tv_sec != 0) || (ds->rpc_timeout.tv_usec != 0))
				{
					dev_rpc_timeout(new_ds, CLSET_TIMEOUT, &(ds->rpc_timeout), error);
				}
			}

/*
 * Decrease the counter of single user accesses if a single user access was freed. 
 * If it was the last single user access for the connection reinstall the initial protocol.
 *
 * Free the security key for the connection.
 */
			if (!ds->no_database && ( nethost->config_flags.security == True))
		    	{
		       		sec_free_tcp_connection (ds, &svr_conns[ds->no_svr_conn]);
		       		free_sec_key (ds);
			}

/* 
 * pass the information back to the user
 */
		    	strncpy(ds->device_name,new_ds->device_name, sizeof(ds->device_name));
			strncpy(ds->device_class,new_ds->device_class, sizeof(ds->device_class));
			strncpy(ds->device_type,new_ds->device_type, sizeof(ds->device_type));
			strncpy(ds->server_name,new_ds->server_name, sizeof(ds->server_name));
			strncpy(ds->server_host,new_ds->server_host, sizeof(ds->server_host));
			ds->prog_number      = new_ds->prog_number;
			ds->vers_number      = new_ds->vers_number;
			ds->clnt 	     = new_ds->clnt;
			ds->ds_id 	     = new_ds->ds_id;
			ds->no_svr_conn      = new_ds->no_svr_conn;
			ds->rpc_conn_counter = new_ds->rpc_conn_counter;
			ds->dev_access       = new_ds->dev_access;

/*
 *  Free new device handle created by dev_import().
 */
			free (new_ds);
			svr_conns[ds->no_svr_conn].first_access_time = False;
/*	
 * relisten  previuosly  registered events,the server has to know, we are there
 */
			relisten_events(ds);
			return (DS_OK);
		}
		else
		{
/*
 * if the dev_import failed, destroy the created client handle and close the socket.
 */
/*			svr_conns[ds->no_svr_conn].clnt = ds->clnt;*/

/*
 * if tcp protocol is used, close the old socket first.
 */
			if (svr_conns[ds->no_svr_conn].rpc_protocol == D_TCP)
			{
#if !defined (WIN32)
                               close (tcp_socket);
#else
                               closesocket (tcp_socket);
#endif /* WIN32 */
			}

			clnt_destroy (clnt);
		}
	}
/*
	else
	{
		hstring = clnt_spcreateerror ("check_rpc_connection");
		dev_printerror (SEND,hstring);
	}
*/
	*error = DevErr_BadServerConnection;
	return (DS_NOTOK);
}

/**@ingroup clientAPIintern
 * The function will reimport the device, and reinitialise the connection parameters, if
 * the RPC handle to the server was detected lost and recreated.
 * 
 * @param ds 	handle to device.
 * @param error Will contain an appropriate error code if the
 * 		corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc reinstall_rpc_connection (devserver ds, long *error)
{
	devserver	new_ds;
	long		h_error;
	long		i_nethost;
	nethost_info	*nethost;
	long		status;
	char        	device_name[LONG_NAME_SIZE];

	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_API, "\nreinstall_rpc_connection() : entering routine\n");

#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
		return(DS_OK);
#endif /* TANGO */

/*	ds->clnt = svr_conns[ds->no_svr_conn].clnt; not needed by stateless anymore ?*/

	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];
	}

	if ( dev_import ( ds->device_name, ds->dev_access, &new_ds, error ) == DS_NOTOK )
	{
		if ( check_rpc_connection (ds, &h_error) == DS_NOTOK )
			*error = h_error;
		return (DS_NOTOK);
	}
/*
 * the "stateless" dev_import() could not import the device, return an error
 * which indicates this and free the new_ds structure at the same time
 */
        if (new_ds->clnt == NULL)
        {
                free (new_ds);
                *error = DevErr_DeviceNotImportedYet;
                return(DS_NOTOK);
        }

/*
 * in the "stateless" case where the device was not imported previously, do not free the security keys
 */
	if (ds->clnt != NULL)
	{
        	ds->clnt = svr_conns[ds->no_svr_conn].clnt;
        	svr_conns[ds->no_svr_conn].no_conns--;

/*
 * Decrease the counter of single user accesses if a single user access was freed. If it was the last
 * single user access for the connection reinstall the initial protocol.
 *
 * Free the security key for the connection.
 */
		if (!ds->no_database && (nethost->config_flags.security == True))
		{
			sec_free_tcp_connection (ds, &svr_conns[ds->no_svr_conn]);
			free_sec_key (ds);
		}
	}
	else
	{
/*
 * in the stateless import case check if the client has requested the rpc protocol or timeout to be changed 
 * in which case modify the client handle accordingly.
 */
		if (ds->rpc_protocol != 0)
		{
			status = dev_rpc_protocol(new_ds, ds->rpc_protocol, error);
			if (status != DS_OK)
			{
				dev_printerror(SEND, "reinstall_rpc_connection(): problems changing protocol to %d (error=%d)\n",
			  	         ds->rpc_protocol,*error);
				dev_printerror_no(SEND,NULL,*error);
			}
		}

		if ((ds->rpc_timeout.tv_sec != 0) || (ds->rpc_timeout.tv_usec != 0))
		{
			status = dev_rpc_timeout(new_ds, CLSET_TIMEOUT, &ds->rpc_timeout, error);
			if (status != DS_OK)
			{
				dev_printerror(SEND, "reinstall_rpc_connection(): problems changing timeout to %d.d (error=%d)\n",
			  	         ds->rpc_timeout.tv_sec,ds->rpc_timeout.tv_usec,*error);
				dev_printerror_no(SEND,NULL,*error);
			}
		}
	}

/* 
 * pass the information back to the user
 */
	strncpy(ds->device_name,new_ds->device_name, sizeof(ds->device_name));
	strncpy(ds->device_class,new_ds->device_class, sizeof(ds->device_class));
	strncpy(ds->device_type,new_ds->device_type, sizeof(ds->device_type));
	strncpy(ds->server_name,new_ds->server_name, sizeof(ds->server_name));
	strncpy(ds->server_host,new_ds->server_host, sizeof(ds->server_host));
	ds->prog_number      = new_ds->prog_number;
	ds->vers_number      = new_ds->vers_number;
	ds->clnt 	     = new_ds->clnt;
	ds->ds_id 	     = new_ds->ds_id;
	ds->no_svr_conn      = new_ds->no_svr_conn;
	ds->rpc_conn_counter = new_ds->rpc_conn_counter;
	ds->dev_access       = new_ds->dev_access;

/* 
 * relisten to events
 */
	relisten_events(ds);

/*
 *  Free new new device handle created by dev_import().
 */
	free (new_ds);

	return (DS_OK);
}


/**@ingroup clientAPIintern
 * Checks whether the remote host is responding or not. Opens a TCP connection to the 
 * portmapper and tests the write access on the socket.
 * 
 * The VXWORKS version simply pings host to check if alive.
 *
 * @param host_name 	name of the remote host.
 * @param error  	Will contain an appropriate error code if the
 * 			corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc rpc_check_host (char *host_name, long *error)
{
#if !defined vxworks
#if ( (! OSK) && (! _OSK))
	int 			s;   /* connected socket descriptor */
	struct hostent 		*hp; /* pointer to host info for remote host */
	struct servent 		*sp; /* pointer to service information */
	struct sockaddr_in 	peeraddr_in; /* for peer socket address */
	static int		portmap_port=-1; /* portmapper port */
	int 			nb;
	int 			nfds;
	int 			nfd;
	struct timeval		my_timeout;
#ifndef linux
        struct fd_set           writemask;
#else
        fd_set           	writemask;
#endif /* !linux */
#endif /* (! OSK) && (! _OSK) */

	*error = 0;
#ifdef LABVIEW
/* 
 * LabView has a problem with the select() call, therefore return(DS_OK)
 */
	return(DS_OK);
#endif /* LABVIEW */

#if ( (! OSK) && (! _OSK) )
/* 
 * clear out address structures 
 */
	memset ((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));

/* 
 * Set up the peer address to which we will connect. 
 */
	peeraddr_in.sin_family = AF_INET;
/* 
 * Get the host information for the hostname that the user passed in.
 */
	hp = gethostbyname (host_name);
	if (hp == NULL)
	{
		dev_printerror (SEND, "rpc_check_host: %s not found in /etc/hosts", host_name );
		*error = DevErr_CannotConnectToHost;
		return (DS_NOTOK);
	}

	peeraddr_in.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
/* 
 * Find the information for the "example" server in order to get the needed port number.
 *
 * Do this only once to prevent Purify complaining that there is a memory leak for HP-UX 
 * andy 15sep98
 */
	if (portmap_port == -1)
	{
/*
 * in order to reduce the network traffic to the NIS server generated by "getservbyname()" suppressed 
 * this call and hardcoded the port no. of the portmapper (which should be 111 for udp+tcp all over)
 *
 *#if ( (defined sun) || (defined linux) || (defined irix))
 *		sp = getservbyname ("sunrpc", "tcp");
 *#else
 *		sp = getservbyname ("portmap", "tcp");
 *#endif * sun *
 *		if (sp == NULL)
 *		{
 *			dev_printerror (SEND, 
 *		    	"rpc_check_host: portmap not found in /etc/services" );
 *			*error = DevErr_CannotConnectToHost;
 *			return (DS_NOTOK);
 *		}
 *		portmap_port = sp->s_port;
 */
		portmap_port = 111;
	}
	peeraddr_in.sin_port = portmap_port;

/* 
 * Create the socket. 
 */
	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s == DS_NOTOK)
	{
		dev_printerror (SEND, "%s", "rpc_check_host: Unable to open tcp socket" );
		*error = DevErr_CannotConnectToHost;
		return (DS_NOTOK);
	}

/*
 * Set the socket into noblocking mode. 
 */
	nb =1;

#if defined (sun) || defined (lynx) || defined (linux) || defined (irix) || defined(FreeBSD)
	ioctl (s, FIONBIO, &nb);

#elif defined (WIN32)
	ioctlsocket (s, FIONBIO, (u_long *)&nb);
#else
	ioctl (s, FIOSNBIO, &nb);
#endif /* sun */

/*
 * try to connect to the remote host.
 */
#if defined (WIN32) || defined (linux) || defined (_solaris) || defined (_XOPEN_SOURCE_EXTENDED) || defined(FreeBSD)
	connect (s,(struct sockaddr *)&peeraddr_in, sizeof(struct sockaddr_in));
#else
	connect (s, (void *) &peeraddr_in, sizeof(struct sockaddr_in));
#endif /* WIN32 || linux || _solaris */

/*
 *  select the socket and wait until the connection is ready for write access. 
 */
	FD_ZERO ((_LPfd_set)&writemask);
	FD_SET  (s, (_LPfd_set)&writemask);
	nfds = s+1;

/*
 * make a local copy of the timeout structure because Linux will update
 * this value on return with the time remaining to wait  
 * andy 19nov98
 */
	my_timeout = check_host_timeout;
#if defined (__hpux)
#if defined (__STDCPP__) && !defined (_GNU_PP)
	nfd = select (nfds, NULL, &writemask, NULL, &my_timeout);
#else
	nfd = select (nfds, NULL, (int *)&writemask, NULL, &my_timeout);
#endif /* __hpux10 */
#else
#if defined (linux)
	nfd = select (nfds, NULL, &writemask, NULL, &my_timeout);
#else
	nfd = select (nfds, NULL, (_LPfd_set)&writemask, NULL, &my_timeout);
#endif /* linux */
#endif /* __hpux */

	if (nfd == DS_NOTOK)
	{
		dev_printerror (SEND, "%s", "rpc_check_host: select on socket descriptor failed" );
		*error = DevErr_CannotConnectToHost;
#if !defined (WIN32)
                close (s);
#else
                closesocket (s);
#endif /* WIN32 */
		return (DS_NOTOK);
	}

/*
 * check whether the select call was timed out
 */
	if ( FD_ISSET (s, &writemask) == FALSE)
	{
		*error = DevErr_CannotConnectToHost;
#if !defined (WIN32)
                close (s);
#else
                closesocket (s);
#endif /* WIN32 */
		return (DS_NOTOK);
	}

#if !defined (WIN32)
	close (s);
#else
	closesocket(s);
#endif /* WIN32 */
#endif /* (! OSK) && (! _OSK) */

	return (DS_OK);
#else  /* !vxworks */
	STATUS status;

	status = ping(host_name, 1, PING_OPT_SILENT);

	if (status != OK)
	{
                dev_printerror (SEND, "rpc_check_host: ping to host failed" );
                *error = DevErr_CannotConnectToHost;
		return(DS_NOTOK);
	}
	return(DS_OK);
#endif /* !vxworks */
}

/**@ingroup clientAPIintern
 * Import a local device, without using an RPC.
 *
 * @param dev_import_in 	
 * @param ds_ptr 	returns a handle to access the device
 * @param error        	Will contain an appropriate error 
 *			code if the corresponding call 
 *			returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
static long dev_import_local (_dev_import_in  *dev_import_in, devserver  *ds_ptr, long* error)
{
	_dev_import_out		*dev_import_out;
	long			ds_id;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_import_local() : entering routine\n");

	*error = 0;

	dev_import_out = rpc_dev_import_4 (dev_import_in, NULL);

	if (dev_import_out->status == DS_OK)
	{
		if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
		{
			dev_import_out->status = DS_NOTOK;
			dev_import_out->error  = DevErr_InsufficientMemory;
		}
		else
		{
			memset((char *)*ds_ptr, 0, sizeof(struct _devserver));
/*
 * Mark in the device id that the device is local!!
 */
			ds_id = (((unsigned long)True) << LOCALACCESS_SHIFT) + dev_import_out->ds_id;

/*
 * pass the information back to the user
 */
			strncpy((*ds_ptr)->device_name,dev_import_in->device_name, sizeof((*ds_ptr)->device_name));
			strncpy((*ds_ptr)->server_name,dev_import_out->server_name, sizeof((*ds_ptr)->server_name));
			(*ds_ptr)->dev_access       = dev_import_in->access_right;
			(*ds_ptr)->clnt             = NULL;
			(*ds_ptr)->ds_id            = ds_id;
			(*ds_ptr)->no_svr_conn      = 0;
			(*ds_ptr)->rpc_conn_counter = 0;
			(*ds_ptr)->i_nethost 	    = 0;
		}
	}

	dev_printdebug (DBG_TRACE | DBG_API, "dev_import_local() : leaving routine\n");

	*error = dev_import_out->error;
	return (dev_import_out->status);
}


/**@ingroup clientAPIintern
 * Free a local device, without using an RPC.
 * 
 * @param dev_free_in 	handle to access the device.
 * @param error Will contain an appropriate error 
 * 		code if the corresponding call 
 * 		returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */ 
static long dev_free_local (_dev_free_in  *dev_free_in, long* error)
{
	_dev_free_out		*dev_free_out;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_free_local() : entering routine\n");

	*error = 0;
	dev_free_out = rpc_dev_free_4 ( dev_free_in );

	dev_printdebug (DBG_TRACE | DBG_API, "dev_free_local() : leaving routine\n");

	*error = dev_free_out->error;
	return (dev_free_out->status);
}

/**@ingroup clientAPIintern
 * Execute a command on a local device, without using an RPC.
 * Will not return any output arguments.
 * 
 * @param server_data	Input data structure.
 * @param error		Will contain an appropriate error 
 * 			code if the corresponding call 
 * 			returns a non-zero value.
 * 
 * Return(s)  :	DS_OK or DS_NOTOK
 */ 
static long dev_put_local (_server_data  *server_data, long* error)
{
	_client_data	*client_data;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_put_local() : entering routine\n");

	*error = 0;

	client_data = rpc_dev_put_4 (server_data);

	dev_printdebug (DBG_TRACE | DBG_API, "dev_put_local() : leaving routine\n");

	*error = client_data->error;
	return (client_data->status);
}

/**@ingroup clientAPIintern
 * Execute a command on a local device, without using an RPC.
 *
 * @param server_data 	Input data structure.
 * @param client_data 	Output data structure.
 * @param error       	Will contain an appropriate error 
 *			code if the corresponding call 
 *			returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
static long dev_putget_local (_server_data  *server_data,
			      _client_data  *client_data, long* error)
{
	_client_data	*ret_data;
	DevDataListEntry type_info;
	XDR 		xdrs_en;
	XDR 		xdrs_de;
	char* 	buf;
	long 		buf_size;
	long		status;


	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_putget_local() : entering routine\n");

	*error = 0;

/*
 * Call the entry point for local putget calls.  The function rpc_dev_putget_4 can not be used,
 * to avoid pointer clashes in the static return structure!!!
 */
	ret_data = rpc_dev_putget_local (server_data);

	if ( ret_data->status == DS_OK )
	{
/*
 * Encode the outgoing arguments into XDR format.  Decode the arguments afterwards, to get the 
 * same memory allocation by XDR as the dev_putget() working with RPCs.
 */
/*
 * Get the XDR data type.
 */
		if (xdr_get_type (ret_data->argout_type, &type_info, error) == DS_NOTOK)
		{
			return (DS_NOTOK);
		}

/*
 * Calculate the size of the buffer needed for the outgoing arguments.
 */
		if (type_info.xdr_length != NULL)
		{
			buf_size = type_info.xdr_length(ret_data->argout);
			if ( buf_size == (_Int)DS_NOTOK)
			{
				*error = DevErr_XDRLengthCalculationFailed;
				return (DS_NOTOK);
			}
			buf_size = buf_size + 32;
		}
		else
		{
/* 
 * If no length calculation is implemented, allocate 8KB as for RPC/UDP.
 */
			buf_size = 8000;
		}

/*
 * Allocate the data buffer.
 */
		if ( (buf = (char *)malloc(buf_size)) == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}

/*
 * Create XDR handles and serialize and deserialize the arguments. This is done to have the 
 * same memory allocation as dev_putget() using RPCs.
 */
#ifdef _UCC
		xdrmem_create(&xdrs_en, (caddr_t)buf, (_Int)buf_size, XDR_ENCODE);
		xdrmem_create(&xdrs_de, (caddr_t)buf, (_Int)buf_size, XDR_DECODE);
#else
		xdrmem_create(&xdrs_en, buf, (_Int)buf_size, XDR_ENCODE);
		xdrmem_create(&xdrs_de, buf, (_Int)buf_size, XDR_DECODE);
#endif /* _UCC */

		if (!xdr_pointer(&xdrs_en, (char **)&(ret_data->argout),
			 (_Int)type_info.size,
			 (xdrproc_t)type_info.xdr))
		{
			*error = DevErr_CannotDecodeArguments;
			return (DS_NOTOK);
		}

		if (!xdr_pointer(&xdrs_de, (char **)&(client_data->argout),
		    (_Int)type_info.size,
			 (xdrproc_t)type_info.xdr))
		{
			*error = DevErr_CannotDecodeArguments;
			return (DS_NOTOK);
		}

		free (buf);
	}

	dev_printdebug (DBG_TRACE | DBG_API, "dev_putget_local() : leaving routine\n");

	*error = ret_data->error;
	status =  ret_data->status;
/*
 * free the returned client data structure
 */
	free(ret_data->argout);
	free(ret_data);

	return(status);
}


/**@ingroup clientAPIintern
 * Verifies the RPC connection to a server.
 *
 * If the device is not fully imported then import it by calling reinstall_rpc_connection(). This is the
 * case for stateless import.
 * 
 * If the connection is marked as a bad connection, it will be tried to reopen the connection to a server.
 * 
 * If the connection was already reopened the device has to be reimported from the server.
 * 
 * @param ds 	handle to device.
 * @param error Will contain an appropriate error code if the
 * 		corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_rpc_connection (devserver ds, long *error)
{
	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_rpc_connection() : entering routine\n");

	*error = 0;

#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
		return(DS_OK);
#endif /* TANGO */

/*
 * in order to make the device server api "stateless" check to see whether the device has been imported at 
 * all i.e. if the server has been identified yet, if not then try to import the device via the 
 * reinstall_rpc_connection() function. If this does not succeed then inform the client the device is not 
 * imported yet
 */
        if (ds->clnt == NULL)
        {
                if ( reinstall_rpc_connection (ds, error) == DS_NOTOK )
/*
 * an unexpected problem occurred, return an error
 */
                        return (DS_NOTOK);
                else
/*
 * the import worked , dev_putget() can carry on !
 */
                        return (DS_OK);
        }

/*
 * to be sure to work with the correct client handle initialise the pointer again.
 */
	ds->clnt = svr_conns[ds->no_svr_conn].clnt;

/*
 * first check if the connection to the server is marked as a bad connection.
 */
	if ((svr_conns[ds->no_svr_conn].rpc_conn_status == BAD_SVC_CONN)
		&& (check_rpc_connection (ds, error) == DS_NOTOK))
		return (DS_NOTOK);

/*
 * check whether the device was already reinstalled in case of a connection update.
 */
	dev_printdebug (DBG_API, "\ndev_putget() : connection statistics\n");
	dev_printdebug (DBG_API, "connections on clnt : %d\n", svr_conns[ds->no_svr_conn].no_conns);
	dev_printdebug (DBG_API, "rpc_conn_counter for clnt = %d\nrpc_conn_counter for ds  = %d\n",
	    	svr_conns[ds->no_svr_conn].rpc_conn_counter, ds->rpc_conn_counter);

	if (( svr_conns[ds->no_svr_conn].rpc_conn_counter != ds->rpc_conn_counter )
		&& (reinstall_rpc_connection (ds, error) == DS_NOTOK))
		return (DS_NOTOK);

	dev_printdebug (DBG_TRACE | DBG_API, "dev_rpc_connection() : leaving routine\n");

	return (DS_OK);
}

/**@ingroup clientAPIintern
 *
 * @param ds            handle to device.
 * @param clnt_stat  	return status of the clnt_call().
 * 
 * @param error		Will contain an appropriate error code if the
 * corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_rpc_error (devserver ds, enum clnt_stat clnt_stat, long *error)
{
	char	*hstring;

	dev_printdebug (DBG_TRACE | DBG_API, "\ndev_rpc_error() : entering routine\n");

	*error = 0;

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
		{
/*
 *  check the rpc connection to the server.
 */
			if ( check_rpc_connection (ds, error) == DS_NOTOK )
				return (DS_NOTOK);

			*error = DevErr_RPCTimedOut;
			return (DS_NOTOK);
		}
		else
		{
/*
 * check whether a tcp connection was lost. do not check only if protocol is TCP because this error 
 * can occur for asynch calls as well
 *			if ( svr_conns[ds->no_svr_conn].rpc_protocol == D_TCP )
 */
			{
				if ( (clnt_stat == RPC_CANTRECV) || (clnt_stat == RPC_CANTSEND) )
					svr_conns[ds->no_svr_conn].rpc_conn_status = BAD_SVC_CONN;
			}

/*
 *  check the rpc connection to the server.
 */
			if ( check_rpc_connection (ds, error) == DS_NOTOK )
				return (DS_NOTOK);

			*error = DevErr_RPCFailed;
			return (DS_NOTOK);
		}
	}

/*
 * mark the RPC connection as working
 */
	svr_conns[ds->no_svr_conn].rpc_error_flag = GOOD_SVC_CONN;

	dev_printdebug (DBG_TRACE | DBG_API, "dev_rpc_error() : leaving routine\n");
	return (DS_OK);
}

/**@ingroup clientAPI
 * By calling this function with one of the two defined protocol parameters D_UDP and D_TCP (API.h), 
 * the transport protocol for an open RPC connection will be set to the chosen protocol. Before
 * switching the protocol, an RPC connection to a device server has to be opened by a @ref dev_import()
 * call.
 *
 * All devices implemented in the same server and imported by the client use the same RPC connection.
 * Changing the protocol of a RPC connection with this function means changing the protocol for all 
 * devices of the same server.
 * @li D_UDP UDP protocol with maximal 8kByte data transfer.
 * @li D_TCP TCP protocol. TCP point to point connection with no transfer limitations.
 *
 * @param ds 		handle to device.
 * @param protocol 	Protocol for the rpc connection tcp or udp.
 * @param error  	Will contain an appropriate error code if the
 * 		        corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_rpc_protocol (devserver ds, long protocol, long *error)
{
	struct 	sockaddr_in 	serv_adr;
#if !defined vxworks
	struct 	hostent 	*ht;
#else  /* !vxworks */
	int 			host_addr;
#endif /* !vxworks */
	CLIENT  		*clnt;
	char			*hstring;
	long			client_id;
	long			i_nethost;
	nethost_info		*nethost;

	*error = 0;

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
		return(DS_OK);
#endif /* TANGO */

/*
 * in the stateless import case the device is not fully imported yet and
 * there is not client handle on which to change the protocol. In this
 * case check the protocol is valid and save it for later when the device
 * is imported by reinstall_rpc_connection()
 */
	if (ds->clnt == NULL)
	{
		switch (protocol) 
		{
			case (D_UDP) :	
			case (D_TCP) :	
				ds->rpc_protocol = protocol;
				return(DS_OK);
			default : 
				*error = DevErr_UnknownInputParameter;
				return(DS_NOTOK);
		}
	}

	if (!ds->no_database)
	{
		i_nethost = ds->i_nethost;
		nethost = &multi_nethost[i_nethost];

/*
 * If the security system is configured, verify the security key
 */
		if ( nethost->config_flags.security == True )
		{
			if ( verify_sec_key (ds, &client_id, error) == DS_NOTOK )
				return (DS_NOTOK);

/*
 * The protocol can only be changed, if no single user access was requested on the connection.
 */
			if (svr_conns[ds->no_svr_conn].open_si_connections > 0)
			{
				*error = DevErr_CantChangeProtWithOpenSIAccess;
				return (DS_NOTOK);
			}
		}
	}

/*
 * make sure the client handle stored in the svr_conn table is used because the copy of the 
 * client handle in the devserver structure could be stale - why is there a local copy anyway ?
 * andy 18jan97
 */
	ds->clnt = svr_conns[ds->no_svr_conn].clnt;

/*
 *  Store the current RPC timeout off the old connection. 
 */
	clnt_control (ds->clnt, CLGET_RETRY_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_retry_timeout);
	clnt_control (ds->clnt, CLGET_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_timeout);

	switch (protocol)
	{
		case (D_UDP):
			if ( svr_conns[ds->no_svr_conn].rpc_protocol == D_UDP )
				break;
/*
 * Before a new handle can be created, verify whether it is possible to connect to the remote host.
 */
			if (rpc_check_host(svr_conns[ds->no_svr_conn].server_host, error) == DS_NOTOK)
				return (DS_NOTOK);
/*
 * check if an UDP handle exists, if so reuse it 
 * andy 30jan97
 */
			if (svr_conns[ds->no_svr_conn].udp_clnt != NULL)
			{
				dev_printdebug (DBG_TRACE | DBG_API, "dev_rpc_protocol(): reuse existing UDP client handle\n");
				clnt = svr_conns[ds->no_svr_conn].udp_clnt;
			}
			else
			{
				clnt = clnt_create ( svr_conns[ds->no_svr_conn].server_host,
			    			svr_conns[ds->no_svr_conn].prog_number,
			    			svr_conns[ds->no_svr_conn].vers_number, "udp");
				if (clnt == NULL)
				{
					*error = DevErr_CannotCreateClientHandle;
					return (DS_NOTOK);
				}
			}
			svr_conns[ds->no_svr_conn].rpc_protocol = D_UDP;

/*
 * try to solve problem with Ultra-C/C++ 2.0.1 by not destroying the TCP client handle, keep it for reuse
 */
			svr_conns[ds->no_svr_conn].tcp_clnt = svr_conns[ds->no_svr_conn].clnt;
			svr_conns[ds->no_svr_conn].clnt = svr_conns[ds->no_svr_conn].udp_clnt = clnt;
			ds->clnt = clnt;

/*
 * If the security system is configured, free the security key for the old connection
 * and recreate it for the new connection.
 */
			if (!ds->no_database && (nethost->config_flags.security == True))
			{
				free_sec_key (ds);
				if ( create_sec_key (ds, error) == DS_NOTOK )
					return (DS_NOTOK);
			}
			break;

		case (D_TCP):
			if ( svr_conns[ds->no_svr_conn].rpc_protocol == D_TCP )
				break;
/*
 * Before a new handle can be created, verify whether it is possible to connect to the remote host.
 */
			if (rpc_check_host(svr_conns[ds->no_svr_conn].server_host, error) == DS_NOTOK)
				return (DS_NOTOK);

/* 
 * if a TCP client handle exists already then reuse it this modification has been introduced to try 
 * to solve the problems with the OS9 Ultra-C/C++ compiler V2.0.1 which timeouts after changing protocols 
 * andy 30/1/97
 */
			if (svr_conns[ds->no_svr_conn].tcp_clnt != NULL)
			{
				dev_printdebug (DBG_TRACE | DBG_API, "dev_rpc_protocol(): reuse existing TCP client handle\n");
				clnt = svr_conns[ds->no_svr_conn].tcp_clnt;
			}
			else
			{
#if !defined vxworks
				if ((ht =gethostbyname(svr_conns[ds->no_svr_conn].server_host)) == NULL )
				{
					*error = DevErr_CannotCreateClientHandle;
					return (DS_NOTOK);
				}
				memcpy ( (char *)&serv_adr.sin_addr, ht->h_addr, (size_t) ht->h_length );
#else /* !vxworks */
	                        host_addr = hostGetByName(svr_conns[ds->no_svr_conn].server_host);
	                        memcpy ( (char*)&serv_adr.sin_addr, (char*)&host_addr, 4);
#endif /* !vxworks */
				serv_adr.sin_family = AF_INET;
				serv_adr.sin_port = 0;
				svr_conns[ds->no_svr_conn].tcp_socket = RPC_ANYSOCK;

				clnt = clnttcp_create ( &serv_adr,
					    	svr_conns[ds->no_svr_conn].prog_number,
					    	svr_conns[ds->no_svr_conn].vers_number,
					    	&( svr_conns[ds->no_svr_conn].tcp_socket), 0, 0);

				if (clnt == NULL)
				{
					*error = DevErr_CannotCreateClientHandle;
					return (DS_NOTOK);
				}
			}
			svr_conns[ds->no_svr_conn].rpc_protocol = D_TCP;
			svr_conns[ds->no_svr_conn].udp_clnt = svr_conns[ds->no_svr_conn].clnt;
			svr_conns[ds->no_svr_conn].clnt = svr_conns[ds->no_svr_conn].tcp_clnt = clnt;
			ds->clnt = clnt;

/*
 * If the security system is configured, free the security key for the old connection
 * and recreate it for the new connection.
 */
			if (!ds->no_database && (nethost->config_flags.security == True))
			{
				free_sec_key (ds);
				if ( create_sec_key (ds, error) == DS_NOTOK )
					return (DS_NOTOK);
			}
			break;
		default:
			*error = DevErr_UnknownInputParameter;
			return (DS_NOTOK);
	}

/*
 *  Initialise the current RPC timeout to the new connection. 
 *
 */
	clnt_control (ds->clnt, CLSET_RETRY_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_retry_timeout);
	clnt_control (ds->clnt, CLSET_TIMEOUT, (char *) &svr_conns[ds->no_svr_conn].rpc_timeout);

	return (DS_OK);
}


/**@ingroup clientAPIintern
 * Allocate and initialise the devserver structure for a device which has not been successfully
 * imported so that it can be imported during a future call to dev_putget().
 *
 * @param device_name 	the device name
 * @param access 	access right requested by client
 * @param i_nethost
 * @param ds_ptr 	pointer to initialise devserver struct
 * @param error  	Will contain an appropriate error code if the
 *                      corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_notimported_init (char *device_name, long access, long i_nethost, devserver *ds_ptr, long *error)
{
        dev_printdebug (DBG_TRACE | DBG_API, "\ndev_notimported_init() : entering routine\n");

        *error = 0;

        if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
        {
                *error = DevErr_InsufficientMemory;
                return(DS_NOTOK);
        }
	memset((char *)*ds_ptr, 0, sizeof(struct _devserver));
        strncpy((*ds_ptr)->device_name, device_name, sizeof((*ds_ptr)->device_name));
        strncpy((*ds_ptr)->device_class,"Unknown", sizeof((*ds_ptr)->device_class));
        strncpy((*ds_ptr)->device_type,"Unknown", sizeof((*ds_ptr)->device_type));
        strncpy((*ds_ptr)->server_name,"Unknown", sizeof((*ds_ptr)->server_name));
        strncpy((*ds_ptr)->server_host,"Unknown", sizeof((*ds_ptr)->server_host));
        (*ds_ptr)->clnt             = NULL;
        (*ds_ptr)->ds_id            = 0;
        (*ds_ptr)->no_svr_conn      = 0;
        (*ds_ptr)->rpc_conn_counter = 0;
        (*ds_ptr)->dev_access       = access;
        (*ds_ptr)->i_nethost        = i_nethost;
	(*ds_ptr)->rpc_protocol	    = 0;
	(*ds_ptr)->rpc_timeout.tv_sec  = 0;
	(*ds_ptr)->rpc_timeout.tv_usec = 0;
	if (strchr(device_name,'?') != NULL)
		(*ds_ptr)->no_database = True;
	else
		(*ds_ptr)->no_database = False;

	return(DS_OK);
}

/**@ingroup clientAPI
 * Sets or reads the import timeout for an import() of a server.  A request to set the timeout
 * has to be asked with CLSET_TIMEOUT. The timeout will be set without any retry.
 * A request to read the timeout has to be asked with CLGET_TIMEOUT.
 * 
 * @param request 	indicates whether the timeout should be set or only read.
 * @param dev_timeout 	timeout structure.
 * @param error  	Will contain an appropriate error code if the
 *                      corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_import_timeout (long request, struct timeval *dev_timeout,
                                  long *error)
{
        *error = 0;
/*
 * the asynchronous timeout is stored as part of the device server handle
 * getting or seting this timeout means simply accessing this variable
 */
        switch (request)
	{
                case (CLSET_TIMEOUT) :
                        import_timeout = *dev_timeout;
                        import_retry_timeout = *dev_timeout;
                        break;
                case (CLGET_TIMEOUT) :
                        *dev_timeout = import_timeout;
                        break;
                default:
                        *error = DevErr_UnknownInputParameter;
                        return (DS_NOTOK);
        }
        return(DS_OK);
} 
