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
 * File:        api_xdr_vers3.c
 * 
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for API-library functions for the
 *		old version3. This file is needed for compatibility
 *		reasons.
 *
 * Author(s): 	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    November 1994
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2008-04-06 09:07:20 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <DevXdrKernel.h>
#include <API_xdr_vers3.h>

/*
 *  xdr_routines for data structures used in the dev_putget(),
 *  dev_put() and dev_putget_raw() calls.
 */
bool_t xdr__server_data_3 (XDR *xdrs, _server_data_3 *objp)
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
 	return (TRUE);
}

bool_t xdr__client_data_3 (XDR *xdrs, _client_data_3 *objp)
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
		fprintf (stderr, "'%s\n", msg);
#endif
		objp->argout   = NULL;
		data_type.size = 0;
		data_type.xdr  = (DevDataFunction)xdr_void;
	}

	if (!xdr_pointer(xdrs, (char **)&objp->argout, data_type.size, (xdrproc_t)data_type.xdr )) 
	  	return (FALSE);
  	return (TRUE);
}

/*
 *  xdr function used by dev_putget_raw to decode return
 *  arguments of a device server in raw opaque format.
 */

bool_t xdr__client_raw_data_3 (XDR *xdrs, _client_raw_data_3 *objp)
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
 * On the client side decode the arguments as a
 * stream of bytes with the length calculated
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
  	return (TRUE);
}


/*
 *  internal xdr_routines for device service  
 */

bool_t xdr__dev_import_in_3 (XDR *xdrs, _dev_import_in_3 *objp)
{
        if (!xdr_string(xdrs, &objp->device_name, MAXU_INT)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_import_out_3 (XDR *xdrs, _dev_import_out_3 *objp)
{
        if (!xdr_opaque(xdrs, (caddr_t)objp->server_name, LONG_NAME_SIZE)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_free_in_3 (XDR *xdrs, _dev_free_in_3 *objp)
{
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr__dev_free_out_3 (XDR *xdrs, _dev_free_out_3 *objp)
{
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	return (TRUE);
}

/*
 *  xdr_routines for dev_cmd_query()  
 */
bool_t xdr__dev_query_in_3 (XDR *xdrs, _dev_query_in_3 *objp)
{
	if (!xdr_DevLong(xdrs, &objp->ds_id)) 
		return (FALSE);
	return (TRUE);
}	

bool_t xdr__dev_query_out_3(XDR *xdrs, _dev_query_out_3 *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(_dev_cmd_info), (xdrproc_t)xdr__dev_cmd_info)) 
		return (FALSE);
	if (!xdr_opaque(xdrs, (caddr_t)objp->class_name, 20)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->error)) 
		return (FALSE);
	if (!xdr_DevLong(xdrs, &objp->status)) 
		return (FALSE);
	return (TRUE);
}	

bool_t xdr__manager_data_3 (XDR *xdrs, _manager_data_3 *objp)
{
        if (!xdr__register_data(xdrs, &objp->msg_info)) 
                return (FALSE);
        if (!xdr__register_data(xdrs, &objp->db_info)) 
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->status)) 
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->error)) 
                return (FALSE);
        return (TRUE);
}

