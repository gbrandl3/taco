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
 * File:		svc_api.c
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Server side of the API.  
 *
 * Author(s);	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	Feb 1994
 *
 * Version:	$Revision: 1.34 $
 *
 * Date:		$Date: 2008-10-23 09:48:34 $
 *
 ********************************************************************-*/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
/*
 * Include files and Static Routine definitions
 */

/*
 * C++ version
 */
#ifndef WIN32
#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#else
#	include <types.h>
#endif
#endif /* !WIN32 */

#include <API.h>
#include <private/ApiP.h>
#include <DevCmds.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>
#if defined WIN32
#include <rpc/Pmap_pro.h>
#include <rpc/pmap_cln.h>
#include <process.h>
/* #define NODATETIMEPICK to avoid compiler error. (I hate MicroSoft!)*/
#define NODATETIMEPICK
#include <commctrl.h>
#undef NODATETIMEPICK
#else
#include <rpc/pmap_clnt.h>
#endif  /* WIN32 */

#include "taco_utils.h"


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h
 */

/*
 *  Configuration flags
 */

#ifdef __cplusplus
extern "C" char 		*dev_error_stack;
#else
extern char 		    	*dev_error_stack;
#endif

/*
 *  Debug flag
 */

extern long 	debug_flag;



/*
 * Declare a pointer for the globale device array.
 * All exported devices are stored here.
 * Memory will be dynamically allocated during the
 * device_export().
 */
DevServerDevices	*devices = NULL;
long 			max_no_of_devices = 0;

/*
 * Flag to communicate errors from rpc_dev_put_asyn() to
 * rpc_dev_put_asyn_cmd().
 */
static long	asyn_error;

/*
 * Internal Functions
 */
static long read_device_id PT_((long, long *, long *, DevLong *));

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_import().
 *
 * Registers a device as imported and returns
 * an identification for the device to the client.
 *
 * @param dev_import_in 	input data structure.
 * @param rqstp         	RPC request structure.
 *
 * @return  	an identification for the device, the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_dev_import_out * _DLLFunc rpc_dev_import_4 (_dev_import_in *dev_import_in,
					     struct svc_req *rqstp)
{
	static _dev_import_out	dev_import_out;

	long 			device_id;
	register int 		i;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_import_4() : entering routine\n");

	device_id = 0;
	dev_import_out.ds_id  = 0;
	dev_import_out.status = DS_OK;
	dev_import_out.error  = DS_OK;
	snprintf (dev_import_out.server_name, sizeof(dev_import_out.server_name), "%s", config_flags->server_name);

	dev_import_out.var_argument.length   = 0;
	dev_import_out.var_argument.sequence = NULL;

/*
 * first try to to find the device among the list of devices already being served 
 */
	for (i=0; i<max_no_of_devices; i++)
	{
		if (devices[i].export_status == EXPORTED)
		{
/*
 * device will be known under its export name to the outside world
 */
			if (strcmp (dev_import_in->device_name,devices[i].export_name) == 0)
			{
/*
 * Do the security checks and initialization for the device import.
 */
				if ( config_flags->security == True )
				{
					if ( sec_svc_import (&devices[i], dev_import_in->connection_id, 
					    dev_import_in->client_id, dev_import_in->access_right,
					    rqstp, &dev_import_out.error )
					    == DS_NOTOK )
					{
						dev_import_out.status = DS_NOTOK;
						return (&dev_import_out);
					}
				}

/*
 * Combine device ID, connection ID and the export count as an identifier for the client.
 */
				device_id = (devices[i].export_counter << COUNT_SHIFT) +
				    (dev_import_in->connection_id << CONNECTIONS_SHIFT) + i;

				dev_import_out.ds_id = device_id;

				dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_import() : device %s found in export list\n",
				    dev_import_in->device_name);
				dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_import() : device_id = %d\n", device_id);
				dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_import() : ds_id = %d\n", i);
				dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_import() : export_counter = %d\n", 
				    devices[i].export_counter);
				dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "rpc_dev_import_4() : leaving routine\n");

				dev_import_out.status = DS_OK;
				return (&dev_import_out);
			}
		}
	}
/*
 * device of this name is not being served 
 * 
 * in the new philosophy do not instantiate 
 * it in this case. devices can only be created
 * at startup time or by calling the device
 * class's method create expressly. the import
 * routine will not do this, it is only possible
 * to import existing devices.
 *
 * - andy 03jul90
 */
	dev_import_out.error  = DevErr_DeviceOfThisNameNotServed;
	dev_import_out.status = DS_NOTOK;
	return (&dev_import_out);
}


/**@ingroup dsAPI
 * RPC procedure corresponding to dev_free().
 *
 * Unregisters an imported device. 
 *
 * @param dev_free_in  	input data structure.
 *
 * @return  	the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_dev_free_out * _DLLFunc rpc_dev_free_4 (_dev_free_in *dev_free_in)
{
	static _dev_free_out 	dev_free_out;
	long			ds_id;
	long			connection_id;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_free_4() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_free_4() : with device_id = %d\n", dev_free_in->ds_id);

	dev_free_out.status = DS_OK;
	dev_free_out.error  = DS_OK;

	dev_free_out.var_argument.length   = 0;
	dev_free_out.var_argument.sequence = NULL;

/*
 * Split up the device identification.
 */
	if (read_device_id (dev_free_in->ds_id, &ds_id, &connection_id, &dev_free_out.error) == DS_NOTOK)
	{
		dev_free_out.status = DS_NOTOK;
		return (&dev_free_out);
	}

/*
 * Clean up the security structure for the client connection.
 */
	if ( config_flags->security == True )
	{
		if ( sec_svc_free (&devices[(_Int)ds_id], connection_id,
		    dev_free_in->client_id, dev_free_in->access_right, 
		    &dev_free_out.error) == DS_NOTOK )
		{
			dev_free_out.status = DS_NOTOK;
			return (&dev_free_out);
		}
	}

	dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_free_4() : ds_id = %d\n", ds_id);
	dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_free_4() : connection_id = %d\n", connection_id);
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "rpc_dev_free_4() : leaving routine\n");

	return (&dev_free_out);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_putget().
 *   
 * Executes commands on devices by calling the command handler. 
 * 
 * @param server_data 	information about device identification, the command, and
 *			the input and output arguments for the command.
 *
 * @return  	the output arguments of the executed command,
 *		the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_client_data * _DLLFunc rpc_dev_putget_4 (_server_data *server_data)
{
	static _client_data	client_data;
	static DevVarArgument	vararg[1];
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 		ds;
#else
/*
 * C++ version
 */
	Device			*device;
#endif /* __cplusplus */
	long			ds_id;
	long 			connection_id;

	DevDataListEntry        data_type;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_4() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_4() : with ds_id = %d\n", server_data->ds_id);

/*
 * Free and preset the structure for outgoing arguments.
 */
	free (client_data.argout);
	memset ((char *)&client_data,0,sizeof(client_data));
/*
 * Clear dynamic error stack
 */
        dev_error_clear(); 

/*
 * Split up the device identification.
 */
	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &client_data.error) == DS_NOTOK)
	{
		client_data.status = DS_NOTOK;
		return (&client_data);
	}


#ifndef __cplusplus
/*
 * OIC version
 */
	ds = devices[(_Int)ds_id].ds;
#else
/*
 * C++ version
 */
	device = devices[(_Int)ds_id].device;
#endif /* __cplusplus*/

/*
 * Do the security checks for the command access.
 */
	if ( config_flags->security == True )
	{
		if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id,
		    server_data->client_id, server_data->access_right, 
		    server_data->cmd, &client_data.error) == DS_NOTOK )
		{
			client_data.status = DS_NOTOK;
			return (&client_data);
		}
	}

/*
 *  allocate and initialise outgoing arguments 
 */
	client_data.argout_type = server_data->argout_type;

	if (client_data.argout_type != D_VOID_TYPE)
	{
/*
 * Get the XDR data type from the loaded type list
 */
		if ( xdr_get_type( client_data.argout_type, &data_type, 
		    &client_data.error) == DS_NOTOK)
		{
			dev_printdebug (DBG_ERROR | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_4() : xdr_get_type(%d) returned error %d\n",
			    client_data.argout_type, client_data.error);

			client_data.status = DS_NOTOK;
			return (&client_data);
		}


		client_data.argout =
		    (char *) malloc ((unsigned int)data_type.size);
		if ( client_data.argout == NULL )
		{
			client_data.status = DS_NOTOK;
			client_data.error  = DevErr_InsufficientMemory;
			return (&client_data);
		}
		memset (client_data.argout, 0, (size_t)data_type.size);
	}
	else
	{
		client_data.argout = NULL;
	}


