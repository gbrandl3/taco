/*+*******************************************************************

 File       :	util_api.c

 Project    :	Device Servers with SUN-RPC

 Description:	Application Programmers Interface 
		
            :	Utilities for the interface to access and 
		handle remote devices.

 Author(s)  :	Jens Meyer
 		$Author: jkrueger1 $

 Original   :	April 1993

 Version:	$Revision: 1.5 $

 Date:		$Date: 2003-05-09 06:33:49 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

********************************************************************-*/
#include <config.h>
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <API_xdr_vers3.h>

#include <Admin.h>
#include <DevErrors.h>

#if !defined _NT
#if  ( OSK | _OSK )
#include <inet/socket.h>
#include <inet/netdb.h>
#else
#ifdef sun
#include <sys/filio.h>
#endif /* sun */
#include <sys/socket.h>
#if !defined vxworks
#include <netdb.h>
#endif /* !vxworks */
#endif /* OSK | _OSK */
#endif  /* _NT */

/* pointer to global error message */
extern char *dev_error_string;

static long get_cmd_string PT_( (devserver ds, long cmd, char *cmd_str, long *error) );


/****************************************
 *          Globals	                *
 ****************************************/

/*
 *  Types for global state flags for the current
 *  server or client status and for Database Server
 *  information are defined in API.h
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
 * Global structure for multiple control systems, setup by
 * setup_config_multi() but used by all multi-nethost functions
 */

#ifdef __cplusplus
extern "C" nethost_info *multi_nethost;
#else
extern nethost_info *multi_nethost;
#endif

#ifdef __cplusplus
extern "C" long  max_nethost;
#else
extern long  max_nethost;
#endif

/*  
 *  Structure for the adnministration of open RPC connections.
 */

extern server_connections	svr_conns [];


/*
 *  Table of command CLASS resources, read for dev_cmd_query()
 */

static db_resource   res_tab [] = {
	{(char *)"In_Type",  D_STRING_TYPE, NULL},
	{(char *)"Out_Type", D_STRING_TYPE, NULL},
};


/**
 * execute commands on a device by passing input data and retrieving 
 * output data in a raw opaque format.
 *
 * @param ds 		handle to access the device.
 * @param cmd   	command to be executed.
 * @param argin 	pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 *
 * @param argout    	pointer to DevOpaque structure (contains the result).
 * @param argout_type 	data type of output arguments, as specified for the command.
 * @param error         Will contain an appropriate error code if the corresponding call 
 *			returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_putget_raw (devserver ds, long cmd, DevArgument argin,
			      DevType argin_type, DevOpaque *argout,
			      DevType argout_type, long *error)
{
	_server_data		server_data;
	_client_raw_data	client_data;
	enum clnt_stat		clnt_stat;
	long			client_id = 0;
	long 			status;

	*error = DS_OK;
#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\ndev_putget_raw() : entering routine\n");
#endif /* EBUG */

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
	if (dev_error_string != NULL)
	{
		free(dev_error_string);
		dev_error_string = NULL;
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
                status = tango_dev_putget_raw(ds, cmd, argin, argin_type,
                                              argout, argout_type, error);
		return(status);
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

	/*
         * Verify the RPC connection.
         */

	if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

	/*
	 * If the security system is configured, 
	 * verify the security key
	 */

	if ( config_flags.security == True )
	{
		if ( verify_sec_key (ds, &client_id, error) == DS_NOTOK )
		{
			return (DS_NOTOK);
		}
	}

	/*
	 *  fill in data to rpc transfer structures server_data
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

#ifdef EBUG
	dev_printdebug (DBG_API,
	    "dev_putget() : server data -> \n");
	dev_printdebug (DBG_API,
	    "ds_id=%d  cmd=%d  intype=%d  outtype=%d\n",
	    server_data.ds_id, server_data.cmd,
	    server_data.argin_type,
	    server_data.argout_type);
#endif /* EBUG */

	/*
	 *  call RPC_DEV_PUTGET_RAW at the correct device server
	 */

	/*
	 * Call a device with the current version number >1.
   	 */
	if ( ds->vers_number > DEVSERVER_VERS)
	{
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUTGET_RAW,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_raw_data, (caddr_t) &client_data, TIMEVAL(timeout));
	}
	else
	{
		/*
	    * Call a device from an old version server.
	    */
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUTGET_RAW,
		    (xdrproc_t)xdr__server_data_3, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_raw_data_3, (caddr_t) &client_data, TIMEVAL(timeout));
	}


	/*
         * Check for errors on the RPC connection.
         */

	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

	/*
         * Free the variable arguments in the client_raw_data
         * structure, coming from the server.
         */

	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray,
	    (char *)&(client_data.var_argument));


	/*
	 * return error code and status from device server
	 */

	argout->length =(u_int) client_data.xdr_length;

	*error = client_data.error;
	return (client_data.status);
}


