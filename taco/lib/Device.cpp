/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * File:	Device.c
 *
 * Project:	Device Servers in C++
 *
 * Description:	source code file containing the implementation of the
 *		the device server base class in C++ (Device).
 *
 * Author(s):	Andy Goetz
 * 		$Author: jkrueger1 $
 *
 * Original:	March 1995
 *
 * Version:	$Revision: 1.18 $
 *
 * Date:	$Date: 2008-12-02 09:58:30 $
 *
 *-**********************************************************************/
		
#include <stdlib.h>
#include <string>
#include <API.h>
#include <Admin.h>
#include <Device.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevStates.h>

#include <iostream>

//
// private global variables of the Device class which have only static scope
//

short Device::class_inited = 0;

/**
 * function to initialise the Device class
 *
 * @param error error code returned in the case of problems
 *
 * @return DS_OK 
 */
long Device::ClassInitialise( DevLong *error )
{
	dev_printdebug(DBG_TRACE,"Device::ClassInitialise() called\n");
	*error = DS_OK;

// The following line is just to force the compiler to insert the RcsId array 
// in the executable code.
//   int l = strlen(RcsId);

	Device::class_inited = 1;
	return(DS_OK);
}

/**
 * constructor to create an object of the base class Device
 *
 * @param devname name (ascii identifier) of device to create
 * @param error error code returned in the case of problems
 *
 */
Device::Device (const char *devname, DevLong *error)
{
	dev_printdebug(DBG_TRACE,"Device::Device() called, devname = %s\n",devname);
	*error = DS_OK;
	this->commands_map.clear();
	this->events_list.clear();
//
// check if ClassInitialise() has been called
//
	if (Device::class_inited != 1 && Device::ClassInitialise(error) != DS_OK)
		return;
//
// initialise class_name (this should be done here because class_name
// is NOT a static member of the device class for the case of device
// server with several embedded classes. Also initialises, device
// type
//
	this->class_name = (char *)"DeviceClass";
	strncpy(this->dev_type, TYPE_DEFAULT, sizeof(this->dev_type) - 1);
//
// initialise the device name
//
	this->n_events = this->events_list.size();
	this->name = (char*)malloc(strlen(devname) + 1);
	if (this->name == NULL)
	{
		*error = DS_NOTOK;
		return;
	}
	strcpy(this->name, devname);
//
// initialise the commands list
//
	this->n_commands = this->commands_map.size();
	this->state = DEVON;
}

/**
 * empty function
 *
 * @param name 
 * @param error error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::GetResources(const char *name, DevLong *error )
{
	dev_printdebug(DBG_TRACE,"Device::GetResources() called\n");
	*error = DS_OK;
	return(DS_OK);
}


/**
 * function to execute a command on a device
 *
 * @param cmd 		command to execute
 * @param argin 	pointer to input argument
 * @param argin_type	type of input argument
 * @param argout	pointer to output argument
 * @param argout_type   type of output argument
 * @param error 	pointer to error returned in case of problems
 *
 * @return DS_NOTOK in case of error otherwise DS_OK
 */
long Device::Command (DevCommand cmd, DevArgument argin, DevArgType argin_type,
                      DevArgument argout, DevArgType argout_type, DevLong *error)
{
	DeviceMemberFunction member_fn;

	dev_printdebug(DBG_TRACE,"Device::Command() called, cmd = %d\n",cmd);
//
// call to GetCommandNumber() will initialise the commands_map if empty
	GetCommandNumber();
//
// add code to execute a command here
	DeviceCommandMap::iterator it = this->commands_map.find(cmd);
	if (it != this->commands_map.end())
	{
		if (argin_type != it->second.argin_type 
		   || argout_type != it->second.argout_type)
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
		member_fn = it->second.fn;
		return (this->*member_fn)(argin,argout,error);
	}

//
// since V8.32 of DSAPI the command_list entry has an extra field 
// (cmd_name), if one of the classes in a device server have not 
// been recompiled then this can cause the command entries to be 
// incorrectly interpreted, therefore if a command is not found 
// then print a warning message.
//
	std::cerr << "Device::Command(): command " << cmd 
	 	<< " not found, maybe you need to recompile the class "
		<< this->class_name << std::endl;
	*error = DevErr_CommandNotImplemented;
	return(DS_NOTOK);
}

