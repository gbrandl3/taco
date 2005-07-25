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
 * File:          haz_xdr.c
 *
 * Project:       Device Servers with sun-rpc
 *
 * Description:   xdr_routines for hazemeyer powersupply types.
 *
 * Author(s):     Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:      January 1991
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:		$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#include <dev_xdr.h>
#include <haz_xdr.h>


bool_t
xdr_DevHazStatus(xdrs, objp)
	XDR *xdrs;
	DevHazStatus *objp;
{
	if (!xdr_char(xdrs, &objp->Power)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Remote)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Aux)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Stat)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Phase)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Mains)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->PassivFilt)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->ActivFilt)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Doors)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Synchro)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->IMains)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Loop)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Waterflow)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->OverTemp)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->DCCTsat)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->OverVolt)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->OverCurrent)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->EarthFault)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->User1)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->User2)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->User3)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->Transformer)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->TOilTemp)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevHazStatus(objp)
        DevHazStatus *objp;
        {
        long  length = 0;

        length = length + xdr_length_DevChar (&objp->Power);
        length = length + xdr_length_DevChar (&objp->Remote);
        length = length + xdr_length_DevChar (&objp->Aux);
        length = length + xdr_length_DevChar (&objp->Stat);
        length = length + xdr_length_DevChar (&objp->Phase);
        length = length + xdr_length_DevChar (&objp->Mains);
        length = length + xdr_length_DevChar (&objp->PassivFilt);
        length = length + xdr_length_DevChar (&objp->ActivFilt);
        length = length + xdr_length_DevChar (&objp->Doors);
        length = length + xdr_length_DevChar (&objp->Synchro);
        length = length + xdr_length_DevChar (&objp->IMains);
        length = length + xdr_length_DevChar (&objp->Loop);
        length = length + xdr_length_DevChar (&objp->Waterflow);
        length = length + xdr_length_DevChar (&objp->OverTemp);
        length = length + xdr_length_DevChar (&objp->DCCTsat);
        length = length + xdr_length_DevChar (&objp->OverVolt);
        length = length + xdr_length_DevChar (&objp->OverCurrent);
        length = length + xdr_length_DevChar (&objp->EarthFault);
        length = length + xdr_length_DevChar (&objp->User1);
        length = length + xdr_length_DevChar (&objp->User2);
        length = length + xdr_length_DevChar (&objp->User3);
        length = length + xdr_length_DevChar (&objp->Transformer);
        length = length + xdr_length_DevChar (&objp->TOilTemp);

        return (length);
}


