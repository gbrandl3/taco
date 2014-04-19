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
 * File:        daemon_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for update daemon types.
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    July 1992
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
#include <daemon_xdr.h>

bool_t
xdr_DevDaemonStatus(xdrs, objp)
	XDR *xdrs;
	DevDaemonStatus *objp;
{
	if (!xdr_char(xdrs, &objp->BeingPolled)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->PollFrequency)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->TimeLastPolled)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->PollMode)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->DeviceAccessError)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->ErrorCode)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->LastCommandStatus)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->ContinueAfterError)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevDaemonStatus(objp)
        DevDaemonStatus *objp;
        {
        long  length = 0;

        length += xdr_length_DevChar (&objp->BeingPolled);
        length += xdr_length_DevLong (&objp->PollFrequency);
        length += xdr_length_DevLong (&objp->TimeLastPolled);
        length += xdr_length_DevChar (&objp->PollMode);
        length += xdr_length_DevChar (&objp->DeviceAccessError);
        length += xdr_length_DevLong (&objp->ErrorCode);
        length += xdr_length_DevLong (&objp->LastCommandStatus);
        length += xdr_length_DevChar (&objp->ContinueAfterError);

        return (length);
}



bool_t
xdr_DevDaemonData(xdrs, objp)
	XDR *xdrs;
	DevDaemonData *objp;
{
	if (!xdr_DevLong(xdrs, &objp->ddid)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->long_data)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->string_data, ~0)) {
		return (FALSE);
	}

	return (TRUE);
}

long
xdr_length_DevDaemonData(objp)
        DevDaemonData *objp;
        {
        long  length = 0;

        length += xdr_length_DevLong (&objp->ddid);
        length += xdr_length_DevLong (&objp->long_data);
        length += xdr_length_DevString (&objp->string_data);

        return (length);
}
