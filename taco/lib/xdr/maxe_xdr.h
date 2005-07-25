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
 * File:        maxe_xdr.h
 * 
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the MAXE Steppermotor data types.
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

#ifndef MAXE_XDR_H
#define MAXE_XDR_H


/*
 *  definitions for steppermotors
 */

#if !defined(FORTRAN)
struct DevMulMove {
 	long action[8];
  	long delay[8];
   	float position[8];
};
typedef struct DevMulMove 	DevMulMove;
bool_t 	_DLLFunc xdr_DevMulMove PT_((XDR *xdrs, DevMulMove *objp));
#endif /* FORTRAN */
#define D_MULMOVE_TYPE		46
#if !defined(FORTRAN)
#define LOAD_MULMOVE_TYPE(A)    xdr_load_type ( D_MULMOVE_TYPE, \
						xdr_DevMulMove, \
						sizeof(DevMulMove), \
						NULL, \
						A )
struct DevMotorLong {
       long axisnum;
       long value;
     };
typedef struct DevMotorLong 	DevMotorLong;
bool_t 	_DLLFunc xdr_DevMotorLong PT_((XDR *xdrs, DevMotorLong *objp));
#endif /* FORTRAN */
#define D_MOTOR_LONG            54
#if !defined(FORTRAN)
#define LOAD_MOTOR_LONG(A)      xdr_load_type ( D_MOTOR_LONG, \
						xdr_DevMotorLong, \
						sizeof(DevMotorLong), \
						NULL, \
						A )
struct DevMotorFloat {
       long axisnum;
       float value;
     };
typedef struct DevMotorFloat 	DevMotorFloat;
bool_t 	_DLLFunc xdr_DevMotorFloat PT_((XDR *xdrs, DevMotorFloat *objp));
#endif /* FORTRAN */
#define D_MOTOR_FLOAT           55
#if !defined(FORTRAN)
#define LOAD_MOTOR_FLOAT(A)     xdr_load_type ( D_MOTOR_FLOAT, \
						xdr_DevMotorFloat, \
						sizeof(DevMotorFloat), \
						NULL, \
						A )
#endif /* FORTRAN */
#endif /* _maxe_xdr_h */

