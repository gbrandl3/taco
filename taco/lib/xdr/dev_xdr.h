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
 * File:        dev_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for general XDR definitions
 *		and the list of all available data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: andy_gotz $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-10-13 13:31:37 $
 *
 *******************************************************************-*/

#ifndef DEV_XDR_H
#define DEV_XDR_H

#include <macros.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(FORTRAN)
typedef char	DevChar;
long    	_DLLFunc xdr_length_DevChar PT_((char *objp));
#endif /* FORTRAN */
                                                                         
/* 
 *  device server types which will be used to pass 
 *  device server arguments.
 */

#if !defined(FORTRAN)
typedef void			DevVoid;
long    			_DLLFunc xdr_length_DevVoid PT_((DevVoid *objp));
#endif /* FORTRAN */
#define D_VOID_TYPE             0L
#if !defined(FORTRAN)
#define LOAD_VOID_TYPE(A)	xdr_load_type (	D_VOID_TYPE, \
						(DevDataFunction) xdr_void, \
						0, \
						xdr_length_DevVoid, \
						A )

typedef char			DevBoolean;
long    			_DLLFunc xdr_length_DevBoolean
				         PT_((DevBoolean *objp));
#endif /* FORTRAN */
#define	D_BOOLEAN_TYPE		1L
#if !defined(FORTRAN)
#define LOAD_BOOLEAN_TYPE(A)	xdr_load_type (	D_BOOLEAN_TYPE, \
						(DevDataFunction) xdr_char, \
						sizeof(DevBoolean), \
						xdr_length_DevBoolean, \
						A )

typedef unsigned short      	DevUShort;
long                        	_DLLFunc xdr_length_DevUShort
					 PT_((DevUShort *objp));
#endif /* FORTRAN */
#define D_USHORT_TYPE       	70L
#if !defined(FORTRAN)
#define LOAD_USHORT_TYPE(A) 	xdr_load_type (D_USHORT_TYPE, \
                                               (DevDataFunction) xdr_u_short, \
                                               sizeof(DevUShort), \
                                               xdr_length_DevUShort, \
                                               A)


typedef short			DevShort;
long  				_DLLFunc xdr_length_DevShort
					 PT_((DevShort *objp));
#endif /* FORTRAN */
#define D_SHORT_TYPE            2L
#if !defined(FORTRAN)
#define LOAD_SHORT_TYPE(A)	xdr_load_type (	D_SHORT_TYPE, \
						(DevDataFunction) xdr_short, \
						sizeof(DevShort), \
						xdr_length_DevShort, \
						A )
#if SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long           DevULong;
#elif SIZEOF_UNSIGNED_INT == 4
typedef unsigned int            DevULong;
#endif

bool_t 				_DLLFunc xdr_DevULong
		 			PT_((XDR *xdrs, DevULong *objp));
long                        	_DLLFunc xdr_length_DevULong
					 PT_((DevULong *objp));
#endif /* FORTRAN */
#define D_ULONG_TYPE       	71L
#if !defined(FORTRAN)
#define LOAD_ULONG_TYPE(A) 	xdr_load_type (D_ULONG_TYPE, \
                                               (DevDataFunction) xdr_DevULong, \
                                               sizeof(DevULong), \
                                               xdr_length_DevULong, \
                                               A)

#if SIZEOF_LONG == 4
typedef long                    DevLong;
#elif SIZEOF_INT == 4
typedef int                     DevLong;
#endif

bool_t 				_DLLFunc xdr_DevLong
		 			PT_((XDR *xdrs, DevLong *objp));
long    			_DLLFunc xdr_length_DevLong PT_((DevLong *objp));
#endif /* FORTRAN */
#define D_LONG_TYPE             3L
#if !defined(FORTRAN)
#define LOAD_LONG_TYPE(A)	xdr_load_type (	D_LONG_TYPE, \
						(DevDataFunction) xdr_DevLong, \
						sizeof(DevLong), \
						xdr_length_DevLong, \
						A )

