
/*+*******************************************************************

 File:          grp_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for  data types.

 Author(s):	Jens Meyer

 Original:	July 1992

$Revision: 1.1 $
$Date: 2003-04-25 11:21:46 $

$Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/


#include <dev_xdr.h>
#include <grp_xdr.h>



bool_t
xdr_Frame(xdrs, objp)
	XDR *xdrs;
	Frame *objp;
{
	if (!xdr_double(xdrs, &objp->value)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->output)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->pause)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_Frame(objp)
	Frame *objp;
{
        long  length = 0;

	length = length + xdr_length_DevDouble (&objp->value);
        length = length + xdr_length_DevShort (&objp->output);
        length = length + xdr_length_DevShort (&objp->pause);

        return (length);

}



bool_t
xdr_FramePair(xdrs, objp)
	XDR *xdrs;
	FramePair *objp;
{
	if (!xdr_Frame(xdrs, &objp->dead)) {
		return (FALSE);
	}
	if (!xdr_Frame(xdrs, &objp->live)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_FramePair(objp)
	FramePair *objp;
{
        long  length = 0;

        length = length + xdr_length_Frame (&objp->dead);
        length = length + xdr_length_Frame (&objp->live);

        return (length);

}




bool_t
xdr_DevGrpFramePair(xdrs, objp)
	XDR *xdrs;
	DevGrpFramePair *objp;
{
	if (!xdr_u_int(xdrs, &objp->nb_framepair)) {
		return (FALSE);
	}
	if (!xdr_FramePair(xdrs, &objp->framepair)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_DevGrpFramePair(objp)
	DevGrpFramePair *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong ((long*)&objp->nb_framepair);
        length = length + xdr_length_FramePair (&objp->framepair);

        return (length);

}



bool_t
xdr_DevGrpFramePairArray(xdrs, objp)
	XDR *xdrs;
	DevGrpFramePairArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevGrpFramePair), 
	    (xdrproc_t)xdr_DevGrpFramePair)) {
		return (FALSE);
	}
	return (TRUE);
}


long
xdr_length_DevGrpFramePairArray(objp)
	DevGrpFramePairArray *objp;
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
                 xdr_length_DevGrpFramePair(&objp->sequence[0]) );

        return (length);
}
