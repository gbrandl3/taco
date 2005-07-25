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
 * File:        ram_xdr.h
 * 
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Radiation Monitor data types.
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

#ifndef RAM_XDR_H
#define RAM_XDR_H

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