typedef float			DevFloat;
long    			_DLLFunc xdr_length_DevFloat
					 PT_((DevFloat *objp));
#endif /* FORTRAN */
#define D_FLOAT_TYPE            4L
#if !defined(FORTRAN)
#define LOAD_FLOAT_TYPE(A)	xdr_load_type (	D_FLOAT_TYPE, \
						(DevDataFunction) xdr_float, \
						sizeof(DevFloat), \
						xdr_length_DevFloat, \
						A )

typedef double			DevDouble;
long    			_DLLFunc xdr_length_DevDouble
					 PT_((DevDouble *objp));
#endif /* FORTRAN */
#define D_DOUBLE_TYPE           5L
#if !defined(FORTRAN)
#define LOAD_DOUBLE_TYPE(A)	xdr_load_type (	D_DOUBLE_TYPE, \
						(DevDataFunction) xdr_double, \
						sizeof(DevDouble), \
						xdr_length_DevDouble, \
						A )

typedef char   			*DevString;
bool_t 				_DLLFunc xdr_DevString
					 PT_((XDR *xdrs, DevString *objp));
long   			        _DLLFunc xdr_length_DevString
					 PT_((DevString *objp));
#endif /* FORTRAN */
#define D_STRING_TYPE           6L
#if !defined(FORTRAN)
#define LOAD_STRING_TYPE(A)	xdr_load_type (	D_STRING_TYPE, \
						(DevDataFunction) xdr_wrapstring, \
						sizeof(DevString), \
						xdr_length_DevString, \
						A )


struct DevIntFloat {
	DevLong  state;
	DevFloat value;
};
typedef struct DevIntFloat 	DevIntFloat;
bool_t 	_DLLFunc xdr_DevIntFloat PT_((XDR *xdrs, DevIntFloat *objp));
long    _DLLFunc xdr_length_DevIntFloat PT_((DevIntFloat *objp));
#endif /* FORTRAN */
#define D_INT_FLOAT_TYPE        27L
#if !defined(FORTRAN)
#define LOAD_INT_FLOAT_TYPE(A)	xdr_load_type (	D_INT_FLOAT_TYPE, \
						(DevDataFunction) xdr_DevIntFloat, \
						sizeof(DevIntFloat), \
						xdr_length_DevIntFloat, \
						A )


struct DevFloatReadPoint {
	DevFloat set;
	DevFloat read;
};
typedef struct DevFloatReadPoint DevFloatReadPoint;
bool_t 	_DLLFunc xdr_DevFloatReadPoint
		 PT_((XDR *xdrs, DevFloatReadPoint *objp));
long    _DLLFunc xdr_length_DevFloatReadPoint
		 PT_((DevFloatReadPoint *objp));
#endif /* FORTRAN */
#define D_FLOAT_READPOINT       7L
#if !defined(FORTRAN)
#define LOAD_FLOAT_READPOINT(A)	xdr_load_type (	D_FLOAT_READPOINT, \
						(DevDataFunction) xdr_DevFloatReadPoint, \
						sizeof(DevFloatReadPoint), \
						xdr_length_DevFloatReadPoint, \
						A )


struct DevStateFloatReadPoint {
	DevShort state;
	DevFloat set;
	DevFloat read;
};
typedef struct DevStateFloatReadPoint DevStateFloatReadPoint;
bool_t 	_DLLFunc xdr_DevStateFloatReadPoint
		 PT_((XDR *xdrs, DevStateFloatReadPoint *objp));
long    _DLLFunc xdr_length_DevStateFloatReadPoint
	         PT_((DevStateFloatReadPoint *objp));
#endif /* FORTRAN */
#define D_STATE_FLOAT_READPOINT       8L
#if !defined(FORTRAN)
#define LOAD_STATE_FLOAT_READPOINT(A) xdr_load_type (D_STATE_FLOAT_READPOINT, \
					(DevDataFunction) xdr_DevStateFloatReadPoint, \
					sizeof(DevStateFloatReadPoint), \
					xdr_length_DevStateFloatReadPoint, \
					A )


