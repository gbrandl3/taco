
/*+*******************************************************************

 File:          ws_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for Wire Scanner data types.

 Author(s):	Jens Meyer

 Original:	July 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:47 $

$Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/


#include <dev_xdr.h>
#include <ws_xdr.h>


/*
 *  xdr_routines for wire scanner types
 */

bool_t
xdr_DevWsBeamFitParm(xdrs, objp)
	XDR *xdrs;
	DevWsBeamFitParm *objp;
{
	if (!xdr_DevVarFloatArray(xdrs, &objp->pos_array)) {
		return (FALSE);
	}
	if (!xdr_DevVarFloatArray(xdrs, &objp->meas_array)) {
		return (FALSE);
	}
	if (!xdr_DevVarFloatArray(xdrs, &objp->perc_array)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}


