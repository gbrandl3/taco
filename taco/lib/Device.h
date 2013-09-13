/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:	Device.h
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing the device server Device base class in C++
 *		(DeviceClass).
 *
 * Author(s):	Andy Goetz
 * 		$Author: jkrueger1 $
 *
 * Original:	February 1995
 *
 * Version:	$Revision: 1.15 $
 *
 * Date:	$Date: 2008-12-15 08:21:55 $
 *
 +**********************************************************************/

#ifndef _TACO_DEVICE_H
#define _TACO_DEVICE_H
		
#include "db_setup.h"
#include <iostream>
#include <map>
#include <DevCmds.h>

class Device 
{
//
// private members
//

private :
//
// private virtual functions which should be defined in each new sub-class
//
	static short class_inited;

	virtual long ClassInitialise( DevLong *error );
	virtual long GetResources (const char *res_name, DevLong *error);  

//
// public members
//
public:

typedef long (Device::* DeviceMemberFunction)(void*, void*, DevLong* );
typedef struct _DeviceCommandListEntry {
	DevCommand         	cmd;
	DeviceMemberFunction  	fn;
	DevArgType         	argin_type;
	DevArgType         	argout_type;
	long               	min_access;
	const char              *cmd_name;

/* the following code will force the command list array to be initialised with DeviceCommandListEntry() 
 * objects and not with braces i.e. {DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE}
 */
#ifdef TACO_EXT
	_DeviceCommandListEntry() 
		: cmd(0)
		, fn(NULL) 
		, argin_type(0)
		, argout_type(0)
		, min_access(0)
		, cmd_name(NULL) 
	{};
	_DeviceCommandListEntry(DevCommand _cmd, 
				DeviceMemberFunction f, 
				DevArgType in, 
				DevArgType out, 
				long acc, 
				const char *_cmd_name = NULL) 
		: cmd(_cmd), fn(f)
		, argin_type(in), argout_type(out)
		, min_access(acc)
		, cmd_name(_cmd_name)	
	{};
#endif /* TACO_EXT */
} DeviceCommandListEntry;

typedef struct _DeviceCommandListEntry *DeviceCommandList;

typedef std::map<DevCommand, DeviceCommandListEntry> DeviceCommandMap;

typedef struct  _DeviceEventListEntry {
	DevEvent        event;
	DevArgType      argType;
       	const char      *event_name;
       	_DeviceEventListEntry() 
		: event(0)
		, argType(0)
		, event_name(NULL) 
	{};
       	_DeviceEventListEntry(DevEvent _event, 
				DevArgType type, 
				const char *_event_name)
		: event(_event)
		, argType(type)
		, event_name(_event_name) 
	{};
} DeviceEventListEntry;
        
typedef std::map<DevEvent, DeviceEventListEntry>    DeviceEventList;

public:	// why
	DeviceCommandMap commands_map;
	DeviceEventList events_list;

public:
	virtual long State(DevArgument vargin, DevArgument vargout , DevLong *error);
	virtual long Status(DevArgument vargin, DevArgument vargout, DevLong *error);
	virtual long On(DevArgument vargin, DevArgument vargout, DevLong *error);
	virtual long Off(DevArgument vargin, DevArgument vargout, DevLong *error);
	virtual long Reset(DevArgument vargin, DevArgument vargout, DevLong *error);
	virtual long Close(DevArgument vargin, DevArgument vargout, DevLong *error);

protected:
// 
// class variables
//  
	char* class_name;
	char dev_type[DEV_TYPE_LENGTH];
#ifndef TACO_EXT
public:
#endif
	char* name;
public:
	/**
	 * constructor to create an object of the base class Device
	 *
	 * @param devname name (ascii identifier) of device to create
	 * @param error error code returned in the case of problems
	 *
	 */
	Device (const DevString devname, DevLong *error)
		: name(NULL)
	{
		init(devname, error);
	}

	
	/**
	 * constructor to create an object of the base class Device
	 *
	 * @param devname name (ascii identifier) of device to create
	 * @param error error code returned in the case of problems
	 *
	 * @overload Device (const char *devname, DevLong *error)
	 */
	Device (const char *devname, DevLong *error)
		: name(NULL)
	{
		init(devname, error);
	}

	/**
	 * constructor to create an object of the base class Device
	 *
	 * @param devname name (ascii identifier) of device to create
	 * @param error error code returned in the case of problems
	 *
	 * @overload Device (const std::string  &devname, DevLong *error)
	 */
	Device (const std::string &devname, DevLong *error)
		: name(NULL)
	{
		init(devname.c_str(), error);
	}

	virtual ~Device ();
//
// following method is "virtual" which means derived classes can
// override it with their own version
//
	virtual long Command ( DevCommand cmd, 
                  DevArgument argin, DevArgType argin_type,
                  DevArgument argout, DevArgType argout_type, 
                   DevLong *error);
	void  Get_event_number(unsigned int *);
	long  Event_Query(_dev_event_info *, DevLong *);
	long  Get_min_access_right(DevCommand,long *, DevLong *);
	void  Get_command_number(unsigned int *);
	void  Get_command_name(unsigned int *, char *);
	long  Command_Query(_dev_cmd_info *, DevLong *);
   
// mapper functions for FRM Device class compatibility

	virtual const char 	*GetClassName() {return this->class_name;};
	virtual const char 	*GetDevType(){return this->dev_type;};
	virtual const char 	*GetDevName(){return this->name;};
	virtual unsigned int 	GetCommandNumber();
	virtual long 		CommandQuery(_dev_cmd_info *sequence);
        virtual unsigned        GetEventNumber(void) {return events_list.size();}
        virtual long            EventQuery(_dev_event_info *);
	virtual long		GetMinAccessRight(const DevCommand);
//
// protected members - accessible only be derived classes
// 
protected:
//
// the following virtual commands must exist in all new sub-classes
//
	virtual long StateMachine( DevCommand cmd,  DevLong *error);

	long n_events;		// number of events
	long state; 		// device state
	long n_state; 		// convenience variable for storing next device state
	long n_commands;	// number of commands
   	DeviceCommandList commands_list; // array of commands (for backwards compatibility @ ESRF)
protected: 
	/**
	 * constructor to create an object of the base class Device
	 */
	Device(){};

private:
	void init(const char *name, DevLong *error);
};

#define TYPE_DEFAULT            "DevType_Default"
#define TYPE_INIT               "DevType_"

#endif /* _TACO_DEVICE_H */
