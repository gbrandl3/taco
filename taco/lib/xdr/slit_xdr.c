
/*+*******************************************************************

 File:          slit_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for SLIT types.

 Author(s):     Timo Mett\"al\"a

 Original:      September 1992

Copyright (c) 1992 by European Synchrotron Radiation Facility,
                      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <slit_xdr.h>


bool_t xdr_pslit_head(xdrs, objp)
        XDR *xdrs;
        pslit_head *objp;
{
        if (!xdr_long(xdrs, &objp->pmain_st))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->dev_diag))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->unit))
                return (FALSE);

        return (TRUE);
}


bool_t xdr_blade_state(xdrs, objp)
        XDR *xdrs;
        blade_state *objp;
{
        if (!xdr_long(xdrs, &objp->values_st))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->mot_moving))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->mot_pos))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->enc_pos))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->brake))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->switch_st))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->tuned))
                return (FALSE);
        return (TRUE);
}


bool_t xdr_DevBladeState(xdrs, objp)
        XDR *xdrs;
        DevBladeState *objp;
{
        if (!xdr_pslit_head(xdrs, &objp->pslit_head))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->up_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->do_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->fr_state))
                return (FALSE);
        if (!xdr_blade_state(xdrs, &objp->ba_state))
                return (FALSE); 
        return (TRUE);
}

bool_t xdr_pslit_state(xdrs, objp)
        XDR *xdrs;
        pslit_state *objp;
{
        if (!xdr_long(xdrs, &objp->values_st))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->gap))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->offset))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp_1))
                return (FALSE);
        if (!xdr_float(xdrs, &objp->temp_2))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->brake_1))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->brake_2))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->switch_1))
                return (FALSE);
        if (!xdr_long(xdrs, &objp->switch_2))
                return (FALSE);
        return (TRUE);
}


bool_t xdr_DevPslitState(xdrs, objp)
        XDR *xdrs;
        DevPslitState *objp;
{
        if (!xdr_pslit_head(xdrs, &objp->pslit_head))
                return (FALSE);
        if (!xdr_pslit_state(xdrs, &objp->vertical))
                return (FALSE);
        if (!xdr_pslit_state(xdrs, &objp->horizontal))
                return (FALSE);
        return (TRUE);
}