/*
 * in the simple case the command is passed directly on to the command_handler
 * method
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	client_data.status = (ds__method_finder (ds, DevMethodCommandHandler))
	    (	ds,
	    server_data->cmd,
	    server_data->argin,
	    server_data->argin_type,
	    client_data.argout,
	    client_data.argout_type,
	    &client_data.error);
#else
/*
 * C++ version
 */
	client_data.status = device->Command(server_data->cmd,
                                            (DevArgument)server_data->argin,
                                            server_data->argin_type,
                                            (DevArgument)client_data.argout,
                                            client_data.argout_type,
                                            &client_data.error);
#endif /* __cplusplus */

/*
 *  Because in case of error the status of the outgoing arguments
 *  is undefined, initialise argout to NULL before serialising.
 */
	if (client_data.status == DS_NOTOK)
	{
		free (client_data.argout);
		client_data.argout = NULL;
	}
/*
 * if a dynamic error has been generated tag it onto the end of
 * the client_data structure returned
 */
	if (dev_error_stack != NULL)
	{
		vararg[0].argument_type = D_STRING_TYPE;
		vararg[0].argument = (DevArgument)&dev_error_stack;
		client_data.var_argument.length = 1;
		client_data.var_argument.sequence = vararg;
	}

	return (&client_data);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_put().
 *
 * Executes commands on devices by calling the command handler. 
 *
 * server_data - contains information
 *		about device identification, the command, and
 *		the input arguments for the command.
 *
 * @return(s)  	the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_client_data * _DLLFunc rpc_dev_put_4 (_server_data *server_data)
{
	static _client_data	client_data;
	static DevVarArgument	vararg[1];
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 		ds;
#else
/*
 * C++ version
 */
	Device			*device;
#endif /* __cplusplus */
	long			ds_id;
	long 		connection_id;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_put_4() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_put_4() : with ds_id = %d\n", server_data->ds_id);

/*
 *  initialise outgoing arguments 
 */
	memset ((char *)&client_data,0,sizeof(client_data));
	client_data.argout_type = D_VOID_TYPE;
	client_data.argout = NULL;
/*
 * Clear dynamic error stack
 */
        dev_error_clear();

/*
 * Split up the device identification.
 */
	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &client_data.error) == DS_NOTOK)
	{
		client_data.status = DS_NOTOK;
		return (&client_data);
	}

#ifndef __cplusplus
/*
 * OIC version
 */
	ds = devices[(_Int)ds_id].ds;
#else
/*
 * C++ version
 */
	device = devices[(_Int)ds_id].device;
#endif /* __cplusplus */

/*
 * Do the security checks for the command access.
 */
	if ( config_flags->security == True )
	{
		if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id,
		    server_data->client_id, server_data->access_right, 
		    server_data->cmd, &client_data.error) == DS_NOTOK )
		{
			client_data.status = DS_NOTOK;
			return (&client_data);
		}
	}

/*
 * in the simple case the command is passed directly on to the command_handler
 * method
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	client_data.status = (ds__method_finder (ds, DevMethodCommandHandler))
	    (	ds,
	    server_data->cmd,
	    server_data->argin,
	    server_data->argin_type,
	    client_data.argout,
	    client_data.argout_type,
	    &client_data.error);
#else
/*
 * C++ version
 */
	client_data.status = device->Command(server_data->cmd,
	                                     (void*)server_data->argin,
	                                     server_data->argin_type,
	                                     (void*)client_data.argout,
	                                     client_data.argout_type,
	                                     &client_data.error);
#endif /* __cplusplus */
/*
 * if a dynamic error has been generated tag it onto the end of
 * the client_data structure returned
 */
	if (dev_error_stack != NULL)
	{
		vararg[0].argument_type = D_STRING_TYPE;
		vararg[0].argument = (DevArgument)&dev_error_stack;
		client_data.var_argument.length = 1;
		client_data.var_argument.sequence = vararg;
	}


	return (&client_data);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_putget_raw().
 *
 * Executes commands on devices by calling the command handler. 
 *
 * The result is returned in a raw opaque format.
 *
 * @param server_data 	information about device identification, the command, and
 * 			the input and output arguments for the command.
 * 
 * @return  	the executed command and the length, they represent
 *		in XDR format, in bytes.
 *
 *		Two output argument types are specified. One is used
 *		to serialise the data on the server side and the other
 *		to deserialise the data in ras opaque format on the
 *		client side.
 *
 *		The status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_client_raw_data * _DLLFunc rpc_dev_putget_raw_4 (_server_data *server_data)
{
	static _client_raw_data	client_data;
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 			ds;
#else
/*
 * C++ version
 */
	Device				*device;
#endif /* __cplusplus */
	long				ds_id;
	long 			connection_id;

	DevDataListEntry        	data_type;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_raw_4() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "rpc_dev_putget_raw_4() : with ds_id = %d\n", server_data->ds_id);

/*
 *  allocate and initialise outgoing arguments 
 */
	if (client_data.argout != NULL) 
		free (client_data.argout);

	memset ((char *)&client_data,0,sizeof(client_data));

/*
 * Clear dynamic error stack
 */
        dev_error_clear();

/*
 * Split up the device identification.
 */
	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &client_data.error) == DS_NOTOK)
	{
		client_data.status = DS_NOTOK;
		return (&client_data);
	}

#ifndef __cplusplus
/*
 * OIC version
 */
	ds = devices[(_Int)ds_id].ds;
#else
/*
 * C++ version
 */
	device = devices[(_Int)ds_id].device;
#endif

/*
 * Do the security checks for the command access.
 */
	if ( config_flags->security == True )
	{
		if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id, 
		    server_data->client_id, server_data->access_right, 
		    server_data->cmd, &client_data.error)
		    == DS_NOTOK )
		{
			client_data.status = DS_NOTOK;
			return (&client_data);
		}
	}

/*  
 * Set the datatypes used to serialise the outgoing arguments. 
 */
	client_data.ser_argout_type   = server_data->argout_type;

/*
 * Get the XDR data type from the loaded type list
 */
	if ( xdr_get_type( client_data.ser_argout_type, &data_type, 
	    &client_data.error) == DS_NOTOK)
	{
		dev_printdebug (DBG_ERROR | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_raw_4() : xdr_get_type(%d) returned error %d\n",
		client_data.ser_argout_type, client_data.error);
		client_data.status     = DS_NOTOK;
		return (&client_data);
	}

/*  
 * Set the datatypes used to serialise the outgoing arguments. 
 */
	client_data.deser_argout_type = D_OPAQUE_TYPE;

	if (client_data.ser_argout_type != D_VOID_TYPE)
	{
		client_data.argout =
		    (char *) malloc ((unsigned int)data_type.size);
		if ( client_data.argout == NULL )
		{
			client_data.status = DS_NOTOK;
			client_data.error  = DevErr_InsufficientMemory;
			return (&client_data);
		}
		memset (client_data.argout, 0, (size_t)data_type.size);
	}
	else
	{
		client_data.argout = NULL;
	}

/*
 * in the simple case the command is passed directly on to the command_handler
 * method
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	client_data.status = (ds__method_finder (ds, DevMethodCommandHandler))
	    (	ds,
	    server_data->cmd,
	    server_data->argin,
	    server_data->argin_type,
	    client_data.argout,
	    client_data.ser_argout_type,
	    &client_data.error);
#else
/*
 * C++ version
 */
	client_data.status = device->Command(server_data->cmd,
	                                     (void*)server_data->argin,
	                                     server_data->argin_type,
	                                     (void*)client_data.argout,
	                                     client_data.ser_argout_type,
	                                     &client_data.error);
#endif /* __cplusplus */
/*
 *  Now calculate the number of bytes the outgoing arguments 
 *  represent in XDR-format. 
 *  With the help of the xdr_length the parametes can be read on
 *  the client side in an opaque format.
 */
	if ( client_data.status == DS_OK )
	{
		client_data.xdr_length = 
	        ( (long (*)(void *)) data_type.xdr_length) 
		(client_data.argout);

		if ( client_data.xdr_length == DS_NOTOK)
		{
			client_data.error = DevErr_XDRLengthCalculationFailed;
			client_data.status = DS_NOTOK;
		}
	}

/*
 *  Because in case of error the status of the outgoing arguments
 *  is undefined, initialise argout to NULL before serialising.
 */
	if (client_data.status == DS_NOTOK)
	{
		free (client_data.argout);
		client_data.argout     = NULL;
		client_data.xdr_length = 0;
	}

	dev_printdebug (DBG_DEV_SVR_CLASS, "rpc_dev_putget_raw_4() : _client_raw_data\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "status            = %d\n", client_data.status);
	dev_printdebug ( DBG_DEV_SVR_CLASS, "error             = %d\n", client_data.error);
	dev_printdebug ( DBG_DEV_SVR_CLASS, "ser_argout_type   = %d\n", client_data.ser_argout_type);
	dev_printdebug ( DBG_DEV_SVR_CLASS, "deser_argout_type = %d\n", client_data.deser_argout_type);
	dev_printdebug ( DBG_DEV_SVR_CLASS, "xdr_length        = %d\n", client_data.xdr_length);

	return (&client_data);
}