/**
 * application interface to execute commands on a device
 * with only the possibility to pass input data. 
 * The function will return before the command
 * is executed. No errors or status of the command
 * will be returned.
 * 
 * @param ds       	handle to access the device.
 * @param cmd           command to be executed.
 * @param argin  	pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 * @param error         Will contain an appropriate error 
 * 			code if the corresponding call 
 * 			returns a non-zero value.
 * 
 * @return	DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_put_asyn (devserver ds, long cmd, DevArgument argin,
			    DevType argin_type, long *error )
{
	_server_data		server_data;
	_client_data     	client_data;
	enum clnt_stat		clnt_stat;
	long			client_id = 0;

	*error = DS_OK;

#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\ndev_put_asyn() : entering routine\n");
#endif /* EBUG */

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
/* TODO - implement dev_put_asyn() */
		*error = DevErr_CommandNotImplemented;
		return(DS_NOTOK);
	}
#endif /* TANGO */
	/*
         * Verify the RPC connection.
         */

	if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

	/*
	 * If the security system is configured, 
	 * verify the security key
	 */

	if ( config_flags.security == True )
	{
		if ( verify_sec_key (ds, &client_id, error) == DS_NOTOK )
		{
			return (DS_NOTOK);
		}
	}

	/*
	 *  fill in data to rpc transfer structures server_data
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

#ifdef EBUG
	dev_printdebug (DBG_API,
	    "dev_put_asyn() : server data -> \n");
	dev_printdebug (DBG_API,
	    "ds_id=%d  cmd=%d  intype=%d\n",
	    server_data.ds_id, server_data.cmd,
	    server_data.argin_type);
#endif /* EBUG */

	/*
	 *  call RPC_DEV_PUT_ASYN at the correct device server
	 */

	/*
	 * Call a device with the current version number >1.
   	 */
	if ( ds->vers_number > DEVSERVER_VERS)
	{
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUT_ASYN,
		    (xdrproc_t)xdr__server_data, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_data, (caddr_t) &client_data, TIMEVAL(timeout));
	}
	else
	{
		/*
	    * Call a device from an old version server.
	    */
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PUT_ASYN,
		    (xdrproc_t)xdr__server_data_3, (caddr_t) &server_data,
		    (xdrproc_t)xdr__client_data_3, (caddr_t) &client_data, TIMEVAL(timeout));
	}

	/*
         * Check for errors on the RPC connection.
         */

	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

	/*
         * Free the variable arguments in the client_data
         * structure, coming from the server.
         */

	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray,
	    (char *)&(client_data.var_argument));

	*error = client_data.error;
	return (client_data.status);
}


/**
 * Returns a sequence of structures containig all
 * available commands, their names, their input and
 * output data types, and type describtions for one
 * device.
 *
 * Commands and data types are read from the command
 * list in the device server by calling 
 * RPC_DEV_CMD_QUERY.
 *
 * Command names are read from the command name list, defined in DevCmds.h. 
 *
 * Data type describtions have to be specified as CLASS resources as:
 * 	- CLASS/class_name/cmd_name/IN_TYPE:
 * 	- CLASS/class_name/cmd_name/OUT_TYPE:
 *
 * @param ds 		client handle for the associated device.
 * @param varcmdarr 	sequence of DevCmdInfo structures.
 * @param error     	Will contain an appropriate error code if the
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_cmd_query (devserver ds, DevVarCmdArray *varcmdarr, long *error)
{
	_dev_query_in	dev_query_in;
	_dev_query_out	dev_query_out;
	enum clnt_stat  clnt_stat;
	char		class_name[SHORT_NAME_SIZE];
	char		cmd_name[SHORT_NAME_SIZE];
	long		length;
	long		ret_stat;
	char		res_path [LONG_NAME_SIZE];
	int		i;
	u_int 		res_tab_size = sizeof(res_tab) / sizeof(db_resource);
	long		status;
	static char	**cmd_names=NULL;
	int		n_cmd_names;

	*error = DS_OK;
#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\ndev_cmd_query() : entering routine\n");
#endif /* EBUG */

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
		status = tango_dev_cmd_query(ds, varcmdarr, error);
		return(status);
	}
