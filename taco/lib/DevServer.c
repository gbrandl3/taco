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
 * File:	DevServer.c
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	code for implementing the public methods for the 
 *		device server class.
 *
 * Author(s);	Andy Goetz 
 *		Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	June 1990
 *
 * Version:	$Revision: 1.11 $
 *
 * Date:	$Date: 2006-09-18 22:31:43 $
 *
 ********************************************************************-*/

/*
 * Include files and Static Routine definitions
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#include <errno.h>

#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h
 */

/*
 *  Configuration flags
 */

#ifdef __cplusplus
extern "C" configuration_flags      config_flags;
#else
extern configuration_flags      config_flags;
#endif

/*
 *  Debug flag
 */

#ifdef __cplusplus
extern "C"  long     debug_flag; 
#else
extern long 	debug_flag;
#endif



/*
 * Declare a pointer for the globale device array.
 * All exported devices are stored here.
 * Memory will be dynamically allocated during the
 * device_export().
 */
extern DevServerDevices	*devices;
extern long 		max_no_of_devices;

/* 
 * private methods accessible only via the method_finder
 */

static long class_initialise	PT_( (long *error) );
/*
static long device_export	PT_( (char *dev_name, DevServer ds, 
				long *error) );
 */
static long command_handler	PT_( (DevServer ds, long cmd, 
				DevArgument argin,  long argin_type, 
				DevArgument argout, long argout_type, 
				long *error) );
static long error_handler	PT_( (DevServer ds, long *error) );
static long device_destroy		PT_( ( DevServer ds, long *error) );

static DevMethodListEntry methods_list[] = {
 {DevMethodClassInitialise, (DevMethodFunction)class_initialise},
 {DevMethodDevExport, (DevMethodFunction)dev_export},
 {DevMethodCommandHandler, (DevMethodFunction)command_handler},
 {DevMethodErrorHandler, (DevMethodFunction)error_handler},
 {DevMethodDestroy, (DevMethodFunction)device_destroy},
};

/*
 * only the list of methods have to be initialise in
 * the static region, all the other fields get initialised
 * at runtime
 */

DevServerClassRec devServerClassRec = {
   /* n_methods */		5,
   /* methods_list */		methods_list,
};

DevServerClass devServerClass = (DevServerClass)&devServerClassRec;

/**@ingroup dsAPI
 * Device server class initialise.
 *
 * Called by the method finder via : DevMethodClassInitialise
 *		
 * @param error Will contain an appropriate error
 *		code if the corresponding call
 *		returns a non-zero value.
 *
 * @return  DS_OK or DS_NOTOK
 */
static long class_initialise (long *error)
{
	*error = 0;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nclass_initialise() [devServerClass] : entering routine\n");

/*
 * initialise all class related fields
 */
	devServerClass->devserver_class.class_name = (char*)malloc(sizeof("DevServerClass")+1);
	if ( devServerClass->devserver_class.class_name == NULL )
	{
		*error = DevErr_InsufficientMemory;
		return(DS_NOTOK);
	}
	strcpy(devServerClass->devserver_class.class_name,"DevServerClass");

/*
 * this is a root class
 */
	devServerClass->devserver_class.superclass = 0;
	devServerClass->devserver_class.class_inited = 1;

/*
 * the devServerClass has no commands of its own
 */
	devServerClass->devserver_class.n_commands = 0;
	devServerClass->devserver_class.commands_list = 0;
 
	return (0);
}


