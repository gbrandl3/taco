
/*+*******************************************************************

 File:          mstat_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for the XDR definitions
		for the ESRF machine status data type.

 Author(s):  	Jens Meyer

 Original:	March 1993


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _mstat_xdr_h
#define _mstat_xdr_h


/*
 *  definition for the ESRF machine status array
 */

struct SysState { 
	char 	*sys_name;
	long 	sys_ident;		
	long 	sys_state;		
};
typedef struct SysState 	SysState;
bool_t 	_DLLFunc xdr_SysState PT_((XDR *xdrs, SysState *objp));
long   	_DLLFunc xdr_length_SysState PT_((SysState *objp));


struct VarSysStateArray {
	u_int 		length;
	SysState 	*sequence;
};
typedef struct VarSysStateArray VarSysStateArray;
bool_t 	_DLLFunc xdr_VarSysStateArray
		 PT_((XDR *xdrs, VarSysStateArray *objp));
long   	_DLLFunc xdr_length_VarSysStateArray PT_((VarSysStateArray *objp));
#define D_VAR_MSTATARR          58
#define LOAD_VAR_MSTATARR(A)    xdr_load_type ( D_VAR_MSTATARR, \
						xdr_VarSysStateArray, \
						sizeof(VarSysStateArray), \
						xdr_length_VarSysStateArray, \
						A )
#endif /* _mstat_xdr_h */

