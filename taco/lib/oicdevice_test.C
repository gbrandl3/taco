//+**********************************************************************
//
// File:	main.c
//
// Project:	Device Servers in C++
//
// Description:	main source code file for testing the OICDevice wrapper class
//		in C++ to create (old) AGPowerSupply OIC devices.
//		
//
// Author(s):	Andy Goetz
//
// Original:	November 1996
//
// $Revision: 1.1 $
//
// $Date: 2003-04-25 11:21:36 $
//
// $Author: jkrueger1 $
//
//+**********************************************************************
		
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
