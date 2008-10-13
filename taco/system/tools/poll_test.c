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
 * File         : poll_test.c
 *
 * Project      : TACO tools
 *
 * Description  :
 *
 *
 * Author       : 
 *                $Author: andy_gotz $
 *
 * Original     :
 *
 * Version      : $Revision: 1.4 $
 *
 * Date         : $Date: 2008-10-13 19:01:46 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <stdio.h>

#ifdef _solaris
#include <taco_utils.h>
#endif /* _solaris */


db_resource pl_tab [] = {
   {"ud_poll_list",   D_VAR_STRINGARR},
   };
   long pl_tab_size=sizeof(pl_tab) / sizeof(db_resource);

long devcmd(char *devname,char *pollname);

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options]\n", cmd);
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

int main(int argc, char **argv)
{
	int 		status,i,j;
	char 		**device_tab;
	unsigned int 	dev_nb;
	DevLong 	error;
	DevVarStringArray stringarray;
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
        if (optind != argc)
                usage(argv[0]);

   if(db_import(&error)) 
   {
	   printf("Error during db_import\n");
	   exit(0);
   }
   status=db_getdevexp("sys/daemon/*",&device_tab,&dev_nb,&error);
   if (status==-1) 
   {
      printf("%s\n",dev_error_str(error));
      fflush(stdout);
      exit(0);
   }
   if(dev_nb==0) 
   {
      printf("no device has this name\n");
      exit(0);
   }
   for(i=0;i<dev_nb;i++)
   {
       pl_tab[0].resource_adr = &stringarray;

      status = db_getresource(device_tab[i],pl_tab,pl_tab_size,&error);
      if (status != DS_OK)
      {
         printf("error %d in getting ud_poll_list for %s\n",device_tab[i],error);
      }
      else 
     {
         for(j=0;j<stringarray.length;j++)
         {
            /*printf("device %s:	",stringarray.sequence[j]);*/
            devcmd(stringarray.sequence[j],device_tab[i]);
         }
    }
   }
   db_freedevexp(device_tab);
}
long devcmd(char *devname,char *pollname)
{
   long status,long_state;
   DevLong error;
   short pts;
   char cmd_string[80];
   devserver ds;
   status= dev_import(devname,0,&ds,&error);
   if(status<0)
   {
      printf("import %s (%s): %s\n",devname,pollname,dev_error_str(error)+25);
      return(-1);
   }
   status= dev_putget(ds,DevState,NULL,D_VOID_TYPE,&pts,D_SHORT_TYPE,&error);
   if(status!=0)
   {
      switch (error)
      {
	 case (DevErr_IncompatibleCmdArgumentTypes):
	 case (DevErr_CommandNotImplemented):
	     break;
         default:
      printf("putget %s (%s): %s\n",devname,pollname,dev_error_str(error)+25);
   	}
   } 
    dev_free(ds,&error);
}
