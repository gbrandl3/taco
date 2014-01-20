/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        icv101_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for icv101 types
 *
 * Author(s):   Timo Mettala
 *		$Author: jkrueger1 $
 *
 * Original:    March  1993
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

static char RcsId[] =
"$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/icv101_xdr.c,v 1.5 2008-04-06 09:07:21 jkrueger1 Exp $";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
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
	if (!xdr_DevLong(xdrs, &objp->external_stop)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->external_start)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->continuous)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->main_frequency)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->sub_frequency)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->npfs2)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->npost)) {
		return (FALSE);
	}
	return (TRUE);
}

