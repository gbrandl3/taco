/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * File:	dcrd_xdr.c
 *
 * Description:
 *
 * Author(s):   Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original:    1992
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2006-09-18 22:13:30 $
 *
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>

#include "dc.h"
#include "dcrd_xdr.h"

extern DevDataListEntry DevData_List[];
static const unsigned int maxi=(unsigned int)~0;

bool_t xdr_xdevget(XDR *xdrs, xdevget *objp)
{
	if (!xdr_string(xdrs, &objp->xdev_name, 24)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xcmd)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xargout_type)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xdevgetv(XDR *xdrs, xdevgetv *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->xdevgetv_val, (u_int *)&objp->xdevgetv_len, maxi, sizeof(xdevget), (xdrproc_t)xdr_xdevget)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xdevgeth(XDR *xdrs, xdevgeth *objp)
{
	if (!xdr_string(xdrs, &objp->xdev_name, 24)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xcmd)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xargout_type)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xnb_rec)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xcmdpar(XDR *xdrs, xcmdpar *objp)
{
	if (!xdr_int(xdrs, &objp->xcmd)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->xargout_type)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_mxdev(XDR *xdrs, mxdev *objp)
{
	if (!xdr_string(xdrs, &objp->xdev_name, 24)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->mcmd.mcmd_val, (u_int *)&objp->mcmd.mcmd_len, maxi, sizeof(xcmdpar), (xdrproc_t)xdr_xcmdpar)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_mpar(XDR *xdrs, mpar *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->mpar_val, (u_int *)&objp->mpar_len, maxi, sizeof(mxdev), (xdrproc_t)xdr_mxdev)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xres(XDR *xdrs, xres *objp)
{
	if (!xdr_int(xdrs, &objp->xerr)) 
		return (FALSE);
	if (objp->xerr == 0) 
	{
		if (!xdr_pointer(xdrs, (char **)&objp->xbufp, (u_int)sizeof(xdc), (xdrproc_t)xdr_dcopaque)) 
			return (FALSE);
	
	}
	else if (((objp->xerr >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError) 
	{
		if (!xdr_pointer(xdrs, (char **)&objp->xbufp, (u_int)sizeof(xdc), (xdrproc_t)xdr_dcopaque)) 
			return (FALSE);
	}
	return (TRUE);
}

bool_t xdr_dcopaque(XDR *xdrs, xdc *objp)
{
	if (!xdr_opaque(xdrs, (caddr_t)objp->xdc_val, (u_int)objp->xdc_len)) 
		return(FALSE);
	return (TRUE);
}

bool_t xdr_xresv(XDR *xdrs, xresv *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->xresa.xresv_val, (u_int *)&objp->xresa.xresv_len, maxi, sizeof(xres), (xdrproc_t)xdr_xres)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xresh(XDR *xdrs, xresh *objp)
{
	if (!xdr_int(xdrs, &objp->xerr)) 
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->xtime)) 
		return (FALSE);
	if (objp->xerr == 0) 
	{
		if (!xdr_pointer(xdrs, (char **)&objp->xbufp, (u_int)sizeof(xdc), (xdrproc_t)xdr_dcopaque)) 
			return (FALSE);
	}
	else if (((objp->xerr >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError) 
	{
		if (!xdr_pointer(xdrs, (char **)&objp->xbufp, (u_int)sizeof(xdc), (xdrproc_t)xdr_dcopaque)) 
			return (FALSE);
	}
	return (TRUE);
}

bool_t xdr_xresh_mast(XDR *xdrs, xresh_mast *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->xresb.xresh_val, (u_int *)&objp->xresb.xresh_len, maxi, sizeof(xresh), (xdrproc_t)xdr_xresh)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_mxres(XDR *xdrs, mxres *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->mxres_val, (u_int *)&objp->mxres_len, maxi, sizeof(xres), (xdrproc_t)xdr_xres)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_mpar_back(XDR *xdrs, mpar_back *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->xxres.xxres_val, (u_int *)&objp->xxres.xxres_len, maxi, sizeof(mxres), (xdrproc_t)xdr_mxres)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xres_clnt(XDR *xdrs, xres_clnt *objp)
{
	long ftype;
	long error;
	DevDataListEntry type_info;

	ftype = objp->xargout_type;

	if (!xdr_int(xdrs, &objp->xerr))
		return (FALSE);

/* 
 * Directly access the API DevData_List array to get XDR type decode function
 * pointer if the type is one of the basic types. If not, get this pointer
 * via the xdr_get_type function 
 */
	if (objp->xerr == 0)
	{
		if (ftype < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (DevData_List[ftype].type != ftype)
				return(FALSE);
			if (!xdr_pointer(xdrs, (char **)&objp->xargout, DevData_List[ftype].size, (xdrproc_t)DevData_List[ftype].xdr))
				return (FALSE);
		}
		else
		{
			if (xdr_get_type(ftype,&type_info,&error) != DS_OK)
				return(FALSE);
			if (!xdr_pointer(xdrs,(char **)&objp->xargout, type_info.size,(xdrproc_t)type_info.xdr))
				return(FALSE);
		}
	}
	else if (((objp->xerr >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError)
	{
		if (ftype < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (!xdr_pointer(xdrs, (char **)&objp->xargout, DevData_List[ftype].size, (xdrproc_t)DevData_List[ftype].xdr))
				return (FALSE);
		}
		else
		{
			if (xdr_get_type(ftype,&type_info,&error) != DS_OK)
				return(FALSE);
			if (!xdr_pointer(xdrs,(char **)&objp->xargout, type_info.size,(xdrproc_t)type_info.xdr))
				return(FALSE);
		}
	}
	return (TRUE);
}

bool_t xdr_xresv_clnt(XDR *xdrs, xresv_clnt *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->xresa_clnt.xresv_clnt_val, 
		       (u_int *)&objp->xresa_clnt.xresv_clnt_len, maxi, sizeof(xres_clnt), (xdrproc_t)xdr_xres_clnt)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xresh_clnt(XDR *xdrs, xresh_clnt *objp)
{
	long ftype;
	long error;
	DevDataListEntry type_info;

	ftype = objp->xargout_type;

	if (!xdr_int(xdrs, &objp->xerr))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->xtime))
		return (FALSE);

/* 
 * Directly access the API DevData_List array to get XDR type decode function
 * pointer if the type is one of the basic types. If not, get this pointer
 * via the xdr_get_type function 
 */
	if (objp->xerr == 0)
	{
		if (ftype < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (DevData_List[ftype].type != ftype)
				return(FALSE);
			if (!xdr_pointer(xdrs, (char **)&objp->xargout, DevData_List[ftype].size, (xdrproc_t)DevData_List[ftype].xdr))
				return (FALSE);
		}
		else
		{
			if (xdr_get_type(ftype,&type_info,&error) != DS_OK)
				return(FALSE);
			if (!xdr_pointer(xdrs,(char **)&objp->xargout, type_info.size,(xdrproc_t)type_info.xdr))
				return(FALSE);
		}
	}
	else if (((objp->xerr >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError)
	{
		if (ftype < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (!xdr_pointer(xdrs, (char **)&objp->xargout, DevData_List[ftype].size, (xdrproc_t)DevData_List[ftype].xdr))
				return (FALSE);
		}
		else
		{
			if (xdr_get_type(ftype,&type_info,&error) != DS_OK)
				return(FALSE);
			if (!xdr_pointer(xdrs,(char **)&objp->xargout, type_info.size,(xdrproc_t)type_info.xdr))
				return(FALSE);
		}
	}
	return (TRUE);
}

bool_t xdr_xres_hist_clnt(XDR *xdrs, xres_hist_clnt *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->xresb_clnt.xresh_clnt_val, 
		       (u_int *)&objp->xresb_clnt.xresh_clnt_len, maxi, sizeof(xresh_clnt), (xdrproc_t)xdr_xresh_clnt)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_mint(XDR *xdrs, mint *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->mint_val, (u_int *)&objp->mint_len, maxi, sizeof(xres_clnt), (xdrproc_t)xdr_xres_clnt)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xresm_clnt(XDR *xdrs, xresm_clnt *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->x_clnt.x_clnt_val, 
		       (u_int *)&objp->x_clnt.x_clnt_len, maxi, sizeof(mint), (xdrproc_t)xdr_mint)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xdevdef_name(XDR *xdrs, xdevdef_name *objp)
{
	if (!xdr_string(xdrs, objp, 24)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_imppar(XDR *xdrs, imppar *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->imppar_val, (u_int *)&objp->imppar_len, maxi, sizeof(xdevdef_name), (xdrproc_t)xdr_xdevdef_name)) 
		return (FALSE);
	return (TRUE);
}

bool_t xdr_xdev_err(XDR *xdrs, xdev_err *objp)
{
	if (!xdr_int(xdrs, &objp->devnumb)) 
		return (FALSE);
	if (!xdr_int(xdrs, &objp->deverr))
		return (FALSE);
	return (TRUE);
}

bool_t xdr_outpar(XDR *xdrs, outpar *objp)
{
	if (!xdr_int(xdrs, &objp->xgen_err)) 
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->taberr.taberr_val, (u_int *)&objp->taberr.taberr_len, maxi, sizeof(xdev_err), (xdrproc_t)xdr_xdev_err)) 
		return (FALSE);
	return (TRUE);
}

