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
 * File:        poller.c
 *
 * Project:     Device Servers 
 *
 * Description: 
 *
 * Author(s):   
 *              $Author: jkrueger1 $
 *
 * Original:    
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-11-02 16:53:04 $
 */

static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/poller.c,v 1.5 2006-11-02 16:53:04 jkrueger1 Exp $";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif

#include <dataport.h>

#if HAVE_TIME_H
#	include <time.h>
#endif
#include <stdlib.h>
#include <string.h>
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif
/* #include <strings.h> */

#include <API.h>
#include <DevServer.h> 
#include <DevErrors.h>
#include <DevSignal.h>

#include <daemon.h>

#include <Admin.h>

static long dev_id;
static devserver ds;
static Dataport *dp;
static dataport_struct_type *dipc;
static daemon_device_struct_type device_data[MAX_DEVICES];
int no_of_device=FIRST_DEVICE-1;
char dc_problems=FALSE;
long dc_error_num=0;
long dc_last_error=0;
long dc_tries=0,dc_oks[ERROR_CYCLE],dc_cycle=0;
long dpg_last_error=0;
long dpg_tries=0,dpg_oks[ERROR_CYCLE],dpg_cycle=0;

int running=TRUE;

long proc_command(int);
void store_status(int,long,long);
long get_time();   /* CONVERTED TO RETURN FACTORths of SECONDS */
int time_diff_calc (long,long,long *);
void get_next_poll_time (long *,long,long);
#ifdef __cplusplus
extern "C"
{
void user_signal(int);
void signal_handler(int);
}
#else
void user_signal(int);
void signal_handler(int);
#endif
long check_status(long *);
long poll_device(int);

/* template resource table for each device */

db_resource devres_table[] = {                           /* DEFAULT */
                   {"ud_poll_on_init",   D_BOOLEAN_TYPE},   /* False */
                   {"ud_poll_mode",      D_STRING_TYPE},    /* survey */
                   {"ud_num_retries",    D_LONG_TYPE},      /* 5 */
                   {"ud_continue",       D_BOOLEAN_TYPE},   /* True */
                   {"ud_poll_interval",  D_LONG_TYPE},      /* FACTOR*10 */
                   {"ud_command_list",   D_VAR_STRINGARR},  /* No Default */
                   {"ud_poll_recover",   D_STRING_TYPE},    /* catchup */
                   {"ud_poll_forget",    D_LONG_TYPE}       /* 5 times */
                   };

const char* devres_name[] = {                         
                        "ud_poll_on_init",
                        "ud_poll_mode",  
                        "ud_num_retries", 
                        "ud_continue", 
                        "ud_poll_interval",
                        "ud_command_list",  
                        "ud_poll_recover",  
                        "ud_poll_forget",    
                       };

int devres_tab_size = sizeof(devres_table)/sizeof(db_resource);

struct poll_list_type {
		   int                   index;
		   struct poll_list_type *next;
		   struct poll_list_type *previous;
		       };

struct poll_list_type *first_poll;
struct poll_list_type *last_poll;

static char dataport_name[64];

void round_interval (long *interval)
{
  /* ROUNDING MODIFICATION */

  	*interval = ( (*interval) * FACTOR ) / GRAIN;
  	if (*interval < (FACTOR / GRAIN))
  	{
    		*interval = FACTOR / GRAIN;
  	}
}


/****************************************************************************/
struct poll_list_type *get_element (int index)
{
  	struct poll_list_type *new_element;
  	int size;

#if defined (TRACE)
  	fprintf(stderr, "get element\n");
#endif
  	size=sizeof(struct poll_list_type);
  	new_element = (struct poll_list_type *)malloc(size);
  	if (new_element != NULL)
    	new_element->index = index;
#if defined (TRACE)
  	fprintf(stderr, "end get element\n");
#endif
  	return (new_element);
}


/****************************************************************************/
void output_list (struct poll_list_type *top)
{
  	if (top!=NULL)
  	{
    		fprintf (stderr, "index %d polltime %ld\n",top->index,
		      	device_data[top->index].NextPollTime);
    		output_list (top->next);
 	}
}


/****************************************************************************/
void insert_device (struct poll_list_type *new_element,
                    struct poll_list_type *current_ptr)
{
	long diff;
   	struct poll_list_type *prev_element;
   
#if defined(TRACE)
     	printf("begin insert_device()\n");
#endif

   	if (current_ptr==NULL) /**  NO DEVICE POLLED  **/
   	{
/* FREE LIST */
 
     		new_element->next = NULL;
    	 	new_element->previous = NULL;
     		first_poll = new_element;
     		last_poll = new_element;

#if defined(TRACE)
     		printf("end insert_device()\n");
#endif
     		return;
   	}
   	else  /** AT LEAST , ONE DEVICE POLLED **/
   	{
     		if (time_diff_calc (device_data[current_ptr->index].NextPollTime,
		       		    device_data[new_element->index].NextPollTime,
		       		    &diff) == TRUE)
     		{
/* SHIFT ONE PLACE LEFT */
       			if (current_ptr->previous != NULL)  /** MORE THAN ONE DEVICE **/
       			{
	
				insert_device(new_element,current_ptr->previous);

#if defined(TRACE)
         			printf("end insert_device()\n");
#endif
         			return;
       
       			}
       			else  /** ONLY ONE DEVICE **/
       			{
	 
/* REACHED TOP OF LIST */

	 			new_element->next = first_poll;
	 			new_element->previous = NULL;
         			first_poll->previous = new_element;
	 			first_poll = new_element;

#if defined(TRACE)
         			printf("end insert_device()\n");
#endif 
	 			return;
       			}
     		}
/* INSERT HERE */

     		new_element->next = current_ptr->next;
     		new_element->previous = current_ptr;
     		current_ptr->next = new_element;
     		if (new_element->next != NULL)
       			new_element->next->previous = new_element;
     		else
       			last_poll=new_element;
 	}

#if defined(TRACE)
   	printf("end insert_device()\n");
#endif 

}


/****************************************************************************/
void check_next_device (long check_time)
{
	struct poll_list_type *shift_element;
	static daemon_device_struct_type *current_device_ptr;
	struct timeval wait_time;
	long time_diff,dT;
	long status,error,error_dev;
	long oldpolltime=0;
	int signaltestloop;
	int i;

	if (first_poll == NULL)  
	{

#if defined (TRACE)
		printf ("begin check_next_device\n");
#endif
  
		return;
  
	}

#if defined (TRACE)
	printf ("check_next_device\n");
#endif

	current_device_ptr=&(device_data[first_poll->index]);


	if (time_diff_calc(check_time,current_device_ptr->NextPollTime,&dT)==TRUE) 
	{
#ifdef FORGETCODE
		if (current_device_ptr->PollRecoverMode==FORGET)
		{
			if (check_time >= current_device_ptr->NextPollTime+
                        		current_device_ptr->PollForgetTime)
			{
/* The poller is severely out of synchronisation, so */
/* it is best to forget all outstanding polls for this */
/* device and `begin again' */

				current_device_ptr->NextPollTime=check_time;
			}
		}
#endif
#if defined (WAIT)
		printf ("NO WAIT REQUIRED\n");
#endif
		current_device_ptr->TimeLastPolled=get_time();
    
		if (current_device_ptr->BeingPolled==TRUE)
		{
			poll_device(first_poll->index);
		}
		oldpolltime = current_device_ptr->NextPollTime;

		if (current_device_ptr->PollRecoverMode == DRIFT)
		{
			if (current_device_ptr->DeviceImported==FALSE)
			{
				get_next_poll_time (&current_device_ptr->NextPollTime,
                                  		    current_device_ptr->TimeLastPolled,
                                  		    POLL_INTERVAL_NOT_IMP);
			}
	 		else
	 		{
             			get_next_poll_time (&current_device_ptr->NextPollTime,
                                  		    current_device_ptr->TimeLastPolled,
                                  		    current_device_ptr->PollInterval);
	 		}
      		}
      		else   
      		{ 
	 		get_next_poll_time (&current_device_ptr->NextPollTime,
                             		    current_device_ptr->NextPollTime,
                             		    current_device_ptr->PollInterval);
      		}

    
    		if (first_poll->next == NULL)
    		{
      			status = store_data (TRUE,current_device_ptr,&error_dev);  /* Finished servicing data on this time interval */
      			if (status != DS_OK)
      			{
        			dc_problems  = TRUE;
				dc_error_num = current_device_ptr->DCError;
				dc_last_error= dc_error_num;
        			dc_oks[dc_cycle]=DS_NOTOK;
      			}
      			else if (error_dev>=0)
      			{
				dc_problems  = FALSE;
				dc_error_num = 0;
        			dc_oks[dc_cycle]=DS_OK;
      			} 
      			if (++dc_cycle >= ERROR_CYCLE) dc_cycle=0;
      			if (dc_tries<ERROR_CYCLE) dc_tries++;
      			current_device_ptr->DCError=dc_error_num;
    		}
    		else 
    		{
      			if (oldpolltime != device_data[first_poll->next->index].NextPollTime)
      			{
        			status = store_data (TRUE,current_device_ptr,&error_dev);
        			if (status != DS_OK)
        			{
          				dc_problems  = TRUE;
	  				dc_error_num = current_device_ptr->DCError;
	  				dc_last_error= dc_error_num;
          				dc_oks[dc_cycle]=DS_NOTOK;
        			}
        			else if (error_dev>=0)
        			{
  	  				dc_problems  = FALSE;
	  				dc_error_num = 0;
          				dc_oks[dc_cycle]=DS_OK;
        			} 
        			if (++dc_cycle >= ERROR_CYCLE) dc_cycle=0;
        			if (dc_tries<ERROR_CYCLE) dc_tries++;
        			current_device_ptr->DCError=dc_error_num;
      			}

      			shift_element=first_poll;
      			first_poll=first_poll->next;
      			first_poll->previous=NULL;
      			insert_device (shift_element, last_poll);

    		}


    		status = store_data (FALSE,current_device_ptr,&error_dev); /* FALSE means check of number of elements */
    		if (status != DS_OK)
    		{
      			dc_problems=TRUE;
      			dc_error_num    = current_device_ptr->DCError;
      			dc_last_error   = dc_error_num;
      			dc_oks[dc_cycle]=DS_NOTOK;
    		}
    		else if (error_dev>=0)
    		{
      			dc_problems=FALSE;
      			dc_error_num = 0;
      			dc_oks[dc_cycle]=DS_OK;
    		}
    		if (++dc_cycle >=ERROR_CYCLE) dc_cycle=0;
    		if (dc_tries<ERROR_CYCLE) dc_tries++;
    		current_device_ptr->DCError=dc_error_num;


  	}
 	else
  	{
/* NEXT TIME TO POLL NOT YET ARRIVED */
/* dc code */
    		status = store_data (TRUE,current_device_ptr,&error_dev); /* TRUE means store data no matter how many elements */
    		if (status != DS_OK)
    		{
      			dc_problems=TRUE;
      			dc_error_num = current_device_ptr->DCError;
      			dc_last_error= dc_error_num;
      			dc_oks[dc_cycle]=DS_NOTOK;
    		}
    		else if (error_dev>=0)
    		{
      			dc_problems=FALSE;
      			dc_error_num = 0;
      			dc_oks[dc_cycle]=DS_OK;
    		}
    		if (++dc_cycle >=ERROR_CYCLE) dc_cycle=0;
    		if (dc_tries<ERROR_CYCLE) dc_tries++;
    		current_device_ptr->DCError=dc_error_num;
  /* end dc code */

/* change */

		dipc->signal_valid = TRUE;
    		while (time_diff_calc(get_time(),current_device_ptr->NextPollTime,&time_diff)==FALSE)
    		{

/* Before sleeping, check if a command has not been posted */

    			check_status(&(dipc->status));
			
/* Compute again the remaining time and sleep if possible */
			
			if (time_diff_calc(get_time(),current_device_ptr->NextPollTime,&time_diff)== FALSE)
			{

#if defined (WAIT)
      				fprintf (stderr, "timediff = %ld %ldths for dev %s\n",time_diff,FACTOR,current_device_ptr->DeviceName);
#endif

     		 		wait_time.tv_sec = (long)(time_diff/FACTOR);
     				wait_time.tv_usec = (long)(FACTOR*(time_diff - wait_time.tv_sec*FACTOR));

#if defined (WAIT)
      				fprintf (stderr, "waiting for %ld.%000ld seconds\n",wait_time.tv_sec,wait_time.tv_usec);
#endif


/*      				dipc->signal_valid=TRUE;*/
      				select (0,0,0,0,&wait_time);
/*      				dipc->signal_valid=FALSE;*/
			}
    		}
		dipc->signal_valid = FALSE;

  	}

#if defined (TRACE)
  	printf ("end check_next_device\n");
#endif

  	return;
}


