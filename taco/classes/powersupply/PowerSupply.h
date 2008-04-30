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
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2008-04-30 14:04:11 $
 */

#include <Device.h>
		
/**
 * PowerSupply superclass in C++
 */
class PowerSupply : public Device {


//
// private members
// 

private :

   long ClassInitialise( DevLong *error );
   long GetResources (char *res_name, DevLong *error);

   static short class_inited;
//
// protected members
//

protected:


   float set_val;
   float read_val;
   long channel;
   long n_ave;
   long fault_val;
   float cal_val;
   float conv_val;
   char *conv_unit;
   float set_offset;
   float read_offset;
   float set_u_limit;
   float set_l_limit;
   float idot_limit;
   long polarity;
   float delta_i;
   long time_const;
   long last_set_t;

   long CheckReadValue(DevBoolean *check, DevLong *error);

   virtual long StateMachine( DevCommand cmd, DevLong *error) = 0; // pure virtual function
//
// public members
//
public:

     PowerSupply (char *name, DevLong *error);
     ~PowerSupply ();

};
