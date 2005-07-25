/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2005 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
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
 * File:	Python.c
 *
 * Project:	Python device server
 *
 * Description:	Supports a device server class written in Python
 *
 * Author(s):	J. Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	June 2000
 *
 * Date:	$Date: 2005-07-25 13:33:42 $
 *
 * Version:	$Revision: 1.4 $
 */

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <Admin.h>

#include <DevServerP.h>
#include <pythonP.h>
#include <python.h>


/*
 * external python object
 */
 
extern PyObject 	*Py_devices;
extern PyObject		*ErrorObject;
extern PyThreadState	*thread_state;
 
/*
 * public methods
 */

static long class_initialise(long *error);
static long object_create(char *name,DevServer *ds_ptr,long *error);
static long object_initialise(Python ds,long *error);
static long state_handler(Python ds,DevCommand cmd,long *error);

static	DevMethodListEntry methods_list[] = {
   	{DevMethodClassInitialise,	class_initialise},
   	{DevMethodInitialise,		object_initialise},
  	{DevMethodCreate,		object_create},
   	{DevMethodStateHandler,		state_handler},
   };


PythonClassRec pythonClassRec = {
   /* n_methods */        sizeof(methods_list)/sizeof(DevMethodListEntry),
   /* methods_list */     methods_list,
   };

PythonClass pythonClass = (PythonClass)&pythonClassRec;

/*
 * public commands
 */

static long dev_exec_cmd (Python ds,DevVoid *argin, DevVoid *argout, long *error);


/*
 * reserve space for a default copy of the python object
 */

static PythonRec pythonRec;
static Python python = (Python)&pythonRec;

/**
 * Routine to be called the first time a device is created which belongs to this 
 * class (or is a subclass thereof). This routine will be called only once.
 *
 * @param error pointer to error code, in case routine fails
 * 
 * @return DS_OK
 */
static long class_initialise(long *error)
{
	*error = 0; 

/*
 * PythonClass is a subclass of the DevServerClass
 */

	pythonClass->devserver_class.superclass = devServerClass;
	pythonClass->devserver_class.class_inited = 1;
          		
/*
 * initialise object with default values. These will be used for every Python object created.
 */
	python->devserver.class_pointer = (DevServerClass)pythonClass;
	python->devserver.state         = DEVUNKNOWN;
  
	return(DS_OK);
}

/**
 * Routine to be called on creation of a device object.
 *
 * @param name name to be given to device
 * @param ds_ptr pointer to created device
 * @param error pointer to error code, in case routine fails
 *
 * @return DS_OK
 */
static long object_create(char *name, DevServer *ds_ptr, long *error)
{
	Python ds = (Python)malloc(sizeof(PythonRec));
/*
 * initialise device with default object
 */
	*(PythonRec*)ds = *(PythonRec*)python;
/*
 * finally initialise the non-default values
 */
	ds->devserver.name = (char*)malloc(strlen(name)+1);
	sprintf(ds->devserver.name,"%s",name);

	dev_printdebug (DBG_TRACE | DBG_STARTUP, " %s device_name = %s\n", __FUNCTION__, ds->devserver.name);

	*ds_ptr = (DevServer)ds;
	return(DS_OK);
}

/**
 * Routine to be called on initialisation of a device object.
 *
 * @param ds object to initialise
 * @param error pointer to error code, in case routine fails
 *
 * @return DS_OK
 */
