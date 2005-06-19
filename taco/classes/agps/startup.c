static char RcsId[] = "$Header: /segfs/dserver/classes/powersupply/ag/src/RCS/startup.c,v 1.12 2003/12/10 20:57:59 goetz Exp $";

/*********************************************************************

 File:		startup.c

 Project:	Device Servers

 Description:	Startup procedure for AGPowerSupplyClass. The
		startup procedure is the first procedure called
		from main() when the device server starts up.
		All toplevel devices to be created for the device 
		server should be done in startup(). The startup 
		should make use of the database to determine which 
		devices it should create. Initialisation of devices
		is normally done from startup().

 Author(s);	A. Goetz 

 Original:	November 1990

 $Log: startup.c,v $
 Revision 1.12  2003/12/10 20:57:59  goetz
 y
 ls

 Revision 1.11  2000/06/22 09:06:49  goetz
 *** empty log message ***

 * Revision 1.10  97/12/02  15:26:13  15:26:13  klotz (W.D. Klotz)
 * Win_NT_95_released_NOV_97
 * 
 * Revision 1.5  97/12/02  15:22:01  15:22:01  klotz (W.D. Klotz)
 * Win_NT_95_released_NOV_97
 * 
 * Revision 1.4  97/12/01  16:01:28  16:01:28  goetz (Andy Goetz)
 * *** empty log message ***
 * 
 * Revision 1.3  96/11/27  10:42:05  10:42:05  goetz (Andy Goetz)
 * changed dev_no from int to u_int
 * 
 * Revision 1.2  96/09/17  17:21:57  17:21:57  klotz (W.D. Klotz)
 * WD Klotz: checked NT build
 * 
 * Revision 1.1  95/07/27  17:54:57  17:54:57  goetz (Andy Goetz)
 * Initial revision
 * 

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France



 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#include <AGPowerSupplyP.h>
#include <AGPowerSupply.h>


extern long debug_flag;

#ifdef _NT
/* global pointer to text to be displayed in main window */
MainWndTextDisplay gMWndTxtDisplay;  
/*
 * Here is the place to define what to put into
 * the main window's backdrop.
 */
static char* info[]= {
		{"AG Powersupply Simulator"},
		{"32 bit Version rev. 1.2 for Win95 & WinNT4, Oct 1997"},
		{"ESRF, BP 220, 38043 Grenoble, France"}
		};
#endif

/***************************/
/* AG PowerSupply startup */
/***************************/

long startup(svr_name, error)
char *svr_name;
long *error;
{
   AGPowerSupply ps_list[100];
   u_int dev_no, u_i;
   char msg[512];
/*
 * pointer to list of devices returned by database.
 */ 
   char **dev_list;

/*   debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_DEV_SVR_CLASS | DBG_API);*/

   debug_flag = (DEBUG_ON_OFF | DBG_ASYNCH ); 

   if (db_getdevlist(svr_name,&dev_list,&dev_no,error))
   {
#ifdef WIN32
      sprintf(msg,"startup(): db_getdevlist() failed, error %d",*error);
      dprintf1(msg);
#else
      printf("startup(): db_getdevlist() failed, error %d\n",*error);
#endif
      return(-1);
   }
   else
   {
#ifdef WIN32
      sprintf(msg,"following devices found in static database");
	  dprintf(msg);
      for (u_i=0;u_i<dev_no;u_i++) {
         sprintf(msg,"%s",dev_list[u_i]);
		 dprintf(msg);
      }
#else
      printf("following devices found in static database \n\n");
      for (u_i=0;u_i<dev_no;u_i++)
      {
         printf("%s\n",dev_list[u_i]);
      }
#endif
   }

/*
 * ye olde methode ...
 *
 * if (getdevlist(&dev_no,dev_list,MAX_NO_OF_DEVICES,error) != 0)
 * {
 *    *error = DbErr_DatabaseAccess;
 *    return(-1);
 * }
 */

/*
 * create, initialise and export all devices served by this server
 */

   for (u_i=0; u_i < dev_no; u_i++)
   {
      if (ds__create(dev_list[u_i], aGPowerSupplyClass, &(ps_list[u_i]),error) != 0)
      {
         return(-1);
      }

/*
 * initialise the newly created powersupply
 */ 
  
      if (ds__method_finder(ps_list[u_i],DevMethodInitialise)(ps_list[u_i],error) != 0)
      {
         return(-1);
      }

/*
 * now export it to the outside world
 */

#ifdef WIN32
      sprintf(msg,"created %s, going to export it",dev_list[u_i]);
	  dprintf(msg);
#else
	  printf("created %s, going to export it\n",dev_list[u_i]);
#endif
      if (dev_export(dev_list[u_i],ps_list[u_i],error) != 0)
      {
         return(-1);
      }
#ifdef WIN32
   sprintf(msg,"left startup and all's OK");
   dprintf(msg);
#else
      printf("export worked\n");
#endif

   }

#ifdef _NT
   sprintf(msg,"left startup and all's OK");
   dprintf(msg);
/*
 * Here is the place to assign what to put into
 * the main window's backdrop.
 */
   gMWndTxtDisplay.lines= 3;
   gMWndTxtDisplay.text= info;
   
#else
   printf("left startup and all's OK\n");
#endif

   return(0);
}



