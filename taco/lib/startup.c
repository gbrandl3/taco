/*********************************************************************

 File:		startup.c

 Project:	Device Servers

 Description:	Generic startup procedure for any class. The
		startup procedure is the first procedure called
		from main() when the device server starts up.
		All toplevel devices to be created for the device 
		server should be done in startup(). The startup 
		should make use of the database to determine which 
		devices it should create. Initialisation of devices
		is normally done from startup().

 Author(s);	A. Goetz 

 Original:	October 1996

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France

 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#if 0
#ifdef __cplusplus
#include "DeviceFRM.H"
#endif //__cplusplus
#endif


extern long debug_flag;

/********************/
/* generic startup */
/*******************/

long startup(char *svr_name, long *error)
{

#ifndef __cplusplus
   DevServer ds_list[1];
#else
   Device *ds_list[1];
#if 0
   FRMDevice *ds_list[1];
#endif
#endif /* __cplusplus */ 
   int i,status;
/*
 * pointer to list of devices returned by database.
 */ 
   char **dev_list;
   unsigned int dev_no;

/*   debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_DEV_SVR_CLASS | DBG_API);*/


   if (db_getdevlist(svr_name,&dev_list,&dev_no,error))
   {
      printf("startup(): db_getdevlist() failed, error %d\n",*error);
      return(-1);
   }
   else
   {
      printf("following devices found in static database \n\n");
      for (i=0;i<dev_no;i++)
      {
         printf("%s\n",dev_list[i]);
      }
   }

/*
 * create, initialise and export all devices served by this server
 */

   for (i=0; i < dev_no; i++)
   {
#ifndef __cplusplus
      if (ds__create(dev_list[i], devServerClass, &(ds_list[i]),error) != 0)
      {
#else
      if ((ds_list[i] = new Device(dev_list[i],error)) != NULL)
#if 0
      long	lError = 0;
      if ((ds_list[i] = new FRMDevice(dev_list[i], lError)) != NULL)
#endif

      {
#if 0
	*error = lError;
#endif
#endif /* __cplusplus */ 
         return(-1);
      }

/*
 * initialise the newly created powersupply
 */ 
  
      if (ds__method_finder(ds_list[i],DevMethodInitialise)(ds_list[i],error) != 0)
      {
         return(-1);
      }

/*
 * now export it to the outside world
 */

      printf("created %s, going to export it\n",dev_list[i]);
      if (dev_export(dev_list[i],ds_list[i],error) != 0)
      {
         return(-1);
      }
      printf("export worked\n");

   }

   printf("left startup and all's OK\n");
   return(0);
}


