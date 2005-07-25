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
 * File:        dev_xdr.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr_routines for device server data passing.
 *
 * Author(s):	Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef WIN32
#include "config.h"
#endif /* WIN32 */
#include <dev_xdr.h>

/*
 *  Simple definitions of the xdr length of the
 *  basic data types.
 */

#define STD_XDR_LENGTH	   4       /* Standard length of a XDR data block */

static long adjust_size(const long value)
{
	return (value + STD_XDR_LENGTH - value % STD_XDR_LENGTH); 
}

/* D_VOID_TYPE */
long xdr_length_DevVoid(DevVoid *objp)
{
	return (0);
}

/* D_BOOLEAN_TYPE */
long xdr_length_DevBoolean(DevBoolean *objp)
{
	return (STD_XDR_LENGTH);
}

/* D_CHAR_TYPE */
long xdr_length_DevChar(DevChar *objp)
{
	return (STD_XDR_LENGTH);
}

/* D_USHORT_TYPE */
long xdr_length_DevUShort(DevUShort *objp)
{
  	return (STD_XDR_LENGTH);
}

/* D_SHORT_TYPE */
long xdr_length_DevShort(DevShort *objp)
{
	return (STD_XDR_LENGTH);
}

/* D_ULONG_TYPE */
long xdr_length_DevULong(DevULong *objp)
{
  	return (STD_XDR_LENGTH);
}

/* D_LONG_TYPE */
long xdr_length_DevLong(DevLong *objp)
{
	return (STD_XDR_LENGTH);
}

/* D_FLOAT_TYPE */
long xdr_length_DevFloat(DevFloat *objp)
{
	return (STD_XDR_LENGTH);
}

/* D_DOUBLE_TYPE */
long xdr_length_DevDouble(DevDouble *objp)
{
	return (2*STD_XDR_LENGTH);
}

/* D_STRING_TYPE */
bool_t xdr_DevString(XDR *xdrs, DevString *objp)
{
	if (!xdr_string(xdrs, objp, MAXU_INT)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevString (DevString *objp)
{
/* number of bytes coded as unsigned int */
	long  length = STD_XDR_LENGTH;

/* length of the string in bytes */
	length += strlen (*objp);
/* 
 * only packets of four bytes can be send by XDR  
 * Calculate the next multiple of four. 
 */
	length = adjust_size(length);
	return (length);
}


/* D_INT_FLOAT_TYPE */
bool_t xdr_DevIntFloat(XDR *xdrs, DevIntFloat *objp)
{
	if (!xdr_long(xdrs, &objp->state)) 
		return (FALSE);
	if (!xdr_float(xdrs, &objp->value)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevIntFloat(DevIntFloat *objp)
{
	return xdr_length_DevLong  (&objp->state) + xdr_length_DevFloat (&objp->value);
}

/* D_FLOAT_READPOINT */
bool_t xdr_DevFloatReadPoint(XDR *xdrs, DevFloatReadPoint *objp)
{
	if (!xdr_float(xdrs, &objp->set)) 
		return (FALSE);
	if (!xdr_float(xdrs, &objp->read)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevFloatReadPoint(DevFloatReadPoint *objp)
{
	return xdr_length_DevFloat (&objp->set) + xdr_length_DevFloat (&objp->read);
}

/* D_STATE_FLOAT_READPOINT */
bool_t xdr_DevStateFloatReadPoint(XDR *xdrs, DevStateFloatReadPoint *objp)
{
	if (!xdr_short(xdrs, &objp->state)) 
		return (FALSE);
	if (!xdr_float(xdrs, &objp->set)) 
		return (FALSE);
	if (!xdr_float(xdrs, &objp->read)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevStateFloatReadPoint(DevStateFloatReadPoint *objp)
{
	return xdr_length_DevShort (&objp->state) + xdr_length_DevFloat (&objp->set) + xdr_length_DevFloat (&objp->read);
}

/* D_LONG_READPOINT */
bool_t xdr_DevLongReadPoint(XDR *xdrs, DevLongReadPoint *objp)
{
	if (!xdr_long(xdrs, &objp->set)) 
		return (FALSE);
	if (!xdr_long(xdrs, &objp->read)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevLongReadPoint(DevLongReadPoint *objp)
{
	return xdr_length_DevLong (&objp->set) + xdr_length_DevLong (&objp->read);
}

/* D_DOUBLE_READPOINT */
bool_t xdr_DevDoubleReadPoint(XDR *xdrs, DevDoubleReadPoint *objp)
{
	if (!xdr_double(xdrs, &objp->set)) 
		return (FALSE);
	if (!xdr_double(xdrs, &objp->read)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevDoubleReadPoint(DevDoubleReadPoint *objp)
{
	return xdr_length_DevDouble (&objp->set) + xdr_length_DevDouble (&objp->read);
}

/*
 *  xdr_routines for variable length arrays 
 */

/* D_VAR_CHARARR */
bool_t xdr_DevVarCharArray(XDR *xdrs, DevVarCharArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, MAXU_INT, sizeof(DevChar), (xdrproc_t)xdr_char)) 
                return (FALSE);
        return (TRUE);
}

long xdr_length_DevVarCharArray (DevVarCharArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);
/*
 *  now calculate the length of the array
 */
	length += (objp->length * sizeof(DevChar));
	length = adjust_size(length);
	return (length);
}

/* D_VAR_STRINGARR */
bool_t xdr_DevVarStringArray(XDR *xdrs, DevVarStringArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevString), (xdrproc_t)xdr_DevString)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarStringArray (DevVarStringArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long 	length = xdr_length_DevLong ((long *)&objp->length),
		type_length;
	u_long	i;

/*
 *  now find the lengthes of all strings in the array
 *  and add the up.
 */
	for (i = 0; i < objp->length; i++)
	{
	   	if ( (type_length = xdr_length_DevString (&objp->sequence[i])) == -1 )
			return (-1);
	   	length += type_length;
	}
	return (length);
}

/* D_VAR_USHORTARR */
bool_t xdr_DevVarUShortArray (XDR *xdrs, DevVarUShortArray *objp)
{
  	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevUShort), (xdrproc_t)xdr_u_short)) 
    		return (FALSE);
  	return (TRUE);
}

long xdr_length_DevVarUShortArray (DevVarUShortArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
  	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
  	length += ( objp->length * sizeof(DevUShort));
	length = adjust_size(length);
  	return (length);
}

/* D_VAR_SHORTARR */
bool_t xdr_DevVarShortArray(XDR *xdrs, DevVarShortArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
		MAXU_INT, sizeof(DevShort), (xdrproc_t)xdr_short)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarShortArray (DevVarShortArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
  	length += ( objp->length * sizeof(DevShort));
	length = adjust_size(length); 
	return (length);
}


/* D_VAR_ULONGARR */
bool_t xdr_DevVarULongArray(XDR *xdrs, DevVarULongArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevULong), (xdrproc_t)xdr_long)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarULongArray (DevVarULongArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = length + xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
	length += ( objp->length * sizeof(DevULong));
	length = adjust_size(length);
	return (length);
}


/* D_VAR_LONGARR */
bool_t xdr_DevVarLongArray(XDR *xdrs, DevVarLongArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevLong), (xdrproc_t)xdr_long)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarLongArray (DevVarLongArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
	length += ( objp->length * sizeof(DevLong));
	length = adjust_size(length);
	return (length);
}

/* D_VAR_FLOATARR */
bool_t xdr_DevVarFloatArray(XDR *xdrs, DevVarFloatArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevFloat), (xdrproc_t)xdr_float)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarFloatArray (DevVarFloatArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);
/*
 *  now calculate the length of the array
 */
	length += ( objp->length * sizeof(DevFloat));
	length = adjust_size(length);
	return (length);
}

/* D_VAR_DOUBLEARR */
bool_t xdr_DevVarDoubleArray(XDR *xdrs, DevVarDoubleArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevDouble), (xdrproc_t)xdr_double)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarDoubleArray (DevVarDoubleArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);
/*
 *  now calculate the length of the array
 */
	length += ( objp->length * sizeof(DevDouble));
	length = adjust_size(length);
	return (length);
}

