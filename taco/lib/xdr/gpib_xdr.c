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
 * File:        gpib_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for Gpib types.
 *
 * Author(s):   Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    January 1992
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
#include <gpib_xdr.h>

bool_t
xdr_DevGpibWrite(xdrs, objp)
        XDR *xdrs;
        DevGpibWrite *objp;
{
        if (!xdr_short(xdrs, &objp->number)) {
                return (FALSE);
        }
        if (!xdr_string(xdrs, &objp->msg_str, MAXU_INT)) {
                return (FALSE);
        }
        return (TRUE);
}



bool_t
xdr_DevGpibMulWrite(xdrs, objp)
        XDR *xdrs;
        DevGpibMulWrite *objp;
{
        if (!xdr_DevVarShortArray(xdrs, &objp->number_arr)) {
                return (FALSE);
        }
        if (!xdr_string(xdrs, &objp->msg_str, MAXU_INT)) {
                return (FALSE);
        }
        return (TRUE);
}



bool_t
xdr_DevGpibLoc(xdrs,objp)
       XDR *xdrs;
       DevGpibLoc *objp;
{
        if (!xdr_char(xdrs, &objp->slave)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->startCtrl)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->fullAddr)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->sendREN)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->serialPoll)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->paraPoll)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->autoRead)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->autoSRQ)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->defStat)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->writeStat)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->waitREN)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->res0)) {
                return (FALSE);
        }
        return (TRUE);
}



bool_t
xdr_DevGpibRes(xdrs,objp)
       XDR *xdrs;
       DevGpibRes *objp;
{
        if (!xdr_short(xdrs, &objp->number)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->sendDC)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->DoPPoll)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->readMode)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->readEnd1)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->readEnd2)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->writeMode)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->writeEnd1)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->writeEnd2)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->primAddr)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->readStat)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->readMask)) {
                return (FALSE);
        }
        if (!xdr_char(xdrs, &objp->res1)) {
                return (FALSE);
        }
        if (!xdr_DevLong(xdrs, &objp->frameCnt)) {
                return (FALSE);
        }
        return (TRUE);
}




