/*+*******************************************************************

 File:          _xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:	 	$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _grp_xdr_h
#define _grp_xdr_h


struct Frame {
	double		value;
	short		output;
	short		pause;
};
typedef struct Frame 		Frame;
bool_t 	_DLLFunc xdr_Frame  PT_((XDR *xdrs, Frame *objp));
long   	_DLLFunc xdr_length_Frame  PT_((Frame *objp));


struct FramePair {
	Frame		dead;
	Frame		live;
};
typedef struct FramePair 	FramePair;
bool_t 	_DLLFunc xdr_FramePair  PT_((XDR *xdrs, FramePair *objp));
long    _DLLFunc xdr_length_FramePair  PT_((FramePair *objp));


struct DevGrpFramePair {
	u_int		nb_framepair;
	FramePair	framepair;
};
typedef struct DevGrpFramePair 	DevGrpFramePair;
bool_t 	_DLLFunc xdr_DevGrpFramePair PT_((XDR *xdrs, DevGrpFramePair *objp));
long   	_DLLFunc xdr_length_DevGrpFramePair  PT_((DevGrpFramePair *objp));
#define D_GRPFP_TYPE            65
#define LOAD_GRPFP_TYPE(A)      xdr_load_type ( D_GRPFP_TYPE, \
                                                xdr_DevGrpFramePair, \
                                                sizeof(DevGrpFramePair), \
                                                xdr_length_DevGrpFramePair, \
                                                A )


struct DevGrpFramePairArray {
	u_int		length;
	DevGrpFramePair	*sequence;
};
typedef struct DevGrpFramePairArray DevGrpFramePairArray;
bool_t 	_DLLFunc xdr_DevGrpFramePairArray
		 PT_((XDR *xdrs, DevGrpFramePairArray *objp));
long   	_DLLFunc xdr_length_DevGrpFramePairArray
		 PT_((DevGrpFramePairArray *objp));
#define D_VAR_GRPFPARR          66
#define LOAD_VAR_GRPFPARR(A)    xdr_load_type ( D_VAR_GRPFPARR, \
                                            xdr_DevGrpFramePairArray, \
                                            sizeof(DevGrpFramePairArray), \
                                            xdr_length_DevGrpFramePairArray, \
                                            A )
#endif /* _grp_xdr_h */

