
//+**********************************************************************
//
// File:	OICDeviceFRM.c
//
// Project:	OICDevice Servers in C++
//
// Description:	source code file which implements a "wrapper" for old Objects
//		In C (OIC) device classes (belonging to the root class 
//		DevServer) in C++ for use in C++ classes derived from the 
//		Device base class. 
//
//		OICDeviceFRM is derived from the Device class
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
//-**********************************************************************
		
#include <cstdlib>
#include <string>
#include <API.h>
#include <Admin.h>
#include <Device.H>
#include <DevServer.h>
#include <DevServerP.h>
#include <OICDeviceFRM.H>
#include <DevErrors.h>
#include <DevStates.h>


//
// private global variables of the OICDeviceFRM class which have only static scope
//

short OICDeviceFRM::classInited = 0;

//+=====================================================================
//
// Function:	OICDeviceFRM::ClassInitialise() 
//
// Description:	method to initialise the OICDeviceFRM class. This method
//		will normally NEVER be called because in the OIC model
//		the DevMethodClassInitialise function is called 
//		automatically. It has been included here simply
//		as a place-holder.
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDeviceFRM::ClassInitialise(void) throw (long)
{
   long	lError;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::ClassInitialise() called\n");

// The following line is just to force the compiler to insert the RcsId array 
// in the executable code.

//   int l = strlen(RcsId);

   if ((ds__method_finder(this->ds,DevMethodClassInitialise)) (&lError) 
	!= DS_OK)
   {
      	throw lError;
   }
   OICDeviceFRM::classInited = 1;
   return(DS_OK);
}


