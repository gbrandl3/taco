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
 * File:        api_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for API-library functions.
 *
 * Author(s): 	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    January 1991
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2008-04-06 09:07:20 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <Admin.h>
#include <DevXdrKernel.h>
#include <macros.h>

/*
 * Xdr_routines for variable argument passing for API calls.
 * Will avoid recompilation of the system when adding variables.
 */

bool_t xdr_DevVarArgument (XDR *xdrs, DevVarArgument *objp)
{
        DevDataListEntry        data_type;
        DevLong                 error;
	char			*p;

	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : entering routine.\n");
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : call xdr_DevLong(argument_type).\n");
        if (!xdr_DevLong(xdrs, &objp->argument_type)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : leaving routine, xdr_DevLong failed.\n");
                return (FALSE);
	}

/*
 * Get the XDR data type from the loaded type list
 */
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : call xdr_get_type().\n");
        if ( xdr_get_type(objp->argument_type, &data_type, &error) == DS_NOTOK)
	{
		char msg[512];
		snprintf(msg, sizeof(msg), "xdr_DevVarArgument() : error = %d", error);
#ifdef WIN32
		PRINTF(msg);
#else
		fprintf(stderr, msg);
#endif
		dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : leaving routine, xdr_get_type failed.\n");
		return (FALSE);
        }

	p = (char *)objp->argument;
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : objp->argument %p.\n", &objp->argument);
        if (!xdr_pointer(xdrs, (char **)&objp->argument, data_type.size, (xdrproc_t)data_type.xdr )) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : leaving routine, xdr_pointer failed.\n");
                return (FALSE);
	}
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgument() : leaving routine.\n");
	return (TRUE);
}

bool_t xdr_DevVarArgumentArray(XDR *xdrs, DevVarArgumentArray *objp)
{
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgumentArray() : entering routine.\n");
        if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
		MAXU_INT, sizeof(DevVarArgument), (xdrproc_t)xdr_DevVarArgument)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgumentArray() : leaving routine, xdr_array failed.");
                return (FALSE);
	}
	dev_printdebug(DBG_TRACE | DBG_API, "xdr_DevVarArgumentArray() : leaving routine.\n");
        return (TRUE);
}

/*
 *  xdr_routines for data structures used in the dev_putget(),
 *  dev_put() and dev_putget_raw() calls.
 */
bool_t xdr__server_data(XDR *xdrs, _server_data *objp)
{
	DevDataListEntry	data_type;
	DevLong			error;	

	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->cmd)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->argin_type)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->argout_type)) 
		return (FALSE);
/*
 * Get the XDR data type from the loaded type list
 */
	if ( xdr_get_type(objp->argin_type, &data_type, &error) == DS_NOTOK)
	{
		char msg[512];
		snprintf (msg, sizeof(msg), "xdr__server_data() : error = %d", error);
#ifdef WIN32
		PRINTF(msg);
#else
		fprintf (stderr, "%s\n", msg);
#endif
		return (FALSE);
	}

	if (!xdr_pointer(xdrs, (char **)&objp->argin, data_type.size, (xdrproc_t)data_type.xdr )) 
		return (FALSE);

	if (!xdr_DevLong(xdrs, &objp->access_right)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->client_id)) 
		return (FALSE);
        if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
                return (FALSE);
 	return (TRUE);
}

bool_t xdr__client_data(XDR *xdrs, _client_data *objp)
{
	DevDataListEntry	data_type;
	DevLong			error;	

	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->argout_type)) 
		return (FALSE);

/*
 * Get the XDR data type from the loaded type list
 */
	if ( xdr_get_type(objp->argout_type, &data_type, &error) == DS_NOTOK)
	{
		char msg[512];
		snprintf (msg, sizeof(msg), "xdr__client_data() : error = %d", error);
#ifdef WIN32
		PRINTF(msg);
#else
		fprintf (stderr, "%s\n", msg);
#endif
		objp->argout   = NULL;
		data_type.size = 0;
		data_type.xdr  = (DevDataFunction)xdr_void;
	}

	if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)data_type.xdr )) 
	  	return (FALSE);

        if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
                return (FALSE);
  	return (TRUE);
}

/*
 *  xdr function used by dev_putget_raw to decode return
 *  arguments of a device server in raw opaque format.
 */
bool_t xdr__client_raw_data(XDR *xdrs, _client_raw_data *objp)
{
	DevOpaque 		*opaque;
	DevDataListEntry	data_type;
	DevLong			error;	

	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->deser_argout_type)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->xdr_length)) 
		return (FALSE);
/*  On the server side encode the data with the data
 *  type specified for the command as return argument.
 */
	if ( xdrs->x_op == XDR_ENCODE )
	{
/*
 * Get the XDR data type from the loaded type list
 */
		if ( xdr_get_type(objp->ser_argout_type, &data_type, &error) == DS_NOTOK)
		{
			char msg[512];
			snprintf (msg, sizeof(msg), "xdr__client_raw_data() : error = %d", error);
#ifdef WIN32
			PRINTF(msg);
#else
			fprintf (stderr, "%s\n", msg);
#endif
			return (FALSE);
		}
		if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)data_type.xdr )) 
	  		return (FALSE);
	}
	else
	{
/*
 *  On the client side decode the arguments as a
 *  stream of bytes with the length calculated
 * on the server side (xdr_length).
 *
 * Get the XDR data type from the loaded type list
 */
		if ( xdr_get_type(objp->deser_argout_type, &data_type, &error) == DS_NOTOK)
		{
			char msg[512];
			snprintf (msg, sizeof(msg), "xdr__client_raw_data() : error = %d", error);
#ifdef WIN32
			PRINTF(msg);
#else
			fprintf (stderr, "%s\n", msg);
#endif
			return (FALSE);
		}

		opaque = (DevOpaque *)objp->argout;
		opaque->length       = objp->xdr_length;

		if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)xdr_DevOpaqueRaw )) 
	  		return (FALSE);
	}

        if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
                return (FALSE);
  	return (TRUE);
}

