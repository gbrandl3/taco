/*+*******************************************************************

 File:          seism_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for
		the Seismic Survey System data types.

 Author(s):  	Jean Michel Chaize

 Original:	August 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <seism_xdr.h>


bool_t xdr_DevSeismEvent(xdrs, objp)
XDR *xdrs;
DevSeismEvent *objp;
{
	if (!xdr_int ( xdrs, &objp->nsta )) return (FALSE);
 if (!xdr_vector(xdrs, (char *)objp, 16, sizeof(short), (xdrproc_t)xdr_short)) return (FALSE);
	if (!xdr_short ( xdrs, &objp->coincid_sum )) return (FALSE);
	if (!xdr_short ( xdrs, &objp->pre_eventtime )) return (FALSE);
	if (!xdr_short ( xdrs, &objp->post_eventtime )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->t_beg )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->t_ana )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->win_type )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->win_width )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_fmin )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_fmax )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->smooth_flag )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->smo_width )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->nspecout )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->out_freqinc )) return (FALSE);
	return (TRUE);
}

bool_t xdr_DevSeismStat(xdrs, objp)
XDR *xdrs;
DevSeismStat *objp;
{
        if (!xdr_char ( xdrs, &objp->comp )) return (FALSE);
	if (!xdr_DevString ( xdrs, &objp->sismo_type )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->n_lat )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->e_lon )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->elev )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->freq_prop )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->damping )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->gain )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->sampling )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->transduct )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->conv )) return (FALSE);
	if (!xdr_DevString ( xdrs, &objp->pos )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->high_pass )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->tr_mode )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->tr_hpass )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->tr_lpass )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->tr_sta )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->tr_lta )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->tr_stalta )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->trigger )) return (FALSE);
	if (!xdr_int ( xdrs, &objp->v_test )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->vmin_a )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->vmax_a )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->vmin_w )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->vmax_w )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->dmin )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->dmax )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_time )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_spec )) return (FALSE);
	return (TRUE);
}