/**
 * destructor to destroy an object of the base class Device
 */
Device::~Device ()
{
	dev_printdebug(DBG_TRACE,"Device::~Device() called\n");
//
// add code to destroy a device here
//
// M. Diehl  2.11.99
// free the memory malloc'ed by Device::Device()
//
	if( name != NULL )
	{
		free(name);
		name = NULL;
	}
}


/**
 * Implements the state machine fo the Device class.
 * It should be redefined in each new sub-classes.
 *
 * @param cmd 	command which is to be executed
 * @param error error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::StateMachine(DevCommand cmd, DevLong *error)
{
	dev_printdebug(DBG_TRACE,"Device::StateMachine() called\n");
	
	*error = DS_OK;
//
// default state machine is to allow all commands
//
	switch (state)
	{
		case DEVOFF :
			if (cmd == DevOff)
				return DS_NOTOK;
			break;
		case DEVON :
			if (cmd == DevOn || cmd == DevClose)
				return DS_NOTOK;
			break; 
		default : 
			return DS_OK;
	}
	return(DS_OK);
}



/**
 * Implements the command to return the present state of a device as a short variable.
 * It should be redefined in each new sub-classes.
 *
 * @param vargin 	not used should be a NULL pointer
 * @param vargout	a pointer to the short variable which should store the state
 * @param error  	error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::State(DevArgument, DevArgument vargout, DevLong *error)
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

/**
 * Implements the command to return the present 
 * state and additional info on a device as an ascii string.
 * It should be redefined in each new sub-classes.
 *
 * @param vargin 	not used should be a NULL pointer
 * @param vargout	a pointer to the string variable which should store the status
 * @param error  	error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::Status(DevArgument, DevArgument vargout, DevLong *error)
{
	static char 	lstatus[20], 
			**status;

	dev_printdebug(DBG_TRACE,"Device::Status() called\n");

	*error = DS_OK;
	status = (char **)vargout;
//
// code to implement the DevStatus command here
//
	snprintf(lstatus, sizeof(status), "%s",DEVSTATES[this->state]);
	*status = lstatus;
	return(DS_OK);
}

/**
 *
 */
long Device::On(DevArgument, DevArgument, DevLong *error)
{
	*error = 0;
	this->state = DEVON;
	return DS_OK;
}

/**
 *
 */
long Device::Off(DevArgument, DevArgument, DevLong *error)
{
	*error = 0;
	this->state = DEVOFF;
	return DS_OK;
}

/**
 *
 */
long Device::Reset(DevArgument, DevArgument, DevLong *error)
{
	*error = 0;
	this->state = DEVOFF;
	return DS_OK;
}

/**
 *
 */
long Device::Close(DevArgument, DevArgument, DevLong *error)
{
	delete this;
	return(DS_OK);
}

/**
 * Method to return the number of commands implemented in the device class 
 *
 * @param cmd_nb pointer to command number
 */
void Device::Get_command_number(unsigned int *cmd_nb)
{
	*cmd_nb = this->n_commands;
}


/**
 * Method to return command code, input argument and output
 *		argument for every command defined in the class
 *
 * @param cmd_info 	pointer to an array of structures.
 *              	There must be one structure for each command.
 * @param error 	error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::Command_Query(_dev_cmd_info *cmd_info,DevLong *error)
{
	*error = DS_OK;
	long	i = 0;
	for (std::map<DevCommand, DeviceCommandListEntry>::iterator it = this->commands_map.begin(); it != this->commands_map.end(); ++it, ++i)
	{
		cmd_info[i].cmd = it->second.cmd;
		cmd_info[i].in_type = it->second.argin_type;
		cmd_info[i].out_type = it->second.argout_type;
	}
	return(DS_OK);
}

/**
 * FRM compatibility version of Command_Query
 *
 * @param cmd_info 	pointer to an array of structures.
 *              	There must be one structure for each command.
 * 
 * @return DS_OK
 *
 * @see Command_Query
 */
