/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        daemon.h
 *
 * Project:     Device Servers
 *
 * Description: include file for the update daemon programs 
 *
 * Author(s);   Michael Schofield
 *              $Author: jkrueger1 $
 *
 * Original:    September 1992
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifndef DAEMON_H
#define DAEMON_H

#include <DevServer.h>

/* PRIVATE ERROR MESSAGE */
 
#define DevErr_CannotFreeDevice			NewDevBase + 1
#define DcErr_CantCloseDcForDevice		NewDevBase + 2

/* DAEMON_DATAPORT - a name used to identify temporary files in /tmp */

#if (OSK || _OSK)
#define     DAEMON_DATAPORT   "ud"
#else
#define	    DAEMON_DATAPORT   "updatedp"
#endif

/* POLLER_PATH - the location of the daemon poller to be spawned */
/* #define     POLLER_PATH       "/users/a/dc/bin/ud_poller" */

#define     POLLER_PATH       "./ud_poller" 

/* FIRST_DEVICE - the number of the first device in the device list */
/* MAX_DEVICES  - the maximum number of devices that the daemon can handle */
/* MAX_DEVICES is also defined in ApiP.h include file. This file is
   included after ApiP.h. Undefined ApiP.h MAX_DEVICES in order to have our
   own definition of MAX_DEVICES */
/* ERROR_CYCLE  - the number of commands used to estimate the %age success */
/* GET_DP_WAIT  - the uS wait time between polls of a busy dataport */

#define     FIRST_DEVICE 1
#ifdef MAX_DEVICES
#undef MAX_DEVICES
#endif
#define     MAX_DEVICES  256
#define     ERROR_CYCLE  1000
#define     GET_DP_WAIT  100000

/* definitions of the dataport command transfer protocol */
#define     D_INIT       100
#define     D_READY      101
#define     D_NEW        102
#define     D_TAKEN      103
#define     D_PROCESSED  104
#define     D_ERROR      105
#define     D_CLEARED    106
#define     D_OFF        107

/* for device initialisation */

#define     DB_UPDATE    0;	

/* device identificator management for ud_poll_list modification */

#define     ID_MASK      0x00FF;
#define     ID_COUNT     16;


/* poll mode defines */
#define     SURVEY       0
#define     READOUT      1

/* poll recover mode defines */
#define     CATCHUP      2
#define     FORGET       3
#define     DRIFT        4

/* timeout (in secs) used to check that poller is still running */
#define     TIMEOUT      60

/* timing factors */
#define GRAIN 10
#define FACTOR 1000
#define SECONDS 10000
/* #define THRESHOLD 60000 */
#define THRESHOLD FACTOR*SECONDS
#define	SEC_GET_DP_WAIT	1		/* 1 second */
#define	POLL_INTERVAL_NOT_IMP	20000	/* 20 seconds */

#define	MAX_NO_OF_DEVICES	100

/* the type for the contents of the user area of the dataport */
typedef struct _dataport_struct_type {
                int     command;
                long    data[5];
                char    string[255];
                long    status;
                long    error;
                long    timestamp;
		long    poller_id;
		char    signal_valid;
} dataport_struct_type;

/* the command list link structure */
struct _command_list {
                int     command;
		char    command_name[20];
                int     command_type;
                struct _command_list *next;
};

typedef struct _command_list command_list;

/* the command look up table structure */
typedef struct _command_lut_struct_type {
                int       command;
                char      string[32];
} command_lut_struct_type;

/* the type for a daemon data element */
typedef struct _daemon_device_struct_type {
                char      *DeviceName;
                devserver DeviceHandle;
                int       PollOnInitialise;
                int       PollMode;
                long      TimeLastPolled;
                long      systemTLP;
                long      NextPollTime;
                int       BeingPolled;
                int       DeviceBeingAccessed;
                DevLong   NumRetries;
                int       ContinueAfterError;
                int       DeviceAccessFailure;
                long      PollInterval;          /* in 10ths of seconds */
                int       DaemonAccess;
                int       DeviceImported;
		int       TypesRetrieved;
		int       DCInitialised;
		long      NumberOfCommands;
                command_list *CommandList;
                long      LastCommandStatus;
                long      LastErrorCode;
		long      DCError;
                int       PollRecoverMode;
                long      PollForgetTime;
                long      PollForgetMultiple;
                long      DeviceCounter;
                long      TangoFlag;
} daemon_device_struct_type;

/* Some functions declarations */

void printerror(long);
long queue_data (char *,devserver,int,long,int,int,char *, DevLong *);
long store_data (int,daemon_device_struct_type *, DevLong *);
void print_devdata();
long open_dc (daemon_device_struct_type *, DevLong *);

#ifdef _OSK
#define pid_t int
#endif

void Make_Dataport_Name (char *, size_t, char *,pid_t);

#endif
