/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef RF_XDR_H
#define RF_XDR_H


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

