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
 * File:        rf_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for radio frequency types.
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    April 1992
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <dev_xdr.h>
#include <ct_xdr.h>

bool_t
xdr_DevCtIntLifeTime (xdrs, objp)
	XDR *xdrs;
	DevCtIntLifeTime *objp;
{
	if (!xdr_float(xdrs, &objp->DeltaIntensity)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->LifeTime)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->DateTicks)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->DeltaTused)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevCtIntLifeTime(objp)
	DevCtIntLifeTime *objp;
	{
	long  length = 0;

	length += xdr_length_DevFloat (&objp->DeltaIntensity);
	length += xdr_length_DevFloat (&objp->LifeTime);
	length += xdr_length_DevLong  (&objp->DateTicks);
	length += xdr_length_DevLong  (&objp->DeltaTused);

	return (length);
}




bool_t
xdr_DevVarCtIntLifeTimeArray(xdrs, objp)
	XDR *xdrs;
	DevVarCtIntLifeTimeArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevCtIntLifeTime),
	    (xdrproc_t)xdr_DevCtIntLifeTime)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarCtIntLifeTimeArray (objp)
	DevVarCtIntLifeTimeArray *objp;
{
	long    length = 0;

	/*
	 *  four bytes for the number of array elements
  	 */

	length += xdr_length_DevLong (&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length += (objp->length * xdr_length_DevCtIntLifeTime(&objp->sequence[0]) );

	return (length);
}


