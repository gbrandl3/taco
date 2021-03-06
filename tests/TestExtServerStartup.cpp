/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 ESRF, www.esrf.fr
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
 * File:        TestExtServerStartup.cpp
 *
 * Description: Code for TestExtServer test program
 *              Synopsis : TestExtServer <personal name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:46 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <string>
#include <vector>
#include <iostream>

#include <TestExtServer.h>
#include <TACOExtensions.h>
#include <Admin.h>

long startup( char *serverName, DevLong *e)
{
	extern long debug_flag;
//	debug_flag = DEBUG_ON_OFF | DBG_DEV_SVR_CLASS;

	dev_printdebug(DBG_STARTUP,  "startup: starting device server: %s", serverName);
	TACO::Server::setServerName(serverName);

// Query the list of device names for the corresponding device server from the database
	std::vector<std::string> deviceList;
	try 
	{
		deviceList = TACO::queryDeviceList( serverName);
	}
	catch (const ::TACO::Exception& tmp) 
	{
		*e = tmp;
		dev_printerror(SEND, "startup: error: getting device list failed: %s", tmp.what());
		return DS_NOTOK;
	}

// Allocate memory for the devices
	if ( 0 < Test::DEVICE_MAX && Test::DEVICE_MAX < deviceList.size()) 
	{
		dev_printerror(SEND, "startup: error: too many devices");
		return DS_NOTOK;
	}
	::TACO::Server** devices = new ::TACO::Server* [deviceList.size()];
	if (devices == 0) 
	{
		dev_printerror(SEND, "startup: error: no memory");
		return DS_NOTOK;
	}

	for (unsigned int i = 0; i < deviceList.size(); ++i) 
		dev_printdebug(DBG_STARTUP, "startup: device: %s\n", deviceList[i].c_str());

// Create and export the devices
	unsigned int counter = 0;
	for (unsigned int i = 0; i < deviceList.size(); ++i) 
	{
// Determine which device should be created
		std::string type;
		try 
		{
			type = TACO::queryResource<std::string>( deviceList [i], "type");
		} 
		catch (const ::TACO::Exception& tmp) 
		{
			*e = tmp;
			dev_printdiag(WRITE, "startup: error: cannot get type for: %s : %s\n", deviceList[i].c_str(), tmp.what());
			continue;
		}

// Create the device
		try 
		{
			devices [i] = 0;
			if (type == "TestExtServer") 
			{
				devices [i] = new Test::Ext::Server(deviceList [i], *e);
			} 
			else 
			{
				dev_printdiag(WRITE, "startup: error: unsupported type: %s\n", deviceList[i].c_str());
				continue;
			}
			dev_printdebug(DBG_STARTUP, "startup: created device: %s\n", deviceList[i].c_str());
		}
		catch (const ::TACO::Exception& tmp) 
		{
			*e = tmp;
			dev_printdiag(WRITE, "startup: error: cannot create device: %s : %s\n", deviceList[i].c_str(), tmp.what());
			continue;
		}

// Export the device
		if (dev_export (const_cast<char*>(deviceList [i].c_str()), devices [i], e) != DS_OK) 
		{
			delete devices [i];
			dev_printdiag(WRITE, "startup: error: cannot export device: %s : %s\n", deviceList[i].c_str(), ::TACO::errorString( *e).c_str());
		} 
		else 
		{
			++counter;
			dev_printdebug(DBG_STARTUP, "startup: exported device: %s\n", deviceList[i].c_str());
		}
	}

	if (counter == deviceList.size()) 
	{
		dev_printdebug(DBG_STARTUP, "startup: success\n");
	} 
	else if (counter != 0) 
	{
		dev_printdiag(SEND, "startup: some errors occured\n");
		dev_printerror(SEND, "startup: some errors occured");
	} 
	else 
	{
		delete [] devices;
		dev_printerror(SEND, "startup: failed");
		return DS_NOTOK;
	}
	return DS_OK;
}
