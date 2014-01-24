/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Description: interface python - taco
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
 * File:        taco_types.c
 *
 * Description: interface python - taco
 *
 * Author(s):   MCD
 *		$Author: jkrueger1 $
 *
 * Original:    March 99
 * 
 * Date:	$Date: 2008-10-15 14:52:49 $
 *
 * Version:	$Revision: 1.10 $
 */
 
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef HAVE_FSTAT
#undef HAVE_FSTAT
#endif

#include <Python.h>

#if PY_MAJOR_VERSION < 3
#	define PYINT_FROMLONG		PyInt_FromLong
#	define PYINT_ASLONG		PyInt_AsLong
#	define PYINT_CHECK		PyInt_Check
#	define PYSTRING_ASSTRING	PyString_AsString
#	define PYSTRING_FROMSTRING	PyString_FromString
#	define PYSTRING_FROMSTRINGANDSIZE	PyString_FromStringAndSize
#	define PYSTRING_CHECK		PyString_Check
#else
#	define PYINT_FROMLONG		PyLong_FromLong
#	define PYINT_ASLONG		PyLong_AsLong
#	define PYINT_CHECK		PyLong_Check
#	define PYSTRING_ASSTRING	PyBytes_AsString
#	define PYSTRING_FROMSTRING	PyBytes_FromString
#	define PYSTRING_FROMSTRINGANDSIZE	PyBytes_FromStringAndSize
#	define PYSTRING_CHECK		PyBytes_Check
#endif

#include <ctype.h>

/* for taco */
#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <atte_xdr.h>
#include <grp_xdr.h>
#include <maxe_xdr.h>

#ifdef NUMPY
#	if HAVE_NUMERIC_ARRAYOBJECT_H
#		include <Numeric/arrayobject.h>
#	elif HAVE_NUMARRAY_ARRAYOBJECT_H
#		include <numarray/arrayobject.h>
#	elif HAVE_NUMPY_ARRAYOBJECT_H
#		include <numpy/oldnumeric.h>
#		include <numpy/arrayobject.h>
#	endif
#endif 

#define _taco_py

long get_argin_single(DevArgument ds_argin, long ds_in, 
                      PyObject *item, char* mymess);
long get_argin_array(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess);
long get_argin_special(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess);
long get_argout_single(DevArgument ds_argout, long ds_out, 
                      PyObject **item, char* mymess);
long get_argout_array(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess);
long get_argout_special(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess);
long check_type(long ds_ty, long *is_a_single, 
                long *is_an_array, long *is_a_special);
void display_single(DevArgument ds_argin,long ds_in,char * ms);
void display_array(DevArgument ds_argin,long ds_in, char* ms);
void display_special(DevArgument ds_argin,long ds_in,char * ms);
long get_dmulmove_longfloatarr(DevArgument ds_arg,long ds_type,
      PyObject * mytuple, long len, char* mymess);
char p2c_BOOLEAN(PyObject *item,long *err);     
unsigned short p2c_USHORT(PyObject *item,long *err);     
short p2c_SHORT(PyObject *item,long *err);     
unsigned long p2c_ULONG(PyObject *item,long *err);     
long p2c_LONG(PyObject *item,long *err);     
float p2c_FLOAT(PyObject *item,long *err);     
double p2c_DOUBLE(PyObject *item,long *err);     
DevString p2c_STRING(PyObject *item,long *err);     
long Ctype2numeric(long ds_out);
long lenoftype(long ds_out);     


/**
 * prints out the "single" value according to the type
 *
 * @param ds_argin  structure containing the value
 * @param ds_in data type
 * @param ms IN or OUT (for appropriate display)
 */
void display_single(DevArgument ds_argin,long ds_in,char * ms)
{
   long i;

   switch (ds_in)
   {
      case D_BOOLEAN_TYPE:
          printf("-- esrf_io: %s: boolean: %d\n",ms,*(char *)(ds_argin));
	  break;
      case D_USHORT_TYPE:
          printf("-- esrf_io: %s: ushort: %d\n",ms,*(unsigned short *)(ds_argin));
	  break;
      case D_SHORT_TYPE:
          printf("-- esrf_io: %s: short: %d\n",ms,*(short *)(ds_argin));
	  break;
      case D_ULONG_TYPE:
          printf("-- esrf_io: %s: ulong: %d\n",ms,*(unsigned long *)(ds_argin));
	  break;
      case D_LONG_TYPE:
          printf("-- esrf_io: %s: long: %d\n",ms,*(long*)(ds_argin));
	  break;
      case D_FLOAT_TYPE:
          printf("-- esrf_io: %s: float: %f\n",ms,*(float *)(ds_argin));
	  break;
     case D_DOUBLE_TYPE:
          printf("-- esrf_io: %s: double: %f\n",ms,*(double *)(ds_argin));
	  break;
     case D_STRING_TYPE:
          printf("-- esrf_io: %s: string: %s\n",ms,*(DevString*)(ds_argin));
	  break;
	  
     case D_OPAQUE_TYPE:
          printf("-- esrf_io: %s: opaque type diplayed as char array: ",ms);
	  		 for  (i=0; i< ((DevVarCharArray *)(ds_argin))->length; i++)
	     		 printf("%d ",((DevVarCharArray *)(ds_argin))->sequence[i]);
	  		 printf("\n");
	  break;
	  	  
     default:
          printf("display_single: %s unknown type %d\n",ms,ds_in);
   }
}

