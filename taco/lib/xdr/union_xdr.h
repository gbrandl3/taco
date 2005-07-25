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
 * File:        union_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for ancient union type.
 *
 * Author(s):  	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef UNION_XDR_H
#define UNION_XDR_H

#include <dev_xdr.h>
#include <bpm_xdr.h>
#include <ws_xdr.h>

/*
 *  definition for an union type 
 *  this type exits only for compatibility reasons with
 *  the old device server system !
 */

struct DevUnion {
	short designator;
	union {
	      char B_data;
	      short S_data;
	      long L_data;
	      float F_data;
	      double D_data;
	      char *STR_data;
	      DevFloatReadPoint FRP_data;
	      DevStateFloatReadPoint SFRP_data;
	      DevVarCharArray VCA_data;
	      DevVarShortArray VSA_data;
	      DevVarLongArray VLA_data;
	      DevVarFloatArray VFA_data;
	      DevBpmPosField BPMPF_data;
	      DevBpmElecField BPMEF_data;
	      DevWsBeamFitParm WSBFP_data;
	      }u;
};
typedef struct DevUnion 	DevUnion;
typedef struct DevUnion 	DevArg;
bool_t 	_DLLFunc xdr_DevUnion PT_((XDR *xdrs, DevUnion *objp));
#define D_UNION_TYPE            19
#define LOAD_UNION_TYPE(A)      xdr_load_type ( D_UNION_TYPE, \
						xdr_DevUnion, \
						sizeof(DevUnion), \
						NULL, \
						A )

#endif /* _union_xdr_h */

