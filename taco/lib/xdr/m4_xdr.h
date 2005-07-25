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
 * File:        m4_xdr.h
 *
 * Project:       
 *
 * Description: Data types for m4 (four mirrors device)
 *
 * Author(s):  	C. Dominguez
 *		$Author: jkrueger1 $
 *
 * Original:	May 93
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef M4_XDR_H
#define M4_XDR_H


/* Structures for the four mirror device */

struct DevVarLongFloatSet
{
	long length;
	long *axis;
	long *value;
        float *valuef;
};
typedef struct DevVarLongFloatSet DevVarLongFloatSet;
bool_t 	_DLLFunc xdr_DevVarLongFloatSet
		 PT_((XDR *xdrs, DevVarLongFloatSet *objp));
#define D_VAR_LONGFLOATSET      59
#define LOAD_VAR_LONGFLOATSET(A) xdr_load_type ( D_VAR_LONGFLOATSET, \
                                                xdr_DevVarLongFloatSet, \
                                                sizeof(DevVarLongFloatSet), \
                                                NULL, \
                                                A )


struct DevVarLongRead
{
	long length;
        long *value;
	long *stat;
	long *err;
};
typedef struct DevVarLongRead 	DevVarLongRead; 
bool_t 	_DLLFunc xdr_DevVarLongRead
		 PT_((XDR *xdrs, DevVarLongRead *objp));
#define D_VAR_LONGREAD          60
#define LOAD_VAR_LONGREAD(A)    xdr_load_type ( D_VAR_LONGREAD, \
                                                xdr_DevVarLongRead, \
                                                sizeof(DevVarLongRead), \
                                                NULL, \
                                                A )


struct DevVarPosRead
{
	long length;
	float *pos;
	long *stat;
	long *err;
};
typedef struct DevVarPosRead 	DevVarPosRead; 
bool_t 	_DLLFunc xdr_DevVarPosRead PT_((XDR *xdrs, DevVarPosRead *objp));
#define D_VAR_POSREAD           61
#define LOAD_VAR_POSREAD(A)     xdr_load_type ( D_VAR_POSREAD, \
                                                xdr_DevVarPosRead, \
                                                sizeof(DevVarPosRead), \
                                                NULL, \
                                                A )


struct DevVarAxeRead
{
	long length;
	float *m_pos;
	float *s_pos;
	long *m_stat;
	long *s_stat;
	long *swit;
        long *clutch;
	long *err;
};
typedef struct DevVarAxeRead 	DevVarAxeRead; 
bool_t 	_DLLFunc xdr_DevVarAxeRead PT_((XDR *xdrs, DevVarAxeRead *objp));
#define D_VAR_AXEREAD           62
#define LOAD_VAR_AXEREAD(A)     xdr_load_type ( D_VAR_AXEREAD, \
                                                xdr_DevVarAxeRead, \
                                                sizeof(DevVarAxeRead), \
                                                NULL, \
                                                A )


struct DevVarParRead
{
	long length;
	float *veloc;
	float *acc;
	float *fstep;
	float *hslew;
        float *backlash;
	long *stepmode;
	long *unit;
	long *err;
};
typedef struct DevVarParRead 	DevVarParRead; 
bool_t 	_DLLFunc xdr_DevVarParRead PT_((XDR *xdrs, DevVarParRead *objp));
#define D_VAR_PARREAD           63
#define LOAD_VAR_PARREAD(A)     xdr_load_type ( D_VAR_PARREAD, \
                                                xdr_DevVarParRead, \
                                                sizeof(DevVarParRead), \
                                                NULL, \
                                                A )


struct DevVarErrRead
{
	long length;
	long *err;
};
typedef struct DevVarErrRead 	DevVarErrRead; 
bool_t 	_DLLFunc xdr_DevVarErrRead PT_((XDR *xdrs, DevVarErrRead *objp));
#define D_VAR_ERRREAD           64
#define LOAD_VAR_ERRREAD(A)     xdr_load_type ( D_VAR_ERRREAD, \
                                                xdr_DevVarErrRead, \
                                                sizeof(DevVarErrRead), \
                                                NULL, \
                                                A )

#endif /* _m4_xdr_h */

