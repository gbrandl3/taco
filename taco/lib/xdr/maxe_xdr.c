
/*+*******************************************************************

 File:          maxe_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for maxe types.

 Author(s):     Timo Mettala

 Original:      June 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:47 $

$Author: jkrueger1 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <maxe_xdr.h>

bool_t
xdr_DevMulMove(xdrs, objp)
	XDR *xdrs;
	DevMulMove *objp;
{
	if (!xdr_vector(xdrs, (char *)objp->action, 8,
		        sizeof(long), (xdrproc_t)xdr_long)) {
		return (FALSE);
     	}
     	if (!xdr_vector(xdrs, (char *)objp->delay, 8,
		        sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
  	}
 	if (!xdr_vector(xdrs, (char *)objp->position, 8,
       			sizeof(float), (xdrproc_t)xdr_float)) {
       		return (FALSE);
       }
       return (TRUE);
}

bool_t
xdr_DevMotorLong(xdrs, objp)
	XDR *xdrs;
	DevMotorLong *objp;
{
	if (!xdr_long(xdrs, &objp->axisnum))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->value))
		return (FALSE);

       return (TRUE);
}



bool_t
xdr_DevMotorFloat(xdrs, objp)
	XDR *xdrs;
	DevMotorFloat *objp;
{
	if (!xdr_long(xdrs, &objp->axisnum))
		return (FALSE);
	if (!xdr_float(xdrs, &objp->value))
		return (FALSE);

       return (TRUE);
}
