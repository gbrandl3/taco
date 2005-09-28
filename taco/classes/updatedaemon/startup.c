static char RcsId[] = " $Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/startup.c,v 1.1 2005-09-28 20:08:33 andy_gotz Exp $ ";

/*********************************************************************

 File:		startup.c

 Project:	Device Server

 Description:	startup procedure for Update Daemon Class
		

 Author(s);	Michael Schofield

 Original:	April 1992

 $Log: not supported by cvs2svn $
 Revision 4.19  2005/01/24 08:12:17  taurel
 Fix some compile time error in order to compile poller on Linux with CPP compiler.

 Revision 4.18  2004/01/15 11:40:55  taurel
 With Linux, the select call may change the timeout struc. Re-set it before any select call.

 Revision 4.17  2003/12/11 16:39:02  taurel
 POrted

 * Revision 4.16  2002/07/04  08:58:05  08:58:05  taurel (Emmanuel Taurel)
 * Added TACO_PATH env. variable to find poller path
 * 
 Revision 4.15  2001/05/11 17:13:48  taurel
 No change, check out only for debug purpose

 * Revision 4.14  2000/10/17  17:01:00  17:01:00  taurel (E.Taurel)
 * Remove some fancy printf
 * 
 * Revision 4.13  2000/10/17  15:47:31  15:47:31  goetz (Andy Goetz)
 * daemon renamed ud_daemon for Linux port
 * 
 * Revision 4.12  2000/06/15  10:36:16  10:36:16  taurel (Emmanuel Taurel)
 * The
 * 
 * Revision 4.11  99/02/25  15:59:52  15:59:52  taurel (Emmanuel TAUREL)
 * change algo. used for the signal_valid data in order to spped up data exchange between daemon and poller.
 * 
 * Revision 4.10  99/02/04  13:23:42  13:23:42  taurel (Emmanuel Taurel)
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
 * Revision 4.5  95/03/20  10:43:10  10:43:10  taurel (Emmanuel TAUREL)
 * Change variable name from errno to dev_errno for
 * device server error.
 * 
 * Revision 4.4  95/01/12  16:02:09  16:02:09  taurel (Emmanuel TAUREL)
 * Nothing change. co and ci jutest purpose.
 * 
 * Revision 4.3  95/01/11  15:17:40  15:17:40  taurel (Emmanuel TAUREL)
 * Remove some printf
 * 
 * Revision 4.2  95/01/11  15:14:38  15:14:38  taurel (Emmanuel TAUREL)
 * Change dataport name
 * 
 * Revision 4.1  95/01/03  17:30:31  17:30:31  taurel (Emmanuel TAUREL)
 * Ported to OS-9 ULTRA-C. Also fix two bugs for OS-9 :
 *   - Now time spent in command execution is correct
 *   - Correctly removed the dataport when the daemon is correctly killed.
 * 
 * Revision 4.0  94/10/17  14:55:58  14:55:58  taurel (Emmanuel TAUREL)
 * First
 * 
 * Revision 3.5  94/06/28  18:41:43  18:41:43  servant (Florence Servant)
 * *** empty log message ***
 * 
 * Revision 3.4  94/03/11  15:37:40  15:37:40  taurel (Emmanuel TAUREL)
 * Replace MAX_NUMBER_OF_DEVICES with 100. This symbol has been removed in the
 * new release of the API library.
 * 
 * Revision 3.3  93/12/03  09:09:30  09:09:30  taurel (Emmanuel TAUREL)
 * Change
 * 
 * Revision 3.2  93/09/28  09:41:11  09:41:11  taurel (Emmanuel TAUREL)
 * Modified for the new API release (3.25) so, don't use the
 * DevCmdNameList array anymore in the poller. The poller has also been
 * modified for the DS_WARNING system.
 * 
 * Revision 3.0  93/05/18  17:18:39  17:18:39  taurel (Emmanuel Taurel)
 * Adapted for SUN release of the update daemon/update poller.
 * 
 * Revision 1.4  93/02/03  14:53:57  14:53:57  schofiel (Michael Schofield)
 * testing
 * 
 * Revision 1.3  93/02/03  14:52:59  14:52:59  schofiel (Michael Schofield)
 * Test check
 * 
 * Revision 1.2  93/02/03  14:52:20  14:52:20  schofiel (Michael Schofield)
 * Precautionary checkin
 * 
 * Revision 1.1  92/10/09  09:22:13  09:22:13  schofiel (Michael Schofield)
 * Initial revision
 * 

 Copyleft (c) 1992 by European Synchrotron Radiation Facility, 
                      Grenoble, France



 *********************************************************************/

#include <config.h>
#include <API.h>
#include <Admin.h>
#include <DevErrors.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>

#include <ud_server.h>
#include <ud_serverP.h>
#include <daemon.h>

#include <dataport.h>

extern long debug_flag;

long startup(svr_name, error)
char *svr_name;
long *error;
{
   char **dev_list;
   int i,j;
   unsigned int dev_no;

   Daemon daemon_list[100];

/*
 * here is place for all devices known to this server to be
 * created and exported
 */


#ifdef __unix
   signal (SIGHUP, SIG_IGN);
#endif

   printf ("UPDATE DAEMON INITIALISATION STARTUP\n"); 
   fflush(stdout);

#if defined(EBUG)
   fprintf(stderr,"startup() - svr_name=%s\n",svr_name);
/*   debug_flag = (  DEBUG_ON_OFF | DBG_ERROR | DBG_STARTUP | DBG_COMMANDS | DBG_METHODS | DBG_DEV_SVR_CLASS);
*/
#endif

   if (db_getdevlist (svr_name,&dev_list,&dev_no,error)!=DS_OK)
   {
     return (DS_NOTOK);
   }

   for (i=0;i<dev_no;i++)
   {
     if (ds__create(dev_list[i],daemonClass,&(daemon_list[i]),error) == DS_NOTOK)
     {
       return (DS_NOTOK);
     }

     if (ds__method_finder (daemon_list[i], DevMethodInitialise)
                           (daemon_list[i], error) == DS_OK)
       {

         if (ds__method_finder (daemon_list[i], DevMethodDevExport)
                               (dev_list[i], daemon_list[i], error) == DS_NOTOK)
         {
           dev_printerror (SEND,
                           "startup() : export of %s failed :\n    %s",
                            dev_list[i], dev_error_str (*error));
         }
       }
     else
       {
         dev_printerror (SEND,
                         "startup() : initialisation of %s failed :\n    %s",
                          dev_list[i], dev_error_str (*error));

       }
 
   }

#if defined(EBUG)
   fprintf(stderr,"end startup()\n");
#endif
   
   printf ("UPDATE DAEMON %s INITIALISATION FINISHED\n",svr_name); fflush(stdout);

   return(DS_OK);
}
