
//+**********************************************************************
//
// File:		Device.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing OICDevice class in C++. The OICDevice class
//		wraps (old) OIC classes in C++ so that they can be used
//		in C++ classes derived from the Device base class.
//
// Author(s):	Andy Goetz
//
// Original:	November 1996
//
// $Revision: 1.1 $
//
// $Date: 2003-04-25 11:21:28 $
//
// $Author: jkrueger1 $
//
//+**********************************************************************

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
   long GetResources (char *res_name, long *error); 


//
// public members
//

public:

   long State(void *vargin, void *vargout , long *error);
   long Status(void *vargin, void *vargout, long *error);

// 
// class variables
//
  
   OICDevice (DevString devname, DevServerClass devclass, long *error);
   ~OICDevice ();
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

   long StateMachine( long cmd, long *error);
   
//
// OICDevice member fields
//
   
   DevServer ds; 			// pointer to the old OIC object
   DevServerClass ds_class;		// pointer to the old OIC class
   
};

#endif /* _OICDEVICE_H */
