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
 * File:        ws_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Wire Scanner data types.
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

#ifndef WS_XDR_H
#define WS_XDR_H

/*
 * definitions for Wire Scanner types
 */

struct DevWsBeamFitParm { 
	DevVarFloatArray pos_array;
	DevVarFloatArray meas_array;
	DevVarFloatArray perc_array;
	DevShort         mode;
};
typedef struct DevWsBeamFitParm DevWsBeamFitParm;
bool_t 	_DLLFunc xdr_DevWsBeamFitParm 
	         PT_((XDR *xdrs, DevWsBeamFitParm *objp));
#define D_WS_BEAMFITPARM        15
#define LOAD_WS_BEAMFITPARM(A)  xdr_load_type ( D_WS_BEAMFITPARM, \
						xdr_DevWsBeamFitParm, \
						sizeof(DevWsBeamFitParm), \
						NULL, \
						A )

#endif /* _ws_xdr_h */

