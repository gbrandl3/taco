
//+**********************************************************************
//
// File:		Device.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing OICDeviceFRM class in C++. The OICDeviceFRM class
//		wraps (old) OIC classes in C++ so that they can be used
//		in C++ classes derived from the Device base class.
//
// Author(s):	Andy Goetz
//
// Original:	November 1996
//
// $Revision: 1.1 $
//
// $Date: 2004-11-04 14:02:40 $
//
// $Author: andy_gotz $
//
//+**********************************************************************

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
