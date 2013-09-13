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
 * File         : dev_menu.c
 *
 * Project      : TACO tools, generic ascii menu
 *
 * Description  :
 *
 *
 * Author       : JL PONS, R. Wilcke
 *                $Author: jkrueger1 $
 *
 * Original     : JL PONS
 *
 * Version      : $Revision: 1.6 $
 *
 * Date         : $Date: 2008-10-24 16:08:39 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <MDSSignal.h>
#ifdef __cplusplus
}
#endif

#include <DevErrors.h>
#include <stdlib.h>

#include <taco_utils.h>

char *T0="D_VOID_TYPE";
char *T1="D_BOOLEAN_TYPE";
char *T2="D_USHORT_TYPE";
char *T3="D_SHORT_TYPE";
char *T4="D_ULONG_TYPE";
char *T5="D_LONG_TYPE";
char *T6="D_FLOAT_TYPE";
char *T7="D_DOUBLE_TYPE";
char *T8="D_STRING_TYPE";
char *T9="D_INT_FLOAT_TYPE";
char *T10="D_FLOAT_READPOINT";
char *T11="D_STATE_FLOAT_READPOINT";
char *T12="D_LONG_READPOINT";
char *T13="D_DOUBLE_READPOINT";
char *T14="D_VAR_CHARARR";
char *T15="D_VAR_STRINGARR";
char *T16="D_VAR_USHORTARR";
char *T17="D_VAR_SHORTARR";
char *T18="D_VAR_ULONGARR";
char *T19="D_VAR_LONGARR";
char *T20="D_VAR_FLOATARR";
char *T21="D_VAR_DOUBLEARR";
char *T22="D_VAR_FRPARR";
char *T23="D_VAR_SFRPARR";
char *T24="D_VAR_LRPARR";
char *T25="D_OPAQUE_TYPE";
char *T29="Unknown type";

DevVarStringArray sig_config={0,NULL};
devserver ds;
int use_signal=0;

/**********************************************************************/
/* Func   : Type_Str()                                                */
/* Descr  : Return a constant string wich contains date type name.    */
/* Arg in : code type                                                 */
/**********************************************************************/
char *Type_Str(long t)
{
   switch(t) {
      case D_VOID_TYPE:              return T0;  break;
      case D_BOOLEAN_TYPE:           return T1;  break;
      case D_USHORT_TYPE:            return T2;  break;
      case D_SHORT_TYPE:             return T3;  break;
      case D_ULONG_TYPE:             return T4;  break;
      case D_LONG_TYPE:              return T5;  break;
      case D_FLOAT_TYPE:             return T6;  break;
      case D_DOUBLE_TYPE:            return T7;  break;
      case D_STRING_TYPE:            return T8;  break;
      case D_INT_FLOAT_TYPE:         return T9;  break;
      case D_FLOAT_READPOINT:        return T10; break;
      case D_STATE_FLOAT_READPOINT:  return T11; break;
      case D_LONG_READPOINT:         return T12; break;
      case D_DOUBLE_READPOINT:       return T13; break;
      case D_VAR_CHARARR:            return T14; break;
      case D_VAR_STRINGARR:          return T15; break;
      case D_VAR_USHORTARR:          return T16; break;
      case D_VAR_SHORTARR:           return T17; break;
      case D_VAR_ULONGARR:           return T18; break;
      case D_VAR_LONGARR:            return T19; break;
      case D_VAR_FLOATARR:           return T20; break;
      case D_VAR_DOUBLEARR:          return T21; break;
      case D_VAR_FRPARR:             return T22; break;
      case D_VAR_SFRPARR:            return T23; break;
      case D_VAR_LRPARR:             return T24; break;
      case D_OPAQUE_TYPE:            return T25; break;
      default:                       return T29; break;
   }
}

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options] <device name>\n", cmd);
        fprintf(stderr, "         options: -h display this message\n");
	fprintf(stderr, "                  -t use tcp as communication protocol\n");
        fprintf(stderr, "                  -n nethost\n");
        fprintf(stderr, "                  -v display the current version\n");
        exit(1);
}

void version(const char *cmd)
{
        fprintf(stderr, "%s version %s\n", cmd,  VERSION);
        exit(0);
}

int get_value(char *format,void *v)
{
   char s[128];
   rewind(stdin);
   /* gets(s); */
   scanf("%s",&s);

   if(strcmp(s,"#")==0) {
     printf("Canceled.\n");
     return 0;
   }

   if(sscanf(s,format,v)==0) {
     printf("Invalid value. Don't perform anything.\n");
     return 0;
   }

   return 1;
}

