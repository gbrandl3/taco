/*+*******************************************************************

 File:          ct_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for the XDR definitions
		of the Current Transformer data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _ct_xdr_h
#define _ct_xdr_h


/*
 * definitions for current transformer types
 */

struct DevCtIntLifeTime {
		float DeltaIntensity;  /* delta-intensity for this measure */
		float LifeTime;        /* value of the life-time */
		long  DateTicks;       /* date in ticks since midnight */
		long  DeltaTused;      /* delta-T used for calculations */
};
typedef struct DevCtIntLifeTime DevCtIntLifeTime;
bool_t 	_DLLFunc xdr_DevCtIntLifeTime
		 PT_((XDR *xdrs, DevCtIntLifeTime *objp));
long   	_DLLFunc xdr_length_DevCtIntLifeTime  PT_((DevCtIntLifeTime *objp));


struct DevVarCtIntLifeTimeArray {
		u_int             length;
		DevCtIntLifeTime  *sequence;
};
typedef struct DevVarCtIntLifeTimeArray DevVarCtIntLifeTimeArray;
bool_t 	_DLLFunc xdr_DevVarCtIntLifeTimeArray
		 PT_((XDR *xdrs, DevVarCtIntLifeTimeArray *objp));
long   	_DLLFunc xdr_length_DevVarCtIntLifeTimeArray
		 PT_((DevVarCtIntLifeTimeArray *objp));
#define D_CT_LIFETIME           42
#define LOAD_CT_LIFETIME(A)     xdr_load_type ( D_CT_LIFETIME, \
                                         xdr_DevVarCtIntLifeTimeArray, \
                                         sizeof(DevVarCtIntLifeTimeArray), \
                                         xdr_length_DevVarCtIntLifeTimeArray, \
                                         A )

#endif /* _ct_xdr_h */