/**
 * Retrieves a BOOLEAN from python object:
 *
 * @param item python object
 * @param err  error code 0: no error, 1: error
 *
 * @return C value
 */
char p2c_BOOLEAN(PyObject *item,long *err)
{
   *err = 0;
   if (PYINT_CHECK(item))
      return (char)PYINT_ASLONG(item);
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves an unsigned short from python object
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @return C value
 */
unsigned short p2c_USHORT(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
       return (unsigned short)PyLong_AsLong(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
       return (unsigned short)PyInt_AsLong(item);
#endif
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves a SHORT from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @return C value
 */
short p2c_SHORT(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
      return (short)PyLong_AsLong(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
      return (short)PyInt_AsLong(item);
#endif
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves an unsigned long from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @return C value
 */
unsigned long p2c_ULONG(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
      return (unsigned long)PyLong_AsLong(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
      return (unsigned long)PyInt_AsLong(item);
#endif
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves a LONG from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @returns C value
 */
long p2c_LONG(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
      return (long)PyLong_AsLong(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
      return (long)PyInt_AsLong(item);
#endif
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves a FLOAT from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @returns C value
 */
float p2c_FLOAT(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
      return (float)PyLong_AsDouble(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
      return (float)PyInt_AsLong(item);
#endif
   else if (PyFloat_Check(item))
      return (float)PyFloat_AsDouble(item);
   else
   {
      *err = 1;
      return(0);
   }
}


/**
 * Retrieves a DOUBLE from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @returns C value
 */
double p2c_DOUBLE(PyObject *item,long *err)
{
   *err = 0;
   if (PyLong_Check(item))
      return (double)PyLong_AsDouble(item);
#if PY_MAJOR_VERSION < 3
   else if (PyInt_Check(item))
      return (double)PyInt_AsLong(item);
#endif
   else if (PyFloat_Check(item))
      return (double)PyFloat_AsDouble(item);
   else
   {
      *err = 1;
      return(0);
   }
}

/**
 * Retrieves a STRING from python object:
 *
 * @param item python object
 * @param err  error code 0: no error 1: error
 *
 * @returns C value
 */
char* p2c_STRING(PyObject *item,long *err)
{
   *err = 0;
   if (PYSTRING_CHECK(item))
	return PYSTRING_ASSTRING(item);
   else
   {
      *err = 1;
      return("0");
   }
}

/**
 * retrieve from python object, the "single" value
 *
 * @param ds_argin structure to fill with the python item
 * @param ds_in data type 
 * @param item python object
 * @param mymess: to fill in case of error (allocation made by caller !!)
 *
 * @return  0:  no error -1: error
 */
long get_argin_single(DevArgument ds_argin, long ds_in, PyObject *item, char* mymess)
{
   int len;
   long merr;
	char *str_buf;
   
   if (ds_in != D_STRING_TYPE && ds_in != D_OPAQUE_TYPE)
   {
      /* check that item is a number */    
      if ( PyNumber_Check(item) == 0)
      {
         printf("item should be a number\n");
	 strcpy(mymess,"item should be a number\n");
	 return(-1);
      }
   }   
   
   switch (ds_in) 
   {
      case D_BOOLEAN_TYPE:
         *(char *)(ds_argin) = p2c_BOOLEAN(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C char type\n");
	    return(-1);	 
	 }
	 break;
      case D_USHORT_TYPE:
         *(unsigned short *)(ds_argin) = p2c_USHORT(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C unsigned short type\n");
	    return(-1);
	 }
	 break;
	 
      case D_SHORT_TYPE:
	 *(short *)(ds_argin) = p2c_SHORT(item,&merr);
	 if (merr != 0)
	 {
	     strcpy(mymess,"argin cannot be cast as C short type\n");
	     return(-1);
	 }		 
	 break;
	 
      case D_ULONG_TYPE:
	 *(unsigned long *)(ds_argin) = p2c_ULONG(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C unsigned long type\n");
	    return(-1);
	 }               
	 break;
	 
      case D_LONG_TYPE:
	 *(long *)(ds_argin) = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C long type\n");
            return(-1);
	 }
	 break;
	 
      case D_FLOAT_TYPE:
	 *(float *)(ds_argin) = p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C float type\n");
	    return(-1);
	 }
	 break;
	 
      case D_DOUBLE_TYPE:
         *(double *)(ds_argin) = p2c_DOUBLE(item,&merr);
	 if (merr != 0)
	 {
	    strcpy(mymess,"argin cannot be cast as C double type\n");
	    return(-1);
	 }
         break;

      case D_STRING_TYPE:
	 *(DevString *)(ds_argin) = p2c_STRING(item,&merr);
	 if (merr != 0)
         {
	    strcpy(mymess,"argin cannot be cast as C string\n");
	    return(-1);
	 }
         break;

    case D_OPAQUE_TYPE:
	 	/*
		   * OPAQUE type in python is handled as string
			*/
    	str_buf =  p2c_STRING(item,&merr);    
	 	if (merr != 0)
    		{
	   	strcpy(mymess,"argin cannot be cast as opaque type\n");
	   	return(-1);
	 		}
		
		((DevVarCharArray *)(ds_argin))->length = strlen (str_buf);
		((DevVarCharArray *)(ds_argin))->sequence = str_buf;
    	break;
	 	 
      default:
         sprintf(mymess,"get_argin_single: unknown type %d\n",ds_in);
	 return(-1);
   }

   return(0);
}

/**
 * print out the "array" value according to the type
 * 	  
 * @param ds_argin  structure containing the data
 * @param ds_in data type
 * @param ms IN or OUT for display
 */
void display_array(DevArgument ds_argin,long ds_in, char* ms)
{
   long i;
   
   switch (ds_in)
   {
      case D_VAR_CHARARR:
          printf("-- esrf_io: %s: char array: ",ms);
	  for  (i=0; i< ((DevVarCharArray *)(ds_argin))->length; i++)
	     printf("%d ",((DevVarCharArray *)(ds_argin))->sequence[i]);
	  printf("\n");
	  break;
      case D_VAR_USHORTARR:
          printf("-- esrf_io: %s: u short array: ",ms);
	  for  (i=0; i< ((DevVarUShortArray *)(ds_argin))->length; i++)
	     printf("%d ",((DevVarUShortArray *)(ds_argin))->sequence[i]);
	  printf("\n");
	  break;
      case D_VAR_SHORTARR:
          printf("-- esrf_io: %s: short array: ",ms);
	  for  (i=0; i< ((DevVarShortArray *)(ds_argin))->length; i++)
	     printf("%d ",((DevVarShortArray *)(ds_argin))->sequence[i]);
	  printf("\n");	  
	  break;
      case D_VAR_ULONGARR:
          printf("-- esrf_io: %s: u long array : ",ms);
	  for  (i=0; i< ((DevVarULongArray *)(ds_argin))->length; i++)
	     printf("%d ",((DevVarULongArray *)(ds_argin))->sequence[i]);
	  printf("\n");	  
	  break;
      case D_VAR_LONGARR:
          printf("-- esrf_io: %s: long array: ",ms);
	  for  (i=0; i< ((DevVarLongArray *)(ds_argin))->length; i++)
	     printf("%d ",((DevVarLongArray *)(ds_argin))->sequence[i]);
	  printf("\n");	  
	  break;
      case D_VAR_FLOATARR:
          printf("-- esrf_io: %s: float array: ",ms);
	  for  (i=0; i< ((DevVarFloatArray *)(ds_argin))->length; i++)
	     printf("%f ",((DevVarFloatArray *)(ds_argin))->sequence[i]);
	  printf("\n");
	  break;
     case D_VAR_DOUBLEARR:
          printf("-- esrf_io: %s: double array: ",ms);
	  for  (i=0; i< ((DevVarDoubleArray *)(ds_argin))->length; i++)
	     printf("%f ",((DevVarDoubleArray *)(ds_argin))->sequence[i]);
	  printf("\n");
	  break;
     case D_VAR_STRINGARR:
          printf("-- esrf_io: %s: string array: ",ms);
	  for  (i=0; i< ((DevVarStringArray *)(ds_argin))->length; i++)
	     printf("%s ",((DevVarStringArray *)(ds_argin))->sequence[i]);
	  printf("\n");
	  break;
     case D_VAR_FRPARR:
     case D_VAR_SFRPARR:
     case D_VAR_LRPARR:
          break;
     default:
          printf("display_array: unknown type %d\n",ds_in);
   }
}


/**
 * retrieve from python object, the "array" value
 *  
 * @param ds_argin structure to fill with the python item
 * @param ds_in data type 
 * @param mytuple the python tuple
 * @param mymess to fill in case of error (allocation made by caller !!)
 * 
 * @return  0: no error -1: error
 */
long get_argin_array(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess)
{
   int len,i;
   long ret,merr;
   PyObject *item;

   len = PySequence_Length(mytuple);

   switch (ds_in) 
   {
      case D_VAR_CHARARR:
         ((DevVarCharArray *)(ds_argin))->length = len;
         ((DevVarCharArray *)(ds_argin))->sequence = 
	      (char *) malloc(sizeof(char)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarCharArray*)(ds_argin))->sequence[i]= p2c_BOOLEAN(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a char type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }
	 break;
	 
      case D_VAR_USHORTARR:
         ((DevVarUShortArray *)(ds_argin))->length = len;
         ((DevVarUShortArray *)(ds_argin))->sequence = 
	      (unsigned short *) malloc(sizeof(unsigned short)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarUShortArray*)(ds_argin))->sequence[i]= p2c_USHORT(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not an ushort type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }
	 break;
	 
      case D_VAR_SHORTARR:
         ((DevVarShortArray *)(ds_argin))->length = len;
         ((DevVarShortArray *)(ds_argin))->sequence = 
	      (short *) malloc(sizeof(short)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarShortArray*)(ds_argin))->sequence[i]= p2c_SHORT(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not an short type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }      		 
	 break;
	 
      case D_VAR_ULONGARR:
         ((DevVarULongArray *)(ds_argin))->length = len;
         ((DevVarULongArray *)(ds_argin))->sequence = (DevULong *) malloc(sizeof(DevULong)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarULongArray*)(ds_argin))->sequence[i]= p2c_ULONG(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not an unsigned long type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }                     
	 break;
	 
      case D_VAR_LONGARR:
         ((DevVarLongArray *)(ds_argin))->length = len;
         ((DevVarLongArray *)(ds_argin))->sequence = (DevLong *) malloc(sizeof(DevLong)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarLongArray*)(ds_argin))->sequence[i]= p2c_LONG(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a long type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }       
	 break;
	 
      case D_VAR_FLOATARR:
         ((DevVarFloatArray *)(ds_argin))->length = len;
         ((DevVarFloatArray *)(ds_argin))->sequence = 
	      (float *) malloc(sizeof(float)*len);
	 
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarFloatArray*)(ds_argin))->sequence[i]= p2c_FLOAT(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a float type",i);
	       return(-1);
	    }
	    /*
	    Py_XDECREF (item);
	    */
	 }      
	 break;
	 
      case D_VAR_DOUBLEARR:
         ((DevVarDoubleArray *)(ds_argin))->length = len;
         ((DevVarDoubleArray *)(ds_argin))->sequence = 
	      (double *) malloc(sizeof(double)*len);

	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarDoubleArray*)(ds_argin))->sequence[i]= p2c_DOUBLE(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a double type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 } 
         break;

      case D_VAR_STRINGARR:	 
         ((DevVarStringArray *)(ds_argin))->length = len;
         ((DevVarStringArray *)(ds_argin))->sequence = 
	      (DevString *) malloc(sizeof(DevString)*len);
	 for (i=0; i<len; i++)
	    ((DevVarStringArray *)(ds_argin))->sequence[i]=NULL;
	 for (i=0; i<len; i++)
	 {
	    item = PySequence_GetItem(mytuple,i);
            ((DevVarStringArray *)(ds_argin))->sequence[i]= p2c_STRING(item,&merr);
	    if (merr != 0)
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a string type",i);
	       return(-1);
	    }
	    Py_XDECREF (item);	    
	 }
         break;
	 
      case D_VAR_FRPARR:
         ((DevVarFloatReadPointArray *)(ds_argin))->length = len;
         ((DevVarFloatReadPointArray *)(ds_argin))->sequence = 
	      (DevFloatReadPoint *) malloc(sizeof(DevFloatReadPoint)*len); 
	      
	 for (i=0; i<len; i++)
	 {
	    if ( ret = get_argin_special( (DevArgument)&(((DevVarFloatReadPointArray*)(ds_argin))->sequence[i]),
	            D_FLOAT_READPOINT,item,mymess))
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a float-read-point type",i);
	       return(-1);
	    }
	 }
	 break;
	 
      case D_VAR_SFRPARR:
         ((DevVarStateFloatReadPointArray *)(ds_argin))->length = len;
         ((DevVarStateFloatReadPointArray *)(ds_argin))->sequence = 
	      (DevStateFloatReadPoint *) malloc(sizeof(DevStateFloatReadPoint)*len); 
	      
	 for (i=0; i<len; i++)
	 {
	    if ( ret = get_argin_special((DevArgument)&(((DevVarStateFloatReadPointArray *)(ds_argin))->sequence[i]),
	            D_STATE_FLOAT_READPOINT,item,mymess))
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a state-float-read-point type",i);
	       return(-1);
	    }
	 }
	 break;
	 
      case D_VAR_LRPARR:
         ((DevVarLongReadPointArray *)(ds_argin))->length = len;
         ((DevVarLongReadPointArray *)(ds_argin))->sequence = 
	      (DevLongReadPoint *) malloc(sizeof(DevLongReadPoint)*len); 
	      
	 for (i=0; i<len; i++)
	 {
	    if ( ret = get_argin_special((DevArgument)&(((DevVarLongReadPointArray *)(ds_argin))->sequence[i]),
	            D_LONG_READPOINT,item,mymess))
	    {
	       sprintf(mymess,"get_argin_array : input item %d is not a long-read-point type",i);
	       return(-1);
	    }
	 }
	 break;

      default:
         sprintf(mymess,"get_argin_array: unknown type %d\n",ds_in);
	 return(-1);
   }

   return(0);
}

