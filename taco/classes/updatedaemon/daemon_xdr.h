
/*+*******************************************************************

 File:          daemon_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Update Daemon data types.

 Author(s):  	Jens Meyer

 Original:	July 1992


 $Revision: 4.19 $
 $Date: 2005/01/24 08:12:18 $

 $Author: taurel $

 $Log: daemon_xdr.h,v $
 Revision 4.19  2005/01/24 08:12:18  taurel
 Fix some compile time error in order to compile poller on Linux with CPP compiler.

 Revision 4.18  2004/01/15 11:40:56  taurel
 With Linux, the select call may change the timeout struc. Re-set it before any select call.

 Revision 4.17  2003/12/11 16:39:03  taurel
 POrted

 * Revision 4.16  2002/07/04  08:58:07  08:58:07  taurel (Emmanuel Taurel)
 * Added TACO_PATH env. variable to find poller path
 * 
 Revision 4.15  2001/05/11 17:13:51  taurel
 No change, check out only for debug purpose

 * Revision 4.14  2000/10/17  17:01:02  17:01:02  taurel (E.Taurel)
 * Remove some fancy printf
 * 
 * Revision 4.13  2000/10/17  15:47:33  15:47:33  goetz (Andy Goetz)
 * daemon renamed ud_daemon for Linux port
 * 
 * Revision 4.12  2000/06/15  10:36:19  10:36:19  taurel (Emmanuel Taurel)
 * The
 * 
 * Revision 4.11  99/02/25  15:59:55  15:59:55  taurel (Emmanuel TAUREL)
 * change algo. used for the signal_valid data in order to spped up data exchange between daemon and poller.
 * 
 * Revision 4.10  99/02/04  13:23:45  13:23:45  taurel (Emmanuel Taurel)
 * Add closing open file in poller startup and remove check before sending signal to poller
 * 
 * Revision 4.9  99/01/13  14:24:22  14:24:22  taurel (E.Taurel)
 * Fix bug in datac.c when the open_dc function called by the store_data function
 * 
 * Revision 4.8  96/12/04  11:32:55  11:32:55  taurel (E.Taurel)
 * Added a printf to print the number of devices
 * retrived from db with the ud_poll_list resource.
 * 
 * Revision 4.7  96/03/11  10:43:15  10:43:15  taurel (E.Taurel)
 * No change, co just for test.
 * 
 * Revision 4.6  96/02/27  15:51:03  15:51:03  taurel (Emmanuel TAUREL)
 * Ported to Solaris.
 * 
 * Revision 4.5  95/03/20  10:43:13  10:43:13  taurel (Emmanuel TAUREL)
 * Change variable name from errno to dev_errno for
 * device server error.
 * 
 * Revision 4.4  95/01/12  16:02:12  16:02:12  taurel (Emmanuel TAUREL)
 * Nothing change. co and ci jutest purpose.
 * 
 * Revision 4.3  95/01/11  15:17:42  15:17:42  taurel (Emmanuel TAUREL)
 * Remove some printf
 * 
 * Revision 4.2  95/01/11  15:14:39  15:14:39  taurel (Emmanuel TAUREL)
 * Change dataport name
 * 
 * Revision 4.1  95/01/03  17:30:33  17:30:33  taurel (Emmanuel TAUREL)
 * Ported to OS-9 ULTRA-C. Also fix two bugs for OS-9 :
 *   - Now time spent in command execution is correct
 *   - Correctly removed the dataport when the daemon is correctly killed.
 * 
 * Revision 4.0  94/10/17  14:55:59  14:55:59  taurel (Emmanuel TAUREL)
 * First
 * 
 * Revision 1.1  94/06/28  18:50:01  18:50:01  servant (Florence Servant)
 * Initial revision
 * 

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/
#include <DserverTeams.h>
#include <SysNumbers.h>

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
bool_t 				xdr_DevDaemonStatus();
long 				xdr_length_DevDaemonStatus();
#define D_DAEMON_STATUS         43
#define LOAD_DAEMON_STATUS(A)   xdr_load_type ( D_DAEMON_STATUS, \
                                                xdr_DevDaemonStatus, \
                                                sizeof(DevDaemonStatus), \
                                                xdr_length_DevDaemonStatus, \
                                                A )


struct DevDaemonData {
   	long ddid;
      	long long_data;
};
typedef struct DevDaemonData 	DevDaemonData;
bool_t 				xdr_DevDaemonData();
long 				xdr_length_DevDaemonData();
#define D_DAEMON_DATA           44
#define LOAD_DAEMON_DATA(A)   	xdr_load_type ( D_DAEMON_DATA, \
                                                xdr_DevDaemonData, \
                                                sizeof(DevDaemonData), \
                                                xdr_length_DevDaemonData, \
                                                A )

struct DevDaemonStruct {
	char *dev_n;
   	DevVarStringArray cmd_list;
      	DevLong poller_frequency;
};
typedef struct DevDaemonStruct 	DevDaemonStruct;
bool_t 				xdr_DevDaemonStruct();
long 				xdr_length_DevDaemonStruct();
#define D_DAEMON_STRUCT         NewDevBase + 1 
#define LOAD_DAEMON_NEWD(A)   	xdr_load_type ( D_DAEMON_STRUCT, \
                                                xdr_DevDaemonStruct, \
                                                sizeof(DevDaemonStruct), \
                                                xdr_length_DevDaemonStruct, \
                                                A )

#endif /* _daemon_xdr_h */

