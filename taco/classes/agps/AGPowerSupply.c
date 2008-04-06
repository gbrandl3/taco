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
 * File:        AGPowerSupply.c
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description:	Code for implementing the AG Power Supply class
 *		The AG Power Supply is a simulation of a typical
 *		power supply at the ESRF. This means it has two
 *		main state DEVON and DEVOFF, DEVSTANDBY is unknown.
 *		All the common power supply commands are implemented.
 *		The simulation runs under OS9 and Unix. It has been
 *		developed for application program developers who want to
 *		test their applications without accessing real devices
 *
 * Author(s):   A. Goetz
 *              $Author: jkrueger1 $
 *
 * Original:    March 1991
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 
#ifdef linux
#include <stddef.h>
#include <stdio.h>
#endif /* linux */
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif
#if HAVE_PTHREAD_H
#	include <pthread.h>
#endif
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#include <PowerSupply.h>
#include <AGPowerSupplyP.h>
#include <AGPowerSupply.h>
#include <private/ApiP.h>

#include <signal.h>
#ifdef _UCC
#include <inet/netdb.h>
#endif /* _UCC */


 /*
 * public methods
 */

static long class_initialise(DevLong *error);
static long object_create(char *name,DevServer *ds_ptr,DevLong *error);
static long object_initialise(AGPowerSupply ds,DevLong *error);
static long state_handler(AGPowerSupply ds,DevCommand cmd,DevLong *error);

int mythread_start();

static DevMethodListEntry methods_list[] = {
 {DevMethodClassInitialise, class_initialise},
 {DevMethodCreate, object_create},
 {DevMethodInitialise, object_initialise},
 {DevMethodStateHandler, state_handler},
};

AGPowerSupplyClassRec aGPowerSupplyClassRec = {
   /* n_methods */     sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */  methods_list,
};

AGPowerSupplyClass aGPowerSupplyClass = 
                   (AGPowerSupplyClass)&aGPowerSupplyClassRec;

/*
 * public commands 
 */

static long dev_off();
static long dev_on();
static long dev_state();
static long dev_setvalue();
static long dev_readvalue();
static long dev_reset();
static long dev_error();
static long dev_local();
static long dev_remote();
static long dev_status();
static long dev_update();
static long dev_write();
static long dev_read();

