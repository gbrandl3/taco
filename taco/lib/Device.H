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
// $Revision: 1.1 $
//
// $Date: 2003-04-25 11:21:27 $
//
// $Author: jkrueger1 $
//
//+**********************************************************************

#ifndef _TACO_DEVICE_H
#define _TACO_DEVICE_H
		

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

#if 0
// common base class for ESRF/FRM classes
// this class only declares functions used in the api library
// every thing else is in the derived classes
class DeviceBase {
//
// public members
//
public:

   DeviceBase();
   virtual ~DeviceBase ();
//
// following method is "virtual" which means derived classes can
// override it with their own version
//
   virtual long Command ( long cmd, 
                  void *argin, long argin_type,
                  void *argout, long argout_type, 
                  long *error)=0;
   

// mapper functions for FRM Device class compatibility

  virtual const char * GetClassName()=0;
  virtual const char * GetDevType()=0;
  virtual const char * GetDevName()=0;
  virtual unsigned int GetCommandNumber()=0;
  virtual long CommandQuery(_dev_cmd_info *sequence)=0;
  virtual unsigned 	GetEventNumber(void)=0;
  virtual long	EventQuery(_dev_event_info *)=0;
  virtual long	GetMinAccessRight(const long)=0;
//
// protected members - accessible only be derived classes
// 

protected:

};
#endif

class Device 
#if 0
	:public DeviceBase{
#endif
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
   virtual long GetResources (char *res_name, long *error); // pure virtual
#if 0
   virtual long GetResources (char *res_name, long *error)=0; // pure virtual
#endif



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
                                    }
               DeviceCommandListEntry;

typedef struct _DeviceCommandListEntry *DeviceCommandList;

typedef struct _DeviceEventListEntry {
                                     DevEvent         	event;
                                     DevArgType         argout_type;
                                     char               *event_name;
                                    }
               DeviceEventListEntry;

typedef struct _DeviceEventListEntry *DeviceEventList;

   virtual long State(void *vargin, void *vargout , long *error);
   virtual long Status(void *vargin, void *vargout, long *error);

// 
// class variables
//
  
   char* class_name;
   char dev_type[24];

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
   DeviceCommandList commands_list;
   DeviceEventList events_list;
   

#if 0
// mapper functions for FRM Device class compatibility

  virtual const char * GetClassName() {return this->class_name;};
  virtual const char * GetDevType(){return this->dev_type;};
  virtual const char * GetDevName(){return this->name;};
  virtual unsigned int GetCommandNumber(){return this->n_commands;};
  virtual long CommandQuery(_dev_cmd_info *sequence);
  virtual unsigned 	GetEventNumber(void){return this->n_events;};
  virtual long	EventQuery(_dev_event_info *);
  virtual long	GetMinAccessRight(const long);
#endif
//
// protected members - accessible only be derived classes
// 

protected:


//
// the following virtual commands must exist in all new sub-classes
//
   virtual long StateMachine( long cmd, long *error);
   long n_events;

   
   long state; // device state
   long n_state; // convenience variable for storing next device state
   long n_commands;
   
   Device(){};

};

#if 0
#include "DeviceFRM.H"
#endif


#define TYPE_DEFAULT		"DevType_Default"
#define TYPE_INIT 		"DevType_"

#endif /* _TACO_DEVICE_H */
