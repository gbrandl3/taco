//+**********************************************************************
//
// File:	DeviceBase.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing the device server Device base class in C++
//		(DeviceClass).
//
// Author(s):	Björn Pedersen
// 		$Author: jkrueger1 $
//
// Original:	 
//
// $Revision: 1.2 $
//
// $Date: 2004-09-17 07:53:16 $
//
//+**********************************************************************

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
	virtual unsigned int 	GetCommandNumber() {return commands_list.size();}
	virtual long 		CommandQuery(_dev_cmd_info *sequence) = 0;
	virtual unsigned 	GetEventNumber(void) {return events_list.size();}
	virtual long		EventQuery(_dev_event_info *) = 0;
	virtual long		GetMinAccessRight(const long) = 0;
//
// protected members - accessible only be derived classes
// 
	typedef long	DevEvent;
	typedef long 	(DeviceBase::*DeviceMemberFunction)(void*, void*, long*);

	typedef struct  _DeviceCommandListEntry
			{
				DevCommand         	cmd;
				DeviceMemberFunction	fn;
				DevArgType		arginType;
				DevArgType		argoutType;
				long			minAccess;
				char			*cmd_name;
				_DeviceCommandListEntry() : cmd(0), fn(NULL), arginType(0), argoutType(0), minAccess(0), cmd_name(NULL) {}
				_DeviceCommandListEntry(DevCommand _cmd, DeviceMemberFunction f, DevArgType in, DevArgType out, 
							long acc, char *_cmd_name = NULL) 
					: cmd(_cmd), fn(f), arginType(in), argoutType(out), minAccess(acc), cmd_name(_cmd_name)

				{}
			}DeviceCommandListEntry;
	typedef std::map<DevCommand, DeviceCommandListEntry>        	DeviceCommandList;

	typedef struct  _DeviceEventListEntry
                        {
				DevEvent	event;
				DevArgType	argType;
				char		*event_name;
				_DeviceEventListEntry() : event(0), argType(0), event_name(NULL) {}
				_DeviceEventListEntry(DevEvent _event, DevArgType type, char *_event_name) 
					: event(_event), argType(type), event_name(_event_name) {}
			}DeviceEventListEntry;
	typedef std::map<DevEvent, DeviceEventListEntry>    DeviceEventList;

	DeviceCommandList commands_list;
	DeviceEventList events_list;

protected:

};

#define TYPE_DEFAULT		"DevType_Default"
#define TYPE_INIT 		"DevType_"

#endif /* _TACO_DEVICE_H */
