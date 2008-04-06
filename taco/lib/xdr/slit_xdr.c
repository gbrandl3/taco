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
 * File:        slit_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for SLIT types.
 *
 * Author(s):   Timo Mett\"al\"a
 *		$Author: jkrueger1 $
 *
 * Original:    September 1992
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
#include <slit_xdr.h>


bool_t xdr_pslit_head(xdrs, objp)
        XDR *xdrs;
        pslit_head *objp;
{
        if (!xdr_DevLong(xdrs, &objp->pmain_st))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->dev_diag))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->unit))
                return (FALSE);

        return (TRUE);
}


bool_t xdr_blade_state(xdrs, objp)
        XDR *xdrs;
        blade_state *objp;
{
        if (!xdr_DevLong(xdrs, &objp->values_st))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->mot_moving))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->mot_pos))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->enc_pos))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->brake))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->switch_st))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->tuned))
                return (FALSE);
        return (TRUE);
}


bool_t xdr_DevBladeState(xdrs, objp)
        XDR *xdrs;
        DevBladeState *objp;
{
        if (!xdr_pslit_head(xdrs, &objp->pslit_head))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->up_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->do_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->fr_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->ba_state))
                return (FALSE); 
        return (TRUE);
}

bool_t xdr_pslit_state(xdrs, objp)
        XDR *xdrs;
        pslit_state *objp;
{
        if (!xdr_DevLong(xdrs, &objp->values_st))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->gap))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->offset))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp_1))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp_2))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->brake_1))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->brake_2))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->switch_1))
                return (FALSE);
        if (!xdr_DevLong(xdrs, &objp->switch_2))
                return (FALSE);
        return (TRUE);
}


bool_t xdr_DevPslitState(xdrs, objp)
        XDR *xdrs;
        DevPslitState *objp;
{
        if (!xdr_pslit_head(xdrs, &objp->pslit_head))
                return (FALSE);
        if (!xdr_pslit_state(xdrs, &objp->vertical))
                return (FALSE);
        if (!xdr_pslit_state(xdrs, &objp->horizontal))
                return (FALSE);
        return (TRUE);
}