#endif /* TANGO */
	/*
         * Verify the RPC connection.
         */

	if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
	{
		return (DS_NOTOK);
	}


	/*
         *  fill in data transfer structures dev_query_in
         *  and dev_query_out.
         */

	dev_query_in.ds_id = ds->ds_id;

	dev_query_in.var_argument.length   = 0;
	dev_query_in.var_argument.sequence = NULL;


/*
 *  Call the rpc entry point RPC_DEV_CMD_QUERY at the specified device server.
 */

	memset ((char *)&dev_query_out, 0, sizeof (dev_query_out));

/*
 * Query a device with the current version number >1.
 */
	if ( ds->vers_number > DEVSERVER_VERS)
	{
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_CMD_QUERY,
		    (xdrproc_t)xdr__dev_query_in,  (caddr_t) &dev_query_in,
		    (xdrproc_t)xdr__dev_query_out, (caddr_t) &dev_query_out, TIMEVAL(timeout));
	}
	else
	{
/*
 * Query a device from an old version server.
 */
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_CMD_QUERY,
		    (xdrproc_t)xdr__dev_query_in_3,  (caddr_t) &dev_query_in,
		    (xdrproc_t)xdr__dev_query_out_3, (caddr_t) &dev_query_out, TIMEVAL(timeout));
	}


/*
 * Check for errors on the RPC connection.
 */

	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
	{
		return (DS_NOTOK);
	}

/*
 * Free the variable arguments in the dev_query_out
 * structure, coming from the server.
 */
	if (dev_query_out.var_argument.length > 0)
	{
		cmd_names = (char**)realloc(cmd_names,dev_query_out.var_argument.length*sizeof(char*));
		for (i=0; i<dev_query_out.var_argument.length; i++)
		{
			cmd_names[i] = (char*)malloc(strlen(*(char**)dev_query_out.var_argument.sequence[i].argument)+1);
			sprintf(cmd_names[i],"%s",*(char**)dev_query_out.var_argument.sequence[i].argument);
		}
	}
	n_cmd_names = dev_query_out.var_argument.length;
			
	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray,
	    (char *)&(dev_query_out.var_argument));


	/*
 * Allocate memory for a sequence of DevCmdInfo structures
 * returned with varcmdarr.
 */

	varcmdarr->length   = dev_query_out.length;
	varcmdarr->sequence = (DevCmdInfo *) malloc
	    (varcmdarr->length * sizeof (DevCmdInfo));
	if ( varcmdarr->sequence == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}
	memset ((char *)varcmdarr->sequence, 0,
	    (varcmdarr->length * sizeof (DevCmdInfo)));

	/*
 * Now get command and types name strings for the returned
 * command sequence. Command names are retrieved from the
 * global command-name-list and name strings for the data types
 * are searched in the resource CLASS table of the object class.
 * 
 * Undefined names will be initialised with NULL.
 */

	for ( i=0; (u_long)i<varcmdarr->length; i++ )
	{
/*
 * initialise varcmdarr->sequence[i] with command and
 * argument types, returned with dev_query_out from the
 * device servers command list.
 */
		varcmdarr->sequence[i].cmd      = dev_query_out.sequence[i].cmd;
		varcmdarr->sequence[i].in_type  = dev_query_out.sequence[i].in_type;
		varcmdarr->sequence[i].out_type = dev_query_out.sequence[i].out_type;

/*
 * get command name string from the resource database
 */


/* 
 * check to see if device server returned command names 
 */
		if (i < n_cmd_names && n_cmd_names > 0)
		{
			sprintf(varcmdarr->sequence[i].cmd_name,"%s",cmd_names[i]);
			free(cmd_names[i]);
		}
		else
		{
			if (!ds->no_database)
			{
				if ( (ret_stat = get_cmd_string (ds, varcmdarr->sequence[i].cmd, 
		    		varcmdarr->sequence[i].cmd_name, 
		    		error )) == DS_NOTOK )
				{
/*
 * An error will be only returned if the database
 * access fails.
 */
					return (DS_NOTOK);
				}
			}
			else
			{
				sprintf(varcmdarr->sequence[i].cmd_name,"command%d",i);
			}
		}
		if (!ds->no_database)
		{


/*
 *  Check wether command name was found.
 *  If the name was not found, get_cmd_string() returns
 *  DS_WARNING.
 */

			if ( ret_stat != DS_WARNING )
			{
/*
 * Limit the class_name and the command_name
 * strings to 19 characters. This is the limit
 * of the static database name fields.
 */

				length = strlen (dev_query_out.class_name);
				if ( length > MAX_RESOURCE_FIELD_LENGTH )
					length = MAX_RESOURCE_FIELD_LENGTH;
				strncpy (class_name, dev_query_out.class_name, 
			    	MAX_RESOURCE_FIELD_LENGTH);
				class_name[(_Int)length] = '\0';

				length = strlen (varcmdarr->sequence[i].cmd_name);
				if ( length > MAX_RESOURCE_FIELD_LENGTH )
					length = MAX_RESOURCE_FIELD_LENGTH;
				strncpy (cmd_name, varcmdarr->sequence[i].cmd_name,
			    	MAX_RESOURCE_FIELD_LENGTH);
				cmd_name[(_Int)length] = '\0';
			}

/*
 *  setup resource path to read information about
 *  data types from the CLASS resource table.
 *
 * but first check to see whether the device belongs to another
 * nethost domain i.e. i_nethost != 0
 */

			if (ds->i_nethost > 0)
			{
				sprintf(res_path, "//%s/CLASS/%s/%s",
		   		get_nethost_by_index(ds->i_nethost, error),
		   		class_name, cmd_name);
			}
/*
 * use default nethost
 */
			else
			{
				sprintf (res_path, "CLASS/%s/%s", 
		   		class_name, cmd_name);
			}

			/*
	       *  read CLASS resources from database
	       */

			res_tab[0].resource_adr = &(varcmdarr->sequence[i].in_name);
			res_tab[1].resource_adr = &(varcmdarr->sequence[i].out_name);

			if (db_getresource (res_path, res_tab, res_tab_size, error) < 0)
			{
				return (DS_NOTOK);
			}
		}
/*
 * no database, set in_name and out_name to NULL
 */
		else
		{
			varcmdarr->sequence[i].in_name = NULL;
			varcmdarr->sequence[i].out_name = NULL;
		}
	}

	/*
	 *  free dev_query_out 
	 */

	xdr_free ((xdrproc_t)xdr__dev_query_out,
	    (char *)&dev_query_out);

	/*
 * Return error code and status from device server.
 */

	*error = dev_query_out.error;
	return (dev_query_out.status);
}


