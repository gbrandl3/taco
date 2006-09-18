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
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:39:09 $
 */

#ifndef _AGPOWERSUPPLY_H
#define _AGPOWERSUPPLY_H
		
class AGPowerSupply : public PowerSupply {


//
// private members
// 

private :

   long ClassInitialise (long *error );
   long GetResources (char *res_name, long *error);

   static short class_inited;

//
// protected members
//

protected:

   long StateMachine (long cmd, long *error);
//
// commands
//
   long Off (void *argin, void *argout, long *error);
   long On (void *argin, void*argout, long *error);
   long Status (void *argin, void*argout, long *error);
   long SetValue (void *argin, void*argout, long *error);
   long ReadValue (void *argin, void*argout, long *error);
   long Reset (void *argin, void*argout, long *error);
   long Error (void *argin, void*argout, long *error);
   long Local (void *argin, void*argout, long *error);
   long Remote (void *argin, void*argout, long *error);
   long Update (void *argin, void*argout, long *error);
   long Hello (void *argin, void*argout, long *error);
//   long State (void *argin, void*argout, long *error);

//
// public members
//
public:

   AGPowerSupply (char *name, long *error);
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
