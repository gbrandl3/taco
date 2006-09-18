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
 * File:        datac.c
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
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:41:09 $
 */

static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/datac.c,v 1.2 2006-09-18 22:41:09 jkrueger1 Exp $";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <DevServer.h>
#include <DevErrors.h>
#include <daemon.h>
#include <API.h>
#include <dc.h>
#ifndef OSK
#include <stdlib.h>
#endif

#define DEVICE_ENTRY_SIZE 2
#define COMMAND_ENTRY_SIZE 4
#define MAX_ENTRIES_IN_QUEUE 20

extern char dc_problems;
extern long dc_error_num;
extern long dc_last_error;
extern long dc_tries,dc_oks[ERROR_CYCLE],dc_cycle;

static int num_devices=0;
static unsigned int size_of_area=0;
static int num_of_entries=0;
static int size;

struct qcomm_info_table {   devserver   q_handle;
       	                    int         qcomm_num;
		            int         qcomm_area_size;};
typedef struct qcomm_info_table qcomm_info_table;


#ifndef DCNOTAV
  dc_dev_dat *dev_data;
#endif

qcomm_info_table *dev_comms;

void print_devdata();
long open_dc ();
extern long get_time();

/* function to store next command/device in queue ready for sending to dc */

/*** FUNCTION QUEUE_DATA ***/

