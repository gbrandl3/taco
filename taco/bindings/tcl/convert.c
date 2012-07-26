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
 * File : 	convert.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains necessary functions for dev_io.c
 *  	  	All the functions present in this file are used for the conversion
 *  	  	of the input and output arguments passed to dev_putget.
 *
 * Author(s): 	Denis Beauvois, ESRF
 *		Gilbert Pepellin
 *		J. Meyer
 *              $Author: jensmeyer $
 *
 * Original:  	June 1996
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2012-07-26 15:52:48 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <tcl.h>

#include "dev_io.h"
#include "debug.h"

void		*InputArg,
		*OutputArg,
		*SequencePointer;

DevString	StringType,
		StringTypeOut,
		*StringArray;

/* #define CONV_TRACE */

/* ===================================== */
int F_D_BOOLEAN_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevBoolean tmp;
 const char *aux;
 int temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_GetBoolean (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevBoolean)temp;
     InputArg = (DevBoolean*) malloc(sizeof(DevBoolean));
     memcpy (InputArg,&tmp,sizeof(DevBoolean));
     return True;
    }
 else
    {
     OutputArg = (DevBoolean*) malloc(sizeof(DevBoolean));
     return True;
    }
}

/* ==================================== */
int F_D_USHORT_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevUShort tmp;
 const char *aux;
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_ExprLong (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevUShort)temp;
     InputArg = (DevUShort *) malloc(sizeof(DevUShort));
     memcpy (InputArg,&tmp,sizeof(DevUShort));
     return True;
    }
 else
    {
     OutputArg = (DevUShort*) malloc(sizeof(DevUShort));
     return True;
    }
}

/* =================================== */
int F_D_SHORT_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevShort tmp;
 const char *aux;
 int temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_GetInt (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevShort)temp;
     InputArg = (DevShort*) malloc(sizeof(DevShort));
     memcpy (InputArg,&tmp,sizeof(DevShort));
     return True;
    }
 else
    {
     OutputArg = (DevShort *) malloc (sizeof(DevShort));
     return True;
    }
}

/* =================================== */ 
int F_D_ULONG_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevULong tmp;
 const char *aux;
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_ExprLong (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevULong)temp;
     InputArg = (DevULong*) malloc (sizeof(DevULong));
     memcpy (InputArg,&tmp,sizeof(DevULong));
     return True;
    }
 else
    {
     OutputArg = (DevULong*) malloc(sizeof(DevULong));
     return True;
    }
}

/* ================================== */
int F_D_LONG_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevLong tmp;
 const char *aux;
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_ExprLong (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevLong)temp;
     InputArg = (DevLong *) malloc(sizeof(DevLong));
     memcpy (InputArg,&tmp,sizeof(DevLong));
     return True;
    }
 else
    {
     OutputArg = (DevLong *) malloc(sizeof(DevLong));
     return True;
    }
}

/* =================================== */
int F_D_FLOAT_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevFloat tmp;
 const char *aux;
 double temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_ExprDouble (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevFloat)temp;
     InputArg = (DevFloat *) malloc (sizeof(DevFloat));
     memcpy (InputArg,&tmp,sizeof(DevFloat));
     return True;
    }
 else
    {
     OutputArg = (DevFloat *) malloc (sizeof(DevFloat));
     return True;
    }
}

/* ==================================== */
int F_D_DOUBLE_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 DevDouble tmp;
 const char *aux;
 double temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_ExprDouble (interp,aux,&temp);
     if (R==TCL_ERROR) return False;
     tmp = (DevDouble)temp;
     InputArg = (DevDouble *) malloc (sizeof(DevDouble));
     memcpy (InputArg,&tmp,sizeof(DevDouble));
     return True;
    }
 else
    {
     OutputArg = (DevDouble *) malloc(sizeof(DevDouble));
     return True;
    }
}

/* ==================================== */
int F_D_STRING_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     StringType =(DevString ) calloc (strlen(aux)+1,sizeof(char));
     if(TCL_TEST) printf("convert: StringType= %p\n",StringType);
     memcpy (StringType,aux,strlen(aux));
     if(TCL_TEST) printf("convert: [StringType]= %s\n",StringType);
      InputArg = &(StringType);
     return True;
    }
 else
    {
     /* StringType = NULL; GP140197 */
     OutputArg = &(StringTypeOut); /* GP140197 */
     return True;
    }
}

