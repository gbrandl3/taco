/*+*******************************************************************

 File:          bpss_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Booster Powersupply System
		data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _bpss_xdr_h
#define _bpss_xdr_h


/*
 *  definition for booster power supplies system
 */
struct DevStateIndic {
		short State;
		short Indic;
		     };
typedef struct DevStateIndic 	DevStateIndic;
bool_t 	_DLLFunc xdr_DevStateIndic PT_((XDR *xdrs, DevStateIndic *objp));
long   	_DLLFunc xdr_length_DevStateIndic PT_((DevStateIndic *objp));
#define D_STATE_INDIC           34
#define LOAD_STATE_INDIC(A)     xdr_load_type ( D_STATE_INDIC, \
                                                xdr_DevStateIndic, \
                                                sizeof(DevStateIndic), \
                                                xdr_length_DevStateIndic, \
                                                A )

struct DevBpssState {
		short Ac;
		short Dc;
		    };
typedef struct DevBpssState 	DevBpssState;
bool_t 	_DLLFunc xdr_DevBpssState PT_((XDR *xdrs, DevBpssState *objp));
long   	_DLLFunc xdr_length_DevBpssState PT_((DevBpssState *objp));
#define D_BPSS_STATE            35
#define LOAD_BPSS_STATE(A)      xdr_load_type ( D_BPSS_STATE, \
                                                xdr_DevBpssState, \
                                                sizeof(DevBpssState), \
                                                xdr_length_DevBpssState, \
                                                A )



struct DevBpssLine {
		float Set;
		float Read;
		short Control;
		short Alarm;
		     };
typedef struct DevBpssLine 	DevBpssLine;
bool_t 	_DLLFunc xdr_DevBpssLine PT_((XDR *xdrs, DevBpssLine *objp));
long   	_DLLFunc xdr_length_DevBpssLine PT_((DevBpssLine *objp));
#define D_BPSS_LINE             41
#define LOAD_BPSS_LINE(A)      xdr_load_type ( D_BPSS_LINE, \
                                                xdr_DevBpssLine, \
                                                sizeof(DevBpssLine), \
                                                xdr_length_DevBpssLine, \
                                                A )

struct DevBpssReadPoint {
		 DevBpssLine IACinv;
		 DevBpssLine IACmag;
		 DevBpssLine IDCmag;
		 DevBpssLine Period;
		 DevBpssLine PhLag;
                 float      PhLagUI;
                 float      VACinv;
                 float      VDCmag;
                 float      CapTemp;
                 float      ChokeTemp;
		 float	    Temperature[8];
		 DevBpssState State;
		 DevBpssState ExtState;
		 DevBpssState Faults[32];
		 short	    Master;
			};
typedef struct DevBpssReadPoint DevBpssReadPoint;
bool_t 	_DLLFunc xdr_DevBpssReadPoint
		 PT_((XDR *xdrs, DevBpssReadPoint *objp));
long   	_DLLFunc xdr_length_DevBpssReadPoint PT_((DevBpssReadPoint *objp));
#define D_BPSS_READPOINT        36
#define LOAD_BPSS_READPOINT(A)  xdr_load_type ( D_BPSS_READPOINT, \
                                                xdr_DevBpssReadPoint, \
                                                sizeof(DevBpssReadPoint), \
                                                xdr_length_DevBpssReadPoint, \
                                                A )

#endif /* _bpss_xdr_h */

