
/*+*******************************************************************

 File:          atte_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines

 Author(s):     Jens Meyer

 Original:      Nov 1991

$Revision: 1.1 $
$Date: 2003-04-25 11:21:45 $

$Author: jkrueger1 $

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


