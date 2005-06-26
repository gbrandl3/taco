#include <API.h>
#include <Admin.h>
#include <DevServer.h>
#include "test_serverP.h"
#include "test_server.h"

#include <iostream>
#include <iomanip>

TestDevice::TestDevice(const std::string name, long &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
	static Device::DeviceCommandListEntry commands_list[] = {
		{DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE, 0, "DevState"},
		{DevStatus, &Device::Status, D_VOID_TYPE, D_STRING_TYPE, 0, "DevStatus"},
		{DevOn, &Device::On, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOn"},
		{DevOff, &Device::Off, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOff"},
		{DevReset, &Device::Reset, D_VOID_TYPE, D_VOID_TYPE, 0, "DevReset"},
		{SendByteArray, (DeviceMemberFunction)&TestDevice::tacoSendByteArray, D_VOID_TYPE, D_VAR_CHARARR, 0, "SendByteArray"},
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

long TestDevice::tacoSendByteArray(void *argin, void *argout, long *error)
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

extern "C" long startup(char *serverName, long *error)
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
			long		lError;
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

TestDevice2::TestDevice2(const std::string name, long &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
	static Device::DeviceCommandListEntry commands_list[] = {
		{DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE, 0, "DevState"},
		{DevStatus, &Device::Status, D_VOID_TYPE, D_STRING_TYPE, 0, "DevStatus"},
		{DevOn, &Device::On, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOn"},
		{DevOff, &Device::Off, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOff"},
		{DevReset, &Device::Reset, D_VOID_TYPE, D_VOID_TYPE, 0, "DevReset"},
		{ReadByteArray, (DeviceMemberFunction)&TestDevice2::tacoRead, D_VAR_CHARARR, D_VOID_TYPE, 0, "ReadByteArray"},
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
        if (db_getresource(const_cast<char *>(name.c_str()), &r, 1, &error) == DS_OK) 
	{
		std::cout << "Longresource = " << value << std::endl;
	}
}

TestDevice2::~TestDevice2()
{
}

long TestDevice2::tacoRead(void *argin, void *argout, long *error)
{
	*error = DS_OK;
	DevVarCharArray	*p; 
	p = (DevVarCharArray *)argin;
	return DS_OK;
}