/**
 * Device server's command handler   
 *
 * Called by the method finder via : DevMethodCommandHandler.	
 *
 * @param ds       	pointer to the object.
 * @param ds_cmd        command to be executed.
 * @param argin		pointer to input arguments.
 * @param argin_type 	data type of input arguments.
 *
 * @param argout	pointer for output arguments.
 * @param argout_type	data type of output arguments.
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
static long command_handler (DevServer ds, long ds_cmd, 
			     DevArgument argin,  long argin_type,
			     DevArgument argout, long argout_type, long *error)
{
	long 			i, 
				iret = 0;
	DevServerClass 		ds_class;
	DevMethodFunction 	cmd_function;
	static char	 	error_message[1024];

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\ncommand_handler() : entering routine\n");

	ds_class = ds->devserver.class_pointer;

/*
 * first check if the command is implemented in the device server
 */
	for (i = 0; i < ds_class->devserver_class.n_commands; i++)
	{
		if (ds_cmd == (ds_class->devserver_class.commands_list[i].cmd))
		{
/*
 *  check types of incoming and outgoing arguments for
 *  the command function
 */
	 		if (argin_type != ds_class->devserver_class.commands_list[i].argin_type 
				|| argout_type != ds_class->devserver_class.commands_list[i].argout_type)  
	 		{
				(*error) = DevErr_IncompatibleCmdArgumentTypes;
				return (-1);
			}

/*
 * found command , now check the device state if it is possible to
 * execute this command
 *
 * this version does not recuperate the new state from the state-handler
 * in the command execution part the state will be updated. 
 *
 * NOTE - this method is not using the david carron Finite State Automaton
 */
         		if((iret = (ds__method_finder(ds,DevMethodStateHandler)) (ds, ds_cmd, error)) != DS_OK)
            			return (DS_NOTOK);

/*
 *  call command function
 */
         		cmd_function = (DevMethodFunction)(ds_class->devserver_class.commands_list[i].fn);
         		return (cmd_function)(ds, argin, argout, error);
      		}
	}
   
/*
 * if program arrives here then command is not implemented
 *
 * since V8.32 of DSAPI the command_list entry has an extra field
 * (cmd_name), if one of the classes in a device server have not
 * been recompiled then this can cause the command entries to be
 * incorrectly interpreted, therefore if a command is not found
 * then print a warning message.
 */
   	snprintf(error_message, sizeof(error_message), "command_handler(): command %d not found, maybe you need to recompile the class %s\n",
		ds_cmd, ds_class->devserver_class.class_name);
	fprintf(stderr, error_message);
	dev_error_push(error_message);
	(*error) = DevErr_CommandNotImplemented;
	return (DS_NOTOK);
}


/**@ingroup dsAPI
 * Device server's error handler   
 *
 * Called by the method finder via : DevMethodErrorHandler.	
 *
 * NOT YET USED !!
 *
 * @param ds    pointer to the object.
 * @param error Will contain an appropriate error code if the 
 *		corresponding call returns a non-zero value.
 *
 * @return  DS_OK or DS_NOTOK
 */
static long error_handler (DevServer ds, long *error)
{
	int iret = DS_OK;
/*
	printf("ds::error_handler()\n");
 */
	return (iret);
}

/**@ingroup dsAPI
 * Device server's device destroy   
 *
 * Called by the method finder via DevMethodDestroy.	
 *
 * Simply frees the object structure.
 *
 *
 * @param ds	pointer to the object.
 * @param error Will contain an appropriate error code if the 
 *		corresponding call returns a non-zero value.
 * 
 * @return  DS_OK or DS_NOTOK
 */
static long device_destroy (DevServer ds, long *error)
{
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\ndevice_destroy() : entering routine\n");

/*
 * this very simple version of device_destroy simply does a
 * free on the space occupied by the object
 */
	free (ds->devserver.name);
	free (ds);

	return (DS_OK);
}

/***************************************************************************
 * Convenience functions for the Device Server programs. These provide     *
 * and alternate calling sequence for executing methods of the root        *
 * class. This enables programmers who are more familiar with procedure    *
 * oriented interfaces to write procedure driven programs without being    *
 * aware of the object oriented design of the Device Servers.              *
 *                                                                         *
 * - andy 25nov90                                                          *
 *                                                                         *
 ***************************************************************************/

