/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
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

