
/*+*******************************************************************

 File:          vrif_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for VRIF vme card types.

 Author(s):     Jens Meyer
		$Author: jkrueger1 $

 Original:      January 1991

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <vrif_xdr.h>


bool_t
xdr_DevVrifWdog(xdrs, objp)
	XDR *xdrs;
	DevVrifWdog *objp;
{
	if (!xdr_char(xdrs, &objp->Value)) {
		return (FALSE);
	}
        if (!xdr_string(xdrs, &objp->PassWord, 80)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t
xdr_DevVrifVmeStat(xdrs, objp)
	XDR *xdrs;
	DevVrifVmeStat *objp;
{
	if (!xdr_long(xdrs, &objp->Address)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Stop)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Sysfail)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Dtack)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Berr)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->DtatStrobe)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Iack)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Lword)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Am)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Write)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->As)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t
xdr_DevVrifPowerStat(xdrs, objp)
	XDR *xdrs;
	DevVrifPowerStat *objp;
{
	if (!xdr_float(xdrs, &objp->Vcc)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->VccP2)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Flag_p12v)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->Flag_m12v)) {
		return (FALSE);
	}
	return (TRUE);
}

