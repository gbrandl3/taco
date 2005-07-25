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
 *		of the Update Daemon data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef DAEMON_XDR_H
#define DAEMON_XDR_H

/*
 *  definitions for the update daemon
 */

struct DevDaemonStatus {
   	char BeingPolled;
	long PollFrequency;
 	long TimeLastPolled;
    	char PollMode;
    	char DeviceAccessError;
  	long ErrorCode;
    	long LastCommandStatus;
	char ContinueAfterError;
};
typedef struct DevDaemonStatus 	DevDaemonStatus;
bool_t 	_DLLFunc xdr_DevDaemonStatus PT_((_LPXDR xdrs, DevDaemonStatus _Far *objp));
long 	_DLLFunc xdr_length_DevDaemonStatus PT_((DevDaemonStatus _Far *objp));
#define D_DAEMON_STATUS         43
#define LOAD_DAEMON_STATUS(A)   xdr_load_type ( D_DAEMON_STATUS, \
                                                xdr_DevDaemonStatus, \
                                                sizeof(DevDaemonStatus), \
                                                xdr_length_DevDaemonStatus, \
                                                A )


struct DevDaemonData {
   	long ddid;
      	long long_data;
	char *string_data;
};
typedef struct DevDaemonData 	DevDaemonData;
bool_t 	_DLLFunc xdr_DevDaemonData PT_((_LPXDR xdrs, DevDaemonData _Far *objp));
long 	_DLLFunc xdr_length_DevDaemonData PT_((DevDaemonData _Far *objp));
#define D_DAEMON_DATA           44
#define LOAD_DAEMON_DATA(A)   	xdr_load_type ( D_DAEMON_DATA, \
                                                xdr_DevDaemonData, \
                                                sizeof(DevDaemonData), \
                                                xdr_length_DevDaemonData, \
                                                A )

#endif /* _daemon_xdr_h */

