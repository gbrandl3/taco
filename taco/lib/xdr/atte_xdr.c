/*+*******************************************************************

 File:          atte_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines

 Author(s):     Jens Meyer
		$Author: jkrueger1 $

 Original:      Nov 1991

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France
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


