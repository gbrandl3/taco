/*
 *******************************************************************************
 * Module:
 *		$Id: DeviceFRM.cpp,v 1.1 2004-11-04 14:02:40 andy_gotz Exp $
 * Description:
 *
 * Author:
 *		$Author: andy_gotz $
 *		
 *******************************************************************************
 */

#include <vector>
#include <string>
#include <iostream>

#include <DeviceFRM.H>

short FRMDevice::classInited = 0;

FRMDevice::FRMDevice(std::string devname, long &lError)
	: devType("Default"), devName(devname),
	  className("FRMDevice"), 
	  nextState(0), state(DEVOFF)
{
    lError = DS_OK;
    try
    { 
        this->ClassInitialise(); 
    }
    catch (const long &Error)
    {
	lError = Error;
        return;
    }

// Initialise the command list for the class
    this->commandList[DevOn] = DeviceCommandListEntry(&FRMDevice::tacoOn, D_VOID_TYPE, D_VOID_TYPE, 0);
    this->commandList[DevOff] = DeviceCommandListEntry(&FRMDevice::tacoOff, D_VOID_TYPE, D_VOID_TYPE, 0);
    this->commandList[DevReset] = DeviceCommandListEntry(&FRMDevice::tacoReset, D_VOID_TYPE, D_VOID_TYPE, 0);
    this->commandList[DevState] = DeviceCommandListEntry(&FRMDevice::tacoState, D_VOID_TYPE, D_SHORT_TYPE, 0);
    this->commandList[DevStatus] = DeviceCommandListEntry(&FRMDevice::tacoStatus, D_VOID_TYPE, D_STRING_TYPE, 0);
// DevVersion is a frm-extension
//    this->commandList[DevVersion] = DeviceCommandListEntry(&FRMDevice::tacoVersion, D_VOID_TYPE, D_LONG_TYPE, 0);
    this->state = DEVOFF;
}

/* 
   do only things inside here what have to do only for the whole class
   i.e. initialisation of class variables and so on
*/
long FRMDevice::ClassInitialise(void) throw (long)
{
    if (this->classInited)
	return DS_OK;

    this->classInited = 1;
    return DS_OK;
}

long FRMDevice::GetResources(const std::string name) throw (long)
{
    return DS_OK;
}

long FRMDevice::StateMachine(long cmd) throw (long)
{
// default state machine is to allow all commands
   switch (cmd)
   {
      default : break;
   }
   return (DS_OK);                                                                                
}

FRMDevice::~FRMDevice()
{
    // this->classInited = 0;
    this->state = DEVOFF;
}

long FRMDevice::tacoOn(void *argin, void *argout) throw (long)
{
    try
    {
	this->On();
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

void FRMDevice::On(void) throw (long) 
{
    this->state = DEVON;
}

long FRMDevice::tacoOff(void *argin, void *argout) throw (long)
{
    try
    {
	this->Off();
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

void FRMDevice::Off(void) throw (long)
{
    this->state = DEVOFF;
}

long FRMDevice::tacoState(void *argin, void *argout) throw (long)
{
    try
    {
	*(short *)argout = this->State();
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

short FRMDevice::State(void) throw (long) 
{
    return (short)this->state;
}


long FRMDevice::tacoStatus(void *argin, void *argout) throw (long)
{
    try
    {
	*(char **)argout = const_cast<char *>(this->Status().c_str());
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

std::string FRMDevice::Status(void) throw (long)
{
    return std::string(this->state == DEVON ? "Device is on" : "Device is off");
}

long FRMDevice::tacoReset(void *argin, void *argout) throw (long)
{
    try
    {
	this->Reset();
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

void FRMDevice::Reset(void) throw (long)
{
    this->state = DEVOFF;
}

long FRMDevice::tacoVersion(void *argin, void *argout) throw (long)
{
    try
    {
	*(long *)argout = this->Version();
    }
    catch (const long &lError)
    {
	throw lError;
    }
    return (DS_OK);
}

long FRMDevice::Version(void) throw (long)
{
    return (long)(10000 * this->VER_MAJOR + 100 * this->VER_MINOR + this->VER_RELEASE);
}

long FRMDevice::Command(long cmd, void *argin, long intype, void *argout, long outtype, long *error)
{
    DeviceMemberFunction mFunc;
    try
    {
        DeviceCommandList::iterator p = this->commandList.find(cmd);
	if (p == this->commandList.end())
	{
	    *error = DevErr_CommandNotImplemented;
	    return (DS_NOTOK);
        }
        if (!((p->second.arginType == intype) && (p->second.argoutType == outtype)))
        {
            *error = DevErr_IncompatibleCmdArgumentTypes;
            return (DS_NOTOK);
        }
        if (this->StateMachine(cmd) == DS_OK) 	// check state machine
	{					// now execute the command
	    mFunc = p->second.fn;
	    return (this->*mFunc)(argin, argout);
	}
    }
    catch (const long &lError)
    {
	*error = lError;
        return (DS_NOTOK);
    }
}

const char *FRMDevice::GetClassName(void)
{
    return this->className.c_str();
}

const char *FRMDevice::GetDevName(void)
{
    return this->devName.c_str();
}

const char *FRMDevice::GetDevType(void)
{
    return this->devType.c_str();
}

long FRMDevice::GetMinAccessRight(long cmd) throw (long)
{
    DeviceCommandList::iterator p = this->commandList.find(cmd);
    if (p != this->commandList.end())
        return this->commandList[cmd].minAccess;
    else
	throw long(DevErr_CommandNotImplemented);
}

unsigned FRMDevice::GetCommandNumber(void)
{
    return this->commandList.size();
}

unsigned FRMDevice:: GetEventNumber(void)
{
    return this->eventList.size();
}

long FRMDevice::CommandQuery(_dev_cmd_info *cmdInfo)
{
    int	i = 0;
    for (DeviceCommandList::iterator p = this->commandList.begin(); 
		p != this->commandList.end(); ++p, ++i)
    {
        cmdInfo[i].cmd = p->first;
        cmdInfo[i].in_type = p->second.arginType;
        cmdInfo[i].out_type = p->second.argoutType;
    }
    return (DS_OK);
}

long FRMDevice::EventQuery(_dev_event_info *eventInfo)
{
    int	i = 0;

    for (DeviceEventList::iterator p = this->eventList.begin();
		p != this->eventList.end(); ++p, ++i)
    {
        eventInfo[i].event = p->first;
        eventInfo[i].out_type = p->second.argType;
    }

    return (DS_OK);
}