static long object_initialise (Python ds, long *error)
{
	PythonClass	object_class;
	PyObject 	*Py_device,
			*py_cmd_list,
			*py_cmd_list_keys,
			*py_key,
			*py_cmd_list_item,
			*py_argin_type,
			*py_argout_type,
			*py_cmd_name,
			*py_name;
	
	long		nu_of_cmds;
	char		*class_name;
	short		i;	
	
	*error = 0;
	
/*
 * Allocate a new class structure for every object.  This class is a generic class which 
 * can export serveral pyhton classes at the same time.
 */
	object_class = (PythonClass) calloc (1, sizeof(PythonClassRec));
	
/*
 * Copy the references from the global pyhton class object
 */
	object_class->devserver_class.n_methods = pythonClass->devserver_class.n_methods;
	object_class->devserver_class.methods_list = pythonClass->devserver_class.methods_list;
	object_class->devserver_class.superclass = pythonClass->devserver_class.superclass; 
	object_class->devserver_class.class_inited = pythonClass->devserver_class.class_inited; 
				
/*
 * Read the class name from the python object
 */
	py_name    = PyObject_CallMethod(ds->python.Py_device, "get_class_name", NULL);
	class_name = PyString_AsString(py_name);
	object_class->devserver_class.class_name = (char*)malloc(strlen(class_name)+1);
	sprintf (object_class->devserver_class.class_name, class_name);
   
/*
 * Initialise the command list from the Python object!
 *
 * Get the number of available commands
 */
	py_cmd_list      = PyObject_CallMethod(ds->python.Py_device, "get_cmd_list", NULL);	 
	py_cmd_list_keys = PyDict_Keys (py_cmd_list);
	nu_of_cmds       = PyList_Size (py_cmd_list_keys);       
	object_class->devserver_class.n_commands = nu_of_cmds;
/*
 * Allocate space for the command list
 */
	object_class->devserver_class.commands_list = (DevCommandListEntry *)calloc(nu_of_cmds, sizeof(DevCommandListEntry));  

/*
 * Fill the command list
 */
	for (i = 0; i < nu_of_cmds; i++)
	{
/* 
 *Get the command 
 */
		py_key = PyList_GetItem (py_cmd_list_keys, i);
		object_class->devserver_class.commands_list[i].cmd = PyInt_AsLong(py_key);
      
/* 
 * Store the command function pointer 
 */
		object_class->devserver_class.commands_list[i].fn = dev_exec_cmd;
      
/* 
 * Get the input and output argument data types 
 */
		py_cmd_list_item = PyDict_GetItem (py_cmd_list, py_key);
		py_argin_type    = PyList_GetItem (py_cmd_list_item, 0);
		py_argout_type   = PyList_GetItem (py_cmd_list_item, 1);
		py_cmd_name      = PyList_GetItem (py_cmd_list_item, 2);
      
		object_class->devserver_class.commands_list[i].argin_type = PyInt_AsLong(py_argin_type);
		object_class->devserver_class.commands_list[i].argout_type = PyInt_AsLong(py_argout_type);  
		object_class->devserver_class.commands_list[i].cmd_name = PyString_AsString(py_cmd_name);  
	}  
        		
/*
 * initialise object with default values. These will be used for every Python object created.
 */
	ds->devserver.class_pointer = (DevServerClass)object_class;

/*
 * Free allocated memory
 */
	Py_XDECREF (py_name);
	Py_XDECREF (py_cmd_list);
	Py_XDECREF (py_cmd_list_keys);
 
	return(DS_OK);
}

/**
 * This routine is reserved for checking wether the command requested can be executed 
 * in the present state.
 *
 * @param ds device on which command is to executed
 * @param cmd command to be executed
 * @param error pointer to error code, in case routine fails
 * 
 * @return DS_OK
 */
static long state_handler (Python ds, DevCommand cmd, long *error)
{
	*error = 0;

/*
 * Store the command to treat
 */
	ds->python.cmd_code = cmd;
	return (DS_OK);
}

/**
 * A command function called for all incomming commands. Searches in the command list of 
 * the Python object the python method to be called.
 *
 * @param ds pointer to C object
 * @param argin Arguments to be passed to Python object
 * @param argout Arguments returned from Python object
 * @param long error pointer to error code, in case routine fails
 *
 * @return DS_OK
 */
