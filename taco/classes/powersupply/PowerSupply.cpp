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
 * File:	PowerSupply.cpp
 *
 * Project:	Device Servers in C++
 *
 * Description:	source code file containing the implementation of the
 *		the powersupply base class for device classes in C++ 
 *		(PowerSupply)
 *
 * Author(s):	Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:	April 1995
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:37 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cstdlib>
#include <string>
#include <cmath>
#include <Admin.h>
#include <API.h>
#include <PowerSupply.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevStates.h>


//
// private global variables of the Device class which have only static scope
//

short PowerSupply::class_inited = 0;

//+======================================================================
// Function:    PowerSupply::GetResources()
//
// Description: Interrogate the static database for PowerSupply resources
//                for the specified device. This routine can also be used
//                to initialise the class default resources.
//
// Arg(s) In:   char *res_name - name in the database to interrogate
//
// Arg(s) Out:  long *error - pointer to error code if routine fails.
//-=======================================================================
long PowerSupply::GetResources (char *res_name, DevLong *error)
{
   static db_resource res_powersupply[] = { {"delta_i", D_FLOAT_TYPE},
		                            {"time_constant", D_LONG_TYPE}, };
   static unsigned int res_powersupply_size = sizeof(res_powersupply)/
                                              sizeof(db_resource);
   register int ires;

   *error = DS_OK;

//
// setup the db_resource structure so that we can interrogate the database
// for the two resources "delta_i" and "time_constant" which are needed
// by all powersupplies to implement the read<>set check
//

   ires = 0;
   res_powersupply[ires].resource_adr = &(this->delta_i); ires++;
   res_powersupply[ires].resource_adr = &(this->time_const); ires++;

   if (db_getresource(res_name, res_powersupply, res_powersupply_size, error) != DS_OK)
   {
      printf("PowerSupply::GetResources() db_getresource failed, error %d\n", *error);
   }

   return(DS_OK);
}

//+=====================================================================
//
// Function:	PowerSupply::ClassInitialise() 
//
// Description:	function to initialise the Device class
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long PowerSupply::ClassInitialise( DevLong *error )
{
   dev_printdebug(DBG_TRACE,"PowerSupply::ClassInitialise() called\n");

   *error = 0;

//   int l = strlen(RcsId);

   PowerSupply::class_inited = 1;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	PowerSupply::PowerSupply() 
//
// Description:	constructor to create an object of the base class PowerSupply
//
// Input:	char *name - name (ascii identifier) of device to create
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

PowerSupply::PowerSupply (char *devname, DevLong *error)
	    :Device (devname, error)
{
   dev_printdebug(DBG_TRACE,"PowerSupply::PowerSupply() called, devname = %s\n",devname);

   *error = DS_OK;

//
// check if ClassInitialise() has been called
//
   if (PowerSupply::class_inited != 1)
   {
      if (PowerSupply::ClassInitialise(error) != DS_OK)
      {
         return;
      }
   }

//
// Initialise the class_name filed of the device class
//

   this->class_name = const_cast<char *>("PowerSupplyClass");

//
// initialise the object with default values via the database
// this code was previously in a separate initialise method
// but has now been included in the PowerSupply create method
// this avoids the necessity to call the initialise function
// separately - andy 6jun95
//
   if (this->GetResources(devname, error) != DS_OK);
   {
      return;
   }
}

//+=====================================================================
//
// Function:	PowerSupply::~PowerSupply() 
//
// Description:	destructor to destroy an object of the base class PowerSupply
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

PowerSupply::~PowerSupply ()
{
   dev_printdebug(DBG_TRACE,"PowerSupply::~PowerSupply() called\n");

//
// add code to destroy a device here
//
}

//+=====================================================================
//
// Function:	PowerSupply::CheckReadValue() 
//
// Description:	function to implement a check for read<>set value for
//		all members of the PowerSupply class. It will check if 
//		the read value lies within the tolerance of the desired
//		set value - it if doesn't then it returns an alarm
//		in the boolean value alarm. This metod assumes that
//		the powersupply has been read recently and that all 
//		information necessary for doing the check is passed
//		via the object (cf. input parameters below).
//
// Input:	none - it is assumed that the subclass has update
//		       the object fields last_set_t, time_const,
//		       set_val and read_val before calling this function
//
// Output:	DevBoolean alarm - 
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long PowerSupply::CheckReadValue(DevBoolean *alarm, DevLong *error)
{
   dev_printdebug(DBG_TRACE,"PowerSupply::CheckReadValue() called\n");

   time_t now;
   float delta;

   *error = DS_OK;
//
// start off by assuming that there is no alarm condition
//
   *alarm = FALSE;

//
// only do the check if the delta_i value is initialised to a +ve
// value
//
   if (this->delta_i > 0.0)
   {
//
// first check to see wether the last set value was sent at least
// time_const seconds beforehand
//
      time(&now);

      if ((now - this->last_set_t) > this->time_const)
      {
         delta = this->set_val - this->read_val;
         if (fabs(delta) > this->delta_i)
         {
            *alarm = TRUE;
         }
      }
   }

   return(DS_OK);
}