/**
 * reads an tuple from python and stores it in ds_arg, according to the type
 *	
 * @param ds_arg long* or float* to store the tuple
 * @param ds_type D_LONG_TYPE or D_FLOAT_TYPE
 * @param mytuple the tuple of 8 elements
 * @param mymess string with the message in case of error
 * 	
 * @return 0 OK -1 pb
 */
long get_dmulmove_longfloatarr(DevArgument ds_arg,long ds_type,
      PyObject * mytuple, long len, char* mymess)
{
   long i,merr;
   PyObject *item;

   if (ds_type == D_LONG_TYPE) 
   {
      for (i=0; i<len; i++)
      {
	 item = PySequence_GetItem(mytuple,i);
         ((long *)ds_arg)[i]= p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input item %d is not a long type",i);
	    return(-1);
	 }
	 Py_XDECREF (item);
      }      
   }
   else
   {	 
      for (i=0; i<len; i++)
      {
	 item = PySequence_GetItem(mytuple,i);
         ((float *)ds_arg)[i] = p2c_FLOAT(item,&merr);		    
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_array : input item %d is not a float type",i);
	    return(-1);
	 }
	 Py_XDECREF (item);
      }
   }
   return(0);
}

/**
 * print out the "special" value according to the type
 *
 * @param ds_argin structure containing the value
 * @param ds_in data type
 * @param ms IN or OUT (for appropriate display)
 * 
 * @return nothing
 */
