//+*********************************************************************
//
// File:	AGPowerSupply.C
//
// Project:	Device Servers in C++
//
// Description:	Code for implementing the AG Power Supply class in C++
//		The AG Power Supply is a simulation of a typical
//		power supply at the ESRF. This means it has two
//		main state DEVON and DEVOFF, DEVSTANDBY is unknown.
//		All the common power supply commands are implemented.
//		The simulation runs under OS9 and Unix. It has been
//		developped for application program developers who want to
//		test their applications without accessing real devices
//
// Author(s);	A. Goetz 
//
// Original:	June 1995
//
// Copyright (c) 1995 by European Synchrotron Radiation Facility, 
//                       Grenoble, France
//
//-*********************************************************************

#include <stdlib.h>
#include <string.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <PowerSupply.h>
#include <AGPowerSupply.h>


//
// a template copy of the default powersupply that normally gets created
// by the create method. it is initialised in CLassInitialise to default 
// values. these defaults can also be specified in the resource file or 
// via an admin command.
//

static AGPowerSupply *agps_template;
short AGPowerSupply::class_inited = 0;


//+======================================================================
// Function:    AGPowerSupply::GetResources()
//
// Description:	Interrogate the static database for AGPowerSupply resources 
//		for the specified device. This routine can also be used
//		to initialise the class default resources.
//
// Arg(s) In:	char *res_name - name in the database to interrogate
//
// Arg(s) Out:	long *error - pointer to error code if routine fails.
//-=======================================================================
long AGPowerSupply::GetResources (char *res_name, long *error)
{
   static db_resource res_table[] = {
               {"state",D_LONG_TYPE},
               {"set_val",D_FLOAT_TYPE},
               {"channel",D_SHORT_TYPE},
               {"n_ave",D_SHORT_TYPE},
               {"conv_unit",D_STRING_TYPE},
               {"set_offset",D_FLOAT_TYPE},
               {"read_offset",D_FLOAT_TYPE},
               {"set_u_limit",D_FLOAT_TYPE},
               {"set_l_limit",D_FLOAT_TYPE},
               {"polarity",D_SHORT_TYPE},
                             };
   static int res_tab_size = sizeof(res_table)/sizeof(db_resource);

   *error = 0;

   res_table[0].resource_adr = &(this->state);
   res_table[1].resource_adr = &(this->set_val);
   res_table[2].resource_adr = &(this->channel);
   res_table[3].resource_adr = &(this->n_ave);
   res_table[4].resource_adr = &(this->conv_unit);
   res_table[5].resource_adr = &(this->set_offset);
   res_table[6].resource_adr = &(this->read_offset);
   res_table[7].resource_adr = &(this->set_u_limit);
   res_table[8].resource_adr = &(this->set_l_limit);
   res_table[9].resource_adr = &(this->polarity);

   if(db_getresource(res_name,res_table,res_tab_size,error))
   {
      printf("AGPowerSupply::GetResources(): db_getresource() failed, error %d\n",error);
      return(DS_NOTOK);
   }
   else
   {
      printf("initial values after searching the static database for %s\n\n",res_name);
/*      printf("state         D_LONG_TYPE    %6d\n",this->state);
      printf("set_val       D_FLOAT_TYPE   %6.0f\n",this->set_val);
      printf("channel       D_SHORT_TYPE   %6d\n",this->channel);
      printf("n_ave         D_SHORT_TYPE   %6d\n",this->n_ave);
      printf("conv_unit     D_STRING_TYPE  %6s\n",this->conv_unit);
      printf("set_offset    D_FLOAT_TYPE   %6.0f\n",this->set_offset);
      printf("read_offset   D_FLOAT_TYPE   %6.0f\n",this->read_offset);
      printf("set_u_limit   D_FLOAT_TYPE   %6.0f\n",this->set_u_limit);
      printf("set_l_limit   D_FLOAT_TYPE   %6.0f\n",this->set_l_limit);
      printf("polarity      D_SHORT_TYPE   %6d\n",this->polarity);*/
   }

   return(DS_OK);
}
//+======================================================================
// Function:    AGPowerSupply::ClassInitialise()
//
// Description:	Initialise the AGPowerSupplyClass, is called once for
//		this class per process. ClassInitialise() will initialise
//		the class variables and the template powersupply device 
//		agps_template. 
//
// Arg(s) In:	none
//
// Arg(s) Out:	long *error - pointer to error code if routine fails.
//-=======================================================================

