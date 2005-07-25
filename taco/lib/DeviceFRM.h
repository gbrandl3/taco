/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2000-2005 by Björn Pedersen, <bpedersen@users.sf.net>
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
 * File:	FRMDevice.h
 *		
 * Description:
 *
 * Author(s):	Björn Pedersen
 *		$Author: jkrueger1 $
 *
 * Original:
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 13:08:22 $
 *
 *******************************************************************************
 */

#ifndef FRMDEVICE_H__
#define FRMDEVICE_H__

#include <map>
#include <string>
#include <API.h>
#include <DevCmds.h>
#include "DeviceBase.h"

class FRMDevice:public DeviceBase
{
private:
    static const short	VER_MAJOR = 0;
    static const short  VER_MINOR = 0;
    static const short  VER_RELEASE = 1;
    static short	classInited;

public:
    typedef long	DevEvent;
/*
    typedef long	(FRMDevice::*DeviceMemberFunction)(void *, void *); 
    typedef struct	_DeviceCommandListEntry
  			{
  			    DeviceMemberFunction	fn;
  			    DevArgType			arginType;
  			    DevArgType			argoutType;
  			    long			minAccess;
			    _DeviceCommandListEntry()
			    {
				fn = NULL;
				arginType = argoutType = minAccess = 0;
			    };
			    _DeviceCommandListEntry(DeviceMemberFunction f, 
						DevArgType in, DevArgType out, long acc)
			    {
				fn = f;
				arginType = in;
				argoutType = out;
				minAccess = acc;
			    };
  			}DeviceCommandListEntry;
    typedef std::map<DevCommand, DeviceCommandListEntry>	DeviceCommandList;

    typedef struct	_DeviceEventListEntry
			{
			    DevArgType			argType;
			    _DeviceEventListEntry()
			    {
				argType = 0;
			    };
			    _DeviceEventListEntry(DevArgType type)
			    {
				argType = type;
			    };
			}DeviceEventListEntry;
    typedef std::map<DevEvent, DeviceEventListEntry>	DeviceEventList;
*/
protected:
    std::string			className;
    long			nEvents,
    				nCommands;
    DeviceCommandList   	commandList;
    DeviceEventList		eventList;
    std::string			devType,
    				devName;
    short			state,
    				nextState;

protected:
    virtual long	ClassInitialise(void)
				throw (long);
    virtual long	GetResources(const std::string = "")
				throw (long);
    virtual long	StateMachine(long)
				throw (long);
    virtual short	State(void) throw (long);
    virtual std::string	Status(void) throw (long);
    virtual void	On(void) throw (long);
    virtual void	Off(void) throw (long);
    virtual void	Reset(void) throw (long);
    virtual long	Version(void) throw (long);

    	    long	tacoOn(void *, void *) throw (long);
    	    long	tacoOff(void *, void *) throw (long);
    	    long	tacoState(void *, void *) throw (long);
    	    long	tacoStatus(void *, void *) throw (long);
    	    long	tacoReset(void *, void *) throw (long);
    	    long	tacoVersion(void *, void *) throw (long);
	    long	FindCommand(long) throw (long);
public:
			FRMDevice(const std::string, long &);
    virtual		~FRMDevice();
    virtual long	Command(const long, void *, long, void *, long, long*);

	    const char* GetClassName(void);				// needed in svc_api.c 
	    const char* GetDevType(void);				// needed in svc_api.c 
	    const char* GetDevName(void);				// needed in svc_api.c
	    long	GetMinAccessRight(const long) throw (long);	// needed in sec_api.c
	    unsigned	GetCommandNumber(void);				// needed in svc_api.c
	    long	CommandQuery(_dev_cmd_info *);			// needed in svc_api.c
	    unsigned 	GetEventNumber(void);				// needed in svc_api.c
	    long	EventQuery(_dev_event_info *);			// needed in svc_api.c

//    friend class 	DeviceCommandListEntry;
};

#endif //FRMDEVICE_H_
