/*

 Author(s):     Emmanuel Taurel
		$Author: jkrueger1 $

 Original:      1992

 Version:       $Revision: 1.1 $

 Date:          $Date: 2003-03-18 16:16:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

*/
/* XDR structures definition for the read part of DC server */

struct xdevget {
	char *xdev_name;
	int xcmd;
	int xargout_type;
};
typedef struct xdevget xdevget;
bool_t xdr_xdevget();


struct xdevgetv {
	u_int xdevgetv_len;
	xdevget *xdevgetv_val;
};
typedef struct xdevgetv xdevgetv;
bool_t xdr_xdevgetv();


struct xdevgeth {
	char *xdev_name;
	int xcmd;
	int xargout_type;
	int xnb_rec;
};
typedef struct xdevgeth xdevgeth;
bool_t xdr_xdevgeth();


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
bool_t xdr_xres();
bool_t xdr_dcopaque();


struct xresv {
	int xgen_err;
	struct {
		u_int xresv_len;
		xres *xresv_val;
	} xresa;
};
typedef struct xresv xresv;
bool_t xdr_xresv();


struct xresh {
	int xerr;
	unsigned int xtime;
	xdc *xbufp;
};
typedef struct xresh xresh;
bool_t xdr_xresh();


struct xresh_mast {
	int xgen_err;
	struct {
		u_int xresh_len;
		xresh *xresh_val;
	} xresb;
};
typedef struct xresh_mast xresh_mast;
bool_t xdr_xresh_mast();


struct xres_clnt {
	int xerr;
	DevType xargout_type;
	DevArgument xargout;
};
typedef struct xres_clnt xres_clnt;
bool_t xdr_xres_clnt();


struct xresv_clnt {
	int xgen_err;
	struct {
		u_int xresv_clnt_len;
		xres_clnt *xresv_clnt_val;
	} xresa_clnt;
};
typedef struct xresv_clnt xresv_clnt;
bool_t xdr_xresv_clnt();


struct xresh_clnt {
	int xerr;
	unsigned int xtime;
	DevType xargout_type;
	DevArgument xargout;
};
typedef struct xresh_clnt xresh_clnt;
bool_t xdr_xresh_clnt();


struct xres_hist_clnt {
	int xgen_err;
	struct {
		u_int xresh_clnt_len;
		xresh_clnt *xresh_clnt_val;
	} xresb_clnt;
};
typedef struct xres_hist_clnt xres_hist_clnt;
bool_t xdr_xres_hist_clnt();


struct xcmdpar {
	int xcmd;
	int xargout_type;
};
typedef struct xcmdpar xcmdpar;
bool_t xdr_xcmdpar();


struct mxdev {
	char *xdev_name;
	struct {
		u_int mcmd_len;
		xcmdpar *mcmd_val;
	} mcmd;
};
typedef struct mxdev mxdev;
bool_t xdr_mxdev();


typedef struct {
	u_int mpar_len;
	mxdev *mpar_val;
} mpar;
bool_t xdr_mpar();


struct mxres {
	u_int mxres_len;
	xres *mxres_val;
};
typedef struct mxres mxres;
bool_t xdr_mxres();


struct mpar_back {
	int xgen_err;
	struct {
		u_int xxres_len;
		mxres *xxres_val;
	} xxres;
};
typedef struct mpar_back mpar_back;
bool_t xdr_mpar_back();


struct mint {
	u_int mint_len;
	xres_clnt *mint_val;
};
typedef struct mint mint;
bool_t xdr_mint();


struct xresm_clnt {
	int xgen_err;
	struct {
		u_int x_clnt_len;
		mint *x_clnt_val;
	} x_clnt;
};
typedef struct xresm_clnt xresm_clnt;
bool_t xdr_xresm_clnt();

/* Definition for the DC_DEVDEF call */

typedef char *xdevdef_name;
bool_t xdr_xdevdef_name();


typedef struct {
	u_int imppar_len;
	xdevdef_name *imppar_val;
} imppar;
bool_t xdr_imppar();


struct xdev_err {
	int devnumb;
	int deverr;
};
typedef struct xdev_err xdev_err;
bool_t xdr_xdev_err();


struct outpar {
	int xgen_err;
	struct {
		u_int taberr_len;
		xdev_err *taberr_val;
	} taberr;
};
typedef struct outpar outpar;
bool_t xdr_outpar();

