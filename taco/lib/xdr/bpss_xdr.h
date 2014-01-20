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
 * File:        bpss_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Booster Powersupply System
 *		data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

#ifndef BPSS_XDR_H
#define BPSS_XDR_H

/*
 *  definition for booster power supplies system
 */
struct DevStateIndic {
		DevShort State;
		DevShort Indic;
		     };
typedef struct DevStateIndic 	DevStateIndic;
bool_t 	_DLLFunc xdr_DevStateIndic PT_((XDR *xdrs, DevStateIndic *objp));
long   	_DLLFunc xdr_length_DevStateIndic PT_((DevStateIndic *objp));
#define D_STATE_INDIC           34
#define LOAD_STATE_INDIC(A)     xdr_load_type ( D_STATE_INDIC, \
                                                xdr_DevStateIndic, \
                                                sizeof(DevStateIndic), \
                                                xdr_length_DevStateIndic, \
                                                A )

struct DevBpssState {
		DevShort Ac;
		DevShort Dc;
		    };
typedef struct DevBpssState 	DevBpssState;
bool_t 	_DLLFunc xdr_DevBpssState PT_((XDR *xdrs, DevBpssState *objp));
long   	_DLLFunc xdr_length_DevBpssState PT_((DevBpssState *objp));
#define D_BPSS_STATE            35
#define LOAD_BPSS_STATE(A)      xdr_load_type ( D_BPSS_STATE, \
                                                xdr_DevBpssState, \
                                                sizeof(DevBpssState), \
                                                xdr_length_DevBpssState, \
                                                A )



struct DevBpssLine {
		DevFloat Set;
		DevFloat Read;
		DevShort Control;
		DevShort Alarm;
		     };
typedef struct DevBpssLine 	DevBpssLine;
bool_t 	_DLLFunc xdr_DevBpssLine PT_((XDR *xdrs, DevBpssLine *objp));
long   	_DLLFunc xdr_length_DevBpssLine PT_((DevBpssLine *objp));
#define D_BPSS_LINE             41
#define LOAD_BPSS_LINE(A)      xdr_load_type ( D_BPSS_LINE, \
                                                xdr_DevBpssLine, \
                                                sizeof(DevBpssLine), \
                                                xdr_length_DevBpssLine, \
                                                A )

struct DevBpssReadPoint {
		 DevBpssLine  IACinv;
		 DevBpssLine  IACmag;
		 DevBpssLine  IDCmag;
		 DevBpssLine  Period;
		 DevBpssLine  PhLag;
                 DevFloat     PhLagUI;
                 DevFloat     VACinv;
                 DevFloat     VDCmag;
                 DevFloat     CapTemp;
                 DevFloat     ChokeTemp;
		 DevFloat     Temperature[8];
		 DevBpssState State;
		 DevBpssState ExtState;
		 DevBpssState Faults[32];
		 DevShort     Master;
			};
typedef struct DevBpssReadPoint DevBpssReadPoint;
bool_t 	_DLLFunc xdr_DevBpssReadPoint
		 PT_((XDR *xdrs, DevBpssReadPoint *objp));
long   	_DLLFunc xdr_length_DevBpssReadPoint PT_((DevBpssReadPoint *objp));
#define D_BPSS_READPOINT        36
#define LOAD_BPSS_READPOINT(A)  xdr_load_type ( D_BPSS_READPOINT, \
                                                xdr_DevBpssReadPoint, \
                                                sizeof(DevBpssReadPoint), \
                                                xdr_length_DevBpssReadPoint, \
                                                A )

#endif /* _bpss_xdr_h */

