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
 * File:	dc_xdr.h
 *
 * Description: structures definition for XDR transfer 
 *
 * Author(s):	E. Taurel
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2008-04-06 09:07:14 $
 *
 *******************************************************************************/

#ifndef DC_XDR_H
#define DC_XDR_H
/*
 *   Include structures definition for XDR transfer for read commands
 */
#include <dcrd_xdr.h>

/*
 *   Include structures definition for XDR transfer for write commands
 */
#include <dcwr_xdr.h>

/*
 * Define dc server version 
 */

#define DC_VERS ((u_long)1)

/*
 * Call number for read part of DC server
 */


#define DC_DEVGET ((u_long)4)
#define DC_DEVGETV ((u_long)5)
#define DC_DEVGETM ((u_long)6)
#define DC_DEVDEF ((u_long)7)
#define DC_DEVGET_H ((u_long)10)

#ifndef __cplusplus
extern xres_clnt *dc_devget_clnt_1();
extern xres *dc_devget_1();

extern xresv_clnt *dc_devgetv_clnt_1();
extern xresv *dc_devgetv_1();

extern xresm_clnt *dc_devgetm_clnt_1();
extern mpar_back *dc_devgetm_1();

extern outpar *dc_devdef_1();

extern xres_hist_clnt *dc_devgeth_clnt_1();
extern xresh_mast *dc_devgeth_1();
#else /*__cplusplus*/
extern "C" {
extern xres_clnt *dc_devget_clnt_1(xdevget *argp, CLIENT *clnt, DevArgument argout, DevType argout_type, DevLong *perr);

extern xres *dc_devget_1();

extern xresv_clnt *dc_devgetv_clnt_1(xdevgetv *argp, CLIENT *clnt, xres_clnt *st_ptr, DevLong *perr);
extern xresv *dc_devgetv_1(...);

extern xresm_clnt *dc_devgetm_clnt_1(mpar *argp, CLIENT *clnt, mint *st_ptr, DevLong *perr);
extern mpar_back *dc_devgetm_1(...);

extern outpar *dc_devdef_1(imppar *argp, CLIENT *clnt, DevLong *perr);

extern xres_hist_clnt *dc_devgeth_clnt_1(xdevgeth *argp, CLIENT *clnt, xresh_clnt *st_ptr, DevLong *perr);
extern xresh_mast *dc_devgeth_1(...);
}
#endif /* _cplusplus */


/*
 * Call number for write part of DC server
 */
#define DC_OPEN ((u_long)1)
#define DC_CLOSE ((u_long)2)
#define DC_DATAPUT ((u_long)3)
#define DC_INFO ((u_long)4)
#define DC_DEVALL ((u_long)5)
#define DC_DEVINFO ((u_long)6)

/* 
 * amazing but true - two versions of these function exist with the same
 * name but different prototypes (depending on wether they are on the client
 * or server side)
 */
#ifdef DC_SERVER
extern dc_xdr_error *dc_open_1(dc_open_in * rece);
extern dc_xdr_error *dc_close_1(register name_arr *dev_name);
extern dc_xdr_error *dc_dataput_1(dev_datarr *rece);
extern dc_infox_back *dc_info_1(int *fill);
extern dc_devallx_back *dc_devall_1(int *fill);
extern dc_devinfx_back *dc_devinfo_1(char **rece);
#else
extern dc_xdr_error *dc_open_1(dc_open_in *argp,CLIENT *clnt, DevLong *perr);
extern dc_xdr_error *dc_close_1(name_arr *argp,CLIENT *clnt, DevLong *perr);
extern dc_xdr_error *dc_dataput_1(dev_datarr *argp,CLIENT *clnt, DevLong *perr);
extern dc_infox_back *dc_info_1(void *argp,CLIENT *clnt, DevLong *perr);
extern dc_devallx_back *dc_devall_1(void *argp,CLIENT *clnt, DevLong *perr);
extern dc_devinfx_back *dc_devinfo_1(name *argp,CLIENT *clnt, DevLong *perr);
#endif /* DC_CLIENT */


#ifdef ALONE
#define DC_PROG ((u_long)22000002)
#else
bool_t xdr_register_data();
#endif

#endif