/* D_VAR_FRPARR */
bool_t xdr_DevVarFloatReadPointArray(XDR *xdrs, DevVarFloatReadPointArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevFloatReadPoint), (xdrproc_t)xdr_DevFloatReadPoint)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarFloatReadPointArray (DevVarFloatReadPointArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
	length += (objp->length * sizeof(DevFloatReadPoint));
	length = adjust_size(length);
	return (length);
}

/* D_VAR_SFRPARR */
bool_t xdr_DevVarStateFloatReadPointArray(XDR *xdrs, DevVarStateFloatReadPointArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevStateFloatReadPoint), (xdrproc_t)xdr_DevStateFloatReadPoint)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarStateFloatReadPointArray (DevVarStateFloatReadPointArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
	length += (objp->length * sizeof(DevStateFloatReadPoint));
	length = adjust_size(length); 
	return (length);
}

/* D_VAR_LRPARR */
bool_t xdr_DevVarLongReadPointArray(XDR *xdrs, DevVarLongReadPointArray *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, (u_int *)&objp->length, 
			MAXU_INT, sizeof(DevLongReadPoint), (xdrproc_t)xdr_DevLongReadPoint)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevVarLongReadPointArray (DevVarLongReadPointArray *objp)
{
/*
 *  four bytes for the number of array elements
 */
	long length = xdr_length_DevLong ((long *)&objp->length);

/*
 *  now calculate the length of the array
 */
	length += (objp->length * sizeof(DevLongReadPoint));
	length = adjust_size(length);
	return (length);
}

/*
 *  Opaque data type to transfer blocks of data
 */

/* D_OPAQUE_TYPE */
bool_t xdr_DevOpaque(XDR *xdrs, DevOpaque *objp)
{
        if (!xdr_bytes(xdrs, (char **)&objp->sequence, (u_int *)&objp->length, MAXU_INT)) 
		return (FALSE);
	return (TRUE);
}

long xdr_length_DevOpaque (DevOpaque *objp)
{
/* 
 * only packets of four bytes can be send by XDR
 * Calculate the next multiple of four. 
 */
	return adjust_size(objp->length);
}

