/*+*******************************************************************

 File:          mstat_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   Source file for the XDR definitions
		for the ESRF machine status data type.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	March 1993

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <mstat_xdr.h>


/*
 *  definition for the ESRF machine status array
 */

bool_t
xdr_SysState(xdrs, objp)
	XDR *xdrs;
	SysState *objp;
{
	if (!xdr_string(xdrs, &objp->sys_name, MAXU_INT)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->sys_ident)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->sys_state)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_SysState(objp)
	SysState *objp;
{
        long  length = 0;

        length = length + xdr_length_DevString (&objp->sys_name);
        length = length + xdr_length_DevLong (&objp->sys_ident);
        length = length + xdr_length_DevLong (&objp->sys_state);

        return (length);
}


bool_t xdr_VarSysStateArray(xdrs, objp)
        XDR *xdrs;
        VarSysStateArray *objp;
{
        if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
	     (u_int *)&objp->length, MAXU_INT, sizeof(SysState), (xdrproc_t)xdr_SysState)) {
                return (FALSE);
	}
        return (TRUE);
}

long
xdr_length_VarSysStateArray(objp)
	VarSysStateArray *objp;
{
        long    length = 0;
	long	i;

        /*
         *  four bytes for the number of array elements
         */

	length = length + xdr_length_DevLong ((long *)&objp->length);

        /*
         *  now calculate the length of the array
         */

	for (i=0; (u_long)i<objp->length; i++)
	   {
	   length = length + xdr_length_SysState(&objp->sequence[i]);
	   }

        return (length);
}
