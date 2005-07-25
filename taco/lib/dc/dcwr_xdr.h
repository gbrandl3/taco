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
 * File:	dcwr_xdr.h
 *
 * Description: XDR structures definition for teh write part of dc server
 *
 * Author(s):   Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original:      1992
 *
 * Version:       $Revision: 1.2 $
 *
 * Date:          $Date: 2005-07-25 12:55:41 $
 *
*******************************************************************************/

#ifndef DCWR_XDR_H
#define DCWR_XDR_H

struct cmd_dat {
	int xcmd;
	long xcmd_error;
	int xcmd_time;
	struct {
		u_int xsequence_len;
		char *xsequence_val;
	} xsequence;
};
typedef struct cmd_dat cmd_dat;
bool_t xdr_cmd_dat();


struct dev_dat {
	char *xdev_name;
	struct {
		u_int xcmd_dat_len;
		cmd_dat *xcmd_dat_val;
	} xcmd_dat;
};
typedef struct dev_dat dev_dat;
bool_t xdr_dev_dat();


typedef struct {
	u_int dev_datarr_len;
	dev_dat *dev_datarr_val;
} dev_datarr;
bool_t xdr_dev_datarr();


typedef char *name;
bool_t xdr_name();


typedef struct {
	u_int name_arr_len;
	name *name_arr_val;
} name_arr;
bool_t xdr_name_arr();


struct dc_cmd_x {
	int cmd;
	int cmd_argout;
};
typedef struct dc_cmd_x dc_cmd_x;
bool_t xdr_dc_cmd_x();


struct dc_dev_x {
	char *dev_name;
	int poll_int;
	struct {
		u_int dc_cmd_ax_len;
		struct dc_cmd_x *dc_cmd_ax_val;
	} dc_cmd_ax;
};
typedef struct dc_dev_x dc_dev_x;
bool_t xdr_dc_dev_x();


typedef struct {
	u_int dc_open_in_len;
	dc_dev_x *dc_open_in_val;
} dc_open_in;
bool_t xdr_dc_open_in();


struct dc_xdr_error {
	int error_code;
	int dev_error;
};
typedef struct dc_xdr_error dc_xdr_error;
bool_t xdr_dc_xdr_error();

struct dom_x {
	char *dom_name;
	int dom_nb_dev;
};
typedef struct dom_x dom_x;
bool_t xdr_dom_x();

struct dc_infox {
	unsigned int free_mem;
	unsigned int mem;
	int nb_dev;
	struct {
		u_int dom_ax_len;
		struct dom_x *dom_ax_val;
	} dom_ax;
};
typedef struct dc_infox dc_infox;
bool_t xdr_dc_infox();


struct dc_infox_back {
	int err_code;
	dc_infox back;
};
typedef struct dc_infox_back dc_infox_back;
bool_t xdr_dc_infox_back();


struct dc_devallx_back {
	int err_code;
	name_arr dev_name;
};
typedef struct dc_devallx_back dc_devallx_back;
bool_t xdr_dc_devallx_back();


struct cmd_infox {
	int cmdx;
	int cmd_argoutx;
	int cmd_timex;
};
typedef struct cmd_infox cmd_infox;
bool_t xdr_cmd_infox();


struct dev_infx {
	struct {
		u_int cmd_dev_len;
		cmd_infox *cmd_dev_val;
	} cmd_dev;
	int pollx;
	u_int diff_time;
	u_int ptr_offset;
	u_int data_offset;
	u_int data_base;
	int deltax[5];
};
typedef struct dev_infx dev_infx;
bool_t xdr_dev_infx();


struct dc_devinfx_back {
	int err_code;
	dev_infx device;
};
typedef struct dc_devinfx_back dc_devinfx_back;
bool_t xdr_dc_devinfx_back();

#endif