static long dev_exec_cmd (Python ds, DevVoid *argin, DevVoid *argout, long *error)
{
	static PyObject *py_argout	= NULL,
			*py_error	= NULL;
	static char	*sequence_ptr   = NULL;
	
	PyObject	*py_argin	= NULL,
			*py_cmd_list	= NULL,
			*py_cmd_list_keys = NULL,
			*py_key		= NULL,
			*py_cmd_list_item = NULL,
			*py_method_name	= NULL,
			*py_argout_type	= NULL,
			*py_argin_type	= NULL,
			*py_exception	= NULL;

	char		error_msg[256],	
			*method_name;
	long		nu_of_cmds,
			argout_type,
			argin_type,
			is_single,
			is_array,
			is_special;
	short		i;
			
   	*error = 0;

/*
 * Acquire the global interpreter lock and restore the tread state
*/
	PyEval_RestoreThread (thread_state);
	
/*
 * Free allocated memory from last call
 */
	Py_XDECREF (py_argout);
        Py_XDECREF (py_error);
	if (sequence_ptr != NULL)
	{
		free (sequence_ptr);
	};	   
  
/*
 * Read the command list
 */
	py_cmd_list = PyObject_CallMethod(ds->python.Py_device, "get_cmd_list", NULL);
	
/*
 * Get the list of commands
 */
	py_cmd_list_keys = PyDict_Keys (py_cmd_list);
	nu_of_cmds       = PyList_Size (py_cmd_list_keys);
	
/*
 * Search the command in list
 */
	for (i = 0; i < nu_of_cmds; i++)
	{
		py_key = PyList_GetItem (py_cmd_list_keys, i);
	    
		if ( ds->python.cmd_code == PyInt_AsLong(py_key) )
		{
/*
 * Get the python methode to execute
 */
			py_cmd_list_item = PyDict_GetItem (py_cmd_list, py_key);
			py_method_name  = PyList_GetItem (py_cmd_list_item, 2);
			method_name     = PyString_AsString (py_method_name);
	      
/*
 * Clear all pending python errors
 */
	      		PyErr_Clear(); 
	       
/*
 * convert the incoming data
 */
			py_argin_type   = PyList_GetItem (py_cmd_list_item, 0);
			argin_type      = PyInt_AsLong(py_argin_type);

/*
 * Check the output argument type
 */
			is_single	 = 0;
			is_array   = 0;
			is_special = 0;      
			if (check_type(argin_type, &is_single, &is_array, &is_special) == DS_NOTOK )
			{
				printf("unknown input argument type\n");
				*error = DevErr_DevArgTypeNotRecognised;
			}
			else
			{		 
/* 
 * If the argument contains only a single value 
 */
				if (is_single)
				{
/*
 * function is called argout, because it was created for the client part! 
 */
					if (get_argout_single(argin, argin_type, &py_argin, error_msg) == DS_NOTOK)
					{
						printf ("Error: %s\n", error_msg);
						*error = DevErr_PythonDataConversionError;
					}
				}
				else
				{
/* 
 * convert TACO array to python tuple 
 */
					if (is_array)
					{
/*
 * function is called argout, because it was created for the client part! 
 */
						if (get_argout_array(argin, argin_type, &py_argin, error_msg)==DS_NOTOK)
						{
							printf ("Error: %s\n", error_msg);
							*error = DevErr_PythonDataConversionError;
						}		
					}
				}
		    
				if (argin_type != D_VOID_TYPE)
				{
					py_argout = PyObject_CallMethod(ds->python.Py_device, method_name, "O", py_argin);
				}
				else
				{
					py_argout = PyObject_CallMethod (ds->python.Py_device, method_name, NULL);
				}   	      	      
			}	      
	      
/*
 * Free allocated memory for incoming aguments
 */
			Py_XDECREF (py_argin);
	      		 
/*
 * Check for exceptions from the python module
 */
			if (py_argout == NULL)
			{
				if (PyErr_ExceptionMatches(ErrorObject) == True)
				{
					printf ("received a TACO style exception!\n");
		    
					py_error = PyObject_GetAttrString (ErrorObject, "taco_error");
					printf ("error = %d \n", PyInt_AsLong(py_error));
									    
					*error = PyInt_AsLong (py_error);
				}
				else
				{
					PyErr_Print();
					*error = DevErr_PythonException;
				}

/*
 * release the global interpreter lock and save the thread state
 */
				thread_state = PyEval_SaveThread();
				return (DS_NOTOK);
			}		 				       
	     
/*
 * convert the outgoing data
 */
			py_argout_type   = PyList_GetItem (py_cmd_list_item, 1);
			argout_type      = PyInt_AsLong(py_argout_type);

/*
 * Check the output argument type
 */
			is_single = is_array = is_special = 0;      
			if (check_type(argout_type, &is_single, &is_array, &is_special) == DS_NOTOK )
			{
				printf("unknown output argument type\n");
				*error = DevErr_DevArgTypeNotRecognised;
			}
			else
			{
/*
 * Set the sequence pointer to NULL if no array type was requested
 */
				if (!is_array)
				{
					sequence_ptr = NULL;
				} 
		  
/* 
 * If the argument contains only a single value 
 */
				if (is_single)
				{
/*
 * function is called argin, because it was created for the client part! 
 */
		    			if (get_argin_single(argout, argout_type, py_argout, error_msg) == DS_NOTOK)
					{
						printf ("Error: %s\n", error_msg);
						*error = DevErr_PythonDataConversionError;
					}	       
				}
				else
				{
					if (is_array)
					{
/* 
 * convert python tuple to TACO variable length array 
 */
						if (get_argin_array(argout, argout_type, py_argout, error_msg) == DS_NOTOK)
						{
							printf ("Error: %s\n", error_msg);
							*error = DevErr_PythonDataConversionError;
						}
			  
/*
 * Get the sequence pointer which needs to be freed
 */
						sequence_ptr = ((DevVarCharArray *)(argout))->sequence;		       
					}
				}
			}
		 
/*
 * Free allocated memory before returning
 */
			Py_XDECREF (py_cmd_list_keys);	      
			Py_XDECREF (py_cmd_list);
			break;
		}
	}
	   	   
/*
 * release the global interpreter lock and save the thread state
 */
	thread_state = PyEval_SaveThread();

	return (*error == 0) ?  (DS_OK) : (DS_NOTOK);
}