void display_special(DevArgument ds_argin,long ds_in,char * ms)
{
   long i;
   
   switch (ds_in)
   {
      case D_INT_FLOAT_TYPE:
          printf("-- esrf_io: %s: INT_FLOAT: (%d,%f)\n",ms,
	          ((DevIntFloat *)(ds_argin))->state, 
		  ((DevIntFloat *)(ds_argin))->value);
	  break;
      case D_FLOAT_READPOINT:
          printf("-- esrf_io: %s: FLOAT_READPOINT: (%f,%f)\n",ms,
	          ((DevFloatReadPoint *)(ds_argin))->set, 
		  ((DevFloatReadPoint *)(ds_argin))->read);
	  break;
      case D_DOUBLE_READPOINT:
          printf("-- esrf_io: %s: DOUBLE_READPOINT: (%f,%f)\n",ms,
	          ((DevDoubleReadPoint *)(ds_argin))->set, 
		  ((DevDoubleReadPoint *)(ds_argin))->read);
	  break;
      case D_LONG_READPOINT:
          printf("-- esrf_io: %s: LONG_READPOINT: (%d,%d)\n",ms,
	          ((DevLongReadPoint *)(ds_argin))->set, 
		  ((DevLongReadPoint *)(ds_argin))->read);
	  break;
      case D_MOTOR_LONG:
          printf("-- esrf_io: %s: MOTOR_LONG: (%d,%d)\n",ms,
	          ((DevMotorLong *)(ds_argin))->axisnum, 
		  ((DevMotorLong *)(ds_argin))->value);
	  break;
      case D_MOTOR_FLOAT:
          printf("-- esrf_io: %s: MOTOR_FLOAT: (%d,%f)\n",ms,
	          ((DevMotorFloat *)(ds_argin))->axisnum, 
		  ((DevMotorFloat *)(ds_argin))->value);
	  break;
      case D_STATE_FLOAT_READPOINT:
          printf("-- esrf_io: %s: STATE FLOAT READ POINT: (%d %f,%f)\n",ms,
	          ((DevStateFloatReadPoint *)(ds_argin))->state, 
		  ((DevStateFloatReadPoint *)(ds_argin))->set,
		  ((DevStateFloatReadPoint *)(ds_argin))->read);
	  break;
      case D_MULMOVE_TYPE:
          printf("-- esrf_io: %s: MULMOVE: \n",ms);
	  printf("( ");
	  for (i=0; i<8; i++)
	     printf("%d ",((DevMulMove *)(ds_argin))->action[i]);
	  printf(")\n");
	  printf("( ");
	  for (i=0; i<8; i++)
	     printf("%f ",((DevMulMove *)(ds_argin))->position[i]);
	  printf(")\n");	  
	  printf("( ");
	  for (i=0; i<8; i++)
	     printf("%d ",((DevMulMove *)(ds_argin))->delay[i]);
	  printf(")\n");	   

	  break; 	  	  	  
     default:
          printf("display_special: %s unknown type %d\n",ms,ds_in);
   }
}