long queue_data (char *name,devserver handle,int cmd,long cmd_error,
                 int cmd_time,int length,char *sequence,long *error)
{
  int i,found,qcomm,k;
  int current_device=0;

#ifdef DCNOTAV
  return (DS_OK);
#else

#if defined (TRACE)
  fprintf (stderr, "queue_data ()\n");
#endif
#if defined (EBUG)
  fprintf(stderr,"#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#\n");
  fprintf(stderr,"** THE QUEUE DATA ROUTINE FOR THE DATA COLLECTOR IS HERE *\n");
  fprintf(stderr,"#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#\n");
  fprintf(stderr,"NAME:     %s\n",name);
  fprintf(stderr,"HANDLE:   %d\n",(int)handle);
  fprintf(stderr,"CMD:      %d\n",cmd);
  fprintf(stderr,"CMD_ERROR:%d\n",cmd_error);
  fprintf(stderr,"CMD_TIME: %d\n",cmd_time);
  fprintf(stderr,"LENGTH:   %d\n",length);
#endif

  /* check if device has already command to send to dc in queue */
  i=0;
  found=FALSE;
  if (num_devices>0)
  {
    while (i<num_devices && found==FALSE)
    {
      if (dev_comms[i].q_handle==handle) /* using device handle as key */
      {
	found=TRUE;
	current_device=i;
      }
      else
      {
	i++;
      }
    }
  }

  if (found==FALSE)  /* must create new device entry */
  {
    num_devices++;
    if (num_devices>size_of_area) /* require more space for device list */
    {
      size_of_area +=DEVICE_ENTRY_SIZE;
      if (size_of_area == DEVICE_ENTRY_SIZE) /* first creation */
      {
        size = sizeof(qcomm_info_table);
#if defined (ALLOC)
  printf ("** QUEUE DATA dev comms CALLOCATION OF %d * %d BYTES \n", DEVICE_ENTRY_SIZE, size);
#endif
        dev_comms = (qcomm_info_table *)calloc(DEVICE_ENTRY_SIZE,size);
        if (dev_comms == NULL)
        {
          *error=DevErr_InsufficientMemory;
#if defined (TRACE)
          fprintf (stderr, "endf queue_data ()\n");
#endif
          return (DS_NOTOK);
        }
        size = sizeof(dc_dev_dat);
#if defined (ALLOC)
  printf ("QUEUE DATA dev data CALLOCATION OF %d * %d BYTES \n", DEVICE_ENTRY_SIZE,size);
#endif
        dev_data = (dc_dev_dat *)calloc(DEVICE_ENTRY_SIZE,size);
        if (dev_data == NULL)
        {
          *error=DevErr_InsufficientMemory;
#if defined (TRACE)
          fprintf (stderr, "endf queue_data ()\n");
#endif
          return (DS_NOTOK);
        }
      }
      else /* must reallocate */
      {
        size = size_of_area*sizeof(qcomm_info_table);
#if defined (ALLOC)
  printf ("QUEUE DATA dev comms REALLOCATION OF %d BYTES \n", size);
#endif
        dev_comms = (qcomm_info_table *)realloc(dev_comms,size);
        if (dev_comms == NULL)
        {
          *error=DevErr_InsufficientMemory;
#if defined (TRACE)
          fprintf (stderr, "endf queue_data ()\n");
#endif
          return (DS_NOTOK);
        }
        size = size_of_area*sizeof(dc_dev_dat);
#if defined (ALLOC)
  printf ("QUEUE DATA dev data REALLOCATION OF %d BYTES \n", size);
#endif
        dev_data = (dc_dev_dat *)realloc(dev_data,size);
        if (dev_data == NULL)
        {
          *error=DevErr_InsufficientMemory;
#if defined (TRACE)
          fprintf (stderr, "endf queue_data ()\n");
#endif
          return (DS_NOTOK);
        }
      }
      for (i=size_of_area-DEVICE_ENTRY_SIZE;i<size_of_area;i++)
      { /* initialise to zero */
        dev_comms[i].q_handle = 0;
        dev_comms[i].qcomm_num  = 0;
        dev_comms[i].qcomm_area_size = 0;
      }
    }
    current_device = num_devices-1;
    dev_comms[current_device].q_handle = handle;
    dev_comms[current_device].qcomm_num  = 0;
  }

  dev_comms[current_device].qcomm_num++;        /* increase command count */
  num_of_entries++;

  if (dev_comms[current_device].qcomm_area_size <
      dev_comms[current_device].qcomm_num)
  {
    dev_comms[current_device].qcomm_area_size += COMMAND_ENTRY_SIZE;
    if (dev_comms[current_device].qcomm_area_size == COMMAND_ENTRY_SIZE)
    {
      size = sizeof(dc_cmd_dat);
#if defined (ALLOC)
  printf ("QUEUE DATA cmd data CALLOCATION OF %d * %d BYTES \n", COMMAND_ENTRY_SIZE, size);
#endif
      dev_data[current_device].cmd_data = 
	      (dc_cmd_dat *)calloc(COMMAND_ENTRY_SIZE,size);
      if (dev_data[current_device].cmd_data == NULL)
      {
        *error=DevErr_InsufficientMemory;
#if defined (TRACE)
        fprintf (stderr, "endf queue_data ()\n");
#endif
        return (DS_NOTOK);
      }
      for (i=dev_comms[current_device].qcomm_area_size-COMMAND_ENTRY_SIZE;i<dev_comms[current_device].qcomm_area_size;i++)
      { /* initialise to zero */
        dev_data[current_device].cmd_data[i].cmd = 0;
        dev_data[current_device].cmd_data[i].cmd_error = 0;
        dev_data[current_device].cmd_data[i].cmd_time = 0;
        dev_data[current_device].cmd_data[i].length = 0;
        dev_data[current_device].cmd_data[i].sequence = NULL;
      }
    }
    else
    {
      size = dev_comms[current_device].qcomm_area_size*sizeof(dc_cmd_dat);
#if defined (ALLOC)
  printf ("QUEUE DATA cmd data REALLOCATION OF %d BYTES \n", size);
#endif
      dev_data[current_device].cmd_data = 
	      (dc_cmd_dat *)realloc(dev_data[current_device].cmd_data,size);
      if (dev_data[current_device].cmd_data == NULL)
      {
        *error=DevErr_InsufficientMemory;
#if defined (TRACE)
        fprintf (stderr, "endf queue_data ()\n");
#endif
        return (DS_NOTOK);
      }
      for (i=dev_comms[current_device].qcomm_area_size-COMMAND_ENTRY_SIZE;i<dev_comms[current_device].qcomm_area_size;i++)
      { /* initialise to zero */
        dev_data[current_device].cmd_data[i].cmd = 0;
        dev_data[current_device].cmd_data[i].cmd_error = 0;
        dev_data[current_device].cmd_data[i].cmd_time = 0;
        dev_data[current_device].cmd_data[i].length = 0;
        dev_data[current_device].cmd_data[i].sequence = NULL;
      }
    }
  }

  qcomm = dev_comms[current_device].qcomm_num-1;
  dev_data[current_device].cmd_data[qcomm].cmd = cmd;
  dev_data[current_device].cmd_data[qcomm].cmd_error = cmd_error;
  dev_data[current_device].cmd_data[qcomm].cmd_time = cmd_time;

/* DON'T NEED THIS CAUSE I DO DIRECT ASSIGNMENT OF POINTERS */

/*
  if (length > dev_data[current_device].cmd_data[qcomm].length)
  {
    dev_data[current_device].cmd_data[qcomm].sequence = (char *)(malloc(length));
  }
*/

/* NO BYTE DATA OUTPUT
#if defined (EBUG)
  for (k=0;k<length;k++)
  {
    fprintf(stderr,"FROM BYTE (%02d) : %x : %c\n",k,sequence[k],sequence[k]);
  }
#endif
*/
  dev_data[current_device].cmd_data[qcomm].sequence = sequence;
/* NO BYTE DATA OUTPUT
#if defined (EBUG)
  for (k=0;k<length;k++)
  {
    fprintf(stderr,"TO BYTE (%02d) : %x : %c\n",k,dev_data[current_device].cmd_data[qcomm].sequence[k],dev_data[current_device].cmd_data[qcomm].sequence[k]);
  }
#endif
*/
  dev_data[current_device].cmd_data[qcomm].length = length;
  dev_data[current_device].device_name = name;
  dev_data[current_device].nb_cmd = qcomm+1;
#if defined (PRINTQUEUE)
  print_devdata();
#endif

#endif

  return (DS_OK);
}

