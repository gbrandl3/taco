/*+*******************************************************************

 File:          pss_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Personal Safety System data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.3 $

 Date:		$Date: 2004-02-19 15:49:34 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _pss_xdr_h
#define _pss_xdr_h


/*
 * definitions for PSS
 */
struct PSSInterlockStatus {
	int		Module;
	int		GuardA;
	int		GuardB;
	int		GuardC;
	unsigned short 	Interlocks;
};
typedef struct PSSInterlockStatus PSSInterlockStatus;
bool_t 	_DLLFunc xdr_PSSInterlockStatus
		 PT_((XDR *xdrs, PSSInterlockStatus *objp));
long   	_DLLFunc xdr_length_PSSInterlockStatus
		 PT_((PSSInterlockStatus *objp));


struct DevDaresburyStatus {
	u_int length;
	PSSInterlockStatus *sequence;
};
typedef struct DevDaresburyStatus DevDaresburyStatus;
bool_t 	_DLLFunc xdr_DevDaresburyStatus
		 PT_((XDR *xdrs, DevDaresburyStatus *objp));
long   	_DLLFunc xdr_length_DevDaresburyStatus
		 PT_((DevDaresburyStatus *objp));
#define D_PSS_STATUS            39
#define LOAD_PSS_STATUS(A)      xdr_load_type ( D_PSS_STATUS, \
                                                xdr_DevDaresburyStatus, \
                                                sizeof(DevDaresburyStatus), \
                                                xdr_length_DevDaresburyStatus, \
                                                A )
#endif /* _pss_xdr_h */