/**@ingroup dsAPIintern
 * The first part of the aynchronous call.
 *
 * Will do the administration part of the call. Only the execution of the command is excluded
 * and handled in the second part of the call (rpc_dev_put_asyn_cmd) after the reponse
 * was sent back to the client. Errors are returned from this function to the client. 
 *
 * @param server_data 	information about device identification, the command, and
 *			the input arguments for the command.
 *
 * @return the status of the function (DS_OK or DS_NOTOK) and an appropriate error number 
 *		if the function fails.
 */
_client_data * _DLLFunc rpc_dev_put_asyn_4 (_server_data *server_data)
{
	static _client_data	client_data;
	long			ds_id;
	long 		connection_id;

	asyn_error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_4() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_4() : with ds_id = %d\n", server_data->ds_id);

/*
 *  initialise outgoing arguments 
 */
	memset ((char *)&client_data,0,sizeof(client_data));
	client_data.argout_type = D_VOID_TYPE;
	client_data.argout      = NULL;
	client_data.status      = DS_OK;
	client_data.error       = DS_OK;

/*
 * Clear dynamic error stack
 */
        dev_error_clear();

/*
 * Split up the device identification.
 */
	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &client_data.error) == DS_NOTOK)
	{
		asyn_error = client_data.error;
		client_data.status = DS_NOTOK;
		return (&client_data);
	}

/*
 * Do the security checks for the command access.
 */
	if ( config_flags->security == True )
	{
		if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id,
		    server_data->client_id, server_data->access_right, 
		    server_data->cmd, &client_data.error) == DS_NOTOK )
		{
			asyn_error = client_data.error;
			client_data.status = DS_NOTOK;
			return (&client_data);
		}
	}

	return (&client_data);
}

/**@ingroup dsAPI
 * The second part of the asynchronous call.
 * The command will be executed without any return values. 
 * Sends the RPC reply before executing this function.
 *
 * @param server_data 	information about device identification, the 
 *			command, and the input arguments for the command.
 */
void _DLLFunc rpc_dev_put_asyn_cmd (_server_data *server_data)
{
	static _client_data	client_data;
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 		ds;
#else
/*
 * C++ version
 */
	Device			*device;
#endif /* __cplusplus */
	long			ds_id;
	long 			connection_id;
	DevLong			error;

	error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_cmd() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_cmd() : with ds_id = %d\n", server_data->ds_id);

/*
 * Clear dynamic error stack
 */
        dev_error_clear();

/*
 * If an error occurred in rpc_dev_put_asyn(), do not execute
 * the command.
 */

	if ( asyn_error != DS_OK )
	{
		return;
	}

/*
 * Split up the device identification.
 */

	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &error) == DS_NOTOK)
	{
		return;
	}

#ifndef __cplusplus
/*
 * OIC version
 */
	ds = devices[(_Int)ds_id].ds;
#else
/*
 * C++ version
 */
	device = devices[(_Int)ds_id].device;
#endif

/*
 * in the simple case the command is passed directly on 
 * to the command_handler method.
 */

	client_data.argout_type = D_VOID_TYPE;
	client_data.argout      = NULL;

#ifndef __cplusplus
/*
 * OIC version
 */
	client_data.status = (ds__method_finder (ds, DevMethodCommandHandler))
	    (	ds,
	    server_data->cmd,
	    server_data->argin,
	    server_data->argin_type,
	    client_data.argout,
	    client_data.argout_type,
	    &error);
#else
/*
 * C++ version
 */
	client_data.status = device->Command(server_data->cmd,
	                                     (void*)server_data->argin,
	                                     server_data->argin_type,
	                                     (void*)client_data.argout,
	                                     client_data.argout_type,
	                                     &error);
#endif /* __cplusplus */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "rpc_dev_put_asyn_cmd() : leaving routine\n");
}

/**@ingroup dsAPIserver
 * This function makes devices visible for device server clients. All necessary connection information
 * for a @ref dev_import() call will be stored in a database table. Moreover the exported devices are 
 * added to the device server's global list of exported devices. The function is installed as a method
 * in the DeviceServerClass and accessible by the name @b DevMethodDevExport.
 *
 * @param name  name of the device.
 * @param ptr_ds pointer to the object.
 * @param error Will contain an appropriate error code if the corresponding 
 *		call returns a non-zero value.
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
#ifndef __cplusplus
/*
 * OIC version
 */
long dev_export (char *name, void *ptr_ds, DevLong *error)
#else
/*
 * C++ version
 */
long dev_export (char *name, Device *ptr_dev, DevLong *error)
#endif
{
	db_devinf		devinfo;
	DevServerDevices 	*new_devices;
	char			*strptr,
				proc_name[PROC_NAME_LENGTH],
				server_host[HOST_NAME_LENGTH];
	long			next_free;
	register long 	i;

#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer       ds;
	ds = (DevServer) ptr_ds;
#else
/*
 * C++ version
 */
	Device		*device;
	device =        ptr_dev;
#endif

	*error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\ndev_export() : entering routine\n");

/*
 * If no space is yet allocated in the global device array, allocate the first block of devices.
 */
	if ( max_no_of_devices == 0 )
	{
		devices = (DevServerDevices *) calloc (BLOCK_SIZE, sizeof(DevServerDevices));
		if ( devices == NULL )
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}

		max_no_of_devices = BLOCK_SIZE;
/*
 * Initialise the allocated device array.
 */
		for (i=0; i<max_no_of_devices; i++)
			devices[i].export_status = NOT_EXPORTED;
	}
/*
 * used to export a device of the specified name to the outside. ds points to the space allocated 
 * for the device. the next free entry will be found in the global device[] array and it will be 
 * reserved for the device. this array is accessed by the dev_import function to determine what 
 * devices are known to the server.
 */
	for (i=0; i<max_no_of_devices; i++)
	{
		if (devices[i].export_status == NOT_EXPORTED)
		{
			next_free = i;
			goto free_found;
		}
	}
/*
 * If all allocated devices are already used, allocate a new block of devices.
 */
	if ( (max_no_of_devices + BLOCK_SIZE) > MAX_DEVICES )
	{
		*error = DevErr_ExceededMaximumNoOfDevices;
		return (DS_NOTOK);
	}

	new_devices = (DevServerDevices *) realloc(devices, (unsigned int)((max_no_of_devices+BLOCK_SIZE) * sizeof(DevServerDevices)));
	if ( new_devices == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}
	devices = new_devices;
/*
 * Initialise the new allocated structures
 */
	memset ((char *)&devices[max_no_of_devices], 0, (BLOCK_SIZE*sizeof(DevServerDevices)));
	for (i=max_no_of_devices; i<(max_no_of_devices+BLOCK_SIZE); i++)
		devices[i].export_status = NOT_EXPORTED;

	next_free = max_no_of_devices;
	max_no_of_devices += BLOCK_SIZE;

free_found:
/*
 *  convert the device name to lower case letters
 */
	str_tolower(name);

/* 
 * do not export the device to the static database if no_database is selected ...
 */
	if (!config_flags->no_database)
	{
/*
 *  export device to the static database. device server configuration for this device
 *  will be stored in the static database
 */
		devinfo.device_name = name;
		strncpy(server_host, config_flags->server_host, sizeof(server_host));
		devinfo.host_name   = server_host;
		devinfo.pn	    = (u_int)config_flags->prog_number;
		devinfo.vn          = (u_int)config_flags->vers_number;
/*
 * Extract the name of the executable
 */
		snprintf (proc_name, sizeof(proc_name), "%s", config_flags->server_name);
		strptr = strchr (proc_name, '/');
		*strptr = '\0'; 
		devinfo.proc_name   = proc_name;

#ifndef __cplusplus
/*
 * OIC version
 */
		devinfo.device_class = (ds->devserver.class_pointer)->devserver_class.class_name;
#else
/*
 * C++ version
 */
		devinfo.device_class = const_cast<char *>(device->GetClassName());
#endif /* __cplusplus */

/*
 *  check whether the pointer to the device type is already initialised
 */
#ifndef __cplusplus
/*
 * OIC version
 */
      		if ( strncmp (ds->devserver.dev_type, TYPE_INIT, strlen(TYPE_INIT)) != 0 )
         		snprintf (ds->devserver.dev_type, sizeof(ds->devserver.dev_type), TYPE_DEFAULT );
      		devinfo.device_type  = ds->devserver.dev_type;
#else
/*
 * C++ version
 */
		devinfo.device_type  = const_cast<char *>(device->GetDevType());
#endif /* __cplusplus */

		dev_printdebug (DBG_DEV_SVR_CLASS, "dev_export() :  name = %s , class = %s , type = %s\n",
			devinfo.device_name, devinfo.device_class, devinfo.device_type);
		dev_printdebug (DBG_DEV_SVR_CLASS, "dev_export() :  host_name = %s , proc = %s , pn = %d , vn = %d\n",
			devinfo.host_name, devinfo.proc_name, devinfo.pn, devinfo.vn );
		if ( db_dev_export (&devinfo,1,error) < DS_OK )
			return (DS_NOTOK);
	}
