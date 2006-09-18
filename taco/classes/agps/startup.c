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
 * File:        startup.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description:	Startup procedure for AGPowerSupplyClass. The
 * 		startup procedure is the first procedure called
 *		from main() when the device server starts up.
 *		All toplevel devices to be created for the device 
 *		server should be done in startup(). The startup 
 *		should make use of the database to determine which 
 *		devices it should create. Initialisation of devices
 *		is normally done from startup().
 *
 * Author(s):   A. Goetz 
 *              $Author: jkrueger1 $
 *
 * Original:    November 1990
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

static char RcsId[] = "$Header: /segfs/dserver/classes/powersupply/ag/src/RCS/startup.c,v 1.12 2003/12/10 20:57:59 goetz Exp $";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

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



