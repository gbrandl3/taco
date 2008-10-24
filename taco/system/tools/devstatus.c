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
 * File         : devstatus.c
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
 * Version      : $Revision: 1.6 $
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
#include <stdio.h>
#include <stdlib.h>
#include <TACOBasicErrors.h>

#include <taco_utils.h>

long devcmd(char *devname);

void usage(const char *cmd)
{
        fprintf(stderr, "usage : %s [options] <device name>\n", cmd);
        fprintf(stderr, " controls one or more devices\n");
        fprintf(stderr, " device name can be sr/v-*/c29*\n");
        fprintf(stderr, " in case of several devices, the function ask confirmation\n");
        fprintf(stderr, " displays the resources of the device\n");
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
	int 		status,
			i;
	char 		**device_tab;
	unsigned int 	dev_nb;
	DevLong 	error;
	long 		error_flag;
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

	if(strchr(argv[optind],'*')==0)
	{
		if(devcmd(argv[optind])!=0)
			error_flag = 1;
	}
	else
	{
		status=db_getdevexp(argv[optind],&device_tab,&dev_nb,&error);
		if (status==-1) 
		{
			printf("%s\n",dev_error_str(error));
			fflush(stdout);
			return(-1);
		}
		if(dev_nb==0) 
		{
			printf("no device has this name\n");
			return(-1);
		}
		for(i=0;i<dev_nb;i++)
		{
			if(devcmd(device_tab[i])!=0)
				error_flag = 1;
		}
		db_freedevexp(device_tab);
	}
	if(error_flag ==1) 
		return (-1);
	else 
		return(0);
}
long devcmd(char *devname)
{
	long 	status,
		long_state;
	DevLong	error;
	char  	*pts;
	char 	cmd_string[80];
	devserver 	ds;

	status= dev_import(devname,0,&ds,&error);
	if(status<0)
	{
		printf("** import %s: %s **\n",devname,dev_error_str(error)+25);
		return(-1);
	}
	pts=NULL;
	status= dev_putget(ds,DevStatus,NULL,D_VOID_TYPE,&pts,D_STRING_TYPE,&error);
	if(status==0)
	{
		printf("%s:	%s\n",devname,pts);
		dev_xdrfree(D_STRING_TYPE,&pts,&error);
	}
	else if (error == DevErr_TypeError)
	{
		DevVarCharArray ptr = {0, NULL};
		status = dev_putget(ds, DevStatus, NULL, D_VOID_TYPE, &ptr, D_VAR_CHARARR, &error);
		if (status == 0)
		{
			pts = (char *)malloc(ptr.length + 1);
			strncpy(pts, ptr.sequence, ptr.length);
			pts[ptr.length] = '\0';
			printf("%s:       %s\n",devname, pts);
			free(pts);
			dev_xdrfree(D_VAR_CHARARR,&ptr,&error);
		}	
		else
		{
			printf("** %s: %s **\n",devname,dev_error_str(error)+25);
			dev_free(ds,&error);
			return(-1);
		} 
	}
	else
	{
		printf("** %s: %s **\n",devname,dev_error_str(error)+25);
		dev_free(ds,&error);
		return(-1);
	} 
	dev_free(ds,&error);
	return(0);
}
