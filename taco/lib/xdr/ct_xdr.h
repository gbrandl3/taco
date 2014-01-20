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
 * File:        ct_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for the XDR definitions
 *		of the Current Transformer data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef CT_XDR_H
#define CT_XDR_H

/*
 * definitions for current transformer types
 */

struct DevCtIntLifeTime {
		DevFloat DeltaIntensity;  /* delta-intensity for this measure */
		DevFloat LifeTime;        /* value of the life-time */
		DevLong  DateTicks;       /* date in ticks since midnight */
		DevLong  DeltaTused;      /* delta-T used for calculations */
};
typedef struct DevCtIntLifeTime DevCtIntLifeTime;
bool_t 	_DLLFunc xdr_DevCtIntLifeTime
		 PT_((XDR *xdrs, DevCtIntLifeTime *objp));
long   	_DLLFunc xdr_length_DevCtIntLifeTime  PT_((DevCtIntLifeTime *objp));


struct DevVarCtIntLifeTimeArray {
		DevULong          length;
		DevCtIntLifeTime  *sequence;
};
typedef struct DevVarCtIntLifeTimeArray DevVarCtIntLifeTimeArray;
bool_t 	_DLLFunc xdr_DevVarCtIntLifeTimeArray
		 PT_((XDR *xdrs, DevVarCtIntLifeTimeArray *objp));
long   	_DLLFunc xdr_length_DevVarCtIntLifeTimeArray
		 PT_((DevVarCtIntLifeTimeArray *objp));
#define D_CT_LIFETIME           42
#define LOAD_CT_LIFETIME(A)     xdr_load_type ( D_CT_LIFETIME, \
                                         xdr_DevVarCtIntLifeTimeArray, \
                                         sizeof(DevVarCtIntLifeTimeArray), \
                                         xdr_length_DevVarCtIntLifeTimeArray, \
                                         A )

#endif /* _ct_xdr_h */

