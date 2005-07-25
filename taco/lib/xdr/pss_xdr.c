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
 * File:        pss_xdr.c
 * 
 * Project:     Device Servers with sun-rpc
 * 
 * Description: xdr_routines for Personal Safety System data types.
 * 
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 * 
 * Original:	July 1992
 * 
 * Version:	$Revision: 1.3 $
 * 
 * Date:	$Date: 2005-07-25 13:05:45 $
 * 
 *******************************************************************-*/

#include <dev_xdr.h>
#include <pss_xdr.h>



/*
 *  xdr_routines for personal safety system
 */

bool_t
xdr_PSSInterlockStatus(xdrs, objp)
	XDR *xdrs;
	PSSInterlockStatus *objp;
{
	if (!xdr_int(xdrs, &objp->Module)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardA)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardB)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardC)) {
		return (FALSE);
	}
	if (!xdr_u_short(xdrs, &objp->Interlocks)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_PSSInterlockStatus(objp)
	PSSInterlockStatus *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong ((long *)&objp->Module);
        length = length + xdr_length_DevLong ((long *)&objp->GuardA);
        length = length + xdr_length_DevLong ((long *)&objp->GuardB);
        length = length + xdr_length_DevLong ((long *)&objp->GuardC);
	length = length + xdr_length_DevShort((short *)&objp->Interlocks);

        return (length);

}





bool_t
xdr_DevDaresburyStatus(xdrs, objp)
	XDR *xdrs;
	DevDaresburyStatus *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(PSSInterlockStatus), 
	    (xdrproc_t)xdr_PSSInterlockStatus)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevDaresburyStatus(objp)
	DevDaresburyStatus *objp;
{
        long    length = 0;

        /*
         *  four bytes for the number of array elements
         */

        length = length + xdr_length_DevLong ((long *)&objp->length);

        /*
         *  now calculate the length of the array
         */

        length = length + (objp->length *
                 xdr_length_PSSInterlockStatus(&objp->sequence[0]) );

        return (length);
}
