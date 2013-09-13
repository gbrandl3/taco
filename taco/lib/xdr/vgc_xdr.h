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
 * File:        vgc_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions of the
 *		Vacuum Gauge Controller data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:23 $
 *
 *******************************************************************-*/

#ifndef VGC_XDR_H
#define VGC_XDR_H

/*
 * definitions for vacuum gauge controller types
 */

struct DevVgcStat {
	DevChar status[8];
	DevChar error[8];
};
typedef struct DevVgcStat 	DevVgcStat;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcStat PT_((XDR *xdrs, DevVgcStat *objp));
long   	_DLLFunc xdr_length_DevVgcStat PT_((DevVgcStat *objp));
#ifdef __cplusplus
}
#endif
#define D_VGC_STATUS            16
#define LOAD_VGC_STATUS(A)      xdr_load_type ( D_VGC_STATUS, \
                                                (DevDataFunction)xdr_DevVgcStat, \
                                                sizeof(DevVgcStat), \
                                                (DevDataLengthFunction)xdr_length_DevVgcStat, \
                                                A )

struct DevVgcGauge {
	DevChar   status[8];
	DevChar   error[8];
	DevChar   relay_status[16];
	DevChar   type;
	DevChar   number;
	DevChar   gauge_status[8];
	DevChar   gauge_error[8];
	DevDouble pressure;
};
typedef struct DevVgcGauge 	DevVgcGauge;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcGauge PT_((XDR *xdrs, DevVgcGauge *objp));
long   	_DLLFunc xdr_length_DevVgcGauge PT_((DevVgcGauge *objp));
#ifdef __cplusplus
}
#endif
#define D_VGC_GAUGE             17
#define LOAD_VGC_GAUGE(A)       xdr_load_type ( D_VGC_GAUGE, \
                                                (DevDataFunction)xdr_DevVgcGauge, \
                                                sizeof(DevVgcGauge), \
                                                (DevDataLengthFunction)xdr_length_DevVgcGauge, \
                                                A )


struct DevVgcPiraniGauge {
	DevChar   type;
	DevChar   number;
	DevChar   calibration;
	DevDouble gas_factor;
	DevChar   status[8];
	DevChar   error[8];
	DevDouble pressure;
};
typedef struct DevVgcPiraniGauge DevVgcPiraniGauge;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcPiraniGauge
		 PT_((XDR *xdrs, DevVgcPiraniGauge *objp));
long   	_DLLFunc xdr_length_DevVgcPiraniGauge
		 PT_((DevVgcPiraniGauge *objp));
#ifdef __cplusplus
}
#endif


struct DevVgcPenningGauge {
	DevChar   type;
	DevChar   number;
	DevChar   filter;
	DevLong   start_time;
	DevChar   calibration;
	DevDouble min_current;
	DevChar   status[8];
	DevChar   error[8];
	DevDouble pressure;
};
typedef struct DevVgcPenningGauge DevVgcPenningGauge;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcPenningGauge
		 PT_((XDR *xdrs, DevVgcPenningGauge *objp));
long   	_DLLFunc xdr_length_DevVgcPenningGauge
		 PT_((DevVgcPenningGauge *objp));
#ifdef __cplusplus
}
#endif


struct DevVgcRelay {
	DevChar   letter;
	DevChar   status;
	DevDouble setpoint;
	DevChar   ass_gauge;
};
typedef struct DevVgcRelay 	DevVgcRelay;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcRelay PT_((XDR *xdrs, DevVgcRelay *objp));
long   	_DLLFunc xdr_length_DevVgcRelay PT_((DevVgcRelay *objp));
#ifdef __cplusplus
}
#endif


struct DevVgcSystem {
	DevChar   interlock;
	DevChar   relay_conf;
	DevChar   cold_cathode;
	DevChar   ROM_version[4];
};
typedef struct DevVgcSystem 	DevVgcSystem;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcSystem PT_((XDR *xdrs, DevVgcSystem *objp));
long   	_DLLFunc xdr_length_DevVgcSystem PT_((DevVgcSystem *objp));
#ifdef __cplusplus
}
#endif


struct DevVgcController {
	DevVgcPiraniGauge  pirani[2];
	DevVgcPenningGauge penning[4];
	DevVgcRelay        relay[12];
	DevLong            nb_pir;
	DevLong            nb_pen;
	DevLong            nb_relay;
	DevVgcSystem       syst;
	DevChar            status[8];
	DevChar            error[8];
	DevChar            relay_status[16];
};
typedef struct DevVgcController DevVgcController;
#ifdef __cplusplus
extern "C" {
#endif
bool_t 	_DLLFunc xdr_DevVgcController
		 PT_((XDR *xdrs, DevVgcController *objp));
long   	_DLLFunc xdr_length_DevVgcController
		PT_((DevVgcController *objp));
#ifdef __cplusplus
}
#endif
#define D_VGC_CONTROLLER        18
#define LOAD_VGC_CONTROLLER(A)  xdr_load_type ( D_VGC_CONTROLLER, \
                                                (DevDataFunction)xdr_DevVgcController, \
                                                sizeof(DevVgcController), \
                                                (DevDataLengthFunction)xdr_length_DevVgcController, \
                                                A )
#endif /* _vgc_xdr_h */
