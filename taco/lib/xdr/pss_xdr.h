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
 * File:        pss_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the Personal Safety System data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef PSS_XDR_H
#define PSS_XDR_H

/*
 * definitions for PSS
 */
struct PSSInterlockStatus {
	DevLong		Module;
	DevLong		GuardA;
	DevLong		GuardB;
	DevLong		GuardC;
	DevUShort 	Interlocks;
};
typedef struct PSSInterlockStatus PSSInterlockStatus;
bool_t 	_DLLFunc xdr_PSSInterlockStatus
		 PT_((XDR *xdrs, PSSInterlockStatus *objp));
long   	_DLLFunc xdr_length_PSSInterlockStatus
		 PT_((PSSInterlockStatus *objp));


struct DevDaresburyStatus {
	DevULong 	   length;
	PSSInterlockStatus *sequence;
};
typedef struct DevDaresburyStatus DevDaresburyStatus;
bool_t 	_DLLFunc xdr_DevDaresburyStatus
		 PT_((XDR *xdrs, DevDaresburyStatus *objp));
long   	_DLLFunc xdr_length_DevDaresburyStatus
		 PT_((DevDaresburyStatus *objp));
#define D_PSS_STATUS            39
#define LOAD_PSS_STATUS(A)      xdr_load_type ( D_PSS_STATUS, \
                                                xdr_DevDaresburyStatus, \
                                                sizeof(DevDaresburyStatus), \
                                                xdr_length_DevDaresburyStatus, \
                                                A )
#endif /* _pss_xdr_h */

