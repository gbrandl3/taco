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
 * File:        test_server.cpp
 *
 * Description: Code for test_server test program
 *              Synopsis : test_server <personal name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.13 $
 *
 * Date:        $Date: 2008-04-30 13:42:48 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <API.h>
#include <Admin.h>
#include <DevServer.h>
#include "test_serverP.h"
#include "test_server.h"

#include <iostream>
#include <iomanip>

TestDevice::TestDevice(const std::string name, DevLong &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
	static Device::DeviceCommandListEntry commands_list[] = {
		DeviceCommandListEntry(DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE, READ_ACCESS, "DevState"),
		DeviceCommandListEntry(DevStatus, &Device::Status, D_VOID_TYPE, D_STRING_TYPE, READ_ACCESS, "DevStatus"),
		DeviceCommandListEntry(DevOn, &Device::On, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevOn"),
		DeviceCommandListEntry(DevOff, &Device::Off, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevOff"),
		DeviceCommandListEntry(DevReset, &Device::Reset, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevReset"),
		DeviceCommandListEntry(SendByteArray, (DeviceMemberFunction)&TestDevice::tacoSendByteArray, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "SendByteArray"),
		};
	static long n_commands = sizeof(commands_list) / sizeof(DeviceCommandListEntry);
//
// initialise the commands list to point to the commands list
// implemented for the AG PowerSupply class
//
	this->n_commands = n_commands;
	this->commands_list = commands_list;

	events_list[HeartBeat] = DeviceEventListEntry(HeartBeat, D_LONG_TYPE, "HeartBeat");
	this->class_name = "TestDevice";
}

TestDevice::~TestDevice()
{
}

long TestDevice::tacoSendByteArray(void *argin, void *argout, DevLong *error)
{
	static	DevVarCharArray	*p; 
	static	DevVarCharArray	arr = {0, NULL};
	if (arr.sequence == NULL)
	{
		arr.length = 8192 - sizeof(u_int);
		arr.sequence = new DevBoolean[arr.length];
		for (int i = 0; i < 10; ++i)
		{
			arr.sequence[i] = 'A' + i;
			arr.sequence[arr.length - 1 - i] = 'Z' - i;
		}
	} 
	p = (DevVarCharArray *)argout;
	p->length = arr.length;
	p->sequence = arr.sequence;	
	return DS_OK;
}

extern "C" long startup(char *serverName, DevLong *error)
{
/*
 * pointer to list of devices returned by database.
 */
	char            **dev_list;
	unsigned int    dev_no;

//      debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_STARTUP);

	if (db_getdevlist(serverName, &dev_list, &dev_no, error) != DS_OK)
	{
		std::cerr << "startup(): db_getdevlist() failed, error = "<< *error << std::endl; 
		return(DS_NOTOK);
        }
        else
        {
		dev_printdebug(DBG_TRACE | DBG_STARTUP, "Number of devices: %d\n", dev_no);
		Device	**dev = new Device*[dev_no];
		for (unsigned int i = 0; i < dev_no; ++i)
		{
			DevLong		lError;
			db_resource	r;
			char		*type;
                	r.resource_name = "type";
                	r.resource_type = D_STRING_TYPE;
                	r.resource_adr = &type;
                	if (db_getresource( dev_list[i], &r, 1, &lError) == DS_OK) 
			{
				if (std::string(type) == "TestDevice")
				dev[i] = new TestDevice(dev_list[i], lError);
				else if (std::string(type) == "TestDevice2")
					dev[i] = new TestDevice2(dev_list[i], lError);
				else
					continue;
				if (dev[i])
					dev_export(dev_list[i], dev[i], error);
			}
			else
				fprintf(stdout, "could not get resource \"type\": %d\n", lError); 
			free(dev_list[i]);
		}
		free(dev_list);
	}
	*error = 0;
        debug_flag = 0;
	return DS_OK;
}

TestDevice2::TestDevice2(const std::string name, DevLong &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
	static Device::DeviceCommandListEntry commands_list[] = {
		DeviceCommandListEntry(DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE, READ_ACCESS, "DevState"),
		DeviceCommandListEntry(DevStatus, &Device::Status, D_VOID_TYPE, D_STRING_TYPE, READ_ACCESS, "DevStatus"),
		DeviceCommandListEntry(DevOn, &Device::On, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevOn"),
		DeviceCommandListEntry(DevOff, &Device::Off, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevOff"),
		DeviceCommandListEntry(DevReset, &Device::Reset, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "DevReset"),
		DeviceCommandListEntry(ReadByteArray, (DeviceMemberFunction)&TestDevice2::tacoRead, D_VAR_CHARARR, D_VOID_TYPE, READ_ACCESS, "ReadByteArray"),
		};
	static long n_commands = sizeof(commands_list) / sizeof(DeviceCommandListEntry);
//
// initialise the commands list to point to the commands list
// implemented for the AG PowerSupply class
//
	this->n_commands = n_commands;
	this->commands_list = commands_list;


	this->class_name = "TestDevice2";
	long	value;
	db_resource	r;
        r.resource_name = "longresource";
        r.resource_type = D_LONG_TYPE;
        r.resource_adr = &value;
        if (db_getresource(const_cast<char *>(name.c_str()), &r, 1, &error) != DS_OK) 
	{
		std::cout << "Longresource = " << value << std::endl;
	}
}

TestDevice2::~TestDevice2()
{
}

long TestDevice2::tacoRead(void *argin, void *argout, DevLong *error)
{
	*error = DS_OK;
	DevVarCharArray	*p; 
	p = (DevVarCharArray *)argin;
	return DS_OK;
}