/*
 * Store the object pointer and the device name in the internal
 * devices array.
 */
#ifndef __cplusplus
/*
 * OIC version
 */
	devices[next_free].ds            = ds;
#else
/*
 * C++ version
 */
	devices[next_free].device        = device;
#endif /* __cplusplus */
	devices[next_free].export_status = EXPORTED;
	devices[next_free].export_counter++;
/*
 * If the export counter reaches its maximum value,
 * restart the counter.
 */
	if (devices[next_free].export_counter > MAX_COUNT)
      		devices[next_free].export_counter = 1;

/*
 * The length of the device name can only be 80 characters!
 */
	if ( strlen(name) < MAX_NAME_LENGTH )
      		snprintf (devices[next_free].export_name, sizeof(devices[next_free].export_name), "%s", name);
	else
      	{
      		*error  = DevErr_NameStringToLong;
      		return (DS_NOTOK);
      	}
	dev_printdebug(DBG_TRACE | DBG_DEV_SVR_CLASS, "dev_export() : leaving routine\n");
	return(DS_OK);
}

/**@ingroup oicAPI
 * This function searches for a destroy method (@b DevMethodDestroy) in the device server object class.
 * If no destroy method is implemented in the objext class, its superclasses are searched. Arriving at 
 * the end of the class tree, the destroy method of the general device server class will be executed.
 * 
 * The general destroy method will free the object correctly only, if no memory allocation was done for
 * the object fields outside the @b DevServerPart structure of the object. The device name, as a field 
 * of DevServerPart will be freed correctly by the general device server class destroy method.
 *
 * Also exported objects can be destroyed. They will be deleted from the list of exported devices and 
 * all clients accesses will be stopped.
 *
 * @param ptr_ds  	pointer to the object structure.
 * @param error 	Will contain an appropriate error
 *		code if the corresponding call returns a non-zero value.
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long ds__destroy (void *ptr_ds, DevLong *error)
{
	short	i;

#ifndef __cplusplus
	DevServer 	ds = (DevServer) ptr_ds;
#else
	Device	 	*device = (Device*) ptr_ds;

#endif /* __cplusplus */

	*error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nds__destroy() : entering routine\n");

/*
 * If the object was exported, delete the entry in the list of exported devices before freeing the object.
 */
	for (i=0; i<max_no_of_devices; i++)
	{
		if (devices[i].export_status == EXPORTED)
		{
/*
 * compare the device name
 */
#ifndef __cplusplus
			if (strcmp (ds->devserver.name, devices[i].export_name) == 0)
#else
			if (strcmp (device->GetDevName(), devices[i].export_name) == 0)
#endif /* __cplusplus */
			{
/*
 * Delete the device from the export list
 * and block all connected client connections.
 */
				devices[i].export_status = NOT_EXPORTED;
#ifndef __cplusplus
				devices[i].ds            = NULL;
#else
				devices[i].device        = NULL;
#endif
				devices[i].export_counter++;

				dev_printdebug ( DBG_DEV_SVR_CLASS, "ds__destroy() : device %s deleted from export list\n",
#ifndef __cplusplus
				    ds->devserver.name);
#else
				    device->GetDevName());
#endif /* __cplusplus */

				break;
			}
		}
	}

/*
 * Destroy the object.
 */
#ifndef __cplusplus
	if ( (ds__method_finder (ds, DevMethodDestroy)(ds, error)) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}
#else
  	delete device;
#endif /* __cplusplus */

	return (DS_OK);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_cmd_query().
 *
 * Retrieves all available information from the command list of the specified device.
 *
 * Information about one command is stored in a _dev_cmd_info structure defined in API_xdr.h.
 *
 * A sequence of these stuctures will be returned as well as the name of the device class.
 *
 * The _dev_query_out type is defined in API_xdr.h.
 *
 * @param dev_query_in 	input structure with the identification of the device.
 *
 * @return	pointer to structure containig a sequence of _dev_cmd_info 
 *		structures, the name of the device class, an error and a status flag.
 */
