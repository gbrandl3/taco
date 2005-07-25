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
 * File:        atte_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    Nov 1991
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#include  <dev_xdr.h>
#include  <atte_xdr.h>

bool_t xdr_DevAtte(xdrs, objp)
        XDR *xdrs;
        DevAtte *objp;
{
        if (!xdr_short(xdrs, &objp->number)) {
                return (FALSE);
        }
        if (!xdr_short(xdrs, &objp->filter)) {
                return (FALSE);
        }
        return (TRUE);
}


long
xdr_length_DevAtte(objp)
        DevAtte *objp;
{
        long  length = 0;

        length = length + xdr_length_DevShort (&objp->number);
        length = length + xdr_length_DevShort (&objp->filter);

        return (length);
}


