/*+*******************************************************************

 File:          thc_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Termocouple Controller data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _thc_xdr_h
#define _thc_xdr_h


/*
 * definitions for Thermocouples
 */
struct DevThermocouple {
	float temperature;
	short th_broken;
	short th_alarm;
};
typedef struct DevThermocouple 	DevThermocouple;
bool_t 	_DLLFunc xdr_DevThermocouple PT_((XDR *xdrs, DevThermocouple *objp));
long   	_DLLFunc xdr_length_DevThermocouple PT_((DevThermocouple *objp));


struct DevVarTHArray {
	u_int 		  length;
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
	float	temp[64];
	char	alarm[8];
	char	cut[8];
	char	output[8];
	char	input[8];
	char	err[8];
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

