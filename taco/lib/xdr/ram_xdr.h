
/*+*******************************************************************

 File:          ram_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Radiation Monitor data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _ram_xdr_h
#define _ram_xdr_h


/*
 * definitions for NE Technologies Gamma- and Neutron Monitor types
 */

#define UNIT_MSG_SIZE	10

struct DevNeg14StatusRec {
	int unit_type;
	float unitrange;
	long samplermode;
	int last_update;
	long monitorerror;
	char msgalarm;
	long unitstatus;
	long unitfaultcode;
	char checksum[UNIT_MSG_SIZE];
	char aritherrorcode[UNIT_MSG_SIZE];
	long alarmstatus;
	float doserate;
	double lowalarmth;
	double highalarmth;
	double alertalarmth;
	double primingdoserate;
	char AlarmResponseOpts[UNIT_MSG_SIZE];
};
typedef struct DevNeg14StatusRec DevNeg14StatusRec;
bool_t 	_DLLFunc xdr_DevNeg14StatusRec
		 PT_((XDR *xdrs, DevNeg14StatusRec *objp));
long   	_DLLFunc xdr_length_DevNeg14StatusRec
		 PT_((DevNeg14StatusRec *objp));
#define D_NEG_STATUS            20
#define LOAD_NEG_STATUS(A)      xdr_load_type ( D_NEG_STATUS, \
                                                xdr_DevNeg14StatusRec, \
                                                sizeof(DevNeg14StatusRec), \
                                                xdr_length_DevNeg14StatusRec, \
                                                A )

struct DevRadiationDoseValue {
	short state;
	short type;
	float read;
};
typedef struct DevRadiationDoseValue DevRadiationDoseValue;
bool_t 	_DLLFunc xdr_DevRadiationDoseValue
		 PT_((XDR *xdrs, DevRadiationDoseValue *objp));
long   	_DLLFunc xdr_length_DevRadiationDoseValue
		 PT_((DevRadiationDoseValue *objp));
#define D_RAD_DOSE_VALUE        21
#define LOAD_RAD_DOSE_VALUE(A)  xdr_load_type ( D_RAD_DOSE_VALUE, \
                                            xdr_DevRadiationDoseValue, \
                                            sizeof(DevRadiationDoseValue), \
                                            xdr_length_DevRadiationDoseValue, \
                                            A )

#endif /* _ram_xdr_h */

