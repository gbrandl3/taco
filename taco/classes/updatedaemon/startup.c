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
 * Project:     Device Servers 
 *
 * Description: startup procedure for Update Daemon Class
 *
 * Author(s):   Michael Schofield
 *              $Author: andy_gotz $
 *
 * Original:    April 1992
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-19 09:29:39 $
 */

static char RcsId[] = " $Header: /home/jkrueger1/sources/taco/backup/taco/classes/updatedaemon/startup.c,v 1.3 2006-09-19 09:29:39 andy_gotz Exp $ ";

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

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

long startup(char *svr_name, long *error)
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
