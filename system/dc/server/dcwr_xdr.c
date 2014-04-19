/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File         : dcwr_xdr.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author(s)    : E. Taurel
 *                $Author: jkrueger1 $
 *
 * Original     : February 1993
 *
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2008-04-06 09:07:50 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <dcwr_xdr.h>

#include <DserverTeams.h>
static const unsigned int maxi=(unsigned int)~0;

bool_t
xdr_dc_cmd_x(xdrs, objp)
	XDR *xdrs;
	dc_cmd_x *objp;
{
	if (!xdr_int(xdrs, &objp->cmd)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cmd_argout)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dc_dev_x(xdrs, objp)
	XDR *xdrs;
	dc_dev_x *objp;
{
	if (!xdr_string(xdrs, &objp->dev_name, 24)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->poll_int)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->dc_cmd_ax.dc_cmd_ax_val, (u_int *)&objp->dc_cmd_ax.dc_cmd_ax_len, maxi, sizeof(dc_cmd_x), (xdrproc_t)xdr_dc_cmd_x)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dc_open_in(xdrs, objp)
	XDR *xdrs;
	dc_open_in *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->dc_open_in_val, (u_int *)&objp->dc_open_in_len, maxi, sizeof(dc_dev_x), (xdrproc_t)xdr_dc_dev_x)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t
xdr_dc_xdr_error(xdrs, objp)
	XDR *xdrs;
	dc_xdr_error *objp;
{
	if (!xdr_int(xdrs, &objp->error_code)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->dev_error)) {
		return (FALSE);
	}
	return (TRUE);
}



bool_t
xdr_name(xdrs, objp)
	XDR *xdrs;
	name *objp;
{
	if (!xdr_string(xdrs, objp, 256)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_name_arr(xdrs, objp)
	XDR *xdrs;
	name_arr *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->name_arr_val, (u_int *)&objp->name_arr_len, maxi, sizeof(name), (xdrproc_t)xdr_name)) {
		return (FALSE);
	}
	return (TRUE);
}



bool_t
xdr_cmd_dat(xdrs, objp)
	XDR *xdrs;
	cmd_dat *objp;
{
	if (!xdr_int(xdrs, &objp->xcmd)) {
		return (FALSE);
	}
	if (!xdr_DevLong(xdrs, &objp->xcmd_error)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->xcmd_time)) {
		return (FALSE);
	}
	if (objp->xcmd_error == 0) {
	if (!xdr_bytes(xdrs, (char **)&objp->xsequence.xsequence_val, (u_int *)&objp->xsequence.xsequence_len, maxi)) {
		return (FALSE);
	}
				   }
	else if (((objp->xcmd_error >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError) {
	if (!xdr_bytes(xdrs, (char **)&objp->xsequence.xsequence_val, (u_int *)&objp->xsequence.xsequence_len, maxi)) {
		return (FALSE);
	}
	}
	return (TRUE);
}




bool_t
xdr_dev_dat(xdrs, objp)
	XDR *xdrs;
	dev_dat *objp;
{
	if (!xdr_string(xdrs, &objp->xdev_name, 24)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->xcmd_dat.xcmd_dat_val, (u_int *)&objp->xcmd_dat.xcmd_dat_len, maxi, sizeof(cmd_dat), (xdrproc_t)xdr_cmd_dat)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dev_datarr(xdrs, objp)
	XDR *xdrs;
	dev_datarr *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->dev_datarr_val, (u_int *)&objp->dev_datarr_len, maxi, sizeof(dev_dat), (xdrproc_t)xdr_dev_dat)) {
		return (FALSE);
	}
	return (TRUE);
}



bool_t
xdr_dom_x(xdrs, objp)
	XDR *xdrs;
	dom_x *objp;
{
	if (!xdr_string(xdrs, &objp->dom_name, 20)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs,&objp->dom_nb_dev)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dc_infox(xdrs, objp)
	XDR *xdrs;
	dc_infox *objp;
{
	if (!xdr_u_int(xdrs, &objp->free_mem)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->mem)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->nb_dev)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs,(caddr_t *)&objp->dom_ax.dom_ax_val,
			(u_int *)&objp->dom_ax.dom_ax_len,maxi,
			sizeof(dom_x),(xdrproc_t)xdr_dom_x)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dc_infox_back(xdrs, objp)
	XDR *xdrs;
	dc_infox_back *objp;
{
	if (!xdr_int(xdrs, &objp->err_code)) {
		return (FALSE);
	}
	if (!xdr_dc_infox(xdrs, &objp->back)) {
		return (FALSE);
	}
	return (TRUE);
}



bool_t
xdr_dc_devallx_back(xdrs, objp)
	XDR *xdrs;
	dc_devallx_back *objp;
{
	if (!xdr_int(xdrs, &objp->err_code)) {
		return (FALSE);
	}
	if (!xdr_name_arr(xdrs, &objp->dev_name)) {
		return (FALSE);
	}
	return (TRUE);
}



bool_t
xdr_cmd_infox(xdrs, objp)
	XDR *xdrs;
	cmd_infox *objp;
{
	if (!xdr_int(xdrs, &objp->cmdx)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cmd_argoutx)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cmd_timex)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dev_infx(xdrs, objp)
	XDR *xdrs;
	dev_infx *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->cmd_dev.cmd_dev_val, (u_int *)&objp->cmd_dev.cmd_dev_len, maxi, sizeof(cmd_infox), (xdrproc_t)xdr_cmd_infox)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->pollx)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->diff_time)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->ptr_offset)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->data_offset)) {
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->data_base)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->deltax, 5, sizeof(int), (xdrproc_t)xdr_int)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_dc_devinfx_back(xdrs, objp)
	XDR *xdrs;
	dc_devinfx_back *objp;
{
	if (!xdr_int(xdrs, &objp->err_code)) {
		return (FALSE);
	}
	if (!xdr_dev_infx(xdrs, &objp->device)) {
		return (FALSE);
	}
	return (TRUE);
}