/* ======================================= */
int F_D_INT_FLOAT_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 long tmp1;
 double tmp2;
 DevIntFloat tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 2)       /* 2 elements in the structure */
        {
         return False;
        }
     R=Tcl_ExprLong (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.state = tmp1;
     tmp3.value = (float)tmp2;
     InputArg = (DevIntFloat*) malloc (sizeof(DevIntFloat));
     memcpy (InputArg,&tmp3,sizeof(DevIntFloat));
     Tcl_Free ((char *)SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevIntFloat*) malloc(sizeof(DevIntFloat));
     return True;
    }
}

/* ======================================== */
int F_D_FLOAT_READPOINT (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 double tmp1;
 double tmp2;
 DevFloatReadPoint tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 2)       /* 2 elements in the structure */
        {
         return False;
        }
     R=Tcl_ExprDouble (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.set  = (float)tmp1;
     tmp3.read = (float)tmp2;
     InputArg = (DevFloatReadPoint*) malloc (sizeof(DevFloatReadPoint));
     memcpy (InputArg,&tmp3,sizeof(DevFloatReadPoint));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevFloatReadPoint*) malloc (sizeof(DevFloatReadPoint));
     return True;
    }
}

/* ============================================== */
int F_D_STATE_FLOAT_READPOINT (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 int tmp1;
 double tmp2;
 double tmp3;
 DevStateFloatReadPoint tmp4;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 3)       /* 3 elements in the structure */
        {
         return False;
        }
     R=Tcl_GetInt (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[2],&tmp3);
     if (R==TCL_ERROR) return False;
     tmp4.state = (short)tmp1;
     tmp4.set   = (float)tmp2;
     tmp4.read  = (float)tmp3;
     InputArg = (DevStateFloatReadPoint*) malloc(sizeof(DevStateFloatReadPoint));
     memcpy (InputArg,&tmp4,sizeof(DevStateFloatReadPoint));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevStateFloatReadPoint*) malloc(sizeof(DevStateFloatReadPoint));
     return True;
    }
}

/* ======================================= */
int F_D_LONG_READPOINT (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 long tmp1;
 long tmp2;
 DevLongReadPoint tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (Count != 2)       /* 2 elements in the structure */
        {
         return False;
        }
     R=Tcl_ExprLong (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprLong (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.set  = tmp1;
     tmp3.read = tmp2;
     InputArg = (DevLongReadPoint*) malloc(sizeof(DevLongReadPoint));
     memcpy (InputArg,&tmp3,sizeof(DevLongReadPoint));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevLongReadPoint*) malloc(sizeof(DevLongReadPoint));
     return True;
    }
}

/* ========================================= */
int F_D_DOUBLE_READPOINT (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 double tmp1;
 double tmp2;
 DevDoubleReadPoint tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 2)       /* 2 elements in the structure */
        {
         return False;
        }
     R=Tcl_ExprDouble (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.set  = tmp1;
     tmp3.read = tmp2;
     InputArg = (DevDoubleReadPoint*) malloc(sizeof(DevDoubleReadPoint));
     memcpy (InputArg,&tmp3,sizeof(DevDoubleReadPoint));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevDoubleReadPoint*) malloc(sizeof(DevDoubleReadPoint));
     return True;
    }
}

