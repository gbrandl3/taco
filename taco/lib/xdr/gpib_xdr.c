
/*+*******************************************************************

 File:          gpib_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for Gpib types.

 Author(s):     Jens Meyer

 Original:      January 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:46 $

$Author: jkrueger1 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France

*******************************************************************-*/

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
        if (!xdr_long(xdrs, &objp->frameCnt)) {
                return (FALSE);
        }
        return (TRUE);
}




