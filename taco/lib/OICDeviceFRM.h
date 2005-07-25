/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2000-2005 by Björn Pedersen <bpedersen@users.sf.net>
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
 * File:	FRMOICDevice.h
 *
 * Project:	Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing OICDeviceFRM class in C++. The OICDeviceFRM class
 *		wraps (old) OIC classes in C++ so that they can be used
 *		in C++ classes derived from the Device base class.
 *
 * Author(s):	Björn Pedersen
 * 		$Author: jkrueger1 $
 *
 * Original:	November 2000
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 13:08:28 $
 *
 *+**********************************************************************/

#ifndef FRM_OICDEVICE_H
#define FRM_OICDEVICE_H
		

#include "DeviceFRM.H"
// Some remarks about the OICDeviceFRM class definition
//
// 
//


class OICDeviceFRM : public FRMDevice {

//
// private members
//

private :
   static short classInited;

//
// public members
//

public:
// 
// class variables
//
  
   OICDeviceFRM (const std::string devname, DevServerClass devclass, long &error);
   ~OICDeviceFRM ();
   long Command ( long cmd, 
                  void *argin, long argin_type,
                  void *argout, long argout_type, 
                  long *error);
   DevMethodFunction MethodFinder( DevMethod method);
   
   inline short get_state(void) {return(this->ds->devserver.state);}
   inline DevServer get_ds(void) {return(this->ds);}
   inline DevServerClass get_ds_class(void) {return(this->ds_class);}

//
// protected members - accessible only from derived classes
// 

protected:

   virtual long StateMachine( long cmd) throw (long);
   virtual long ClassInitialise(void) throw (long);
   virtual long GetResources(const std::string res_name) throw (long); 
   
   	   long tacoState(void *vargin, void *vargout, long *);
   	   long tacoStatus(void *vargin, void *vargout, long *);
//
// OICDeviceFRM member fields
//
   
   DevServer ds; 			// pointer to the old OIC object
   DevServerClass ds_class;		// pointer to the old OIC class
   
};

#endif /* FRM_OICDEVICE_H */
