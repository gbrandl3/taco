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
 * File:        Device.h
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing the device server AGPowerSupply class in C++
 *		(AGPowerSupplyClass).
 *
 * Author(s):	Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:	June 1995
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2008-12-02 12:51:10 $
 */

#ifndef _AGPOWERSUPPLY_H
#define _AGPOWERSUPPLY_H

#include "PowerSupply.h"
		
/**
 * The AG Power Supply is a simulation of a typical power supply at the ESRF. 
 * This means it has two main state DEVON and DEVOFF, DEVSTANDBY is unknown.
 * All the common power supply commands are implemented. The simulation runs 
 * under OS9 and Unix. It has been developped for application program developers 
 * who want to test their applications without accessing real devices.
 */
class AGPowerSupply : public PowerSupply 
{
//
// private methods
// 
private :
	long ClassInitialise(DevLong *error );
	long GetResources(const char *res_name, DevLong *error);
//
// private members
//
	static DevShort class_inited;
//
// protected methods
//
protected:
	long StateMachine(DevCommand cmd, DevLong *error);
//
// commands
//
	long Off (DevArgument argin, DevArgument argout, DevLong *error);
	long On (DevArgument argin, DevArgument argout, DevLong *error);
	long Status (DevArgument argin, DevArgument argout, DevLong *error);
	long SetValue (DevArgument argin, DevArgument argout, DevLong *error);
	long ReadValue (DevArgument argin, DevArgument argout, DevLong *error);
	long Reset (DevArgument argin, DevArgument argout, DevLong *error);
	long Error (DevArgument argin, DevArgument argout, DevLong *error);
	long Local (DevArgument argin, DevArgument argout, DevLong *error);
	long Remote (DevArgument argin, DevArgument argout, DevLong *error);
	long Update (DevArgument argin, DevArgument argout, DevLong *error);
	long Hello (DevArgument argin, DevArgument argout, DevLong *error);
	long State (DevArgument argin, void*argout, DevLong *error);

//
// public methods
//
public:
	AGPowerSupply (const char *name, DevLong *error);
	~AGPowerSupply ();
};

// symbolic constants

#define AG_MAX_CUR      100.0
#define AG_MIN_CUR      0.0
#define AG_PER_ERROR    0.001

// fault values

#define AG_OVERTEMP     0x01
#define AG_NO_WATER     0x02
#define AG_CROWBAR      0x04
#define AG_RIPPLE       0x08
#define AG_MAINS        0x10
#define AG_LOAD         0x20
#define AG_TRANSFORMER  0x40
#define AG_THYRISTOR    0x80

#endif // _AG_POWERSUPPLY_H
