/*
 
 Author:	$Author: jkrueger1 $

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-03-18 16:16:13 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France

 *  */


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
extern xres_clnt *dc_devget_clnt_1();
extern xres *dc_devget_1();

#define DC_DEVGETV ((u_long)5)
extern xresv_clnt *dc_devgetv_clnt_1();
extern xresv *dc_devgetv_1();

#define DC_DEVGETM ((u_long)6)
extern xresm_clnt *dc_devgetm_clnt_1();
extern mpar_back *dc_devgetm_1();

#define DC_DEVDEF ((u_long)7)
extern outpar *dc_devdef_1();

#define DC_DEVGET_H ((u_long)10)
extern xres_hist_clnt *dc_devgeth_clnt_1();
extern xresh_mast *dc_devgeth_1();


/*
 * Call number for write part of DC server
 */


#define DC_OPEN ((u_long)1)
extern dc_xdr_error *dc_open_1();

#define DC_CLOSE ((u_long)2)
extern dc_xdr_error *dc_close_1();

#define DC_DATAPUT ((u_long)3)
extern dc_xdr_error *dc_dataput_1();

#define DC_INFO ((u_long)4)
extern dc_infox_back *dc_info_1();

#define DC_DEVALL ((u_long)5)
extern dc_devallx_back *dc_devall_1();

#define DC_DEVINFO ((u_long)6)
extern dc_devinfx_back *dc_devinfo_1();


#ifdef ALONE
#define DC_PROG ((u_long)22000002)
#else
bool_t xdr_register_data();
#endif