int get_argin(int argin_type,void **arg)
{
   short booll;
   int i,b;
   unsigned int lng;
   char tmp[128];

   static DevBoolean arg_boolean=0;
   static DevUShort arg_ushort=0;
   static DevShort arg_short=0;
   static DevULong arg_ulong=0;
   static DevLong arg_long=0;
   static DevFloat arg_float=0.0;
   static DevDouble arg_double=0.0;
   static DevString arg_string=NULL;

   static DevIntFloat arg_intfloat={0,0.};
   static DevFloatReadPoint arg_floatread={0.,0.};
   static DevStateFloatReadPoint arg_statefloatread={0,0.,0.};
   static DevLongReadPoint arg_longread={0,0};
   static DevDoubleReadPoint arg_doubleread={0.,0.};

   static DevVarCharArray arg_chararr={0,NULL};
   static DevVarStringArray arg_stringarr={0,NULL};
   static DevVarUShortArray arg_ushortarr={0,NULL};
   static DevVarShortArray arg_shortarr={0,NULL};
   static DevVarULongArray arg_ulongarr={0,NULL};
   static DevVarLongArray arg_longarr={0,NULL};
   static DevVarFloatArray arg_floatarr={0,NULL};
   static DevVarDoubleArray arg_doublearr={0,NULL};

   static DevVarFloatReadPointArray arg_frparr={0,NULL};
   static DevVarStateFloatReadPointArray arg_sfrparr={0,NULL};
   static DevVarLongReadPointArray arg_lrparr={0,NULL};

   switch(argin_type) {

      case D_BOOLEAN_TYPE:
         printf("Enter D_BOOLEAN_TYPE value (# to cancel):");
         if(get_value("%hd",&booll))
         {
            arg_boolean = (char)booll;
            *arg=&arg_boolean;
            return 1;
         }
         return 0;
      
      case D_USHORT_TYPE:
         printf("Enter D_USHORT_TYPE value (# to cancel):");
         if(get_value("%hu",&arg_ushort))
         {
            *arg=&arg_ushort;
            return 1;
         }
         return 0;
      
      case D_SHORT_TYPE:
         printf("Enter D_SHORT_TYPE value (# to cancel):");
         if(get_value("%hd",&arg_short))
         {
            *arg=&arg_short;
            return 1;
         } else {
            return 0;
         }
         break;
      
      case D_ULONG_TYPE:
         printf("Enter D_ULONG_TYPE value (# to cancel):");
         if(get_value("%lu",&arg_ulong))
         {
            *arg=&arg_ulong;
            return 1;
         }
         return 0;
      
      case D_LONG_TYPE:
         printf("Enter D_LONG_TYPE value (# to cancel):");
         if(get_value("%ld",&arg_long))
         {
            *arg=&arg_long;
            return 1;
         } else {
            return 0;
         }
         break;
      
      case D_FLOAT_TYPE:
         printf("Enter D_FLOAT_TYPE value (# to cancel):");
         if(get_value("%f",&arg_float))
         {
            *arg=&arg_float;
            return 1;
         } else {
            return 0;
         }
         break;
      
      case D_DOUBLE_TYPE:
         printf("Enter D_DOUBLE_TYPE value (# to cancel):");
         if(get_value("%lf",&arg_double))
         {
            *arg=&arg_double;
            return 1;
         } else {
            return 0;
         }
         break;
      
      case D_STRING_TYPE:
         printf("Enter D_STRING_TYPE value (# to cancel):");
         rewind(stdin); 
         /* gets(tmp); */
         scanf("%s",tmp);
	 
         if(strcmp(tmp,"#")==0) {
            printf("Canceled.\n");
            return 0;
         }
         arg_string=(char *)malloc(strlen(tmp)+1);
         strcpy(arg_string,tmp);
         *arg=&arg_string;
         return 1;
         break;

      case D_INT_FLOAT_TYPE:
         printf("Enter D_INT_FLOAT_TYPE State value (# to cancel):");
         if(get_value("%ld",&(arg_intfloat.state))==0)
            return 0;
         printf("Enter D_STATE_FLOAT_READPOINT Value value (# to cancel):");
         if(get_value("%f",&(arg_intfloat.value))==0)
            return 0;
         *arg=&arg_intfloat;
         return 1;

      case D_FLOAT_READPOINT:
         printf("Enter D_FLOAT_READPOINT Set value (# to cancel):");
         if(get_value("%f",&(arg_floatread.set))==0)
            return 0;
         printf("Enter D_FLOAT_READPOINT Read value (# to cancel):");
         if(get_value("%f",&(arg_floatread.read))==0)
            return 0;
         *arg=&arg_floatread;
         return 1;

      case D_STATE_FLOAT_READPOINT:
         printf("Enter D_STATE_FLOAT_READPOINT State value (# to cancel):");
         if(get_value("%hd",&(arg_statefloatread.state))==0)
            return 0;
         printf("Enter D_STATE_FLOAT_READPOINT Set value (# to cancel):");
         if(get_value("%f",&(arg_statefloatread.set))==0)
            return 0;
         printf("Enter D_STATE_FLOAT_READPOINT Read value (# to cancel):");
         if(get_value("%f",&(arg_statefloatread.read))==0)
            return 0;
         *arg=&arg_statefloatread;
         return 1;

      case D_LONG_READPOINT:
         printf("Enter D_LONG_READPOINT Set value (# to cancel):");
         if(get_value("%ld",&(arg_longread.set))==0)
            return 0;
         printf("Enter D_LONG_READPOINT Read value (# to cancel):");
         if(get_value("%ld",&(arg_longread.read))==0)
            return 0;
         *arg=&arg_longread;
         return 1;

      case D_DOUBLE_READPOINT:
         printf("Enter D_DOUBLE_READPOINT Set value (# to cancel):");
         if(get_value("%lf",&(arg_doubleread.set))==0)
            return 0;
         printf("Enter D_DOUBLE_READPOINT Read value (# to cancel):");
         if(get_value("%lf",&(arg_doubleread.read))==0)
            return 0;
         *arg=&arg_doubleread;
         return 1;
      
      case D_VAR_CHARARR:
         printf("Enter D_VAR_CHARARR length (# to cancel):");
         if(get_value("%u",&lng)==0)
            return 0;
         arg_chararr.length=lng;
         arg_chararr.sequence=(char *)malloc(lng*sizeof(char));
         b=1;
         i=0;
         while(i<lng && b)
         {
            printf("ARR[%02d]=",i);
            b=get_value("%c",&(arg_chararr.sequence[i]));
            i++;
         }
         *arg=&arg_chararr;
         return b;
      
      case D_VAR_STRINGARR:
         printf("Enter D_VAR_STRINGARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_stringarr.length=lng;
            arg_stringarr.sequence=(char **)malloc(lng*sizeof(char *));
            b=1;
            i=0;
            while(i<lng && b)
            {
               printf("ARR[%02d]=",i);
               rewind(stdin); 
               /* gets(tmp); */
               scanf("%s",&tmp);

               if(strcmp(tmp,"#")==0) {
                  printf("Canceled.\n");
                  b=0;
               }
               arg_stringarr.sequence[i]=(char *)malloc(strlen(tmp)+1);
               strcpy(arg_stringarr.sequence[i],tmp);
               i++;
            }
         }
         *arg=&arg_stringarr;
         return b;
         break;
      
      case D_VAR_USHORTARR:
         printf("Enter D_VAR_USHORTARR length (# to cancel):");
         if(get_value("%u",&lng)==0)
            return 0;
         arg_ushortarr.length=lng;
         arg_ushortarr.sequence=(unsigned short *)malloc(lng*
            sizeof(unsigned short));
         b=1;
         i=0;
         while(i<lng && b)
         {
            printf("ARR[%02d]=",i);
            b=get_value("%hu",&(arg_ushortarr.sequence[i]));
            i++;
         }
         *arg=&arg_ushortarr;
         return b;
      
      case D_VAR_SHORTARR:
         printf("Enter D_VAR_SHORTARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_shortarr.length=lng;
            arg_shortarr.sequence=(short *)malloc(lng*sizeof(short));
            b=1;
            i=0;
            while(i<lng && b)
            {
               printf("ARR[%02d]=",i);
               b=get_value("%hd",&(arg_shortarr.sequence[i]));
               i++;
            }
            *arg=&arg_shortarr;
            return b;
         }
         break;
      
      case D_VAR_ULONGARR:
         printf("Enter D_VAR_ULONGARR length (# to cancel):");
         if(get_value("%u",&lng)==0)
            return 0;
         arg_ulongarr.length=lng;
         arg_ulongarr.sequence=(DevULong *)malloc(lng*
            sizeof(DevULong));
         b=1;
         i=0;
         while(i<lng && b)
         {
            printf("ARR[%02d]=",i);
            b=get_value("%lu",&(arg_ulongarr.sequence[i]));
            i++;
         }
         *arg=&arg_ulongarr;
         return b;
      
      case D_VAR_LONGARR:
         printf("Enter D_VAR_LONGARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_longarr.length=lng;
            arg_longarr.sequence=(DevLong *)malloc(lng*sizeof(DevLong));
            b=1;
            i=0;
            while(i<lng && b)
            {
               printf("ARR[%02d]=",i);
               b=get_value("%ld",&(arg_longarr.sequence[i]));
               i++;
            }
            *arg=&arg_longarr;
            return b;
         }
         break;
      
      case D_VAR_FLOATARR:
         printf("Enter D_VAR_FLOATARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_floatarr.length=lng;
            arg_floatarr.sequence=(float *)malloc(lng*sizeof(float));
            b=1;
            i=0;
            while(i<lng && b)
            {
               printf("ARR[%02d]=",i);
               b=get_value("%f",&(arg_floatarr.sequence[i]));
               i++;
            }
            *arg=&arg_floatarr;
            return b;
         }
         break;
      
      case D_VAR_DOUBLEARR:
         printf("Enter D_VAR_DOUBLEARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_doublearr.length=lng;
            arg_doublearr.sequence=(double *)malloc(lng*sizeof(double));
            b=1;
            i=0;
            while(i<lng && b)
            {
               printf("ARR[%02d]=",i);
               b=get_value("%lf",&(arg_doublearr.sequence[i]));
               i++;
            }
            *arg=&arg_doublearr;
            return b;
         }
         break;

      case D_VAR_FRPARR:
         printf("Enter D_VAR_FRPARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_frparr.length=lng;
            arg_frparr.sequence=(DevFloatReadPoint *)
               malloc(lng*sizeof(DevFloatReadPoint));
            i=0;
            while(i<lng)
            {
               printf("ARR[%02d].set =",i);
               if(get_value("%f",&(arg_frparr.sequence[i].set))==0)
                  return 0;
               printf("ARR[%02d].read=",i);
               if(get_value("%f",&(arg_frparr.sequence[i]).read)==0)
                  return 0;
               i++;
            }
            *arg=&arg_frparr;
            return 1;
         }
         break;

      case D_VAR_SFRPARR:
         printf("Enter D_VAR_SFRPARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_sfrparr.length=lng;
            arg_sfrparr.sequence=(DevStateFloatReadPoint *)
               malloc(lng*sizeof(DevStateFloatReadPoint));
            i=0;
            while(i<lng)
            {
               printf("ARR[%02d].state=",i);
               if(get_value("%hd",&(arg_sfrparr.sequence[i].state))==0)
                  return 0;
               printf("ARR[%02d].set  =",i);
               if(get_value("%f",&(arg_sfrparr.sequence[i].set))==0)
                  return 0;
               printf("ARR[%02d].read =",i);
               if(get_value("%f",&(arg_sfrparr.sequence[i]).read)==0)
                  return 0;
               i++;
            }
            *arg=&arg_sfrparr;
            return 1;
         }
         break;

      case D_VAR_LRPARR:
         printf("Enter D_VAR_LRPARR length (# to cancel):");
         if(get_value("%u",&lng)==0) {
            return 0;
         } else {
            arg_lrparr.length=lng;
            arg_lrparr.sequence=(DevLongReadPoint *)
               malloc(lng*sizeof(DevLongReadPoint));
            i=0;
            while(i<lng)
            {
               printf("ARR[%02d].set =",i);
               if(get_value("%ld",&(arg_lrparr.sequence[i].set))==0)
                  return 0;
               printf("ARR[%02d].read=",i);
               if(get_value("%ld",&(arg_lrparr.sequence[i]).read)==0)
                  return 0;
               i++;
            }
            *arg=&arg_lrparr;
            return 1;
         }
         break;

      default:
         printf("Sorry but %s isn't supported.\n",Type_Str(argin_type));
         return 0;
   }
}

