
/*+*******************************************************************

 File:          ws_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Wire Scanner data types.

 Author(s):  	Jens Meyer

 Original:	July 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

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