_dev_query_out * _DLLFunc rpc_dev_cmd_query_4 (_dev_query_in *dev_query_in)
{
	static _dev_query_out	dev_query_out = { 0, NULL, "", 0, 0, {0, NULL}};
	long        		ds_id;
	long        		connection_id;
	static DevVarArgument 	vararg[1024];
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer	ds;
	DevCommandList	ds_cl;
	DevServerClass	ds_class;
	_Int		i;
#else
/*
 * C++ version
 */
	Device		*device;
	long 		error;
	long 		ret;
	u_long		i;
#endif

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_cmd_query_4() : entering routine\n");

	dev_query_out.error  = DS_OK;
	dev_query_out.status = DS_OK;

	dev_query_out.var_argument.length   = 0;
	dev_query_out.var_argument.sequence = NULL;
/*
 * Free last allocated memory for the command info sequence.
 */
	if (dev_query_out.sequence != NULL)
	{
		free (dev_query_out.sequence);
		dev_query_out.sequence = NULL;
	}
/*
 * Split up the device identification.
 */
	if (read_device_id (dev_query_in->ds_id, &ds_id, &connection_id, &dev_query_out.error) == DS_NOTOK)
	{
		dev_query_out.status = DS_NOTOK;
		return (&dev_query_out);
	}

/*
 * Open access to device and class structures.
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	ds       = devices[(_Int)ds_id].ds;
	ds_class = ds->devserver.class_pointer;
	ds_cl    = ds_class->devserver_class.commands_list;
#else
/*
 * C++ version
 */
	device   = devices[(_Int)ds_id].device;
#endif /* __cplusplus */

/*
 * Get number of implemented commands and allocate memory for the command info sequence.
 */
#ifndef __cplusplus
/*
 * OIC version
 */
	dev_query_out.length = ds_class->devserver_class.n_commands;
#else
/*
 * C++ version
 */
	dev_query_out.length = device->GetCommandNumber();
#endif
	dev_query_out.sequence = (_dev_cmd_info *) malloc (dev_query_out.length * sizeof (_dev_cmd_info));
	if ( dev_query_out.sequence == NULL )
	{
		dev_query_out.error  = DevErr_InsufficientMemory;
		dev_query_out.status = DS_NOTOK;
		return (&dev_query_out);
	}
/*
 * Get information about command code, data types and class for every implemented command.
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	for ( i=0; (u_long)i<dev_query_out.length; i++ )
	{
		dev_query_out.sequence[i].cmd      = ds_cl[i].cmd;
		dev_query_out.sequence[i].in_type  = ds_cl[i].argin_type;
		dev_query_out.sequence[i].out_type = ds_cl[i].argout_type;
	}
	snprintf (dev_query_out.class_name, sizeof(dev_query_out.class_name), "%s", ds_class->devserver_class.class_name);
#else
/*
 * C++ version
 */
	ret = device->CommandQuery(dev_query_out.sequence);
	if (ret != DS_OK)
	{
		free(dev_query_out.sequence);
		dev_query_out.error  = error;
		dev_query_out.status = DS_NOTOK;
		return (&dev_query_out);
	}
	snprintf (dev_query_out.class_name, sizeof(dev_query_out.class_name), "%s", device->GetClassName());
#endif /* __cplusplus */

/* 
 * if the device server command list has specified the command names (by testing the first entry != NULL)
 * then tag them onto the end of the sequence so that the client does not have to query the database for 
 * them (very useful if -nodb)
 *
 * andy 3/3/2002
 */

#ifndef __cplusplus
	if (ds_cl[0].cmd_name != NULL)
	{
        	dev_query_out.var_argument.length = 0;
		for (i=0; i < dev_query_out.length; i++)
		{
        		vararg[i].argument_type      = D_STRING_TYPE;
        		vararg[i].argument           = (DevArgument)&ds_cl[i].cmd_name;
		}
        	dev_query_out.var_argument.length = dev_query_out.length;
        	dev_query_out.var_argument.sequence = vararg;
	}
#else
	if (device->commands_map.begin() != device->commands_map.end() && (device->commands_map.begin())->second.cmd_name != NULL)
	{
	        dev_printdebug(DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_cmd_query_4() : %d %p\n", (device->commands_map.begin())->first, (device->commands_map.begin())->second.cmd_name);
		for (u_long i = 0; i < dev_query_out.length; i++)
		{
			dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_cmd_query_4() : %d %d %s\n", i, dev_query_out.sequence[i].cmd,
								device->commands_map[dev_query_out.sequence[i].cmd].cmd_name);
			vararg[i].argument_type      = D_STRING_TYPE;
			vararg[i].argument           = (DevArgument)&(device->commands_map[dev_query_out.sequence[i].cmd].cmd_name);
		}
	        dev_query_out.var_argument.length = dev_query_out.length;
	        dev_query_out.var_argument.sequence = vararg;
	}
#endif /* __cplusplus */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_cmd_query_4() : leaving routine\n");
	return (&dev_query_out);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_event_query().
 *
 * Retrieves all available information from the event list of the specified device.
 *
 * Information about one command is stored in a _dev_event_info structure defined in API_xdr.h.
 *
 * A sequence of these stuctures will be returned as well as the name of the device class.
 *
 * The _dev_query_out type is defined in API_xdr.h.
 *
 * @param dev_query_in
 *
 * @return
 */
_dev_queryevent_out * _DLLFunc rpc_dev_event_query_4 (_dev_query_in *dev_query_in)
{
	static _dev_queryevent_out	dev_query_out = { 0, NULL, "", 0, 0, { 0, NULL}};
	long        			ds_id;
	long        			connection_id;
	static DevVarArgument 		vararg[1024];
#ifndef __cplusplus
 /*
  * OIC version
  */
	DevServer			ds;
	DevEventList			ds_ev;
	DevServerClass			ds_class;
	_Int				i;
#else
/*
 * C++ version
 */
	Device				*device;
	long 				error;
	long 				ret;
#endif

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_event_query_4() : entering routine\n");

	dev_query_out.error  = 0;
	dev_query_out.status = 0;
	dev_query_out.length = 0;
	dev_query_out.var_argument.sequence = NULL;

/*
 * Split up the device identification.
 */
	if (read_device_id (dev_query_in->ds_id, &ds_id, &connection_id, &dev_query_out.error) == DS_NOTOK)
	{
		dev_query_out.status = DS_NOTOK;
		return (&dev_query_out);
	}

/*
 * Open access to device and class structures.
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	ds       = devices[(_Int)ds_id].ds;
	ds_class = ds->devserver.class_pointer;
	ds_ev    = ds_class->devserver_class.events_list;
#else
/*
 * C++ version
 */
	device   = devices[(_Int)ds_id].device;
#endif /* __cplusplus */

/*
 * Free last allocated memory for the command info sequence.
 */
	if (dev_query_out.sequence != NULL)
	{
		free (dev_query_out.sequence);
	}
/*
 * Get number of implmented events and allocate memory for the event info sequence.
 */
#ifndef __cplusplus
/*
 * OIC version
 */
	dev_query_out.length = ds_class->devserver_class.n_events;
#else
/*
 * C++ version
 */
	dev_query_out.length = device->GetEventNumber();
#endif
	dev_query_out.sequence = (_dev_event_info *) malloc(dev_query_out.length * sizeof (_dev_event_info));
	if ( dev_query_out.sequence == NULL )
	{
		dev_query_out.error  = DevErr_InsufficientMemory;
		dev_query_out.status = -1;
		return (&dev_query_out);
	}

/*
 * Get information about event code, data types and class for every implented event.
 */
#ifndef __cplusplus
/*
 * OIC version
 */
	for ( i=0; (u_long)i<dev_query_out.length; i++ )
	{
		dev_query_out.sequence[i].event    = 0;
		dev_query_out.sequence[i].out_type = ds_ev[i].argout_type;
	}

	snprintf (dev_query_out.class_name, sizeof(dev_query_out.class_name), "%s", ds_class->devserver_class.class_name);
#else
/*
 * C++ version
 */
	ret = device->EventQuery(dev_query_out.sequence);
	if (ret != DS_OK)
	{
		free(dev_query_out.sequence);
		dev_query_out.error  = error;
		dev_query_out.status = -1;
		return (&dev_query_out);
	}
	snprintf (dev_query_out.class_name, sizeof(dev_query_out.class_name), "%s", device->GetClassName());
#endif /* __cplusplus */

/* 
 * if the device server event list has specified the event names (by testing the first entry != NULL)
 * then tag them onto the end of the sequence so that the client does not have to query the database for 
 * them (very useful if -nodb)
 */
#ifndef __cplusplus
	if (ds_ev && ds_ev[0].event_name != NULL)
	{
		for (i=0; i < dev_query_out.length; i++)
		{
        		vararg[i].argument_type      = D_STRING_TYPE;
        		vararg[i].argument           = (DevArgument)&ds_ev[i].event_name;
		}
        	dev_query_out.var_argument.length = dev_query_out.length;
        	dev_query_out.var_argument.sequence = vararg;
	}
#else
	if ((device->events_list.begin())->second.event_name != NULL)
	{
		for (u_long i = 0; i < dev_query_out.length; i++)
		{
			dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_event_query_4() : %d %d %s\n", i, dev_query_out.sequence[i].event,
								device->events_list[dev_query_out.sequence[i].event].event_name);
			vararg[i].argument_type      = D_STRING_TYPE;
			vararg[i].argument           = (DevArgument)&(device->events_list[dev_query_out.sequence[i].event].event_name);
		}
	        dev_query_out.var_argument.length = dev_query_out.length;
	        dev_query_out.var_argument.sequence = vararg;
	}
#endif /* __cplusplus */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_event_query_4() : leaving routine\n");
	return (&dev_query_out);
}

/**@ingroup dsAPIintern
 * Split up the device identification into its information fields.
 *
 * @param device_id   	client handle to access the device.
 * @param ds_id 	access to exported device. Place in the array of exported devices.
 * @param connection_id access to device connections. Place in the connections (or 
 *			client access) array of an exported device.
 * @param error 	pointer to error code, in case routine fails.
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 *
 */
static long read_device_id (long device_id, long *ds_id, long *connection_id, DevLong *error)
{
	long 	export_counter;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nread_device_id() : entering routine\n");

	*error = DS_OK;

	/*
    	* Split up the device identification
    	*/

	*ds_id        = device_id & DEVICES_MASK;

	export_counter = device_id >> COUNT_SHIFT;
	export_counter = export_counter & COUNT_MASK;

	*connection_id = device_id >> CONNECTIONS_SHIFT;
	*connection_id = *connection_id & CONNECTIONS_MASK;

	/*
    	* Verify the count of exports.
    	*/

	if ( export_counter != devices[(_Int)*ds_id].export_counter )
	{
		*error = DevErr_DeviceNoLongerExported;
		return (DS_NOTOK);
	}

	dev_printdebug (DBG_DEV_SVR_CLASS, "read_device_id() : ds_id = %d   conn_id = %d\n",
	    *ds_id, *connection_id);
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "read_device_id() : leaving routine\n");

	return (DS_OK);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_putget_local().
 *
 * Executes commands on devices by calling the command handler. 
 *
 * The normal entry point rpc_dev_putget_4() cannot be used, because reinitialising 
 * the static return structure client_data would cause pointer clashes.
 *
 * @param server_data 	information about device identification, the command, and
 *			the input and output arguments for the command.
 *
 * @return   	the output arguments of the executed command,
 *		the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
 */
_client_data * _DLLFunc rpc_dev_putget_local (_server_data *server_data)
{
	_client_data	*client_data;
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 		ds;
#else
/*
 * C++ version
 */
	Device			*device;
#endif
	long			ds_id;
	long 		connection_id;

	DevDataListEntry        data_type;


	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_local() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_local() : with ds_id = %d\n", server_data->ds_id);

/*
 * Free and preset the structure for outgoing arguments.
 */

	client_data = (_client_data*) calloc(1, sizeof(_client_data));


/*
 * Split up the device identification.
 */

	if (read_device_id (server_data->ds_id, &ds_id, &connection_id, 
	    &client_data->error) == DS_NOTOK)
	{
		client_data->status = DS_NOTOK;
		return (client_data);
	}

#ifndef __cplusplus
/*
 * OIC version
 */
	ds = devices[(_Int)ds_id].ds;
#else
/*
 * C++ version
 */
	device = devices[(_Int)ds_id].device;
#endif


/*
 * Do the security checks for the command access.
 */

	if ( config_flags->security == True )
	{
		if ( sec_svc_cmd (&devices[(_Int)ds_id], connection_id,
		    server_data->client_id, server_data->access_right, 
		    server_data->cmd, &client_data->error)
		    == DS_NOTOK )
		{
			client_data->status = DS_NOTOK;
			return (client_data);
		}
	}

/*
 *  allocate and initialise outgoing arguments 
 */

	client_data->argout_type = server_data->argout_type;

	if (client_data->argout_type != D_VOID_TYPE)
	{
/*
 * Get the XDR data type from the loaded type list
 */

		if ( xdr_get_type( client_data->argout_type, &data_type, 
		    &client_data->error) == DS_NOTOK)
		{
			dev_printdebug (DBG_ERROR | DBG_DEV_SVR_CLASS, "\nrpc_dev_putget_local() : xdr_get_type(%d) returned error %d\n",
			    client_data->argout_type, client_data->error);

			client_data->status = DS_NOTOK;
			return (client_data);
		}


		client_data->argout =
		    (char *) malloc ((unsigned int)data_type.size);
		if ( client_data->argout == NULL )
		{
			client_data->status = DS_NOTOK;
			client_data->error  = DevErr_InsufficientMemory;
			return (client_data);
		}
		memset (client_data->argout, 0, (size_t)data_type.size);
	}
	else
	{
		client_data->argout = NULL;
	}

/*
 * in the simple case the command is passed directly on to the command_handler method
 */

#ifndef __cplusplus
/*
 * OIC version
 */
	client_data->status = (ds__method_finder (ds, DevMethodCommandHandler))
	    (	ds,
	    server_data->cmd,
	    server_data->argin,
	    server_data->argin_type,
	    client_data->argout,
	    client_data->argout_type,
	    &client_data->error);
#else
/*
 * C++ version
 */
	client_data->status = device->Command(server_data->cmd,
	                                     (void*)server_data->argin,
	                                     server_data->argin_type,
	                                     (void*)client_data->argout,
	                                     client_data->argout_type,
	                                     &client_data->error);
#endif /* __cplusplus */

/*
 *  Because in case of error the status of the outgoing arguments
 *  is undefined, initialise argout to NULL before serialising.
 */
	if (client_data->status == DS_NOTOK)
	{
		free (client_data->argout);
		client_data->argout = NULL;
	}

	return (client_data);
}