struct DevLongReadPoint {
	DevLong set;
	DevLong read;
};
typedef struct DevLongReadPoint 	DevLongReadPoint;
bool_t 	_DLLFunc xdr_DevLongReadPoint
		 PT_((XDR *xdrs, DevLongReadPoint *objp));
long    _DLLFunc xdr_length_DevLongReadPoint
		 PT_((DevLongReadPoint *objp));
#endif /* FORTRAN */
#define D_LONG_READPOINT        	22L
#if !defined(FORTRAN)
#define LOAD_LONG_READPOINT(A)    	xdr_load_type (D_LONG_READPOINT, \
						(DevDataFunction) xdr_DevLongReadPoint, \
						sizeof(DevLongReadPoint), \
						xdr_length_DevLongReadPoint, \
						A )


struct DevDoubleReadPoint {
	DevDouble set;
	DevDouble read;
};
typedef struct DevDoubleReadPoint 	DevDoubleReadPoint;
bool_t 	_DLLFunc xdr_DevDoubleReadPoint
		 PT_((XDR *xdrs, DevDoubleReadPoint *objp));
long    _DLLFunc xdr_length_DevDoubleReadPoint
		 PT_((DevDoubleReadPoint *objp));
#endif /* FORTRAN */
#define D_DOUBLE_READPOINT      	23L
#if !defined(FORTRAN)
#define LOAD_DOUBLE_READPOINT(A)    	xdr_load_type (D_DOUBLE_READPOINT, \
						(DevDataFunction) xdr_DevDoubleReadPoint, \
						sizeof(DevDoubleReadPoint), \
						xdr_length_DevDoubleReadPoint, \
						A )


/*
 * variable length array types
 */

struct DevVarCharArray {
	DevULong   length;
	DevChar    *sequence;
};
typedef struct DevVarCharArray 		DevVarCharArray;
bool_t 	_DLLFunc xdr_DevVarCharArray
		 PT_((XDR *xdrs, DevVarCharArray *objp));
long   	_DLLFunc xdr_length_DevVarCharArray
		  PT_((DevVarCharArray *objp));
#endif /* FORTRAN */
#define D_VAR_CHARARR           	9L
#if !defined(FORTRAN)
#define LOAD_VAR_CHARARR(A)   		xdr_load_type (D_VAR_CHARARR, \
						(DevDataFunction) xdr_DevVarCharArray, \
						sizeof(DevVarCharArray), \
						xdr_length_DevVarCharArray, \
						A )


struct DevVarStringArray {
	DevULong   length;
	DevString  *sequence;
};
typedef struct DevVarStringArray 	DevVarStringArray;
bool_t 	_DLLFunc xdr_DevVarStringArray
		 PT_((XDR *xdrs, DevVarStringArray *objp));
long   	_DLLFunc xdr_length_DevVarStringArray
		 PT_((DevVarStringArray *objp));
#endif /* FORTRAN */
#define D_VAR_STRINGARR         	24L
#if !defined(FORTRAN)
#define LOAD_VAR_STRINGARR(A)         	xdr_load_type (D_VAR_STRINGARR, \
						(DevDataFunction) xdr_DevVarStringArray, \
						sizeof(DevVarStringArray), \
						xdr_length_DevVarStringArray, \
						A )

/*
 *  Unsigned short array
 */

struct DevVarUShortArray {
  	DevULong  length;
  	DevUShort *sequence;
};
typedef struct DevVarUShortArray 	DevVarUShortArray;
bool_t  _DLLFunc xdr_DevVarUShortArray
		 PT_((XDR *xdrs, DevVarUShortArray *objp));
long    _DLLFunc xdr_length_DevVarUShortArray
		 PT_((DevVarUShortArray *objp));

#endif /* FORTRAN */
#define D_VAR_USHORTARR      		72L 
#if !defined(FORTRAN)
#define LOAD_VAR_USHORTARR(A) 		xdr_load_type (D_VAR_USHORTARR, \
                                                (DevDataFunction) xdr_DevVarUShortArray, \
                                                sizeof(DevVarUShortArray),\
                                                xdr_length_DevVarUShortArray, \
                                                A)


