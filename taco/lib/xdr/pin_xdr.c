
/*+*******************************************************************

 File:          pin_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr structure to read without argin argument the whole
                state of the Pinhole device.

 Author(s):	Jens Meyer

 Original:	June 1993

$Revision: 1.1 $
$Date: 2003-04-25 11:21:47 $

$Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/


#include <dev_xdr.h>
#include <pin_xdr.h>



bool_t
xdr_pin_head(xdrs, objp)
	XDR *xdrs;
	pin_head *objp;
{
	if (!xdr_long(xdrs, &objp->main_st)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->dev_diag)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->tuned)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_pin_head(objp)
	pin_head *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong (&objp->main_st);
        length = length + xdr_length_DevLong (&objp->dev_diag);
        length = length + xdr_length_DevLong (&objp->tuned);
        length = length + xdr_length_DevLong (&objp->mode);

        return (length);
}




bool_t
xdr_axis_state(xdrs, objp)
	XDR *xdrs;
	axis_state *objp;
{
	if (!xdr_long(xdrs, &objp->unit)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->moving)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->mot_pos)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->enc_pos)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->switch_st)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->tuned)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_axis_state(objp)
	axis_state *objp;
{
        long  length = 0;

        length = length + xdr_length_DevLong (&objp->unit);
        length = length + xdr_length_DevLong (&objp->moving);
        length = length + xdr_length_DevFloat (&objp->mot_pos);
        length = length + xdr_length_DevFloat (&objp->enc_pos);
        length = length + xdr_length_DevLong (&objp->switch_st);
        length = length + xdr_length_DevLong (&objp->tuned);

        return (length);
}




bool_t
xdr_DevPinState(xdrs, objp)
	XDR *xdrs;
	DevPinState *objp;
{
	if (!xdr_pin_head(xdrs, &objp->pin_head)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->z1_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->z2_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->tx_state)) {
		return (FALSE);
	}
	if (!xdr_axis_state(xdrs, &objp->tt_state)) {
		return (FALSE);
	}
	return (TRUE);
}

long 
xdr_length_DevPinState(objp)
	DevPinState *objp;
{
        long  length = 0;

        length = length + xdr_length_pin_head (&objp->pin_head);
        length = length + xdr_length_axis_state (&objp->z1_state);
        length = length + xdr_length_axis_state (&objp->z2_state);
        length = length + xdr_length_axis_state (&objp->tx_state);
        length = length + xdr_length_axis_state (&objp->tt_state);

        return (length);
}