/**@ingroup dsAPI
 * RPC procedure corresponding to dev_ping().
 *
 * Checks to see if the requested device is served by this server.
 *
 * @param dev_import_in 	input data structure.
 * @param rqstp         	RPC request structure.
 *
 * @return  	an identification for the device, the status of the function 
 *		(DS_OK or DS_NOTOK) and an appropriate error number if the call fails.
 */
_dev_import_out * _DLLFunc rpc_dev_ping_4 (_dev_import_in *dev_import_in,
					   struct svc_req *rqstp)
{
	static _dev_import_out	dev_import_out;
	register int 		i;
	char			*in_name_stripped;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_ping_4() : entering routine\n");

	dev_import_out.ds_id  = 0;
	dev_import_out.status = DS_OK;
	dev_import_out.error  = DS_OK;
	snprintf (dev_import_out.server_name, sizeof(dev_import_out.server_name), "%s", config_flags->server_name);

	dev_import_out.var_argument.length   = 0;
	dev_import_out.var_argument.sequence = NULL;

/* 
 * strip nethost name, if present 
 */
	if(dev_import_in->device_name[0]=='/' && dev_import_in->device_name[1]=='/') /* Nethost part present */
		in_name_stripped=strchr(dev_import_in->device_name, '/');
	else
		in_name_stripped=dev_import_in->device_name;
/*
 * first try to to find the device among the list of
 * devices already being served 
 *
 */
	for (i=0; i<max_no_of_devices; i++)
		if (devices[i].export_status == EXPORTED)
/*
 * device will be known under its export name to the outside world
 */
			if (strcmp (in_name_stripped, devices[i].export_name) == 0)
			{
				dev_import_out.status = DS_OK;
				return (&dev_import_out);
			}
/*
 * device of this name is not being served 
 * 
 */
	dev_import_out.error  = DevErr_DeviceOfThisNameNotServed;
	dev_import_out.status = DS_NOTOK;
	return (&dev_import_out);
}

static int      udp_socket;
static int      tcp_socket;

extern long synch_svc_udp_sock;
extern long synch_svc_tcp_sock;

/*
 * debug flag
 */

extern long debug_flag;

static SVCXPRT *transp;
static SVCXPRT *transp_tcp;

extern long minimal_access;

static void _WINAPI devserver_prog_4    PT_( (struct svc_req *rqstp,SVCXPRT *transp) );
#ifdef DARWIN
	typedef void (* MyRpcFuncPtr)();
#else
	typedef void (* MyRpcFuncPtr)(struct svc_req *, SVCXPRT *);
#endif

static MyRpcFuncPtr devserver_prog = (MyRpcFuncPtr)devserver_prog_4;

long svc_check  PT_( (DevLong *error) );
long db_check	PT_( (DevLong *error) );

#ifdef WIN32  /* WIN32 */

#include "resource.h"

#define TITLE_STR 	"TACO-Device Server: "
#define	MB_ERR		(MB_OK | MB_ICONEXCLAMATION)
#define	MB_INFO		(MB_OK | MB_ICONINFORMATION)
#endif /* WIN32 */

static int error_msg(const char *msg)
{
#ifdef WIN32
	MessageBox((HWND)NULL, msg, TITLE_STR, MB_INFO);
	return (FALSE);
#else
	fprintf (stderr, msg);
	return (DS_NOTOK);
#endif
}

/**
 * @ingroup dsAPIintern
 * VXWORKS and NOMAIN routine to create a device server - device_server()
 *
 * @param server_name device server name registered in the database
 * @param pers_name   personal device server name registered in the database
 * @param m_opt       if it is TRUE use the message server for sending messages
 * @param s_opt             if TRUE call the 'startup' function of the server
 * @param nodb              if TRUE do not use the database server for the resources
 * @param pn        if different from 0 use this RPC program number
 * @param n_device    number of devices to export
 * @param device_list list of devices
 *
 * @retval DS_OK
 * @retval DS_NOTOK
 */
