
/*+*******************************************************************

 File:          bpm_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions 
		of the Beam Position Monitor data
		types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _bpm_xdr_h
#define _bpm_xdr_h

/*
 * definitions for Beam Position Monitor types
 */

typedef float DevBpmElec [4];
bool_t _DLLFunc xdr_DevBpmElec PT_((XDR *xdrs, DevBpmElec objp));
long   _DLLFunc xdr_length_DevBpmElec PT_((DevBpmElec objp));



struct DevBpmMeasHead {
	unsigned short 	mn;
	unsigned short 	gms;
        struct {
		u_int 	    length;
		DevBpmElec *sequence;
	} momint;
};
typedef struct DevBpmMeasHead DevBpmMeasHead;
bool_t _DLLFunc xdr_DevBpmMeasHead PT_((XDR *xdrs, DevBpmMeasHead *objp));
long   _DLLFunc xdr_length_DevBpmMeasHead PT_((DevBpmMeasHead *objp));

struct DevBpmPos { 
	unsigned short 	mms;
	float 		xcoord;
	float 		zcoord;
};
typedef struct DevBpmPos DevBpmPos;
bool_t _DLLFunc xdr_DevBpmPos PT_((XDR *xdrs, DevBpmPos *objp));
long   _DLLFunc xdr_length_DevBpmPos PT_((DevBpmPos *objp));

struct DevBpmPosMeasure { 
	DevBpmMeasHead 	meashead;
	struct {
		u_int 	    length;
		DevBpmPos  *sequence;
	} posdata;
};
typedef struct DevBpmPosMeasure DevBpmPosMeasure;
bool_t _DLLFunc xdr_DevBpmPosMeasure PT_((XDR *xdrs,DevBpmPosMeasure *objp));
long   _DLLFunc xdr_length_DevBpmPosMeasure PT_((DevBpmPosMeasure *objp));



struct DevBpmElecMeasure { 
	DevBpmMeasHead 	meashead;
        struct {
		u_int 	    length;
		DevBpmElec *sequence;
	} elecdata;
};
typedef struct DevBpmElecMeasure DevBpmElecMeasure;
bool_t _DLLFunc xdr_DevBpmElecMeasure
		PT_((XDR *xdrs, DevBpmElecMeasure *objp));
long   _DLLFunc xdr_length_DevBpmElecMeasure PT_((DevBpmElecMeasure *objp));

struct DevBpmMeasParm { 
	long expbeamint;
	long timewind;
	long delaytim[6];
};
typedef struct DevBpmMeasParm DevBpmMeasParm;
bool_t _DLLFunc xdr_DevBpmMeasParm PT_((XDR *xdrs, DevBpmMeasParm *objp));
long   _DLLFunc xdr_length_DevBpmMeasParm PT_((DevBpmMeasParm *objp));

struct DevBpmInjectHead	{ 
	unsigned short min;
	unsigned long  gis;
	DevBpmMeasParm bpm_measpar;
};
typedef struct DevBpmInjectHead DevBpmInjectHead;
bool_t _DLLFunc xdr_DevBpmInjectHead
		PT_((XDR *xdrs, DevBpmInjectHead *objp));
long   _DLLFunc xdr_length_DevBpmInjectHead PT_((DevBpmInjectHead *objp));



struct DevBpmPosField { 
	DevBpmInjectHead injhead;
	struct 	{
		u_int 		 length;
	  	DevBpmPosMeasure *sequence;
		} posmeas;
};
typedef struct DevBpmPosField 	DevBpmPosField;
bool_t 	_DLLFunc xdr_DevBpmPosField PT_((XDR *xdrs, DevBpmPosField *objp));
long   	_DLLFunc xdr_length_DevBpmPosField PT_((DevBpmPosField *objp));
#define D_BPM_POSFIELD          13
#define LOAD_BPM_POSFIELD(A)    xdr_load_type ( D_BPM_POSFIELD, \
                                                xdr_DevBpmPosField, \
                                                sizeof(DevBpmPosField), \
                                                xdr_length_DevBpmPosField, \
                                                A )


struct DevBpmElecField { 
	DevBpmInjectHead injhead;
	struct 	{
		u_int 		  length;
		DevBpmElecMeasure *sequence;
		} elecmeas;
};
typedef struct DevBpmElecField 	DevBpmElecField;
bool_t 	_DLLFunc xdr_DevBpmElecField PT_((XDR *xdrs, DevBpmElecField *objp));
long   	_DLLFunc xdr_length_DevBpmElecField PT_((DevBpmElecField *objp));
#define D_BPM_ELECFIELD         14
#define LOAD_BPM_ELECFIELD(A)    xdr_load_type ( D_BPM_ELECFIELD, \
                                                xdr_DevBpmElecField, \
                                                sizeof(DevBpmElecField), \
                                                xdr_length_DevBpmElecField, \
                                                A )


#endif /* _bpm_xdr_h */

