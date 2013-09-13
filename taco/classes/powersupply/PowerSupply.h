/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        PowerSupply.c
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing the device server PowerSupply superclass in C++
 *		(DeviceClass).
 *
 * Author(s):	Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:	April 1995
 *
 * Version:     $Revision: 1.8 $
 *
 * Date:        $Date: 2008-12-02 12:51:10 $
 */

#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <Device.h>
		
/**
 * PowerSupply superclass in C++
 */
class PowerSupply : public Device 
{
//
// private methods
// 
private :
	long ClassInitialise(DevLong *error);

	long GetResources(const char *res_name, DevLong *error);
//
// private members
//
	static short class_inited;
//
// protected members
//
protected:
	DevFloat 	set_val;
	DevFloat 	read_val;
	DevLong 	channel;
	DevLong 	n_ave;
	DevLong 	fault_val;
	DevFloat 	cal_val;
	DevFloat 	conv_val;
	DevString 	conv_unit;
	DevFloat 	set_offset;
	DevFloat 	read_offset;
	DevFloat 	set_u_limit;
	DevFloat 	set_l_limit;
	DevFloat 	idot_limit;
	DevLong 	polarity;
	DevFloat 	delta_i;
	DevLong 	time_const;
	DevLong 	last_set_t;

//
// protected methods
//
	long CheckReadValue(DevBoolean *check, DevLong *error);

	virtual long StateMachine(DevCommand cmd, DevLong *error) = 0; // pure virtual function
//
// public members
//
public:
     PowerSupply (const char *name, DevLong *error);
     ~PowerSupply ();
};

#endif // POWERSUPPLY_H
