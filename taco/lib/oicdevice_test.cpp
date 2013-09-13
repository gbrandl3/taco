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
 * Project:	Device Servers in C++
 *
 * Description:	main source code file for testing the OICDevice wrapper class
 *		in C++ to create (old) AGPowerSupply OIC devices.
 *		
 *
 * Author(s):	Andy Goetz
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1996
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 13:08:28 $
 *
 *+**********************************************************************/
		
#include <iostream>

#include <API.h>
#include <Device.H>
#include <DevServer.h>
#include <DevServerP.h>
#include <OICDevice.H>
#include <PowerSupply.h>
#include <PowerSupplyP.h>
#include <AGPowerSupply.h>
#include <AGPowerSupplyP.h>

void main (int argc, char **argv)
{
   char **dev_list;
   unsigned int n_devices, i;
   Device *device[10];
   short state;
   long status, error;

   printf ("main++() program to test OICDevice\n");
//
// get the list of device name to be served from the static database
//
//
// now loop round creating and exporting the devices
//
      device[i] = new OICDevice ("TL1/PS-D/D++",(DevServerClass)aGPowerSupply,&error);
//
// test calling Device::State via Device::Command method
//
      device[i]->Command(DevState, NULL, D_VOID_TYPE, (void*)&state, D_SHORT_TYPE, &error);

//
// export the device onto the network
//
      status = dev_export((char*)device[i]->name,(Device*)device[i],(long*)&error);

      printf("main() dev_export() returned %d (error = %d)\n",status,error);
}
