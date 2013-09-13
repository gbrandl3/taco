/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2000-2013 by Björn Pedersen <bpedersen@users.sf.net>
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
 * File:	DeviceBase.h
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing the device server Device base class in C++
 *		(DeviceClass).
 *
 * Author(s):	Björn Pedersen
 * 		$Author: jkrueger1 $
 *
 * Original:	 
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:06:59 $
 *
 *+**********************************************************************/

#ifndef _TACO_DEVICE_BASE_H
#define _TACO_DEVICE_BASE_H
		
#ifdef EBUG
#include <iostream>
#endif

#include <map>

// Some remarks about the Device class definition
//
// 1 - Members class_name and dev_type should not be defined as static members
//     otherwise, there will be only one copy of them for the device server
//     process and it is not possible to correctly handle device server
//     with several embedded classes
//     Therefore, don't forget to initialize them in the object constructor
//     and not in the class_initialise function which is executed only once
//     for a class.
//
// 2 - The State and Status member function are declared as public. This is due
//     to the OS-9 C++ compiler. To reuse them in a device derived class
//     (by specifying a pointer to them in the command list), the OS-9 compiler
//     needs the function to be declared as public !!
//

typedef long DevEvent  ;

// common base class for ESRF/FRM classes
// this class only declares functions used in the api library
// every thing else is in the derived classes
class DeviceBase {
//
// public members
//
public:
	DeviceBase()
	{
#ifdef EBUG
		cerr<<"DevBase constructor" << endl;
#endif
	}
	
	virtual ~DeviceBase ()
	{
#ifdef EBUG
		cerr<<"DevBase destructor" << endl;
#endif
	}

//
// following method is "virtual" which means derived classes can
// override it with their own version
//
	virtual long Command ( long cmd, 
                  void *argin, long argin_type,
                  void *argout, long argout_type, 
                  long *error) = 0;
//
// mapper functions for FRM Device class compatibility
//
	virtual const char 	*GetClassName() = 0;
	virtual const char 	*GetDevType() = 0;
	virtual const char 	*GetDevName() = 0;
	virtual unsigned int 	GetCommandNumber() {return commands_map.size();}
	virtual long 		CommandQuery(_dev_cmd_info *sequence) = 0;
	virtual unsigned 	GetEventNumber(void) {return events_list.size();}
	virtual long		EventQuery(_dev_event_info *) = 0;
	virtual long		GetMinAccessRight(const long) = 0;
//
// protected members - accessible only be derived classes
// 
	typedef long	DevEvent;
	typedef long 	(DeviceBase::*DeviceBaseMemberFunction)(void*, void*, long*);

	typedef struct  _DeviceCommandMapEntry
			{
				DevCommand         	cmd;
				DeviceBaseMemberFunction	fn;
				DevArgType		arginType;
				DevArgType		argoutType;
				long			minAccess;
				const char			*cmd_name;
				_DeviceCommandMapEntry() : cmd(0), fn(NULL), arginType(0), argoutType(0), minAccess(0), cmd_name(NULL) {};
				_DeviceCommandMapEntry(DevCommand _cmd, DeviceBaseMemberFunction f, DevArgType in, DevArgType out, 
							long acc, char *_cmd_name = NULL) 
					: cmd(_cmd), fn(f), arginType(in), argoutType(out), minAccess(acc), cmd_name(_cmd_name)	{};
			}DeviceCommandMapEntry;
	typedef std::map<DevCommand, DeviceCommandMapEntry>        	DeviceCommandMap;

	typedef struct  _DeviceEventListEntry
                        {
				DevEvent	event;
				DevArgType	argType;
				const char		*event_name;
				_DeviceEventListEntry() : event(0), argType(0), event_name(NULL) {};
				_DeviceEventListEntry(DevEvent _event, DevArgType type, char *_event_name) 
					: event(_event), argType(type), event_name(_event_name) {};
			}DeviceEventListEntry;
	typedef std::map<DevEvent, DeviceEventListEntry>    DeviceEventList;

	DeviceCommandMap commands_map;
	DeviceEventList events_list;

protected:

};

#define TYPE_DEFAULT		"DevType_Default"
#define TYPE_INIT 		"DevType_"

#endif /* _TACO_DEVICE_H */