void display_argout(long cmd,int argout_type,void *argout)
{
   int nu_of_properties;
   int nu_of_signals;
   char format[128];
   int i,state;
   double v;

   if((cmd==DevReadSigValues) && use_signal)
   {
      if(argout_type == D_VAR_FLOATARR || argout_type == D_VAR_DOUBLEARR)
      {
         nu_of_properties = atol(sig_config.sequence[0]);
         nu_of_signals    = (sig_config.length -1) / nu_of_properties;

         for(i=0; i<nu_of_signals; i++)
         {

            if(argout_type == D_VAR_FLOATARR)
               v = ((DevVarFloatArray *)argout)->sequence[i];
            else
               v = ((DevVarDoubleArray *)argout)->sequence[i];

            if(v != 65536.0) {
               sprintf(format,"%24s [%3s] : %s\n", 
                  sig_config.sequence[(i*nu_of_properties) + 2],
                  sig_config.sequence[(i*nu_of_properties) + 3],
                  sig_config.sequence[(i*nu_of_properties) + 4]);
               printf(format,v);
            } else {
               sprintf (format, "%24s [%3s] : ###\n", 
                  sig_config.sequence[(i*nu_of_properties) + 2],
                  sig_config.sequence[(i*nu_of_properties) + 3]);
               printf(format);
            }

         }

         return;

      } else {
         printf("Warning : non standard type for DevReadSigValues\n");
      }
   }

   if(cmd == DevState) {

      if(argout_type == D_SHORT_TYPE || argout_type == D_LONG_TYPE) {

         if(argout_type == D_SHORT_TYPE)
            state=(int) *(DevShort *)argout;
         else
            state=(int) *(DevLong *)argout;

         printf("State:%d",state);

         if((state>=0) && (state<=DEVON_NOT_REACHED))
            printf(" [%s]\n",DEVSTATES[ state ]);
         else
            printf("\n");

         return;
      } else {
         printf("Warning : non standard type for DevState\n");
      }

   }

   switch(argout_type) {
      case D_BOOLEAN_TYPE:
         printf("%hd\n",*(DevBoolean *)argout);
         break;
      case D_USHORT_TYPE:
         printf("%hu\n",*(DevUShort *)argout);
         break;
      case D_SHORT_TYPE:
         printf("%hd\n",*(DevShort *)argout);
         break;
      case D_ULONG_TYPE:
         printf("%lu\n",*(DevULong *)argout);
         break;
      case D_LONG_TYPE:
         printf("%ld\n",*(DevLong *)argout);
         break;
      case D_FLOAT_TYPE:
         printf("%g\n",*(DevFloat *)argout);
         break;
      case D_DOUBLE_TYPE:
         printf("%g\n",*(DevDouble *)argout);
         break;
      case D_STRING_TYPE:
         printf("%s\n",*(DevString *)argout);
         break;

      case D_INT_FLOAT_TYPE:
         printf("State = %ld\n",((DevIntFloat *)argout)->state);
         printf("Set   = %g\n",((DevIntFloat *)argout)->value);
         break;
      case D_FLOAT_READPOINT:
         printf("Set  = %g\n",((DevFloatReadPoint *)argout)->set);
         printf("Read = %g\n",((DevFloatReadPoint *)argout)->read);
         break;
      case D_STATE_FLOAT_READPOINT:
         printf("State = %hd\n",((DevStateFloatReadPoint *)argout)->state);
         printf("Set   = %g\n",((DevStateFloatReadPoint *)argout)->set);
         printf("Read  = %g\n",((DevStateFloatReadPoint *)argout)->read);
         break;
      case D_LONG_READPOINT:
         printf("Set  = %ld\n",((DevLongReadPoint *)argout)->set);
         printf("Read = %ld\n",((DevLongReadPoint *)argout)->read);
         break;
      case D_DOUBLE_READPOINT:
         printf("Set  = %g\n",((DevDoubleReadPoint *)argout)->set);
         printf("Read = %g\n",((DevDoubleReadPoint *)argout)->read);
         break;

      case D_VAR_CHARARR:
         for(i=0; i<((DevVarCharArray *)argout)->length; i++)
            printf("ARR[%03d] = %c\n",i,
               ((DevVarCharArray *)argout)->sequence[i]);
         break;
      case D_VAR_STRINGARR:
         for(i=0; i<((DevVarStringArray *)argout)->length; i++)
            printf("ARR[%03d] = %s\n",i,
               ((DevVarStringArray *)argout)->sequence[i]);
         break;
      case D_VAR_USHORTARR:
         for(i=0; i<((DevVarUShortArray *)argout)->length; i++)
            printf("ARR[%03d] = %hu\n",i,
               ((DevVarUShortArray *)argout)->sequence[i]);
         break;
      case D_VAR_SHORTARR:
         for(i=0; i<((DevVarShortArray *)argout)->length; i++)
            printf("ARR[%03d] = %hd\n",i,
               ((DevVarShortArray *)argout)->sequence[i]);
         break;
      case D_VAR_ULONGARR:
         for(i=0; i<((DevVarULongArray *)argout)->length; i++)
            printf("ARR[%03d] = %u\n",i,
               ((DevVarULongArray *)argout)->sequence[i]);
         break;
      case D_VAR_LONGARR:
         for(i=0; i<((DevVarLongArray *)argout)->length; i++)
            printf("ARR[%03d] = %d\n",i,
               ((DevVarLongArray *)argout)->sequence[i]);
         break;
      case D_VAR_FLOATARR:
         for(i=0; i<((DevVarFloatArray *)argout)->length; i++)
            printf("ARR[%03d] = %g\n",i,
               ((DevVarFloatArray *)argout)->sequence[i]);
         break;
      case D_VAR_DOUBLEARR:
         for(i=0; i<((DevVarDoubleArray *)argout)->length; i++)
            printf("ARR[%03d] = %g\n",i,
               ((DevVarDoubleArray *)argout)->sequence[i]);
         break;

      case D_VAR_FRPARR:
         for(i=0; i<((DevVarFloatReadPointArray *)argout)->length; i++)
         {
            printf("ARR[%03d].set  = %g\n",i,
               ((DevVarFloatReadPointArray *)argout)->sequence[i].set);
            printf("ARR[%03d].read = %g\n",i,
               ((DevVarFloatReadPointArray *)argout)->sequence[i].read);
         }
         break;
      case D_VAR_SFRPARR:
         for(i=0; i<((DevVarStateFloatReadPointArray *)argout)->length; i++)
         {
            printf("ARR[%03d].state = %hd\n",i,
               ((DevVarStateFloatReadPointArray *)argout)->sequence[i].state);
            printf("ARR[%03d].set   = %g\n",i,
               ((DevVarStateFloatReadPointArray *)argout)->sequence[i].set);
            printf("ARR[%03d].read  = %g\n",i,
               ((DevVarStateFloatReadPointArray *)argout)->sequence[i].read);
         }
         break;
      case D_VAR_LRPARR:
         for(i=0; i<((DevVarLongReadPointArray *)argout)->length; i++)
         {
            printf("ARR[%03d].set  = %ld\n",i,
               ((DevVarLongReadPointArray *)argout)->sequence[i].set);
            printf("ARR[%03d].read = %ld\n",i,
               ((DevVarLongReadPointArray *)argout)->sequence[i].read);
         }
         break;
   }
}

