/*+*******************************************************************

 File:		dev_states.c

 Project:	Device Servers

 Description:	List of device states in text

 Author(s):	Andy Goetz
		$Author: jkrueger1 $

 Original:	March 1992

 Version:       $Revision: 1.2 $

 Date:          $Date: 2003-04-25 11:21:34 $

 Copyright (c) 1992 by European Synchrotron Radiation Facility, 
                       Grenoble, France

********************************************************************-*/
#include <macros.h>
#include <DevStates.h>

/*
 * a globally defined dictionary for known device states
 */

_DLLFunc const char 	*DEVSTATES[] = 	   { 
	"Unknown",

	"Off",
	"On",

	"Device is Close",
	"Device is Open",

	"Low",
	"High",

	"Inserted",
	"Extracted",
	"Moving",

	"Warmup",
	"Init",

	"Standby",
	"Service",
	"Run",

	"Local",
	"Remote",
	"Automatic",

	"Ramp",
	"Tripped",
	"High Voltage Enable",
	"Beam Enable",

	"Blocked",
	"Fault",

	"Starting",
	"Stopping",
	"Starting requested but not initialised",
	"Stopping requested but not initialised",

	"Positive End Stop",
	"Negative End Stop",

	"Bake requested but not started",
	"Bakeing",
	"Bake is stopping",

	"Device forced open",
	"Device forced close",

	"off, restarting not authorised",
	"on, temp. not regulated",
	"resetting",
	"Device is forbidden",
	"Device is opening",
	"Device is closing",
	"Device is in an undetermined state",
	"Device is counting",
	"Device has been stopped",
	"Device is running",
	"Alarm detected",
	"Externally disabled",

	"Standby not reached",
	"On not reached",
};