/*** FUNCTION STORE_DATA ***/

long store_data (int flag,daemon_device_struct_type *cdp,long *error_device)
{
  long status,tmp_error;
  dc_error error;
  int i,j;
  int offset;

#ifdef DCNOTAV
  return (DS_OK);
#else

#if defined (NEVER)
  fprintf(stderr,"#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#\n");
  fprintf(stderr,"** %d STORE DATA STORE DATA STORE DATA STORE DATA STORE DATA*\n",num_of_entries);
  fprintf(stderr,"#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#!#\n");
#endif

#if defined (TRACE)
  fprintf (stderr, "store_data ()\n");
#endif
/* wait till last */
/* */

  cdp->DCError = 0;
  *error_device = 0;
  offset = 0;

  status = 0;
  if (num_of_entries >0 && (flag==TRUE || num_of_entries > MAX_ENTRIES_IN_QUEUE))
  {
    while (offset<num_devices)
    {

#if defined (PRINTQUEUE)
      print_devdata();
#endif

#if defined (EBUG)
      fprintf(stderr,"before dc_dataput\n"); fflush(stdout);
#endif

      status = dc_dataput ((dc_dev_dat *)(&(dev_data[offset])),num_devices-offset,&error);
#if defined (PUTGET_D)
      printf ("DATAPUT : %ld\n",get_time());
#endif
#if defined (EBUG)
      fprintf(stderr,"status from dc_dataput = %ld\n",status);
      fprintf(stderr,"num_devices from dc_dataput = %ld\n",num_devices-offset);
      fprintf(stderr,"error  from dc_dataput = %ld\n",error.error_code);
      fprintf(stderr,"error_dev  from dc_dataput = %ld\n",error.dev_error);
#endif

      if (status != DS_OK)
      {
        printerror (error.error_code);
        cdp->DCError = error.error_code;
        *error_device = error.dev_error; /*dev_data[error.dev_error-1]; */
        if ((*error_device) == 0)
        {
          offset = offset + 1;   /* move to next because problem exists still */
        }
	else
	{
	  offset = *error_device + offset; /* try again from error */
	} 
	if (error.error_code == DcErr_DeviceNotDefined)
        {
          status = open_dc (cdp, &tmp_error);
        }
	else if (error.error_code == DcErr_CantGetDcServerNetInfo)
        {
	  offset=num_devices; /* try again next poll, this time data is lost */
        }
      }
      else
      {
        offset=num_devices;
      }
    }
    for (i=0;i<num_devices;i++)
    {
      for (j=0;j<dev_comms[i].qcomm_num;j++)
      {
#if defined (ALLOC)
        printf ("STORE_DATA dev_data[%d].cmd_data[%d] FREE\n",i,j);
#endif
        free (dev_data[i].cmd_data[j].sequence);
      }
    }
#if defined (EBUG)
    fprintf(stderr,"******************* QUEUE CLEARED *******************\n");
#endif
    num_devices = 0;
    num_of_entries = 0;
  }
  else
  {
     *error_device = -1; /* nothing to store indication */
  }

#if defined (TRACE)
  fprintf (stderr, "end store_data ()\n");
#endif
  return (status);

#endif

}

/*** FUNCTION PRINT_DEVDATA ***/

void print_devdata()
{
  int i,j,k;
  int qcomm;


#ifdef DCNOTAV
  return ;
#else

#if defined (EBUG)

  i=0;j=0;k=0;
  fprintf(stderr,"no of devices = %d\n",num_devices);
  for (i=0;i<num_devices;i++)
  {

     qcomm = dev_comms[i].qcomm_num-1;

     fprintf(stderr,"DEV %2d  : %s\n",i+1,dev_data[i].device_name);
     fprintf(stderr,"          Noc = %d\n",qcomm+1);
     fprintf(stderr,"          SNoc = %d\n",dev_data[i].nb_cmd);
     for (j=0;j<=qcomm;j++)
     {
       fprintf(stderr,"         %2d) CMD:%d ERR:%d SZE:%d\n",j+1,dev_data[i].cmd_data[j].cmd,
							                   dev_data[i].cmd_data[j].cmd_error,
					          		           dev_data[i].cmd_data[j].length);
/* NO BYTE DATA OUTPUT
      for (k=0;k<dev_data[i].cmd_data[j].length;k++)
      {
        fprintf(stderr,"              Byte (%02d) : %x : %c\n",k,dev_data[i].cmd_data[j].sequence[k],dev_data[i].cmd_data[j].sequence[k]);
      }
*/

     }
  }
  fprintf(stderr,"\n");

#endif

#endif

}

