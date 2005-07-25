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
 * File:        haz_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Hazemeyer Powersupply data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef HAZ_XDR_H
#define HAZ_XDR_H

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

