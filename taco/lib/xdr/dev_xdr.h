/*+*******************************************************************

 File:          dev_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for general XDR definitions
		and the list of all available data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-04-25 11:21:46 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _dev_xdr_h
#define _dev_xdr_h

#include <macros.h>

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
long    			_DLLFunc xdr_length_DevVoid PT_((void *objp));
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
				         PT_((char *objp));
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
					 PT_((unsigned short *objp));
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
					 PT_((short *objp));
#endif /* FORTRAN */
#define D_SHORT_TYPE            2L
#if !defined(FORTRAN)
#define LOAD_SHORT_TYPE(A)	xdr_load_type (	D_SHORT_TYPE, \
						(DevDataFunction) xdr_short, \
						sizeof(DevShort), \
						xdr_length_DevShort, \
						A )

typedef unsigned long      	DevULong;
long                        	_DLLFunc xdr_length_DevULong
					 PT_((unsigned long *objp));
#endif /* FORTRAN */
#define D_ULONG_TYPE       	71L
#if !defined(FORTRAN)
#define LOAD_ULONG_TYPE(A) 	xdr_load_type (D_ULONG_TYPE, \
                                               (DevDataFunction) xdr_u_long, \
                                               sizeof(DevULong), \
                                               xdr_length_DevULong, \
                                               A)


typedef	long			DevLong;
long    			_DLLFunc xdr_length_DevLong PT_((long *objp));
#endif /* FORTRAN */
#define D_LONG_TYPE             3L
#if !defined(FORTRAN)
#define LOAD_LONG_TYPE(A)	xdr_load_type (	D_LONG_TYPE, \
						(DevDataFunction) xdr_long, \
						sizeof(DevLong), \
						xdr_length_DevLong, \
						A )

typedef float			DevFloat;
long    			_DLLFunc xdr_length_DevFloat
					 PT_((float *objp));
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
					 PT_((double *objp));
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
					 PT_((XDR *xdrs, char **objp));
long   			        _DLLFunc xdr_length_DevString
					 PT_((char **objp));
#endif /* FORTRAN */
#define D_STRING_TYPE           6L
#if !defined(FORTRAN)
#define LOAD_STRING_TYPE(A)	xdr_load_type (	D_STRING_TYPE, \
						(DevDataFunction) xdr_wrapstring, \
						sizeof(DevString), \
						xdr_length_DevString, \
						A )


struct DevIntFloat {
	long  state;
	float value;
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
	float set;
	float read;
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
	short state;
	float set;
	float read;
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
	long set;
	long read;
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
	double set;
	double read;
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
	u_int length;
	char  *sequence;
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
	u_int 	  length;
	DevString *sequence;
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
  	u_int   length;
  	u_short *sequence;
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
	u_int length;
	short *sequence;
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
	u_int length;
	unsigned long *sequence;
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
	u_int length;
	long  *sequence;
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
	u_int length;
	float *sequence;
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
	u_int length;
	double *sequence;
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
	u_int 		  length;
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
	u_int 		        length;
	DevStateFloatReadPoint *sequence;
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
	u_int 		  length;
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
	u_int	length;
	char    *sequence;
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
