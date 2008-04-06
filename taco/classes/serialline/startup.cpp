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
 * File:	startup.cpp
 *
 * Project:     Device Servers in C++
 *
 * Description:	Startup file for the Serial class implemented in C++
 *
 * Author(s):	Manuel Perez
 *              $Author: jkrueger1 $
 *
 * Original:	January 1997 by Emmanuel Taurel, Andy Gotz
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:38 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <DevSignal.h>

#include <DevServer.h> /* will include Device.H */
#include <Serial.h>

// extern void catchSig(int);

Serial *device_ds[MAX_DEVICES];
unsigned int n_devices;

long startup(char *svr_name, DevLong *error)
{
   char **dev_list;
   unsigned int i;
   long status;

   printf ("startup++() called (server name = %s)\n",svr_name);
//   int l = strlen(RcsId); /* necessary for Ultra C++ to include RcsId */

//
// install signal handler
//

//   ds__signal(SIGINT,catchSig,error);
//   ds__signal(SIGQUIT,catchSig,error);
//   ds__signal(SIGHUP,catchSig,error);

//
// get the list of device name to be served from the static database
//

   if (db_getdevlist(svr_name,&dev_list,&n_devices,error))
   {
        printf("startup(): db_getdevlist() failed, error %d\n",*error);
        return(DS_NOTOK);
   }
   printf("following devices found in static database: \n");
   for (i=0;i<n_devices;i++)
   {
       	printf("\t%s\n",dev_list[i]);
   }

//
// now loop round creating and exporting the devices
//

   for (i=0; i<n_devices; i++)
   {
      	device_ds[i] = new Serial(dev_list[i],error);
	if ((device_ds[i] == 0) || (*error != 0))
	{
		printf("Error when trying to create %s device\n",dev_list[i]);
		return(DS_NOTOK);
	}

//
// export the device onto the network
//

      	status = dev_export((char*)device_ds[i]->GetDevName(),(Device*)device_ds[i],error);

      	printf("startup++() dev_export() returned %d (error = %d)\n",status,*error);
   }

   printf("leaving startup++() and all's OK\n");

   return(DS_OK);
}