/**
 * retrieve from python object, the "special" structure
 * 
 * @param - ds_argin: structure to fill with the python item
 * @param - ds_in: data type 
 * @param - mytuple: the python tuple
 * @param - mymess: to fill in case of error (allocation made by caller !!
 *
 * @return  0 no error -1 error
 */
long get_argin_special(DevArgument ds_argin, long ds_in, 
                      PyObject *mytuple, char* mymess)
{
   int len,i;
   long merr;
   PyObject *item;
   long ret;

   len = PySequence_Length(mytuple);

   switch (ds_in) 
   {
      case D_INT_FLOAT_TYPE:		/*(long state, float value)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (long,float)");
	    return(-1);	 
	 }
	 /* state */
	 item = PySequence_GetItem(mytuple,0);
         ((DevIntFloat*)(ds_argin))->state = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,float): first item not long");
	    return(-1);
	 }
	 Py_XDECREF (item);	    

	 /* value */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevIntFloat*)(ds_argin))->value= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,float): second item  not float");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;

      case D_FLOAT_READPOINT:		/*(float set, float read)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (float,float)");
	    return(-1);	 
	 }
	 /* set */
	 item = PySequence_GetItem(mytuple,0);
         ((DevFloatReadPoint*)(ds_argin))->set= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (float,float): first item not float");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* read */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevFloatReadPoint*)(ds_argin))->read= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (float,float): second item not float");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;

      case D_LONG_READPOINT:		/*(long set, long read)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (long,long)");
	    return(-1);	 
	 }
	 /* set */
	 item = PySequence_GetItem(mytuple,0);
         ((DevLongReadPoint*)(ds_argin))->set = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,long): first item not long");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* read */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevLongReadPoint*)(ds_argin))->read = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,long): second item not long");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;
	 
      case D_DOUBLE_READPOINT:		/*(double set, double read)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (double,double)");
	    return(-1);	 
	 }
	 /* set */
	 item = PySequence_GetItem(mytuple,0);
         ((DevDoubleReadPoint*)(ds_argin))->set= p2c_DOUBLE(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (double,double): first item not double");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* read */	 
	 item = PySequence_GetItem(mytuple,1);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (double,double): second item not double");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;
	 
      case D_MOTOR_LONG:		/*(long axisnum, long value)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (long,long)");
	    return(-1);	 
	 }
	 /* axisnum */
	 item = PySequence_GetItem(mytuple,0);
	 ((DevMotorLong*)(ds_argin))->axisnum = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,long): first item not long");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* value */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevMotorLong*)(ds_argin))->value = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,long): second item not long");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;	

      case D_MOTOR_FLOAT:		/*(long axisnum, float value)*/
         if (len != 2)
	 {
	    sprintf(mymess,"get_argin_special : input should be (long,float)");
	    return(-1);	 
	 }
	 /* axisnum */
	 item = PySequence_GetItem(mytuple,0);
         ((DevMotorFloat*)(ds_argin))->axisnum = p2c_LONG(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,float): first item 1 not long");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* value */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevMotorFloat*)(ds_argin))->value= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (long,float): second item not float");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 break;
	 
      case D_STATE_FLOAT_READPOINT:		/*(short state, float set,float read)*/
         if (len != 3)
	 {
	    sprintf(mymess,"get_argin_special : input should be (short,float,float)");
	    return(-1);	 
	 }
	 /* state */
	 item = PySequence_GetItem(mytuple,0);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (short,float,float): first item not short");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* set */	 
	 item = PySequence_GetItem(mytuple,1);
         ((DevStateFloatReadPoint*)(ds_argin))->set= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (short,float,float): second item not float");
	    return(-1);
	 }
	 Py_XDECREF (item);
	 
	 /* read */	 
	 item = PySequence_GetItem(mytuple,2);
         ((DevStateFloatReadPoint*)(ds_argin))->read= p2c_FLOAT(item,&merr);
	 if (merr != 0)
	 {
	    sprintf(mymess,"get_argin_special : input (short,float,float): third item not float");
	    return(-1);
	 }
	 Py_XDECREF (item);	 
	 break;

      case D_MULMOVE_TYPE:		/*(long action[8],long delay[8],float position[8])*/
         if (len != 3)
	 {
	    sprintf(mymess,"get_argin_special : input should be (long[8],long[8],float[8])");
	    return(-1);	 
	 }
	 for (i=0; i<3; i++)
	 {
	    item = PySequence_GetItem(mytuple,i); /* this should be a tuple */
	    if (PySequence_Check(item) != 1)
	    {
	       Py_XDECREF (item);
	       sprintf(mymess,"get_argin_special: input (long[8],long[8],float[8]: item %d not correct\n",i);
               return(-1);
	    }
	    if ( (len = PySequence_Length(item)) != 8 )
	    {
	       sprintf(mymess,"get_argin_special: input (long[8],long[8],float[8]: item %d with size != 8\n",i);
               return(-1);
	    }
	    if (i == 0)
	    {
	       if ( (ret = get_dmulmove_longfloatarr((DevArgument)(((DevMulMove*)ds_argin)->action),
	           D_LONG_TYPE, item, len, mymess)) != 0 )
	          return(-1);
	    }	    
	    else if (i == 1)
	    {
	       if ( (ret = get_dmulmove_longfloatarr((DevArgument)(((DevMulMove*)ds_argin)->position),
	           D_FLOAT_TYPE, item, len, mymess)) != 0 )

	          return(-1);
	    }	    
	    else
	    {
	       if ( (ret = get_dmulmove_longfloatarr((DevArgument)(((DevMulMove*)ds_argin)->delay),
	           D_LONG_TYPE, item, len, mymess)) != 0 )
	          return(-1);
	    }
	    Py_XDECREF (item);	    
	 }
	 break;
	 

      default:
         sprintf(mymess,"get_argin_special: unknown type %d\n",ds_in);
	 return(-1);
   }

   return(0);
}



