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
 * File:        vrif_xdr.c
 * 
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for VRIF vme card types.
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    January 1991
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
	if (!xdr_DevLong(xdrs, &objp->Address)) {
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

