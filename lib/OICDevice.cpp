/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:	OICDevice.c
 *
 * Project:	OICDevice Servers in C++
 *
 * Description:	source code file which implements a "wrapper" for old Objects
 *		In C (OIC) device classes (belonging to the root class 
 *		DevServer) in C++ for use in C++ classes derived from the 
 *		Device base class. 
 *
 *		OICDevice is derived from the Device class
 *
 * Author(s):	Andy Goetz
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1996
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2008-04-06 09:06:59 $
 *
 *-**********************************************************************/
		
#include <stdlib.h>
#include <string.h>
#include <API.h>
#include <Admin.h>
#include <Device.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <OICDevice.h>
#include <DevErrors.h>
#include <DevStates.h>


//
// private global variables of the OICDevice class which have only static scope
//

short OICDevice::class_inited = 0;

/**
 * method to initialise the OICDevice class. This method
 * will normally NEVER be called because in the OIC model
 * the DevMethodClassInitialise function is called 
 * automatically. It has been included here simply
 * as a place-holder.
 *
 * @param error error code returned in the case of problems
 *
 * @return DS_OK
 */
long OICDevice::ClassInitialise( long *error )
{
	dev_printdebug(DBG_TRACE,"OICDevice::ClassInitialise() called\n");

	*error = DS_OK;

// The following line is just to force the compiler to insert the RcsId array 
// in the executable code.
//   int l = strlen(RcsId);

	if ((ds__method_finder(this->ds,DevMethodClassInitialise))(error) != DS_OK)
		return(DS_NOTOK);

	OICDevice::class_inited = 1;
	return(DS_OK);
}

/**
 * dummy method to get resources for an OICDevice
 * presently this method does nothing and is purely a place-holder.
 * 
 * @param name	Device name
 * @param error	error code returned in the case of problems
 *
 * @return DS_OK
 */
long OICDevice::GetResources( char *name, DevLong *error )
{
	dev_printdebug(DBG_TRACE,"OICDevice::GetResources() called\n");
	*error = 0;
	return(DS_OK);
}

/**
 * constructor to create an object of the base class OICDevice
 *
 * @param devname 	name (ascii identifier) of oicdevice to create
 * @param devclass
 * @param error 	error code returned in the case of problems
 */
OICDevice::OICDevice (DevString devname, DevServerClass devclass, DevLong *error)
          : Device (devname, error)
{
//	static  DeviceCommandListEntry dev_cmd_list[] = {
//                DeviceCommandListEntry(DevState, (DeviceMemberFunction)&OICDevice::State, D_VOID_TYPE, D_SHORT_TYPE, 0, "State"),
//                DeviceCommandListEntry(DevStatus, (DeviceMemberFunction)&OICDevice::Status, D_VOID_TYPE, D_STRING_TYPE, 0, "Status"),
//                                              };
//	static long no_commands = sizeof(dev_cmd_list) / sizeof(DeviceCommandListEntry);
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
		return;

	oicclass = this->ds->devserver.class_pointer;

//
// set the class name to the OIC class name ("OICDevice" is only a wrapper
// class and therefore does not exist as a class per se
//
	this->class_name = oicclass->devserver_class.class_name;
	snprintf(this->dev_type, sizeof(this->dev_type), TYPE_DEFAULT);
	this->class_inited = oicclass->devserver_class.class_inited;

//
// initialise the oicdevice name
//
	this->name = (char*)malloc(strlen(devname)+1);
	strcpy(this->name, devname);

//
// initialise the OICDevice object fields by copying the necessary
// values from the DevServer object and DevServerClass class.
//
	this->n_commands = oicclass->devserver_class.n_commands;
	this->commands_list = (Device::DeviceCommandListEntry*)malloc(this->n_commands*sizeof(DeviceCommandListEntry));
//
// copy the list of commands and their input and output argument
// types to the DeviceCommandListEntry. This code makes the assumption
// that one can cast and copy a DevCommandListEntry* to a Device::Device
// CommandListEntry*
//
	for (int i = 0; i < this->n_commands; i++)
	{
		DevCommand _cmd;
		DeviceMemberFunction _fn;
		DevArgType _argin_type, _argout_type;
		long _min_access;
		char *_cmd_name;
		DeviceCommandListEntry cmd_map;
		
        	_cmd = oicclass->devserver_class.commands_list[i].cmd;
		_fn = (DeviceMemberFunction)NULL;
		_argin_type = (oicclass->devserver_class.commands_list[i].argin_type);
		_argout_type = (oicclass->devserver_class.commands_list[i].argout_type);
		_min_access = (oicclass->devserver_class.commands_list[i].min_access);
		_cmd_name = (char*)(oicclass->devserver_class.commands_list[i].cmd_name);
        
        	this->commands_list[i].cmd = _cmd;
        	this->commands_list[i].fn = _fn;
        	this->commands_list[i].argin_type = _argin_type;
        	this->commands_list[i].argout_type = _argout_type;
        	this->commands_list[i].min_access = _min_access;
        	this->commands_list[i].cmd_name = _cmd_name;
/*
 * initialise the commands map at the same time
 *
 * andy 16nov2005
 */

	
		cmd_map.cmd = _cmd;
		cmd_map.fn = _fn;
		cmd_map.argin_type = _argin_type;
		cmd_map.argout_type = _argout_type;
		cmd_map.min_access = _min_access;
		cmd_map.cmd_name = _cmd_name;
		this->commands_map[_cmd] =  cmd_map;
		/*
			Device::DeviceCommandMapEntry(_cmd, _fn, _argin_type, _argout_type, _min_access, _cmd_name);
		printf("OICDevice::OICDevice() command(%d) cmd=%d argin_type=%d argout_type=%d min_access=%d\n",
			i,this->commands_list[i].cmd,this->commands_list[i].argin_type,
			this->commands_list[i].argout_type,this->commands_list[i].min_access);
		*/
	}
// same for the new event list 
	this->n_events = oicclass->devserver_class.n_events;
//
// now copy the list of events and their input and output argument
// types to the DeviceEventListEntry. This code makes the assumption
// that one can cast and copy a DevEventListEntry* to a Device::Device
// EventListEntry*
//
	if (n_events > 0)
	{
		for (i=0; i<this->n_events; i++)
		{
	       		this->events_list[oicclass->devserver_class.events_list[i].event] = 
				DeviceEventListEntry(oicclass->devserver_class.events_list[i].event,
							oicclass->devserver_class.events_list[i].argout_type,
//							oicclass->devserver_class.events_list[i].event_name
							NULL);
	   	} 
	}
}

