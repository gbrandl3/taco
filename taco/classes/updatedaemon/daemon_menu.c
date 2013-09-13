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
 * File:        daemon_menu.c
 *
 * Project:     Device Servers 
 *
 * Description: source code file testing the update daemon server 
 *
 * Author(s):   
 *              $Author: andy_gotz $
 *
 * Original:    
 *
 * Version:     $Revision: 1.7 $
 *
 * Date:        $Date: 2009-09-23 11:42:34 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <DevServer.h>
#include <DevSignal.h>
#include <DevErrors.h>

#ifdef unix
#include <string.h>
#include <stdlib.h>
#endif

#ifdef OSK
#include <strings.h>
#endif

#include <ud_server.h>
#include <daemon.h>
#include <daemon_xdr.h>
#include <dev_xdr.h>



/****************************************************************************/
void printerror(long    error)
{
   long i;

   for(i=0;i<MAX_DEVERR;i++)
      if(error==DevErr_List[i].dev_errno)
         {
         printf("%s\n",DevErr_List[i].message);
         return;
         };
   return;
}


/****************************************************************************/
void print_boolean(char flag)
{
   if (flag == TRUE)
   {
     printf ("TRUE\n");
   }
   else
   {
     printf ("FALSE\n");
   }
}


int main(int argc, char **argv)
{
        char cstr[60];
	char *cmdstr;
	char *cmdlst; 
	long j,k;
	int i,ind;
	char d_n[60];
	db_resource resource;
	unsigned int num_resource;
        struct DevDaemonStruct input_struct;	
	devserver daemon;
	long readwrite = 0; 
	DevLong error;
	int cmd, status;
	int readout;
	short devstate;
	static char input_string[256];
	DevString devstatus,status_argout;
	DevString devname;
	char *output_string;
	char *ch_ptr,cmd_string[256];
	DevLong ddid,long_output;
	char *input_addr,*resfile;
	static DevDaemonData ddata;
	DevDaemonStatus dstatus;
	float fsecs;
        static struct timeval client_retry_timeout = { 20 , 0 };
        static struct timeval client_timeout = { 20 , 0 };
	struct timeval ti;


	if (argc < 2)
	{
	   printf("usage: %s device-name\n",argv[0]);
	   exit(1);
	}

	status = dev_import(argv[1],readwrite,&daemon,&error);
	printf("dev_import(%s) returned %d\n",argv[1],status);

	ti.tv_sec = 10;
	ti.tv_usec = 0;
	dev_rpc_timeout(daemon,CLSET_TIMEOUT,&ti,&error);

	if (status != 0) 
	{
           dev_printerror_no (SEND, "dev_import", error);
	   exit(1);
	}


        if ( LOAD_DAEMON_DATA(&error) == DS_NOTOK ) { return(DS_NOTOK); } 
        if ( LOAD_DAEMON_NEWD(&error) == DS_NOTOK ) { return(DS_NOTOK); } 
        if ( LOAD_DAEMON_STATUS(&error) == DS_NOTOK ) { return(DS_NOTOK); } 


	while (1)
	{
	   printf("DAEMON_MENU FOR %s\n",argv[1]);
	   printf("Select one of the following commands : \n\n");
	   printf(" 1. Exit             2. Status            3. State\n");
	   printf(" 4. GetDeviceIdent   5. InitialiseDevice  6. StartPolling\n");
	   printf(" 7. StopPolling      8. ChangeInterval    9. LongStatus\n");
	   printf("10. DefineMode      11. PollStatus       12. AccessStatus\n");
	   printf("13. GetDeviceName   14. SaveConfig       15. RemoveDevice \n");
	   printf("cmd ? ");
/*
 * to get around the strange effects of scanf() wait for something read 
 */
	   for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	   status = sscanf(cmd_string,"%d",&cmd);

	   switch (cmd) {

      /*   
	   case () : printf("ON\n");
		      status = dev_putget(daemon,DevOn,
					  NULL,D_VOID_TYPE,NULL,
                                          D_VOID_TYPE,
					  &error);
	              printf("\nDevOn dev_put() returned %d\n",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevOn ", error);
	              break;


	   case () : printf("OFF\n");
		      status = dev_putget(daemon,DevOff,
					  NULL,D_VOID_TYPE,
					  NULL,D_VOID_TYPE,
					  &error);
	              printf("\nDevOff dev_put() returned %d\n",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevOff ", error);
	              break;   
      */ 

	   case (3) : printf("STATE\n");
		      status = dev_putget(daemon,DevState,
					  NULL,D_VOID_TYPE,
                                          &devstate,D_SHORT_TYPE,
					  &error);
	              printf("\nDevState dev_putget() returned %d\n ",status);
		      if (status == 0) 
		      {
	                 printf("state read %d , %s \n",devstate,DEVSTATES[devstate]);
		      }
                      else
                          dev_printerror_no (SEND, "DevState ", error);
	              break;

	   case (4) : printf("GET DEVICE IDENT\n");
		      printf("Device : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
		      input_addr = input_string;
	              sscanf(cmd_string,"%s,",input_string);
		      printf("cmd_string = %s\n",input_string);
                      status = dev_putget(daemon,DevGetDeviceIdent,
					  &input_addr,D_STRING_TYPE,
                                          &long_output,D_LONG_TYPE,
					  &error);
	              printf("\nDevGetDeviceIdent dev_putget() returned %d\n ",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevGetDeviceIdent ", error);
		      else
	                  printf("Device ident = %d \n ",long_output);
	              break;

	   case (5) : printf("INITIALISE DEVICE\n");

                      /* Structure Initialisation */
		      
		      input_struct.dev_n = d_n;
		      input_struct.cmd_list.length = 0;
		      (input_struct.cmd_list.sequence) = NULL; 
		      input_struct.poller_frequency = 0;

		      cmdstr = cstr;

	              /* New Device Name */				    
		      printf("Device : "); fflush(stdout);
	              scanf("%s",input_struct.dev_n);
		      for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%s,",input_struct.dev_n);
	              printf("cmd_string = %s\n",input_struct.dev_n);

              printf("insert the number of commands whose the device need \n");
	      fflush(stdout);
	      scanf("%d",&j);	     
	      (input_struct.cmd_list.sequence) = (char **) calloc(j,sizeof(DevString));

                      /* New Command List Associated with new device */
			
	   	      k = 1;
		      ind = 0;

		      while(j >= 1)
		      {
			
			printf("insert the command name number %d\n",k);
			fflush(stdout);
			scanf("%s",cmdstr);
                        (input_struct.cmd_list.sequence)[ind] = (char *) malloc(strlen(cmdstr)+1); 
			strcpy((input_struct.cmd_list.sequence)[ind],cmdstr); 
                        (input_struct.cmd_list.length) += 1; 
		        ind++;
                        j--;
		        k++;
		      
		       } 
	
	              /* New Frequency */
		      
		      fflush(stdin); 
		      printf("Frequency : "); 
		      fflush(stdout);
		      scanf("%f",&(fsecs));
		      for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              /*
		      sscanf(cmd_string,"%f,",&(fsecs));
		      */
		      input_struct.poller_frequency = (long) (fsecs*10); 
		      status = dev_putget(daemon,DevInitialiseDevice,
			 		    &input_struct,D_DAEMON_STRUCT,
                                            &long_output,D_LONG_TYPE,
					    &error);
	              /* free memory */
		      
		      for (i=0;i<ind;i++)
		      {
                        free((input_struct.cmd_list.sequence)[i]);
                      }
                        free(input_struct.cmd_list.sequence);

		      printf("\nDevInitialiseDevice dev_putget() returned %d\n",status);
                      if (status < 0)
                         dev_printerror_no (SEND, "DevInitialiseDevice ", error);
	              break;
	   
	   case (6) : printf ("START POLLING\n");
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&ddid);
		      status = dev_putget(daemon,DevStartPolling,
					  &ddid,D_LONG_TYPE,
                                          NULL,D_VOID_TYPE,
					  &error);
	              printf("\nDevStartPolling dev_putget() returned %d, ",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevStartPolling ", error);
	              break;

	   case (7) : printf ("STOP POLLING\n");
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&ddid);
		      status = dev_putget(daemon,DevStopPolling,
					  &ddid,D_LONG_TYPE,
                                          NULL,D_VOID_TYPE,
					  &error);
	              printf("\nDevStopPolling dev_putget() returned %d, ",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevStopPolling ", error);
	              break;

	   case (8) : printf ("CHANGE POLL INTERVAL\n");
		      printf("Device : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
		      input_addr = input_string;
	              sscanf(cmd_string,"%s,",input_string);
		      printf("cmd_string = %s\n",input_string);
                      status = dev_putget(daemon,DevGetDeviceIdent,
					  &input_addr,D_STRING_TYPE,
                                          &long_output,D_LONG_TYPE,
					  &error);
	              printf("\nDevGetDeviceIdent dev_putget() returned %d\n ",status);
                      printf("long_output %d \n",long_output);
		      fflush(stdout);
		      if (status < 0)
                          dev_printerror_no (SEND, "DevGetDeviceIdent ", error);
		      else
	              {
		       printf("Device ident = %d \n ",long_output);
		       ddata.ddid = long_output;
		       printf("New interval : "); fflush(stdout);
	               for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	               sscanf(cmd_string,"%f,",&(fsecs));
		       ddata.long_data = (long)(fsecs*10);
		       status = dev_putget(daemon,DevChangeInterval,
		 			   &ddata,D_DAEMON_DATA,
                                           NULL,D_VOID_TYPE,
					   &error);
	               printf("\nDevChangeInterval dev_putget() returned %d, ",status);
                       if (status < 0)
                          dev_printerror_no (SEND, "DevChangeInterval ", error);
	              }
		      break;


	   case (9) : printf("STATUS\n");
		      devstatus=NULL;
		      status = dev_putget(daemon,DevLongStatus,
					  NULL,D_VOID_TYPE,
                                          &devstatus,D_STRING_TYPE,
					  &error);
	              printf("\nDevStatus dev_putget() returned %d\n ",status);
		      if (status == 0) 
		      {
	                 printf("Status is %s\n",devstatus);
		      }
                      else
                          dev_printerror_no (SEND, "DevLongStatus ", error);
	              break;

	   case (10) :printf("DEFINE MODE\n");
	   
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&(ddata.ddid));
		      printf("Access status : \n");
		      printf("  1. SURVEY \n");
		      printf("  2. READOUT \n");
		      readout=0;
		      while (readout<1 || readout>2)
		      {
		        printf("> "); fflush(stdout);
	                for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	                sscanf(cmd_string,"%d,",&(readout));
		      }
		      switch (readout)
		      {
			case 1 : ddata.long_data = SURVEY;
				 printf ("SURVEY selected\n");
				 break;
			case 2 : ddata.long_data = READOUT;
				 printf ("READOUT selected\n");
				 break;
		      }

		      status = dev_putget(daemon,DevDefineMode,
					  &ddata,D_DAEMON_DATA,
                                          NULL,D_VOID_TYPE, 
					  &error);
	              printf("\nDevDefineMode dev_put() returned %d\n",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevDefineMode", error);
	              break;

	   case (11) :printf("POLL STATUS\n");
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&ddid);
		      output_string=NULL;
		      devname=NULL;
		      status = dev_putget(daemon,DevGetDeviceName,
					  &ddid,D_LONG_TYPE,
                                          &devname,D_STRING_TYPE,
					  &error);
	              printf("\nDevGetDeviceName dev_putget() returned %d\n ",status);
		      if (status < 0) 
		      {
                        dev_printerror_no (SEND, "DevGetDeviceName ", error);
		      }
                      else
		      {
	                printf("Device name is %s\n",devname);
                        status = dev_putget(daemon,DevPollStatus,
					  &ddid,D_LONG_TYPE,
                                          &dstatus,D_DAEMON_STATUS,
					  &error);
	                printf("\nDevPollStatus dev_putget() returned %d\n",status);
		      if (status >= 0)
		      {
			printf ("Device poll status    : \n");
			printf ("  Being polled        : ");
			print_boolean(dstatus.BeingPolled);
			printf ("  Poll interval       : %.2f sec\n",((float)(dstatus.PollFrequency/(FACTOR*1.0))));
			printf ("  Poll frequency      : %.2f Hz\n",((float)((FACTOR*1.0)/dstatus.PollFrequency)));
			/*
			strftime (tlpstring,
				  32,
				  "%x %X",
				  localtime((time_t *)dstatus.TimeLastPolled));
				  */
			printf ("  Time last polled    : %ld\n",dstatus.TimeLastPolled);
			printf ("  Poll mode           : ");
			if (dstatus.PollMode == SURVEY)
			{
			  printf ("SURVEY\n");
			}
			else
			{
			  printf ("READOUT\n");
			}
			printf ("  Device access error : ");
		        print_boolean(dstatus.DeviceAccessError);
                      }
                      else
		      {
                          dev_printerror_no (SEND, "DevPollStatus", error);
		      }
                     }
	              break;


	   case (12) :printf("DEVICE ACCESS STATUS\n");
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&ddid);
		      devname=NULL;
		      status = dev_putget(daemon,DevGetDeviceName,
					  &ddid,D_LONG_TYPE,
                                          &devname,D_STRING_TYPE,
					  &error);
	              printf("\nDevGetDeviceName dev_putget() returned %d\n ",status);
		      if (status != 0) 
		      {
                          dev_printerror_no (SEND, "DevGetDeviceName ", error);
		      }
                      else
	              {   
			 printf("Device name is %s\n",devname);
		          output_string=NULL;
                          status = dev_putget(daemon,DevAccessStatus,
					      &ddid,D_LONG_TYPE,
                                              &dstatus,D_DAEMON_STATUS,
					      &error);
	                  printf("\nDevAccessStatus dev_putget() returned %d\n",status);
                          if (status >= 0)
		          {
			   printf("Device access details : \n");
                           printf("  Last command status = %ld\n",dstatus.LastCommandStatus);
                           printf("  Last error code     = %ld\n",dstatus.ErrorCode);
			   if (dstatus.LastCommandStatus < 0)
			   {
                            printf ("  Last error message : ");
                            printerror (dstatus.ErrorCode);
			   }
	 		   printf("  ContinueAfterError  = ");
			   print_boolean(dstatus.ContinueAfterError);

                          }
                          else
		          {
                            dev_printerror_no (SEND, "DevAccessStatus", error);
		          }
	               } 
	              break;

	   case (13): printf("NAME\n");
		      printf("Device id (0=server) : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
	              sscanf(cmd_string,"%d,",&ddid);
		      devname=NULL;
		      status = dev_putget(daemon,DevGetDeviceName,
					  &ddid,D_LONG_TYPE,
                                          &devname,D_STRING_TYPE,
					  &error);
	              printf("\nDevGetDeviceName dev_putget() returned %d\n ",status);
		      if (status == 0) 
		      {
	                 printf("Device name is %s\n",devname);
		      }
                      else
                          dev_printerror_no (SEND, "DevGetDeviceName ", error);
	              break;
           
	   case (14): printf("SAVE DEVICE CONFIGURATION \n"); 
                      
		      status = dev_putget(daemon,DevSaveConfi,
					  NULL,D_VOID_TYPE,
                                          NULL,D_VOID_TYPE,
					  &error);
	              printf("\nDevSaveConfi dev_putget() returned %d\n ",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevSaveConfi ", error);
	              break;

	   case (15): printf("REMOVE DEVICE  \n"); 
                      
	              /***  Ask a device id  ***/
		      
		      printf("Device : "); fflush(stdout);
	              for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
		      input_addr = input_string;
	              sscanf(cmd_string,"%s,",input_string);
		      printf("cmd_string = %s\n",input_string);
                      status = dev_putget(daemon,DevGetDeviceIdent,
					  &input_addr,D_STRING_TYPE,
                                          &long_output,D_LONG_TYPE,
					  &error);
	              printf("\nDevGetDeviceIdent dev_putget() returned %d\n ",status);
                      printf("long_output %d \n",long_output);
		      fflush(stdout);
		      if (status < 0)
                          dev_printerror_no (SEND, "DevGetDeviceIdent ", error);
		      else
	              {
		       printf("Device ident = %d \n ",long_output);
		       ddata.ddid = long_output;
		       status = dev_putget(daemon,DevRemoveDevice,
			 		   &ddata.ddid,D_LONG_TYPE,
                                           NULL,D_VOID_TYPE,
					   &error);
	               printf("\nDevRemoveDevice dev_putget() returned %d\n ",status);
                       if (status < 0)
                          dev_printerror_no (SEND, "DevRemoveDevice ", error);
		       }
		      break;

           case (2): printf("STATUS \n");
                      status_argout = NULL; 
		      status = dev_putget(daemon,DevStatus,
					  NULL,D_VOID_TYPE,
                                          &status_argout,D_STRING_TYPE,
					  &error);
	              printf("\nDevStatus dev_putget() returned %d\n ",status);
                      if (status < 0)
                          dev_printerror_no (SEND, "DevStatus ", error);
	              else
		        printf("%s \n",status_argout);
		      break;

	   case (1) :printf("FREE AND EXIT\n");
		      status = dev_free (daemon,&error);
	              exit(0);

	   default : break;
	   }
	   printf ("\nMENU COMMAND ERROR NUMBER : %ld\n",error);
	}
}

