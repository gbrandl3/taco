//+**********************************************************************
//
// File:		Device.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing the device server Device base class in C++
//		(DeviceClass).
//
// Author(s):	Andy Goetz
//
// Original:	February 1995
//
// $Revision: 1.2 $
//
// $Date: 2004-11-24 20:45:56 $
//
// $Author: andy_gotz $
//
//+**********************************************************************

#ifndef _TACO_DEVICE_H
#define _TACO_DEVICE_H
		
#include "DeviceBase.h"
#include "db_setup.h"

class Device : public DeviceBase
{
//
// private members
//

private :
//
// private virtual functions which should be defined in each new sub-class
//
	static short class_inited;

	virtual long ClassInitialise( long *error );
	virtual long GetResources (char *res_name, long *error);  

//
// public members
//
public:

typedef long (Device::* DeviceMemberFunction)(void*, void*, long* );
typedef struct _DeviceCommandListEntry {
                                     DevCommand         cmd;
                                     DeviceMemberFunction  fn;
                                     DevArgType         argin_type;
                                     DevArgType         argout_type;
                                     long               min_access;
                                     char               *cmd_name;
				_DeviceCommandListEntry() : cmd(0), fn(NULL), argin_type(0), argout_type(0), min_access(0), cmd_name(NULL) {};
				_DeviceCommandListEntry(DevCommand _cmd, DeviceMemberFunction f, DevArgType in, DevArgType out, 
							long acc, char *_cmd_name = NULL) 
					: cmd(_cmd), fn(f), argin_type(in), argout_type(out), min_access(acc), cmd_name(_cmd_name)	{};
                                    }
               DeviceCommandListEntry;

typedef struct _DeviceCommandListEntry *DeviceCommandList;

	virtual long State(void *vargin, void *vargout , long *error);
	virtual long Status(void *vargin, void *vargout, long *error);
	virtual long On(void *vargin, void *vargout, long *error);
	virtual long Off(void *vargin, void *vargout, long *error);
	virtual long Reset(void *vargin, void *vargout, long *error);
	virtual long Close(void *vargin, void *vargout, long *error);
// 
// class variables
//
  
	char* class_name;
	char dev_type[DEV_TYPE_LENGTH];

	char* name;



	Device (DevString name, long *error);
	virtual ~Device ();
//
// following method is "virtual" which means derived classes can
// override it with their own version
//
	virtual long Command ( long cmd, 
                  void *argin, long argin_type,
                  void *argout, long argout_type, 
                  long *error);
	void  Get_event_number(unsigned int *);
	long  Event_Query(_dev_event_info *,long *);
	long  Get_min_access_right(long,long *,long *);
	void  Get_command_number(unsigned int *);
	void  Get_command_name(unsigned int *, char *);
	long  Command_Query(_dev_cmd_info *,long *);
   
// mapper functions for FRM Device class compatibility

	virtual const char 	*GetClassName() {return this->class_name;};
	virtual const char 	*GetDevType(){return this->dev_type;};
	virtual const char 	*GetDevName(){return this->name;};
//	virtual unsigned int 	GetCommandNumber(){return this->n_commands;};
	virtual long 		CommandQuery(_dev_cmd_info *sequence);
//	virtual unsigned 	GetEventNumber(void){return this->n_events;};
	virtual long		EventQuery(_dev_event_info *);
	virtual long		GetMinAccessRight(const long);
//
// protected members - accessible only be derived classes
// 
protected:
//
// the following virtual commands must exist in all new sub-classes
//
	virtual long StateMachine( long cmd, long *error);

	long n_events;		// number of events
	long state; 		// device state
	long n_state; 		// convenience variable for storing next device state
	long n_commands;	// number of commands
   	DeviceCommandList commands_list; // array of commands (for backwards compatibility @ ESRF)
  
private: 
	Device(){};
};

#endif /* _TACO_DEVICE_H */
