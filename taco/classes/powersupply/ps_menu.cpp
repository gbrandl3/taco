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
 * File:        PowerSupply.cpp
 *
 * Project:     Device Servers in C++
 *
 * Description: source code file to test the powersupply base class for device 
 *              classes in C++ (PowerSupply)
 *
 * Author(s):   Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:    April 1995
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2009-01-16 15:16:23 $
 */


#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <stdio.h>

extern long debug_flag;

int main(int argc,char **argv)
{

	devserver ps;
	long readwrite = 0;
	DevLong error;
	int cmd, status;
	float setcurrent;
	DevFloatReadPoint readcurrent;
	DevStateFloatReadPoint statereadpoint;
	short devstatus;
	char *ch_ptr,cmd_string[256];

	/*debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/

	switch (argc)	
	{
		case 1:
			printf("enter device name [\"tl1/ps-d/d\"]? ");
			if(NULL==fgets(cmd_string, sizeof(cmd_string), stdin) || '\0'==cmd_string[0])
				strcpy(cmd_string,"tl1/ps-d/d");
			else if (cmd_string[strlen(cmd_string) - 1] == '\n')
				cmd_string[strlen(cmd_string) - 1] = '\0';	
			break;
		case 2:
			strcpy(cmd_string,argv[1]);
			break;
		default:
			printf("usage: ps_menu [device name]\n");
			exit(1);
	}

	status = dev_import(cmd_string,readwrite,&ps,&error);
	printf("dev_import(%s) returned %d (error=%d)\n",cmd_string,status,error);

	if (status != 0) 
	{
		printf("%s\n", dev_error_str(error));
		exit(1);
	}

	while (1)
	{
		printf("Select one of the following commands :\n\n");
		printf("0. Quit\n");
		printf("1. On          2. Off          3. State\n");
		printf("4. Status      5. Set          6. Read\n");
		printf("7. Update      8. Local        9. Error\n");
		printf("10.Resett\n");

		printf("cmd ?");
/*
 * to get around the strange effects of scanf() wait for something read 
 */
		for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
		status = sscanf(cmd_string,"%d",&cmd);

		switch (cmd) 
		{
	   		case 1 : 
				status = dev_putget(ps,DevOn,NULL,D_VOID_TYPE,NULL, D_VOID_TYPE,&error);
				printf("\nDevOn dev_put() returned %d\n",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
	   		case 2 : 
				status = dev_putget(ps,DevOff,NULL,D_VOID_TYPE,NULL, D_VOID_TYPE,&error);
				printf("\nDevOff dev_put() returned %d\n",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 3 : 
				status = dev_putget(ps,DevState,NULL,D_VOID_TYPE, (char*)&devstatus,D_SHORT_TYPE,&error);
				printf("\nDevState dev_putget() returned %d\n ",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				if (status == 0) 
					printf("status read %d , %s\n",devstatus,DEVSTATES[devstatus]);
				break;
			case 4 : 
				ch_ptr=NULL; 
				status = dev_putget(ps,DevStatus,NULL,D_VOID_TYPE, (char*)&ch_ptr,D_STRING_TYPE,&error);
				printf("\nDevStatus dev_putget() returned %d\n ",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				if (status == 0) 
					printf(" %s\n",ch_ptr);
				break;
			case 5 : 
				printf("set current to ?");
				for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
				sscanf(cmd_string,"%f,",&setcurrent);
				status = dev_putget(ps,DevSetValue,(char*)&setcurrent,D_FLOAT_TYPE, NULL,D_VOID_TYPE,&error);
				printf("\nDevSetValue dev_putget() returned %d, ",status);
				printf("current should be set to %6.2f amps\n",setcurrent);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 6 : 
				status = dev_putget(ps,DevReadValue,NULL,D_VOID_TYPE,
				(char*)&readcurrent,D_FLOAT_READPOINT,&error);
				printf("\nDevReadValue dev_putget() returned %d, ",status);
				printf("current set to %6.3f read %6.3f\n",readcurrent.set, readcurrent.read);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 10 : 
				status = dev_put(ps,DevReset,NULL,D_VOID_TYPE,&error);
				printf("\nDevReset dev_put() returned %d\n",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 9 : 
				status = dev_put(ps,DevError,NULL,D_VOID_TYPE,&error);
				printf("\nDevError dev_put() returned %d\n",status);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 8 : 
				status = dev_put(ps,DevLocal,NULL,D_VOID_TYPE,&error);
				printf("\nDevLocal dev_put() returned %d\n",status);
				if (status < 0)
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 7 : 
				status = dev_putget(ps,DevUpdate,NULL,D_VOID_TYPE, (char*)&statereadpoint,D_STATE_FLOAT_READPOINT,&error);
				printf("\nDevUpdate devputget() returned %d (error %d)\n",status,error);
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				if (status >= 0) 
				{
					printf("status read %d , %s\n",statereadpoint.state,DEVSTATES[statereadpoint.state]);
					printf("current set to %6.3f read %6.3f\n",statereadpoint.set, statereadpoint.read);
				}
				break;
				
			case 0 : 
				dev_free(ps,&error);
				exit(0);
			default : break;
		}
	}
	return 0;
}

