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
 * File:	startup.c
 *
 * Project:	Device Servers
 *
 * Description:	Generic startup procedure for any class. The
 *		startup procedure is the first procedure called
 *		from main() when the device server starts up.
 *		All toplevel devices to be created for the device 
 *		server should be done in startup(). The startup 
 *		should make use of the database to determine which 
 *		devices it should create. Initialisation of devices
 *		is normally done from startup().
 *
 * Author(s);	A. Goetz 
 *		$Author: andy_gotz $
 *
 * Original:	October 1996
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2005-12-09 15:11:25 $
 *
 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#ifdef __cplusplus
#include "Device.h"
#endif //__cplusplus

extern long debug_flag;

/**
 * Generic startup procedure for any class. The
 * startup procedure is the first procedure called
 * from main() when the device server starts up.
 * All toplevel devices to be created for the device 
 * server should be done in startup(). The startup 
 * should make use of the database to determine which 
 * devices it should create. Initialisation of devices
 * is normally done from startup(). 
 *
 * For your own device servers you should overwrite this routine by
 * linking a file containing your own startup routine statically
 * to the server. Due to the mechanism of dynamically loading unresolved
 * symbols at runtime, this routine will never called if the startup
 * routine is linked statically. Your server should statically linked against
 * the TACO libraries.
 *
 * @param svr_name 	the name of the server
 * @param error		the error code in case of error
 *
 * @return DS_NOTOK in case of error, DS_OK else
 */
long startup(char *svr_name, long *error)
{
#ifndef __cplusplus
	DevServer 	ds_list[1];
#else
	Device	 	*ds_list[1];
#endif /* __cplusplus */ 
	int 		i,
			status;
/*
 * pointer to list of devices returned by database.
 */ 
	char	 	**dev_list;
   	unsigned int 	dev_no;
   	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_DEV_SVR_CLASS | DBG_API);
	if (db_getdevlist(svr_name,&dev_list,&dev_no,error))
	{
		fprintf(stderr, "startup(): db_getdevlist() failed, error %d\n",*error);
		return(DS_NOTOK);
	}
	else
	{
		printf("following devices found in static database \n\n");
		for (i=0;i<dev_no;i++)
			printf("%s\n",dev_list[i]);
	}
/*
 * create, initialise and export all devices served by this server
 */
	for (i=0; i < dev_no; i++)
	{
#ifndef __cplusplus
		if (ds__create(dev_list[i], devServerClass, &(ds_list[i]),error) == DS_OK)
		{
/*
 * initialise the newly created device
 */ 
			if (ds__method_finder(ds_list[i],DevMethodInitialise)(ds_list[i],error) != DS_OK)
		        	return(DS_NOTOK);
		}
		else
#else
      		if ((ds_list[i] = new Device(dev_list[i],error)) == NULL)
#endif /* __cplusplus */ 
         			return(DS_NOTOK);
/*
 * now export it to the outside world
 */
		printf("created %s, going to export it\n",dev_list[i]);
      		if (dev_export(dev_list[i],ds_list[i],error) != DS_OK)
         		return(DS_NOTOK);
      		printf("export worked\n");
   	}
   	printf("left startup and all's OK\n");
   	return(DS_OK);
}

