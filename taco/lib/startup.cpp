
//+**********************************************************************
//
// File:	main.c
//
// Project:	Device Servers in C++
//
// Description:	main source code file for testing the AGPowerSupply class
//		in C++. AGPowerSupply class implements a simulated powersupply
//		derived from the base classes PowerSupply and Device (root 
//		class).
//		
//
// Author(s):	Andy Goetz
// 		$Author: jkrueger1 $
//
// Original:	June 1995
//
// Version:	$Revision: 1.2 $
//
// Date:	$Date: 2003-05-02 09:12:49 $
//
//+**********************************************************************
		
#include <iostream>

#include <API.h>
#include <Device.H>
#include <DevServer.h>
#include <DevServerP.h>
#include <OICDevice.H>
// #include <PowerSupply.h>
// #include <PowerSupplyP.h>
// #include <AGPowerSupply.h>
// #include <AGPowerSupplyP.h>

#define MAX_DEVICES 1000

//extern "C" { long startup(char *svr_name, long *error);}

long startup(char *svr_name, long *error)
{
   char **dev_list;
   unsigned int n_devices, i;
   OICDevice *device[MAX_DEVICES];
   short state;
   long status;

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
//
// DO NOT create AGPowerSupply (C++) objects
//
//    device[i] = new AGPowerSupply(dev_list[i],error);

//
// create old (OIC) AGPowerSupply objects
//
      device[i] = new OICDevice(dev_list[i],(DevServerClass)aGPowerSupplyClass,error); 
//
// test calling Device::State via Device::Command method
//
      device[i]->Command(DevState, NULL, D_VOID_TYPE, (void*)&state, D_SHORT_TYPE, error);

//
// export the device onto the network
//
      status = dev_export((char*)device[i]->name,(Device*)device[i],(long*)error);

      printf("startup++(): dev_export() returned %d (error = %d)\n",status,*error);

//
// test the method finder
//
      printf("startup++(): OICDevice::MethodFinder() DevMethodStateHandler 0x%08x DevMethodCommandHandler 0x%08x\n",
	     device[i]->MethodFinder(DevMethodStateHandler),
	     device[i]->MethodFinder(DevMethodCommandHandler));
      printf("startup++(): calling DevMethodStateHandler via OICDevice::MethodFinder ... returned %d\n",
             (device[i]->MethodFinder(DevMethodStateHandler))(device[i]->get_ds(), DevState, &error));
   }

   return(DS_OK);
}
