/*+*******************************************************************

 File:		svc_api.c

 Project:	Device Servers with SUN-RPC

 Description:	Server side of the API.  

 Author(s);	Jens Meyer
 		$Author: jkrueger1 $

 Original:	Feb 1994

 Version:	$Revision: 1.10 $

 Date:		$Date: 2004-03-09 17:15:50 $

 Copyright (c) 1990-1997 by European Synchrotron Radiation Facility, 
                           Grenoble, France

********************************************************************-*/
#include <config.h>
/*
 * Include files and Static Routine definitions
 */

/*
 * C++ version
 */
#ifdef __cplusplus
#ifdef _UCC
#include <types.h>
#else
#include <sys/types.h>
#endif
#endif /* __cplusplus */

#include <API.h>
#include <private/ApiP.h>
#include <DevCmds.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>


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
extern "C" configuration_flags      config_flags;
#else
extern configuration_flags      config_flags;
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
 * Dynamic error stack
 */
extern char *dev_error_stack;

/*
 * Internal Functions
 */

static long read_device_id 	PT_( (long device_id, long *ds_id,
long *connection_id, long *error) );

/**
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

	long 		device_id;
	register int 	i;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_import_4() : entering routine\n");

	device_id = 0;
	dev_import_out.ds_id  = 0;
	dev_import_out.status = DS_OK;
	dev_import_out.error  = DS_OK;
	snprintf (dev_import_out.server_name, sizeof(dev_import_out.server_name), "%s", config_flags.server_name);

	dev_import_out.var_argument.length   = 0;
	dev_import_out.var_argument.sequence = NULL;

	/*
 * first try to to find the device among the list of
 * devices already being served 
 *
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
	     * Do the security checks and initialization for the 
	     * device import.
	     */

				if ( config_flags.security == True )
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
	     * Combine device ID, connection ID and the export count as an
	     * identifier for the client.
	     */
				device_id = (devices[i].export_counter << COUNT_SHIFT) +
				    (dev_import_in->connection_id << CONNECTIONS_SHIFT) + 
				    i;

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


/**
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
	static _dev_free_out dev_free_out;
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

	if (read_device_id (dev_free_in->ds_id, &ds_id, &connection_id, 
	    &dev_free_out.error) == DS_NOTOK)
	{
		dev_free_out.status = DS_NOTOK;
		return (&dev_free_out);
	}

	/*
    * Clean up the security structure for the client connection.
    */

	if ( config_flags.security == True )
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

/**
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
	DeviceBase		*device;
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
        if (dev_error_stack != NULL) free(dev_error_stack);
	dev_error_stack = NULL;



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

	if ( config_flags.security == True )
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
                                             (void*)server_data->argin,
                                             server_data->argin_type,
                                             (void*)client_data.argout,
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


/**
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
	DeviceBase		*device;
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
        if (dev_error_stack != NULL) free(dev_error_stack);
	dev_error_stack = NULL;


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

	if ( config_flags.security == True )
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


/**
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
	DeviceBase			*device;
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

	if ( config_flags.security == True )
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



/**
 * The first part of the aynchronous call.
 *
 * Will do the administration part of the call.
 * Only the execution of the command is excluded
 * and handled in the second part of the call
 * (rpc_dev_put_asyn_cmd) after the reponse
 * is send back to the client.
 *
 * Errors are returned from this function to the client. 
 *
 * @param server_data 	information about device identification, the command, and
 *			the input arguments for the command.
 *
 * @return(s)  	the status of the function (DS_OK or DS_NOTOK) and
 *		an appropriate error number if the function fails.
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

	if ( config_flags.security == True )
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

/**
 * The second part of the asynchronous call.
 *
 * The command will be executed without any return values. 
 *
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
	DeviceBase		*device;
#endif /* __cplusplus */
	long			ds_id;
	long 			connection_id;
	long			error;

	error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_cmd() : entering routine\n");
	dev_printdebug ( DBG_DEV_SVR_CLASS, "\nrpc_dev_put_asyn_cmd() : with ds_id = %d\n", server_data->ds_id);

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
 * @return  DS_OK or DS_NOTOK
 */
#ifndef __cplusplus
/*
 * OIC version
 */
long dev_export (char *name, void *ptr_ds, long *error)
#else
/*
 * C++ version
 */
