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
	commands_map[DevState] = DeviceCommandMapEntry(DevState, static_cast<DeviceBaseMemberFunction>(&Device::State),
				D_VOID_TYPE, D_SHORT_TYPE, 0, "State");
	commands_map[DevStatus] = DeviceCommandMapEntry(DevStatus, static_cast<DeviceBaseMemberFunction>(&Device::Status),
				D_VOID_TYPE, D_STRING_TYPE, 0, "Status");
	commands_map[DevReset] = DeviceCommandMapEntry(DevReset, static_cast<DeviceBaseMemberFunction>(&Device::Reset),
				D_VOID_TYPE, D_VOID_TYPE, 0, "Reset");
	commands_map[DevOn] = DeviceCommandMapEntry(DevOn, static_cast<DeviceBaseMemberFunction>(&Device::On),
				D_VOID_TYPE, D_VOID_TYPE, 0, "On");
	commands_map[DevOff] = DeviceCommandMapEntry(DevOff, static_cast<DeviceBaseMemberFunction>(&Device::Off),
				D_VOID_TYPE, D_VOID_TYPE, 0, "Off");
	commands_map[DevClose] = DeviceCommandMapEntry(DevClose, static_cast<DeviceBaseMemberFunction>(&Device::Close),
				D_VOID_TYPE, D_VOID_TYPE, 0, "Close");
	commands_map[SendByteArray] = DeviceCommandMapEntry(SendByteArray, static_cast<DeviceBaseMemberFunction>(&TestDevice::tacoSendByteArray),
				D_VOID_TYPE, D_VAR_CHARARR, 0, "SendByteArray");

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

//        debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_DEV_SVR_CLASS | DBG_API);

	if (db_getdevlist(serverName, &dev_list, &dev_no, error) != DS_OK)
	{
		std::cerr << "startup(): db_getdevlist() failed, error = "<< *error << std::endl; 
		return(DS_NOTOK);
        }
        else
        {
		for (unsigned int i = 0; i < dev_no; ++i)
		{
			long		lError;
			DeviceBase	*dev = new TestDevice(dev_list[i], lError);
			if (dev)
				dev_export(dev_list[i], dev, error);
			free(dev_list[i]);
		}
		free(dev_list);
	}
	*error = 0;
	return DS_OK;
}

