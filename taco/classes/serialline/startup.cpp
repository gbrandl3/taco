
//+**********************************************************************
//
// File:	startup.C
//
// Project:	Device Servers in C++
//
// Description:	Startup file for the Serial class implemented in C++
//		
//
// Author(s):	Manuel Perez
//
// Original:	January 1997 by Emmanuel Taurel, Andy Gotz
//
//+**********************************************************************
		
#include "config.h"
#include <API.h>
#include <DevSignal.h>

#include <DevServer.h> /* will include Device.H */
#include <Serial.h>

// extern void catchSig(int);

Serial *device_ds[MAX_DEVICES];
unsigned int n_devices;

long startup(char *svr_name, long *error)
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

      	status = dev_export((char*)device_ds[i]->name,(Device*)device_ds[i],(long*)error);

      	printf("startup++() dev_export() returned %d (error = %d)\n",status,*error);
   }

   printf("leaving startup++() and all's OK\n");

   return(DS_OK);
}
