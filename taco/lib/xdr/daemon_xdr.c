
/*+*******************************************************************

 File:          daemon_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for update daemon types.

 Author(s):     Jens Meyer

 Original:      July 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:46 $

$Author: jkrueger1 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

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
xdr_length_DevDaemonStatus(objp)
        DevDaemonStatus *objp;
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
xdr_DevDaemonData(xdrs, objp)
	XDR *xdrs;
	DevDaemonData *objp;
{
	if (!xdr_long(xdrs, &objp->ddid)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->long_data)) {
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

        length = length + xdr_length_DevLong (&objp->ddid);
        length = length + xdr_length_DevLong (&objp->long_data);
        length = length + xdr_length_DevString (&objp->string_data);

        return (length);
}