/****************************************************************************/
void printerror(long error)
{
   	long i;

   	for(i=0;i<MAX_DEVERR;i++)
      	if(error==DevErr_List[i].dev_errno)
      	{
#if defined (EBUG)
         	fprintf(stderr,"%s\n",DevErr_List[i].message);
#endif
         	return;
      	};

#if defined (EBUG)
   	fprintf(stderr,"error code %d",error);
#endif
   	return;
}


/****************************************************************************/
void user_signal(int signo)
{
    	long time_now;
    	static int depth=0;

    	fprintf(stderr,".");

/*    	dipc->signal_valid=FALSE; /* don't allow any more user signals for now */

#if defined (SIGNALTRACE)
   	fprintf(stderr,"THIS IS THE POLLER USER SIGNAL HANDLER\n");
   	fprintf(stderr,"DEPTH = %d\n",++depth);
#endif

    	time_now=get_time();
    	check_status(&(dipc->status));

#if defined (SIGNALTRACE)
   	fprintf(stderr,"END POLLER USER SIGNAL HANDLER\n");
   	fprintf(stderr,"DEPTH = %d\n",--depth);
#endif

	dipc->signal_valid = TRUE;
}


/****************************************************************************/
void signal_handler(int signal)
{

   	register int dev;
   	long status,error;
   	dc_error dc_error_val;

#if defined (EBUG)
   	fprintf(stderr,"THIS IS THE POLLER SIGNAL HANDLER\n");
#endif

   	printf ("UD_POLLER: SIGNAL HANDLER WITH SIGNAL=%d\n",signal);
   	fflush(stdout);
   	for (dev=FIRST_DEVICE;dev<=no_of_device;dev++)
   	{
#if defined (EBUG)
     		printf("Freeing %s ... ",device_data[dev].DeviceName);fflush(stdout);
#endif
     		if (device_data[dev].DeviceImported==TRUE)
     		{
       			status = dev_free (device_data[dev].DeviceHandle,&error);
       			if (status == DS_OK)
       			{
#if defined (EBUG)
  				printf("OK\n"); fflush(stdout);
#endif
       			}
       			else
       			{
#if defined (EBUG)
  	  			printf("FAILED\n"); fflush(stdout);
  	  			printerror(error);
#endif
       			}
     		}
     		else
     		{
#if defined (EBUG)
  			printf("NEVER IMPORTED\n"); fflush(stdout);
#endif
     		}

#if defined (EBUG)
     		printf("dc_closing %s ... ",device_data[dev].DeviceName);fflush(stdout);
#endif

     		if ((device_data[dev].DCInitialised==TRUE) &
         	    (device_data[dev].PollMode==READOUT))
/* Added READOUT test 281092 */
     		{
       			status = dc_close (&(device_data[dev].DeviceName),1,&dc_error_val);
       			if (status == DS_OK)
       			{
#if defined (EBUG)
  				printf("OK\n"); fflush(stdout);
#endif
       			}
       			else
       			{
#if defined (EBUG)
  	  			printf("FAILED\n"); fflush(stdout);
  	  			printerror(dc_error_val.error_code);
#endif
       			}
     		}
     		else
     		{
#if defined (EBUG)
  			printf("NEVER IMPORTED\n"); fflush(stdout);
#endif
     		}
   	}
   	exit (-1);
}


/****************************************************************************/
void free_command(int cmd_num,command_list *cmd_ptr)
{
   int i;
   command_list *array[20];
   command_list *last;

#if defined(TRACE)
   fprintf(stderr,"free_command()\n");
#endif

   last = cmd_ptr;
   for (i = 0;i < cmd_num;i++)
   {
	array[i] = last;
        last = last->next;
   }

   for (i = 0;i < cmd_num;i++)
   {
    	free(array[i]);
   }

#if defined(TRACE)
   fprintf(stderr,"end free_command()\n");
#endif

}


/****************************************************************************/
long store_command(char *command_name,int command_type,
                   command_list **list_ptr,long *error)
{

  int size;
  command_list *last=NULL;
  size=sizeof(command_list);

#if defined(TRACE)
   fprintf(stderr,"store_command()\n");
#endif

  *error = 0;
  last=*list_ptr;

  if (last==NULL)
  {
    last=(command_list *)malloc(size);
    if (last == NULL)
    {
      *error = DevErr_InsufficientMemory;
      return (DS_NOTOK);
    }
    *list_ptr=last;
  }
  else
  {
    while (last->next!=NULL)
    {
      last=last->next;
    }
    last->next=(command_list *)malloc(size);
    if (last->next == NULL)
    {
      *error = DevErr_InsufficientMemory;
      return (DS_NOTOK);
    }
    last=last->next;
  }
  last->command=0;
  last->command_name[0]=0;
  strcpy(last->command_name,command_name);
  last->command_type=command_type;
  last->next=NULL;

#if defined(TRACE)
  fprintf(stderr,"end store_command()\n");
#endif

  return(DS_OK);
}


/****************************************************************************/
long initialise_ipc(pid_t pid)
{
  long error;

#if defined(TRACE)
  fprintf(stderr,"initialise_ipc()\n");
#endif

  Make_Dataport_Name (dataport_name, sizeof(dataport_name) - 1, (char *)DAEMON_DATAPORT,pid);

  dp=OpenDataport (dataport_name,sizeof(dataport_struct_type));
  if (dp==NULL)
  {
#if defined(EBUG)
    fprintf(stderr, "OpenDataport failed.\n");
#endif

    error=DevErr_DeviceOpen;  /* DevErr_OpenDataportFailed XXX */
    store_status(no_of_device,DS_NOTOK,error);

#if defined(EBUG)
    fprintf(stderr,"end initialise_ipc()\n");
#endif

    return(DS_NOTOK);
  }
  dipc = (dataport_struct_type *)&(dp->body);
  AccessDataport(dp);
  dipc->status = D_READY;
  dipc->poller_id=(long)getpid();
  ReleaseDataport(dp);

#if defined(TRACE)
  fprintf(stderr,"end initialise_ipc()\n");
#endif

  return(DS_OK);
}


/****************************************************************************/
void store_status(int dev_num,long status,long error)
{

#if defined(TRACE)
  fprintf(stderr,"store_status()\n");
#endif

  device_data[dev_num].LastCommandStatus=status;
  device_data[dev_num].LastErrorCode=error;

#if defined(TRACE)
  fprintf(stderr,"end store_status()\n");
#endif

  return;
}