struct DevVarShortArray {
	DevULong length;
	DevShort *sequence;
};
typedef struct DevVarShortArray 	DevVarShortArray;
bool_t	_DLLFunc xdr_DevVarShortArray
		 PT_((XDR *xdrs, DevVarShortArray *objp));
long   	_DLLFunc xdr_length_DevVarShortArray
		 PT_((DevVarShortArray *objp));
#endif /* FORTRAN */
#define D_VAR_SHORTARR          	10L
#if !defined(FORTRAN)
#define LOAD_VAR_SHORTARR(A)         	xdr_load_type (D_VAR_SHORTARR, \
						(DevDataFunction) xdr_DevVarShortArray, \
						sizeof(DevVarShortArray), \
						xdr_length_DevVarShortArray, \
						A )

/*
 *  Unsigned long array
 */

struct DevVarULongArray {
	DevULong length;
	DevULong *sequence;
};
typedef struct DevVarULongArray		DevVarULongArray;
bool_t	_DLLFunc xdr_DevVarULongArray
		 PT_((XDR *xdrs, DevVarULongArray *objp));
long	_DLLFunc xdr_length_DevVarULongArray
		 PT_((DevVarULongArray *objp));
#endif /* FORTRAN */
#define D_VAR_ULONGARR			69L
#if !defined(FORTRAN)
#define LOAD_VAR_ULONGARR(A)        	xdr_load_type (D_VAR_ULONGARR, \
						(DevDataFunction) xdr_DevVarULongArray, \
						sizeof(DevVarULongArray), \
						xdr_length_DevVarULongArray,\
						A )


struct DevVarLongArray {
	DevULong length;
	DevLong  *sequence;
};
typedef struct DevVarLongArray 		DevVarLongArray;
bool_t 	_DLLFunc xdr_DevVarLongArray
		 PT_((XDR *xdrs, DevVarLongArray *objp));
long   	_DLLFunc xdr_length_DevVarLongArray
		 PT_((DevVarLongArray *objp));
#endif /* FORTRAN */
#define D_VAR_LONGARR           	11L
#if !defined(FORTRAN)
#define LOAD_VAR_LONGARR(A)         	xdr_load_type (D_VAR_LONGARR, \
						(DevDataFunction) xdr_DevVarLongArray, \
						sizeof(DevVarLongArray), \
						xdr_length_DevVarLongArray, \
						A )


struct DevVarFloatArray {
	DevULong length;
	DevFloat *sequence;
};
typedef struct DevVarFloatArray 	DevVarFloatArray;
bool_t 	_DLLFunc xdr_DevVarFloatArray
		 PT_((XDR *xdrs, DevVarFloatArray *objp));
long   	_DLLFunc xdr_length_DevVarFloatArray
		 PT_((DevVarFloatArray *objp));
#endif /* FORTRAN */
#define D_VAR_FLOATARR          	12L
#if !defined(FORTRAN)
#define LOAD_VAR_FLOATARR(A)         	xdr_load_type (D_VAR_FLOATARR, \
						(DevDataFunction) xdr_DevVarFloatArray, \
						sizeof(DevVarFloatArray), \
						xdr_length_DevVarFloatArray, \
						A )


struct DevVarDoubleArray {
	DevULong  length;
	DevDouble *sequence;
};
typedef struct DevVarDoubleArray 	DevVarDoubleArray;
bool_t 	_DLLFunc xdr_DevVarDoubleArray
		 PT_((XDR *xdrs, DevVarDoubleArray *objp));
long   	_DLLFunc xdr_length_DevVarDoubleArray
		  PT_((DevVarDoubleArray *objp));
#endif /* FORTRAN */
#define D_VAR_DOUBLEARR         	68L
#if !defined(FORTRAN)
#define LOAD_VAR_DOUBLEARR(A)         	xdr_load_type (D_VAR_DOUBLEARR, \
						(DevDataFunction) xdr_DevVarDoubleArray, \
						sizeof(DevVarDoubleArray), \
						xdr_length_DevVarDoubleArray,\
						A )


