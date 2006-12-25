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
 * File:	Inst_verify.c
 *
 * Project:	Device Server Distribution
 *
 * Description:	Dummy class to test control system installations
 *
 * Author(s):   Jens Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	10.5.95
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2006-12-25 19:24:34 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServerP.h>
#include <Inst_verifyP.h>
#include <Inst_verify.h>

/*
 * public methods
 */

static	long	class_initialise();
static	long	object_create();
static	long	object_initialise();
static	long	state_handler();

long minimal_access = WRITE_ACCESS;

static	DevMethodListEntry methods_list[] = {
   	{DevMethodClassInitialise,	class_initialise},
   	{DevMethodInitialise,		object_initialise},
  	{DevMethodCreate,		object_create},
   	{DevMethodStateHandler,		state_handler},
   };


Inst_verifyClassRec inst_verifyClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
   };

Inst_verifyClass inst_verifyClass = (Inst_verifyClass)&inst_verifyClassRec;

/*
 * public commands
 */
static	long	dev_readvalue();
static	long	dev_setvalue();

static	long	dev_status();
static  long    dev_state();

static	DevCommandListEntry commands_list[] = {
   	{DevReadValue, dev_readvalue, D_VOID_TYPE, D_STATE_FLOAT_READPOINT},
   	{DevSetValue, dev_setvalue, D_FLOAT_TYPE, D_VOID_TYPE},
   	{DevState, dev_state, D_VOID_TYPE, D_SHORT_TYPE},
   	{DevStatus, dev_status, D_VOID_TYPE, D_STRING_TYPE},
};
static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);

/*
 * reserve space for a default copy of the inst_verify object
 */
static Inst_verifyRec inst_verifyRec;
static Inst_verify inst_verify = (Inst_verify)&inst_verifyRec;

/*
 * Inst_verify resource tables used to access the static database
 *
 */
db_resource res_object[] = {
	{"read",	D_FLOAT_TYPE, NULL},
	{"set",		D_FLOAT_TYPE, NULL},
   	};
int res_object_size = sizeof(res_object)/sizeof(db_resource);

db_resource res_class[] = {
	{"state",       D_LONG_TYPE, NULL},
	{"config",   	D_LONG_TYPE, NULL},
   	};
int res_class_size = sizeof(res_class)/sizeof(db_resource);


/*======================================================================
 Function:      static long class_initialise()

 Description:	routine to be called the first time a device is 
 		created which belongs to this class (or is a subclass
		thereof. This routine will be called only once.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/
static long class_initialise(long *error)
{
/*
 * Inst_verifyClass is a subclass of the DevServerClass
 */
	inst_verifyClass->devserver_class.superclass = devServerClass;
	inst_verifyClass->devserver_class.class_name = (char*)malloc(sizeof("Inst_verifyClass")+1);
	sprintf(inst_verifyClass->devserver_class.class_name,"Inst_verifyClass");
	inst_verifyClass->devserver_class.class_inited = 1;
	inst_verifyClass->devserver_class.n_commands = n_commands;
	inst_verifyClass->devserver_class.commands_list = commands_list;

/*
 * initialise inst_verify with default values. these will be used
 * for every Inst_verify object created.
 */
	inst_verify->devserver.class_pointer = (DevServerClass)inst_verifyClass;
	inst_verifyClass->inst_verify_class.config_param	= 0;
	inst_verify->devserver.state = DEVON;

/*
 * Interrogate the static database for default values
 *
 */
	res_class[0].resource_adr	= &(inst_verify->devserver.n_state);
	res_class[1].resource_adr	= &(inst_verifyClass->inst_verify_class.config_param);
	if(db_getresource("CLASS/Inst_verify/DEFAULT", res_class, res_class_size, error))
		return(DS_NOTOK);
	return(DS_OK);
}

/*======================================================================
 Function:	static long object_create()

 Description:	routine to be called on creation of a device object

 Arg(s) In:	char *name - name to be given to device

 Arg(s) Out:	DevServer *ds_ptr - pointer to created device
		long *error - pointer to error code, in case routine fails
 =======================================================================*/
static long object_create(char *name, DevServer *ds_ptr, long *error)
{
	Inst_verify ds;
	ds = (Inst_verify)malloc(sizeof(Inst_verifyRec));

/*
 * initialise device with default object
 */
	*(Inst_verifyRec*)ds = *(Inst_verifyRec*)inst_verify;

/*
 * finally initialise the non-default values
 */
	ds->devserver.name = (char*)malloc(strlen(name)+1);
	sprintf(ds->devserver.name,"%s",name);
	*ds_ptr = (DevServer)ds;

	return(DS_OK);
}

