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
 * File:        ws_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for Wire Scanner data types.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2006-09-18 22:07:20 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
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


