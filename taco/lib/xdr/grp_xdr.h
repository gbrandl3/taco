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
 * File:        grp_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		data types.
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

#ifndef GRP_XDR_H
#define GRP_XDR_H

struct Frame {
	DevDouble	value;
	DevShort	output;
	DevShort	pause;
};
typedef struct Frame 		Frame;
bool_t 	_DLLFunc xdr_Frame  PT_((XDR *xdrs, Frame *objp));
long   	_DLLFunc xdr_length_Frame  PT_((Frame *objp));


struct FramePair {
	Frame		dead;
	Frame		live;
};
typedef struct FramePair 	FramePair;
bool_t 	_DLLFunc xdr_FramePair  PT_((XDR *xdrs, FramePair *objp));
long    _DLLFunc xdr_length_FramePair  PT_((FramePair *objp));


struct DevGrpFramePair {
	DevULong	nb_framepair;
	FramePair	framepair;
};
typedef struct DevGrpFramePair 	DevGrpFramePair;
bool_t 	_DLLFunc xdr_DevGrpFramePair PT_((XDR *xdrs, DevGrpFramePair *objp));
long   	_DLLFunc xdr_length_DevGrpFramePair  PT_((DevGrpFramePair *objp));
#define D_GRPFP_TYPE            65
#define LOAD_GRPFP_TYPE(A)      xdr_load_type ( D_GRPFP_TYPE, \
                                                xdr_DevGrpFramePair, \
                                                sizeof(DevGrpFramePair), \
                                                xdr_length_DevGrpFramePair, \
                                                A )


struct DevGrpFramePairArray {
	DevULong	length;
	DevGrpFramePair	*sequence;
};
typedef struct DevGrpFramePairArray DevGrpFramePairArray;
bool_t 	_DLLFunc xdr_DevGrpFramePairArray
		 PT_((XDR *xdrs, DevGrpFramePairArray *objp));
long   	_DLLFunc xdr_length_DevGrpFramePairArray
		 PT_((DevGrpFramePairArray *objp));
#define D_VAR_GRPFPARR          66
#define LOAD_VAR_GRPFPARR(A)    xdr_load_type ( D_VAR_GRPFPARR, \
                                            xdr_DevGrpFramePairArray, \
                                            sizeof(DevGrpFramePairArray), \
                                            xdr_length_DevGrpFramePairArray, \
                                            A )
#endif /* _grp_xdr_h */

