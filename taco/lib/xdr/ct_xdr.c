
/*+*******************************************************************

 File:          rf_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for radio frequency types.

 Author(s):     Jens Meyer
		$Author: jkrueger1 $

 Original:      April 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

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
	if (!xdr_long(xdrs, &objp->DateTicks)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->DeltaTused)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevCtIntLifeTime(objp)
	DevCtIntLifeTime *objp;
	{
	long  length = 0;

	length = length + xdr_length_DevFloat (&objp->DeltaIntensity);
	length = length + xdr_length_DevFloat (&objp->LifeTime);
	length = length + xdr_length_DevLong  (&objp->DateTicks);
	length = length + xdr_length_DevLong  (&objp->DeltaTused);

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

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + (objp->length *
	         xdr_length_DevCtIntLifeTime(&objp->sequence[0]) );

	return (length);
}


