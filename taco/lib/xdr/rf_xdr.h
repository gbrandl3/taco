
/*+*******************************************************************

 File:          rf_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for the XDR definitions
		of the Radio Frquency System data types.

 Author(s):  	Jens Meyer

 Original:	July 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _rf_xdr_h
#define _rf_xdr_h


/*
 *  definition for radio frequency system
 */

struct DevRfSigConfig {
        int 	number;
	char 	name[12];
	char 	family[4];
	int 	related_setpoint;
	char 	units[8];
	float 	warn_high;
	float 	warn_low;
	float 	error_high;
	float 	error_low;
	float 	scale_high;
	float 	scale_low;
	char 	format[16];
	char 	description[80];
	double 	setptr;
	char 	false_text[10];
	char 	true_text[10];
};
typedef struct DevRfSigConfig 	DevRfSigConfig ;
bool_t 	_DLLFunc xdr_DevRfSigConfig PT_((XDR *xdrs, DevRfSigConfig *objp));
#define D_RF_SIGCONFIG          40
#define LOAD_RF_SIGCONFIG(A)    xdr_load_type ( D_RF_SIGCONFIG, \
						xdr_DevRfSigConfig, \
						sizeof(DevRfSigConfig), \
						NULL, \
						A )

#endif /* _rf_xdr_h */