int device_server (char *server_name, char *pers_name, int m_opt, int s_opt, int nodb, int pn, int n_device, char** device_list)
{
	char    		host_name [HOST_NAME_LENGTH],
				dsn_name [37],
		//		*proc_name,
				*display,
				res_path [80],
				res_name[80];
	DevVarStringArray	default_access;
	db_resource		res_tab;
	void		*test_heap;

	long			prog_number=0,
				status;
	DevLong			error = 0;
	int			pid = 0;
	short			sig,
				i,
				j;

	if (strlen(server_name) >= DS_NAME_LENGTH)
	{
		char msg[80];
		snprintf(msg, sizeof(msg),"Filename to long : server_name <= %d char's\n", DS_NAME_LENGTH - 1);
		return error_msg(msg);
	}

	if (strlen(pers_name) >= DSPERS_NAME_LENGTH)
	{
		char msg[80];
		snprintf(msg, sizeof(msg), "Personal DS_name to long : personal_dsname <= %d char's\n", DSPERS_NAME_LENGTH - 1);
		return error_msg(msg);
	}
/*
 * unregister this program number from the portmapper - this is potentially
 * dangerous because it can unregister another running server. we assume
 * the -pn option is used by those who know what they are doing ...
 */
	if (prog_number != 0)
	{
		pmap_unset (prog_number, API_VERSION);
		pmap_unset (prog_number, ASYNCH_API_VERSION);
	}

	snprintf(dsn_name, sizeof(dsn_name), "%s/%s", server_name, pers_name);
	nethost_alloc(&error);  /* allocate multi_nethost */
/*
 * option nodb means run device server without database
 */
	if (nodb > 0)
	{
		config_flags->no_database = True;
		xdr_load_kernel(&error);
	}
/*
 * option -device means remaining command line arguments are device names
 */
	config_flags->device_no = n_device;
	config_flags->device_list = device_list;

#ifdef vxworks
/*
 * call rpcTaskInit() to initialise task-specific data structures 
 * required by RPC (cf. VxWorks Reference manual pg. 1-203). 
 * Failure to do so will result in the task crashing the first
 * time a call to is made to an RPC function
 */
	rpcTaskInit();
#endif /* vxworks */
/*
 *  get process ID, host_name 
 *  and create device server network name
 */

#if defined (WIN32)
	pid = _getpid ();
#elif !defined (vxworks)
	pid = getpid ();
#else  /* !vxworks */
	pid = taskIdSelf ();
#endif /* !vxworks */
/*
 * M. Diehl, 22.7.1999
 * We have to take care here, since hostname might be set to the FQDN thus
 * gethostname() returns "host.and.complete.domain" which may easily exceed
 * the 19 characters reserved for it!
 * There are 3 possible solutions:
 * 1) General extension of sizeof(host_name) reasonably beyond 19 characters
 * 2) Extracting the hostname from the FQDN
 * 3) Switching to an IP-String
 *
 * Here are some ideas on that:
 *
 * 1) Is obviously the best way, especially as there are several limitations
 *    to SHORT_NAME_SIZE=32 in quite a number of files. However this implies
 *    some major issues with respect to DBM and platform independence.
 * 2) Means switching to the intended behaviour and is rather easy to
 *    achieve. However this will fail, if the host (on which the device
 *    server is running e.g.) is not in our search domain - which is
 *    a disadvantage of the current implementation anyway, I believe.
 * 3) Replacing the hostname by it's IP-String-Quad (if it won't fit into
 *    19 characters) is possible without any changes at DBM code and
 *    should work in all situations. The disadvantage however is,
 *    that db_tools show up with xxx.xxx.xxx.xxx values instead of
 *    well-known hostnames, so one might have to use nslookup.
 *
 * What follows realizes suggestion 3) using some code from sec_api.c
 * with respect to different IP-retrieving for VxWorks and the rest of the
 * world. However, one has to keep in mind, that a bunch of other stuff
 * will fail, if FQDN exceeds SHORT_NAME_SIZE=32 - which is not too hard!
 */
	if( taco_gethostname(host_name, sizeof(host_name)) != 0 )
		exit(DS_NOTOK);

	str_tolower(dsn_name);
	str_tolower(host_name);
	strncpy(config_flags->server_name, dsn_name, sizeof(config_flags->server_name)); 
	strncpy(config_flags->server_host, host_name, sizeof(config_flags->server_host)); 

/*
 * install signal handling for HPUX, SUN, OS9
 */
	(void) signal(SIGINT,  main_signal_handler);
	(void) signal(SIGTERM, main_signal_handler);
        (void) signal(SIGABRT, main_signal_handler);

#if defined (unix)
	(void) signal(SIGQUIT, main_signal_handler);
/*
 * SIGHUP and SIGPIPE are now caught in the main signal handler
 * which will simply return. This is needed for asynchronous
 * clients and servers to detect a server/client going down.
 *
 * andy 8may97
 */
	(void) signal(SIGHUP,  main_signal_handler);
	(void) signal(SIGPIPE, main_signal_handler);
#endif /* unix */

#if defined (WIN32)
	(void) signal(SIGBREAK,main_signal_handler);
#endif /* WIN32 */

#if ( OSK || _OSK )
	(void) signal(SIGQUIT, main_signal_handler);
#endif /* OSK || _OSK */

	if (nodb == False)
	{
/*
 *  if database required then import database server  
 */
		if ( db_check (&error) < 0 )
	   	{
	   		dev_printerror_no (SEND,"db_import failed",error);
	   		exit(DS_NOTOK);
	   	}		
/*
 *  check wether an old server with the same name
 *  is mapped to portmap or still running
 */
		if ( svc_check(&error) < 0 )
	   	{
	   		dev_printerror_no (SEND,"svc_check()",error);
	   		exit(DS_NOTOK);
	   	}		
/*
 * If the security system is switched on, read the minimal
 * access right for version 3 clients from the 
 * security database.
 */
		if ( config_flags->security == True )
		{
			default_access.length   = 0;
			default_access.sequence = NULL;

			strncpy (res_name, "default", sizeof(res_name));
			res_tab.resource_name = res_name;
			res_tab.resource_type = D_VAR_STRINGARR;
			res_tab.resource_adr  = &default_access;

			strncpy(res_path, "SEC/MINIMAL/ACC_RIGHT", sizeof(res_path));

			if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
			{
				dev_printerror_no (SEND, "db_getresource() get default security access right\n",error);
				exit (DS_NOTOK);
			}
/*
 * Transform the string array into an access right value.
 */
			if ( default_access.length > 0 )
			{
				for (i=0; i<SEC_LIST_LENGTH; i++)
				{
					if (strcmp (default_access.sequence[0], DevSec_List[i].access_name) == 0)
					{
						minimal_access = DevSec_List[i].access_right;
						break;
					}
				}
				if ( i == SEC_LIST_LENGTH )
					minimal_access = NO_ACCESS;
			}	 
			else 
				minimal_access = NO_ACCESS;

			free_var_str_array (&default_access);
		}
	}		
	else
	{
		prog_number = pn;
	}
/*
 * let portmapper choose port numbers for services 
 */
	udp_socket = RPC_ANYSOCK;
	tcp_socket = RPC_ANYSOCK;

/*
 *  create server handle and register to portmap
 *
 *  register udp port
 */
	transp = svcudp_create (udp_socket);
	if (transp == NULL) 
	{
		char msg[]="Cannot create udp service, exiting...\n";
		return error_msg(msg);
	}
/*
 *  make 3 tries to get transient progam number
 */
	synch_svc_udp_sock = -1;
	for (i = 0; i < 3; i++)
	{
		if (prog_number == 0)
			prog_number = gettransient(dsn_name);
		if( prog_number == 0 )
		{
	  		dev_printerror_no(SEND,"gettransient: no free programm nnumber\n",error);
	  		exit(DS_NOTOK);
		}
/*
 * Write the device server identification to the global
 * configuration structure.
 */
		config_flags->prog_number = prog_number;
		config_flags->vers_number = API_VERSION;


                if (!svc_register(transp, prog_number, API_VERSION, devserver_prog, IPPROTO_UDP))
		{
			char msg[]="Unable to register server (UDP,4), retry...\n"; 
			return error_msg(msg);
		}
		else
		{
/*
 * keep the socket, we need it later for dev_synch();
 */
			udp_socket = transp->xp_sock;
			synch_svc_udp_sock = transp->xp_sock;
			break;
		}
	}
	if (synch_svc_udp_sock == -1)
	{
		char msg[]="Unable to register server (UDP,4), exiting...\n"; 
		return error_msg(msg);
	}

/*
 *  register tcp port
 */
	transp_tcp = svctcp_create(tcp_socket,0,0);
	if (transp_tcp == NULL) 
	{
		char msg[]= "Cannot create tcp service, exiting...\n";
		return error_msg(msg);
	}

        if (!svc_register(transp_tcp, prog_number, API_VERSION, devserver_prog, IPPROTO_TCP))
	{
		char msg[]= "Unable to register server (TCP,4), exiting...\n";
		return error_msg(msg);
	}

/*
 * keep the socket, we need it later
 */
	tcp_socket = transp_tcp->xp_sock;
	synch_svc_tcp_sock = transp_tcp->xp_sock;
/*
 * if the process has got this far then it is a bona-fida device server set the appropiate flag
 */
	config_flags->device_server = True;
/*
 * startup message service 
 */
	if (m_opt ==True)
	{	
		display=getenv("DISPLAY");
		if(msg_import(dsn_name,host_name,prog_number,display,&error)!=DS_OK)
/* 
 * we dont care 
 */
			fprintf(stderr, "can not import message service\n");
	}
/*
 * Register the asynchronous rpc service so that the device server can receive asynchronous calls 
 * from clients. The asynchronous calls are sent as batched tcp requests without wait. The server
 * will return the results to the client asynchronously using batched TCP.
 */
	status = asynch_rpc_register(&error);
	if (status != DS_OK)
	{
		dev_printerror_no (SEND,"failed to register asynchronus rpc",error);
	}
/* 
 * DO NOT abort server, continue (without asynchronous server) ...
 * startup device server
 */
	if (s_opt == True)
	{
/*
 * Set the startup configuration flag to SERVER_STARTUP
 * during the startup phase.
 */
	    	config_flags->startup = SERVER_STARTUP;
		status = startup(config_flags->server_name, &error);
		if ( status != DS_OK )
		{
			dev_printerror_no (SEND,"startup failed",error);
			return (DS_NOTOK);
		}		
/*
 *  if ds__svcrun() is used, the server can return from
 *  the startup function with status=1 to avoid svc_run()
 *  and to do a proper exit.
 */
		if ( status == 1 )
			return (DS_NOTOK);
		multi_nethost[0].config_flags.startup = True;
	}

#ifndef WIN32
/*
 *  set server into wait status
 */
	svc_run();
		
	fprintf (stderr, "svc_run returned\n"); 
	return DS_NOTOK;
#else   /* WIN32 */
/*
 * show up the main dialog
 */
	return TRUE;
#endif
}

/**
 * @ingroup dsAPIintern
 * Entry point for received RPCs. Switches to the wished remote procedure.
 * 
 * @param rqstp   RPC request handle
 * @param transp  Service transport handle
 *
 */
static void _WINAPI devserver_prog_4 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	char	*help_ptr;
	int	pid = 0;

	union {
		_dev_import_in	rpc_dev_import_4_arg;
		_dev_free_in 	rpc_dev_free_4_arg;
		_server_data 	rpc_dev_putget_4_arg;
		_server_data 	rpc_dev_put_4_arg;
                _dev_query_in   rpc_dev_query_cmd_4_arg;
/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *		long 		rpc_admin_import_4_arg;
 *		_server_admin 	rpc_admin_4_arg;
 */
		_server_data 	rpc_dev_putget_raw_4_arg;
		_server_data 	rpc_dev_put_asyn_4_arg;
	} argument;

	char *result;
	xdrproc_t xdr_argument,xdr_result;
#ifdef __cplusplus
	DevRpcLocalFunc local;
#else
	char *(*local)();
#endif

/*
 *  call the right server routine
 */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "devserver_prog_4(): called with rqstp->rq_proc %d\n", rqstp->rq_proc);
	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
			return;

        	case RPC_QUIT_SERVER:
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
#if defined (WIN32)
                	raise(SIGABRT);
