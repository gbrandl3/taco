/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        thc_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for Thermocouple Controller data types.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
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
#include <thc_xdr.h>


/*
 *  xdr_routines for Thermocouples
 */

bool_t
xdr_DevThermocouple(xdrs, objp)
	XDR *xdrs;
	DevThermocouple *objp;
{
	if (!xdr_float(xdrs, &objp->temperature)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->th_broken)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->th_alarm)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevThermocouple (objp)
	DevThermocouple *objp;
{
	long  length = 0;

	length += xdr_length_DevFloat (&objp->temperature);
	length += xdr_length_DevShort (&objp->th_broken);
	length += xdr_length_DevShort (&objp->th_alarm);

	return (length);
}



bool_t
xdr_DevVarTHArray(xdrs, objp)
	XDR *xdrs;
	DevVarTHArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevThermocouple), 
	    (xdrproc_t)xdr_DevThermocouple)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarTHArray (objp)
	DevVarTHArray *objp;
{
        long    length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length += xdr_length_DevLong (&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length += (objp->length * xdr_length_DevThermocouple(&objp->sequence[0]) );

 	return (length);
}




bool_t
xdr_DevLienState(xdrs, objp)
	XDR *xdrs;
	DevLienState *objp;
{
        if (!xdr_vector(xdrs, (char *)objp->temp, 64, 
			sizeof(float), (xdrproc_t)xdr_float)) {
 		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->alarm, 8)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->cut, 8)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->output, 8)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->input, 8)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->err, 8)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevLienState (objp)
	DevLienState *objp;
{
        long    length = 0;

	length += (64 * xdr_length_DevFloat (&objp->temp[0]));
	length += 8;
	length += 8;
	length += 8;
	length += 8;
	length += 8;

 	return (length);
}