void exec_cmd(long cmd,char *cmd_name,int argin_type,void *arg_in,
   int argout_type)
{
   void *arg_out=NULL;
   DevLong error=0;
   struct timeval t0;
   struct timeval t1;

   static DevBoolean                     argout_boolean=0;
   static DevUShort                      argout_ushort=0;
   static DevShort                       argout_short=0;
   static DevULong                       argout_ulong=0;
   static DevLong                        argout_long=0;
   static DevFloat                       argout_float=0.0;
   static DevDouble                      argout_double=0.0;
   static DevString                      argout_string=NULL;

   static DevIntFloat                    argout_intfloat={0,0.};
   static DevFloatReadPoint              argout_floatread={0.,0.};
   static DevStateFloatReadPoint         argout_statefloatread={0,0.,0.};
   static DevLongReadPoint               argout_longread={0,0};
   static DevDoubleReadPoint             argout_doubleread={0.,0.};

   static DevVarCharArray                argout_chararr={0,NULL};
   static DevVarStringArray              argout_stringarr={0,NULL};
   static DevVarUShortArray              argout_ushortarr={0,NULL};
   static DevVarShortArray               argout_shortarr={0,NULL};
   static DevVarULongArray               argout_ulongarr={0,NULL};
   static DevVarLongArray                argout_longarr={0,NULL};
   static DevVarFloatArray               argout_floatarr={0,NULL};
   static DevVarDoubleArray              argout_doublearr={0,NULL};

   static DevVarFloatReadPointArray      argout_floatreadarr={0,NULL};
   static DevVarStateFloatReadPointArray argout_statefloatreadarr={0,NULL};
   static DevVarLongReadPointArray       argout_longreadarr={0,NULL};

   switch(argout_type) {
      case D_VOID_TYPE:             arg_out = NULL; break;
      case D_BOOLEAN_TYPE:          arg_out = &argout_boolean; break;
      case D_USHORT_TYPE:           arg_out = &argout_ushort; break;
      case D_SHORT_TYPE:            arg_out = &argout_short; break;
      case D_ULONG_TYPE:            arg_out = &argout_ulong; break;
      case D_LONG_TYPE:             arg_out = &argout_long; break;
      case D_FLOAT_TYPE:            arg_out = &argout_float; break;
      case D_DOUBLE_TYPE:           arg_out = &argout_double; break;
      case D_STRING_TYPE:           arg_out = &argout_string; break;

      case D_INT_FLOAT_TYPE:        arg_out = &argout_intfloat; break;
      case D_FLOAT_READPOINT:       arg_out = &argout_floatread; break;
      case D_STATE_FLOAT_READPOINT: arg_out = &argout_statefloatread; break;
      case D_LONG_READPOINT:        arg_out = &argout_longread; break;
      case D_DOUBLE_READPOINT:      arg_out = &argout_doubleread; break;

      case D_VAR_CHARARR:           arg_out = &argout_chararr; break;
      case D_VAR_STRINGARR:         arg_out = &argout_stringarr; break;
      case D_VAR_USHORTARR:         arg_out = &argout_ushortarr; break;
      case D_VAR_SHORTARR:          arg_out = &argout_shortarr; break;
      case D_VAR_ULONGARR:          arg_out = &argout_ulongarr; break;
      case D_VAR_LONGARR:           arg_out = &argout_longarr; break;
      case D_VAR_FLOATARR:          arg_out = &argout_floatarr; break;
      case D_VAR_DOUBLEARR:         arg_out = &argout_doublearr; break;

      case D_VAR_FRPARR:            arg_out = &argout_floatreadarr; break;
      case D_VAR_SFRPARR:           arg_out = &argout_statefloatreadarr; break;
      case D_VAR_LRPARR:            arg_out = &argout_longreadarr; break;

      default:
         printf("Sorry but %s isn't supported.\n",Type_Str(argout_type));
         return;
   }
   
   gettimeofday(&t0,NULL);

   if(dev_putget(ds,cmd,arg_in,argin_type,arg_out,argout_type,&error) < 0)
   {
      printf("Execution error [%d]: %s\n",error,dev_error_str(error));
      return;
   }
   
   gettimeofday(&t1,NULL);

   if(argout_type == D_VOID_TYPE ) {
      printf("%s OK\n",cmd_name);
   } else {
      printf("\n");
      display_argout(cmd,argout_type,arg_out);
   }
   
   printf("\nCommand execution time: %.1f ms\n" , 
     (((double)t1.tv_sec + (double)t1.tv_usec/1.0e6) -
     ((double)t0.tv_sec + (double)t0.tv_usec/1.0e6))*1000.0);

   /*
    * Free the input argument buffers that have been allocated in get_argin().
    */
   switch(argin_type) {
      case D_STRING_TYPE: 
         free(*(char **)arg_in);
         break;
      case D_VAR_CHARARR: 
      case D_VAR_STRINGARR:
      case D_VAR_USHORTARR: 
      case D_VAR_SHORTARR: 
      case D_VAR_ULONGARR:   
      case D_VAR_LONGARR:   
      case D_VAR_FLOATARR: 
      case D_VAR_DOUBLEARR:
      case D_VAR_FRPARR:
      case D_VAR_SFRPARR:
      case D_VAR_LRPARR:
      /*
       * The general structure of all these types is
       *
       * {u_int  length; (some data type) *sequence}
       *
       * The "sequence" pointer will be freed by the call below.
       */
         free((char *)arg_in + sizeof(u_int));
   }

   /*
    * Free the output argument buffers that have been allocated by the device
    * server routines.
    */
   switch(argout_type) {
      case D_STRING_TYPE: 
      case D_VAR_CHARARR: 
      case D_VAR_STRINGARR:
      case D_VAR_USHORTARR: 
      case D_VAR_SHORTARR: 
      case D_VAR_ULONGARR:   
      case D_VAR_LONGARR:   
      case D_VAR_FLOATARR: 
      case D_VAR_DOUBLEARR:
      case D_VAR_FRPARR:
      case D_VAR_SFRPARR:
      case D_VAR_LRPARR:
         dev_xdrfree(argout_type,arg_out,&error);
   }

}

