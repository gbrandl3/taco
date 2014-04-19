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
 * File:	dev_states.c
 *
 * Project:	Device Servers
 *
 * Description:	List of device states in text
 *
 * Author(s):	Andy Goetz
 *		$Author: jkrueger1 $
 *
 * Original:	March 1992
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2005-07-25 13:08:28 $
 *
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