/*** FUNCTION OPEN_DC ***/

long open_dc (daemon_device_struct_type *cdp,long *error)
{

  static dc_dev dc_poll_list;
  int count;
  command_list *current_command;
  long status;
  dc_error error_str;


#ifdef DCNOTAV
  return (DS_OK);
#else

#if defined (TRACE)
  fprintf (stderr, "open_dc ()\n");
#endif
  cdp->DCInitialised = FALSE;
  size = strlen(cdp->DeviceName)+1;
#if defined (ALLOC)
  printf ("OPEN_DC device name MALLOCATION OF %d BYTES \n", size);
#endif
  dc_poll_list.device_name = (char *)(malloc (size));
  if (dc_poll_list.device_name==NULL)
  {
    *error = DevErr_InsufficientMemory;
#if defined (TRACE)
    fprintf (stderr, "endf open_dc ()\n");
#endif
    return (DS_NOTOK);
  }
  strcpy (dc_poll_list.device_name, cdp->DeviceName);

/* This multiple of 10 is for benefit of the dc */
  dc_poll_list.poll_interval = (10 * cdp->PollInterval ) / FACTOR;

  size = sizeof(dc_cmd);
#if defined (ALLOC)
  printf ("OPEN_DC dev cmd CALLOCATION OF %d * %d BYTES \n",cdp->NumberOfCommands, size);
#endif
  dc_poll_list.dev_cmd
       = (dc_cmd *)(calloc(cdp->NumberOfCommands,size));
  if (dc_poll_list.dev_cmd == NULL)
  {
    *error=DevErr_InsufficientMemory;
    return (DS_NOTOK);
  }
  current_command = cdp->CommandList;
  count=-1;
  while (current_command != NULL)
  {
     count++;
     dc_poll_list.dev_cmd[count].cmd
            = current_command->command;
     dc_poll_list.dev_cmd[count].argout_type
            = current_command->command_type;
#if defined (DCOPEN)
     fprintf(stderr,"COM:%d,TYP:%d\n",dc_poll_list.dev_cmd[count].cmd,dc_poll_list.dev_cmd[count].argout_type);
     if (dc_poll_list.dev_cmd[count].argout_type == 0)
        fprintf (stderr,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>TYPE VOID <<<<<<\n");
#endif
     current_command = current_command->next;
  }
  dc_poll_list.nb_cmd = count+1;

  /* wait till last */


#if defined (DCOPEN)
  printf("dc_open details : %s , %ds, %d\n",dc_poll_list.device_name,dc_poll_list.poll_interval,dc_poll_list.nb_cmd);
#endif

  status = dc_open (&dc_poll_list, (unsigned int)1, &error_str);

#if defined (DCOPEN)
  printf("dc_open status = %d\n",status);
  printf("dc_open error  = %ld\n",error_str.error_code);
#endif

  free (dc_poll_list.device_name);
  free (dc_poll_list.dev_cmd);

#if defined (ALLOC)
  printf ("OPEN_DC dev name FREE\n");
  printf ("OPEN_DC dev cmd FREE\n");
#endif

  if (status != DS_OK)
  {
    dc_problems=TRUE;

    dc_error_num = error_str.error_code;
    dc_last_error= error_str.error_code;
    dc_oks[dc_cycle]=DS_NOTOK;
    printerror (error_str.error_code);
    *error=error_str.error_code;
    if (++dc_cycle>ERROR_CYCLE) dc_cycle=0;
    if (dc_tries<ERROR_CYCLE) dc_tries++;
#if defined (TRACE)
    fprintf (stderr, "endf open_dc ()\n");
#endif
    return (DS_NOTOK);
  }
  dc_problems=FALSE;
  dc_error_num = 0;
  dc_oks[dc_cycle]=DS_OK;
  if (++dc_cycle >ERROR_CYCLE) dc_cycle=0;
  if (dc_tries<ERROR_CYCLE) dc_tries++;
  *error=0;
  cdp->DCInitialised = TRUE;
  
  
#if defined (TRACE)
  fprintf (stderr, "end open_dc ()\n");
#endif
  return (DS_OK);

#endif

}