/****************************************************************************/
long get_types (daemon_device_struct_type *cdp,long *error)
{

  DevVarCmdArray    ValidCommandList;
  command_list      *comm_ptr;
  int i;
  long status;

#if defined(TRACE)
  fprintf(stderr,"get_types()\n");
#endif

  comm_ptr = cdp->CommandList;

  if (cdp->DeviceHandle != NULL)
  {
#if defined (VOIDTYPE)
      printf ("Inside get_types with cdp->DeviceHandle = %d\n", cdp->DeviceHandle);
#endif
      status = dev_cmd_query (cdp->DeviceHandle, 
                              &ValidCommandList,
                              error);
      if (status != DS_OK)
      {
/*        printf ("dev_cmd_query error : %d : ",*error);
        printerror(*error);*/
	cdp->TypesRetrieved = FALSE;
        *error = DevErr_CommandNotImplemented;
	return (DS_NOTOK);
      }
      cdp->TypesRetrieved = TRUE;
      while (comm_ptr != NULL)
      {
#if defined(EBUG)
        fprintf(stderr,"comm = %d, ",comm_ptr->command);
#endif
        i=0;
        comm_ptr->command_type = 0;
        while (i<ValidCommandList.length)
        {
          if (strcmp(ValidCommandList.sequence[i].cmd_name,comm_ptr->command_name) == 0)
          {
            comm_ptr->command_type = ValidCommandList.sequence[i].out_type;
	    comm_ptr->command = ValidCommandList.sequence[i].cmd;
#if defined(EBUG)
            fprintf(stderr,"found ");
#endif
            break;
          }
          i++;
        }
	if (comm_ptr->command_type == 0)
	{
	 free(ValidCommandList.sequence);
	 *error = DevErr_CommandNotImplemented;
	 return(DS_NOTOK);
        }
#if defined(VOIDTYPE)
        printf ("%s : CMD %d - TYPE SEARCH\n",cdp->DeviceName,comm_ptr->command);
        if (comm_ptr->command_type == 0)
        {
          printf ("Couldn't find command type\n");
        }
        else
        {
          printf ("Command type = %d\n", comm_ptr->command_type);
        }
#endif
#if defined(EBUG)
        fprintf(stderr,"out type = %d\n",comm_ptr->command_type);
#endif
        comm_ptr=comm_ptr->next;
      }
#if defined(TRACE)
  fprintf(stderr,"end get_types()\n");
#endif
 
  free(ValidCommandList.sequence); 
  return(DS_OK);
  }
  else
  {
     *error = DevErr_MsgImportFailed;
     return (DS_NOTOK);
  }
}


/****************************************************************************/
long import_device(int dev_num)
{
  long status,error;
  char dev_name[60];
 
#if defined(VOIDTYPE)
  fprintf(stderr,"import_device()\n");
  fprintf(stderr,"dev_num=%d\ndevice=%s\n",
                  dev_num,device_data[dev_num].DeviceName);
#endif

  if (device_data[dev_num].TangoFlag == True)
  {
        strcpy(dev_name,"tango:");
        strcat(dev_name,device_data[dev_num].DeviceName);
  }
  else
        strcpy(dev_name,device_data[dev_num].DeviceName);

  device_data[dev_num].DeviceImported=FALSE;
  device_data[dev_num].TypesRetrieved=FALSE;
  device_data[dev_num].DCInitialised=FALSE;

  status = dev_import(dev_name,
                      (long)NULL,
                      &(device_data[dev_num].DeviceHandle),
                      &error);
  store_status(dev_num,status,error);
  if (status != DS_OK)
  {
    dpg_last_error = error;
    dpg_oks[dc_cycle]=DS_NOTOK;
  }
  else
  {
    dpg_oks[dc_cycle]=DS_OK;
  } 
  if (++dpg_cycle >= ERROR_CYCLE) dpg_cycle=0;
  if (dpg_tries<ERROR_CYCLE) dpg_tries++;

  if (status==DS_OK)
  {
    device_data[dev_num].DeviceImported=TRUE;
  
#if defined(TRACE)
    fprintf(stderr,"end import_device()\n");
#endif
    return(DS_OK);
  }
  else
  {
  
#if defined(TRACE)
    fprintf(stderr,"endf import_device()\n");
#endif
    return(DS_NOTOK);
  }
}


