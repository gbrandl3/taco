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
 * File:        seism_xdr.h
 * 
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Seismic Survey System data types.
 *
 * Author(s):  	Jean Michel Chaize
 * 		$Author: jkrueger1 $
 *
 * Original:	August 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:23 $
 *
 *******************************************************************-*/

#ifndef SEISM_XDR_H
#define SEISM_XDR_H

struct DevSeismEvent {
	DevLong   nsta;
        DevShort weight[16];
	DevShort coincid_sum;
	DevShort pre_eventtime;
	DevShort post_eventtime;
        DevChar  tape_id;
        DevChar  file_num[3];
	DevFloat t_beg;
	DevFloat t_ana;
	DevLong   win_type;
	DevFloat win_width;
        DevLong   inst_cor_flag;
	DevFloat rms_fmin;
	DevFloat rms_fmax;
	DevLong   smooth_flag;
	DevFloat smo_width;
	DevFloat freq_inc;
	DevLong   nspecout;
	DevFloat out_freqinc;
		     };
typedef struct DevSeismEvent 	DevSeismEvent;
bool_t 	_DLLFunc xdr_DevSeismEvent PT_((XDR *xdrs, DevSeismEvent *objp));
#define D_SEISM_EVENT           48
#define LOAD_SEISM_EVENT(A)     xdr_load_type ( D_SEISM_EVENT, \
                                                xdr_DevSeismEvent, \
                                                sizeof(DevSeismEvent), \
                                                NULL, \
                                                A )

struct DevSeismStat {
                DevChar comp;
		DevString sismo_type;
		DevFloat n_lat;
		DevFloat e_lon;
		DevFloat elev;
		DevFloat freq_prop;
		DevFloat damping;
		DevFloat gain;
		DevFloat sampling;
		DevFloat transduct;
		DevFloat conv;
		DevString pos;
		DevFloat high_pass;
		DevFloat tr_mode;
		DevFloat tr_hpass;
		DevFloat tr_lpass;
		DevFloat tr_sta;
		DevFloat tr_lta;
		DevLong tr_stalta;
		DevLong trigger;
		DevLong v_test;
		DevFloat vmin_a;
		DevFloat vmax_a;
		DevFloat vmin_w;
		DevFloat vmax_w;
		DevFloat dmin;
		DevFloat dmax;
		DevFloat rms_time;
		DevFloat rms_spec;
		    };
typedef struct DevSeismStat 	DevSeismStat;
bool_t 	_DLLFunc xdr_DevSeismStat PT_((XDR *xdrs, DevSeismStat *objp));
#define D_SEISM_STAT            49
#define LOAD_SEISM_STAT(A)      xdr_load_type ( D_SEISM_STAT, \
                                                xdr_DevSeismStat, \
                                                sizeof(DevSeismStat), \
                                                NULL, \
                                                A )

#endif /* _seism_xdr_h */
