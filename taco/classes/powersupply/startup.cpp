
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
//
// Original:	June 1995
//
// $Revision: 1.2 $
//
// $Date: 2004-02-19 15:36:33 $
//
// $Author: jkrueger1 $
//
//+**********************************************************************
		
#include <iostream>

#include <API.h>
#include <Device.H>
#include <DevServer.h>
#include <PowerSupply.h>
#include <AGPowerSupply.h>

#define MAX_DEVICES 1000

long startup(char *svr_name, long *error)
{
   char **dev_list;
   unsigned int n_devices, i;
   Device *device[MAX_DEVICES];
   devserver ds[MAX_DEVICES];
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
      device[i] = new AGPowerSupply(dev_list[i],error);
//
// test calling Device::State via Device::Command method
//
      device[i]->Command(DevState, NULL, D_VOID_TYPE, (void*)&state, D_SHORT_TYPE, error);

//
// export the device onto the network
//
      status = dev_export((char*)device[i]->name,(Device*)device[i],(long*)error);

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
