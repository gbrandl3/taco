static char RcsId[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/dev_xdr.c,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";

/*+*******************************************************************

 File:          dev_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for device server data passing.

 Author(s):	Jens Meyer
		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>

/*
 *  Simple definitions of the xdr length of the
 *  basic data types.
 */

#define STD_XDR_LENGTH	   4       /* Standart length of a XDR data block */


/* D_VOID_TYPE */

long
xdr_length_DevVoid(objp)
	DevVoid *objp;
{
	return (0);
}


/* D_BOOLEAN_TYPE */

long
xdr_length_DevBoolean(objp)
	DevBoolean *objp;
{
	return (STD_XDR_LENGTH);
}


/* D_CHAR_TYPE */

long
xdr_length_DevChar(objp)
	DevChar *objp;
{
	return (STD_XDR_LENGTH);
}


/* D_USHORT_TYPE */

long 
xdr_length_DevUShort(objp)
     	DevUShort *objp;
{
  	return (STD_XDR_LENGTH);
}


/* D_SHORT_TYPE */

long
xdr_length_DevShort(objp)
	DevShort *objp;
{
	return (STD_XDR_LENGTH);
}


/* D_ULONG_TYPE */

long 
xdr_length_DevULong(objp)
     	DevULong *objp;
{
  	return (STD_XDR_LENGTH);
}


/* D_LONG_TYPE */

long
xdr_length_DevLong(objp)
	DevLong *objp;
{
	return (STD_XDR_LENGTH);
}


/* D_FLOAT_TYPE */

long
xdr_length_DevFloat(objp)
	DevFloat *objp;
{
	return (STD_XDR_LENGTH);
}


/* D_DOUBLE_TYPE */

long
xdr_length_DevDouble(objp)
	DevDouble *objp;
{
	return (2*STD_XDR_LENGTH);
}


/* D_STRING_TYPE */

bool_t
xdr_DevString(xdrs, objp)
	XDR *xdrs;
	DevString *objp;
{
	if (!xdr_string(xdrs, objp, MAXU_INT)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevString (objp)
	DevString *objp;
{
	long  length = 0;
	short add_bytes;

	/* number of bytes coded as unsigned int */
	length = STD_XDR_LENGTH;

	/* length of the string in bytes */
	length = length + strlen (*objp);

	/* only packets of four bytes can be send by XDR
	 * Calculate the next multiple of four. */

	if ( (add_bytes = (STD_XDR_LENGTH - 
			  (length - (STD_XDR_LENGTH * 
			  (int)(length / STD_XDR_LENGTH))))) == STD_XDR_LENGTH)
	   {
	   add_bytes = 0;
	   }

	length = length + add_bytes;

	return (length);
}


/* D_INT_FLOAT_TYPE */

bool_t
xdr_DevIntFloat(xdrs, objp)
	XDR *xdrs;
	DevIntFloat *objp;
{
	if (!xdr_long(xdrs, &objp->state)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->value)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevIntFloat(objp)
	DevIntFloat *objp;
{
	long  length = 0;

	length = length + xdr_length_DevLong  (&objp->state);
	length = length + xdr_length_DevFloat (&objp->value);

	return (length);
}



/* D_FLOAT_READPOINT */

bool_t
xdr_DevFloatReadPoint(xdrs, objp)
	XDR *xdrs;
	DevFloatReadPoint *objp;
{
	if (!xdr_float(xdrs, &objp->set)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->read)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevFloatReadPoint(objp)
	DevFloatReadPoint *objp;
{
	long  length = 0;

	length = length + xdr_length_DevFloat (&objp->set);
	length = length + xdr_length_DevFloat (&objp->read);

	return (length);
}


/* D_STATE_FLOAT_READPOINT */

bool_t
xdr_DevStateFloatReadPoint(xdrs, objp)
	XDR *xdrs;
	DevStateFloatReadPoint *objp;
{
	if (!xdr_short(xdrs, &objp->state)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->set)) {
		return (FALSE);
	}
	if (!xdr_float(xdrs, &objp->read)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevStateFloatReadPoint(objp)
	DevStateFloatReadPoint *objp;
{
	long  length = 0;

	length = length + xdr_length_DevShort (&objp->state);
	length = length + xdr_length_DevFloat (&objp->set);
	length = length + xdr_length_DevFloat (&objp->read);

	return (length);
}



/* D_LONG_READPOINT */

bool_t
xdr_DevLongReadPoint(xdrs, objp)
	XDR *xdrs;
	DevLongReadPoint *objp;
{
	if (!xdr_long(xdrs, &objp->set)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->read)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevLongReadPoint(objp)
	DevLongReadPoint *objp;
{
	long  length = 0;

	length = length + xdr_length_DevLong (&objp->set);
	length = length + xdr_length_DevLong (&objp->read);

	return (length);
}



/* D_DOUBLE_READPOINT */

bool_t
xdr_DevDoubleReadPoint(xdrs, objp)
	XDR *xdrs;
	DevDoubleReadPoint *objp;
{
	if (!xdr_double(xdrs, &objp->set)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->read)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevDoubleReadPoint(objp)
	DevDoubleReadPoint *objp;
{
	long  length = 0;

	length = length + xdr_length_DevDouble (&objp->set);
	length = length + xdr_length_DevDouble (&objp->read);

	return (length);
}



/*
 *  xdr_routines for variable length arrays 
 */

/* D_VAR_CHARARR */

bool_t
xdr_DevVarCharArray(xdrs, objp)
	XDR *xdrs;
	DevVarCharArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarCharArray (objp)
	DevVarCharArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevBoolean(&objp->sequence[0]) );

	return (length);
}



/* D_VAR_STRINGARR */

bool_t
xdr_DevVarStringArray(xdrs, objp)
	XDR *xdrs;
	DevVarStringArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevString), (xdrproc_t)xdr_DevString)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarStringArray (objp)
	DevVarStringArray *objp;
{
	long 	type_length;
	long 	length = 0;
	short	i;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now find the lengthes of all strings in the array
	 *  and add the up.
	 */

	for (i=0; (u_long)i<objp->length; i++ )
	   {
	   if ( (type_length = xdr_length_DevString (&objp->sequence[i])) 
		 == -1 )
		return (-1);

	   length = length + type_length;
	   }

	return (length);
}



/* D_VAR_USHORTARR */

bool_t 
xdr_DevVarUShortArray (xdrs, objp)
     	XDR               *xdrs;
     	DevVarUShortArray *objp;
{
  	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(u_short), (xdrproc_t)xdr_u_short)) {
    		return (FALSE);
	}
  	return (TRUE);
}

long 
xdr_length_DevVarUShortArray (objp)
     	DevVarUShortArray *objp;
{
  	long length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

  	length = length + xdr_length_DevLong ((long *)&objp->length);
	
	/*
	 *  now calculate the length of the array
	 */

  	length = length + 
		 ( objp->length * xdr_length_DevUShort(&objp->sequence[0]) );

  	return (length);
}



/* D_VAR_SHORTARR */

bool_t
xdr_DevVarShortArray(xdrs, objp)
	XDR *xdrs;
	DevVarShortArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(short), (xdrproc_t)xdr_short)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarShortArray (objp)
	DevVarShortArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevShort(&objp->sequence[0]) );

	return (length);
}