long AGPowerSupply::ClassInitialise (long *error)
{
   static AGPowerSupply *agps_template = (AGPowerSupply*)malloc(sizeof(AGPowerSupply)); 

   int iret=0;

   printf ("AGPowerSupply::ClassInitialise() called\n");
  
   debug_flag = 0xffffffff;

//
// initialise the template powersupply so that DevMethodCreate has
// default values for creating a powersupply, these values will be
// overridden by the static database (if defined there). 
//
// default is to start with powersupply switched OFF; the state 
// variable gets (ab)used during initialisation to interpret the
// initial state of the powersupply: 0==DEVOFF, 1==DEVON. this is
// because the database doesn't support the normal state variables
// like DEVON, DEVSTANDBY, DEVINSERTED, etc.

   agps_template->state = 0;
   agps_template->n_state = agps_template->state;
   agps_template->set_val = 0.0;
   agps_template->read_val = 0.0;
   agps_template->channel = 1;
   agps_template->n_ave = 1;
   agps_template->conv_unit = (char*)malloc(sizeof("AMP")+1);
   sprintf(agps_template->conv_unit,"AMP");
   agps_template->set_offset = 0.0,
   agps_template->read_offset = 0.0;
   agps_template->set_u_limit = AG_MAX_CUR;
   agps_template->set_l_limit = AG_MIN_CUR;
   agps_template->polarity = 1;

//
// interrogate the static database for default values 
//
  
// if(this->GetResources("CLASS/AGPS/DEFAULT",error))
   if(GetResources("CLASS/AGPS/DEFAULT",error))
   {
      printf("AGPowerSupply::ClassInitialise(): GetResources() failed, error %d\n",error);
      return(DS_NOTOK);
   }

   agps_template->state = state;
   agps_template->set_val = set_val;
   agps_template->channel = channel;
   agps_template->n_ave = n_ave;
   agps_template->read_offset = read_offset;
   agps_template->set_u_limit = set_u_limit;
   agps_template->set_l_limit = set_l_limit;
   agps_template->polarity = polarity;

   AGPowerSupply::class_inited = 1;

   printf("returning from AGPowerSupply::ClassInitialise()\n");

   return(iret);
}

//+======================================================================
// Function:    AGPowerSupply::AGPowerSupply()
//
// Description:	create a AGPowerSupply object. This involves allocating
//		memory for this object and initialising its name.
//
// Arg(s) In:	char *name - name of object.
//
// Arg(s) Out:	DevServer *ds_ptr - pointer to object created.
//		long *error - pointer to error code (in case of failure)
//-=====================================================================