/**@ingroup dsAPIserver
 * This function executes a command on a given object locally i a device server.
 * Memory freeing must be done with free() and @b not with dev_xdrfree().
 *
 * With the extended functionality of @b dev_putget and @b dev_put the function
 * should be used only to access objects which are not exported.
 *
 * To access internal exported devices the unified interface must be used, to
 * avoid access and security problems in the coming releases.
 *
 * @param ptr_ds	pointer to the object.
 * @param cmd           command to be executed.
 * @param argin  	pointer to input arguments.
 * @param argin_type  	data type of input arguments.
 * @param argout  	pointer for output arguments.
 * @param argout_type  	data type of output arguments.
 * @param error         Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long dev_cmd (void *ptr_ds, long cmd, DevArgument argin,  long argin_type,
	      DevArgument argout, long argout_type, long *error)
{
   	DevServer       ds;
	ds = (DevServer) ptr_ds;

	return (ds__method_finder(ds, DevMethodCommandHandler)
               (ds, cmd, argin, argin_type, argout, argout_type, error));
}

#if !defined(WIN32)
/**@ingroup dsAPI
 * The function supports the checking of pending  device server commands  (rpc requests) 
 * on all open sockets.  If commands are available on filedesciptors (sockets), the next 
 * pending request for every descriptor will be executed and ds__svcrun will return afterwards.
 * If no commands are pending on any descriptor ds__svcrun should return after 10ms.
 * 
 * Problem : OS9 seems to have a fixed minimal timeout for the select() about 1sec.
 *
 * @param error	pointer to error code, in case routine fails.
 * 
 * @return DS_OK or DS_NOTOK
 */
long ds__svcrun (long *error)
{
	struct timeval	timeout;
#ifdef linux
	fd_set		readfds;
#else
	int 		readfds;
#endif /* linux */


	*error = 0;

/*
 * Set the startup configuration flag.
 * Indicates, that the process acts as a server now.
 */
	config_flags.startup = True;

/*
 *  set timeout to the shortest time os9 can measure (1 tick).
 */
	timeout.tv_sec  = 0;
	timeout.tv_usec = 10000;

/*
 *  set filedescriptor mask 
 */
#if !defined (vxworks)
#ifdef linux
	readfds = svc_fdset;
#else
   	readfds = svc_fds;
#endif /* linux */
#else  /* !vxworks */
/*
 * the global symbol svc_fds is apparently NOT DEFINED for VxWorks
 * ignore it for the moment BUT this means that ds__svcrun() will
 * NOT WORK because the select() call will not be looking at the
 * correct (rpc) file descriptors. 
 *
 * andy 28apr97
 */
	readfds = 0;
#endif /* !vxworks */

/*
 *  check for pending commands on sockets
 */

#if defined (__hpux)
#if defined (__STDCPP__) && !defined (_GNU_PP)
	switch ( select (32, (fd_set *) &readfds, NULL, NULL, &timeout) )
#else
	switch ( select (32, (int *) &readfds, NULL, NULL, &timeout) )
#endif /* hpux10 and STDCPP */
#else
#ifdef linux
	switch ( select (32, &readfds, NULL, NULL, &timeout) )
#else
	switch ( select (32, (struct fd_set *) &readfds, NULL, NULL, &timeout) )
#endif /* linux */
#endif /* hpux */
	{
		case -1:
/* 
 * A signal was delivered before any of the
 * selected for events occurred or before the
 * time limit expired.
 */
			if ( errno == EINTR )
				break;

/* 
 * Return error message
 */
			dev_printerror (SEND, "ds__svcrun() : select() : errno = %d", (char*)errno);	
			*error = DevErr_RPCFailed;
			return (DS_NOTOK);
	   	case  0:
/*
 * timeout, no commands pending.
 */
			break;
		default:
/*
 *  execute the next available command on every socket.
 */
#ifdef linux
			svc_getreqset (&readfds);
#else
			svc_getreq (readfds);
#endif /* linux */
			break;
	}
	return (DS_OK);
}
#endif  /* WIN32 */

