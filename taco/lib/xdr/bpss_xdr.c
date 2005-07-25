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
 * File:        bpss_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for bpss powersupply types.
 *
 * Author(s):   Jean Michel Chaize
 *		$Author: jkrueger1 $
 *
 * Original:    December 1991
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#include <dev_xdr.h>
#include <bpss_xdr.h>


bool_t xdr_DevStateIndic(xdrs, objp)
	XDR *xdrs;
	DevStateIndic *objp;
{
	if (!xdr_short(xdrs, &objp->State))  return (FALSE);
	if (!xdr_short(xdrs, &objp->Indic))  return (FALSE);
	return (TRUE);
}

long
xdr_length_DevStateIndic(objp)
	DevStateIndic *objp;
{
        long  length = 0;

	length = length + xdr_length_DevShort (&objp->State);
	length = length + xdr_length_DevShort (&objp->Indic);

	return (length);
}




bool_t xdr_DevBpssState(xdrs, objp)
        XDR *xdrs;
	DevBpssState *objp;
{
        if (!xdr_short(xdrs, &objp->Ac))  return (FALSE);
        if (!xdr_short(xdrs, &objp->Dc))  return (FALSE);
        return (TRUE);
}

long
xdr_length_DevBpssState(objp)
	DevBpssState *objp;
{
        long  length = 0;

	length = length + xdr_length_DevShort (&objp->Ac);
	length = length + xdr_length_DevShort (&objp->Dc);

	return (length);
}





bool_t xdr_DevBpssLine(xdrs,objp)
	XDR *xdrs;
        DevBpssLine *objp;
{
	if (!xdr_float (xdrs, &objp->Set    ))  return (FALSE);
	if (!xdr_float (xdrs, &objp->Read   ))  return (FALSE);
	if (!xdr_short (xdrs, &objp->Control))  return (FALSE);
	if (!xdr_short (xdrs, &objp->Alarm  ))  return (FALSE);
	return (TRUE);
}

long
xdr_length_DevBpssLine(objp)
        DevBpssLine *objp;
{
        long  length = 0;

	length = length + xdr_length_DevFloat (&objp->Set);
	length = length + xdr_length_DevFloat (&objp->Read);
	length = length + xdr_length_DevShort (&objp->Control);
	length = length + xdr_length_DevShort (&objp->Alarm);

	return (length);
}




bool_t xdr_DevBpssReadPoint(xdrs, objp)
        XDR *xdrs;
        DevBpssReadPoint *objp;
{
        if (!xdr_DevBpssLine ( xdrs, &objp->IACinv ))  return (FALSE);
        if (!xdr_DevBpssLine ( xdrs, &objp->IACmag ))  return (FALSE);
        if (!xdr_DevBpssLine ( xdrs, &objp->IDCmag ))  return (FALSE);
        if (!xdr_DevBpssLine ( xdrs, &objp->Period ))  return (FALSE);
        if (!xdr_DevBpssLine ( xdrs, &objp->PhLag ))   return (FALSE);
	if (!xdr_float (xdrs, &objp->PhLagUI    ))  return (FALSE);
	if (!xdr_float (xdrs, &objp->VACinv    ))  return (FALSE);
	if (!xdr_float (xdrs, &objp->VDCmag    ))  return (FALSE);
	if (!xdr_float (xdrs, &objp->CapTemp    ))  return (FALSE);
	if (!xdr_float (xdrs, &objp->ChokeTemp    ))  return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->Temperature, 8, sizeof(float),
					(xdrproc_t)xdr_float)) return (FALSE);
        if (!xdr_DevBpssState( xdrs, &objp->State  ))  return (FALSE);
        if (!xdr_DevBpssState( xdrs, &objp->ExtState  ))  return (FALSE);
	if (!xdr_vector(xdrs, (char *)objp->Faults, 32, sizeof(DevBpssState),
					(xdrproc_t)xdr_DevBpssState)) return (FALSE);
        if (!xdr_short( xdrs, &objp->Master ))  return (FALSE);
        return (TRUE);
}

long
xdr_length_DevBpssReadPoint(objp)
        DevBpssReadPoint *objp;
{
        long  length = 0;

	length = length + xdr_length_DevBpssLine (&objp->IACinv);
	length = length + xdr_length_DevBpssLine (&objp->IACmag);
	length = length + xdr_length_DevBpssLine (&objp->IDCmag);
	length = length + xdr_length_DevBpssLine (&objp->Period);
	length = length + xdr_length_DevBpssLine (&objp->PhLag);

	length = length + xdr_length_DevFloat (&objp->PhLagUI);
	length = length + xdr_length_DevFloat (&objp->VACinv);
	length = length + xdr_length_DevFloat (&objp->VDCmag);
	length = length + xdr_length_DevFloat (&objp->CapTemp);
	length = length + xdr_length_DevFloat (&objp->ChokeTemp);

	length = length + (8 * xdr_length_DevFloat (&objp->Temperature[0]));

	length = length + xdr_length_DevBpssState (&objp->State);
	length = length + xdr_length_DevBpssState (&objp->ExtState);
	length = length + (32 * xdr_length_DevBpssState (&objp->Faults[0]));

	length = length + xdr_length_DevShort (&objp->Master);

	return (length);
}