//+=====================================================================
//
// Function:	OICDeviceFRM::GetResources() 
//
// Description:	dummy method to get resources for an OICDeviceFRM
//		presently this method does nothing and is purely
//		a place-holder.
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDeviceFRM::GetResources(const std::string name) throw (long)
{
   dev_printdebug(DBG_TRACE,"OICDeviceFRM::GetResources() called\n");
   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDeviceFRM::OICDeviceFRM() 
//
// Description:	constructor to create an object of the base class OICDeviceFRM
//
// Input:	char *name - name (ascii identifier) of oicdevice to create
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

OICDeviceFRM::OICDeviceFRM (const std::string devname, DevServerClass devclass, long &error)
          :FRMDevice (devname, error)
{
   DevServerClass oicclass;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::OICDeviceFRM() called, devname = %s\n",devname.c_str());
   error = DS_OK;

//
// call the create method of the (old) OIC class via the convenience
// function ds__create(), this returns a DevServer object pointer
// which is stored as part of the OICDeviceFRM class for future accesses
//

   if (ds__create(const_cast<char *>(devname.c_str()), devclass, &(this->ds), &error) != DS_OK)
   {
      return;
   }
   oicclass = this->ds->devserver.class_pointer;

//
// set the class name to the OIC class name ("OICDeviceFRM" is only a wrapper
// class and therefore does not exist as a class per se
//
   this->className = oicclass->devserver_class.class_name;
   devType = TYPE_DEFAULT;
   this->classInited = oicclass->devserver_class.class_inited;

// now copy the list of commands and their input and output argument
// types to the DeviceCommandListEntry. This code makes the assumption
// that one can cast and copy a DevCommandListEntry* to a Device::Device
// CommandListEntry*
   for (int i = 0; i< oicclass->devserver_class.n_commands; i++)
   	commandList[oicclass->devserver_class.commands_list[i].cmd] = DeviceCommandListEntry((DeviceMemberFunction)NULL, 
				oicclass->devserver_class.commands_list[i].argin_type,
				oicclass->devserver_class.commands_list[i].argout_type,
				oicclass->devserver_class.commands_list[i].min_access);
}

//+=====================================================================
//
// Function:	OICDeviceFRM::Command() 
//
// Description:	function to execute a command on an oicdevice by calling
//		the DevMethodCommandHandler of the OIC class
//		
//		BIG QUESTION MARK - do we need this method at all ?
//		                    why not simply use the base class Command
//		                    method ?
//
// Input:	long cmd - command to execute
//		DevArgument argin - pointer to input argument
//		long argin_type   - type of input argument
//
// Output:	DevArgument argout - pointer to output argument
//		long argout_type   - type of output argument
//		long *error - pointer to error returned in case of problems
//
//-=====================================================================

long OICDeviceFRM::Command (long cmd, void* argin, long argin_type,
                      void* argout, long argout_type, long *error)
{
   int i;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::Command() called, cmd = %d\n",cmd);
//
// simply pass the command and its parameters on to the DevMethodCommandHandler
// in the OIC class
//
   if ((ds__method_finder(this->ds, DevMethodCommandHandler))
        (this->ds, cmd, argin, argin_type, argout, argout_type, error) != DS_OK)
   {
      return(DS_NOTOK);
   }
   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDeviceFRM::MethodFinder() 
//
// Description:	function to return a pointer to the function implementing
//		the method requested.
//		
// Input:	DevMethod method - method to search for
//
// Output:	DevMethodFunction - method requested
//
//-=====================================================================

DevMethodFunction OICDeviceFRM::MethodFinder ( DevMethod method)
{
	return (ds__method_finder(this->ds, method));
}

//+=====================================================================
//
// Function:	OICDeviceFRM::~OICDeviceFRM() 
//
// Description:	destructor to destroy an object of the base class OICDeviceFRM
//
// Input:	none
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

OICDeviceFRM::~OICDeviceFRM ()
{
   long error;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::~OICDeviceFRM() called\n");

//
// add code to destroy a oicdevice here
//
   if (ds__destroy(this->ds, &error) != DS_OK)
   {
      return;
   }

   return;
}

//
// virtual functions which should be defined in each new sub-classes
//

//+=====================================================================
//
// Function:	OICDeviceFRM::StateMachine() 
//
// Description:	function to call the state machine fo the OICDeviceFRM class
//		by calling the DevMethodStateMachine of the OIC class
//
// Input:	long cmd - command which is to be executed
//
// Output:	long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDeviceFRM::StateMachine(long cmd) throw (long)
{
   long lError;
   dev_printdebug(DBG_TRACE,"OICDeviceFRM::StateMachine() called\n");

   if ((ds__method_finder(this->ds, DevMethodStateHandler))(this->ds, cmd, &lError)
       != DS_OK)
   {
      throw lError;
   }
   return(DS_OK);
}


//
// the following virtual commands must exist in all new sub-classes
//

//+=====================================================================
//
// Function:	OICDeviceFRM::State() 
//
// Description:	function to implement the command to return the present 
//		state of a oicdevice as a short variable
//
// Input:	none
//
// Output:	short *state - pointer to state returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDeviceFRM::tacoState(void *vargin, void *vargout, long *error)
{
   static short *state;
   *error = DS_OK;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::State() called\n");

   state = (short*)vargout;
//
// code to implement the DevState command here
//
   *state = (short)this->ds->devserver.state;

   return(DS_OK);
}

//+=====================================================================
//
// Function:	OICDeviceFRM::Status() 
//
// Description:	function to implement the command to return the present 
//		state and additional info on a oicdevice as an ascii string
//
// Input:	none
//
// Output:	char *status - pointer to string returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long OICDeviceFRM::tacoStatus(void *vargin, void *vargout, long *error)
{
   static char lstatus[20], **status;

   dev_printdebug(DBG_TRACE,"OICDeviceFRM::Status() called\n");

   *error = DS_OK;
   status = (char **)vargout;
//
// code to implement the DevStatus command here
//
   snprintf(lstatus, sizeof(lstatus), "%s",DEVSTATES[this->ds->devserver.state]);

   *status = lstatus;

   return(DS_OK);
}

