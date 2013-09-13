/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:	main.c
 *
 * Project:     Device Servers in C++
 *
 * Description:	main source code file for testing the AGPowerSupply class
 *		in C++. AGPowerSupply class implements a simulated powersupply
 *		derived from the base classes PowerSupply and Device (root 
 *		class).
 *
 * Author(s):   Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:	June 1995
 *
 * Version:     $Revision: 1.7 $
 *
 * Date:        $Date: 2008-04-06 09:06:37 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <iostream>

#include <API.h>
#include <Device.h>
#include <DevServer.h>
#include <PowerSupply.h>
#include <AGPowerSupply.h>

#define MAX_DEVICES 1000

long minimal_access;

long startup(char *svr_name, DevLong *error)
{
   char **dev_list;
   unsigned int n_devices, i;
   Device *device[MAX_DEVICES];
   devserver ds[MAX_DEVICES];
   short state;
   long status;

   //debug_flag = 0xffffffff;

   printf ("startup++() program to test dserver++ (server name = %s)\n",svr_name);
//
// get the list of device name to be served from the static database
//
   if (db_getdevlist(svr_name,&dev_list,&n_devices,error))
   {
        printf("startup(): db_getdevlist() failed, error %d\n",*error);
        return(-1);
   }
   printf("following devices found in static database: \n\n");
   for (i=0;i<n_devices;i++)
   {
        printf("\t%s\n",dev_list[i]);
   }

//
// now loop round creating and exporting the devices
//
   for (i=0; i<n_devices; i++)
   {
      device[i] = new AGPowerSupply(dev_list[i],error);
//
// test calling Device::State via Device::Command method
//
      device[i]->Command(DevState, NULL, D_VOID_TYPE, (void*)&state, D_SHORT_TYPE, error);

//
// export the device onto the network
//
      status = dev_export((char*)device[i]->GetDevName(),(Device*)device[i], error);

      printf("startup++() dev_export() returned %d (error = %d)\n",status,*error);

      if (dev_import(dev_list[i],0,&ds[i],error) != DS_OK)
      {
         printf("startup(): dev_import(%s) failed , error %s\n",dev_list[i],
                 dev_error_str(*error));
      }
      else
      {
         printf("startup(): dev_import(%s) worked !\n",dev_list[i]);
	 dev_putget(ds[i], DevState, NULL,D_VOID_TYPE, &state, D_SHORT_TYPE, error);
	 printf("startup(): dev_putget(%s,DevState) = %d , error = %d\n",
		 dev_list[i],state,*error);
      }
   }

   return(DS_OK);
}
