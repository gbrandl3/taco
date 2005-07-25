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
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef VGC_XDR_H
#define VGC_XDR_H

/*
 * definitions for vacuum gauge controller types
 */

struct DevVgcStat {
	char status[8];
	char error[8];
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
	char status[8];
	char error[8];
	char relay_status[16];
	char type;
	char number;
	char gauge_status[8];
	char gauge_error[8];
	double pressure;
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
	char type;
	char number;
	char calibration;
	double gas_factor;
	char status[8];
	char error[8];
	double pressure;
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
	char type;
	char number;
	char filter;
	int start_time;
	char calibration;
	double min_current;
	char status[8];
	char error[8];
	double pressure;
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
	char letter;
	char status;
	double setpoint;
	char ass_gauge;
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
	char interlock;
	char relay_conf;
	char cold_cathode;
	char ROM_version[4];
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
	DevVgcPiraniGauge pirani[2];
	DevVgcPenningGauge penning[4];
	DevVgcRelay relay[12];
	int nb_pir;
	int nb_pen;
	int nb_relay;
	DevVgcSystem syst;
	char status[8];
	char error[8];
	char relay_status[16];
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
