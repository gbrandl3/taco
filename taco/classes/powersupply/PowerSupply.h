//+**********************************************************************
//
// File:	Device.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing the device server PowerSupply superclass in C++
//		(DeviceClass).
//
// Author(s):	Andy Goetz
//
// Original:	April 1995
//
// $Revision: 1.1 $
//
// $Date: 2003-04-25 12:22:28 $
//
// $Author: jkrueger1 $
//
//+**********************************************************************
		
class PowerSupply : public Device {


//
// private members
// 

private :

   long ClassInitialise( long *error );
   long GetResources (char *res_name, long *error);

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

   long CheckReadValue(DevBoolean *check, long *error);

   virtual long StateMachine( long cmd, long *error) = 0; // pure virtual function
//
// public members
//
public:

     PowerSupply (char *name, long *error);
     ~PowerSupply ();

};
