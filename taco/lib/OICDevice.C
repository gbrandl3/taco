//+**********************************************************************
//
// File:	OICDevice.c
//
// Project:	OICDevice Servers in C++
//
// Description:	source code file which implements a "wrapper" for old Objects
//		In C (OIC) device classes (belonging to the root class 
//		DevServer) in C++ for use in C++ classes derived from the 
//		Device base class. 
//
//		OICDevice is derived from the Device class
//
// Author(s):	Andy Goetz
//
// Original:	November 1996
//
// $Revision: 1.1 $
//
// $Date: 2003-04-25 11:21:28 $
//
// $Author: jkrueger1 $
//
//-**********************************************************************
		
#include <stdlib.h>
#include <string.h>
#include <API.h>
#include <Admin.h>
#include <Device.H>
#include <DevServer.h>
#include <DevServerP.h>
#include <OICDevice.H>
#include <DevErrors.h>
#include <DevStates.h>


//
// private global variables of the OICDevice class which have only static scope
//

short OICDevice::class_inited = 0;

//+=====================================================================
//
// Function:	OICDevice::ClassInitialise() 
//
// Description:	method to initialise the OICDevice class. This method
//		will normally NEVER be called because in the OIC model
//		the DevMethodClassInitialise function is called 
//		automatically. It has been included here simply
//		as a place-holder.
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDevice::ClassInitialise( long *error )
{
   dev_printdebug(DBG_TRACE,"OICDevice::ClassInitialise() called\n");

   *error = DS_OK;

// The following line is just to force the compiler to insert the RcsId array 
// in the executable code.

//   int l = strlen(RcsId);

   if ((ds__method_finder(this->ds,DevMethodClassInitialise)) (error) 
	!= DS_OK)
   {
      return(DS_NOTOK);
   }

   OICDevice::class_inited = 1;

   return(DS_OK);
}


