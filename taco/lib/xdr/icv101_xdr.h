/*+*******************************************************************
 File:          icv101_xdr.h
 Project:       Device Servers with sun-rpc
 Description:   Include file for XDR definitions
		of the icv101 data types.
 Author(s):  	Timo Mettala
 Original:	March 1993

 $Revision $
 $Date: 2003-04-25 11:21:47 $
 $Author: jkrueger1 $

 Copyright (c) 1993 by European Synchrotron Radiation Facility,
		       Grenoble, France
*******************************************************************-*/

#ifndef _icv101_xdr_h
#define _icv101_xdr_h

/*
 *  definitions for icv101 type D_VAR_PATTERNARR
 */

struct DevPattern {
	short  channel;
	short  gain;
	short  end;
};
typedef struct DevPattern 	DevPattern;
bool_t 	_DLLFunc xdr_DevPattern PT_((XDR *xdrs, DevPattern *objp));


struct DevVarPatternArray {
	u_int 		length;
	DevPattern  	*sequence;
};
typedef struct DevVarPatternArray DevVarPatternArray;
bool_t 	_DLLFunc xdr_DevVarPatternArray
		 PT_((XDR *xdrs, DevVarPatternArray *objp));
#define D_VAR_PATTERNARR        56
#define LOAD_VAR_PATTERNARR(A)  xdr_load_type (D_VAR_PATTERNARR, \
                                               xdr_DevVarPatternArray, \
                                               sizeof(DevVarPatternArray), \
                                               NULL, \
                                               A )


/*
 *  definitions for icv101 type D_ICV_MODE
 */

struct DevIcv101Mode {
       long external_stop;
       long external_start;
       long continuous;
       long main_frequency;
       long sub_frequency;
       long npfs2;
       long npost;
};
typedef struct DevIcv101Mode 	DevIcv101Mode;
bool_t 	_DLLFunc xdr_DevIcv101Mode PT_((XDR *xdrs, DevIcv101Mode *objp));
#define D_ICV_MODE              57
#define LOAD_ICV_MODE(A)  	xdr_load_type (D_ICV_MODE, \
                                               xdr_DevIcv101Mode, \
                                               sizeof(DevIcv101Mode), \
                                               NULL, \
                                               A )
	  
#endif /* _icv101_xdr_h */
