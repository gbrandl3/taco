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
 * File:	dcrd_xdr.h
 *
 *
 * Description: XDR structures definition for the read part of DC server
 *
 * Author(s):   Emmanuel Taurel
 *		$Author: andy_gotz $
 *
 * Original:      1992
 *
 * Version:       $Revision: 1.3 $
 *
 * Date:          $Date: 2006-01-22 21:19:19 $
 *
 *****************************************************************************/

#ifndef DCRD_XDR_H
#define DCRD_XDR_H

struct xdevget {
	char *xdev_name;
	int xcmd;
	int xargout_type;
};
typedef struct xdevget xdevget;
bool_t xdr_xdevget(XDR *xdrs, xdevget *objp);


struct xdevgetv {
	u_int xdevgetv_len;
	xdevget *xdevgetv_val;
};
typedef struct xdevgetv xdevgetv;
bool_t xdr_xdevgetv(XDR *xdrs, xdevgetv *objp);


struct xdevgeth {
	char *xdev_name;
	int xcmd;
	int xargout_type;
	int xnb_rec;
};
typedef struct xdevgeth xdevgeth;
bool_t xdr_xdevgeth(XDR *xdrs, xdevgeth *objp);


struct xdc {
	u_int xdc_len;
	char *xdc_val;
};
typedef struct xdc xdc;


struct xres {
	int xerr;
	xdc *xbufp;
};
typedef struct xres xres;
bool_t xdr_xres(XDR *xdrs, xres *objp);
bool_t xdr_dcopaque(XDR *xdrs, xdc *objp);


struct xresv {
	int xgen_err;
	struct {
		u_int xresv_len;
		xres *xresv_val;
	} xresa;
};
typedef struct xresv xresv;
bool_t xdr_xresv(XDR *xdrs, xresv *objp);


struct xresh {
	int xerr;
	unsigned int xtime;
	xdc *xbufp;
};
typedef struct xresh xresh;
bool_t xdr_xresh(XDR *xdrs, xresh *objp);


struct xresh_mast {
	int xgen_err;
	struct {
		u_int xresh_len;
		xresh *xresh_val;
	} xresb;
};
typedef struct xresh_mast xresh_mast;
bool_t xdr_xresh_mast(XDR *xdrs, xresh_mast *objp);


struct xres_clnt {
	int xerr;
	DevType xargout_type;
	DevArgument xargout;
};
typedef struct xres_clnt xres_clnt;
bool_t xdr_xres_clnt(XDR *xdrs, xres_clnt *objp);


struct xresv_clnt {
	int xgen_err;
	struct {
		u_int xresv_clnt_len;
		xres_clnt *xresv_clnt_val;
	} xresa_clnt;
};
typedef struct xresv_clnt xresv_clnt;
bool_t xdr_xresv_clnt(XDR *xdrs, xresv_clnt *objp);


struct xresh_clnt {
	int xerr;
	unsigned int xtime;
	DevType xargout_type;
	DevArgument xargout;
};
typedef struct xresh_clnt xresh_clnt;
bool_t xdr_xresh_clnt(XDR *xdrs, xresh_clnt *objp);


struct xres_hist_clnt {
	int xgen_err;
	struct {
		u_int xresh_clnt_len;
		xresh_clnt *xresh_clnt_val;
	} xresb_clnt;
};
typedef struct xres_hist_clnt xres_hist_clnt;
bool_t xdr_xres_hist_clnt(XDR *xdrs, xres_hist_clnt *objp);


struct xcmdpar {
	int xcmd;
	int xargout_type;
};
typedef struct xcmdpar xcmdpar;
bool_t xdr_xcmdpar(XDR *xdrs, xcmdpar *objp);


struct mxdev {
	char *xdev_name;
	struct {
		u_int mcmd_len;
		xcmdpar *mcmd_val;
	} mcmd;
};
typedef struct mxdev mxdev;
bool_t xdr_mxdev(XDR *xdrs, mxdev *objp);


typedef struct {
	u_int mpar_len;
	mxdev *mpar_val;
} mpar;
bool_t xdr_mpar(XDR *xdrs, mpar *objp);


struct mxres {
	u_int mxres_len;
	xres *mxres_val;
};
typedef struct mxres mxres;
bool_t xdr_mxres(XDR *xdrs, mxres *objp);


struct mpar_back {
	int xgen_err;
	struct {
		u_int xxres_len;
		mxres *xxres_val;
	} xxres;
};
typedef struct mpar_back mpar_back;
bool_t xdr_mpar_back(XDR *xdrs, mpar_back *objp);


struct mint {
	u_int mint_len;
	xres_clnt *mint_val;
};
typedef struct mint mint;
bool_t xdr_mint(XDR *xdrs, mint *objp);


struct xresm_clnt {
	int xgen_err;
	struct {
		u_int x_clnt_len;
		mint *x_clnt_val;
	} x_clnt;
};
typedef struct xresm_clnt xresm_clnt;
bool_t xdr_xresm_clnt(XDR *xdrs, xresm_clnt *objp);

/* Definition for the DC_DEVDEF call */

typedef char *xdevdef_name;
bool_t xdr_xdevdef_name(XDR *xdrs, xdevdef_name *objp);


typedef struct {
	u_int imppar_len;
	xdevdef_name *imppar_val;
} imppar;
bool_t xdr_imppar(XDR *xdrs, imppar *objp);


struct xdev_err {
	int devnumb;
	int deverr;
};
typedef struct xdev_err xdev_err;
bool_t xdr_xdev_err(XDR *xdrs, xdev_err *objp);


struct outpar {
	int xgen_err;
	struct {
		u_int taberr_len;
		xdev_err *taberr_val;
	} taberr;
};
typedef struct outpar outpar;
bool_t xdr_outpar(XDR *xdrs, outpar *objp);
#endif
