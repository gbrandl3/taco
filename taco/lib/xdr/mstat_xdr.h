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
 * File:        mstat_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for the XDR definitions
 *		for the ESRF machine status data type.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	March 1993
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef MSTAT_XDR_H
#define MSTAT_XDR_H

/*
 *  definition for the ESRF machine status array
 */

struct SysState { 
	DevString 	sys_name;
	DevLong 	sys_ident;		
	DevLong 	sys_state;		
};
typedef struct SysState 	SysState;
bool_t 	_DLLFunc xdr_SysState PT_((XDR *xdrs, SysState *objp));
long   	_DLLFunc xdr_length_SysState PT_((SysState *objp));


struct VarSysStateArray {
	DevULong 	length;
	SysState 	*sequence;
};
typedef struct VarSysStateArray VarSysStateArray;
bool_t 	_DLLFunc xdr_VarSysStateArray
		 PT_((XDR *xdrs, VarSysStateArray *objp));
long   	_DLLFunc xdr_length_VarSysStateArray PT_((VarSysStateArray *objp));
#define D_VAR_MSTATARR          58
#define LOAD_VAR_MSTATARR(A)    xdr_load_type ( D_VAR_MSTATARR, \
						xdr_VarSysStateArray, \
						sizeof(VarSysStateArray), \
						xdr_length_VarSysStateArray, \
						A )
#endif /* _mstat_xdr_h */

