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
 * File:        thc_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Termocouple Controller data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef THC_XDR_H
#define THC_XDR_H

/*
 * definitions for Thermocouples
 */
struct DevThermocouple {
	DevFloat temperature;
	DevShort th_broken;
	DevShort th_alarm;
};
typedef struct DevThermocouple 	DevThermocouple;
bool_t 	_DLLFunc xdr_DevThermocouple PT_((XDR *xdrs, DevThermocouple *objp));
long   	_DLLFunc xdr_length_DevThermocouple PT_((DevThermocouple *objp));


struct DevVarTHArray {
	DevULong 	  length;
	DevThermocouple	  *sequence;
};
typedef struct DevVarTHArray 	DevVarTHArray;
bool_t 	_DLLFunc xdr_DevVarTHArray PT_((XDR *xdrs, DevVarTHArray *objp));
long   	_DLLFunc xdr_length_DevVarTHArray PT_((DevVarTHArray *objp));
#define D_VAR_THARR             26
#define LOAD_VAR_THARR(A)       xdr_load_type ( D_VAR_THARR, \
                                                xdr_DevVarTHArray, \
                                                sizeof(DevVarTHArray), \
                                                xdr_length_DevVarTHArray, \
                                                A )



struct DevLienState {
	DevFloat	temp[64];
	DevChar		alarm[8];
	DevChar		cut[8];
	DevChar		output[8];
	DevChar		input[8];
	DevChar		err[8];
};
typedef struct DevLienState 	DevLienState;
bool_t 	_DLLFunc xdr_DevLienState PT_((XDR *xdrs, DevLienState *objp));
long   	_DLLFunc xdr_length_DevLienState PT_((DevLienState *objp));
#define D_LIEN_STATE            50
#define LOAD_LIEN_STATE(A)      xdr_load_type ( D_LIEN_STATE, \
                                                xdr_DevLienState, \
                                                sizeof(DevLienState), \
                                                xdr_length_DevLienState, \
                                                A )

#endif /* _thc_xdr_h */

