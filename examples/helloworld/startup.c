/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2014 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
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
 * File:        hello.c
 *
 * Description: example device server
 *
 * Author(s):   $Author: jkrueger1 $
 *
 * Original:    December 99
 * 
 * Date:	$Date: 2013-05-17 08:05:49 $
 *
 * Version:	$Revision: 1.1 $
 */
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include "helloP.h"

#include <stdio.h>

#define MAX_DEVICES	10

long startup(DevString serverName, DevLong *error)
{
	DevString *dev_list;
	DevShort  state;
	DevLong	  status;
	u_int	  n_devices;
	DevLong	  i = 0;

	Hello	  device[MAX_DEVICES];

        printf("startup() : program to test dserver (server name = %s)\n", serverName);

/*
 * get the list of device name to be served from the static database
 */
	if (db_getdevlist(serverName, &dev_list, &n_devices, error) == DS_NOTOK)
	{
		printf("startup() : db_getdevlist() failed, error = %d\n", error);
		return DS_NOTOK;
	}
	printf("startup() :  following devices found in the static database:\n\n");
	for (i = 0; i < n_devices; ++i)
	{
		printf("\t%s\n", dev_list[i]);
	}

/*
 * create, initialise, and export all devices served by this server
 */
	for (i = 0; i < n_devices; ++i)
	{
		if (ds__create(dev_list[i], helloClass, &(device[i]), error) != DS_OK)
		{
			continue;
		}
/*
 * initialise the newly created device
 */
		if (ds__method_finder(device[i], DevMethodInitialise)(device[i], error) != DS_OK)
		{
			continue;
		}
/*
 * now export it into the outside world
 */
		printf("startup() : created %s, going to export it\n", dev_list[i]);
		if (dev_export(dev_list[i], device[i], error) != DS_OK)
		{
			continue;
		}
		printf("startup() : export worked !\n");
	}
        printf("startup() : left startup and all's OK\n");
	return DS_OK;
}