/****************************************************************************/
long d_initdev(long *error)
{
  long status,counter;
  static daemon_device_struct_type *current_device_ptr;
  DevVarStringArray ReadCommandList;
  register int count,savcount;
  char *PollModeString;
  char *PollRecoverString;
  struct poll_list_type *new_element;
  int size,found;
  char device_name[60];
  dc_error error_str;
  command_list *current_command;
  int i,j,k;
  char full_name[60];
  char *tmp;
  int tango_flag;


  /**************************************************************/
  /** in this procedure , modifications of M.Schofield work    **/ 
  /**************************************************************/


/* Test to check if device already exists in device list */
  
/* Device name is dipc->string */
  
  AccessDataport(dp);
    strcpy(full_name,dipc->string);
  ReleaseDataport(dp);

  /* manage the case of tango device */

  if ((tmp = strchr(full_name,':')) != NULL)
  {
        tmp++;
        strcpy(device_name,tmp);
        tango_flag = True;
  }
  else
  {
        strcpy(device_name,full_name);
        tango_flag = False;
  }


  found = FALSE;
  count = FIRST_DEVICE;
  i = no_of_device;
  j = 1;


  /****************************************************/
  /**  test of existing device in device_data table  **/
  /****************************************************/
 
  while (count<=i)  
  {
      if (device_data[count].DeviceName != NULL)
      {
       if (strcmp(device_name,device_data[count].DeviceName) == 0) 
       {
	found = TRUE;
	break;
       }
       else 
	   count++;
      }
      else 
      {
	 i++;
	 count++;
      }
   }


  /**************************************************************/
  /**  device has not already been added in device_data table  **/
  /**************************************************************/  

  if (found == FALSE)
  {
   /** searching for a possible free place in the table  **/
   /** between two implemented devices :                 **/
   
   savcount = FIRST_DEVICE;
   while (device_data[savcount].DeviceName != NULL) savcount++;

   /** if no place available between two implemented devices :  **/ 


    no_of_device++;
    if (no_of_device>=MAX_DEVICES)
    {

     *error=DevErr_ExceededMaximumNoOfDevices; /* DevErr_DaemonTooManyDevices */
     store_status(no_of_device,DS_NOTOK,*error);

#if defined(TRACE)
     fprintf(stderr,"endf d_initdev()\n");
#endif
     *error=DevErr_InsufficientMemory;
     return(DS_NOTOK);
   }
    current_device_ptr=&(device_data[savcount]);
#if defined(EBUG)
    fprintf(stderr,"current_device_ptr=0x%x, device_data[%d]=0x%x\n",
                  current_device_ptr, no_of_device,
                  &(device_data[no_of_device]));
#endif
   size=strlen(device_name)+1;
   current_device_ptr->DeviceName=(char *)malloc(size);
   if (current_device_ptr->DeviceName==NULL) /** pb here with this test **/
   {
     *error=DevErr_InsufficientMemory;
     store_status(no_of_device,DS_NOTOK,*error);

#if defined(TRACE)
     fprintf(stderr,"endf d_initdev()\n");
#endif
     no_of_device--;
     return (DS_NOTOK);
   }
   strcpy(current_device_ptr->DeviceName,device_name);
   current_device_ptr->TangoFlag = tango_flag;
   } 
   else  /**  found == TRUE  **/ 
   {
    current_device_ptr = &(device_data[count]);
#if defined(EBUG)
    fprintf(stderr,"current_device_ptr=0x%x, device_data[%d]=0x%x\n",
                  current_device_ptr, no_of_device,
                  &(device_data[count]));
#endif
   }

  if (found == FALSE)
       status=import_device(savcount);
  current_device_ptr->PollOnInitialise = TRUE;
  current_device_ptr->PollMode = READOUT;
  current_device_ptr->TimeLastPolled = 0;
  current_device_ptr->NextPollTime = 0;
  current_device_ptr->BeingPolled = FALSE;
  current_device_ptr->DeviceBeingAccessed = FALSE;
  current_device_ptr->NumRetries = 1;
  current_device_ptr->ContinueAfterError = TRUE;
  current_device_ptr->DeviceAccessFailure = FALSE;
  current_device_ptr->PollInterval = GRAIN*FACTOR;
  current_device_ptr->DaemonAccess = FALSE;
  current_device_ptr->NumberOfCommands = 0;
  ReleaseDataport(dp);

  PollModeString=NULL;
  PollRecoverString=NULL;
  ReadCommandList.length = 0;
  devres_table[0].resource_adr=&(current_device_ptr->PollOnInitialise);
  devres_table[1].resource_adr=&(PollModeString);
  devres_table[2].resource_adr=&(current_device_ptr->NumRetries);
  devres_table[3].resource_adr=&(current_device_ptr->ContinueAfterError);
  devres_table[4].resource_adr=&(current_device_ptr->PollInterval);
  devres_table[5].resource_adr=&(ReadCommandList);
  devres_table[6].resource_adr=&(PollRecoverString);
  devres_table[7].resource_adr=&(current_device_ptr->PollForgetMultiple);
#if defined(EBUG)
  fprintf(stderr,"before db_getresource\n");
#endif

  status = db_getresource (current_device_ptr->DeviceName,
                           devres_table,
                           devres_tab_size,
                           error);
#if defined(EBUG)
  fprintf(stderr,"after db_getresource\n");
#endif
  if (status!=DS_OK)
  {
#if defined(EBUG)
    fprintf(stderr,"before 2nd db_getresource\n");
#endif
    status = db_getresource (current_device_ptr->DeviceName,
                             devres_table,
                             devres_tab_size,
                             error);
#if defined(EBUG)
    fprintf(stderr,"after 2nd db_getresource\n");
#endif
    if (status!=DS_OK)
    {
      if (found == FALSE)
           store_status(savcount,status,*error);
      else
        store_status(count,status,*error);

#if defined(EBUG)
     fprintf(stderr,"db_getresource failed\n");
#endif

    }
  }
  current_device_ptr->PollMode=READOUT;
  if (PollModeString != NULL)
  {
    if (strcmp(PollModeString,"SURVEY")==0)
      {
        current_device_ptr->PollMode=SURVEY;
      }
    free(PollModeString);
  }
  current_device_ptr->PollRecoverMode=DRIFT;
  if (PollRecoverString != NULL)
  {
    if (strcmp(PollRecoverString,"FORGET")==0)
    {
      current_device_ptr->PollRecoverMode=FORGET;
    }
    if (strcmp(PollRecoverString,"CATCHUP")==0)
    {
      current_device_ptr->PollRecoverMode=CATCHUP;
    }
    free(PollRecoverString);
  }
#if defined(EBUG)
  fprintf(stderr,"resources for %s set to :\n",current_device_ptr->DeviceName);
  fprintf(stderr,"  PollOnInitialise   : ");
#endif
  if (current_device_ptr->PollOnInitialise==TRUE)
  {
#if defined (EBUG)
    fprintf(stderr,"TRUE\n");
#endif
  }
  else
  {
#if defined (EBUG)
    fprintf(stderr,"FALSE\n");
#endif
  }
#if defined (EBUG)
  fprintf(stderr,"  PollMode           : ");
#endif
  if (current_device_ptr->PollMode==READOUT)
  {
#if defined (EBUG)
     fprintf(stderr,"READOUT\n");
#endif
  }
  else
  {
#if defined (EBUG)
     fprintf(stderr,"SURVEY\n");
#endif
  }
#if defined (EBUG)
  fprintf(stderr,"  NumRetries         : ");
  fprintf(stderr,"%d\n",current_device_ptr->NumRetries);
  fprintf(stderr,"  ContinueAfterError : ");
#endif
  if (current_device_ptr->ContinueAfterError==TRUE)
  {
#if defined (EBUG)
    fprintf(stderr,"TRUE\n");
#endif
  }
  else
  {
#if defined (EBUG)
    fprintf(stderr,"FALSE\n");
#endif
  }
#if defined (EBUG)
  fprintf(stderr,"  PollInterval      : ");
  fprintf(stderr,"%d\n",current_device_ptr->PollInterval);
#endif

  /* ROUNDING MODIFICATION */
  /*
  current_device_ptr->PollInterval=current_device_ptr->PollInterval/10;
  */
  round_interval (&current_device_ptr->PollInterval);

#if defined (EBUG)
  fprintf(stderr,"  Rounded PollInterval   : ");
  fprintf(stderr,"%d\n",current_device_ptr->PollInterval);
  /* END OF ROUNDING MODIF */
  fprintf(stderr,"  PollRecoverMode    : ");
#endif
  if (current_device_ptr->PollRecoverMode==CATCHUP)
  {
#if defined (EBUG)
     fprintf(stderr,"CATCHUP\n");
#endif
  }
  else if (current_device_ptr->PollRecoverMode==FORGET)
  {
#if defined (EBUG)
     fprintf(stderr,"FORGET\n");
#endif
  }
  else
  {
#if defined (EBUG)
     fprintf(stderr,"DRIFT\n");
#endif
  }

  if (current_device_ptr->PollForgetMultiple<1)
  {
    current_device_ptr->PollForgetMultiple=1;
  }
  current_device_ptr->PollForgetTime=current_device_ptr->PollForgetMultiple*
                                     current_device_ptr->PollInterval;
#if defined (EBUG)
  fprintf(stderr,"  PollForgetInterval : ");
  fprintf(stderr,"%d\n",current_device_ptr->PollForgetMultiple);
  fprintf(stderr,"  PollForgetTime : ");
  fprintf(stderr,"%d\n",current_device_ptr->PollForgetTime);
#endif


  if (current_device_ptr->PollOnInitialise==TRUE)
  {
    current_device_ptr->BeingPolled=TRUE;
  }
  else
  {
    current_device_ptr->BeingPolled=FALSE;
  }


  /*********************************************************/
  /**  in case of device has already been implemented :   **/
  /**  command_list reseting                              **/
  /*********************************************************/

  if (found == TRUE) 
     free_command(current_device_ptr->NumberOfCommands,current_device_ptr->CommandList);

  current_device_ptr->CommandList=NULL;
  current_device_ptr->NumberOfCommands = ReadCommandList.length;

#if defined(EBUG)
  fprintf(stderr,"commandlist.length=%d\n",ReadCommandList.length);
#endif

  /************************************/
  /** if new command_list not NULL : **/
  /************************************/
 
  if (ReadCommandList.length>0 )
  {

#if defined(VOIDTYPE)
    printf ("before store commands with length = %d\n",ReadCommandList.length);
#endif
    for (count=0;count<ReadCommandList.length;count++)
    {
    
        status = store_command(ReadCommandList.sequence[count],
                               D_VOID_TYPE,
                               &(current_device_ptr->CommandList),
		               error);
	if (status == DS_NOTOK)
	{
#if defined(VOIDTYPE)
          printf ("store_command failed - error\n",*error);
#endif
          free(current_device_ptr->DeviceName); 
	  current_device_ptr->DeviceName = NULL;  
	  no_of_device--;
    	  for (count=0;count<ReadCommandList.length;count++)
    		free(ReadCommandList.sequence[count]);
    	  free(ReadCommandList.sequence);
	  *error=DevErr_InsufficientMemory;
	  return (DS_NOTOK);
	}
    }
    
    for (count=0;count<ReadCommandList.length;count++)
    	free(ReadCommandList.sequence[count]);
    free(ReadCommandList.sequence);


#if defined(VOIDTYPE)
    printf ("before get_types\n");
#endif
    status = get_types (current_device_ptr,error);
#if defined(VOIDTYPE)
    printf ("after get_types\n");
#endif
    if (status != DS_OK)
    {
/* Commented by ET (03/01/95)
      free(current_device_ptr->DeviceName); 
      current_device_ptr->DeviceName = NULL;  
      return(DS_NOTOK);*/
    }

  /* COMMAND LIST CREATED, TYPES RETRIEVED, SO CALL PROC FOR DC_OPEN COMMAND */

     if (found == TRUE)
     {
	current_device_ptr->DCInitialised = FALSE;
        if (current_device_ptr->DeviceCounter > 1000)
	   current_device_ptr->DeviceCounter = 1;
	else
	   current_device_ptr->DeviceCounter += 1;
     }
     if ( current_device_ptr->DeviceImported == TRUE &&
          current_device_ptr->TypesRetrieved == TRUE &&
          current_device_ptr->DCInitialised  == FALSE &&
          current_device_ptr->PollMode       == READOUT)
/* Added READOUT test 281092 */
     {
       status = open_dc (current_device_ptr,error);
     }

  }                      /***************************************/ 
  else                   /**  else , new command_list is NULL  **/
  {                      /***************************************/ 
    if (found == FALSE)
    {
      /* if (no_of_device != savcount) current_device_ptr->DeviceName = NULL; */
      free(current_device_ptr->DeviceName); 
      current_device_ptr->DeviceName = NULL;  
      no_of_device--;
    }
#if defined(TRACE)
    fprintf(stderr,"endf d_initdev()\n");
#endif
    *error = DevErr_CommandNotImplemented;
    return(DS_NOTOK);
  }

  /** INSERT NEW DEVICE IN POLL LIST **/
  if (found == FALSE)
  {
    if (current_device_ptr->DeviceCounter == 0) 
       current_device_ptr->DeviceCounter = 1;
    else
    {
     if (current_device_ptr->DeviceCounter > 1000) 
        current_device_ptr->DeviceCounter = 1;
     else
        current_device_ptr->DeviceCounter += 1;
    } 
      counter = current_device_ptr->DeviceCounter;
      counter = counter << ID_COUNT;
      
      dev_id = counter | savcount;
     

    new_element = get_element (savcount);
    if (new_element == NULL)
    {
      free(current_device_ptr->DeviceName); 
      current_device_ptr->DeviceName = NULL; 
      no_of_device--;
      *error = DevErr_InsufficientMemory;
      return (DS_NOTOK);
    }
    insert_device (new_element, last_poll);
  }
  /* DISCRETE TIMES MODIFICATION */
  /* SETS NPT TO NEXT SECOND */

  get_next_poll_time (&current_device_ptr->NextPollTime,get_time(),0);

  return(DS_OK);
}


