/*+*******************************************************************

 File:		oic.c

 Project:	Device Servers in C++

 Description:	source code for Objects In C (OIC) a method for creating 
		classes in C. The source code has been derived from the
		version 4.27 of the original file svc_api.c.

 Author(s);	Andy Gotz
 		$Author: jkrueger1 $

 Original:	July 1995

 Version:	$Revision: 1.3 $

 Date:		$Date: 2004-02-19 15:42:49 $

 Copyright (c) 1995-1997 by European Synchrotron Radiation Facility, 
                           Grenoble, France
********************************************************************-*/

/*
 * Include files and Static Routine definitions
 */

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
 
 
/*+**********************************************************************
 Function   :   extern long ds__method_search()

 Description:   device server method searcher for one class

 Arg(s) In  :   DevServerClass ds_class - class of the object.
	    :   DevMethod method        - method to be searched for.

 Arg(s) Out :   DevMethodFunction *function_ptr - function pointer of method.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long ds__method_search (void *ptr_ds_class, DevMethod method,
			DevMethodFunction *function_ptr)
{
	register int i;

	DevServerClass  ds_class;
	ds_class = (DevServerClass) ptr_ds_class;


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


/*+**********************************************************************
 Function   :   extern DevMethodFunction ds__method_finder()

 Description:   device server's method finder

 Arg(s) In  :   DevServer ds     - pointer to object.
	    :   DevMethod method - method to be searched for.

 Arg(s) Out :   none

 Return(s)  :   DevMethodFunction - function pointer of method.
***********************************************************************-*/

DevMethodFunction ds__method_finder (void *ptr_ds, DevMethod method)
{
	DevServerClass 	ds_class;
	DevMethodFunction 	method_fn;
	int 			pid = 0;

	DevServer       ds;
	ds = (DevServer) ptr_ds;

	/*
 * this function will be take the place of a method dispatcher
 * in its absence. any object oriented programming style has
 * to have a method dispatcher/finder. this way classes are
 * kept encapsulated and the application send messages to 
 * that class. the method finder will search in the methods
 * list of the devices's class for the method and return the
 * function implementing it. if the method is not implemented
 * the method finder takes DRASTIC action and exits. this
 * has been included in the specification to garantuee that
 * on returning from the method finder the method can be directly
 * executed.
 *
 * a method dispatcher would go further and accept the arguments
 * for the method and dispatch them immediately. for the present
 * this kind of method handler is not implemented. 
 *
 * ds has to be derived from the DevServer class i.e. it is
 * assumed to have a DevServerPart which has a pointer to a
 * subclass of DevServerClass, it in turn has a method list 
 * and a number of methods field.
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
	}   while ((ds_class = ds_class->devserver_class.superclass) != 0);

	ds_class = ds->devserver.class_pointer;
	dev_printerror (WRITE,
	    "ds__method_finder() : requested method (%d) not implemented for this class",
	    (char*)method);
	dev_printerror (SEND, "%s", "Server did an exit !!");

	/*
    *  unregister server and exit
    */

#if !defined (_NT)
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

/*+**********************************************************************
 Function   :   extern long ds__create()

 Description:   creates a device server object.

 Arg(s) In  :   char *name              - name of the object to create.
	    :   DevServerClass ds_class - pointer to the object class.

 Arg(s) Out :   DevServer *ds_ptr - pionter to the new object structure.
	    :	long *error       - Will contain an appropriate error
			  	    code if the corresponding call
				    returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long ds__create (char *name, void *ptr_ds_class,void *ptr_ds_ptr, long *error)
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


