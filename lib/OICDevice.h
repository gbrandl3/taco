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
 * File:	OICDevice.h
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing OICDevice class in C++. The OICDevice class
 *		wraps (old) OIC classes in C++ so that they can be used
 *		in C++ classes derived from the Device base class.
 *
 * Author(s):	Andy Goetz
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1996
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2008-04-06 09:06:59 $
 *
 *+**********************************************************************/

#ifndef _OICDEVICE_H
#define _OICDEVICE_H
		

// Some remarks about the OICDevice class definition
//
// 
//


class OICDevice : public Device {

//
// private members
//

private :

//
// private virtual functions which should be defined in each new sub-class
//

   static short class_inited;

   long ClassInitialise( long *error );
//
// not many OIC classes have this method
//
   long GetResources (char *res_name, DevLong *error); 


//
// public members
//

public:

   long State(DevArgument vargin, DevArgument vargout , DevLong *error);
   long Status(DevArgument vargin, DevArgument vargout, DevLong *error);

// 
// class variables
//
  
   OICDevice (DevString devname, DevServerClass devclass, DevLong *error);
   ~OICDevice ();
   long Command ( long cmd, 
                  void *argin, long argin_type,
                  void *argout, long argout_type, 
                  DevLong *error);
   DevMethodFunction MethodFinder( DevMethod method);
   
   inline short get_state(void) {return(this->ds->devserver.state);}
   inline DevServer get_ds(void) {return(this->ds);}
   inline DevServerClass get_ds_class(void) {return(this->ds_class);}

//
// protected members - accessible only from derived classes
// 

protected:

   long StateMachine( DevCommand cmd, DevLong *error);
   
//
// OICDevice member fields
//
   
   DevServer ds; 			// pointer to the old OIC object
   DevServerClass ds_class;		// pointer to the old OIC class
   
};

#endif /* _OICDEVICE_H */
