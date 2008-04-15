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
 * File         : devreset.c
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
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2008-04-15 08:41:55 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/*
* Command shell to execute the DevReset command on one or several devices
* 
* nearly all devices accept the DevReset command with void parameters
* Except PulsedRelay class where a long is requested as argument.
* If this program is called with 1 arguments, it will send DevReset(void,void) 
* else it will send DevReset(long,void) to a presumed PulsedRelay device
************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <stdio.h>

#ifdef _solaris
#include <taco_utils.h>
#endif /* _solaris */

long devcmd(char *devname);
long resetpulsedrelay(char *devname, long resettime);

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options] <device name>\n", cmd);
	fprintf(stderr, " control of one or several devices\n");
	fprintf(stderr, " device name can be sr/v-*/c29*\n");
	fprintf(stderr, " in case of several devices, the function ask confirmation\n");
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

int main(int argc,char *argv[])
{
	int status,i;
	char **device_tab;
	unsigned int dev_nb;
	DevLong error;
	long resettime;
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
                                setenv("NETHOST", optarg, 1);
                                break;
                        case 'v':
                                version(argv[0]);
                                break;
                        case 'h':
                        case '?':
                                usage(argv[0]);
                }
        }
        if (optind > argc - 1 || optind < argc - 2)
                usage(argv[0]);

   if(db_import(&error)) 
   {
	   printf("Error during db_import\n");
	   exit(0);
   }
   if(optind == argc - 2) 
	resettime=atoi(argv[optind + 1]);
   if(strchr(argv[optind],'*')==0)
   {
	if(optind == argc - 2) 
		resetpulsedrelay(argv[optind],resettime);
	else      
		devcmd(argv[optind]);
   }
   else
   {
   status=db_getdevexp(argv[optind],&device_tab,&dev_nb,&error);
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
	if(argc == 3) 
		resetpulsedrelay(device_tab[i],resettime);
	else      
		devcmd(device_tab[i]);
   }
   db_freedevexp(device_tab);
   }
}
long devcmd(char *devname)
{
   long 	status;
   DevLong	error;
   char 	cmd_string[80];
   devserver 	ds;
   status= dev_import(devname,0,&ds,&error);
   if(status<0)
   {
      printf("** import %s: %s **\n",devname,dev_error_str(error)+25);
      return(-1);
   }
/*
* printf("Are you sure you want to reset %s (y or n  (q to exit) ? ",devname);
   while((status = fscanf(stdin,"%s",cmd_string)) <= 0);
   if(cmd_string[0]=='y')
   {
   */
      status= dev_putget(ds,DevReset,NULL,D_VOID_TYPE,NULL,D_VOID_TYPE,&error);
      if(status<0) printf("** %s: %s **\n",devname,dev_error_str(error)+25);
      else printf(" %s reseted \n",devname);
/*
   }
   */
   dev_free(ds,&error);
   if(cmd_string[0]=='q') exit(0);
}
long resetpulsedrelay(char *devname, long resettime)
{
   long 	status;
   DevLong	error;
   char 	cmd_string[80];
   devserver 	ds;

   status= dev_import(devname,0,&ds,&error);
   if(status<0)
   {
      printf("** import %s: %s **\n",devname,dev_error_str(error)+25);
      return(-1);
   }
/*
* printf("Are you sure you want to reset %s (y or n  (q to exit) ? ",devname);
   while((status = fscanf(stdin,"%s",cmd_string)) <= 0);
   if(cmd_string[0]=='y')
   {
   */
      status= dev_putget(ds,DevReset,&resettime,D_LONG_TYPE,NULL,D_VOID_TYPE,&error);
      if(status<0) printf("** %s: %s **\n",devname,dev_error_str(error)+25);
      else printf(" %s reseted \n",devname);
/*
   }
   */
   dev_free(ds,&error);
   if(cmd_string[0]=='q') exit(0);
}
