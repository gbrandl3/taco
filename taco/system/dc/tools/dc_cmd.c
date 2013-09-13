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
 * File         : dc_cmd.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author       :
 *                $Author: jkrueger1 $
 *
 * Original     :
 *
 * Version      : $Revision: 1.4 $
 *
 * Date         : $Date: 2008-04-06 09:07:50 $
 *
 */


#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#include <API.h>
#include <DevErrors.h>
#include <Admin.h>
#include <pss_xdr.h>
#include <SysNumbers.h>
#include <dc.h>

/**********************************/
/* Return the command name of cmd */
/**********************************/
char *GetCmdName(long cmd)
{
  static char ret[30];

  char            res_path[LONG_NAME_SIZE];
  char            res_name[SHORT_NAME_SIZE];
  char            *ret_str = NULL;
  db_resource     res_tab;
  unsigned long   cmd_number_mask = 0x3ffff;
  unsigned short  team;
  unsigned short  server;
  unsigned short  cmds_ident;
  DevLong error;

 /*
  * Decode the command nuber into the fields:
  * team, server and cmds_ident.
  */

  team   = (_Int)(cmd >> DS_TEAM_SHIFT);
  team   = team & DS_TEAM_MASK;
  server = (_Int)(cmd >> DS_IDENT_SHIFT);
  server = server & DS_IDENT_MASK;
  cmds_ident = (_Int)(cmd & cmd_number_mask);
 
#ifdef TANGO 
  if( team == DevTangoBase ) {  
    sprintf(ret,"TangoCommand%d",cmds_ident);    
    return ret;  
  }
#endif

  sprintf(res_path, "CMDS/%d/%d", team, server);
  sprintf (res_name, "%d", cmds_ident);

  res_tab.resource_name = res_name;
  res_tab.resource_type = D_STRING_TYPE;
  res_tab.resource_adr  = &ret_str;

  if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
  {
    sprintf(ret,"UnknownCmd[%d]",cmd);
    return ret;
  }

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */

  if ( ret_str == NULL )
  {
    sprintf(ret,"UnknownCmd[%d]",cmd);
    return ret;
  }

  sprintf(ret,"%s",ret_str);
  free (ret_str);
  return ret; 
}

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
char *T26="D_PSS_STATUS";
char *T29="Unknown type";

char *Type_Str(long t) {
switch(t) {
case D_VOID_TYPE                : return T0;break;
case D_BOOLEAN_TYPE             : return T1;break;
case D_USHORT_TYPE              : return T2;break;
case D_SHORT_TYPE               : return T3;break;
case D_ULONG_TYPE               : return T4;break;
case D_LONG_TYPE                : return T5;break;
case D_FLOAT_TYPE               : return T6;break;
case D_DOUBLE_TYPE              : return T7;break;
case D_STRING_TYPE              : return T8;break;
case D_INT_FLOAT_TYPE           : return T9;break;
case D_FLOAT_READPOINT          : return T10;break;
case D_STATE_FLOAT_READPOINT    : return T11;break;
case D_LONG_READPOINT           : return T12;break;
case D_DOUBLE_READPOINT         : return T13;break;
case D_VAR_CHARARR              : return T14;break;
case D_VAR_STRINGARR            : return T15;break;
case D_VAR_USHORTARR            : return T16;break;
case D_VAR_SHORTARR             : return T17;break;
case D_VAR_ULONGARR             : return T18;break;
case D_VAR_LONGARR              : return T19;break;
case D_VAR_FLOATARR             : return T20;break;
case D_VAR_DOUBLEARR            : return T21;break;
case D_VAR_FRPARR               : return T22;break;
case D_VAR_SFRPARR              : return T23;break;
case D_VAR_LRPARR               : return T24;break;
case D_OPAQUE_TYPE              : return T25;break;
case D_PSS_STATUS               : return T26;break;
default                         : return T29;break;
}
}


/*******************************************************************/
/* Main function                                                   */
/*******************************************************************/

