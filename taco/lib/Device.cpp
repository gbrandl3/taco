
//+**********************************************************************
//
// File:	Device.c
//
// Project:	Device Servers in C++
//
// Description:	source code file containing the implementation of the
//		the device server base class in C++ (Device).
//
// Author(s):	Andy Goetz
//
// Original:	March 1995
//
// $Revision: 1.1 $
//
// $Date: 2003-04-25 11:21:27 $
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
#include <DevErrors.h>
#include <DevStates.h>


//
// private global variables of the Device class which have only static scope
//

short Device::class_inited = 0;

//+=====================================================================
//
// Function:	Device::ClassInitialise() 
//
// Description:	function to initialise the Device class
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::ClassInitialise( long *error )
{
   dev_printdebug(DBG_TRACE,"Device::ClassInitialise() called\n");

   *error = DS_OK;

// The following line is just to force the compiler to insert the RcsId array 
// in the executable code.

   int l = strlen(RcsId);

   Device::class_inited = 1;

   return(DS_OK);
}


//+=====================================================================
//
// Function:	Device::Device() 
//
// Description:	constructor to create an object of the base class Device
//
// Input:	char *name - name (ascii identifier) of device to create
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

Device::Device (char *devname, long *error)
{
   static  DeviceCommandListEntry dev_cmd_list[] = {
                {DevState, &Device::State, D_VOID_TYPE, D_SHORT_TYPE},
                {DevStatus, &Device::Status, D_VOID_TYPE, D_STRING_TYPE},
                                              };
   static long no_commands = sizeof(dev_cmd_list)/
			     sizeof(DeviceCommandListEntry);

   dev_printdebug(DBG_TRACE,"Device::Device() called, devname = %s\n",devname);

   *error = DS_OK;

//
// check if ClassInitialise() has been called
//

   if (Device::class_inited != 1)
   {
      if (Device::ClassInitialise(error) != DS_OK)
      {
         return;
      }
   }

//
// initialise class_name (this should be done here because class_name
// is NOT a static member of the device class for the case of device
// server with several embedded classes. Also initialises, device
// type
//

   this->class_name = (char *)"DeviceClass";
   sprintf(this->dev_type,TYPE_DEFAULT);

//
// initialise the device name
//

   this->n_events = 0;
   this->events_list =NULL;
   this->name = (char*)malloc(strlen(devname)+1);
   sprintf(this->name,"%s",devname);

//
// initialise the commands list
//

   this->n_commands = no_commands;
   this->commands_list = dev_cmd_list;

   this->state = DEVON;
}

//+=====================================================================
//
// Function:	Device::GetResources() 
//
// Description:	empty function
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::GetResources(char *name, long *error )
{
   dev_printdebug(DBG_TRACE,"Device::GetResources() called\n");

   *error = DS_OK;

   return(DS_OK);
}


//+=====================================================================
//
// Function:	Device::Command() 
//
// Description:	function to execute a command ona device
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

long Device::Command (long cmd, void* argin, long argin_type,
                      void* argout, long argout_type, long *error)
{
   int i;
   long iret;
   DeviceMemberFunction member_fn;

   dev_printdebug(DBG_TRACE,"Device::Command() called, cmd = %d\n",cmd);

//
// add code to execute a command here
//
   for (i = 0; i < this->n_commands; i++)
   {
      if (cmd == this->commands_list[i].cmd)
      {
         if (argin_type != this->commands_list[i].argin_type ||
             argout_type != this->commands_list[i].argout_type)
         {
            *error = DevErr_IncompatibleCmdArgumentTypes;
            return(DS_NOTOK);
         }
//
// check state machine
//
         if (this->StateMachine(cmd,error) != DS_OK)
         {
            return(DS_NOTOK);
         }
//
// now execute the command
//

         member_fn = this->commands_list[i].fn;

         iret = (this->*member_fn)(argin,argout,error);

	 return(iret);
      }

   }

   *error = DevErr_CommandNotImplemented;

   return(DS_NOTOK);
}

//+=====================================================================
//
// Function:	Device::~Device() 
//
// Description:	destructor to destroy an object of the base class Device
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

Device::~Device ()
{
   dev_printdebug(DBG_TRACE,"Device::~Device() called\n");

//
// add code to destroy a device here
//

  /* M. Diehl  2.11.99
   * free the memory malloc'ed by Device::Device()
   */

  if( name != NULL )
  {
    free(name);
    name = NULL;
  }
}