/**
 * build from C argout object, the python value
 *
 * @param ds_argout C data
 * @param ds_out data type 
 * @param item python object
 * @param mymess to fill in case of error (allocation made by caller !!)
 *
 * @return  0 no error -1 error
 */
long get_argout_single(DevArgument ds_argout, long ds_out, 
                      PyObject **item, char* mymess)
{
   int len;

   switch (ds_out) 
   {
      case D_BOOLEAN_TYPE:
         *item = PYINT_FROMLONG((long) *((char *)(ds_argout)));
	 break;
	 
      case D_USHORT_TYPE:
         *item = PYINT_FROMLONG((long) (*((unsigned short *)(ds_argout))));
	 break;
	 
      case D_SHORT_TYPE:
         *item = PYINT_FROMLONG((long) (*((short *)(ds_argout))));
	 break;
	 
      case D_ULONG_TYPE:               
         *item = PyLong_FromLong((long) (*((unsigned long *)(ds_argout))));
	 break;
	 
      case D_LONG_TYPE:
         *item = PyLong_FromLong((long) (*((long *)(ds_argout))));
	 break;
	 
      case D_FLOAT_TYPE:
         *item = PyFloat_FromDouble((double) (*((float *)(ds_argout))));
	 break;
	 
      case D_DOUBLE_TYPE:
         *item = PyFloat_FromDouble((double) (*((double *)(ds_argout))));
         break;

      case D_STRING_TYPE:
         *item = PYSTRING_FROMSTRING((DevString) (*((DevString *)(ds_argout))));
         break;
	
    	case D_OPAQUE_TYPE:
         len = ((DevVarCharArray *)(ds_argout))->length;
         *item = PYSTRING_FROMSTRINGANDSIZE( (DevString) (((DevVarCharArray *)(ds_argout))->sequence), len);
         break;         
	 
      default:
         sprintf(mymess,"get_argout_single: unknown type %d\n",ds_out);
	 return(-1);
   }

   return(0);
}

/**
 * build from C structure, the python list
 *
 * @param ds_argout C structure 
 * @param ds_out data type 
 * @param mylist the python list
 * @param mymess to fill in case of error (allocation made by caller !!)
 *
 * @return 0: no error -1: error
 */
