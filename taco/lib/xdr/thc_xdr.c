
/*+*******************************************************************

 File:          thc_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for Thermocouple Controller data types.

 Author(s):	Jens Meyer

 Original:	July 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:47 $

$Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/


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

	length = length + xdr_length_DevFloat (&objp->temperature);
	length = length + xdr_length_DevShort (&objp->th_broken);
	length = length + xdr_length_DevShort (&objp->th_alarm);

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

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + (objp->length *
		 xdr_length_DevThermocouple(&objp->sequence[0]) );

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

	length = length + (64 * xdr_length_DevFloat (&objp->temp[0]));
	length = length + 8;
	length = length + 8;
	length = length + 8;
	length = length + 8;
	length = length + 8;

 	return (length);
}
