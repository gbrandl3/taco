/*+*******************************************************************

 File:          icv101_xdr.c
 Project:       Device Servers with sun-rpc
 Description:   xdr_routines for icv101 types
 Author(s):     Timo Mettala
 Original:      March  1993

Copyright (c) 1993 by European Synchrotron Radiation Facility,
                      Grenoble, France
*******************************************************************-*/

static char RcsId[] =
"$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/icv101_xdr.c,v 1.1 2003-04-25 11:21:47 jkrueger1 Exp $";

#include "dev_xdr.h"
#include "icv101_xdr.h"

bool_t
xdr_DevPattern(xdrs, objp)
	XDR *xdrs;
	DevPattern *objp;
{
	if (!xdr_short(xdrs, &objp->channel)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->gain)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->end)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t xdr_DevVarPatternArray(xdrs, objp)
        XDR *xdrs;
        DevVarPatternArray *objp;
{
        if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
	     (u_int *)&objp->length, MAXU_INT, sizeof(DevPattern), (xdrproc_t)xdr_DevPattern)) {
                return (FALSE);
	}
        return (TRUE);
}


bool_t
xdr_DevIcv101Mode(xdrs, objp)
	XDR *xdrs;
	DevIcv101Mode *objp;
{
	if (!xdr_long(xdrs, &objp->external_stop)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->external_start)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->continuous)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->main_frequency)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->sub_frequency)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->npfs2)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->npost)) {
		return (FALSE);
	}
	return (TRUE);
}

