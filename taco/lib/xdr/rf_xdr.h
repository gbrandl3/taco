/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        rf_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for the XDR definitions
 *		of the Radio Frquency System data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:23 $
 *
 *******************************************************************-*/

#ifndef RF_XDR_H
#define RF_XDR_H


/*
 *  definition for radio frequency system
 */

struct DevRfSigConfig {
        DevLong 	number;
	DevChar 	name[12];
	DevChar 	family[4];
	DevLong 	related_setpoint;
	DevChar 	units[8];
	DevFloat 	warn_high;
	DevFloat 	warn_low;
	DevFloat 	error_high;
	DevFloat 	error_low;
	DevFloat 	scale_high;
	DevFloat 	scale_low;
	DevChar 	format[16];
	DevChar 	description[80];
	DevDouble 	setptr;
	DevChar 	false_text[10];
	DevChar 	true_text[10];
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

