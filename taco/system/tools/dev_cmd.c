/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2008 by European Synchrotron Radiation Facility,
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
 * File         : dev_cmd.c
 *
 * Project      : TACO tools
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
 * Date         : $Date: 2008-10-14 09:52:29 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/**************************************************/
/* dev_cmd_query in shell                         */
/*  Usage : dev_cmd_query dev_name                */
/**************************************************/
#include <API.h>
#include <DevErrors.h>
#include <DevServer.h>

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

char *Type_Str(long t) {
switch(t) {
case D_VOID_TYPE		: return T0;break;
case D_BOOLEAN_TYPE		: return T1;break;
case D_USHORT_TYPE		: return T2;break;
case D_SHORT_TYPE		: return T3;break;
case D_ULONG_TYPE		: return T4;break;
case D_LONG_TYPE		: return T5;break;
case D_FLOAT_TYPE		: return T6;break;
case D_DOUBLE_TYPE		: return T7;break;
case D_STRING_TYPE		: return T8;break;
case D_INT_FLOAT_TYPE		: return T9;break;
case D_FLOAT_READPOINT		: return T10;break;
case D_STATE_FLOAT_READPOINT	: return T11;break;
case D_LONG_READPOINT		: return T12;break;
case D_DOUBLE_READPOINT		: return T13;break;
case D_VAR_CHARARR		: return T14;break;
case D_VAR_STRINGARR		: return T15;break;
case D_VAR_USHORTARR		: return T16;break;
case D_VAR_SHORTARR		: return T17;break;
case D_VAR_ULONGARR		: return T18;break;
case D_VAR_LONGARR		: return T19;break;
case D_VAR_FLOATARR		: return T20;break;
case D_VAR_DOUBLEARR		: return T21;break;
case D_VAR_FRPARR		: return T22;break;
case D_VAR_SFRPARR		: return T23;break;
case D_VAR_LRPARR		: return T24;break;
case D_OPAQUE_TYPE		: return T25;break;
default				: return T29;break;
}
}

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options] <device name>\n", cmd);
        fprintf(stderr, " dev_cmd_query in shell of the device\n");
        fprintf(stderr, "         options: -h display this message\n");
        fprintf(stderr, "                  -n nethost\n");
        fprintf(stderr, "                  -v display the current version\n");
        exit(1);
}

void version(const char *cmd)
{
        fprintf(stderr, "%s version %s\n", cmd,  VERSION);
        exit(0);
}

int main(int argc,char **argv)
{
	devserver ds;
	DevVarCmdArray    DevCmd;
	DevLong error=0;
	int i;
	int ok=0;
	char *NoDes="No description";

	extern int      optopt;
	extern int      optind;
	extern char     *optarg;
	int             c;
/*
 * Argument test and device name structure
 */
        while ((c = getopt(argc,argv,"hvn:")) != -1)
        {
                switch (c)
                {
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

  printf("\n");

  if( db_import(&error) < 0 ) {
      printf("db_import() failed : %s\n",dev_error_str(error));
      exit(0);
  }

   if ( dev_import(argv[optind],0,&ds,&error) < 0 )
   {
      printf("Error : %s\n",dev_error_str(error));
      exit(0);
   }

   while(!ok) {
     if( dev_cmd_query(ds,&DevCmd,&error) <0 )
     {
       printf("Error : %s\n",dev_error_str(error));
       sleep(1);
     } else {
       ok=1;
     }
   }

   printf("NETHOST=%s\n",getenv("NETHOST"));
   printf("command(IN_TYPE,OUT_TYPE) [Cmd code]\n\n");

   for(i=0;i<DevCmd.length;i++) {
     printf("%s(%s,%s) [%d]\n",DevCmd.sequence[i].cmd_name,
     			       Type_Str(DevCmd.sequence[i].in_type),
                               Type_Str(DevCmd.sequence[i].out_type),
			       DevCmd.sequence[i].cmd);

   }

   dev_free(ds,&error);
}

