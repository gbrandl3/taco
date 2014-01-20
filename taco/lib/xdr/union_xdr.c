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
 * File:        union_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for ancient union type.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
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
#include <union_xdr.h>
#include <bpm_xdr.h>
#include <ws_xdr.h>


/*
 *  xdr for an union type
 *  this type exits only for compatibility reasons with
 *  the old device server system !
 */

bool_t
xdr_DevUnion(xdrs, objp)
	XDR *xdrs;
	DevUnion *objp;
{
	if (!xdr_short(xdrs, &objp->designator)) {
		return (FALSE);
	}
	switch (objp->designator) {
	case 1:
		if (!xdr_char(xdrs, &objp->u.B_data)) {
			return (FALSE);
		}
		break;
	case 2:
		if (!xdr_short(xdrs, &objp->u.S_data)) {
			return (FALSE);
		}
		break;
	case 3:
		if (!xdr_DevLong(xdrs, &objp->u.L_data)) {
			return (FALSE);
		}
		break;
	case 4:
		if (!xdr_float(xdrs, &objp->u.F_data)) {
			return (FALSE);
		}
		break;
	case 5:
		if (!xdr_double(xdrs, &objp->u.D_data)) {
			return (FALSE);
		}
		break;
	case 6:
		if (!xdr_string(xdrs, &objp->u.STR_data, MAXU_INT)) {
			return (FALSE);
		}
		break;
	case 7:
		if (!xdr_DevFloatReadPoint(xdrs, &objp->u.FRP_data)) {
			return (FALSE);
		}
		break;
	case 8:
		if (!xdr_DevStateFloatReadPoint(xdrs, &objp->u.SFRP_data)) {
			return (FALSE);
		}
		break;
	case 9:
		if (!xdr_DevVarCharArray(xdrs, &objp->u.VCA_data)) {
			return (FALSE);
		}
		break;
	case 10:
		if (!xdr_DevVarShortArray(xdrs, &objp->u.VSA_data)) {
			return (FALSE);
		}
		break;
	case 11:
		if (!xdr_DevVarLongArray(xdrs, &objp->u.VLA_data)) {
			return (FALSE);
		}
		break;
	case 12:
		if (!xdr_DevVarFloatArray(xdrs, &objp->u.VFA_data)) {
			return (FALSE);
		}
		break;
	case 13:
		if (!xdr_DevBpmPosField(xdrs, &objp->u.BPMPF_data)) {
			return (FALSE);
		}
		break;
	case 14:
		if (!xdr_DevBpmElecField(xdrs, &objp->u.BPMEF_data)) {
			return (FALSE);
		}
		break;
	case 15:
		if (!xdr_DevWsBeamFitParm(xdrs, &objp->u.WSBFP_data)) {
			return (FALSE);
		}
		break;
	}
	return (TRUE);
}



