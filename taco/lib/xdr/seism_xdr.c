/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:        seism_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for
 *		the Seismic Survey System data types.
 *
 * Author(s):  	Jean Michel Chaize
 * 		$Author: jkrueger1 $
 *
 * Original:	August 1992
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <dev_xdr.h>
#include <seism_xdr.h>


bool_t xdr_DevSeismEvent(xdrs, objp)
XDR *xdrs;
DevSeismEvent *objp;
{
	if (!xdr_DevLong ( xdrs, &objp->nsta )) return (FALSE);
 if (!xdr_vector(xdrs, (char *)objp, 16, sizeof(short), (xdrproc_t)xdr_short)) return (FALSE);
	if (!xdr_short ( xdrs, &objp->coincid_sum )) return (FALSE);
	if (!xdr_short ( xdrs, &objp->pre_eventtime )) return (FALSE);
	if (!xdr_short ( xdrs, &objp->post_eventtime )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->t_beg )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->t_ana )) return (FALSE);
	if (!xdr_DevLong ( xdrs, &objp->win_type )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->win_width )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_fmin )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->rms_fmax )) return (FALSE);
	if (!xdr_DevLong ( xdrs, &objp->smooth_flag )) return (FALSE);
	if (!xdr_float ( xdrs, &objp->smo_width )) return (FALSE);
	if (!xdr_DevLong ( xdrs, &objp->nspecout )) return (FALSE);
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
	if (!xdr_DevLong ( xdrs, &objp->tr_stalta )) return (FALSE);
	if (!xdr_DevLong ( xdrs, &objp->trigger )) return (FALSE);
	if (!xdr_DevLong ( xdrs, &objp->v_test )) return (FALSE);
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