//
// virtual functions which should be defined in each new sub-classes
//

//+=====================================================================
//
// Function:	Device::StateMachine() 
//
// Description:	function to implement the state machine fo the Device class
//
// Input:	long cmd - command which is to be executed
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::StateMachine(long cmd, long *error)
{
   dev_printdebug(DBG_TRACE,"Device::StateMachine() called\n");

   *error = DS_OK;
//
// default state machine is to allow all commands
//
   switch (cmd) 
   {
      default : break;
   }

   return(DS_OK);
}


//
// the following virtual commands must exist in all new sub-classes
//

//+=====================================================================
//
// Function:	Device::State() 
//
// Description:	function to implement the command to return the present 
//		state of a device as a short variable
//
// Input:	none
//
// Output:	short *state - pointer to state returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::State(void *vargin, void *vargout, long *error)
{
   static short *pstate;
   *error = DS_OK;

   dev_printdebug(DBG_TRACE,"Device::State() called\n");

   pstate = (short*)vargout;
//
// code to implement the DevState command here
//
   *pstate = (short)this->state;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	Device::Status() 
//
// Description:	function to implement the command to return the present 
//		state and additional info on a device as an ascii string
//
// Input:	none
//
// Output:	char *status - pointer to string returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::Status(void *vargin, void *vargout, long *error)
{
   static char lstatus[20], **status;

   dev_printdebug(DBG_TRACE,"Device::Status() called\n");

   *error = DS_OK;
   status = (char **)vargout;
//
// code to implement the DevStatus command here
//
   sprintf(lstatus,"%s",DEVSTATES[this->state]);

   *status = lstatus;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	Device::Get_command_number() 
//
// Description:	Method to return the number of commands implemented
//              in the device class 
//
// Input:	none
//
// Output:	unsigned int *cmd_nb - pointer to command number
//
//-=====================================================================

void Device::Get_command_number(unsigned int *cmd_nb)
{
	*cmd_nb = this->n_commands;
}


//+=====================================================================
//
// Function:	Device::Command_Query() 
//
// Description:	Method to return command code, input argument and output
//		argument for every command defined in the class
//
// Input:	none
//
// Output:	_dev_cmd_info *cmd_info - pointer to an array of structures.
//              There must be one structure for each command.
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::Command_Query(_dev_cmd_info *cmd_info,long *error)
{
	*error = 0;

	for (long i = 0;i < this->n_commands;i++)
	{
		cmd_info[i].cmd = this->commands_list[i].cmd;
		cmd_info[i].in_type = this->commands_list[i].argin_type;
		cmd_info[i].out_type = this->commands_list[i].argout_type;
	}

	return(DS_OK);
}

//+=====================================================================
//
// Function:	Device::Get_min_access_right() 
//
// Description:	Method to return minimun access right for a command
//
// Input:	long cmd - The command code
//
// Output:	long *min_access - Pointer where the min access right must
//				   be stored
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::Get_min_access_right(long cmd,long *min_access,long *error)
{
	*error = 0;

	for (long i = 0;i < this->n_commands;i++)
	{
		if (cmd == this->commands_list[i].cmd)
		{
			*min_access = this->commands_list[i].min_access;
			return(DS_OK);
		}
	}

	*error = DevErr_CommandNotImplemented;
	return(DS_NOTOK);
}

//+=====================================================================
//
// Function:	Device::Get_event_number() 
//
// Description:	Method to return the number of events implemented
//              in the device class 
//
// Input:	none
//
// Output:	unsigned int *cmd_nb - pointer to event number
//
//-=====================================================================

void Device::Get_event_number(unsigned int *cmd_nb)
{
	*cmd_nb = this->n_events;
}


//+=====================================================================
//
// Function:	Device::Event_Query() 
//
// Description:	Method to return event code, input argument and output
//		argument for every event defined in the class
//
// Input:	none
//
// Output:	_dev_cmd_info *cmd_info - pointer to an array of structures.
//              There must be one structure for each event.
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long Device::Event_Query(_dev_event_info *cmd_info,long *error)
{
	*error = 0;

	for (long i = 0;i < this->n_events;i++)
	{
		cmd_info[i].event = this->events_list[i].event;
		cmd_info[i].out_type = this->events_list[i].argout_type;
	}

	return(DS_OK);

}
