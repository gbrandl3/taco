/*+*******************************************************************

 File:          pss_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for Personal Safety System data types.

 Author(s):	Jens Meyer
		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/


#include <dev_xdr.h>
#include <pss_xdr.h>



/*
 *  xdr_routines for personal safety system
 */

bool_t
xdr_PSSInterlockStatus(xdrs, objp)
	XDR *xdrs;
	PSSInterlockStatus *objp;
{
	if (!xdr_int(xdrs, &objp->Module)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardA)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardB)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->GuardC)) {
		return (FALSE);
	}
	if (!xdr_u_short(xdrs, &objp->Interlocks)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_PSSInterlockStatus(objp)
	PSSInterlockStatus *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong ((long *)&objp->Module);
        length = length + xdr_length_DevLong ((long *)&objp->GuardA);
        length = length + xdr_length_DevLong ((long *)&objp->GuardB);
        length = length + xdr_length_DevLong ((long *)&objp->GuardC);
	length = length + xdr_length_DevShort((short *)&objp->Interlocks);

        return (length);

}





bool_t
xdr_DevDaresburyStatus(xdrs, objp)
	XDR *xdrs;
	DevDaresburyStatus *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(PSSInterlockStatus), 
	    (xdrproc_t)xdr_PSSInterlockStatus)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevDaresburyStatus(objp)
	DevDaresburyStatus *objp;
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
                 xdr_length_PSSInterlockStatus(&objp->sequence[0]) );

        return (length);
}
