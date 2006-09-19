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
 * File:        daemon_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for update daemon types.
 *
 * Author(s):   Jens Meyer
 *              $Author: andy_gotz $
 *
 * Original:    July 1992
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-19 09:29:39 $
 */

static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/daemon_xdr.c,v 1.3 2006-09-19 09:29:39 andy_gotz Exp $";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <dev_xdr.h>
#include <daemon_xdr.h>

#ifdef unix

#include <stdlib.h>
#include <string.h>

#endif


bool_t
xdr_DevDaemonStatus(XDR *xdrs, DevDaemonStatus *objp)
{
	if (!xdr_char(xdrs, &objp->BeingPolled)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->PollFrequency)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->TimeLastPolled)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->PollMode)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->DeviceAccessError)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->ErrorCode)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->LastCommandStatus)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->ContinueAfterError)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevDaemonStatus(DevDaemonStatus *objp)
        {
        long  length = 0;

        length = length + xdr_length_DevChar (&objp->BeingPolled);
        length = length + xdr_length_DevLong (&objp->PollFrequency);
        length = length + xdr_length_DevLong (&objp->TimeLastPolled);
        length = length + xdr_length_DevChar (&objp->PollMode);
        length = length + xdr_length_DevChar (&objp->DeviceAccessError);
        length = length + xdr_length_DevLong (&objp->ErrorCode);
        length = length + xdr_length_DevLong (&objp->LastCommandStatus);
        length = length + xdr_length_DevChar (&objp->ContinueAfterError);

        return (length);
}



bool_t
xdr_DevDaemonData(XDR *xdrs, DevDaemonData *objp)
{
	if (!xdr_long(xdrs, &objp->ddid)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->long_data)) {
		return (FALSE);
	}

	return (TRUE);
}

long
xdr_length_DevDaemonData(DevDaemonData *objp)
        {
        long  length = 0;

        length = length + xdr_length_DevLong (&objp->ddid);
        length = length + xdr_length_DevLong (&objp->long_data);

        return (length);
}



/* Structure for new device process resources */

bool_t
xdr_DevDaemonStruct(XDR *xdrs, DevDaemonStruct *objp)
{
	if (!xdr_wrapstring(xdrs, &objp->dev_n)) 
	{
		return (FALSE);
	}
	if (!xdr_DevVarStringArray(xdrs,&objp->cmd_list))
        {
		return (FALSE);
	}  
	if (!xdr_long(xdrs, &objp->poller_frequency)) 
	{
		return (FALSE);
	}

	return (TRUE);
}


long
xdr_length_DevDaemonStruct(DevDaemonStruct *objp)
        {
        long  length = 0;

        length = length + xdr_length_DevChar (&objp->dev_n[0]);
        length = length + xdr_length_DevVarStringArray (&objp->cmd_list);  
        length = length + xdr_length_DevLong (&objp->poller_frequency);

        return (length);
}