static DevCommandListEntry commands_list[] = {
 {DevOff, dev_off, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOff"},
 {DevOn, dev_on, D_VOID_TYPE, D_VOID_TYPE, 0, "DevOn"},
 {DevState, dev_state, D_VOID_TYPE, D_SHORT_TYPE, 0, "DevState"},
 {DevSetValue, dev_setvalue, D_FLOAT_TYPE, D_VOID_TYPE, 0, "DevSetValue"},
 {DevReadValue, dev_readvalue, D_VOID_TYPE, D_FLOAT_READPOINT, 0, "DevReadValue"},
 {DevReset, dev_reset, D_VOID_TYPE, D_VOID_TYPE, 0, "DevReset"},
 {DevStatus, dev_status, D_VOID_TYPE, D_STRING_TYPE, 0, "DevStatus"},
 {DevError, dev_error, D_VOID_TYPE, D_VOID_TYPE, 0, "DevError"},
 {DevLocal, dev_local, D_VOID_TYPE, D_VOID_TYPE, 0, "DevLocal"},
 {DevRemote, dev_remote, D_VOID_TYPE, D_VOID_TYPE, 0, "DevRemote"},
 {DevUpdate, dev_update, D_VOID_TYPE, D_STATE_FLOAT_READPOINT, 0, "DevUpdate"},
 {DevWrite, dev_write, D_VAR_CHARARR, D_VOID_TYPE, 0, "DevWrite"},
 {DevRead, dev_read, D_LONG_TYPE, D_VAR_CHARARR, 0, "DevRead"},
};

static long n_commands = sizeof(commands_list)/sizeof(DevCommandListEntry);

/*
 * a template copy of the default powersupply that normally gets created
 * by the DevMethodCreate. it is initialised in DevMethodCLassInitialise 
 * to default values. these defaults can also be specified in the resource 
 * file or via an admin command.
 */

AGPowerSupplyRec aGPowerSupplyRec;
AGPowerSupply aGPowerSupply = (AGPowerSupply)(&aGPowerSupplyRec);

/*
 * template resource table used to access the static database
 */
  
   db_resource res_table[] = {
               {"state",D_ULONG_TYPE},
               {"set_val",D_FLOAT_TYPE},
               {"channel",D_USHORT_TYPE},
               {"n_ave",D_SHORT_TYPE},
               {"conv_unit",D_STRING_TYPE},
               {"set_offset",D_FLOAT_TYPE},
               {"read_offset",D_FLOAT_TYPE},
               {"set_u_limit",D_FLOAT_TYPE},
               {"set_l_limit",D_FLOAT_TYPE},
               {"polarity",D_SHORT_TYPE},
                             };
   int res_tab_size = sizeof(res_table)/sizeof(db_resource);

/* signal handler */

#ifdef _UCC
#define SIG_ALARM 300

static DevServer ds_g;

void sighandler(long signal, long action)
{
	long error;
	float value;

	printf("sighandler(): received signal %d\n",signal);

	return;
}
#endif /* _UCC */

/*======================================================================
 Function:      static long class_initialise()

 Description:	Initialise the AGPowerSupplyClass, is called once for
		this class per process. class_initialise() will initialise
		the class structure (aGPowerSupplyClass) and the default 
		powersupply device (aGPowerSupply). At present default
		values are hardwired in the include and source code files.

 Arg(s) In:	none

 Arg(s) Out:	long *error - pointer to error code if routine fails.
 =======================================================================*/

static long class_initialise(error)
DevLong *error;
{
   int iret=0;
   AGPowerSupply ps;
   long status;
   unsigned long ul_state=0;
   unsigned short us_channel=0;
  
/*
 * AGPowerSupplyClass is a subclass of PowerSupplyClass
 */

   aGPowerSupplyClass->devserver_class.superclass = (DevServerClass)powerSupplyClass;
   aGPowerSupplyClass->devserver_class.class_name = (char*)malloc(strlen("AGPowerSupply")+1);
#ifdef _NT
   HWALK(aGPowerSupplyClass->devserver_class.class_name);
#endif
   sprintf(aGPowerSupplyClass->devserver_class.class_name,"AGPowerSupply");


/*
 * commands implemented for the AG PowerSUpply class
 */

   aGPowerSupplyClass->devserver_class.n_commands = n_commands;
   aGPowerSupplyClass->devserver_class.commands_list = commands_list;

   aGPowerSupplyClass->devserver_class.class_inited = 1;
/*
 * initialise the template powersupply so that DevMethodCreate has
 * default values for creating a powersupply, these values will be
 * overridden by the static database (if defined there). 
 */

   aGPowerSupply->devserver.class_pointer = (DevServerClass)aGPowerSupplyClass;
/*
 * default is to start with powersupply switched OFF; the state 
 * variable gets (ab)used during initialisation to interpret the
 * initial state of the powersupply: 0==DEVOFF, 1==DEVON. this is
 * because the database doesn't support the normal state variables
 * like DEVON, DEVSTANDBY, DEVINSERTED, etc.
 */
   aGPowerSupply->devserver.state = 0;
   aGPowerSupply->devserver.n_state = aGPowerSupply->devserver.state;
   aGPowerSupply->powersupply.set_val = (float)0.0;
   aGPowerSupply->powersupply.read_val = (float)0.0;
   aGPowerSupply->powersupply.channel = 1;
   aGPowerSupply->powersupply.n_ave = 1;
   aGPowerSupply->powersupply.conv_unit = (char*)malloc(strlen("AMP")+1);
#ifdef _NT
   HWALK(aGPowerSupply->powersupply.conv_unit);
#endif
   sprintf(aGPowerSupply->powersupply.conv_unit,"AMP");
   aGPowerSupply->powersupply.set_offset = (float)0.0,
   aGPowerSupply->powersupply.read_offset = (float)0.0;
   aGPowerSupply->powersupply.set_u_limit = (float)AG_MAX_CUR;
   aGPowerSupply->powersupply.set_l_limit = (float)AG_MIN_CUR;
   aGPowerSupply->powersupply.polarity = 1;

/*
 * interrogate the static database for default values 
 */
  
   ps = aGPowerSupply;
   res_table[0].resource_adr = &ul_state;
   res_table[1].resource_adr = &(ps->powersupply.set_val);
   res_table[2].resource_adr = &us_channel;
   res_table[3].resource_adr = &(ps->powersupply.n_ave);
   res_table[4].resource_adr = &(ps->powersupply.conv_unit);
   res_table[5].resource_adr = &(ps->powersupply.set_offset);
   res_table[6].resource_adr = &(ps->powersupply.read_offset);
   res_table[7].resource_adr = &(ps->powersupply.set_u_limit);
   res_table[8].resource_adr = &(ps->powersupply.set_l_limit);
   res_table[9].resource_adr = &(ps->powersupply.polarity);

   if(db_getresource("CLASS/AGPS/DEFAULT",res_table,res_tab_size,error))
   {
#ifdef _NT
      dprintf1("class_initialise(): db_getresource() failed, error %d",error);
#else
      printf("class_initialise(): db_getresource() failed, error %d\n",*error);
#endif
      dev_printerror_no(SEND,NULL,*error);
      /*return(-1);*/
   }
   else
   {
#ifdef _NT
      dprintf("default values after searching the static database");
      dprintf("CLASS/AGPS/DEFAULT/state         D_ULONG_TYPE   %6d",ul_state);
      dprintf("CLASS/AGPS/DEFAULT/set_val       D_FLOAT_TYPE   %6.0f",ps->powersupply.set_val);
      dprintf("CLASS/AGPS/DEFAULT/channel       D_USHORT_TYPE  %6d",us_channel);
      dprintf("CLASS/AGPS/DEFAULT/n_ave         D_SHORT_TYPE   %6d",ps->powersupply.n_ave);
      dprintf("CLASS/AGPS/DEFAULT/conv_unit     D_STRING_TYPE  %6s",ps->powersupply.conv_unit);
      dprintf("CLASS/AGPS/DEFAULT/set_offset    D_FLOAT_TYPE   %6.0f",ps->powersupply.set_offset);
      dprintf("CLASS/AGPS/DEFAULT/read_offset   D_FLOAT_TYPE   %6.0f",ps->powersupply.read_offset);
      dprintf("CLASS/AGPS/DEFAULT/set_u_limit   D_FLOAT_TYPE   %6.0f",ps->powersupply.set_u_limit);
      dprintf("CLASS/AGPS/DEFAULT/set_l_limit   D_FLOAT_TYPE   %6.0f",ps->powersupply.set_l_limit);
      dprintf("CLASS/AGPS/DEFAULT/polarity      D_SHORT_TYPE   %6d",ps->powersupply.polarity);
#else
      printf("default values after searching the static database\n\n");
/*    printf("CLASS/AGPS/DEFAULT/state         D_LONG_TYPE    %6d\n",ps->devserver.state);*/
      printf("CLASS/AGPS/DEFAULT/ul_state      D_ULONG_TYPE    %6d\n",ul_state);
      printf("CLASS/AGPS/DEFAULT/set_val       D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_val);
/*    printf("CLASS/AGPS/DEFAULT/channel       D_SHORT_TYPE   %6d\n",ps->powersupply.channel);*/
      printf("CLASS/AGPS/DEFAULT/us_channel    D_USHORT_TYPE   %6d\n",us_channel);
      printf("CLASS/AGPS/DEFAULT/n_ave         D_SHORT_TYPE   %6d\n",ps->powersupply.n_ave);
      printf("CLASS/AGPS/DEFAULT/conv_unit     D_STRING_TYPE  %6s\n",ps->powersupply.conv_unit);
      printf("CLASS/AGPS/DEFAULT/set_offset    D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_offset);
      printf("CLASS/AGPS/DEFAULT/read_offset   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.read_offset);
      printf("CLASS/AGPS/DEFAULT/set_u_limit   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_u_limit);
      printf("CLASS/AGPS/DEFAULT/set_l_limit   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_l_limit);
      printf("CLASS/AGPS/DEFAULT/polarity      D_SHORT_TYPE   %6d\n",ps->powersupply.polarity);
#endif
   }

#ifdef NEVER
   status = ds__signal(SIGUSR1, sighandler, error);
   printf("class_initialise(): setup cyclic alarm signal handler (status=%d,error=%d)\n",status,*error);
   if (status == -1 ) dev_printerror_no(SEND,NULL,*error);
   /*alm_cycle(SIGUSR1, 100);*/
#endif /* NEVER */

#if defined(linux) || defined(_HPUX_SOURCE) || defined(solaris)
   mythread_start();
#endif /* linux || _HPUX_SOURCE || solaris */

#ifdef _NT
   dprintf2("returning from class_initialise()");
#else
   printf("returning from class_initialise()\n");
#endif
   return(iret);
}

/*======================================================================
 Function:      static long object_create()

 Description:	create a AGPowerSupply object. This involves allocating
		memory for this object and initialising its name.

 Arg(s) In:	char *name - name of object.

 Arg(s) Out:	DevServer *ds_ptr - pointer to object created.
		long *error - pointer to error code (in case of failure)
 =======================================================================*/

static long object_create(name, ds_ptr, error)
char *name;
DevServer *ds_ptr;
DevLong *error;
{
   int iret = 0;
   AGPowerSupply ps;

#ifdef _NT
   dprintf2("arrived in object_create(), name %s",name);
#else
   printf("arrived in object_create(), name %s\n",name);
#endif

   ps = (AGPowerSupply)malloc(sizeof(AGPowerSupplyRec));
#ifdef _NT
   HWALK(ps);
#endif

/*
 * initialise server with template
 */

   *(AGPowerSupplyRec*)ps = *(AGPowerSupplyRec*)aGPowerSupply;

/*
 * finally initialise the non-default values
 */

   ps->devserver.name = (char*)malloc(strlen(name)+1);
#ifdef _NT
   HWALK(aGPowerSupplyClass->devserver_class.class_name);
#endif
   HWALK(ps->devserver.name);
   sprintf(ps->devserver.name,"%s",name);
   
   *ds_ptr = (DevServer)ps;

#ifdef _NT
   dprintf2("leaving object_create() and all OK");
#else
   printf("leaving object_create() and all OK\n");
#endif

   return(iret);
}
 
static long object_initialise(ps,error)
AGPowerSupply ps;
DevLong *error;
{
   unsigned long ul_state=0;
   unsigned short us_channel=0;

   printf("arrived in object_initialise()\n");
/*
 * initialise powersupply with values defined in database
 */

/* res_table[0].resource_adr = &(ps->devserver.state); */
   res_table[0].resource_adr = &ul_state;
   res_table[1].resource_adr = &(ps->powersupply.set_val);
/* res_table[2].resource_adr = &(ps->powersupply.channel); */
   res_table[2].resource_adr = &us_channel;
   res_table[3].resource_adr = &(ps->powersupply.n_ave);
   res_table[4].resource_adr = &(ps->powersupply.conv_unit);
   res_table[5].resource_adr = &(ps->powersupply.set_offset);
   res_table[6].resource_adr = &(ps->powersupply.read_offset);
   res_table[7].resource_adr = &(ps->powersupply.set_u_limit);
   res_table[8].resource_adr = &(ps->powersupply.set_l_limit);
   res_table[9].resource_adr = &(ps->powersupply.polarity);

   if(db_getresource(ps->devserver.name,res_table,res_tab_size,error))
   {
#ifdef _NT
      dprintf1("object_initialise(): db_getresource() failed, error %d",*error);
#else
      printf("object_initialise(): db_getresource() failed, error %d\n",*error);
#endif
      dev_printerror_no(SEND,NULL,*error);
      /*return(-1);*/
   }
   else
   {
#ifdef _NT
      dprintf("initial values after searching the static database for %s",ps->devserver.name);
      dprintf("state         D_LONG_TYPE    %6d",ps->devserver.state);
      dprintf("set_val       D_FLOAT_TYPE   %6.0f",ps->powersupply.set_val);
      dprintf("channel       D_SHORT_TYPE   %6d",ps->powersupply.channel);
      dprintf("n_ave         D_SHORT_TYPE   %6d",ps->powersupply.n_ave);
      dprintf("conv_unit     D_STRING_TYPE  %6s",ps->powersupply.conv_unit);
      dprintf("set_offset    D_FLOAT_TYPE   %6.0f",ps->powersupply.set_offset);
      dprintf("read_offset   D_FLOAT_TYPE   %6.0f",ps->powersupply.read_offset);
      dprintf("set_u_limit   D_FLOAT_TYPE   %6.0f",ps->powersupply.set_u_limit);
      dprintf("set_l_limit   D_FLOAT_TYPE   %6.0f",ps->powersupply.set_l_limit);
      dprintf("polarity      D_SHORT_TYPE   %6d",ps->powersupply.polarity);
#else
      printf("initial values after searching the static database for %s\n\n",ps->devserver.name);
      printf("ul_state      D_ULONG_TYPE   %6d\n",ul_state);
      printf("set_val       D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_val);
      printf("ul_channel    D_USHORT_TYPE  %6d\n",us_channel);
      printf("n_ave         D_SHORT_TYPE   %6d\n",ps->powersupply.n_ave);
      printf("conv_unit     D_STRING_TYPE  %6s\n",ps->powersupply.conv_unit);
      printf("set_offset    D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_offset);
      printf("read_offset   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.read_offset);
      printf("set_u_limit   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_u_limit);
      printf("set_l_limit   D_FLOAT_TYPE   %6.0f\n",ps->powersupply.set_l_limit);
      printf("polarity      D_SHORT_TYPE   %6d\n",ps->powersupply.polarity);
#endif

/*
 * interpret the initial state of the powersupply
 */
      if (ps->devserver.state == 1)
      {
#ifdef _NT
         dprintf("switching ON");
#else
         printf("switching ON\n");
#endif
         dev_on(ps,NULL,NULL,error);
/*
 * if switched ON then set the current too
 */
         dev_setvalue(ps,&(ps->powersupply.set_val),NULL,error);
      }
      else
      {
#ifdef _NT
         dprintf("switching OFF");
#else
         printf("switching OFF\n");
#endif
/*
 * default is to assume the powersupply is OFF
 */
         dev_off(ps,NULL,NULL,error);
      }
   }

   dev_on(ps,NULL,NULL,error);

   return(DS_OK);
}

/*======================================================================
 Function:      static long state_handler()

 Description:	Check if the command to be executed does not violate
		the present state of the device.

 Arg(s) In:	AGPowerSupply ps - device to execute command to.
		DevCommand cmd - command to be executed.

 Arg(s) Out:	long *error - pointer to error code (in case of failure).
 =======================================================================*/

static long state_handler( ps, cmd, error)
AGPowerSupply ps;
DevCommand cmd;
DevLong *error;
{
   long iret = 0;
   long int p_state, n_state;

   p_state = ps->devserver.state;

/*
 * before checking out the state machine assume that the state
 * doesn't change i.e. new state == old state
 */
   n_state = p_state;

   switch (p_state) {
   
   case (DEVOFF) :
   {
      switch (cmd) {
      
      case (DevOn) : n_state = DEVON;
                     break;

      case (DevError) : n_state = DEVFAULT;
                        break;

      case (DevLocal) : n_state = DEVLOCAL;
                        break;

/* following commands are ignored in this state */

      case (DevSetValue) :
      case (DevReadValue) : iret = -1;
                            *error = DevErr_CommandIgnored;
                            break;
/* following commands don't change the state machine */

      case (DevReset) : 
      case (DevRemote) : 
      case (DevOff) : 
      case (DevState) : 
      case (DevUpdate) :
      case (DevStatus) : break;

      default : break;
      }
      
      break;
   }

   case (DEVON) :
   {
      switch (cmd) {

      case (DevOff) : n_state = DEVOFF;
                      break;
      case (DevError) : n_state = DEVFAULT;
                        break;
      case (DevLocal) : n_state = DEVLOCAL;
                        break;

/* following commands violate the state machine */

      case (DevRemote) : 
      case (DevReset) : iret = -1;
                        (*error) = DevErr_AttemptToViolateStateMachine;
                        break;

/* the following commands don't change the state of the machine */

      case (DevState) : 
      case (DevStatus) : 
      case (DevReadValue) : 
      case (DevSetValue) :
      case (DevUpdate) :
      case (DevRun) : break;

      default : break;
      }

      break;
   }
   case (DEVLOCAL) :
   {
      switch (cmd) {

      case (DevRemote) : n_state = DEVOFF;
                         break;

/* the following commands violate the state machine */

      case (DevOn) :
      case (DevOff) :
      case (DevRun) :
      case (DevReset) :
      case (DevStandby) :
      case (DevError) : iret = -1;
                        (*error) = DevErr_AttemptToViolateStateMachine;
                        break;

/* following commands are ignored */
      case (DevSetValue) : iret = -1;
                           *error = DevErr_CommandIgnored;
                           break;

/* the following commands don't change the state of the machine */

      case (DevState) :
      case (DevStatus) :
      case (DevLocal) :
      case (DevUpdate) :
      case (DevReadValue) : break;

      default : break;

      }

      break;
   }
   case (DEVFAULT) :
   {
      switch (cmd) {

      case (DevReset) : n_state = DEVOFF;
                        break;
      
/* the following commands violate the state machine */

      case (DevOff) :
      case (DevRemote) :
      case (DevOn) :
      case (DevLocal) : iret = -1;
                        (*error) = DevErr_AttemptToViolateStateMachine;
                        break;

/* following commands are ignored */

      case (DevSetValue) : 
      case (DevReadValue) : iret = -1;
                            *error = DevErr_CommandIgnored;
                            break;

/* the following commands don't change the state of the machine */

      case (DevState) : 
      case (DevStatus) : 
      case (DevUpdate) :
      case (DevError) : break;

      default : break;

      }
      break;
   }

   default : break;
   }

/*
 * update powersupply's private variable n_state so that other methods
 * can use it too.
 */

   ps->devserver.n_state = n_state;

/* printf("state_handler(): p_state %2d n_state %2d, iret %2d\n",
          p_state,n_state, iret); */

   return(iret);
}

/*======================================================================
 Function:      static long dev_off()

 Description:	switch the simulated power supply off. the read value 
		and set value get set to zero - this is a convention
		adopted at the ESRF for all powersupplies.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code in case of failure.
 =======================================================================*/

static long dev_off (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->powersupply.read_val = (float)0.0;
	ps->powersupply.set_val = (float)0.0;
	ps->devserver.state = DEVOFF;

	return (iret);
}
	
/*======================================================================
 Function:      static long dev_on()

 Description:	switch simulated powersupply ON

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code (in case of failure)
 =======================================================================*/

static long dev_on (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->devserver.state = DEVON;

	return (iret);
}

/*======================================================================
 Function:      static long dev_state()

 Description:	return state of simulated power supply.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - S_data branch contains status as short
		long *error - pointer to error code (in case routine fails)
 =======================================================================*/

static long dev_state (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevShort *argout;
DevLong *error;
{
	static int n_called=0;
	long iret = 0;

	/*printf("dev_state(%s): called\n",ps->devserver.name);*/

/* this command can be always executed independent of the state  */

	*argout = (short)ps->devserver.state;

/*
 * restart device server - testing reconnection of ud_daemon (andy 28feb01)
 */
#ifdef _UCC
	system("date");
	n_called++;
	if (n_called == 10)
	{
		system("sleep -s 5;AGPSds test -m&");
		exit(0);
	}
#endif /* _UCC */
	return (iret);
}

/*======================================================================
 Function:      static long dev_setvalue()

 Description:	Routine to set current of simulated power supply. This
		version does not check the limits of the set_value.

 Arg(s) In:	AGPowerSupply ps - object to execute command on.
		DevUnion *argin - F_data branch of union contains set value

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code (in the case of failure)
 =======================================================================*/

static long dev_setvalue (ps, argin, argout, error)
AGPowerSupply ps;
DevFloat *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->powersupply.set_val = *argin;

	return (iret);
}

/*======================================================================
 Function:      static long dev_readvalue()

 Description:	Return read value of simulated power supply. The read
		value is the last set value with some simulated noise
		on it. The noise is taken from the time() and is therefore
		repetitive.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - F_data branch contains read value.
		long *error - pointer to error code (in the case of failure)
 =======================================================================*/

static long dev_readvalue (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevFloatReadPoint *argout;
long *error;
{
	long iret = 0;
        float per_error;

	argout->set = ps->powersupply.set_val;

/*
 * the last 4 bits of time() provide some random noise on this
 * simulated reading - andy 11apr90
 */
	per_error = (float)AG_PER_ERROR/(float)100.*(float)2.*
                    (((float)(time(NULL)&0xf)/(float)15.0)-(float)0.5);
/*	            printf("ReadValue(): percent error %6.3f\n",
                            per_error);*/
        argout->read = ps->powersupply.set_val*
                                ((float)1.-per_error);
        ps->powersupply.read_val = argout->read;

	return (iret);
}

/*======================================================================
 Function:      static long dev_remote()

 Description:	Switch simulated power supply from LOCAL mode to socalled
		REMOTE mode i.e. a mode in which the power supply can receive
		commands.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code (in case of failure)
 =======================================================================*/

static long dev_remote (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->devserver.state = ps->devserver.n_state;
	
	return (iret);
}

/*======================================================================
 Function:      static long dev_reset()

 Description:	Reset simulated power supply to a well known state.
		Used to recover from errors mostly. All set and read 
		points are reset at the same time.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error in the case of failure.
 =======================================================================*/

static long dev_reset (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->powersupply.set_val = (float)0.0;
        ps->powersupply.read_val = (float)0.0;
        ps->powersupply.fault_val = 0;
        ps->powersupply.cal_val = (float)0.0;
        ps->powersupply.conv_val = (float)1.0;
        ps->devserver.state = ps->devserver.n_state;

	*error = DevErr_CommandFailed;
	iret = DS_NOTOK;

	return (iret);
}

/*======================================================================
 Function:      static long dev_error()

 Description:	Simulate an error condition on the simulated power supply.
		The system call time() is used to generate any one of 8
		possible errors.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code, in the case of failure.
 =======================================================================*/

static long dev_error (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
DevLong *error;
{
	static int error_no=0;
	char error_message[256];
	long i, iret = 0;

/*
 * use the last 16 bits of the system time to generate an error - andy
 */

	ps->powersupply.fault_val = (unsigned short)(time(NULL)&0xff);
	ps->devserver.state = ps->devserver.n_state;
/*
 * generate a dynamic error message - testing 1-2-3
 */
	for (i=0;i<3;i++)
	{
 	sprintf(error_message,"AGPowerSupply::dev_error(): device %s has generated error no. %d\n",ps->devserver.name, error_no++);
	printf("AGPowerSupply::dev_error(): dynamic error message = %s\n",error_message);
 	dev_error_push(error_message);
	}
	*error = DevErr_CommandFailed;
	iret = DS_NOTOK;

	return (iret);
}

/*======================================================================
 Function:      static long dev_status()

 Description:	Return the state as an ASCII string. Interprets the error
		flag as well if the status is FAULT.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command.
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - STR_data branch of union contains string.
		long *error - pointer to error code (in the case of failure)
 =======================================================================*/

static long dev_status (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevString *argout;
long *error;
{
	static char mess[1024];
	int fault = ps->powersupply.fault_val;
	long iret = 0;
	long p_state;


	p_state = ps->devserver.state;

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

/* translate fault into a string */

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

	*argout = mess;

/* 
   simulate a slow executing command by sleeping for 1 second

 	sleep(1);
 */

	return(iret);
}

/*======================================================================
 Function:      static long dev_local()

 Description:	Switch simulated power supply to LOCAL mode. In this mode
		the power supply does not respond to any commands until
		the next DevRemote command is executed.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - void.
		long *error - pointer to error code in the case of failure.
 =======================================================================*/

static long dev_local (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevVoid *argout;
long *error;
{
	long iret = 0;

	ps->devserver.state = ps->devserver.n_state;

	return(iret);
}

/*======================================================================
 Function:      static long dev_update()

 Description:	Return the state and the read and set points. This command
		is a combination of the DevState and the DevReadValue commands.

 Arg(s) In:	AGPowerSupply ps - object on which to execute command
		DevUnion *argin - void.

 Arg(s) Out:	DevUnion *argout - SRFP_data branch contains the three values.
		long *error - pointer to error code (in the case of failure)
 =======================================================================*/

static long dev_update (ps, argin, argout, error)
AGPowerSupply ps;
DevVoid *argin;
DevStateFloatReadPoint *argout;
long *error;
{
	DevShort darg_short;
	DevFloatReadPoint darg_frp;

/*
 * update state
 */
	dev_state(ps, NULL, &darg_short, error);
	argout->state = darg_short;

/*
 * get latest set and read values
 */
	dev_readvalue(ps, NULL, &darg_frp, error);
	argout->set = darg_frp.set;
	argout->read = darg_frp.read;

	printf("dev_update(): state %d set %f read %f\n",
	        argout->state,
	        argout->set,
	        argout->read);

/*
 * test returning values with DS_WARNING as return status
 */
	return(DS_WARNING);
}

static long dev_write (ps, argin, argout, error)
AGPowerSupply ps;
DevVarCharArray *argin;
DevVoid *argout;
long *error;
{

/*
 * update state
 */
	printf("dev_write(): received %d characters\n",argin->length);

/*
 * get latest set and read values
 */
	return(DS_OK);
}

static long dev_read (ps, argin, argout, error)
AGPowerSupply ps;
DevLong *argin;
DevVarCharArray *argout;
long *error;
{
	static DevVarCharArray vchararr = {0, NULL};
/*
 * update state
 */
	printf("dev_read(): send %d characters\n",*argin);

	vchararr.sequence = realloc(vchararr.sequence,*argin);
	vchararr.length = *argin;

	return(DS_OK);
}

/*======================================================================
 Function:      static long ()

/*======================================================================
 Function:      static long ()

 Description:

 Arg(s) In:

 Arg(s) Out:
 =======================================================================*/

#if defined(linux) || defined(_HPUX_SOURCE) || defined(solaris)

extern DevServerDevices *devices;

void * fire_events(void * arg)
{
  int i;
  long event = 1;
  long counter=0;
  struct timespec t100ms;

  sleep(2);
  fprintf(stderr, "\nfire_events(): starting thread %s\n", (char *) arg);

  for (;;)
  {
     for (i=0; i<2; i++)
     {
     printf("\nfire_events(): firing event (device=%s,type=%08x) counter = %d\n",
	       ((DevServer)devices[i].ds)->devserver.name,event, counter);
     dev_event_fire(devices[i].ds, event,&counter,D_LONG_TYPE,DS_OK,0);
     counter++;
     t100ms.tv_sec = 1;
     t100ms.tv_nsec =  90000000;
     nanosleep(&t100ms, NULL);
     }
  }
  return NULL;
}

int mythread_start()
{
  int retcode;
  pthread_t th_a, th_b;
  void * retval;

#ifdef EVENTS
#if defined(linux) || defined(solaris)
  retcode = pthread_create(&th_a, NULL, fire_events, "a");
#else
  retcode = pthread_create(&th_a,  pthread_attr_default, 
                           (pthread_startroutine_t)fire_events, 
                           (pthread_addr_t)"a");
#endif /* linux */
  if (retcode != 0) fprintf(stderr, "create a failed %d\n", retcode);
#endif /* EVENTS */
  return 0;
}
#endif /* linux || _HPUX_SOURCE */
