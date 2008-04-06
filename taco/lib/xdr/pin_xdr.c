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
 * File:        pin_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr structure to read without argin argument the whole
 *              state of the Pinhole device.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	June 1993
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
#include <pin_xdr.h>

bool_t
xdr_pin_head(xdrs, objp)
	XDR *xdrs;
	pin_head *objp;
{
	if (!xdr_DevLong(xdrs, &objp->main_st)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->dev_diag)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->tuned)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_pin_head(objp)
	pin_head *objp;
{
        long  length = 0;

        length += xdr_length_DevLong (&objp->main_st);
        length += xdr_length_DevLong (&objp->dev_diag);
        length += xdr_length_DevLong (&objp->tuned);
        length += xdr_length_DevLong (&objp->mode);

        return (length);
}




bool_t
xdr_axis_state(xdrs, objp)
	XDR *xdrs;
	axis_state *objp;
{
	if (!xdr_DevLong(xdrs, &objp->unit)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->moving)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->mot_pos)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->enc_pos)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->switch_st)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->tuned)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_axis_state(objp)
	axis_state *objp;
{
        long  length = 0;

        length += xdr_length_DevLong (&objp->unit);
        length += xdr_length_DevLong (&objp->moving);
        length += xdr_length_DevFloat (&objp->mot_pos);
        length += xdr_length_DevFloat (&objp->enc_pos);
        length += xdr_length_DevLong (&objp->switch_st);
        length += xdr_length_DevLong (&objp->tuned);

        return (length);
}




bool_t
xdr_DevPinState(xdrs, objp)
	XDR *xdrs;
	DevPinState *objp;
{
	if (!xdr_pin_head(xdrs, &objp->pin_head)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->z1_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->z2_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->tx_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->tt_state)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_DevPinState(objp)
	DevPinState *objp;
{
        long  length = 0;

        length += xdr_length_pin_head (&objp->pin_head);
        length += xdr_length_axis_state (&objp->z1_state);
        length += xdr_length_axis_state (&objp->z2_state);
        length += xdr_length_axis_state (&objp->tx_state);
        length += xdr_length_axis_state (&objp->tt_state);

        return (length);
}


