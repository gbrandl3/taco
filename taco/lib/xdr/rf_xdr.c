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
#include <rf_xdr.h>


bool_t
xdr_DevRfSigConfig(xdrs, objp)
	XDR *xdrs;
	DevRfSigConfig *objp;
{
	if (!xdr_int(xdrs, &objp->number)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->name, 12)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->family, 4)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->related_setpoint)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->units, 8)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->warn_high)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->warn_low)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->error_high)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->error_low)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->scale_high)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->scale_low)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->format, 16)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->description, 80)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->setptr)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->false_text, 10)) {
		return (FALSE);
	}
        if (!xdr_opaque(xdrs, (caddr_t)objp->true_text, 10)) {
		return (FALSE);
	}
	return (TRUE);
}

