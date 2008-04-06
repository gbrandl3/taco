/*********************************************************************
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
 *
 * File: 	ps_menu.c 
 * 
 * Project: 	Device Servers 
 * 
 * Description: 	Code for a menu driven test program for AGPowerSupplies.
 * 		Allows each command to be executed on a given device. 
 * 		Device name is specified on the command line.
 * 
 * Author(s): 	A. Goetz 
 * 
 * Original: 	March 1991 
 * 
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2008-04-06 09:06:35 $
 * 
 *********************************************************************/ 
#include <API.h> 
#include <DevServer.h>

/*
 * include AGPowerSupply public file to get DevRemote command definition 
 */ 
#include "AGPowerSupply.h"

void dev_perror(long *error)
{
	if (error)	
		printf("%s\n", dev_error_str(*error));
}

int main(int argc, char **argv) 
{
	devserver ps; 
	DevArgument 	arg; 
	long 	readwrite = 0; 
	DevLong	error; 
	int 	cmd, 
		status, 
		nave, 
		chan; 
	float 	setcurrent, 
		setvoltage; 
	DevFloatReadPoint 	readcurrent, 
				readvoltage; 
	DevStateFloatReadPoint 	statereadpoint; 
	short 	devstatus; 
	char 	*ch_ptr,
		cmd_string[256];

	if (argc < 2)
	{
		printf("usage: %s device-name\n",argv[0]); 
		exit(1); 
	}

	status = dev_import(argv[1],readwrite,&ps,&error); 
	printf("dev_import(%s) returned %d\n",argv[1],status);

	if (status != 0) exit(1); 

	while (1) 
	{
		printf("Select one of the following commands : \n\n"); 
		printf("0. Quit\n\n"); 
		printf("1. On 2. Off 3. State\n"); 
		printf("4. Status 5. Set 6. Read\n"); 
		printf("7. Update 8. Local 9. Remote\n"); 
		printf("10.Error 11.Reset\n\n");

		printf("cmd ? "); 
/*
 * to get around the strange effects of scanf() wait for something read 
 */
		for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; ); 
		status = sscanf(cmd_string,"%d",&cmd);

		switch (cmd)
		{  
			case (1) : 
				status = dev_putget(ps,DevOn,NULL,D_VOID_TYPE,NULL,
							D_VOID_TYPE,&error); 
				printf("\nDevOn dev_put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (2) : 
				status = dev_putget(ps,DevOff,NULL,D_VOID_TYPE,NULL,
							D_VOID_TYPE,&error); 
				printf("\nDevOff dev_put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (3) : 
				status = dev_putget(ps,DevState,NULL,D_VOID_TYPE,
							&devstatus,D_SHORT_TYPE,&error); 
				printf("\nDevState dev.putget() returned %d\n",status); 
				if (status == 0) 
				{
					printf("status read %d , %s \n",devstatus,DEVSTATES[devstatus]); 
				}
				break;

			case (4) : 
				status = dev_putget(ps,DevStatus,NULL,D_VOID_TYPE,
							&ch_ptr,D_STRING_TYPE,&error); 
				printf("\nDevStatus dev.putget() returned %d\n",status); 
				if (status == 0)
				{
					printf(" %s\n",ch_ptr); 
				}
				break;

			case (9) : 
				status = dev_put(ps,DevRemote,NULL,D_VOID_TYPE,&error);
				printf("\nDevRemote dev_put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (5) : 
				printf("set current to ? ");
				for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; ); 
				sscanf(cmd_string,"%f,",&setcurrent); 
				status = dev_put(ps,DevSetValue,&setcurrent,D_FLOAT_TYPE,&error); 
				printf("\nDevSetValue dev_putget() returned %d, ",status); 
				printf("current should be set to %6.2f amps\n",setcurrent); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (6) : 
				status = dev_putget(ps,DevReadValue,NULL,D_VOID_TYPE,
							&readcurrent,D_FLOAT_READPOINT,&error); 
				printf("\nDevReadValue dev_putget() returned %d, ",status); 
				printf("current set to %6.3f read %6.3f\n",readcurrent.set,
						readcurrent.read); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (11) : 
				status = dev_put(ps,DevReset,NULL,D_VOID_TYPE,&error);
				printf("\nDevReset dev_put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (10) : 
				status = dev_put(ps,DevError,NULL,D_VOID_TYPE,&error);
				printf("\nDevError dev.put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (8) : 
				status = dev_put(ps,DevLocal,NULL,D_VOID_TYPE,&error);
				printf("\nDevLocal dev_put() returned %d\n",status); 
				if (status < 0) dev_perror(NULL); 
				break;

			case (7) : 
				status = dev_putget(ps,DevUpdate,NULL,D_VOID_TYPE,
						&statereadpoint,D_STATE_FLOAT_READPOINT,&error); 
				printf("\nDevUpdate devputget() returned %d (error %d)\n",status,error); 
				if (status >= 0)
				{
					printf("status read %d , %s \n",statereadpoint.state,
							DEVSTATES[statereadpoint.state]); 
					printf("current set to %6.3f read %6.3f\n",statereadpoint.set,
							statereadpoint.read); 
				}
				break;

			case (0) : 
				dev_free(ps,&error);
				exit(0);

			default : 
				break; 
		}
	}
}
