/*+*******************************************************************

 File:          daemon_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Update Daemon data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		 $Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _daemon_xdr_h
#define _daemon_xdr_h


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

