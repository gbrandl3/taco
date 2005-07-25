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
 * File:        ram_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for Radiation Monitor data types.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#include <dev_xdr.h>
#include <ram_xdr.h>


/*
 *  xdr_routines for NE Technologies Gamma- and Neutron Monitor types
 */

bool_t
xdr_DevNeg14StatusRec(xdrs, objp)
	XDR *xdrs;
	DevNeg14StatusRec *objp;
{
	if (!xdr_int(xdrs, &objp->unit_type)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->unitrange)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->samplermode)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->last_update)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->monitorerror)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->msgalarm)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->unitstatus)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->unitfaultcode)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->checksum, UNIT_MSG_SIZE, 
			sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->aritherrorcode, UNIT_MSG_SIZE, 
			sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->alarmstatus)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->doserate)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->lowalarmth)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->highalarmth)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->alertalarmth)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->primingdoserate)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->AlarmResponseOpts, UNIT_MSG_SIZE, 
			sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevNeg14StatusRec(objp)
	DevNeg14StatusRec *objp;
{
	long 	length = 0;

        length = length + xdr_length_DevLong ((long *)&objp->unit_type);
        length = length + xdr_length_DevFloat (&objp->unitrange);
        length = length + xdr_length_DevLong (&objp->samplermode);
	length = length + xdr_length_DevLong ((long *)&objp->last_update);
        length = length + xdr_length_DevLong (&objp->monitorerror);
        length = length + xdr_length_DevChar (&objp->msgalarm);
        length = length + xdr_length_DevLong (&objp->unitstatus);
        length = length + xdr_length_DevLong (&objp->unitfaultcode);
	length = length + 
	(UNIT_MSG_SIZE * xdr_length_DevChar (&objp->checksum[0]));
	length = length + 
	(UNIT_MSG_SIZE * xdr_length_DevChar (&objp->aritherrorcode[0]));
        length = length + xdr_length_DevLong (&objp->alarmstatus);
        length = length + xdr_length_DevFloat (&objp->doserate);
        length = length + xdr_length_DevDouble (&objp->lowalarmth);
        length = length + xdr_length_DevDouble (&objp->highalarmth);
        length = length + xdr_length_DevDouble (&objp->alertalarmth);
        length = length + xdr_length_DevDouble (&objp->primingdoserate);
	length = length + 
	(UNIT_MSG_SIZE * xdr_length_DevChar (&objp->AlarmResponseOpts[0]));

	return (length);
}


bool_t
xdr_DevRadiationDoseValue(xdrs, objp)
	XDR *xdrs;
	DevRadiationDoseValue *objp;
{
	if (!xdr_short(xdrs, &objp->state)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->type)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->read)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevRadiationDoseValue(objp)
	DevRadiationDoseValue *objp;
{
	long 	length = xdr_length_DevShort (&objp->state);
        length += xdr_length_DevShort (&objp->type);
        length += xdr_length_DevFloat (&objp->read);

	return (length);
}