/* ==================================== */
int F_D_VAR_CHARARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 DevVarCharArray Arr;

 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0); 
     Arr.length   = strlen(aux);
     Arr.sequence = (char *)aux;
       
     InputArg = (DevVarCharArray *) malloc(sizeof(DevVarCharArray));
     memcpy (InputArg,&Arr,sizeof(DevVarCharArray));
     return True;
    }
 else
    {
     OutputArg = (DevVarCharArray*) malloc(sizeof(DevVarCharArray));
     ((DevVarCharArray*)OutputArg)->length = 0;
     ((DevVarCharArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ====================================== */
int F_D_VAR_STRINGARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 DevVarStringArray Arr;
 int Count; /* Number of elements present in the splited list */
 char **SplitList; /* Array of the split list */
 int R;

 if (Param==INPUT)
    {
     Tcl_ResetResult(interp);
     aux = Tcl_GetVar(interp,Name,0);

/* split the input Tcl list whose name is strored in aux. we will get:
     count = number of elements of the Tcl list 
     *SplitList = filled in with the address of an array of pointers to the 
                  strings taht are teh extracted elements of the Tcl list. 
                  There will be count valid entries in the array, followed by
                  a NULL entry
*/
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
#ifdef CONV_TRACE
     printf("F_D_VAR_STRINGARR:.....\n");
     printf("SplitList: count=%d\n",Count);
       for (i=0;i<Count;i++)
       {
         printf("SplitList(%d)= %X\n",i,SplitList+i);
         printf("str%d. l=%d val=%s\n",i,strlen(*(SplitList+i)),*(SplitList+i));
       }
#endif
     if (R==TCL_ERROR) {
#ifdef CONV_TRACE
     printf(" R== TclERROR after Tcl_SpliList\n"); 
#endif
     return False;
     }
     
     if (Count<1)
        {
         return False;
        }

     Arr.length   = (u_int)Count;
     Arr.sequence = SplitList;
     InputArg = (DevVarStringArray *) malloc(sizeof(DevVarStringArray));
     memcpy (InputArg,&Arr,sizeof(DevVarStringArray));
#ifdef CONV_TRACE
     printf("InputArg: length= %d\n",((DevVarStringArray*)InputArg)->length);
     for (i=0;i<Count;i++)
     {
       printf("InputArg->sequence(%d)= %X\n",i,((DevVarStringArray*)InputArg)->sequence+i);
     }
#endif
     Tcl_ResetResult(interp);
     return True;
    }
 else
    {
     OutputArg = (DevVarStringArray*) malloc(sizeof(DevVarStringArray));
		 #ifdef CONV_TRACE
		 printf("F_D_VAR_STRINGARR: OutputArg malloced: %X\n",OutputArg);
		 #endif
     StringType = NULL;
     ((DevVarStringArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ====================================== */
int F_D_VAR_USHORTARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarUShortArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (u_short *) malloc (Count*sizeof(u_short)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((u_short*)SequencePointer+i) = (u_short)temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarUShortArray *) malloc (sizeof(DevVarUShortArray));
     memcpy (InputArg,&Arr,sizeof(DevVarUShortArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarUShortArray*) malloc(sizeof(DevVarUShortArray));
     ((DevVarUShortArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ===================================== */
int F_D_VAR_SHORTARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarShortArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (short *) malloc (Count*sizeof(short)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((short *)SequencePointer+i) = (short)temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarShortArray *) malloc (sizeof(DevVarShortArray));
     memcpy (InputArg,&Arr,sizeof(DevVarShortArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarShortArray*) malloc (sizeof(DevVarShortArray));
     ((DevVarShortArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ===================================== */
int F_D_VAR_ULONGARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarULongArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (unsigned long *) malloc(Count*sizeof(unsigned long)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((unsigned long *)SequencePointer+i) = (unsigned long)temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarULongArray *) malloc (sizeof(DevVarULongArray));
     memcpy (InputArg,&Arr,sizeof(DevVarULongArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarULongArray*) malloc (sizeof(DevVarULongArray));
     ((DevVarULongArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ==================================== */
int F_D_VAR_LONGARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarLongArray Arr;
 int Count; /* Number of elements present in the splited list */
 char **SplitList; /* Array of the split list */
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (long *) malloc (Count*sizeof(long)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((long*)SequencePointer+i) = temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarLongArray *) malloc (sizeof(DevVarLongArray));
     memcpy (InputArg,&Arr,sizeof(DevVarLongArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarLongArray*) malloc(sizeof(DevVarLongArray));
     ((DevVarLongArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ===================================== */
int F_D_VAR_FLOATARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarFloatArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 double temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (float*) malloc (Count*sizeof(float)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprDouble(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((float*)SequencePointer+i) = (float)temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarFloatArray *) malloc (sizeof(DevVarFloatArray));
     memcpy (InputArg,&Arr,sizeof(DevVarFloatArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarFloatArray*) malloc (sizeof(DevVarFloatArray));
     ((DevVarFloatArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ====================================== */
int F_D_VAR_DOUBLEARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarDoubleArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 double temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (double *) malloc (Count*sizeof(double)+1);
     for (i=0;i<Count;i++)
         {
          R=Tcl_ExprDouble(interp,SplitList[i],&temp);
          if (R==TCL_ERROR) return False;
          *((double *)SequencePointer+i) = temp;
         }
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarDoubleArray *) malloc (sizeof(DevVarDoubleArray));
     memcpy (InputArg,&Arr,sizeof(DevVarDoubleArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarDoubleArray*) malloc(sizeof(DevVarDoubleArray));
     ((DevVarDoubleArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* =================================== */
int F_D_VAR_FRPARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarFloatReadPointArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 double temp;
 int R;
 
 fflush(stdout);
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<2 || Count%2==1)
        {
         return False;
        }
     SequencePointer = (DevFloatReadPoint *) malloc (Count*sizeof(DevFloatReadPoint));/*Large!*/
     for (i=0;i<Count/2;i++)
         {
          R=Tcl_ExprDouble(interp,SplitList[2*i],&temp);
          if (R==TCL_ERROR) return False;
          ((DevFloatReadPoint *)(SequencePointer)+i)->set = (float)temp;
          R=Tcl_ExprDouble(interp,SplitList[2*i+1],&temp);
          if (R==TCL_ERROR) return False;
          ((DevFloatReadPoint *)(SequencePointer)+i)->read = (float)temp;
         }
     Arr.length   = (u_int)Count/2;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarFloatReadPointArray *) malloc (sizeof(DevVarFloatReadPointArray));
     memcpy (InputArg,&Arr,sizeof(DevVarFloatReadPointArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarFloatReadPointArray*) malloc(sizeof(DevVarFloatReadPointArray));
     ((DevVarFloatReadPointArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ==================================== */
int F_D_VAR_SFRPARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarStateFloatReadPointArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 double temp1;
 long  temp2;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<3 || Count%3 != 0)
        {
         return False;
        }
     SequencePointer = (DevStateFloatReadPoint *) malloc (Count*sizeof(DevStateFloatReadPoint));/*Large!*/
     for (i=0;i<Count/3;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[3*i],&temp2);
          if (R==TCL_ERROR) return False;
          ((DevStateFloatReadPoint *)(SequencePointer)+i)->state = (short)temp2;
          R=Tcl_ExprDouble(interp,SplitList[3*i+1],&temp1);
          if (R==TCL_ERROR) return False;
          ((DevStateFloatReadPoint *)(SequencePointer)+i)->set = (float)temp1;
          R=Tcl_ExprDouble(interp,SplitList[3*i+2],&temp1);
          if (R==TCL_ERROR) return False;
          ((DevStateFloatReadPoint *)(SequencePointer)+i)->read = (float)temp1;
         }
     Arr.length   = (u_int)Count/3;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarStateFloatReadPointArray *) malloc (sizeof(DevVarStateFloatReadPointArray));
     memcpy (InputArg,&Arr,sizeof(DevVarStateFloatReadPointArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarStateFloatReadPointArray*) malloc(sizeof(DevVarStateFloatReadPointArray));
     ((DevVarStateFloatReadPointArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* =================================== */
int F_D_VAR_LRPARR (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevVarLongReadPointArray Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 long temp;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<2 || Count%2==1)
        {
         return False;
        }
     SequencePointer = (DevLongReadPoint *) malloc (Count*sizeof(DevLongReadPoint));/*Large!*/
     for (i=0;i<Count/2;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[2*i],&temp);
          if (R==TCL_ERROR) return False;
          ((DevLongReadPoint *)(SequencePointer)+i)->set = temp;
          R=Tcl_ExprLong(interp,SplitList[2*i+1],&temp);
          if (R==TCL_ERROR) return False;
          ((DevLongReadPoint *)(SequencePointer)+i)->read = temp;
         }
     Arr.length   = (u_int)Count/2;
     Arr.sequence = SequencePointer;
     InputArg = (DevVarLongReadPointArray *) malloc (sizeof(DevVarLongReadPointArray));
     memcpy (InputArg,&Arr,sizeof(DevVarLongReadPointArray));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevVarLongReadPointArray*) malloc(sizeof(DevVarLongReadPointArray));
     ((DevVarLongReadPointArray*)OutputArg)->sequence = NULL;
     return True;
    }
}

/* ==================================== */
int F_D_OPAQUE_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 DevOpaque Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count<1)
        {
         return False;
        }
     SequencePointer = (char *) malloc (Count*sizeof(char)+1);
     strncpy ((char*)SequencePointer,*SplitList,Count);
     Arr.length   = (u_int)Count;
     Arr.sequence = SequencePointer;
     InputArg = (DevOpaque *) malloc(sizeof(DevOpaque));
     memcpy (InputArg,&Arr,sizeof(DevOpaque));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevOpaque*) malloc(sizeof(DevOpaque));
     ((DevOpaque*)OutputArg)->sequence = NULL;
     return True;
    }
}

#if 0
/* ===================================== */
int F_D_MULMOVE_TYPE (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 int i;
 const char *aux;
 DevMulMove Arr;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 double temp1;
 long  temp2;
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar(interp,Name,0);
     Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (Count!=24)    /* Number of elements in the DevMulMove structure */
        {
         return False; /* Must be modified */
        }
     for (i=0;i<8;i++)
         {
          R=Tcl_ExprLong(interp,SplitList[i],&temp2);
          if (R==TCL_ERROR) return False;
          Arr.action[i] = temp2;
          R=Tcl_ExprLong(interp,SplitList[i+8],&temp2);
          if (R==TCL_ERROR) return False;
          Arr.delay[i] = temp2;
          R=Tcl_ExprDouble(interp,SplitList[i+16],&temp1);
          if (R==TCL_ERROR) return False;
          Arr.position[i] = (float)temp1;
         }
     InputArg = (DevMulMove *) malloc(sizeof(DevMulMove));
     memcpy (InputArg,&Arr,sizeof(DevMulMove));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevMulMove*) malloc(sizeof(DevMulMove));
     return True;
    }
}

/* =================================== */
int F_D_MOTOR_LONG (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 long tmp1;
 long tmp2;
 DevMotorLong tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 2)       /* 2 elements in the structure */
        {
         return False;
        }
     R=Tcl_ExprLong (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprLong (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.axisnum  = tmp1;
     tmp3.value    = tmp2;
     InputArg = (DevMotorLong*) malloc (sizeof(DevMotorLong));
     memcpy (InputArg,&tmp3,sizeof(DevMotorLong));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevMotorLong*) malloc(sizeof(DevMotorLong));
     return True;
    }
}

/* ==================================== */
int F_D_MOTOR_FLOAT (interp,Name,Param)
     Tcl_Interp *interp;
     char *Name;
     int Param;
{
 const char *aux;
 long tmp1;
 double tmp2;
 DevMotorFloat tmp3;
 int Count; /* In order to Know how much elements are present in the split list */
 char **SplitList; /* Array of the split list */
 int R;
 
 if (Param==INPUT)
    {
     aux = Tcl_GetVar (interp,Name,0);
     R=Tcl_SplitList (interp,aux,&Count,(const char ***)&SplitList);
     if (R==TCL_ERROR) return False;
     if (Count != 2)       /* 2 elements in the structure */
        {
        return False;
        }
     R=Tcl_ExprLong (interp,SplitList[0],&tmp1);
     if (R==TCL_ERROR) return False;
     R=Tcl_ExprDouble (interp,SplitList[1],&tmp2);
     if (R==TCL_ERROR) return False;
     tmp3.axisnum  = tmp1;
     tmp3.value    = (float)tmp2;
     InputArg = (DevMotorFloat*) malloc(sizeof(DevMotorFloat));
     memcpy (InputArg,&tmp3,sizeof(DevLongReadPoint));
     free (SplitList);
     return True;
    }
 else
    {
     OutputArg = (DevMotorFloat*) malloc(sizeof(DevMotorFloat));
     return True;
    }
}
#endif

