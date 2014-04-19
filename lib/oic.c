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
 * File:	oic.c
 *
 * Project:	Device Servers in C++
 *
 * Description:	source code for Objects In C (OIC) a method for creating 
 *		classes in C. The source code has been derived from the
 *		version 4.27 of the original file svc_api.c.
 *
 * Author(s);	Andy Gotz
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1995
 *
 * Version:	$Revision: 1.11 $
 *
 * Date:	$Date: 2008-10-23 05:28:17 $
 *
 ********************************************************************-*/

/*
 * Include files and Static Routine definitions
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>

/***************************************************************************
 *                                                                         *
 * the following methods form an integral part of the OIC methodology,     *
 * they have been stored here because all programs using OIC will          *
 * be subclasses of the device servers.                                    *
 *                                                                         *
 * andy goetz jul90                                                        *
 *                                                                         *
 ***************************************************************************/
 
 
/**@ingroup oicAPI
 * This function searches for a method in the class specified. It returns the pointer to the 
 * method function if the requested method was found in the class. If no such method was 
 * specified the status DS_NOTOK is returned.
 *
 * @param ptr_ds_class 	class of the device server object.
 * @param method 	method to be searched for.
 * @param function_ptr 	function pointer of method to be searched for.
 *
 * @retval  DS_OK 
 * @retval DS_NOTOK
 */
long ds__method_search (void *ptr_ds_class, DevMethod method,
			DevMethodFunction *function_ptr)
{
	register int 	i;
	DevServerClass  ds_class = (DevServerClass) ptr_ds_class;

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nds__method_search() : entering routine\n");

	for (i = 0; i < ds_class->devserver_class.n_methods; i++)
	{
		if (method == (ds_class->devserver_class.methods_list[i].method))
		{
/*
 * found method 
 */
			(*function_ptr) = ds_class->devserver_class.methods_list[i].fn;
			return (DS_OK);
		}
	}
/*
 * method not found in the method list
 */
	return (DS_NOTOK);
}

/**@ingroup oicAPI
 * This function searches for a method in the class hierarchy of the object @b ds 
 * and returns a pointer to the method function. If the method was not found in the object's 
 * class, the search continues in all its superclasses up to the general device server class.
 *
 * If the the method is not implemented the method finder takes @b DRASTIC action and exits.
 * This has been included in the specification to guarantee that on returning from the method
 * finder the method can be directly executed.
 *
 * @param ptr_ds     	pointer to device server object.
 * @param method 	method to be searched for.
 * 
 * @return  function pointer of method.
 */
DevMethodFunction ds__method_finder (void *ptr_ds, DevMethod method)
{
	DevServerClass 		ds_class;
	DevMethodFunction 	method_fn;
	int 			pid = 0;
	DevServer       	ds = (DevServer)ptr_ds;

/*
 * This function will be take the place of a method dispatcher in its absence. 
 * Any object oriented programming style has to have a method dispatcher/finder. 
 * This way classes are kept encapsulated and the application send messages to 
 * that class. The method finder will search in the methods list of the devices's 
 * class for the method and return the function implementing it. if the method 
 * is not implemented the method finder takes DRASTIC action and exits. This
 * has been included in the specification to garantuee that on returning from 
 * the method finder the method can be directly executed.
 *
 * A method dispatcher would go further and accept the arguments for the method 
 * and dispatch them immediately. For the present this kind of method handler is 
 * not implemented. 
 *
 * ds has to be derived from the DevServer class i.e. it is assumed to have a 
 * DevServerPart which has a pointer to a subclass of DevServerClass, it in turn 
 * has a method list and a number of methods field.
 *
 * - andy 05jul90
 */

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nds__method_finder() : entering routine\n");

	ds_class = ds->devserver.class_pointer;
	do 
	{
/*
 * first check if the method is implemented in this class 
 */
		if ( ds__method_search(ds_class,method,&method_fn) == 0 )
		{
/*
 * found method 
 */
			return(method_fn);
		}

/*
 * if program arrives here then the method is not implemented in
 * this class. if this is not a root class then go one class higher
 * and repeat the search.
 */
	} while ((ds_class = ds_class->devserver_class.superclass) != 0);

	ds_class = ds->devserver.class_pointer;
	dev_printerror (WRITE,
	    "ds__method_finder() : requested method (%d) not implemented for this class",
	    (char*)method);
	dev_printerror (SEND, "%s", "Server did an exit !!");

/*
 *  unregister server and exit
 */

#if !defined (WIN32)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
	kill (pid,SIGQUIT);
#else
	raise(SIGTERM);
#endif
	return NULL;
}

/**@ingroup oicAPI
 * This function creates a new device server object of the class ds_class and will return
 * a pointer on the object. Before creating the object (DevMethodCreate : obj_create(3X))
 * the class and all its superclasses are checked to see if they have been initialised.
 * If not, then the DevMethodClassInitialise (class_init(3x)) is called for each uninitialised
 * class.
 *
 * @param name 		name of the object to create.
 * @param ptr_ds_class  pointer to the object class.
 * @param ptr_ds_ptr 	pointer to the new object structure.
 * @param error 	Will contain an appropriate error code if the corresponding call returns a non-zero value.
 * 
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long ds__create (char *name, void *ptr_ds_class,void *ptr_ds_ptr, DevLong *error)
{
	DevServerClass ds_class_tmp;
	DevMethodFunction method_fn;

	DevServerClass  ds_class;
	DevServer 	*ds_ptr;

	ds_class = (DevServerClass) ptr_ds_class;
	ds_ptr   = (DevServer *)    ptr_ds_ptr;


	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nds__create() : entering routine\n");

	*error = 0;

/*
 * first check if this class and all its superclasses are initialised
 */

	ds_class_tmp = ds_class;
	do 
	{
		if (ds_class_tmp->devserver_class.class_inited != 1)
		{
			if (ds__method_search(ds_class_tmp,DevMethodClassInitialise,&method_fn) == 0)
			{
/*
 * initialise class
 */
				if ((method_fn) (error) != 0)
				{
					return(-1);
				}
			}
			else
			{
				return(-1);
			}
		}
	}   while ((ds_class_tmp = ds_class_tmp->devserver_class.superclass) != 0);

	if (ds__method_search(ds_class,DevMethodCreate,&method_fn) == 0)
	{
		return((method_fn)(name, ds_ptr, error));
	}

/*
 * class doesn't have any create method ! what lack of class ?!
 */

	return(-1);
}