bool_t xdr_DevOpaqueRaw(XDR *xdrs, DevOpaque *objp)
{
	if (xdrs->x_op == XDR_FREE)
		free (objp->sequence);

	if (objp->sequence == 0)
		objp->sequence = (char *)malloc (objp->length);

	if (!xdr_opaque(xdrs, (caddr_t)objp->sequence, objp->length)) 
		   return (FALSE);
	return (TRUE);
}

/*
 *  internal xdr_routines for device service  
 */

bool_t xdr__dev_import_in(XDR *xdrs, _dev_import_in *objp)
{
        if (!xdr_string(xdrs, &objp->device_name, MAXU_INT)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->access_right)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->client_id)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->connection_id)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_import_out(XDR *xdrs, _dev_import_out *objp)
{
        if (!xdr_opaque(xdrs, (caddr_t)objp->server_name, LONG_NAME_SIZE)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_free_in(XDR *xdrs, _dev_free_in *objp)
{
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->access_right)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->client_id)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_free_out(XDR *xdrs, _dev_free_out *objp)
{
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__server_admin(XDR *xdrs, _server_admin *objp)
{
	if (!xdr_DevLong(xdrs, &objp->cmd)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->debug_bits)) 
		return (FALSE);
	return (TRUE);
}

/*
 *  xdr_routines for dev_cmd_query()  
 */
bool_t xdr__dev_query_in(XDR *xdrs, _dev_query_in *objp)
{
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}	

bool_t xdr__dev_cmd_info(XDR *xdrs, _dev_cmd_info *objp)
{
	if (!xdr_DevLong(xdrs, &objp->cmd)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->in_type)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->out_type)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_query_out(XDR *xdrs, _dev_query_out *objp)
{
	dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : entering routine.\n");
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(_dev_cmd_info), (xdrproc_t)xdr__dev_cmd_info)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : leaving routine, xdr_array failed.\n");
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->class_name, SHORT_NAME_SIZE)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : leaving routine, xdr_opaque failed.\n");
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->error)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : leaving routine, xdr_DevLong(error) failed.\n");
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->status)) 
	{
		dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : leaving routine, xdr_DevLong(status) failed.\n");
		return (FALSE);
	}
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	dev_printdebug(DBG_TRACE | DBG_API, "xdr__dev_query_out() : leaving routine.\n");
	return (TRUE);
}	

/* event query */
bool_t xdr__dev_event_info(XDR *xdrs, _dev_event_info *objp)
{
	if (!xdr_DevLong(xdrs, &objp->event)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->out_type)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_queryevent_out(XDR *xdrs, _dev_queryevent_out *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(_dev_event_info), (xdrproc_t)xdr__dev_event_info)) 
		return (FALSE);
	if (!xdr_opaque(xdrs, (caddr_t)objp->class_name, SHORT_NAME_SIZE)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}	
/*end eventquery*/

/*
 *  internal xdr_routines for message service data transfer 
 */
bool_t xdr__msg_data(XDR *xdrs, _msg_data *objp)
{
        if (!xdr_string(xdrs, &objp->devserver_name, MAXU_INT)) 
		return (FALSE);
        if (!xdr_string(xdrs, &objp->host_name, MAXU_INT)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->prog_number)) 
		return (FALSE);
        if (!xdr_string(xdrs, &objp->display, MAXU_INT)) 
		return (FALSE);
	if (!xdr_short(xdrs, &objp->type_of_message)) 
		return (FALSE);
        if (!xdr_string(xdrs, &objp->message_buffer, MAXU_INT)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__msg_out(XDR *xdrs, _msg_out *objp)
{
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	return (TRUE);
}

/*
 *  internal xdr_routines for network manager service data transfer 
 */

bool_t xdr__register_data(XDR *xdrs, _register_data *objp)
{
	if (!xdr_string(xdrs, &objp->host_name, MAXU_INT)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->prog_number))
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->vers_number)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__manager_data(XDR *xdrs, _manager_data *objp)
{
	if (!xdr__register_data(xdrs, &objp->msg_info)) 
		return (FALSE);
	if (!xdr__register_data(xdrs, &objp->db_info)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->security)) 
		return (FALSE);
	if (!xdr_DevVarArgumentArray(xdrs, &objp->var_argument)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__msg_manager_data(XDR *xdrs, _msg_manager_data *objp)
{
	if (!xdr_string(xdrs, &objp->dshome, MAXU_INT)) 
		return (FALSE);
	if (!xdr_string(xdrs, &objp->display, MAXU_INT)) 
		return (FALSE);
	return (TRUE);
}
