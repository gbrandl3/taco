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
 * File         : devread.c
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
 * Version      : $Revision: 1.9 $
 *
 * Date         : $Date: 2008-10-24 15:41:57 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <TACOBasicErrors.h>
#include <stdio.h>

#define TACODevelTeamNumber DS_LSHIFT(9,DS_TEAM_SHIFT)
#define TACO_COMMAND_READ_LONG (TACODevelTeamNumber + 68L)
#define TACO_COMMAND_READ_DOUBLE (TACODevelTeamNumber + 83L)
#define TACO_COMMAND_READ_U_LONG (TACODevelTeamNumber + 88L)

#include <taco_utils.h>

long devcmd(char *devname);

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

int main(int argc, char **argv)
{
   int status,i;
   char **device_tab;
   unsigned int dev_nb;
   DevLong error;
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

	if(db_import(&error)) 
	{
		printf("** db_import : %s **\n", dev_error_str(error)+25);
	}

	if(index(argv[optind],'*')==0)
		devcmd(argv[optind]);
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
		printf("%-32s  %s\n", "device name", "value");
		for(i=0;i<dev_nb;i++)
		{
		devcmd(device_tab[i]);
		}
		db_freedevexp(device_tab);
	}
}

long devcmd(char *devname)
{
	long 	status,
		long_state;
	DevLong error;
	char cmd_string[80];
	double value;
	devserver ds;
	status= dev_import(devname,0,&ds,&error);
	if(status<0)
	{
		printf("** import %s: %s **\n",devname,dev_error_str(error)+25);
		return(-1);
	}
	status= dev_putget(ds,DevRead,NULL,D_VOID_TYPE,&value,D_DOUBLE_TYPE,&error);
	if(status==0)
	{
		printf("%-32s: %.3g\n",devname,value);
	}
	else if (error == DevErr_RuntimeError)
	{
		status = dev_putget(ds, TACO_COMMAND_READ_DOUBLE, NULL, D_VOID_TYPE, &value, D_DOUBLE_TYPE, &error);
		if (status == 0)
		{
			printf("%-32s: %g\n", devname, value);
		}
		else if (error == DevErr_RuntimeError)
		{
			DevLong tmpLong;
			status = dev_putget(ds, TACO_COMMAND_READ_LONG, NULL, D_VOID_TYPE, &tmpLong, D_LONG_TYPE, &error);
			if (status == 0)
			{
				printf("%-32s: %ld\n", devname, tmpLong);
			}
			else if (error == DevErr_RuntimeError)
			{
				DevULong tmpULong;
				status = dev_putget(ds, TACO_COMMAND_READ_U_LONG, NULL, D_VOID_TYPE, &tmpULong, D_ULONG_TYPE, &error);
				if (status == 0)
				{
					printf("%-32s: %lu\n", devname, tmpULong);
				}
				else 
				{
					printf("** %s: %s **\n",devname,dev_error_str(error)+25);
				}
			}
			else 
			{
				printf("** %s: %s **\n",devname,dev_error_str(error)+25);
			}
		}
		else 
		{
			printf("** %s: %s **\n",devname,dev_error_str(error)+25);
		}
	}
	else 
	{
		printf("** %s: %s **\n",devname,dev_error_str(error)+25);
	} 
	dev_free(ds,&error);
	if(cmd_string[0]=='q') 
		exit(0);
}