long get_argout_array(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess)
{
   int len,i;
   PyObject *item,*sublist;
   long ret;

   switch (ds_out) 
   {
      case D_VAR_CHARARR:
         len = ((DevVarCharArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PYINT_FROMLONG((long) ((((DevVarCharArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }
	 break;
	 
      case D_VAR_USHORTARR:
         len = ((DevVarUShortArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PYINT_FROMLONG((long) ((((DevVarUShortArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }      
	 break;
	 
      case D_VAR_SHORTARR:
         len = ((DevVarShortArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PYINT_FROMLONG((long) ((((DevVarShortArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }          	 
	 break;
	 
      case D_VAR_ULONGARR:
         len = ((DevVarULongArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PyLong_FromLong((long) ((((DevVarULongArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }                
	 break;
	 
      case D_VAR_LONGARR:
         len = ((DevVarLongArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PyLong_FromLong((long) ((((DevVarLongArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }       
	 break;
	 
      case D_VAR_FLOATARR:
         len = ((DevVarFloatArray *)(ds_argout))->length;

	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PyFloat_FromDouble((double) ((((DevVarFloatArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item);
	 } 
	 break;
	 
      case D_VAR_DOUBLEARR:
         len = ((DevVarDoubleArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PyFloat_FromDouble((double) ((((DevVarDoubleArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }   
         break;

      case D_VAR_STRINGARR:
         len = ((DevVarStringArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 for (i=0; i<len; i++)
	 {
            item = PYSTRING_FROMSTRING((char *) ((((DevVarStringArray *)(ds_argout))->sequence)[i]));
	    PyList_SetItem(*mylist,i,item); 
	 }           
	 break;

         case D_VAR_FRPARR:
         len = ((DevVarFloatReadPointArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 
	 for (i=0; i<len; i++)
	 {
	    ret = get_argout_special((DevArgument)&(((DevVarFloatReadPointArray*)(ds_argout))->sequence[i]),
	            D_FLOAT_READPOINT,&sublist,mymess);
            if (ret == 0)
	       PyList_SetItem(*mylist,i,sublist);
	    else
	       return(-1);	    
	 }
	 break;	 

      case D_VAR_SFRPARR:
         len = ((DevVarStateFloatReadPointArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 
	 for (i=0; i<len; i++)
	 {
	    ret = get_argout_special((DevArgument)&(((DevVarStateFloatReadPointArray*)(ds_argout))->sequence[i]),
	            D_STATE_FLOAT_READPOINT,&sublist,mymess);
            if (ret == 0)
	       PyList_SetItem(*mylist,i,sublist);
	    else
	       return(-1);	    
	 }
      break;	

      case D_VAR_LRPARR:
         len = ((DevVarLongReadPointArray *)(ds_argout))->length;
	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_array: cannot build list");
	    return(-1);
	 }
	 
	 for (i=0; i<len; i++)
	 {
	    ret = get_argout_special((DevArgument)&(((DevVarLongReadPointArray*)(ds_argout))->sequence[i]),
	            D_LONG_READPOINT,&sublist,mymess);
            if (ret == 0)
	       PyList_SetItem(*mylist,i,sublist);
	    else
	       return(-1);	    
	 }
      break;		 
	  
      default:
         sprintf(mymess,"get_argout_array: unknown type %d\n",ds_out);
	 return(-1);
   }

   return(0);
}


/**
 * build from C structure, the python list
 *
 * @param ds_argout C structure 
 * @param ds_out data type 
 * @param mylist the python list
 * @param mymess to fill in case of error (allocation made by caller !!)
 * 
 * @return 0: no error -1: error
 */
long get_argout_special(DevArgument ds_argout, long ds_out, 
                      PyObject **mylist, char* mymess)
{
   int len,i;
   PyObject *item;

   switch (ds_out) 
   {
      case D_INT_FLOAT_TYPE:	 
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PYINT_FROMLONG((long) ((((DevIntFloat *)(ds_argout))->state)));
	 PyList_SetItem(*mylist,i,item); 
         item = PyFloat_FromDouble((double) ((((DevIntFloat *)(ds_argout))->value)));
	 PyList_SetItem(*mylist,i,item); 
	 break;
	 
      case D_FLOAT_READPOINT:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PyFloat_FromDouble((double) ((((DevFloatReadPoint *)(ds_argout))->set)));
	 PyList_SetItem(*mylist,i,item); 
         item = PyFloat_FromDouble((double) ((((DevFloatReadPoint *)(ds_argout))->read)));
	 PyList_SetItem(*mylist,i,item);      
	 break;
	 
      case D_LONG_READPOINT:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PYINT_FROMLONG((long) ((((DevLongReadPoint *)(ds_argout))->set)));
	 PyList_SetItem(*mylist,i,item); 
         item = PYINT_FROMLONG((long) ((((DevLongReadPoint *)(ds_argout))->read)));
	 PyList_SetItem(*mylist,i,item);         	 
	 break;
	 
      case D_DOUBLE_READPOINT:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PyFloat_FromDouble((double) ((((DevDoubleReadPoint *)(ds_argout))->set)));
	 PyList_SetItem(*mylist,i,item); 
         item = PyFloat_FromDouble((double) ((((DevDoubleReadPoint *)(ds_argout))->read)));
	 PyList_SetItem(*mylist,i,item);                   
	 break;
	 
      case D_MOTOR_LONG:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PYINT_FROMLONG((long) ((((DevMotorLong *)(ds_argout))->axisnum)));
	 PyList_SetItem(*mylist,i,item); 
         item = PYINT_FROMLONG((long) ((((DevMotorLong *)(ds_argout))->value)));
	 PyList_SetItem(*mylist,i,item);         	 
	 break;
	 
      case D_MOTOR_FLOAT:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PyFloat_FromDouble((double) ((((DevMotorFloat*)(ds_argout))->axisnum)));
	 PyList_SetItem(*mylist,i,item); 
         item = PyFloat_FromDouble((double) ((((DevMotorFloat *)(ds_argout))->value)));
	 PyList_SetItem(*mylist,i,item);      
	 break;

      case D_STATE_FLOAT_READPOINT:
	 if ( (*mylist = PyList_New(len)) == NULL)
	 {
	    sprintf(mymess,"get_argout_special: cannot build list");
	    return(-1);
	 }
         item = PYINT_FROMLONG((long) ((((DevStateFloatReadPoint*)(ds_argout))->state)));
	 PyList_SetItem(*mylist,i,item); 
         item = PyFloat_FromDouble((double) ((((DevStateFloatReadPoint *)(ds_argout))->set)));
	 PyList_SetItem(*mylist,i,item);      
         item = PyFloat_FromDouble((double) ((((DevStateFloatReadPoint *)(ds_argout))->read)));
	 PyList_SetItem(*mylist,i,item);
	 break;	 
	 
      default:
         sprintf(mymess,"get_argout_special: unknown type %d\n",ds_out);
	 return(-1);
   }

   return(0);
}

/**
 * sets some useful variables accoding to type
 *
 * @param ds_ty taco data type 
 * @param is_a_single pointer to value =1 if single, 0 otherwise 
 * @param is_an_array pointer to value =1 if an array, 0 otherwise
 * @param is_a_special pointer to value =1 if special type, 0 otherwise
 *
 * @return 0: no error -1: error
 */
long check_type(long ds_ty, long *is_a_single, 
                long *is_an_array, long *is_a_special)
{   
   switch (ds_ty) 
   {
      case D_VOID_TYPE:
	 return(0);
         break;
      case D_BOOLEAN_TYPE:
      case D_USHORT_TYPE:
      case D_SHORT_TYPE:
      case D_ULONG_TYPE:
      case D_LONG_TYPE:
      case D_FLOAT_TYPE:
      case D_DOUBLE_TYPE:
      case D_STRING_TYPE:
		case D_OPAQUE_TYPE:
         *is_a_single = 1;
	 return(0);
	 break;
      case D_VAR_CHARARR:
      case D_VAR_SHORTARR:
      case D_VAR_LONGARR:
      case D_VAR_ULONGARR:
      case D_VAR_FLOATARR:
      case D_VAR_DOUBLEARR:
      case D_VAR_STRINGARR:
      case D_VAR_FRPARR:
      case D_VAR_SFRPARR:
      case D_VAR_LRPARR:
         *is_an_array = 1;
	 return(0);
	 break;
      case D_INT_FLOAT_TYPE:
      case D_FLOAT_READPOINT:
      case D_LONG_READPOINT:
      case D_DOUBLE_READPOINT:
      case D_MOTOR_LONG:
      case D_MOTOR_FLOAT:
      case D_STATE_FLOAT_READPOINT:
      case D_MULMOVE_TYPE:
         *is_a_special = 1;
	 return(0);
	 break;
      default:
         return(-1);
   } 
}   

#ifdef NUMPY
/**
 * Check that the array typenum is compatible with taco
 *
 * @param typenum:	array type
 * @param ds_out:		argout type
 *
 * @return 0: compatible -1: NOT compatible	
 */
long check_provided(long typenum,long ds_out)
{
    switch (ds_out)
    {
       case D_VAR_CHARARR:
          if ( (typenum == PyArray_CHAR) || 
	       (typenum == PyArray_UBYTE) ||
	       (typenum == PyArray_SBYTE) )
	     return(0);
	  else
	     return(-1);
	  break;
       case D_VAR_USHORTARR:
       case D_VAR_SHORTARR:
          if (typenum == PyArray_SHORT)
	     return(0);
	  else
	     return(1);
	  break;
       case D_VAR_ULONGARR:
       case D_VAR_LONGARR:
          if (typenum == PyArray_LONG)
	     return(0);
	  else
	     return(1);          
       case D_VAR_FLOATARR:
          if (typenum == PyArray_FLOAT)
	     return(0);
	  else
	     return(1);          
       case D_VAR_DOUBLEARR:
          if (typenum == PyArray_DOUBLE)
	     return(0);
	  else
	     return(1);          

    }  
}

/**
 * Returns the Numeric type coresponding to C argout type.
 *
 * @param ds_out argout type
 *
 * @return -1:no conversion possible else numeric type	
 */
long Ctype2numeric(long ds_out)
{
    switch (ds_out)
    {
       case D_VAR_CHARARR:
          return(PyArray_CHAR);
       case D_VAR_USHORTARR:
       case D_VAR_SHORTARR:
          return(PyArray_SHORT);
       case D_VAR_ULONGARR:
       case D_VAR_LONGARR:
          return(PyArray_LONG);        
       case D_VAR_FLOATARR:
          return(PyArray_FLOAT);
       case D_VAR_DOUBLEARR:
          return(PyArray_DOUBLE);
       default:
          return(-1);          
    }      
}
#endif /*NUMPY*/

/**
 * Returns the number of bytes for the basic element of taco variable array data.
 * 
 * @param ds_out type
 * 
 * @return -1: cannot calculate else: number of bytes	
 */
long lenoftype(long ds_out)
{
    switch (ds_out)
    {
       case D_VAR_CHARARR:
          return(sizeof(char));
       case D_VAR_USHORTARR:
       case D_VAR_SHORTARR:
          return(sizeof(short));
       case D_VAR_ULONGARR:
       case D_VAR_LONGARR:
          return(sizeof(long));        
       case D_VAR_FLOATARR:
          return(sizeof(float));
       case D_VAR_DOUBLEARR:
          return(sizeof(double));
       default:
          return(-1);          
    }      
}