long dev_export (char *name, DeviceBase *ptr_dev, long *error)
#endif
{
	db_devinf		devinfo;
	DevServerDevices 	*new_devices;
	char			*strptr,
				proc_name[80],
				server_host[80];
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
	DeviceBase	*device;
	device =        ptr_dev;
#endif

	*error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\ndev_export() : entering routine\n");

/*
 * If no space is yet allocated in the global device array,
 * allocate the first block of devices.
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
 * used to export a device of the specified name 
 * to the outside. ds points to the space allocated 
 * for the device. the next free entry will be
 * found in the global device[] array and it will
 * be reserved for the device. this array is accessed
 * by the dev_import function to determine what devices
 * are known to the server.
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
 * If all allocated devices are already used, allocate
 * a new block of devices.
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
	TOLOWER(name)

/* 
 * do not export the device to the static database if
 * no_database is selected ...
 */
	if (!config_flags.no_database)
	{
/*
 *  export device to the static database.
 *  device server configuration for this device
 *  will be stored in the static database
 */
		devinfo.device_name = name;
		strcpy(server_host, config_flags.server_host);
		devinfo.host_name   = server_host;
		devinfo.pn	    = (u_int)config_flags.prog_number;
		devinfo.vn          = (u_int)config_flags.vers_number;
/*
 * Extract the name of the executable
 */
		snprintf (proc_name, sizeof(proc_name), "%s", config_flags.server_name);
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
 *  check whether the pointer to the device type is
 *  already initialised
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
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "dev_export() : leaving routine\n");
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
 * @return DS_OK or DS_NOTOK
 */
long ds__destroy (void *ptr_ds, long *error)
{
	short	i;

#ifndef __cplusplus
	DevServer 	ds;

	ds       = (DevServer) ptr_ds;
#else
	DeviceBase 		*device = (DeviceBase*) ptr_ds;

#endif /* __cplusplus */

	*error = DS_OK;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nds__destroy() : entering routine\n");

/*
 * If the object was exported, delete the entry in the
 * list of exported devices before freeing the object.
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
 * Retrieves all available information from the command list of the 
 * specified device.
 *
 * Information about one command is stored in a _dev_cmd_info structure 
 * defined in API_xdr.h.
 *
 * A sequence of these stuctures will be returned as well as the name of 
 * the device class.
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
	static _dev_query_out	dev_query_out = { 0, NULL, "", 0, 0, { 0, NULL}};
	long        		ds_id;
	long        		connection_id;
	static DevVarArgument 	vararg[1024];
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer		ds;
	DevCommandList		ds_cl;
	DevServerClass		ds_class;
	_Int			i;
#else
/*
 * C++ version
 */
	DeviceBase	*device;
	long 			error;
	long 			ret;
	int			i;
#endif

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nrpc_dev_cmd_query_4() : entering routine\n");

	dev_query_out.error  = DS_OK;
	dev_query_out.status = DS_OK;

	dev_query_out.var_argument.length   = 0;
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
	ds_cl    = ds_class->devserver_class.commands_list;
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
 * Get number of implmented commands and allocate memory
 * for the command info sequence.
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
 * Get information about command code, data types and class for
 * every implented command.
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
 * if the device server command list has specified the command names
 * (by testing the first entry != NULL)
 * then tag them onto the end of the sequence so that the client
 * does not have to query the database for them (very useful if -nodb)
 *
 * andy 3/3/2002
 */

#ifndef __cplusplus
	if (ds_cl[0].cmd_name != NULL)
	{
        	dev_query_out.var_argument.length = 0;
		for (i=0; (u_long)i<dev_query_out.length; i++)
		{
        		vararg[i].argument_type      = D_STRING_TYPE;
        		vararg[i].argument           = (DevArgument)&ds_cl[i].cmd_name;
		}
        	dev_query_out.var_argument.length = dev_query_out.length;
        	dev_query_out.var_argument.sequence = vararg;
	}
#else
	if ((device->commands_list.begin())->second.cmd_name != NULL)
	{
		dev_query_out.var_argument.length = 0;
		for (i=0; (u_long)i<dev_query_out.length; i++)
		{
			vararg[i].argument_type      = D_STRING_TYPE;
			vararg[i].argument           = (DevArgument)&(device->commands_list[dev_query_out.sequence[i].cmd].cmd_name);
		}
	        dev_query_out.var_argument.length = dev_query_out.length;
	        dev_query_out.var_argument.sequence = vararg;
	}
#endif /* __cplusplus */
	return (&dev_query_out);
}

/* event query */
/**@ingroup dsAPI
 *
 * @param dev_query_in
 * @return
 */
_dev_queryevent_out * _DLLFunc rpc_dev_event_query_4 (_dev_query_in *dev_query_in)
{
	static _dev_queryevent_out	dev_query_out = { 0, NULL, "", 0, 0, { 0, NULL}};
	long        			ds_id;
	long        			connection_id;
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
	DeviceBase			*device;
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
 * Get number of implmented commands and allocate memory
 * for the command info sequence.
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
 * Get information about event code, data types and class for
 * every implented command.
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
 * @return DS_OK or DS_NOTOK
 *
 */
static long read_device_id (long device_id, long *ds_id, long *connection_id, long *error)
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
	DeviceBase		*device;
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

	if ( config_flags.security == True )
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
 * in the simple case the command is passed directly on to the command_handler
 * method
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
	snprintf (dev_import_out.server_name, sizeof(dev_import_out.server_name), "%s", config_flags.server_name);

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

