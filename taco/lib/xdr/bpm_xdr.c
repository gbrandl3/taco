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
 * File:        bpm_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for beam position monitor types.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <dev_xdr.h>
#include <bpm_xdr.h>

/*
 *  xdr for beam position monitor types
 */

bool_t
xdr_DevBpmElec(xdrs, objp)
	XDR *xdrs;
	DevBpmElec objp;
{
	if (!xdr_vector(xdrs, (char *)objp, 4, sizeof(float), (xdrproc_t)xdr_float)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmElec (objp)
        DevBpmElec objp;
{
        long    length = 0;

        length += (4 * xdr_length_DevFloat (&objp[0]));

        return (length);
}



bool_t
xdr_DevBpmMeasHead(xdrs, objp)
	XDR *xdrs;
	DevBpmMeasHead *objp;
{
	if (!xdr_u_short(xdrs, &objp->mn)) {
		return (FALSE);
	}
	if (!xdr_u_short(xdrs, &objp->gms)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->momint.sequence, (u_int *)&objp->momint.length, MAXU_INT, sizeof(DevBpmElec), (xdrproc_t)xdr_DevBpmElec)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmMeasHead (objp)
        DevBpmMeasHead *objp;
{
        long    length = 0;

        length += xdr_length_DevShort (&objp->mn);
        length += xdr_length_DevShort (&objp->gms);

        /*
         *  four bytes for the number of array elements
         */
        length += xdr_length_DevLong (&objp->momint.length);
        /*
         *  now calculate the length of the array
         */
        length += (objp->momint.length * xdr_length_DevBpmElec( objp->momint.sequence[0]) );

        return (length);
}



bool_t
xdr_DevBpmPos(xdrs, objp)
	XDR *xdrs;
	DevBpmPos *objp;
{
	if (!xdr_u_short(xdrs, &objp->mms)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->xcoord)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->zcoord)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmPos (objp)
        DevBpmPos *objp;
{
        long    length = 0;

        length += xdr_length_DevShort ((short *)&objp->mms);
        length += xdr_length_DevFloat (&objp->xcoord);
        length += xdr_length_DevFloat (&objp->zcoord);

        return (length);
}



bool_t
xdr_DevBpmPosMeasure(xdrs, objp)
	XDR *xdrs;
	DevBpmPosMeasure *objp;
{
	if (!xdr_DevBpmMeasHead(xdrs, &objp->meashead)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->posdata.sequence, (u_int *)&objp->posdata.length, MAXU_INT, sizeof(DevBpmPos), (xdrproc_t)xdr_DevBpmPos)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmPosMeasure (objp)
        DevBpmPosMeasure *objp;
{
        long    length = 0;

        length += xdr_length_DevBpmMeasHead (&objp->meashead);

        /*
         *  four bytes for the number of array elements
         */
        length += xdr_length_DevLong (&objp->posdata.length);
        /*
         *  now calculate the length of the array
         */
        length += (objp->posdata.length * xdr_length_DevBpmPos(&objp->posdata.sequence[0]) );

        return (length);
}



bool_t
xdr_DevBpmElecMeasure(xdrs, objp)
	XDR *xdrs;
	DevBpmElecMeasure *objp;
{
	if (!xdr_DevBpmMeasHead(xdrs, &objp->meashead)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->elecdata.sequence, (u_int *)&objp->elecdata.length, MAXU_INT, sizeof(DevBpmElec), (xdrproc_t)xdr_DevBpmElec)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmElecMeasure (objp)
        DevBpmElecMeasure *objp;
{
        long    length = 0;

        length += xdr_length_DevBpmMeasHead (&objp->meashead);

        /*
         *  four bytes for the number of array elements
         */
        length += xdr_length_DevLong (&objp->elecdata.length);
        /*
         *  now calculate the length of the array
         */
        length += (objp->elecdata.length * xdr_length_DevBpmElec( objp->elecdata.sequence[0]) );

        return (length);
}



bool_t
xdr_DevBpmMeasParm(xdrs, objp)
	XDR *xdrs;
	DevBpmMeasParm *objp;
{
	if (!xdr_DevLong(xdrs, &objp->expbeamint)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->timewind)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->delaytim, 6, 
			sizeof(long), (xdrproc_t)xdr_DevLong)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmMeasParm (objp)
        DevBpmMeasParm *objp;
{
        long    length = 0;

        length += xdr_length_DevLong (&objp->expbeamint);
        length += xdr_length_DevLong (&objp->timewind);
        length += (6 * xdr_length_DevLong (&objp->delaytim[0]));

        return (length);
}




bool_t
xdr_DevBpmInjectHead(xdrs, objp)
	XDR *xdrs;
	DevBpmInjectHead *objp;
{
	if (!xdr_u_short(xdrs, &objp->min)) {
		return (FALSE);
	}
	if (!xdr_DevULong(xdrs, &objp->gis)) {
		return (FALSE);
	}
	if (!xdr_DevBpmMeasParm(xdrs, &objp->bpm_measpar)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmInjectHead (objp)
        DevBpmInjectHead *objp;
{
        long    length = 0;

        length += xdr_length_DevShort (&objp->min);
        length += xdr_length_DevLong (&objp->gis);
        length += xdr_length_DevBpmMeasParm (&objp->bpm_measpar);

        return (length);
}



bool_t
xdr_DevBpmPosField(xdrs, objp)
	XDR *xdrs;
	DevBpmPosField *objp;
{
	if (!xdr_DevBpmInjectHead(xdrs, &objp->injhead)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->posmeas.sequence, 
			     (u_int *)&objp->posmeas.length, 
			     MAXU_INT, sizeof(DevBpmPosMeasure), 
			     (xdrproc_t)xdr_DevBpmPosMeasure)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmPosField (objp)
        DevBpmPosField *objp;
{
        long    length = 0;

        length += xdr_length_DevBpmInjectHead (&objp->injhead);

        /*
         *  four bytes for the number of array elements
         */
        length += xdr_length_DevLong (&objp->posmeas.length);
        /*
         *  now calculate the length of the array
         */
        length += (objp->posmeas.length * xdr_length_DevBpmPosMeasure(&objp->posmeas.sequence[0]) );

        return (length);
}




bool_t
xdr_DevBpmElecField(xdrs, objp)
	XDR *xdrs;
	DevBpmElecField *objp;
{
	if (!xdr_DevBpmInjectHead(xdrs, &objp->injhead)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->elecmeas.sequence, 
			     (u_int *)&objp->elecmeas.length, 
			     MAXU_INT, sizeof(DevBpmElecMeasure), 
			     (xdrproc_t)xdr_DevBpmElecMeasure)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevBpmElecField (objp)
        DevBpmElecField *objp;
{
        long    length = 0;

        length += xdr_length_DevBpmInjectHead (&objp->injhead);

        /*
         *  four bytes for the number of array elements
         */
	length += xdr_length_DevLong (&objp->elecmeas.length);
        /*
         *  now calculate the length of the array
         */
        length += (objp->elecmeas.length * xdr_length_DevBpmElecMeasure(&objp->elecmeas.sequence[0]) );

        return (length);
}

