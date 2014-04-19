/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:        vrif_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for the XDR definitions 
 *		of the VRIF vme card data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:24 $
 *
 *******************************************************************-*/

#ifndef VRIF_XDR_H
#define VRIF_XDR_H

/*
 * definitions for VRIF vme card
 */

struct DevVrifWdog {
	DevChar   Value;
	DevString PassWord;
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
	DevLong  Address;
	DevShort Stop;
	DevShort Sysfail;
	DevShort Dtack;
	DevShort Berr;
	DevShort DtatStrobe;
	DevShort Iack;
	DevShort Lword;
	DevShort Am;
	DevShort Write;
	DevShort As;
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
	DevFloat Vcc;
	DevShort VccP2;
	DevShort Flag_p12v;
	DevShort Flag_m12v;
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

