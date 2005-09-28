/* static char RcsId[] = " @(#) $Header: /segfs/dserver/classes/daemon/include/RCS/ud_server.h,v 4.19 2005/01/24 08:12:18 taurel Rel $ "; */

/*********************************************************************

 File:		Server.h

 Project:	Device Servers

 Description:	public include for the ServerClass
		

 Author(s);	Andy Goetz

 Original:	October 1990

 $Log: ud_server.h,v $
 Revision 4.19  2005/01/24 08:12:18  taurel
 Fix some compile time error in order to compile poller on Linux with CPP compiler.

 Revision 4.18  2004/01/15 11:40:56  taurel
 With Linux, the select call may change the timeout struc. Re-set it before any select call.

 Revision 4.17  2003/12/11 16:39:02  taurel
 POrted

 * Revision 4.16  2002/07/04  08:58:07  08:58:07  taurel (Emmanuel Taurel)
 * Added TACO_PATH env. variable to find poller path
 * 
 Revision 4.15  2001/05/11 17:13:49  taurel
 No change, check out only for debug purpose

 * Revision 4.14  2000/10/17  17:01:01  17:01:01  taurel (E.Taurel)
 * Remove some fancy printf
 * 
 * Revision 4.13  2000/10/17  15:47:32  15:47:32  goetz (Andy Goetz)
 * daemon renamed ud_daemon for Linux port
 * 
 * Revision 4.12  2000/06/15  10:36:16  10:36:16  taurel (Emmanuel Taurel)
 * The
 * 
 * Revision 4.11  99/02/25  15:59:53  15:59:53  taurel (Emmanuel TAUREL)
 * change algo. used for the signal_valid data in order to spped up data exchange between daemon and poller.
 * 
 * Revision 4.10  99/02/04  13:23:43  13:23:43  taurel (Emmanuel Taurel)
 * Add closing open file in poller startup and remove check before sending signal to poller
 * 
 * Revision 4.9  99/01/13  14:24:21  14:24:21  taurel (E.Taurel)
 * Fix bug in datac.c when the open_dc function called by the store_data function
 * 
 * Revision 4.8  96/12/04  11:32:54  11:32:54  taurel (E.Taurel)
 * Added a printf to print the number of devices
 * retrived from db with the ud_poll_list resource.
 * 
 * Revision 4.7  96/03/11  10:43:14  10:43:14  taurel (E.Taurel)
 * No change, co just for test.
 * 
 * Revision 4.6  96/02/27  15:51:02  15:51:02  taurel (Emmanuel TAUREL)
 * Ported to Solaris.
 * 
 * Revision 4.5  95/03/20  10:43:11  10:43:11  taurel (Emmanuel TAUREL)
 * Change variable name from errno to dev_errno for
 * device server error.
 * 
 * Revision 4.4  95/01/12  16:02:10  16:02:10  taurel (Emmanuel TAUREL)
 * Nothing change. co and ci jutest purpose.
 * 
 * Revision 4.3  95/01/11  15:17:41  15:17:41  taurel (Emmanuel TAUREL)
 * Remove some printf
 * 
 * Revision 4.2  95/01/11  15:14:38  15:14:38  taurel (Emmanuel TAUREL)
 * Change dataport name
 * 
 * Revision 4.1  95/01/03  17:30:32  17:30:32  taurel (Emmanuel TAUREL)
 * Ported to OS-9 ULTRA-C. Also fix two bugs for OS-9 :
 *   - Now time spent in command execution is correct
 *   - Correctly removed the dataport when the daemon is correctly killed.
 * 
 * Revision 4.0  94/10/17  14:58:37  14:58:37  taurel (Emmanuel TAUREL)
 * First check in after Florence has left ESRF.
 * 
 * Revision 3.3  93/12/03  09:09:30  09:09:30  taurel (Emmanuel TAUREL)
 * Change
 * 
 * Revision 3.2  93/09/28  09:41:12  09:41:12  taurel (Emmanuel TAUREL)
 * Modified for the new API release (3.25) so, don't use the
 * DevCmdNameList array anymore in the poller. The poller has also been
 * modified for the DS_WARNING system.
 * 
 * Revision 1.2  93/02/03  15:00:39  15:00:39  schofiel (Michael Schofield)
 * Precautionary checkin
 * 
 * Revision 1.1  92/10/09  09:23:41  09:23:41  schofiel (Michael Schofield)
 * Initial revision
 * 
 * Revision 1.1  91/05/17  15:46:29  15:46:29  goetz (Andy Goetz)
 * Initial revision
 * 
 * Revision 1.2  90/11/09  11:14:57  11:14:57  goetz (Andy Goetz)
 * added extern Server server, for initialising by external functions
 * 
 * Revision 1.1  90/11/07  08:12:35  08:12:35  goetz (Andy Goetz)
 * Initial revision
 * 

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France



 *********************************************************************/

#ifndef DAEMON_H
#define DAEMON_H

typedef struct _DaemonClassRec *DaemonClass;
typedef struct _DaemonRec *Daemon;

extern DaemonClass daemonClass;
extern Daemon ud_daemon;

#endif /* DAEMON_H */
