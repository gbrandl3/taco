/*+*******************************************************************

 File:          vrif_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for the XDR definitions 
		of the VRIF vme card data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _vrif_xdr_h
#define _vrif_xdr_h


/*
 * definitions for VRIF vme card
 */

struct DevVrifWdog {
	char  Value;
	char *PassWord;
};
typedef struct DevVrifWdog 	DevVrifWdog;
bool_t 	_DLLFunc xdr_DevVrifWdog PT_((XDR *xdrs, DevVrifWdog *objp));
#define D_VRIF_WDOG             29
#define LOAD_VRIF_WDOG(A)       xdr_load_type ( D_VRIF_WDOG, \
                                                xdr_DevVrifWdog, \
                                                sizeof(DevVrifWdog), \
                                                NULL, \
                                                A )


struct DevVrifVmeStat {
	long  Address;
	short Stop;
	short Sysfail;
	short Dtack;
	short Berr;
	short DtatStrobe;
	short Iack;
	short Lword;
	short Am;
	short Write;
	short As;
};
typedef struct DevVrifVmeStat 	DevVrifVmeStat;
bool_t 	_DLLFunc xdr_DevVrifVmeStat PT_((XDR *xdrs, DevVrifVmeStat *objp));
#define D_VRIF_STATUS           30
#define LOAD_VRIF_STATUS(A)     xdr_load_type ( D_VRIF_STATUS, \
                                                xdr_DevVrifVmeStat, \
                                                sizeof(DevVrifVmeStat), \
                                                NULL, \
                                                A )


struct DevVrifPowerStat {
	float Vcc;
	short VccP2;
	short Flag_p12v;
	short Flag_m12v;
};
typedef struct DevVrifPowerStat DevVrifPowerStat;
bool_t 	_DLLFunc xdr_DevVrifPowerStat
		 PT_((XDR *xdrs, DevVrifPowerStat *objp));
#define D_VRIF_POWERSTATUS      31
#define LOAD_VRIF_POWERSTATUS(A) xdr_load_type ( D_VRIF_POWERSTATUS, \
                                                xdr_DevVrifPowerStat, \
                                                sizeof(DevVrifPowerStat), \
                                                NULL, \
                                                A )

#endif /* _vrif_xdr_h */