/*============================================================================

Function:	static long object_initialise()

Description:	routine to be called on initialisation of a device object

Arg(s) In:	Inst_verify ds	- object to initialise

Arg(s) Out:

		long *error     - pointer to error code, in case routine fails
=============================================================================*/
static long object_initialise(Inst_verify ds, long *error)
{
	ds->inst_verify.read_value 	= 0;
	ds->inst_verify.set_value 	= 0;


	res_object[0].resource_adr        = &(ds->inst_verify.read_value);
	res_object[1].resource_adr        = &(ds->inst_verify.set_value);
   
	if(db_getresource(ds->devserver.name, res_object, res_object_size, error))
		return(DS_NOTOK);
	return(DS_OK);
}

/*======================================================================
 Function:      static long state_handler()

 Description:	this routine is reserved for checking wether the command
		requested can be executed in the present state.

 Arg(s) In:	Inst_verify ds - device on which command is to executed
		DevCommand cmd - command to be executed

 Arg(s) Out:	long *error - pointer to error code, in case routine fails
 =======================================================================*/
static long state_handler(Inst_verify ds, DevCommand cmd, long *error)
{
	long int 	p_state, 
			n_state,
			iret = DS_OK;
/*
 * Get here the real state of the physical device
 *
 *  example:
 *  ds->devserver.state=get_state();
 */
	p_state = ds->devserver.state;

/*
 * Before checking out the state machine assume that the state doesn't
 * change i.e. new state == old state
 *
 */
	n_state = p_state;
	switch (p_state)
	{
		case (DEVON) :	/* Device is on */
			switch (cmd)
			{
				/* Allowed Command(s) */
				case (DevReadValue):	
				case (DevSetValue):	
					n_state = DEVON;
					break;
				case (DevStatus):	
				case (DevState):	
					break;
			}
			break;
		default:
			*error = DevErr_UnrecognisedState;
			iret = DS_NOTOK;
			break;
	}
	ds->devserver.n_state = n_state;

	return(iret);
}

/*============================================================================
 Function:      static long dev_readvalue()

 Description:	Reads the read- and set value and the state of the
 		device
   	
 Arg(s) In:	Inst_verify 	ds 	- 
		DevVoid  	*argin  - none
   				  
 Arg(s) Out:	DevStateFloatReadPoint	*argout - Read- and set value and 
					state of the device
		long		        *error	- pointer to error code, 
					in case routine fails. 
					Error code(s): none
 ============================================================================*/
static long  dev_readvalue(Inst_verify ds, DevVoid *argin, DevStateFloatReadPoint *argout, long *error)
{
#ifdef DEBUG
	printf("Inst_verify, dev_readvalue(), entered\n");
#endif 

/*
 *
 * Insert here the code of the command
 *
 */
	argout->set   = ds->inst_verify.set_value;
	argout->read  = ds->inst_verify.read_value;
	argout->state = ds->devserver.state;

	return(DS_OK);
}

/*============================================================================
 Function:      static long dev_setvalue()

 Description:	Apply a new set value
   	
 Arg(s) In:	Inst_verify 	ds 	- 
		DevFloat  	*argin  - New set value
   				  
 Arg(s) Out:	DevVoid		*argout - none
		long		*error	- pointer to error code, in case
		 			  routine fails. Error code(s):
					  none
 ============================================================================*/
static long  dev_setvalue(Inst_verify ds, DevFloat *argin, DevVoid *argout, long *error)
{
#ifdef DEBUG
	printf("Inst_verify, dev_setvalue(), entered\n");
#endif /*PRINT*/

/*
 *
 * Insert here the code of the command
 *
 */
	ds->inst_verify.set_value = *argin;
#ifdef DEBUG
	printf("Inst_verify, dev_setvalue(), value = %f\n",ds->inst_verify.set_value);
#endif /*PRINT*/
	return(DS_OK);
}

/*============================================================================
 Function:      static long dev_state()

 Description:	return the state of the device

 Arg(s) In:	 Inst_verify 	ds 	- 
		 DevVoid  	*argin  - none
   				  
 Arg(s) Out:	 DevShort	*argout - returned state 
		 long *error - 	pointer to error code, in case routine fails

============================================================================*/
static long dev_state(Inst_verify ds, DevVoid *argin, DevShort *argout, long *error)
{
	*argout	= ds->devserver.state;
	return(DS_OK);
}

/*============================================================================
 Function:      static long dev_status()

 Description:	return state of the device as an ASCII string

 Arg(s) In:	 Inst_verify 	ds 	- 
		 DevVoid  	*argin  - none
   				  
 Arg(s) Out:	 DevString	*argout - contains string 
============================================================================*/
static long dev_status(Inst_verify ds, DevVoid *argin, DevString *argout, long *error)
{
	static	char	str[80];

	snprintf(str, sizeof(str), "The device is : %s\n", DEVSTATES[ds->devserver.state]);
	*argout = str;
	return (DS_OK);
}

