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
 * File:        daemon_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *              of the Update Daemon data types.
 *              
 * Author(s):   Jens Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    July 1992
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#include <DserverTeams.h>
#include <SysNumbers.h>

#ifndef _daemon_xdr_h
#define _daemon_xdr_h


/*
 *  definitions for the update daemon
 */

struct DevDaemonStatus {
   	char BeingPolled;
	DevLong PollFrequency;
 	DevLong TimeLastPolled;
    	char PollMode;
    	char DeviceAccessError;
  	DevLong ErrorCode;
    	DevLong LastCommandStatus;
	char ContinueAfterError;
};
typedef struct DevDaemonStatus 	DevDaemonStatus;
bool_t 				xdr_DevDaemonStatus(XDR *xdrs, DevDaemonStatus *objp);
long 				xdr_length_DevDaemonStatus(DevDaemonStatus *objp);
#define D_DAEMON_STATUS         43
#define LOAD_DAEMON_STATUS(A)   xdr_load_type ( D_DAEMON_STATUS, \
                                                (DevDataFunction)xdr_DevDaemonStatus, \
                                                sizeof(DevDaemonStatus), \
                                                (DevDataLengthFunction)xdr_length_DevDaemonStatus, \
                                                A )


struct DevDaemonData {
   	DevLong ddid;
      	DevLong long_data;
};
typedef struct DevDaemonData 	DevDaemonData;
bool_t 				xdr_DevDaemonData(XDR *xdrs, DevDaemonData *objp);
long 				xdr_length_DevDaemonData(DevDaemonData *objp);
#define D_DAEMON_DATA           44
#define LOAD_DAEMON_DATA(A)   	xdr_load_type ( D_DAEMON_DATA, \
                                                (DevDataFunction)xdr_DevDaemonData, \
                                                sizeof(DevDaemonData), \
                                                (DevDataLengthFunction)xdr_length_DevDaemonData, \
                                                A )

struct DevDaemonStruct {
	char *dev_n;
   	DevVarStringArray cmd_list;
      	DevLong poller_frequency;
};
typedef struct DevDaemonStruct 	DevDaemonStruct;
bool_t 				xdr_DevDaemonStruct(XDR *xdrs, DevDaemonStruct *objp);
long 				xdr_length_DevDaemonStruct(DevDaemonStruct *objp);
#define D_DAEMON_STRUCT         NewDevBase + 1 
#define LOAD_DAEMON_NEWD(A)   	xdr_load_type ( D_DAEMON_STRUCT, \
                                                (DevDataFunction)xdr_DevDaemonStruct, \
                                                sizeof(DevDaemonStruct), \
                                                (DevDataLengthFunction)xdr_length_DevDaemonStruct, \
                                                A )

#endif /* _daemon_xdr_h */

