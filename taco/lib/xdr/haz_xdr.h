/*+*******************************************************************

 File:          haz_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Hazemeyer Poersupply data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.3 $

 Date:		$Date: 2004-02-19 15:49:18 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _haz_xdr_h
#define _haz_xdr_h

/*
 * definitions for hazemeyer powersupply
 */
struct DevHazStatus {
        char  Power;
	char  Remote;
	char  Aux;
	char  Stat;
	char  Phase;
	char  Mains;
	char  PassivFilt;
	char  ActivFilt;
	char  Doors;
	char  Synchro;
	char  IMains;
	char  Loop;
	char  Waterflow;
	char  OverTemp;
	char  DCCTsat;
	char  OverVolt;
	char  OverCurrent;
	char  EarthFault;
	char  User1;
	char  User2;
	char  User3;
	char  Transformer;
	char  TOilTemp;
};
typedef struct DevHazStatus 	DevHazStatus;
bool_t 	_DLLFunc xdr_DevHazStatus PT_((XDR *xdrs, DevHazStatus *objp));
long    _DLLFunc xdr_length_DevHazStatus PT_((DevHazStatus *objp));
#define D_HAZ_STATUS            28
#define LOAD_HAZ_STATUS(A)      xdr_load_type ( D_HAZ_STATUS, \
                                                xdr_DevHazStatus, \
                                                sizeof(DevHazStatus), \
                                                xdr_length_DevHazStatus, \
                                                A )

#endif /* _haz_xdr_h */