/* D_VAR_ULONGARR */

bool_t
xdr_DevVarULongArray(xdrs, objp)
	XDR *xdrs;
	DevVarULongArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(unsigned long), (xdrproc_t)xdr_long)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarULongArray (objp)
	DevVarULongArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevLong((long *)&objp->sequence[0]) );

	return (length);
}


/* D_VAR_LONGARR */

bool_t
xdr_DevVarLongArray(xdrs, objp)
	XDR *xdrs;
	DevVarLongArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarLongArray (objp)
	DevVarLongArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevLong(&objp->sequence[0]) );

	return (length);
}



/* D_VAR_FLOATARR */

bool_t
xdr_DevVarFloatArray(xdrs, objp)
	XDR *xdrs;
	DevVarFloatArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarFloatArray (objp)
	DevVarFloatArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevFloat(&objp->sequence[0]) );

	return (length);
}


/* D_VAR_DOUBLEARR */

bool_t
xdr_DevVarDoubleArray(xdrs, objp)
	XDR *xdrs;
	DevVarDoubleArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(double), (xdrproc_t)xdr_double)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarDoubleArray (objp)
	DevVarDoubleArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + 
		 ( objp->length * xdr_length_DevDouble(&objp->sequence[0]) );

	return (length);
}




/* D_VAR_FRPARR */

bool_t
xdr_DevVarFloatReadPointArray(xdrs, objp)
	XDR *xdrs;
	DevVarFloatReadPointArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevFloatReadPoint), 
	    (xdrproc_t)xdr_DevFloatReadPoint)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarFloatReadPointArray (objp)
	DevVarFloatReadPointArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + (objp->length * 
		 xdr_length_DevFloatReadPoint(&objp->sequence[0]) );

	return (length);
}



/* D_VAR_SFRPARR */

bool_t
xdr_DevVarStateFloatReadPointArray(xdrs, objp)
	XDR *xdrs;
	DevVarStateFloatReadPointArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevStateFloatReadPoint), 
	    (xdrproc_t)xdr_DevStateFloatReadPoint)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarStateFloatReadPointArray (objp)
	DevVarStateFloatReadPointArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + (objp->length * 
		 xdr_length_DevStateFloatReadPoint(&objp->sequence[0]) );

	return (length);
}



/* D_VAR_LRPARR */

bool_t
xdr_DevVarLongReadPointArray(xdrs, objp)
	XDR *xdrs;
	DevVarLongReadPointArray *objp;
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence, 
	    (u_int *)&objp->length, MAXU_INT, sizeof(DevLongReadPoint), 
	    (xdrproc_t)xdr_DevLongReadPoint)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVarLongReadPointArray (objp)
	DevVarLongReadPointArray *objp;
{
	long 	length = 0;

	/*
	 *  four bytes for the number of array elements
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the array
	 */

	length = length + (objp->length * 
		 xdr_length_DevLongReadPoint(&objp->sequence[0]) );

	return (length);
}





/*
 *  Opaque data type to transfer blocks of data
 */

/* D_OPAQUE_TYPE */

bool_t
xdr_DevOpaque(xdrs, objp)
	XDR *xdrs;
	DevOpaque *objp;
{
        if (!xdr_bytes(xdrs, (char **)&objp->sequence, 
		      (u_int *)&objp->length, MAXU_INT)) {
			return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevOpaque (objp)
	DevOpaque *objp;
{
	long 	length = 0;
	short   add_bytes;

	/*
	 *  four bytes for the number of bytes
	 */

	length = length + xdr_length_DevLong ((long *)&objp->length);

	/*
	 *  now calculate the length of the byte array
	 */

	length = length + objp->length;

	/* only packets of four bytes can be send by XDR
	 * Calculate the next multiple of four. */


	if ( (add_bytes = (STD_XDR_LENGTH - 
			  (length - (STD_XDR_LENGTH * 
			  (int)(length / STD_XDR_LENGTH))))) 
			  == STD_XDR_LENGTH)
	   {
	   add_bytes = 0;
	   }
	length = length + add_bytes;

	return (length);
}