AGPowerSupply::AGPowerSupply (char *name, long *error)
              :PowerSupply (name, error)
{
	printf("AGPowerSupply::AGPowerSupply called, name %s\n",name);
//
// check to see if AGPowerSupply::ClassInitialise has been called
//
	if (AGPowerSupply::class_inited != 1)
	{
		if (AGPowerSupply::ClassInitialise(error) != DS_OK)
			return;
	}

//
// AGPowerSupplyClass is a subclass of PowerSupplyClass
//
	this->class_name = (char*)malloc(strlen("AGPowerSupplyClass")+1);
	strcpy(this->class_name, "AGPowerSupplyClass");

//
// initialise the commands list to point to the commands list 
// implemented for the AG PowerSupply class
//
        this->commands_map[DevState] = DeviceCommandMapEntry(DevState, (DeviceBaseMemberFunction)(&Device::State), 
							D_VOID_TYPE, D_SHORT_TYPE, 0, "DevState");
        this->commands_map[DevStatus] = DeviceCommandMapEntry(DevStatus, (DeviceBaseMemberFunction)(&AGPowerSupply::Status), 
							D_VOID_TYPE, D_STRING_TYPE, 0, "DevStatus");
        this->commands_map[DevOn] = DeviceCommandMapEntry(DevOn, (DeviceBaseMemberFunction)(&AGPowerSupply::On), 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevOn");
        this->commands_map[DevOff] = DeviceCommandMapEntry(DevOff, (DeviceBaseMemberFunction)&AGPowerSupply::Off, 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevOff");
        this->commands_map[DevSetValue] = DeviceCommandMapEntry(DevSetValue, (DeviceBaseMemberFunction)&AGPowerSupply::SetValue, 
							D_FLOAT_TYPE, D_VOID_TYPE, 0, "DevSetValue");
        this->commands_map[DevReadValue] = DeviceCommandMapEntry(DevReadValue, (DeviceBaseMemberFunction)&AGPowerSupply::ReadValue, 
							D_VOID_TYPE, D_FLOAT_READPOINT, 0, "DevReadValue");
        this->commands_map[DevReset] = DeviceCommandMapEntry(DevReset, (DeviceBaseMemberFunction)&AGPowerSupply::Reset, 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevReset");
        this->commands_map[DevError] = DeviceCommandMapEntry(DevError, (DeviceBaseMemberFunction)&AGPowerSupply::Error, 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevError");
        this->commands_map[DevLocal] = DeviceCommandMapEntry(DevLocal, (DeviceBaseMemberFunction)&AGPowerSupply::Local, 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevLocal");
        this->commands_map[DevRemote] = DeviceCommandMapEntry(DevRemote, (DeviceBaseMemberFunction)&AGPowerSupply::Remote, 
							D_VOID_TYPE, D_VOID_TYPE, 0, "DevRemote");
        this->commands_map[DevUpdate] = DeviceCommandMapEntry(DevUpdate, (DeviceBaseMemberFunction)&AGPowerSupply::Update, 
							D_VOID_TYPE, D_STATE_FLOAT_READPOINT, 0, "DevUpdate");
        this->commands_map[DevHello] = DeviceCommandMapEntry(DevHello, (DeviceBaseMemberFunction)&AGPowerSupply::Hello, 
							D_STRING_TYPE, D_SHORT_TYPE, 0, "DevHello");
		this->n_commands = this->commands_map.size(); 
//
// initialise new AGPowerSupply with template, (this could 
// also be done by overloading the operator=)
//

/*
	this->state = agps_template->state;
	this->set_val = agps_template->set_val;
	this->channel = agps_template->channel;
	this->n_ave = agps_template->n_ave;
	this->conv_unit = (char*)malloc(strlen(agps_template->conv_unit)+1);
	sprintf(this->conv_unit,"%s",agps_template->conv_unit);
	this->set_offset = agps_template->set_offset;
	this->read_offset = agps_template->read_offset;
	this->set_u_limit = agps_template->set_u_limit;
	this->set_l_limit = agps_template->set_l_limit;
	this->polarity = agps_template->polarity;
*/

//
// initialise powersupply with values defined in database
//
	if (this->GetResources(this->name,error) != DS_OK)
	{
		fprintf(stderr, "AGPowerSupply::AGPowerSupply(): GetResources() failed, error %d\n",*error);
		return;
	}

//
// interpret the initial state of the powersupply
//
	if (this->state == DEVON)
	{
		printf("AGPowerSupply::AGPowerSupply() switching ON\n");
		On(NULL,NULL,error);
//
// if switched ON then set the current too
//
		SetValue((void*)&(this->set_val),NULL,error);
	}
	else
	{
		printf("AGPowerSupply::AGPowerSupply() switching OFF\n");
//
// default is to assume the powersupply is OFF
//
		Off(NULL,NULL,error);
	}
	printf("leaving AGPowerSupply::AGPowerSupply() and all OK\n");
	return;
}

//+======================================================================
// Function:    AGPowerSupply::StateMachine()
//
// Description:	Check if the command to be executed does not violate
//		the present state of the device.
//
// Arg(s) In:	DevCommand cmd - command to be executed.
//
// Arg(s) Out:	long *error - pointer to error code (in case of failure).
//-=====================================================================
long AGPowerSupply::StateMachine (long cmd, long *error)
{
	long iret = 0;
	long int p_state, n_state;

	p_state = this->state;

//
// before checking out the state machine assume that the state
// doesn't change i.e. new state == old state
//
	n_state = p_state;

	switch (p_state) 
	{
		case (DEVOFF) :
	      		switch (cmd) 
			{
				case (DevOn) : 
					n_state = DEVON;
					break;
				case (DevError) : 
					n_state = DEVFAULT;
					break;
				case (DevLocal) : 
					n_state = DEVLOCAL;
					break;
// following commands are ignored in this state
				case (DevSetValue) :
				case (DevReadValue) : 
					iret = DS_NOTOK;
					*error = DevErr_CommandIgnored;
					break;
// following commands don't change the state machine
				case (DevReset) : 
				case (DevRemote) : 
				case (DevOff) : 
				case (DevState) : 
				case (DevUpdate) :
				case (DevStatus) : 
					break;
				default : 
					break;
			}
			break;
		case (DEVON) :
			switch (cmd) 
			{
				case (DevOff) : 
					n_state = DEVOFF;
					break;
				case (DevError) : 
					n_state = DEVFAULT;
					break;
				case (DevLocal) : 
					n_state = DEVLOCAL;
					break;
// following commands violate the state machine
				case (DevRemote) : 
				case (DevReset) : 
					iret = DS_NOTOK;
					(*error) = DevErr_AttemptToViolateStateMachine;
					break;
// the following commands don't change the state of the machine
				case (DevState) : 
				case (DevStatus) : 
				case (DevReadValue) : 
				case (DevSetValue) :
				case (DevUpdate) :
				case (DevRun) : 
					break;
				default : 
					break;
			}
			break;
		case (DEVLOCAL) :
			switch (cmd) 
			{
				case (DevRemote) : 
					n_state = DEVOFF;
					break;
// the following commands violate the state machine 
				case (DevOn) :
				case (DevOff) :
				case (DevRun) :
				case (DevReset) :
				case (DevStandby) :
				case (DevError) : 
					iret = DS_NOTOK;
					(*error) = DevErr_AttemptToViolateStateMachine;
					break;
// following commands are ignored 
				case (DevSetValue) : 
					iret = DS_NOTOK; 
					*error = DevErr_CommandIgnored;
					break;
// the following commands don't change the state of the machine
				case (DevState) :
				case (DevStatus) :
				case (DevLocal) :
				case (DevUpdate) :
				case (DevReadValue) : 
					break;
				default : 
					break;
			}
			break;
		case (DEVFAULT) :
			switch (cmd) 
			{
				case (DevReset) : 
					n_state = DEVOFF;
					break;
// the following commands violate the state machine
				case (DevOff) :
				case (DevRemote) :
				case (DevOn) :
				case (DevLocal) : 
					iret = DS_NOTOK;
					(*error) = DevErr_AttemptToViolateStateMachine;
					break;
// following commands are ignored
				case (DevSetValue) : 
				case (DevReadValue) : 
					iret = DS_NOTOK;
					*error = DevErr_CommandIgnored;
					break;
// the following commands don't change the stae of the machine
				case (DevState) : 
				case (DevStatus) : 
				case (DevUpdate) :
				case (DevError) : 
					break;
				default : 
					break;
			}
			break;
		default : 
			break;
	}

//
// update powersupply's private variable n_state so that other methods
// can use it too.
//
	this->n_state = n_state;
	printf("AGPowerSupply::StateMachine(): p_state %2d n_state %2d, iret %2d\n", p_state,n_state, iret);
	return(iret);
}

//+=====================================================================
// Function:    AGPowerSupply::Off()
//
// Description:	switch the simulated power supply off. the read value 
//		and set value get set to zero - this is a convention
//		adopted at the ESRF for all powersupplies.
//
// Arg(s) In:	DevVoid *argin - void.
//
// Arg(s) Out:	DevVoid *argout - void.
//		long *error - pointer to error code in case of failure.
//-=====================================================================

long AGPowerSupply::Off (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::Off(%s) called\n",this->name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

	read_val = 0.0;
	set_val = 0.0;
	state = DEVOFF;

	return (DS_OK);
}
	
//+=====================================================================
// Function:    AGPowerSupply::On()
//
// Description:	switch simulated powersupply ON
//
// Arg(s) In:	DevVoid *argin - void.
//
// Arg(s) Out:	DevVoid *argout - void.
//		long *error - pointer to error code (in case of failure)
//-=====================================================================

long AGPowerSupply::On (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::On(%s) called\n",this->name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

	state = DEVON;

	return (DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::SetValue()
//
// Description:	Routine to set current of simulated power supply. This
//		version does not check the limits of the set_value.
//
// Arg(s) In:	DevFloat *vargin - current to set
//
// Arg(s) Out:	DevVoid *argout - void.
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long AGPowerSupply::SetValue (void *vargin, void *vargout, long *error)
{
	float *setting;
	void *tmp;

	printf("AGPowerSupply::SetValue(%s) called\n",name);

	*error = 0;
	tmp = vargout;

	setting = (float*)vargin;
	set_val = *setting;

	return (DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::ReadValue()
//
// Description:	Return read value of simulated power supply. The read
//		value is the last set value with some simulated noise
//		on it. The noise is taken from the time() and is therefore
//		repetitive.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevFloatReadPoint *vargout - set and read value.
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long AGPowerSupply::ReadValue (void *vargin, void *vargout, long *error)
{
	DevFloatReadPoint *frp;
        float per_error;
	void *tmp;

	printf("AGPowerSupply::ReadValue(%s) called\n",name);

	*error = 0;
	tmp = vargin;

	frp = (DevFloatReadPoint*)vargout;
	frp->set = set_val;

//
// the last 4 bits of time() provide some random noise on this
// simulated reading - andy 11apr90
//
	per_error = AG_PER_ERROR/100.*2.*
                    (((float)(time(NULL)&0xf)/15.0)-0.5);
//	            printf("ReadValue(): percent error %6.3f\n",
//                          per_error);
        frp->read = set_val*(1.-per_error);
        read_val = frp->read;

	return (DS_OK);
}

//+=====================================================================
// Function:   	AGPowerSupply::Remote()
//
// Description:	Switch simulated power supply from LOCAL mode to socalled
//		REMOTE mode i.e. a mode in which the power supply can receive
//		commands.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevVoid *vargout - void.
//		long *error - pointer to error code (in case of failure)
//-=====================================================================

long AGPowerSupply::Remote (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::Remote(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

//
// simply update the present state with the next state
//
	state = n_state;
	
	return (DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::Reset()
//
// Description:	Reset simulated power supply to a well known state.
//		Used to recover from errors mostly. All set and read 
//		points are reset at the same time.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevVoid *vargout - void.
//		long *error - pointer to error in the case of failure.
//-=====================================================================

long AGPowerSupply::Reset (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::Reset(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

	set_val = 0.0;
        read_val = 0.0;
        fault_val = 0;
        cal_val = 0.0;
        conv_val = 1.0;
        state = n_state;

	return (DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::Error()
//
// Description:	Simulate an error condition on the simulated power supply.
//		The system call time() is used to generate any one of 8
//		possible errors.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevVoid *vargout - void.
//		long *error - pointer to error code, in the case of failure.
//-=====================================================================

long AGPowerSupply::Error (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::Error(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

//
// use the last 16 bits of the system time to generate an error - andy
//

	fault_val = (unsigned short)(time(NULL)&0xff);
	state = n_state;

	return (DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::Status()
//
// Description:	Return the state as an ASCII string. Interprets the error
//		flag as well if the status is FAULT.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevString *vargout - contains string.
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long AGPowerSupply::Status ( void *vargin, void *vargout, long *error)
{
	static char mess[1024];
	char **status;
	long fault = fault_val;
	long p_state;
	void *tmp;

	printf("AGPowerSupply::Status(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	p_state = state;

	switch (p_state) {

	case (DEVOFF) : sprintf(mess,"%s","Off");
	                break;

	case (DEVON) : sprintf(mess,"%s","On");
	                    break;

	case (DEVLOCAL) : sprintf(mess,"%s","Local");
	                  break;

	case (DEVFAULT) : sprintf(mess,"%s","Fault\n");
	                  break;

	default : sprintf(mess,"%s","Unknown");
	          break;
	}

// translate fault into a string

	if ((fault != 0) && (p_state == DEVFAULT))
	{
	if ((fault & AG_OVERTEMP) != 0) 
	{
	  sprintf(mess+strlen(mess)," %s","Overtemp");
	}
	if ((fault & AG_NO_WATER) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","No Cooling");
	}
	if ((fault & AG_CROWBAR) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Crowbar");
	}
	if ((fault & AG_RIPPLE) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Ripple");
	}
	if ((fault & AG_MAINS) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Mains");
	}
	if ((fault & AG_LOAD) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Load");
	}
	if ((fault & AG_TRANSFORMER) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Transformer");
	}
	if ((fault & AG_THYRISTOR) != 0)
	{
	   sprintf(mess+strlen(mess)," %s","Thyristor");
	}
	}

	status = (char **)vargout;
	*status = mess;

	return(DS_OK);
}
//+=====================================================================
// Function:    AGPowerSupply::Local()
//
// Description:	Switch simulated power supply to LOCAL mode. In this mode
//		the power supply does not respond to any commands until
//		the next DevRemote command is executed.
//
// Arg(s) In:	DevVoid *vargin - void.
//
// Arg(s) Out:	DevVoid *vargout - void.
//		long *error - pointer to error code in the case of failure.
//-=====================================================================

long AGPowerSupply::Local (void *vargin, void *vargout, long *error)
{
	void *tmp;

	printf("AGPowerSupply::Local(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	tmp = vargout;

	state = n_state;

	return(DS_OK);
}

//+=====================================================================
// Function:    AGPowerSupply::Update()
//
// Description:	Return the state and the read and set points. This command
//		is a combination of the DevState and the DevReadValue commands.
//
// Arg(s) In:	DevVoid *argin - void.
//
// Arg(s) Out:	DevStateFloatReadPoint *vargout - contains the three values.
//		long *error - pointer to error code (in the case of failure)
//-=====================================================================

long AGPowerSupply::Update ( void *vargin, void *vargout, long *error)
{
	DevStateFloatReadPoint *vargout_sfrp;
	DevShort darg_short;
	DevFloatReadPoint darg_frp;
	void *tmp;

	printf("AGPowerSupply::Update(%s) called\n",name);

	*error = 0;
	tmp = vargin;
	vargout_sfrp = (DevStateFloatReadPoint*)vargout;
//
// update state
//
	State(NULL, &darg_short, error);
	vargout_sfrp->state = darg_short;

//
// get latest set and read values
//
	ReadValue(NULL, &darg_frp, error);
	vargout_sfrp->set = darg_frp.set;
	vargout_sfrp->read = darg_frp.read;

  	printf("dev_update(): state %d set %f read %f\n",
  	        vargout_sfrp->state,
  	        vargout_sfrp->set,
  	        vargout_sfrp->read);

	return(DS_NOTOK);
}

//+=====================================================================
//
// Function:	AGPowersupply::State() 
//
// Description:	function to implement the command to return the present 
//		state of a device as a short variable
//
// Input:	none
//
// Output:	short *state - pointer to state returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

/*long AGPowerSupply::State(void *vargin, void *vargout, long *error)
{
   static short *state;
   *error = DS_OK;

   printf("Device::State() called\n");

   state = (short*)vargout;
//
// code to implement the DevState command here
//
   *state = (short)this->state;

   return(DS_OK);
}*/


//+=====================================================================
//
// Function:	AGPowersupply::Hello() 
//
// Description:	function to test dev_putget(DevState) on a local device
//
// Input:	none
//
// Output:	short *state - pointer to state returned
//		long *error - error code returned in the case of problems
//
//-=====================================================================

long AGPowerSupply::Hello(void *vargin, void *vargout, long *error)
{
   static devserver loc_device;
   static long status;
   static short *state;
   static char **name;
   *error = DS_OK;

   printf("AGPowerSupply::Hello() called\n");

   name = (char**)vargin;
   state = (short*)vargout;
//
// import (local) device
//
   status = dev_import(*name,0,&loc_device,error);
   printf("AGPowerSupply::Hello() dev_import(%s) returned %d (error=%d)\n",
          *name, status, *error);
   if (status != DS_OK) return(DS_NOTOK);
//
// execute (local) dev_putget(DevState)
//
   status = dev_putget(loc_device,DevState,NULL,D_VOID_TYPE,
                       state,D_SHORT_TYPE,error);
   printf("AGPowerSupply::Hello() dev_putget(DevState) state = %d returned %d (error=%d)\n",
          *state, status, *error);
   if (status != DS_OK) return(DS_NOTOK);
//
// free (local) device
//
   status = dev_free(loc_device,error);
   printf("AGPowerSupply::Hello() dev_free(%s) returned %d (error=%d)\n",
          *name, status, *error);
   if (status != DS_OK) return(DS_NOTOK);
//
// code to implement the DevState command here
//
   return(DS_OK);
}


//+=====================================================================
//
// Function:	AGPowersupply::~AGPowerSupply() 
//
// Description:	destroy method of AGPowerSupply class (which does nothing)
//
// Input:	none
//
// Output:	none
//
//-=====================================================================

AGPowerSupply::~AGPowerSupply()
{

   printf("AGPowerSupply::~AGPowerSupply() called\n");

}