struct DevVarFloatReadPointArray {
	DevULong	  length;
	DevFloatReadPoint *sequence;
};
typedef struct DevVarFloatReadPointArray DevVarFloatReadPointArray;
bool_t 	_DLLFunc xdr_DevVarFloatReadPointArray
		 PT_((XDR *xdrs, DevVarFloatReadPointArray *objp));
long   	_DLLFunc xdr_length_DevVarFloatReadPointArray
		 PT_((DevVarFloatReadPointArray *objp));
#endif /* FORTRAN */
#define D_VAR_FRPARR            	25L
#if !defined(FORTRAN)
#define LOAD_VAR_FRPARR(A)         	xdr_load_type (D_VAR_FRPARR, \
					  (DevDataFunction) xdr_DevVarFloatReadPointArray, \
					  sizeof(DevVarFloatReadPointArray), \
					  xdr_length_DevVarFloatReadPointArray,\
					  A )


struct DevVarStateFloatReadPointArray {
	DevULong        	length;
	DevStateFloatReadPoint 	*sequence;
};
typedef struct DevVarStateFloatReadPointArray DevVarStateFloatReadPointArray;
bool_t 	_DLLFunc xdr_DevVarStateFloatReadPointArray
		 PT_((XDR *xdrs, DevVarStateFloatReadPointArray *objp));
long   	_DLLFunc xdr_length_DevVarStateFloatReadPointArray
		 PT_((DevVarStateFloatReadPointArray *objp));
#endif /* FORTRAN */
#define D_VAR_SFRPARR           73L
#if !defined(FORTRAN)
#define LOAD_VAR_SFRPARR(A)     xdr_load_type (D_VAR_SFRPARR, \
				(DevDataFunction) xdr_DevVarStateFloatReadPointArray, \
				sizeof(DevVarStateFloatReadPointArray), \
				xdr_length_DevVarStateFloatReadPointArray,\
				A )



struct DevVarLongReadPointArray {
	DevULong 	  length;
	DevLongReadPoint  *sequence;
};
typedef struct DevVarLongReadPointArray DevVarLongReadPointArray;
bool_t  _DLLFunc xdr_DevVarLongReadPointArray
		 PT_((XDR *xdrs, DevVarLongReadPointArray *objp));
long   	_DLLFunc xdr_length_DevVarLongReadPointArray
		 PT_((DevVarLongReadPointArray *objp));
#endif /* FORTRAN */
#define D_VAR_LRPARR            	45L
#if !defined(FORTRAN)
#define LOAD_VAR_LRPARR(A)         	xdr_load_type (D_VAR_LRPARR, \
					  (DevDataFunction) xdr_DevVarLongReadPointArray, \
					  sizeof(DevVarLongReadPointArray), \
					  xdr_length_DevVarLongReadPointArray, \
					  A)


/* 
 *  Opaque data type to transfer blocks of data
 */

struct DevOpaque {
	DevULong	length;
	DevChar    	*sequence;
};
typedef struct DevOpaque 		DevOpaque;
bool_t 					_DLLFunc xdr_DevOpaque
					PT_((XDR *xdrs, DevOpaque *objp));
bool_t 					_DLLFunc xdr_DevOpaqueRaw
					PT_((XDR *xdrs, DevOpaque *objp));
long   					_DLLFunc xdr_length_DevOpaque
					PT_((DevOpaque *objp));
#endif /* FORTRAN */
#define D_OPAQUE_TYPE           	47L
#if !defined(FORTRAN)
#define LOAD_OPAQUE_TYPE(A)        	xdr_load_type (D_OPAQUE_TYPE, \
						(DevDataFunction) xdr_DevOpaque, \
						sizeof(DevOpaque), \
						xdr_length_DevOpaque,\
						A )
#endif /* FORTRAN */

#ifdef __cplusplus
}
#endif

#endif /* _dev_xdr_h */