/**
 * function to execute a command on an oicdevice by calling
 * the DevMethodCommandHandler of the OIC class
 *		
 * BIG QUESTION MARK - do we need this method at all ?
 * why not simply use the base class Command method ?
 *
 * ANSWER - YES. This method calls the C method finder
 * to execute the command which the base class Command
 * does not do!
 *
 * @param cmd 		command to execute
 * @param argin 	pointer to input argument
 * @param argin_type   	type of input argument
 * @param argout  	pointer to output argument
 * @param argout_type   type of output argument
 * @param error 	pointer to error returned in case of problems
 *
 * @return DS_NOT in case of error otherwise DS_OK
 */
long OICDevice::Command (long cmd, void* argin, long argin_type,
                      void* argout, long argout_type, DevLong *error)
{
	dev_printdebug(DBG_TRACE,"OICDevice::Command() called, cmd = %d\n",cmd);
//
// simply pass the command and its parameters on to the DevMethodCommandHandler
// in the OIC class
//
	return (ds__method_finder(this->ds, DevMethodCommandHandler))
        	(this->ds, cmd, argin, argin_type, argout, argout_type, error);
}

/**
 * function to return a pointer to the function implementing the method requested.
 *		
 * @param method method to search for
 *
 * @return 	pointer to the method requested
 *
 */
DevMethodFunction OICDevice::MethodFinder ( DevMethod method)
{
	return (ds__method_finder(this->ds, method));
}

/**
 * destructor to destroy an object of the base class OICDevice
 */
OICDevice::~OICDevice ()
{
	DevLong error;

	dev_printdebug(DBG_TRACE,"OICDevice::~OICDevice() called\n");
//
// add code to destroy a oicdevice here
//
	if (ds__destroy(this->ds, &error) != DS_OK)
		return;
	return;
}

/**
 * function to call the state machine fo the OICDevice class
 * by calling the DevMethodStateMachine of the OIC class
 *
 * @param cmd 		command which is to be executed
 * @param error 	error code returned in the case of problems
 *
 * @return DS_OK if the required command exits otherwise DS_NOTOK
 */
long OICDevice::StateMachine(DevCommand cmd, DevLong *error)
{
	dev_printdebug(DBG_TRACE,"OICDevice::StateMachine() called\n");
	*error = DS_OK;
	return (ds__method_finder(this->ds, DevMethodStateHandler))(this->ds, cmd, error);
}

/**
 * function to implement the command to return the present 
 * state of a oicdevice as a short variable
 *
 * @param vargin	Not used should be NULL	
 * @param vargout 	pointer to state returned
 * @param error 	error code returned in the case of problems
 *
 * @return DS_OK
 */
long OICDevice::State(DevArgument vargin, DevArgument vargout, DevLong *error)
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

/**
 * function to implement the command to return the present 
 * state and additional info on a oicdevice as an ascii string
 *
 * @param vargin	Not used should be NULL	
 * @param vargout 	pointer to string returned
 * @param error 	error code returned in the case of problems
 *
 * @return DS_OK
 */
long OICDevice::Status(DevArgument vargin, DevArgument vargout, DevLong *error)
{
	static char 	lstatus[20], 
			**status;

	dev_printdebug(DBG_TRACE,"OICDevice::Status() called\n");

	*error = DS_OK;
	status = (char **)vargout;
//
// code to implement the DevStatus command here
//
	snprintf(lstatus, sizeof(status), "%s",DEVSTATES[this->ds->devserver.state]);
	*status = lstatus;
	return(DS_OK);
}