/**
 * Read the command name as a string from the resource database.
 * The resource name is:
 * CMD/team_no/server_no/cmd_ident
 * 
 * @param cmd       	error number
 * @param cmd_str  	command name as a string.
 * @param error   	Will contain an appropriate error
 * 			code if the corresponding call
 * 			returns a non-zero value.
 * 
 * @return	DS_OK or DS_NOTOK or DS_WARNING
 * 		DS_WARNING is returned, if the function was
 * 		executed correctly, but no command name
 * 		string was found in the database.
 */ 
static long get_cmd_string (devserver ds, long cmd, char *cmd_str, long *error)
{
	char		res_path[LONG_NAME_SIZE];
	char		res_name[SHORT_NAME_SIZE];
	char		*ret_str = NULL;
	db_resource 	res_tab;
	unsigned long 	cmd_number_mask = 0x3ffff;
	unsigned short 	team;
	unsigned short 	server;
	unsigned short 	cmds_ident;

#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\nget_cmd_string() : entering routine\n");
#endif /* EBUG */

	*error = DS_OK;

	/*
 * Decode the command nuber into the fields:
 * team, server and cmds_ident.
 */
	team   = (_Int)(cmd >> DS_TEAM_SHIFT);
	team   = team & DS_TEAM_MASK;
	server = (_Int)(cmd >> DS_IDENT_SHIFT);
	server = server & DS_IDENT_MASK;
	cmds_ident = (_Int)(cmd & cmd_number_mask);

	/*
 * Create the resource path and the resource structure.
 *
 * first check to see whether the device belongs to another
 * nethost domain i.e. i_nethost != 0
 */

	if (ds->i_nethost > 0)
	{
		sprintf(res_path, "//%s/CMDS/%d/%d", 
		   get_nethost_by_index(ds->i_nethost, error),
		   team, server);
	}
/*
 * use default nethost
 */
	else
	{
		sprintf(res_path, "CMDS/%d/%d", team, server);
	}

	sprintf (res_name, "%d", cmds_ident);
#ifdef EBUG
	dev_printdebug (DBG_API,
	    "get_cmds_string() : res_path = %s\n", res_path);
	dev_printdebug (DBG_API,
	    "get_cmds_string() : res_name = %s\n", res_name);
#endif /* EBUG */

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

	/*
 * Read the command name string from the database.
 */

	if (db_getresource (res_path, &res_tab, 1, error) == DS_NOTOK)
	{
#ifdef EBUG
		dev_printdebug (DBG_API | DBG_ERROR,
		    "get_cmd_string() : db_getresource failed with error %d\n", *error);
#endif /* EBUG */

		return (DS_NOTOK);
	}

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
	{
		return (DS_WARNING);
	}

	sprintf (cmd_str, "%s", ret_str);
	free (ret_str);
	return (DS_OK);
}


