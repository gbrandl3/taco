/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        mstat_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source file for the XDR definitions
 *		for the ESRF machine status data type.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	March 1993
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:23 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <dev_xdr.h>
#include <mstat_xdr.h>

/*
 *  definition for the ESRF machine status array
 */

bool_t
xdr_SysState(xdrs, objp)
	XDR *xdrs;
	SysState *objp;
{
	if (!xdr_string(xdrs, &objp->sys_name, MAXU_INT)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->sys_ident)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->sys_state)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_SysState(objp)
	SysState *objp;
{
        long  length = 0;

        length += xdr_length_DevString (&objp->sys_name);
        length += xdr_length_DevLong (&objp->sys_ident);
        length += xdr_length_DevLong (&objp->sys_state);

        return (length);
}


bool_t xdr_VarSysStateArray(xdrs, objp)
        XDR *xdrs;
        VarSysStateArray *objp;
{
        if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
	     (u_int *)&objp->length, MAXU_INT, sizeof(SysState), (xdrproc_t)xdr_SysState)) {
                return (FALSE);
	}
        return (TRUE);
}

long
xdr_length_VarSysStateArray(objp)
	VarSysStateArray *objp;
{
        long    length = 0;
	long	i;

        /*
         *  four bytes for the number of array elements
         */

	length += xdr_length_DevLong (&objp->length);

        /*
         *  now calculate the length of the array
         */

	for (i=0; (u_long)i<objp->length; i++)
	   {
	   length += xdr_length_SysState(&objp->sequence[i]);
	   }

        return (length);
}
