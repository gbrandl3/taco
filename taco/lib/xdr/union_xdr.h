
/*+*******************************************************************

 File:          union_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for ancient union type.

 Author(s):  	Jens Meyer

 Original:	July 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _union_xdr_h
#define _union_xdr_h

#include <dev_xdr.h>
#include <bpm_xdr.h>
#include <ws_xdr.h>

/*
 *  definition for an union type 
 *  this type exits only for compatibility reasons with
 *  the old device server system !
 */

struct DevUnion {
	short designator;
	union {
	      char B_data;
	      short S_data;
	      long L_data;
	      float F_data;
	      double D_data;
	      char *STR_data;
	      DevFloatReadPoint FRP_data;
	      DevStateFloatReadPoint SFRP_data;
	      DevVarCharArray VCA_data;
	      DevVarShortArray VSA_data;
	      DevVarLongArray VLA_data;
	      DevVarFloatArray VFA_data;
	      DevBpmPosField BPMPF_data;
	      DevBpmElecField BPMEF_data;
	      DevWsBeamFitParm WSBFP_data;
	      }u;
};
typedef struct DevUnion 	DevUnion;
typedef struct DevUnion 	DevArg;
bool_t 	_DLLFunc xdr_DevUnion PT_((XDR *xdrs, DevUnion *objp));
#define D_UNION_TYPE            19
#define LOAD_UNION_TYPE(A)      xdr_load_type ( D_UNION_TYPE, \
						xdr_DevUnion, \
						sizeof(DevUnion), \
						NULL, \
						A )

#endif /* _union_xdr_h */