#else  /* WIN32 */
#if !defined (vxworks)
                	pid = getpid ();
#else  /* !vxworks */
                	pid = taskIdSelf ();
#endif /* !vxworks */
                	kill (pid,SIGQUIT);
#endif /* WIN32 */
			return;

		case RPC_CHECK:
			help_ptr = &(config_flags->server_name[0]);
			svc_sendreply (transp, (xdrproc_t)xdr_wrapstring, (caddr_t) &help_ptr);
			return;

		case RPC_DEV_IMPORT:
			xdr_argument = (xdrproc_t)xdr__dev_import_in;
			xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_import_4;
#else
			local = (char *(*)()) rpc_dev_import_4;
#endif
			break;

		case RPC_DEV_FREE:
			xdr_argument = (xdrproc_t)xdr__dev_free_in;
			xdr_result = (xdrproc_t)xdr__dev_free_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_free_4;
#else
			local = (char *(*)()) rpc_dev_free_4;
#endif
			break;

		case RPC_DEV_PUTGET:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_4;
#else
			local = (char *(*)()) rpc_dev_putget_4;
#endif
			break;

		case RPC_DEV_PUT:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_4;
#else
			local = (char *(*)()) rpc_dev_put_4;
#endif
			break;

		case RPC_DEV_CMD_QUERY:
			xdr_argument = (xdrproc_t)xdr__dev_query_in;
			xdr_result = (xdrproc_t)xdr__dev_query_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_cmd_query_4;
#else
			local = (char *(*)()) rpc_dev_cmd_query_4;
#endif
			break;
/* event query */
		case RPC_DEV_EVENT_QUERY:
        		xdr_argument = (xdrproc_t)xdr__dev_query_in;
        		xdr_result = (xdrproc_t)xdr__dev_queryevent_out;
#ifdef __cplusplus
        		local = (DevRpcLocalFunc) rpc_dev_event_query_4;
#else
        		local = (char *(*)()) rpc_dev_event_query_4;
#endif
        		break;
/* end event query */

/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *		case RPC_ADMIN_IMPORT:
 *			xdr_argument = xdr_DevLong;
 *			xdr_result = xdr_DevLong;
 *			local = (char *(*)()) rpc_admin_import_4;
 *			break;
 *
 *		case RPC_ADMIN:
 *			xdr_argument = (xdrproc_t)xdr__server_admin;
 *			xdr_result = xdr_DevLong;
 *			local = (char *(*)()) rpc_admin_4;
 *			break;
 */
		case RPC_DEV_PUTGET_RAW:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_raw_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_raw_4;
#else
			local = (char *(*)()) rpc_dev_putget_raw_4;
#endif
			break;

		case RPC_DEV_PUT_ASYN:
/*
 * Read incoming arguments and send the reply immediately without waiting
 * the execution of the function.
 */
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
/*
 * Function only for the adminstration and security part of the asynchronous call.
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_asyn_4;
#else
			local = (char *(*)()) rpc_dev_put_asyn_4;
#endif
	        	break;

		case RPC_DEV_PING:
			xdr_argument = (xdrproc_t)xdr__dev_import_in;
			xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_ping_4;
#else
			local = (char *(*)()) rpc_dev_ping_4;
#endif
			break;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND, "svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call (*local) \n");
	result = (*local)(&argument, rqstp);
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call (*local)() %s\n", (result == NULL ? "failed" : "ok"));

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_sendreply()\n");
	if (result != NULL && !svc_sendreply(transp, xdr_result, (caddr_t)result)) 
	{
		dev_printerror (SEND, "svcerr_systemerr : server couldn't send reply arguments");
		svcerr_systemerr(transp);
	}
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_sendreply() ok\n");

/*
 * If an asynchronous call was requested, execute now the command. After the answer was already send back to 
 * the client.
 */
	if ( rqstp->rq_proc == RPC_DEV_PUT_ASYN )
	{
		rpc_dev_put_asyn_cmd ((_server_data *)&argument);
	}

/*
 * If dev_free() was called AND the server is on OS9 AND tcp then give OS9 a hand in closing this end of 
 * the tcp socket. This fixes a bug in the OS9 (>3.x) implementation which led to the server blocking for 
 * a few seconds when closing a tcp connection.
 */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_freeargs()\n");
	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND, "svc_freeargs : server couldn't free arguments !!");
	}
	return;
}

/**
 * @ingroup dsAPIintern
 * Checks wether a database server is available.
 * 
 * @param error Will contain an appropriate error code if the corresponding 
 *              call returns a non-zero value.
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long db_check (DevLong *error)
{
	return db_import(error);
}

/**
 * @ingroup dsAPIintern
 * Checks wether a device server with the same name is already running.
 * 
 * @param error Will contain an appropriate error code if the corresponding 
 *              call returns a non-zero value.
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long svc_check (DevLong *error)
{
        CLIENT          *clnt;
	enum clnt_stat  clnt_stat;
	char		*host_name;
	char		*svc_name = NULL;
	unsigned int	prog_number;
	unsigned int	vers_number;

	*error = 0;

	if ( db_svc_check (config_flags->server_name,
			   &host_name, &prog_number, &vers_number, error) < 0 )
		return (DS_NOTOK);

/*
 * old server already unmapped ?
 */
	if ( prog_number == 0 )
		return (DS_OK);

/*
 *  was the old server running on the same host ?
 *
 *  DON'T - removed this check it is an OS9 anachronism (andy 2/5/2000)
 *
 *	if (strcmp (config_flags->server_host,host_name) != 0)
 *	{
 *		*error = DevErr_ServerRegisteredOnDifferentHost;
 *		return (DS_NOTOK);
 *	}
 */

/*
 * force version number to latest API_VERSION [4] , new servers do not
 * support the old DSERVER_VERSION [1] anymore
 */
	vers_number = API_VERSION;

/*
 *  old server still exists ?
 */
	clnt = clnt_create (host_name, prog_number,vers_number,"udp");
	if (clnt != NULL)
	{
		clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &msg_retry_timeout);
	        clnt_control (clnt, CLSET_TIMEOUT, (char *) &msg_timeout);

/*
 *  call device server check function
 */
		clnt_stat = clnt_call (clnt, RPC_CHECK, (xdrproc_t)xdr_void, NULL,
     				  (xdrproc_t)xdr_wrapstring, (caddr_t) &svc_name, 
			          TIMEVAL(msg_timeout));
		if (clnt_stat == RPC_SUCCESS)
		{
			if (strcmp (config_flags->server_name, svc_name) == 0)
			{
				*error = DevErr_ServerAlreadyExists;
				clnt_destroy (clnt);
				return (DS_NOTOK);
			}
		}
		else if (clnt_stat != RPC_PROCUNAVAIL)
		{
			/*pmap_unset (prog_number, DEVSERVER_VERS);*/
			pmap_unset (prog_number, API_VERSION);
			pmap_unset (prog_number, ASYNCH_API_VERSION);
		}
		clnt_destroy (clnt);
	}
	return (DS_OK);
}

#ifdef _UCC
extern int ds_rpc_svc_fd; /* global variable - client rpc file descriptor */
#define getdtablesize() FD_SETSIZE
#ifdef __STDC__
void svc_run(void)
#else
void svc_run()
/**
 * @ingroup dsAPIintern
 * OS9 function to wait forever for rpc requests to a server.  When a request 
 * arrives it is dispatched to the server via the rpc call svc_getreqset(). 
 *
 * This version specific to OS9 has been modelled on the example in the "OS9 
 * Network File System/Remote Procedure Call User's Manual" pg 2-21 modified by 
 * P.Makijarvi. 
 *
 * It solves the problem of exiting on receiving a signal.
 */
#endif /* __STDC__ */
{
	fd_set readfds;
	int dtbsz = getdtablesize();
	long i;

	FD_ZERO(&readfds);

/*	printf("svc_run(): arrived ...\n");*/
	
	for (;;)
	{
		readfds = svc_fdset;

		/*printf("svc_run(): readfds 0x%08x before select()\n",readfds);*/
		switch (select(dtbsz,&readfds,(fd_set*)0,(fd_set*)0,(struct timeval*)0))
		{
			case -1 : if ((errno == EINTR) || (errno == EOS_SIGNAL))
				  /*perror("svc_run(): select() returns with error");*/
			 	  continue;

				  /*return;*/

			case 0 : break;

			default : 
/*
 * determine which file descriptor the client has called from, this is
 * needed by OS9 in case it has to close the socket e.g. if dev_free()
 */
				  for (i=0; i<FD_SETSIZE; i++)
				  {
				  	if (FD_ISSET(i,&readfds)) ds_rpc_svc_fd = i;
				  }

				  /*printf("svc_run(): readfds 0x%08x after select(), ds_rpc_svc_fd=%d\n",readfds,ds_rpc_svc_fd);*/
				  svc_getreqset(&readfds);
		}

	}
}
#endif /* _UCC */
