/*+*******************************************************************

 File:          ws_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Wire Scanner data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _ws_xdr_h
#define _ws_xdr_h


/*
 * definitions for Wire Scanner types
 */

struct DevWsBeamFitParm { 
	DevVarFloatArray pos_array;
	DevVarFloatArray meas_array;
	DevVarFloatArray perc_array;
	short mode;
};
typedef struct DevWsBeamFitParm DevWsBeamFitParm;
bool_t 	_DLLFunc xdr_DevWsBeamFitParm 
	         PT_((XDR *xdrs, DevWsBeamFitParm *objp));
#define D_WS_BEAMFITPARM        15
#define LOAD_WS_BEAMFITPARM(A)  xdr_load_type ( D_WS_BEAMFITPARM, \
						xdr_DevWsBeamFitParm, \
						sizeof(DevWsBeamFitParm), \
						NULL, \
						A )

#endif /* _ws_xdr_h */