long Device::CommandQuery(_dev_cmd_info *cmd_info)
{
	DevLong error;
	return this->Command_Query(cmd_info, &error);
}

/**
 * Method to return minimun access right for a command
 *
 * @param cmd		The command code
 * @param min_access 	Pointer where the min access right must be stored
 * @param error 	error code returned in the case of unknown command 
 *
 * @return DS_OK if command found otherwise DS_NOTOK
 */
long Device::Get_min_access_right(DevCommand cmd,long *min_access,DevLong *error)
{
	*error = DS_OK;

	int _commands_map_size = this->commands_map.size();
	if (_commands_map_size == 0)
		_commands_map_size = GetCommandNumber();
	std::map<DevCommand, DeviceCommandListEntry>::iterator it = this->commands_map.find(cmd);
	if (it != this->commands_map.end())
	{
//		printf("Device::Get_min_access_right(): commands_map[%d].cmd = %d\n",cmd,this->commands_map[cmd].cmd);
		*min_access = it->second.min_access;
		return(DS_OK);
	}
	*error = DevErr_CommandNotImplemented;
	return(DS_NOTOK);
}

/**
 * FRM compatibiliy version of Get_min_access_right (still unused)
 *
 * @param cmd The command code
 *
 * @return the access right of the command if command found
 * 
 * @see Get_min_access_right
 *
 * @throw long error code of the Get_min_access_right function if command not found
 */

long Device::GetMinAccessRight(const DevCommand cmd)
{
	DevLong error;
	long	min_access;
	if (this->Get_min_access_right(cmd, &min_access, &error) == DS_OK)
		return min_access;
	throw error;
	return(DS_NOTOK);
}

/**
 * Method to return the number of events implemented in the device class 
 *
 * @param cmd_nb pointer to event number
 */
void Device::Get_event_number(unsigned int *cmd_nb)
{
	*cmd_nb = this->n_events;
}


/**
 * Method to return event code, input argument and output
 * argument for every event defined in the class
 *
 * @param event_info 	pointer to an array of structures.
 *               	There must be one structure for each event.
 * @param error 	error code returned in the case of problems
 *
 * @return DS_OK
 */
long Device::Event_Query(_dev_event_info *event_info,DevLong *error)
{
	*error = DS_OK;
	long	i = 0;

        for (std::map<DevEvent, DeviceEventListEntry>::iterator it = this->events_list.begin(); it != this->events_list.end(); ++it, ++i)
	{
		event_info[i].event = it->second.event;
		event_info[i].out_type = it->second.argType;
	}
	return(DS_OK);
}

/**
 * FRM compatibiliy version of Event_Query (still unused).
 *
 * @param event_info 	pointer to an array of structures.
 *               	There must be one structure for each event.
 *
 * @return DS_OK
 *
 * @see Event_Query
 */
long Device::EventQuery(_dev_event_info *event_info)
{
	DevLong error;
	return this->Event_Query(event_info, &error);
}

/**
 * Method to return number of commands. It will initialise the commands_map
 * if it is empty.
 *
 * @return Number of commands
 */
unsigned int Device::GetCommandNumber()
{
//
// in the new scheme of things the command list is a map instead of
// an array. if the map has no commands then assume the device class
// has initialised the commands list and not the map (ESRF case). In
// this case copy the array contents to the map. This should only be 
// done once because afterwards the map will be initialised 
//
// - andy 24nov2004
//
	if (commands_map.size() == 0) 
	{
		dev_printdebug(DBG_TRACE,"Device::GetCommandNumber() initialise commands_map from commands_list array, n_commands = %d\n",this->n_commands);
   		for (int i = 0; i < this->n_commands; i++)
			this->commands_map[this->commands_list[i].cmd] = this->commands_list[i]; 
	}
	int _commands_map_size = commands_map.size();
	return _commands_map_size;
}