/**********************************************************************/
/*    Main                                                            */
/**********************************************************************/
int main(int argc,char **argv)
{
 	char *dev_name;
	DevLong error = 0;
	int i=0;
	int useTcp = 0;
	int cmd;
	DevVarCmdArray cmd_arr = {0,NULL};
	void *argin;
	int ret_argin;

	extern int      optopt;
	extern int      optind;
	extern char     *optarg;
	int             c;
/*
 * Argument test and device name structure
 */
        while ((c = getopt(argc,argv,"hvn:t")) != -1)
        {
                switch (c)
                {
			case 't':
     				useTcp = 1;     
				break;
                        case 'n':
                                taco_setenv("NETHOST", optarg, 1);
                                break;
                        case 'v':
                                version(argv[0]);
                                break;
                        case 'h':
                        case '?':
                                usage(argv[0]);
                }
        }
        if (optind != argc - 1)
                usage(argv[0]);

   /*
    *  import the device
    */
   dev_name = argv[optind];
   printf("\nImporting %s \n",dev_name);
   

   if(dev_import(dev_name,0,&ds,&error) < 0)
   {
	printf("** db_import : %s **\n", dev_error_str(error)+25);
   }
   
   
   if(useTcp) {
     printf("Setting TCP protocol\n");
     if( dev_rpc_protocol(ds,D_TCP,&error) < 0) 
     {
       dev_printerror_no(SEND,"dev_rpc_protocol()",error);
       exit(-1);
     }
   }

   /*
    *  Build the command array structure
    */
   printf("Get device commands \n");
   if(dev_cmd_query(ds,&cmd_arr,&error) < 0)
   {
      dev_printerror_no(SEND,"dev_cmd_query()",error);
      exit(-1);
   }

   while(!use_signal && i<cmd_arr.length)
   {
      use_signal = (cmd_arr.sequence[i].cmd == DevReadSigValues);
      i++;
   }

   if(use_signal)
   {
      printf("Get signal list \n");
      
      if( strncasecmp( dev_name , "tango:" , 6 )==0 ) {
      
        /* Get configuration from the device */        
        if(dev_putget(ds,DevGetSigConfig,
		     NULL,D_VOID_TYPE,
	             &sig_config,D_VAR_STRINGARR,
	             &error) < 0)
        {
          dev_printerror_no(SEND,"Warning : DevGetSigConfig()",error);
          use_signal=0;
        }
      
      } else {
      
        /* Get configuration from the database */
        if(dev_get_sig_config(dev_name,&sig_config,&error) < 0)
        {
          dev_printerror_no(SEND,"Warning : dev_get_sig_config()",error);
          use_signal=0;
        }
	
      }
            
   }

   printf("\nPress [Enter] to continue");
   rewind(stdin);
   getchar();
   system("/usr/bin/clear");

   /*
    * Main menu
    */

   while(1)
   {
      printf("\nExecute one of the following commands on %s: \n\n",dev_name);
     
      printf("\t 0. Quit\n");
      printf("\t 1. Command query\n");
      for(i=0; i<cmd_arr.length; i++)
      {
         printf("\t%2d. %s\n",i+2,cmd_arr.sequence[i].cmd_name);
      }

      printf("\nExecute command number ? ");

      /*
       * to get around the strange effects of scanf() wait for something read 
       */
      rewind(stdin);
      scanf("%d",&cmd);

      if(cmd != 0)
      {
      
         if( cmd==1 ) {
	 
	   /* Command query */
           printf("\n");
           printf("=====================================================\n");
           for(i=0; i<cmd_arr.length; i++)
           {
              printf("%9d: %20s (%s,%s)\n",cmd_arr.sequence[i].cmd,
	             cmd_arr.sequence[i].cmd_name,
		     Type_Str(cmd_arr.sequence[i].in_type),
                     Type_Str(cmd_arr.sequence[i].out_type));
           }
           printf("=====================================================\n");
	   
	 
	 } else {
	 
           ret_argin=1;
           argin=NULL;

           printf("\n");
           printf("=====================================================\n");
           printf("= Cmd %s(%s,%s) [code=%d] \n",cmd_arr.sequence[cmd-2].cmd_name,
             Type_Str(cmd_arr.sequence[cmd-2].in_type),
             Type_Str(cmd_arr.sequence[cmd-2].out_type),
	     cmd_arr.sequence[cmd-2].cmd);
           printf("=====================================================\n");

           if(cmd_arr.sequence[cmd-2].in_type != D_VOID_TYPE)
           {
             ret_argin=get_argin(cmd_arr.sequence[cmd-2].in_type,&argin);
           }

           if(ret_argin)
             exec_cmd(cmd_arr.sequence[cmd-2].cmd,
               cmd_arr.sequence[cmd-2].cmd_name,
               cmd_arr.sequence[cmd-2].in_type,argin,
               cmd_arr.sequence[cmd-2].out_type);
      
           printf("=====================================================\n");
	 
	 }
	 
      } else {

         if(dev_free(ds,&error) < 0)
         {
            dev_printerror_no(SEND,"Dev_free()",error);
            exit(-1);
         }

         exit(0);
      }
   }
}