//+=====================================================================
//
// Function:	OICDevice::GetResources() 
//
// Description:	dummy method to get resources for an OICDevice
//		presently this method does nothing and is purely
//		a place-holder.
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDevice::GetResources( char *name, long *error )
{
   dev_printdebug(DBG_TRACE,"OICDevice::GetResources() called\n");

   *error = 0;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDevice::OICDevice() 
//
// Description:	constructor to create an object of the base class OICDevice
//
// Input:	char *name - name (ascii identifier) of oicdevice to create
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

OICDevice::OICDevice (char *devname, DevServerClass devclass, long *error)
          :Device (devname, error)
{
   static  Device::DeviceCommandListEntry dev_cmd_list[] = {
                {DevState, (DeviceMemberFunction)&OICDevice::State, D_VOID_TYPE, D_SHORT_TYPE},
                {DevStatus, (DeviceMemberFunction)&OICDevice::Status, D_VOID_TYPE, D_STRING_TYPE},
                                              };
   static long no_commands = sizeof(dev_cmd_list)/
			     sizeof(DeviceCommandListEntry);
   DevServerClass oicclass;
   int i;

   dev_printdebug(DBG_TRACE,"OICDevice::OICDevice() called, devname = %s\n",devname);

   *error = DS_OK;

//
// call the create method of the (old) OIC class via the convenience
// function ds__create(), this returns a DevServer object pointer
// which is stored as part of the OICDevice class for future accesses
//

   if (ds__create(devname, devclass, &(this->ds), error) != DS_OK)
   {
      return;
   }

   oicclass = this->ds->devserver.class_pointer;

//
// set the class name to the OIC class name ("OICDevice" is only a wrapper
// class and therefore does not exist as a class per se
//
   this->class_name = oicclass->devserver_class.class_name;
   sprintf(this->dev_type,TYPE_DEFAULT);
   this->class_inited = oicclass->devserver_class.class_inited;

//
// initialise the oicdevice name
//

   this->name = (char*)malloc(strlen(devname)+1);
   sprintf(this->name,"%s",devname);

//
// now initialise the OICDevice object fields by copying the necessary
// values from the DevServer object and DevServerClass class.
//

   this->n_commands = oicclass->devserver_class.n_commands;
//
// now copy the list of commands and their input and output argument
// types to the DeviceCommandListEntry. This code makes the assumption
// that one can cast and copy a DevCommandListEntry* to a Device::Device
// CommandListEntry*
//
//   this->commands_list = (Device::DeviceCommandListEntry*)oicclass->devserver_class.commands_list;
//
   this->commands_list = (Device::DeviceCommandListEntry*)malloc(this->n_commands*sizeof(DeviceCommandListEntry));
   for (i=0; i<this->n_commands; i++)
   {
     this->commands_list[i].cmd = oicclass->devserver_class.commands_list[i].cmd;
//
// following cast is NOT possible in C++, therefore had to add a Command method
// to call the command function
//
//   this->commands_list[i].fn = (DeviceMemberFunction)oicclass->devserver_class.commands_list[i].fn;
//
     this->commands_list[i].argin_type = oicclass->devserver_class.commands_list[i].argin_type;
     this->commands_list[i].argout_type = oicclass->devserver_class.commands_list[i].argout_type;
     this->commands_list[i].min_access = oicclass->devserver_class.commands_list[i].min_access;
     this->commands_list[i].cmd_name = oicclass->devserver_class.commands_list[i].cmd_name;
/*
     printf("OICDevice::OICDevice() command(%d) cmd=%d argin_type=%d argout_type=%d min_access=%d\n",
             i,this->commands_list[i].cmd,this->commands_list[i].argin_type,
             this->commands_list[i].argout_type,this->commands_list[i].min_access);
 */
   }
/* same for the new event list */
   this->n_events = oicclass->devserver_class.n_events;
//
// now copy the list of events and their input and output argument
// types to the DeviceEventListEntry. This code makes the assumption
// that one can cast and copy a DevEventListEntry* to a Device::Device
// EventListEntry*
//
//   this->events_list = (Device::DeviceEventListEntry*)oicclass->devserver_class.events_list;
//
   if (n_events > 0)
   {
   	this->events_list = (Device::DeviceEventListEntry*)malloc(this->n_events*sizeof(DeviceEventListEntry));
   	for (i=0; i<this->n_events; i++)
   	{
       		this->events_list[i].event = oicclass->devserver_class.events_list[i].event;
       		this->events_list[i].argout_type = oicclass->devserver_class.events_list[i].argout_type;
   	} 
   }
}

//+=====================================================================
//
// Function:	OICDevice::Command() 
//
// Description:	function to execute a command on an oicdevice by calling
//		the DevMethodCommandHandler of the OIC class
//		
//		BIG QUESTION MARK - do we need this method at all ?
//		                    why not simply use the base class Command
//		                    method ?
//
// Input:	long cmd - command to execute
//		DevArgument argin - pointer to input argument
//		long argin_type   - type of input argument
//
// Output:	DevArgument argout - pointer to output argument
//		long argout_type   - type of output argument
//		long *error - pointer to error returned in case of problems
//
//-=====================================================================

long OICDevice::Command (long cmd, void* argin, long argin_type,
                      void* argout, long argout_type, long *error)
{

   dev_printdebug(DBG_TRACE,"OICDevice::Command() called, cmd = %d\n",cmd);

//
// simply pass the command and its parameters on to the DevMethodCommandHandler
// in the OIC class
//

   if ((ds__method_finder(this->ds, DevMethodCommandHandler))
        (this->ds, cmd, argin, argin_type, argout, argout_type, error) != DS_OK)
   {
      return(DS_NOTOK);
   }

   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDevice::MethodFinder() 
//
// Description:	function to return a pointer to the function implementing
//		the method requested.
//		
// Input:	DevMethod method - method to search for
//
// Output:	DevMethodFunction - method requested
//
//-=====================================================================

DevMethodFunction OICDevice::MethodFinder ( DevMethod method)
{
	return (ds__method_finder(this->ds, method));
}

//+=====================================================================
//
// Function:	OICDevice::~OICDevice() 
//
// Description:	destructor to destroy an object of the base class OICDevice
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

OICDevice::~OICDevice ()
{
   long error;

   dev_printdebug(DBG_TRACE,"OICDevice::~OICDevice() called\n");

//
// add code to destroy a oicdevice here
//
   if (ds__destroy(this->ds, &error) != DS_OK)
   {
      return;
   }

   return;
}

//
// virtual functions which should be defined in each new sub-classes
//

//+=====================================================================
//
// Function:	OICDevice::StateMachine() 
//
// Description:	function to call the state machine fo the OICDevice class
//		by calling the DevMethodStateMachine of the OIC class
//
// Input:	long cmd - command which is to be executed
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDevice::StateMachine(long cmd, long *error)
{
   dev_printdebug(DBG_TRACE,"OICDevice::StateMachine() called\n");

   *error = DS_OK;

   if ((ds__method_finder(this->ds, DevMethodStateHandler))(this->ds, cmd, error)
       != DS_OK)
   {
      return (DS_NOTOK);
   }

   return(DS_OK);
}


//
// the following virtual commands must exist in all new sub-classes
//

//+=====================================================================
//
// Function:	OICDevice::State() 
//
// Description:	function to implement the command to return the present 
//		state of a oicdevice as a short variable
//
// Input:	none
//
// Output:	short *state - pointer to state returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDevice::State(void *vargin, void *vargout, long *error)
{
   static short *pstate;
   *error = DS_OK;

   dev_printdebug(DBG_TRACE,"OICDevice::State() called\n");

   pstate = (short*)vargout;
//
// code to implement the DevState command here
//
   *pstate = (short)this->ds->devserver.state;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDevice::Status() 
//
// Description:	function to implement the command to return the present 
//		state and additional info on a oicdevice as an ascii string
//
// Input:	none
//
// Output:	char *status - pointer to string returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDevice::Status(void *vargin, void *vargout, long *error)
{
   static char lstatus[20], **status;

   dev_printdebug(DBG_TRACE,"OICDevice::Status() called\n");

   *error = DS_OK;
   status = (char **)vargout;
//
// code to implement the DevStatus command here
//
   sprintf(lstatus,"%s",DEVSTATES[this->ds->devserver.state]);

   *status = lstatus;

   return(DS_OK);
}