/****************************************************************************/
long d_removedev(long *error)
{
 int found,indexvalue;
 long status,ddid,counter;
 dc_error dcerror;
 unsigned int i,j,l,new_memory,num_resource;
 static daemon_device_struct_type *current_device_ptr,*next_device_ptr; 
 register int count,savcount,sav;
 char device_name[60],allocstr[80],f_tmp[60],pourcen[2];
 char *dev_name[60];
 devserver devicehandle; 
 command_list *current_command,*next_command;
 FILE *fileptr;
 char *tmp_f,*str;
 char **res_name;
 struct poll_list_type *current_element,*prev_element,*next_element;

 
 AccessDataport(dp);
  strcpy(device_name,dipc->string);
 ReleaseDataport(dp);


 l = strlen(device_name);
 for ( i=0 ; i<l ; i++)
     device_name[i] = tolower(device_name[i]); 
  
 found = FALSE;
 count = FIRST_DEVICE;
 i = no_of_device;
 
 while (count<=i)
 {
  if (device_data[count].DeviceName != NULL)
  {
   if (strcmp(device_name,device_data[count].DeviceName) == 0)
   {
    
    found = TRUE;
    dev_id = dev_id >> ID_COUNT;
    counter = dev_id & ID_MASK;
    break;
   }
   else
      count++;
  }
  else
  {
    i++;
    count++;
  } 
 }

 if ((found == TRUE) & (counter == device_data[count].DeviceCounter))
 {
  
   /*******************************************************/
   /**  work on the daemon_device_struct_type structure  **/
   /*******************************************************/

  
   /**  keep a pointer to the structure  **/
   
   current_device_ptr = &(device_data[count]);
   savcount = count ;  
   devicehandle = device_data[count].DeviceHandle;


   /**  delete the daemon_device_structure pointed by  **/
   /**  current_device_ptr                             **/

   free(current_device_ptr->DeviceName); 
   free_command(current_device_ptr->NumberOfCommands,(current_device_ptr->CommandList)); 
   current_device_ptr->DeviceName = NULL;   
   if (current_device_ptr->DeviceCounter > 1000)
      current_device_ptr->DeviceCounter = 1;
   else
      current_device_ptr->DeviceCounter += 1;
 
 
   /*********************************************/
   /**  work on the poll_list_type structure   **/
   /*********************************************/
 
 
   /**  poll_list up to date   **/

   current_element = first_poll;

   /**  due to the FOUND = true , don't need to test the last_poll position  **/ 

   while ( (current_element->index != savcount) )
   {
    prev_element = current_element;
    current_element = current_element->next;
   }

   if (current_element == first_poll)
   {
    prev_element = current_element;
    if (current_element->next != NULL) /* first device but not the only one */
    {
     current_element = current_element->next;
     current_element->previous = NULL;
     first_poll = current_element;
     indexvalue = prev_element->index; 
    }
    else  /* Case of an only one device in the index */
    {
     first_poll = NULL;
     last_poll = NULL; 
    }
    free(prev_element);
   } 
   else if (current_element == last_poll) /* last device in the index */ 
   {
    prev_element->next = NULL;
    last_poll = prev_element;
    indexvalue = current_element->index;
    free(current_element);
   }
   else /* device in the middle of the index */ 
   {
    next_element = current_element->next;
    next_element->previous = prev_element;
    prev_element->next = next_element;
    indexvalue = current_element->index; 
    free(current_element);
   } 

   no_of_device -=1;   

   /*************************************************/
   /**  delete the device resources from database  **/
   /*************************************************/

   l = strlen(device_name);
   for ( i=0 ; i<l ; i++)
       device_name[i] = toupper(device_name[i]); 

    res_name = (char **) devres_name;
    num_resource = sizeof(devres_name)/sizeof(char**);

    if (db_delresource(device_name,res_name,num_resource,error) != 0)
    {
#ifdef PRINT
     printf("db_delresource failed : %s %s \n",device_name,dev_error_str(error));
     fflush(stdout);
#endif
    *error = DbErr_ResourceNotDefined;
    return(DS_NOTOK);
    }

 
   /********************************************/ 
   /**  close data collector for this device  **/ 
   /********************************************/ 
   
   l = strlen(device_name);
   for ( i=0 ; i<l ; i++ )
       device_name[i] = tolower(device_name[i]);

   dev_name[0] = device_name;
   j = 1;
 
 
   status = dc_close(dev_name,j,&dcerror);
   if (status == DS_NOTOK)
   {
    *error = DcErr_CantCloseDcForDevice;
    return(DS_NOTOK);
   } 
  
 
 
 
   /**********************/ 
   /**  device freeing  **/    
   /**********************/ 
 

   if (devicehandle != NULL)
   {
     status = dev_free(devicehandle,error);
     if (status == DS_NOTOK)
     {
      *error = DevErr_CannotFreeDevice;
      return(DS_NOTOK);
     }
   }
   else 
     return(DS_NOTOK);

   return(DS_OK);
   
   }
   else 
   {
     *error = DevErr_DeviceOfThisNameNotServed;
     return(DS_NOTOK); 
   }

}


/****************************************************************************/
long check_status(long *d_status)
{
  	int command,dipc_status;
  	long status,error;
  	static int old_status=D_ERROR;

/*
#if defined(TRACE)
   fprintf(stderr,"check_status()\n");
#endif
*/
  	AccessDataport(dp);
   	dipc_status = *d_status;
  	ReleaseDataport(dp);

  	switch (dipc_status)
	{
    	case D_INIT :
		if (old_status!=dipc_status)
		{
#if defined(EBUG)
                         fprintf(stderr,"D_INIT\n");
#endif
                }
             	AccessDataport(dp);
            	*d_status = D_READY;
               	ReleaseDataport(dp);
             	break;

   	 case D_READY : 
	 	if (old_status!=dipc_status)
                {
#if defined(EBUG)
                         fprintf(stderr,"D_READY\n");
#endif
           	}
          	break;

    	case D_NEW : 
		if (old_status!=dipc_status)
            	{
#if defined(EBUG)
                         fprintf(stderr,"D_NEW\n");
#endif
           	}
               	AccessDataport(dp);
           	command=dipc->command;
              	*d_status=D_TAKEN;
            	ReleaseDataport(dp);
               	status=proc_command(command);
              	break;

    	case D_TAKEN :
    		if (old_status!=dipc_status)
                {
#if defined(EBUG)
                         fprintf(stderr,"D_TAKEN\n");
#endif
              	}
            	break;

    	case D_PROCESSED : 
		if (old_status!=dipc_status)
              	{
#if defined(EBUG)
                         fprintf(stderr,"D_PROCESSED\n");
#endif
               	}
          	break;

    	case D_ERROR :
		if (old_status!=dipc_status)
             	{
#if defined(EBUG)
                         fprintf(stderr,"D_ERROR\n");
#endif
              	}
           	break;

    	case D_CLEARED :
    		if (old_status!=dipc_status)
                {
#if defined(EBUG)
                         fprintf(stderr,"D_CLEARED\n");
#endif
             	}
         	AccessDataport(dp);
             	*d_status=D_READY;
            	ReleaseDataport(dp);
              	break;

    	default : 
		if (old_status!=dipc_status)
              	{
#if defined(EBUG)
                         fprintf(stderr,"UNKNOWN\n");
#endif
             	}

#if defined(TRACE)
   		fprintf(stderr,"end check_status()\n");
#endif

            	old_status=dipc_status;

              	error=DevErr_UnrecognisedState; /* DevErr_DataportStatus */
             	store_status(no_of_device,DS_NOTOK,error);

               	return (DS_NOTOK);

  	}

  	old_status=dipc_status;

#if defined(TRACE)
   	fprintf(stderr,"end check_status()\n"); 
#endif

  	return(DS_OK);
}


/****************************************************************************/
long ddid_valid()
{

  long error,ddid;

#if defined(TRACE)
  fprintf(stderr,"ddid_valid()\n");
#endif

  ddid = dev_id & ID_MASK;
  AccessDataport(dp);
  if (ddid<(FIRST_DEVICE-1) || ddid>((long)no_of_device))
  {
    sprintf(dipc->string,"ddid %d is not valid.\n",dev_id);
    ReleaseDataport(dp);

    error=DevErr_ValueOutOfBounds;
    store_status(no_of_device,DS_NOTOK,error);

#if defined(TRACE)
    fprintf(stderr,"end ddid_valid()\n");
#endif

    return (DS_NOTOK);
  }
  else
  {
    ReleaseDataport(dp);

#if defined(TRACE)
    fprintf(stderr,"end ddid_valid()\n");
#endif

    return (DS_OK);
  }
}


/****************************************************************************/
long d_on(long *error)
{

#if defined(TRACE)
  fprintf(stderr,"d_on()\n");
#endif

  running=TRUE;

  *error = 0;

#if defined(TRACE)
  fprintf(stderr,"end d_on()\n");
#endif

  return (DS_OK);
}


/****************************************************************************/
long d_off(long *error)
{

#if defined(TRACE)
  fprintf(stderr,"d_off()\n");
#endif

  running=FALSE;

  *error = 0;

#if defined(TRACE)
  fprintf(stderr,"end d_off()\n");
#endif

  return (DS_OK);
}


