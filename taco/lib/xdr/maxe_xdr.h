
/*+*******************************************************************

 File:          maxe_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the MAXE Steppermotor data types.

 Author(s):  	Jens Meyer

 Original:	July 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _maxe_xdr_h
#define _maxe_xdr_h


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