int main(int argc,char *argv[])
{

  dc_dev_imp dc_imp;
  dc_devinf  dc_inf;
  DevLong error=0;
  DevLong dc_error=0;
  int ret;
  int i,j;
  char c[BUFSIZ];

  DevShort  arg_short;
  DevUShort arg_ushort;
  DevLong   arg_long;
  DevULong  arg_ulong;
  DevFloat  arg_float;
  DevDouble arg_double;
  DevStateFloatReadPoint arg_stfloat;
 
  DevVarShortArray   arg_shortarr;
  DevVarUShortArray  arg_ushortarr;
  DevVarLongArray    arg_longarr;
  DevVarULongArray   arg_ulongarr;
  DevVarFloatArray   arg_floatarr;
  DevVarDoubleArray  arg_doublearr;
  DevDaresburyStatus arg_pss;
  DevString          arg_string;

  setbuf(stdin, (char *)NULL);

  if( argc!=2 ) {
    printf("Usage : dc_cmd device_name\n");
    exit(0);
  }
  
  /* Import PSS type for Daresbury */
  LOAD_PSS_STATUS(&error);
  if( error ) {
    printf("Warning: Failed to load PSS xdr type:%s\n",dev_error_str(error));
    error = 0;
  }
  /* Fill the dc import structure */

  dc_imp.device_name = argv[1];
  dc_imp.dc_dev_error=&dc_error;


  /*****************************************/
  /* DC importing device                   */
  /*****************************************/

  ret = dc_import( &dc_imp , 1 , &error );

  if( ret < 0 ) {
    printf("dc_import %s : %s\n",argv[1],dev_error_str(error));
    exit(0);
  }

  if( ret > 0 ) {
    printf("dc_import %s : %s\n",argv[1],dev_error_str(dc_error));
    exit(0);
  }

  /*********************************************/
  /* Get the polled command list               */
  /*********************************************/

  if ( dc_dinfo (argv[1] , &dc_inf , &error) < 0 )
  {
    printf("dc_dinfo %s : %s\n",argv[1],dev_error_str(error));
    exit(0);
  }


  j=1;
  while( j!=0 ) {

    printf("\n===========================================\n");
    printf("List of DC polled commands\n\n");
    printf("  0) Quit\n");

    for( i=0 ; i < dc_inf.devinf_nbcmd ; i++ )
    {
      printf("  %d) %s %s\n",i+1,GetCmdName(dc_inf.devcmd[i].devinf_cmd),
                             Type_Str(dc_inf.devcmd[i].devinf_argout));
    }

    printf("\n Enter command to execute:");
    fgets(c, sizeof(c), stdin);
    j=atoi(c);
    
    printf("\n");
    if(j>dc_inf.devinf_nbcmd) printf("Value out of range\n");
    else {
      if(j<=0) j=0;
      else {
	switch( dc_inf.devcmd[j-1].devinf_argout ) {

	  case D_FLOAT_TYPE:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_float,D_FLOAT_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %g \n",arg_float);
	    }
	    break;
	  
	  case D_ULONG_TYPE:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_ulong,D_ULONG_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %u \n",arg_ulong);
	    }
	    break;

	  case D_LONG_TYPE:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_long,D_LONG_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %d \n",arg_long);
	    }
	    break;

	  case D_USHORT_TYPE:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_ushort,D_USHORT_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %u \n",arg_ushort);
	    }
	    break;

	  case D_SHORT_TYPE:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_short,D_SHORT_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %d \n",arg_short);
	    }
	    break;

	  case D_STATE_FLOAT_READPOINT:

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_stfloat,D_STATE_FLOAT_READPOINT,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("State = %d \n",arg_stfloat.state);
	      printf("Set   = %g \n",arg_stfloat.set);
	      printf("Read  = %g \n",arg_stfloat.read);
	    }
	    break;

	  case D_VAR_USHORTARR:

	    arg_ushortarr.length=0;
	    arg_ushortarr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_ushortarr,D_VAR_USHORTARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_ushortarr.length;i++)
	        printf("Arr[%2d] = %u \n",i,arg_ushortarr.sequence[i]);
	    }
	    break;
	  
	  case D_VAR_SHORTARR:

	    arg_shortarr.length=0;
	    arg_shortarr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_shortarr,D_VAR_SHORTARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_shortarr.length;i++)
	        printf("Arr[%2d] = %d \n",i,arg_shortarr.sequence[i]);
	    }
	    break;
	  
	  case D_VAR_ULONGARR:

	    arg_ulongarr.length=0;
	    arg_ulongarr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_ulongarr,D_VAR_ULONGARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_ulongarr.length;i++)
	        printf("Arr[%2d] = %d \n",i,arg_ulongarr.sequence[i]);
	    }
	    break;

	  case D_VAR_LONGARR:

	    arg_longarr.length=0;
	    arg_longarr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_longarr,D_VAR_LONGARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_longarr.length;i++)
	        printf("Arr[%2d] = %d \n",i,arg_longarr.sequence[i]);
	    }
	    break;

	  case D_VAR_FLOATARR:

	    arg_floatarr.length=0;
	    arg_floatarr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_floatarr,D_VAR_FLOATARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_floatarr.length;i++)
	        printf("Arr[%2d] = %g \n",i,arg_floatarr.sequence[i]);
	    }
	    break;

	  case D_DOUBLE_TYPE:
            
	    if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_double,D_DOUBLE_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	      printf("Value = %g \n",arg_double);
	    }
	    break;
	  
	  case D_STRING_TYPE:

	    arg_string=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_string,D_STRING_TYPE,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	        printf("String =\n %s \n",arg_string);
	    }
	    break;
	  
	  case D_VAR_DOUBLEARR:

	    arg_doublearr.length=0;
	    arg_doublearr.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_doublearr,D_VAR_DOUBLEARR,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
              for(i=0;i<arg_doublearr.length;i++)
	        printf("Arr[%2d] = %g \n",i,arg_doublearr.sequence[i]);
	    }
	    break;
	    
	  case D_PSS_STATUS:

	    arg_pss.length=0;
	    arg_pss.sequence=NULL;

            if(  dc_devget(dc_imp.dc_ptr,dc_inf.devcmd[j-1].devinf_cmd,
		 &arg_pss,D_PSS_STATUS,&error)<0 )
            {
	      printf("%s : %s\n",GetCmdName(dc_inf.devcmd[j-1].devinf_cmd),
				 dev_error_str(error));
            } else {
	    
              for(i=0;i<arg_pss.length;i++) {
	        printf("Arr[%2d].Module = %d \n",i,arg_pss.sequence[i].Module);
	        printf("Arr[%2d].GuardA = %d \n",i,arg_pss.sequence[i].GuardA);
	        printf("Arr[%2d].GuardB = %d \n",i,arg_pss.sequence[i].GuardB);
	        printf("Arr[%2d].GuardC = %d \n",i,arg_pss.sequence[i].GuardC);
	        printf("Arr[%2d].Itlk   = %04X \n",i,arg_pss.sequence[i].Interlocks);
	      }
	      
	    }
	    break;

	  default:
	    printf("%s not supported\n",
		   Type_Str(dc_inf.devcmd[j-1].devinf_argout));
	
	}
      }
    }
  }
  
  return 0;
}