/****************************************************************************/
long d_accessstatus(long *error)
{
  static char str[255];
  long ddid,counter;

#if defined(TRACE)
  fprintf(stderr,"d_accessstatus()\n");
#endif

  if (ddid_valid()!=DS_OK)
  {
#if defined(TRACE)
   fprintf(stderr,"end d_accessstatus()\n");
#endif
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }

  AccessDataport(dp);
   ddid=dipc->data[0];
  ReleaseDataport(dp);
  
  counter = ddid >> ID_COUNT;
  counter = counter & ID_MASK;
  ddid = ddid & ID_MASK;


  /* poll status required for daemon itself */
  if (ddid==0) 
  {
    AccessDataport(dp);
    strcpy(dipc->string,"No device access status info yet available for daemon\nTry using DevStatus\n");
    ReleaseDataport(dp);
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  else if (counter == device_data[ddid].DeviceCounter)
  {
   AccessDataport(dp);
    dipc->data[0]=(long)(device_data[ddid].LastCommandStatus);
    dipc->data[1]=(long)(device_data[ddid].LastErrorCode);
    dipc->data[2]=(long)(device_data[ddid].ContinueAfterError);
   ReleaseDataport(dp);

   *error = 0;

#if defined(TRACE)
   fprintf(stderr,"end d_accessstatus()\n");
#endif

   return(DS_OK);
  }
  else 
  {
   *error = DevErr_AccessDenied;
   return(DS_NOTOK);
  }
}



/****************************************************************************/
long d_status(long *error)
{
  static char str[511];
  static char temp[255];
  long status;
  int no_survey=0,no_polled=0,no_imported=0,no_dcinit=0,no_retrieved=0;
  register int i;
  long min_int=9999999, max_int=0, sum_int=0;
  int count=0;

  for (i=FIRST_DEVICE;i<=no_of_device;i++)
  {
    if (device_data[i].PollMode==SURVEY)
      no_survey++;
    if (device_data[i].BeingPolled==TRUE)
    {
      sum_int += device_data[i].PollInterval;
      no_polled++;
    }
    if (device_data[i].DeviceImported==TRUE)
      no_imported++;
    if (device_data[i].TypesRetrieved==TRUE)
      no_retrieved++;
    if (device_data[i].DCInitialised==TRUE)
      no_dcinit++;
    if (device_data[i].PollInterval<min_int)
      min_int = device_data[i].PollInterval;
    if (device_data[i].PollInterval>max_int)
      max_int = device_data[i].PollInterval;
  }
  if (running==TRUE)
  {
    sprintf(str,"DAEMON ON/RUNNING\n");
  }
  else
  {
    sprintf(str,"DAEMON OFF/STOPPED\n");
  }
  sprintf(temp,"NUM OF DEV: %d: \n",no_of_device);
  strcat(str,temp);
  sprintf(temp," %d IMPORTED\n",no_imported);
  strcat(str,temp);
  sprintf(temp," %d TYPED\n",no_retrieved);
  strcat(str,temp);
  sprintf(temp," %d DC_OPENED\n",no_dcinit);

  strcat(str,temp);
  sprintf(temp," %d SURVEY, %d READOUT\n",no_survey, no_of_device-no_survey);
  strcat(str,temp);
  sprintf(temp," %d POLLED\n",no_polled, no_of_device-no_polled);
  strcat(str,temp);
  if (no_polled == 0)
  {
    no_polled=1;
  }
  sprintf(temp,"POLL INTERVALS : MIN %.1fs, MAX %.1fs, AVG %.1fs\n",
		(float)(min_int/(FACTOR*1.0)), (float)(max_int/(FACTOR*1.0)), (float)(sum_int/(FACTOR*1.0*no_polled)));
  strcat(str,temp);
  if (dc_problems == TRUE)
  {
    long i;
    sprintf(temp,"DC PROBLEMS : ERR %d : ",dc_error_num);
    strcat(str,temp);

    strcpy(temp,"NO INFO\n");
    for(i=0;i<MAX_DEVERR;i++)
      if(dc_error_num==DevErr_List[i].dev_errno)
         {
           sprintf(temp,"  %s\n",DevErr_List[i].message);
	 }
    strcat(str,temp);
  }
  else
  {
    if (dc_tries==0)
    {
      sprintf(temp,"DATA COLLECTOR NOT YET USED\n");
    }
  }
  count=0;
  for (i=0;i<dc_tries;i++)
  {
    if (dc_oks[i]==DS_OK) count++;
  }
  if (dc_tries==0) dc_tries=1;
  sprintf(temp,"DC COMM %.1f%% : ",(float)(100*(((float)count)/dc_tries)));
  strcat(str,temp);
  sprintf(temp,"LAST ERR %d\n",dc_last_error);
  strcat(str,temp);
  count=0;
  for (i=0;i<dpg_tries;i++)
  {
    if (dpg_oks[i]==DS_OK) count++;
  }
  if (dpg_tries==0) dpg_tries=1;
  sprintf(temp,"DEV COMM %.1f%% : ",(float)(100*(((float)count)/dpg_tries)));
  strcat(str,temp);
  sprintf(temp,"LAST ERR %d\n",dpg_last_error);
  strcat(str,temp);

  AccessDataport(dp);
   strncpy(dipc->string,str,255);
  ReleaseDataport(dp);
#if defined (EBUG)
  fprintf(stderr,"dipr->string = %s\n",dipc->string);
#endif

  *error = 0;
  return(DS_OK);
}


/****************************************************************************/
long d_pollstatus(long *error)
{
  static char str[255];
  long ddid,counter;
  long status;

#if defined(TRACE)
   fprintf(stderr,"d_pollsstatus()\n");
#endif

  if (ddid_valid()!=DS_OK)
  {

#if defined(TRACE)
    fprintf(stderr,"end d_pollsstatus()\n");
#endif

    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  AccessDataport(dp);
   ddid=dipc->data[0];
  ReleaseDataport(dp);
  
  counter = ddid >> ID_COUNT;
  counter = counter & ID_MASK;
  ddid = ddid & ID_MASK;
  
  if (ddid==0) /* status required for daemon itself - should call d_status */
  { 
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }   /* Data transfer via dataport */
  else if (counter == device_data[ddid].DeviceCounter)
  {
   AccessDataport(dp);
    strcpy(dipc->string,device_data[ddid].DeviceName); /* Copy Dev Name */
    dipc->data[0]=(long)(device_data[ddid].BeingPolled);
    dipc->data[1]=(long)(device_data[ddid].PollInterval);
    dipc->data[2]=(long)(device_data[ddid].TimeLastPolled);
    dipc->data[3]=(long)(device_data[ddid].PollMode);  /* ??? */
    dipc->data[4]=(long)(device_data[ddid].DeviceAccessFailure);
   ReleaseDataport(dp);
#if defined(TRACE)
   fprintf(stderr,"end d_pollsstatus()\n");
#endif
   *error = 0;
   return(DS_OK);
  }
  else 
  {
    *error = DevErr_AccessDenied;
    return(DS_NOTOK);
  }
}




/****************************************************************************/
long d_state(long *error)
{

#if defined(TRACE)
  fprintf(stderr,"d_state()\n");
#endif

  AccessDataport(dp);
   if (running==TRUE)
   {
     strcpy(dipc->string,"RUNNING\n");
     dipc->data[0]=DEVON;
   }
   else
   {
     strcpy(dipc->string,"HALTED\n");
     dipc->data[0]=DEVOFF;
   }
  ReleaseDataport(dp);

  *error = 0;

#if defined(TRACE)
  fprintf(stderr,"end d_state()\n");
#endif

  return(DS_OK);
}


/****************************************************************************/
long d_ident(long *error)
{
  static char device_name[255];
  register int count;
  long counter;
  int i;

#if defined(TRACE)
  fprintf(stderr,"d_ident()\n");
#endif

  AccessDataport(dp);
   strcpy(device_name,dipc->string);
  ReleaseDataport(dp);

#if defined(EBUG)
  fprintf(stderr,"find ident for %s\n",device_name);
#endif

  count=FIRST_DEVICE;
  i = no_of_device;


  while (count<=i)
  {
   if (device_data[count].DeviceName != NULL)
   {
      counter = device_data[count].DeviceCounter;
      counter = counter << ID_COUNT;
      dev_id = counter | (long)count;
  
/*      printf("dev_id != %d \n",dev_id);
      fflush(stdout);*/

    if (strcmp(device_name,device_data[count].DeviceName) == 0)
    {
      counter = device_data[count].DeviceCounter;
      counter = counter << ID_COUNT;
      dev_id = counter | (long)count;
      
/*      printf("dev_id == %d \n",dev_id);
      fflush(stdout);*/
      
      AccessDataport(dp);
       dipc->data[0]= dev_id;
      ReleaseDataport(dp);
   
#if defined(TRACE)
      fprintf(stderr,"end d_ident()\n");
#endif

      return(DS_OK);
    }
    else
        count++;
   }
   else
   {
     i++;
     count++;
   } 
  }
  AccessDataport(dp);
   sprintf(dipc->string,"%s not served by this daemon\n",device_name);
   dipc->data[0]=(long)0;
  ReleaseDataport(dp);

  *error=DevErr_DeviceTypeNotRecognised;      /*** TO BE CHANGED ***/

#if defined(TRACE)
  fprintf(stderr,"end d_ident()\n");
#endif

  return (DS_NOTOK);
}


/****************************************************************************/
long d_name(long *error)
{
  static char device_name[255];
  register int count;
  long ddid;

#if defined(TRACE)
  fprintf(stderr,"d_name()\n");
#endif

  AccessDataport(dp);
   ddid=dipc->data[0];
  ReleaseDataport(dp);
    
  ddid = ddid & ID_MASK;
  if (ddid==0)
  {
    /* means the poller itself */
    AccessDataport(dp);
      strcpy(dipc->string,"DAEMON");
    ReleaseDataport(dp);
#if defined(TRACE)
    fprintf(stderr,"end d_name()\n");
#endif
    return (DS_OK);
  }

  if (ddid_valid()!=DS_OK)
  {

    /* means the ddid was invalid */
    AccessDataport(dp);
      strcpy(dipc->string,"INVALID DDID");
    ReleaseDataport(dp);
#if defined(TRACE)
    fprintf(stderr,"end d_name()\n");
#endif
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }


  if (device_data[ddid].DeviceName == NULL)
  {
    *error = DbErr_DeviceNotDefined;
    return(DS_NOTOK);
  }
  else
  {
   AccessDataport(dp);
     strncpy(dipc->string,device_data[ddid].DeviceName,255);
   ReleaseDataport(dp);
#if defined(TRACE)
  fprintf(stderr,"end d_name()\n");
#endif
   return(DS_OK);
  }

}


/****************************************************************************/
long d_startpoll(long *error)
{
  long ddid,counter;
  long status;

#if defined(TRACE)
  fprintf(stderr,"d_startpoll()\n");
#endif

  AccessDataport(dp);
   ddid=dipc->data[0];
  ReleaseDataport(dp);
  
 counter = ddid >> ID_COUNT;
 counter = counter & ID_MASK;

 ddid = ddid & ID_MASK;
 
  if (ddid==0)
  {
    status = d_on(error); /* ddid=0 -> start daemon polling */
    return (status);
  }
  if (ddid_valid()!=DS_OK)
  {
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  if (counter == device_data[ddid].DeviceCounter)
  {
   device_data[ddid].BeingPolled=TRUE;
#if defined(EBUG)
   fprintf(stderr,"%s - start polling\n",device_data[ddid].DeviceName);
#endif
   *error = 0;
   return (DS_OK);
  }
  else 
  {
    *error = DevErr_AccessDenied;
    return(DS_NOTOK);
  }
}


/****************************************************************************/
long d_stoppoll(long *error)
{
  long ddid,counter;
  long status;

#if defined(TRACE)
  fprintf(stderr,"d_stoppoll()\n");
#endif

  AccessDataport(dp);
   ddid=dipc->data[0];
  ReleaseDataport(dp);
  
  counter = ddid >> ID_COUNT;
  counter = counter & ID_MASK;

  ddid = ddid & ID_MASK;
  
  if (ddid == 0)   
  {
    status = d_off(error); /* ddid=0 -> stop daemon polling */
    return (status);
  }
  if (ddid_valid()!=DS_OK)
  {
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  if (counter == device_data[ddid].DeviceCounter)
  {
    device_data[ddid].BeingPolled=FALSE;
#if defined(EBUG)
    fprintf(stderr,"%s - stop polling\n",device_data[ddid].DeviceName);
#endif
    *error = 0;
    return (DS_OK);
  }
  else 
  {
    *error = DevErr_AccessDenied;
    return(DS_NOTOK);
  }
}



/****************************************************************************/
long d_changeinterval(long *error)
{
  long ddid,counter; 
  long interval;

#if defined(TRACE)
  fprintf(stderr,"d_changeinterval()\n");
#endif

  if (ddid_valid()!=DS_OK)
  {

#if defined(TRACE)
    fprintf(stderr,"end d_changeinterval()\n");
#endif
    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  AccessDataport(dp);
   dev_id=dipc->data[0];
   interval=(int)(dipc->data[1]);
  ReleaseDataport(dp);
 
  ddid = dev_id & ID_MASK;
 
  if (interval>=0 & ddid>0) 
  {
  /*
    interval=interval/10;
  */
    round_interval (&interval);
    device_data[ddid].PollInterval=interval;
    open_dc(&(device_data[ddid]),error);

#if defined(TRACE)
    fprintf(stderr,"end d_changeinterval()\n");
#endif

    *error = 0;
    return (DS_OK);
  }
  else
  {

    *error=DevErr_ValueOutOfBounds;
    store_status(no_of_device,DS_NOTOK,*error);

#if defined(TRACE)
    fprintf(stderr,"end d_changeinterval()\n");
#endif

    return (DS_NOTOK);
  }
}


/****************************************************************************/
long d_definemode(long *error)
{
  long ddid,counter;
  int mode;

#if defined(TRACE)
  fprintf(stderr,"d_define_mode()\n");
#endif

  if (ddid_valid()!=DS_OK)
  {

#if defined(TRACE)
    fprintf(stderr,"end d_define_mode()\n");
#endif

    *error = DevErr_DeviceIllegalParameter;
    return (DS_NOTOK);
  }
  AccessDataport(dp);
   ddid=dipc->data[0];
   mode=(int)(dipc->data[1]);
  ReleaseDataport(dp);
  
  counter = ddid >> ID_COUNT;
  counter = counter & ID_MASK;
  ddid = ddid & ID_MASK;
  
  if ((mode==SURVEY || mode==READOUT) & (counter == device_data[ddid].DeviceCounter))
  {
    if (device_data[ddid].DeviceCounter > 1000)
       device_data[ddid].DeviceCounter = 1;
    else
       device_data[ddid].DeviceCounter += 1;
    device_data[ddid].PollMode=mode;
#if defined(TRACE)
    fprintf (stderr,"set mode to %d\n",mode);
    fprintf(stderr,"end d_define_mode()\n");
#endif
    *error = 0;
    return (DS_OK);
  }
  else
  {
    *error=DevErr_CommandIgnored;
    store_status(no_of_device,DS_NOTOK,*error);
#if defined(TRACE)
    fprintf(stderr,"end d_define_mode()\n");
#endif
    return (DS_NOTOK);
  }
}


/****************************************************************************/
long proc_command(int command)
{
  long status,error;

#if defined(TRACE)
  fprintf(stderr,"proc_command()\n");
#endif

  AccessDataport(dp);
  if (dipc->status != D_TAKEN)
  {

#if defined(TRACE)
    fprintf(stderr,"before ReleaseDataport end proc_command() (status not D_TAKEN)\n");
#endif
    ReleaseDataport(dp);
#if defined(TRACE)
    fprintf(stderr,"end proc_command() (status not D_TAKEN)\n");
#endif

    return(DS_NOTOK);   /* status invalid for procedure */
  }
  ReleaseDataport(dp);

  switch (command) {

  case DevOff                : status=d_off(&error);
                               break;

  case DevOn                 : status=d_on(&error);
                               break;

  case DevState              : status=d_state(&error);
                               break;

  case DevGetDeviceIdent     : status=d_ident(&error);
                               break;

  case DevGetDeviceName      : status=d_name(&error);
                               break;

  case DevInitialiseDevice   : status=d_initdev(&error);
                               break;

  case DevRemoveDevice       : status=d_removedev(&error);
                               break;

  case DevStartPolling       : status=d_startpoll(&error);
                               break;

  case DevStopPolling        : status=d_stoppoll(&error);
                               break;

  case DevChangeInterval    :  status=d_changeinterval(&error);
                               break;

  case DevDefineMode         : status=d_definemode(&error);
                               break;

  case DevStatus             : status=d_status(&error);
			       break;

  case DevPollStatus         : status=d_pollstatus(&error);
                               break;

  case DevAccessStatus       : status=d_accessstatus(&error);
                               break;

  }
  if (status != DS_OK)
  {
    AccessDataport(dp);
    dipc->status=D_ERROR;
    dipc->error=error;
    ReleaseDataport(dp);

#if defined(TRACE)
    fprintf(stderr,"end proc_command() (status not OK - D_ERROR)\n");
#endif

    return(DS_NOTOK);
  }
  else
  {
    AccessDataport(dp);
    dipc->status=D_PROCESSED;
    if (command == DevInitialiseDevice) dipc->data[0]=dev_id;
    ReleaseDataport(dp);

#if defined(TRACE)
    fprintf(stderr,"end proc_command() (status - D_PROCESSED)\n");
#endif

    return(DS_OK);
  }

}



/****************************************************************************/
int time_diff_calc (long time1,long time2,long *dT)
{
    	*dT = time1 - time2;

    	if ( *dT > (THRESHOLD/2) )
    	{
       		*dT = time2 + (THRESHOLD - time1);
       		return (FALSE);
    	}

    	if (*dT >=0)
    	{
       		return (TRUE);
    	}

    	*dT=-(*dT);
    	if ( *dT > (THRESHOLD/2) )
    	{
       		return (TRUE);
    	}
    	else
    	{
       		return (FALSE);
    	}
}


/****************************************************************************/
void get_next_poll_time (long *npt,long basetime,long increment)
{
  if (increment>0)
  {
     *npt=(FACTOR/GRAIN)*((basetime+increment)/(FACTOR/GRAIN));
  }
  else
  {
     *npt=(FACTOR/GRAIN)*((basetime+((FACTOR/GRAIN)-1))/(FACTOR/GRAIN));
  }
  while (*npt >= THRESHOLD)
  {
    *npt=*npt-THRESHOLD;
  }
}


/****************************************************************************/
long get_time()   /* CONVERTED TO USE MICROSECONDS */
                  /* RETURNS (long) of FACTORths of SECONDS */
{
  	long now_time;
#ifdef _OSK
  	time_t ti;
  	long tick;
  	short tick_rate;
  	short tick_num;
#else
  	struct timezone tzp;
#endif /* _OSK */
  	struct timeval  time_instant;

#if defined(TIME_DEBUG)
  	fprintf(stderr,"get_time()\n");
#endif

#ifdef _OSK
  	_os_getime(&ti,&((u_int32)tick));
  	tick_num = (short)tick;
  	tick_rate = (short)(tick >> 16);
  	time_instant.tv_sec = (long)ti;
  	time_instant.tv_usec = (1000000 / tick_rate) * tick_num;
#else
  	gettimeofday (&time_instant, &tzp);
#endif /* _OSK */

  	time_instant.tv_sec = time_instant.tv_sec -(SECONDS * (time_instant.tv_sec / SECONDS));
  	now_time=(long)((time_instant.tv_usec/FACTOR)+((time_instant.tv_sec)*FACTOR));

  	while (now_time > THRESHOLD)
  	{
   		now_time = now_time - THRESHOLD;
  	}

  	AccessDataport(dp);
   	dipc->timestamp=time((time_t *)0);
  	ReleaseDataport(dp);

#if defined(TIME_DEBUG)
  	fprintf(stderr,"end get_time()\n");
#endif

  	return((now_time));
}


/****************************************************************************/
long poll_device(int dev_num)
{
  command_list *current_command;
  daemon_device_struct_type *current_device_ptr;
  register int retries;
  long status,error,ddid;
  int i;

  DevOpaque Oargout;

  struct timeval  time_instant;
  struct timezone tzp;
  char timestring[256];
  struct tm *time_now;
  static int dprc=0,xdrf=0;
  char *fred;
  long btime,ttime,dpr_time,proc_time;


#if defined(TRACE)
  fprintf(stderr,"poll_device()\n");
  
  proc_time = get_time();
#endif

  
  ddid = dev_num & ID_MASK;
  
  if (device_data[dev_num].DeviceImported==FALSE)
  {
    status=import_device(dev_num);
    if (status!=DS_OK)
    {

#if defined(TRACE)
      fprintf(stderr,"endf poll_device() after %d\n",get_time()-proc_time);
#endif
      
      return(DS_NOTOK);
    }
  }
  if (device_data[dev_num].TypesRetrieved == FALSE)
  {
    status = get_types (&(device_data[dev_num]),&error);
    if (status!=DS_OK)
    {

#if defined(TRACE)
      fprintf(stderr,"endf poll_device() after %d\n",get_time()-proc_time);
#endif
      error = DevErr_CommandNotImplemented;
      return(DS_NOTOK);
    }
  }
  if ((device_data[dev_num].DCInitialised == FALSE) &
      (device_data[dev_num].PollMode==READOUT))
/* Added READOUT test 281092 */
  {
    status = open_dc (&(device_data[dev_num]),&error);
    if (status != DS_OK)
    {
      return (DS_NOTOK);
    }
  }
  current_device_ptr=&(device_data[dev_num]);
  current_command=current_device_ptr->CommandList;

#if defined(EBUG)
/*  fprintf(stderr,"to poll - %s\n",current_device_ptr->DeviceName); */
#endif

  while (current_command!=NULL)
  {
    retries=current_device_ptr->NumRetries;
    status=DS_NOTOK;
    while ((retries>=0) & (status!=DS_OK))
    {
      Oargout.length = 0;
      Oargout.sequence = NULL;

#if defined (EBUG)
      fprintf (stderr, "dpr call no %d\n",++dprc);
#endif
      btime=get_time();
#if defined (PUTGET)
      printf ("%s : putget_raw COM:%d REQ:%ld, BF:%ld ",current_device_ptr->DeviceName,current_command->command,current_device_ptr->NextPollTime,btime);
#endif
      error = 0;
      status = dev_putget_raw (current_device_ptr->DeviceHandle,
                               current_command->command,
                               NULL,D_VOID_TYPE,
                               &Oargout,current_command->command_type,
                               &error);
      ttime=get_time();
      if (btime < ttime)
        dpr_time=ttime-btime;
      else
        dpr_time=ttime+(THRESHOLD-btime);
#if defined (PUTGET)
      printf ("AF:%ld TKN:%ld ",ttime,dpr_time);

#endif

      if (status != DS_OK)
      {
	dpg_last_error = error;
        dpg_oks[dpg_cycle]=DS_NOTOK;
      }
      else
      {
        dpg_oks[dpg_cycle]=DS_OK;
      } 
      if (++dpg_cycle >= ERROR_CYCLE) dpg_cycle=0;
      if (dpg_tries<ERROR_CYCLE) dpg_tries++;

      if (status != DS_OK)
      {
#if defined (PUTGET)
        printf ("NOTOK %d\n",error);
	fflush(stdout);
#endif
        current_device_ptr->DeviceAccessFailure=TRUE;
	Oargout.length = 0;
	Oargout.sequence = NULL;
      }
      else
      {
#if defined (PUTGET)
        printf ("OK\n");
	fflush(stdout);
#endif
      }

#if defined (EBUG)
#ifndef _OSK
      gettimeofday (&time_instant, &tzp);
      time_now = localtime((time_t *)(&time_instant.tv_sec));
      strftime(timestring,255,"%a, %d %b %Y %X",time_now);
      fprintf(stderr,"TIME : %s.%ld : %ld.%ld\n",
		      timestring,time_instant.tv_usec,
		      time_instant.tv_sec,time_instant.tv_usec);
#endif
#endif
      store_status (dev_num,status,error);
      if (status!=DS_OK)
      {
#if defined (EBUG)
        printerror(error);
#endif
        retries--;
      }
    }
    if (status!=DS_OK)
    {
      if (current_device_ptr->ContinueAfterError==FALSE)
      {

        error=DevErr_RWAccessToDeviceFailed;
        store_status(dev_num,DS_NOTOK,error);

#if defined(EBUG)
        fprintf(stderr,"end poll_device() after %d\n",get_time()-proc_time);
#endif

        return(DS_NOTOK);
      }
    }
#if defined(RESULT)
    fprintf(stderr,"%s - comm %d - ",current_device_ptr->DeviceName,
                             current_command->command);
#endif
    if (current_device_ptr->PollMode==READOUT)
    {
#if defined(RESULT)
      fprintf(stderr,"readout");
#endif
      /* STORE DATA IN DATA COLLECTOR */

      /* dc code */
      status = queue_data (current_device_ptr->DeviceName,
	 	           current_device_ptr->DeviceHandle,
		           current_command->command,
		           error,
                           (int)dpr_time,
		           (int)(Oargout.length),
		           Oargout.sequence,
                           &error);
      /* end dc code */
    }
    else
    {
#if defined(RESULT)
      fprintf(stderr,"survey");
#endif
      free (Oargout.sequence); /* free stuff not needed later */
    }
#if defined(RESULT)
    fprintf(stderr," done.\n");
#endif
#if defined(RESULT)
    fprintf(stderr,"opaque call made\n");
    fprintf(stderr,"opaque output: length=%d\n",Oargout.length);
#endif
    current_command=current_command->next;
    current_device_ptr->DeviceAccessFailure=FALSE;
/*
#if defined (EBUG)
    fprintf (stderr, "free call no %d\n",++xdrf);
#endif
    if ( dev_xdrfree (D_OPAQUE_TYPE, &Oargout, &error) < 0 )
      {
        dev_printerror_no (SEND, "dev_xdrfree", error);
      }
*/
/* This use of the free statement is non-standard for the device server model.

   All other "frees" of argout data is done by dev_xdrfree, but the
   implementation for Opaque types is different, hence the use of a simple
   free statement. The API may be altered to make this fact invisible, but for
   the moment the following line is required. */

#ifdef DCNOTAV
    free (Oargout.sequence);
#endif

  }

#if defined(TRACE)
  fprintf(stderr,"end poll_device() after %d\n",get_time()-proc_time);
#endif

  return(DS_OK);
}


/****************************************************************************/
void user_sig(int sig)
{
  switch (sig)
  {
   case(SIGUSR1) : user_signal(SIGUSR1); 
	           break; 
   case(SIGINT) : signal_handler(SIGINT);
	          break; 
   case(SIGTERM) : signal_handler(SIGTERM);
	           break; 
   case(SIGALRM) : signal_handler(SIGALRM);
	           break; 
   default : break;
  }
}

/****************************************************************************/
int main(int argc,char *argv[])
{
  	long status,error;
  	long time_now;
  	daemon_device_struct_type *current_device_ptr;
  	int polldevice;
  	char *addr;
#ifdef _old_sun_code
#ifndef __cplusplus
  	int sig_mask;
  	struct sigvec sighand;
#endif
#endif /* _solaris */
  	struct timeval pause;
  	pid_t proc_pid;
  	long pid,i;

/* Close all the open file gotten from the ud_daemon which is a server.
   Start with fd=4 because I can't close fd number 3 !! (I don't know why) */

  	i = 4;
  	while (close(i) == 0)
  	{
  		i++;
  	}
  
#if (OSK || _OSK) 
  	intercept(user_sig);
#else
#ifdef _solaris
	sigset_t sigs_to_block;
	sigemptyset(&sigs_to_block);
	sigaddset(&sigs_to_block,SIGHUP);
	sigaddset(&sigs_to_block,SIGPIPE);
	
	sigprocmask(SIG_BLOCK,&sigs_to_block,NULL);
	
	struct sigaction sa;	
	sa.sa_flags = 0;
	sa.sa_handler = user_signal;
	sigemptyset(&sa.sa_mask);
	
	sigaction(SIGUSR1,&sa,0);
	
	sa.sa_handler = signal_handler;
	sigaction(SIGINT,&sa,0);
	sigaction(SIGTERM,&sa,0);
	sigaction(SIGALRM,&sa,0);
#ifdef __old_sun_code
  	sig_mask = sigmask(SIGHUP | SIGPIPE);
  	sigsetmask(sig_mask);

  	sighand.sv_handler = user_signal;
  	sighand.sv_mask = 0;
  	sighand.sv_flags = 0;

  	sigvec(SIGUSR1,&sighand,NULL);

  	sighand.sv_handler = signal_handler;
  	sigvec(SIGINT,&sighand,NULL);
  	sigvec(SIGTERM,&sighand,NULL);
  	sigvec(SIGALRM,&sighand,NULL);
#endif
#else
  	(void) signal(SIGHUP, SIG_IGN);              
  	(void) signal(SIGPIPE, SIG_IGN);    
  	(void) signal(SIGUSR1, user_signal);
  	(void) signal(SIGINT,  signal_handler);
  	(void) signal(SIGTERM, signal_handler);
  	(void) signal(SIGALRM, signal_handler);
#endif /* _solaris */
#endif /* _OSK */


	printf("UPDATE DAEMON POLLER RUNNING \n"); fflush(stdout);
  	pause.tv_sec = 0;
  	pause.tv_usec = 100;

  	first_poll=NULL;
  	last_poll=NULL;
  	if (argc<2)
  	{
		printf("no pid in command line\n");
    		exit (-1);
  	}
  	sscanf (argv[1],"%ld",&pid);

  	proc_pid = (pid_t) pid;

  	status = initialise_ipc(proc_pid);
  	if (status!=DS_OK)
  	{
    		fprintf(stderr,"Exiting - no ipc module %d\n",proc_pid);
    		exit(-1);
  	}

/* moving to inside initialise_ipc()
  AccessDataport(dp);
  dipc->poller_id=(long)getpid();
  ReleaseDataport(dp);
*/

  	printf ("UPDATE DAEMON POLLER INITIALISED\n"); fflush(stdout);

  	while (1)
  	{
    		time_now=get_time();
    		check_status(&(dipc->status));
    		if (running==TRUE)
    		{
      			dipc->signal_valid=TRUE;
  			pause.tv_sec = 0;
  			pause.tv_usec = 100;
      			select (0,0,0,0,&pause);
      			dipc->signal_valid=FALSE;
   		}
    		time_now=get_time();
    		check_next_device (time_now);
  	}


}

