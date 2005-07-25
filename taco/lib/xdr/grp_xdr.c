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
 * File:        grp_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for  data types.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#include <dev_xdr.h>
#include <grp_xdr.h>

bool_t
xdr_Frame(xdrs, objp)
	XDR *xdrs;
	Frame *objp;
{
	if (!xdr_double(xdrs, &objp->value)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->output)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->pause)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_Frame(objp)
	Frame *objp;
{
        long  length = 0;

	length = length + xdr_length_DevDouble (&objp->value);
        length = length + xdr_length_DevShort (&objp->output);
        length = length + xdr_length_DevShort (&objp->pause);

        return (length);

}



bool_t
xdr_FramePair(xdrs, objp)
	XDR *xdrs;
	FramePair *objp;
{
	if (!xdr_Frame(xdrs, &objp->dead)) {
		return (FALSE);
	}
	if (!xdr_Frame(xdrs, &objp->live)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_FramePair(objp)
	FramePair *objp;
{
        long  length = 0;

        length = length + xdr_length_Frame (&objp->dead);
        length = length + xdr_length_Frame (&objp->live);

        return (length);

}




bool_t
xdr_DevGrpFramePair(xdrs, objp)
	XDR *xdrs;
	DevGrpFramePair *objp;
{
	if (!xdr_u_int(xdrs, &objp->nb_framepair)) {
		return (FALSE);
	}
	if (!xdr_FramePair(xdrs, &objp->framepair)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_DevGrpFramePair(objp)
	DevGrpFramePair *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong ((long*)&objp->nb_framepair);
        length = length + xdr_length_FramePair (&objp->framepair);

        return (length);

}



bool_t
xdr_DevGrpFramePairArray(xdrs, objp)
	XDR *xdrs;
	DevGrpFramePairArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevGrpFramePair), 
	    (xdrproc_t)xdr_DevGrpFramePair)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevGrpFramePairArray(objp)
	DevGrpFramePairArray *objp;
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
                 xdr_length_DevGrpFramePair(&objp->sequence[0]) );

        return (length);
}