/**
 * Returns to the user a structure of device
 * information for every specified device client handle.
 * The information structure contains:
 * 	- the name of the device,
 *	- the class name,
 *	- the device type,
 *	- the device server name,
 *	- the host name of the device server
 * 
 * @param clnt_handles 	pointer to a table of client handles.
 * @param num_devices   number of devices.
 * @param dev_info    	pointer to information structures returned.
 * @param 		Will contain an appropriate error code if the
 * 		        corresponding call returns a non-zero value.
 * 
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc dev_inform (devserver *clnt_handles, long num_devices,
			  DevInfo * *dev_info, long *error)
{
	devserver	*clnt_ptr;
	DevInfo		*info_ptr;
	long 		i;


	*error = DS_OK;

#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\ndev_inform() : entering routine\n");
#endif /* EBUG */

	if (clnt_handles == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
/*
 * Allocate memory for the array of information structures
 * only if the pointer "dev_info" is initialised to NULL.
 */
	if (*dev_info == NULL)
	{
		*dev_info = (DevInfo *) malloc (num_devices * sizeof(DevInfo));
		if (*dev_info == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
	}

	/* 
 * Read information from the client handles and store it in
 * the information structures.
 */

	clnt_ptr = (devserver *) clnt_handles;
	info_ptr = (DevInfo *)   *dev_info;

	for (i=0; i<num_devices; i++, clnt_ptr++, info_ptr++)
	{
		sprintf (info_ptr->device_name,  "%s", (*clnt_ptr)->device_name);
		sprintf (info_ptr->device_class, "%s", (*clnt_ptr)->device_class);
		sprintf (info_ptr->device_type,  "%s", (*clnt_ptr)->device_type);
		sprintf (info_ptr->server_name,  "%s", (*clnt_ptr)->server_name);
		sprintf (info_ptr->server_host,  "%s", (*clnt_ptr)->server_host);

#ifdef EBUG
		dev_printdebug (DBG_API,
		    "dev_inform() : information data\n");
		dev_printdebug (DBG_API,
		    "name  = %s\n",info_ptr->device_name);
		dev_printdebug (DBG_API,
		    "class  = %s\n",info_ptr->device_class);
		dev_printdebug (DBG_API,
		    "type  = %s\n",info_ptr->device_type);
		dev_printdebug (DBG_API,
		    "svc_name = %s\n",info_ptr->server_name);
		dev_printdebug (DBG_API,
		    "svc_host = %s\n",info_ptr->server_host);
#endif /* EBUG */
	}

	return (DS_OK);
}

/**
 * Sets or reads the timeout for an UDP connection to a server.
 * 
 * @param ds         	handle to device.
 * @param request	indicates whether the timeout should be set or only read.
 * 			A request to set the timeout has to be asked  with CLSET_TIMEOUT. 
 * 			The timeout will be set without any retry.
 *
 * 			A request to read the timeout has to be asked with CLGET_TIMEOUT.
 * @param dev_timeout 	timeout structure.
 * @param error   	Will contain an appropriate error code if the
 * 			corresponding call returns a non-zero value.
 * 
 * @return	DS_OK or DS_NOTOK
 */ 
long _DLLFunc dev_rpc_timeout (devserver ds, long request, 
			       struct timeval *dev_timeout, long *error)
{
	*error = DS_OK;

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
	if (ds->rpc_protocol == D_IIOP)
	{
/* TODO - implement dev_rpc_timeout() */
		return(DS_OK);
	}
#endif /* TANGO */
/*
 * in the stateless import case the device might not be imported yet
 * and therefore has not client handle. In this case if the client
 * wants to set the timeout (CLSET_TIMEOUT) save the timeout for later 
 * when the device is imported by rpc_reinstall_connection(). If the 
 * client wants to get the timeout (CLGET_TIMEOUT) then return the 
 * default timeout (api_timeout) or requested one if there is one.
 */
	if (ds->clnt == NULL)
	{
		switch (request)
		{
			case (CLSET_TIMEOUT) : 
				ds->rpc_timeout.tv_sec = dev_timeout->tv_sec;
				ds->rpc_timeout.tv_usec = dev_timeout->tv_usec;
				break;

			case (CLGET_TIMEOUT) :
				if ((dev_timeout->tv_sec == 0) &&
				    (dev_timeout->tv_usec == 0))
				{
					dev_timeout->tv_sec = api_timeout.tv_sec;
					dev_timeout->tv_usec = api_timeout.tv_usec;
				}
				else
				{
					dev_timeout->tv_sec = ds->rpc_timeout.tv_sec;
					dev_timeout->tv_usec = ds->rpc_timeout.tv_usec;
				}
				break;
				
        		default:
                		*error = DevErr_UnknownInputParameter;
                		return (DS_NOTOK);

		}
		return(DS_OK);
	}
		
	/*
	 * to be sure to work with the correct client handle
	 * initialise the pointer again.
	 */

	ds->clnt = svr_conns[ds->no_svr_conn].clnt;

	switch (request)
	{
		/*
	    * Set the timeout for the connection.
	    */
	case CLSET_TIMEOUT:
		clnt_control (ds->clnt, CLSET_RETRY_TIMEOUT, (char *) dev_timeout);
		clnt_control (ds->clnt, CLSET_TIMEOUT, (char *) dev_timeout);
		break;

		/*
	    * Set the timeout of the connection.
	    */
	case CLGET_TIMEOUT:
		clnt_control (ds->clnt, CLGET_RETRY_TIMEOUT, (char *) dev_timeout);
		clnt_control (ds->clnt, CLGET_TIMEOUT, (char *) dev_timeout);
		break;

	default:
		*error = DevErr_UnknownInputParameter;
		return (DS_NOTOK);
	}

	return (DS_OK);
}


#if defined(_NT)
/**
 * walks the heap until allocated block pointed to
 * by pointer id found and checks if heap is still OK.
 * 
 * @param pointer    	pointer to allocated block on heap.
 * 
 * @param error  	Will contain an appropriate error code if the
 * 			corresponding call returns a zero value.
 * 
 * @return	size of allocated block on heap
 */ 
#if defined (__STDC__)
size_t _DLLFunc msize(void* pointer, int* error) 
#else
/**/
size_t msize(pointer, error) 
void* pointer;
int*  error;
#endif /* __STDC__ */
{
   struct _heapinfo info;  /* Heap info structure */
   int    heap_status;     /* Heap status */
   size_t ret_val;         /* msize() return value */

   info._pentry= NULL;     /* Initialize heap info structure */

   do {
      heap_status= _heapwalk(&info);   /* Step through heap until */
   } while ((pointer != info._pentry)  /* sesired entry is found */
     && (heap_status == _HEAPOK));     /* or a heap error occurs */

   if(heap_status == _HEAPOK)
      if(info._useflag != _FREEENTRY)
         ret_val= info._size;          /* no errors */
      else {
         ret_val= 0;                   /* Entry has been freed */
         *error= _FREEENTRY;
      }
   else {
      ret_val= 0;                      /* A heap error occurred */
      *error= heap_status;
   }
   return(ret_val);
}
#endif   /* _NT */

/**
 * Get the index for the nethost from the device
 * name. The nethost is specified in the device name
 * as "//nethost/domain/family/member". 
 * 
 * @param device_name  	device name
 * @param error		Will contain an appropriate error code if the
 * 			corresponding call returns a non-zero value.
 * 
 * @return 	returns the index for the nethost 	
 *		If the nethost is not specified then return the index for the
 * 		default nethost (0). If the nethost is specified
 * 		but not imported then return DS_NOTOK.
 */ 
long _DLLFunc get_i_nethost_by_device_name (char *device_name, long *error)
{
	long i_nethost, i;
	char *nethost;
	

/*
 * is the nethost specified ?
 */

	if (strncmp(device_name,"//",2) == 0)
	{
/*
 * make a copy of the nethost name without the "//" and
 * removing the device name which follows
 */
		nethost = (char*)malloc(strlen(device_name)+1);
		if (nethost == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}

		strcpy(nethost,device_name+2);
		for (i=0; i<(int)strlen(nethost); i++)
		{
			if (nethost[i] == '/')
			{
				nethost[i] = 0;
				break;
			}
		}
/*
 * check to see whether the nethost has been imported
 */
	
		if ((i_nethost = get_i_nethost_by_name(nethost,error)) < 0)
/*
 * the specified nethost is not in the list of imported nethosts,
 * (config_setup_multi() should be called to add it)
 */
		{
			i_nethost = DS_NOTOK;
		}
		free(nethost);
	}
/*
 * nethost not specified in the device name, assume the nethost of
 * the control system by default (i_nethost = 0)
 */
	else
	{
		i_nethost = 0;
	}

	return(i_nethost);
}

/**
 * Get the index for the nethost from the nethost name. 
 *
 * @param device_name 	device name
 * @param error  	Will contain an appropriate error code if the
 *		        corresponding call returns a non-zero value.
 *
 * @return 	the index of nethost or DS_NOTOK if not found in the list of known 
 *		NETHOST's.
 */
long _DLLFunc get_i_nethost_by_name (char *nethost, long *error)
{
	long i;
	
/*
 * loop through the array of imported nethosts looking for the 
 * specified nethost
 */
/* no hethost specified */
	if(nethost==NULL)
	    return -1;

	for (i=0; i<max_nethost; i++)
	{
/*
 * first check to see whether this nethost has been imported
 */
		if( (multi_nethost[i].config_flags.configuration == True) &&
		    (multi_nethost[i].nethost != NULL))
		{
			if (strcmp(nethost, multi_nethost[i].nethost) == 0)
			{
				return(i);
			}
		}
	}
	
	return(DS_NOTOK);
}


/**
 * Get the nethost name associated with this index
 * 
 * @param i_nethost 	nethost index
 * @param error  	Will contain an appropriate error code if the
 * 		        corresponding call returns a non-zero value.
 * 
 *  Return(s)  :	DS_OK or DS_NOTOK
 */
char* _DLLFunc get_nethost_by_index (long i_nethost, long *error)
{
	char *nethost;
/*
 * first check to see whether the requested nethost has been imported
 */
	if (multi_nethost[i_nethost].config_flags.configuration)
	{
		nethost = multi_nethost[i_nethost].nethost;
	}
	else
	{
		nethost = NULL;
	}

	return(nethost);
}


/**
 * Extract the domain/family/member part of name from the 
 * full device name "//nethost/domain/family/member?number" ,
 * return a pointer
 *
 * @param full_name 	device name
 * @param error  	Will contain an appropriate error code if the
 *		        corresponding call returns a non-zero value.
 *
 * @return	pointer to string containing only "domain/family/member"
 */
	
char* _DLLFunc extract_device_name (char *full_name, long *error)
{
	static char *device_name_alloc=NULL, *device_name;
	char *prog_url;
	
/*
 * assume full_name == device_name to start off with
 */
	device_name_alloc = (char*)realloc(device_name_alloc,strlen(full_name)+1);
	sprintf(device_name_alloc,"%s",full_name);
	device_name = device_name_alloc;
/*
 * if nethost is specified in the device name "remove" it
 */
	if (strncmp(device_name,"//",2) == 0)
	{
		for (device_name +=2; *device_name != '/'; device_name++)
		{ 
			/* do nothing */ 
		};
		device_name++;
	}
	if (strchr(device_name,'?') != NULL)
	{
		prog_url = strchr(device_name, '?');
		*prog_url = 0;
	}

	return(device_name);
}

/*
 * global arrays required for multi-nethost support, memory is
 * allocated for them by nethost_alloc()
 */
extern struct _devserver *msg_ds;
extern struct _devserver *db_ds;
extern short *auth_flag;

/**
 * alloc place for MIN_NETHOST to global array which contains list of nethosts
 * 
 * @param error Will contain an appropriate error code if the
 * 		corresponding call returns a non-zero value.
 * 
 * @return	DS_OK or DS_NOTOK
 */ 
long _DLLFunc nethost_alloc (long *error)
{
	static long first=1;
	long i, new_max_nethost;

/*
 * first time round allocate space for only one NETHOST, because
 * there is a high probability that the client only ones to work
 * with one control system i.e. $NETHOST
 */
	if (first)
	{
		multi_nethost =
		(nethost_info*)malloc((sizeof(nethost_info)));
		if (multi_nethost == NULL)
		    {
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		    }

		msg_ds = (struct _devserver*)malloc((sizeof(struct _devserver)));
		if (msg_ds == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}

		db_ds = (struct _devserver*)malloc((sizeof(struct _devserver)));
		if (db_ds == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
		auth_flag = (short*)malloc((sizeof(short)));
		first = 0;
		new_max_nethost = 1;
	}
/*
 * if more the one NETHOST is being used allocate space for an additional
 * MIN_NETHOST's at each call to avoid calling realloc() too often
 */
	else
	{
	    void* tmp;
		tmp = realloc(multi_nethost,(sizeof(nethost_info))*(max_nethost+MIN_NETHOST));
		
		if (tmp == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
		multi_nethost=(nethost_info*)tmp;
		tmp=realloc(msg_ds,(sizeof(struct _devserver))*(max_nethost+MIN_NETHOST));
		if (tmp == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
		msg_ds = (struct _devserver*)tmp;
		tmp=realloc(db_ds,(sizeof(struct _devserver))*(max_nethost+MIN_NETHOST));
		if (tmp == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
		db_ds = (struct _devserver*)tmp;
		tmp=realloc(auth_flag,(sizeof(short))*(max_nethost+MIN_NETHOST));
		if (tmp == NULL)
		{
			*error  = DevErr_InsufficientMemory;
			return (DS_NOTOK);
		}
		auth_flag = (short*)tmp;
		new_max_nethost = max_nethost + MIN_NETHOST;
	}
/* 
 * update db_info and msg_info pointers to take into account realloc() of db_ds and msg_ds
 * which could have moved in memory
 */
	for (i=0; i<max_nethost; i++)
	{
		multi_nethost[i].db_info = (devserver)&db_ds[i];
		multi_nethost[i].msg_info = (devserver)&msg_ds[i];
	}
/* 
 * set new entries to False to indicate that they are free to be used
 */
	for (i=max_nethost; i<new_max_nethost; i++)
	{
		multi_nethost[i].config_flags.configuration = False;
/*
 * initialise all flags to zero so that no-one (e.g. ET) tries to use
 * the database (or any other server) as long as it is not imported
 */
		memset((char*)&multi_nethost[i].config_flags, 0, sizeof(multi_nethost[i].config_flags));
		auth_flag[i] = False;
	}
	max_nethost = new_max_nethost;
#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
			"nethost_alloc(): (re)allocate space for nethost, new max = %d\n",
			 max_nethost);
#endif /* EBUG */

	return(DS_OK);
}


/**
 * Pings the device server to find out if this device is being served. 
 *
 * @param ds  	client handle for the associated device.
 * @param error Will contain an appropriate error code if the
 * 		corresponding call returns a non-zero value.
 *
 * @return	DS_OK or DS_NOTOK
 */
long _DLLFunc dev_ping (devserver ds, long *error)
{
	_dev_import_in	dev_import_in;
	_dev_import_out	dev_import_out;
	char 		*in_name_stripped;
	enum clnt_stat  clnt_stat;

	*error = DS_OK;
#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\ndev_ping() : entering routine\n");
#endif /* EBUG */

	if (ds == NULL)
	{
		*error = DevErr_DeviceNotImportedYet;
		return(DS_NOTOK);
	}
#ifdef TANGO
        if (ds->rpc_protocol == D_IIOP)
        {
		tango_dev_ping(ds, error);
                return(DS_OK);
        }
#endif /* TANGO */
	/*
         * Verify the RPC connection.
         */

	if ( dev_rpc_connection (ds, error)  == DS_NOTOK )
		return (DS_NOTOK);


/*
 *  fill in data transfer structures dev_import_in
 *  and dev_import_out.
 *
 * strip nethost name, if present 
 */
	if(ds->device_name[0]=='/' && ds->device_name[1]=='/') /* Net host part present */
            	in_name_stripped = strchr(&(ds->device_name[2]), '/');
        else
        	in_name_stripped = ds->device_name;
			
	dev_import_in.device_name = in_name_stripped;
	dev_import_in.access_right = 0;
	dev_import_in.client_id = 0;
	dev_import_in.connection_id = 0;

	dev_import_in.var_argument.length   = 0;
	dev_import_in.var_argument.sequence = NULL;
/*
 *  Call the rpc entry point RPC_DEV_PING at the specified device server.
 */
	memset ((char *)&dev_import_out, 0, sizeof (dev_import_out));
/*
 * Query a device with the current version number >1.
 */
	if ( ds->vers_number > DEVSERVER_VERS)
	{
		clnt_stat = clnt_call (ds->clnt, RPC_DEV_PING,
		    (xdrproc_t)xdr__dev_import_in,  (caddr_t) &dev_import_in,
		    (xdrproc_t)xdr__dev_import_out, (caddr_t) &dev_import_out, TIMEVAL(timeout));
	}
	else
	{
 	   /*
	    * An old device does not support ping !
	    */
		*error = DevErr_CommandNotImplemented;;
		return(DS_NOTOK);
	}


	/*
         * Check for errors on the RPC connection.
         */

	if ( dev_rpc_error (ds, clnt_stat, error) == DS_NOTOK )
		return (DS_NOTOK);
	/*
	 *  free dev_import_out 
	 */

	xdr_free ((xdrproc_t)xdr__dev_import_out,
	    (char *)&dev_import_out);

	/*
         * Free the variable arguments in the dev_import_out
         * structure, coming from the server.
         */

	xdr_free ((xdrproc_t)xdr_DevVarArgumentArray,
	    (char *)&(dev_import_out.var_argument));

/*
 * Return error code and status from device server.
 */
	*error = dev_import_out.error;
	return (dev_import_out.status);
}
