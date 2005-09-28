
/*********************************************************************

 File:          daemon.h

 Project:       Device Servers

 Description:   include file for the update daemon programs 

 Author(s);     Michael Schofield

 Original:      September 1992

 $Log: daemon.h,v $
 Revision 4.19  2005/01/24 08:12:18  taurel
 Fix some compile time error in order to compile poller on Linux with CPP compiler.

 Revision 4.18  2004/01/15 11:40:56  taurel
 With Linux, the select call may change the timeout struc. Re-set it before any select call.

 Revision 4.17  2003/12/11 16:39:03  taurel
 POrted

 * Revision 4.16  2002/07/04  08:58:07  08:58:07  taurel (Emmanuel Taurel)
 * Added TACO_PATH env. variable to find poller path
 * 
 Revision 4.15  2001/05/11 17:13:52  taurel
 No change, check out only for debug purpose

 * Revision 4.14  2000/10/17  17:01:03  17:01:03  taurel (E.Taurel)
 * Remove some fancy printf
 * 
 * Revision 4.13  2000/10/17  15:47:34  15:47:34  goetz (Andy Goetz)
 * daemon renamed ud_daemon for Linux port
 * 
 * Revision 4.12  2000/06/15  10:36:19  10:36:19  taurel (Emmanuel Taurel)
 * The
 * 
 * Revision 4.11  99/02/25  15:59:55  15:59:55  taurel (Emmanuel TAUREL)
 * change algo. used for the signal_valid data in order to spped up data exchange between daemon and poller.
 * 
 * Revision 4.10  99/02/04  13:23:46  13:23:46  taurel (Emmanuel Taurel)
 * Add closing open file in poller startup and remove check before sending signal to poller
 * 
 * Revision 4.9  99/01/13  14:24:23  14:24:23  taurel (E.Taurel)
 * Fix bug in datac.c when the open_dc function called by the store_data function
 * 
 * Revision 4.8  96/12/04  11:32:55  11:32:55  taurel (E.Taurel)
 * Added a printf to print the number of devices
 * retrived from db with the ud_poll_list resource.
 * 
 * Revision 4.7  96/03/11  10:43:16  10:43:16  taurel (E.Taurel)
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
 * Revision 4.2  95/01/11  15:14:40  15:14:40  taurel (Emmanuel TAUREL)
 * Change dataport name
 * 
 * Revision 4.1  95/01/03  17:30:34  17:30:34  taurel (Emmanuel TAUREL)
 * Ported to OS-9 ULTRA-C. Also fix two bugs for OS-9 :
 *   - Now time spent in command execution is correct
 *   - Correctly removed the dataport when the daemon is correctly killed.
 * 
 * Revision 4.0  94/10/17  14:56:00  14:56:00  taurel (Emmanuel TAUREL)
 * First
 * 
 * Revision 3.4  94/06/28  18:49:49  18:49:49  servant (Florence Servant)
 * *** empty log message ***
 * 
 * Revision 3.3  93/12/03  09:09:32  09:09:32  taurel (Emmanuel TAUREL)
 * Change
 * 
 * Revision 3.2  93/09/28  09:41:15  09:41:15  taurel (Emmanuel TAUREL)
 * Modified for the new API release (3.25) so, don't use the
 * DevCmdNameList array anymore in the poller. The poller has also been
 * modified for the DS_WARNING system.
 * 
 * Revision 1.2  93/02/03  15:00:26  15:00:26  schofiel (Michael Schofield)
 * Precautionary checkin
 * 
 * Revision 1.1  92/10/09  09:23:37  09:23:37  schofiel (Michael Schofield)
 * Initial revision
 * 

 Copyleft (c) 1992 by European Synchrotron Radiation Facility,
                      Grenoble, France


 *********************************************************************/

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
                int       NumRetries;
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
long queue_data (char *,devserver,int,long,int,int,char *,long *);
long store_data (int,daemon_device_struct_type *,long *);
void print_devdata();
long open_dc (daemon_device_struct_type *,long *);

#ifdef _OSK
void Make_Dataport_Name (char *,char *,int);
#else
void Make_Dataport_Name (char *,char *,pid_t);
#endif

