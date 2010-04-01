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
 * File:        Server.c
 *
 * Description: TACO server in Python
 *
 * Author(s):   J. Meyer
 *		$Author: jensmeyer $
 *
 * Original:    June 2000
 *
 * Date:	$Date: 2010-04-01 15:52:49 $
 *
 * Version:	$Revision: 1.12 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef HAVE_FSTAT
#undef HAVE_FSTAT
#endif
 
#include <API.h>
#include <Admin.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <tacopython.h>
#include <pythonP.h>

extern long     debug_flag;

long		startup (char *svc_name, DevLong *error);
void 		initServer();

PyObject 	*Py_devices;
PyObject	*ErrorObject;
PyThreadState	*thread_state;

/**
 *  A static c module to implement a device server startup 
 */
static PyObject *Server_startup(PyObject *self, PyObject *args)
{
	char	*name,
		*pers_name;	
			
        if ( !PyArg_ParseTuple(args, "ssO", &name, &pers_name, &Py_devices) )
	{
        	return PyInt_FromLong(-1L);
	}

	dev_printdebug(DBG_STARTUP, "Got server name\n");
	dev_printdebug(DBG_STARTUP, "name = %s, personal_name = %s\n", name, pers_name);

/*
* initialise Taco
*/
	
	nethost_alloc(&error);

/* 
 * start the device server
 */
	device_server (name, pers_name, (int)0, (int)1, (int)0, (int)0, (int)0, (char **)NULL);
        return PyInt_FromLong(0L);
}

/* 
 * A static c module to implement a device server startup without database 
 */
static PyObject *Server_startup_nodb (PyObject *self, PyObject *args)
{
	char		*name,
			*pers_name,
			*device_list[MAX_PY_DEVICE];
	int		pn,	
			n_device;
	short		i;	
	PyObject 	*Py_device,
        		*py_name;

			
	if ( !PyArg_ParseTuple(args, "ssOi", &name, &pers_name, &Py_devices, &pn) )
	{
		return PyInt_FromLong(-1L);
	}
	dev_printdebug(DBG_STARTUP, "Got server name\n");
	dev_printdebug(DBG_STARTUP, "name = %s, personal_name = %s\npn = %d\n", name, pers_name, pn);
	 	
/*
 * Get all device names
 */
	n_device = PySequence_Size (Py_devices);
        for (i = 0; i < n_device; i++)
	{
		Py_device = PySequence_GetItem (Py_devices, i);
/*
 * Read the device name
 */
		py_name  = PyObject_CallMethod (Py_device, "get_dev_name", NULL);
		device_list[i] = PyString_AsString(py_name);
	}

/*
* initialise Taco
*/
	
	nethost_alloc(&error);

/* 
 * start the device server
 */
	device_server (name, pers_name, 0, 1, 1, pn, n_device, device_list);
			
        return PyInt_FromLong(0L);
}


static PyObject *Server_cmd_string (PyObject *self, PyObject *args)
{
	static char     *ret_str = NULL;
	long		cmd;
	DevLong		error;
        char            res_path[256],
        		res_name[80];
        db_resource     res_tab;
        unsigned long   cmd_number_mask = 0x3ffff;
        unsigned short  team,
        		server,
        		cmds_ident;
			
	if ( !PyArg_ParseTuple(args, "i", &cmd) )
	{
		return Py_None;
	}
/*
 * Decode the command nuber into the fields:  team, server and cmds_ident.
 */
	team   = (_Int)(cmd >> DS_TEAM_SHIFT);
	team   = team & DS_TEAM_MASK;
	server = (_Int)(cmd >> DS_IDENT_SHIFT);
	server = server & DS_IDENT_MASK;
	cmds_ident = (_Int)(cmd & cmd_number_mask);

/*
 * Create the resource path and the resource structure.
 */
	sprintf(res_path, "CMDS/%d/%d", team, server);
	sprintf (res_name, "%d", cmds_ident);

	if (ret_str != NULL)
	{
		free (ret_str);
		ret_str = NULL;
	}
	   
	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the command name string from the database.
 */
	if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
	{
		printf ("db_getresource failed: %s\n", dev_error_str (error));
		return Py_None;
	} 
/*
 * If the variable ret_str is still NULL, no resource value was found in the database, 
 * but the function was executed without error. In this case return the value "None".
 */
	if ( ret_str == NULL )
	{
		return Py_None;
	}

	return PyString_FromString(ret_str);
}

static PyMethodDef Server_methods[] = {
        {"startup",       Server_startup,      METH_VARARGS},
        {"startup_nodb",  Server_startup_nodb, METH_VARARGS},
	{"cmd_string",    Server_cmd_string,   METH_VARARGS},
        {NULL,          NULL}           /* sentinel */
};

void initServer()
{
	PyObject	*py_module,
			*py_dict,
			*py_exception_dict;
	
	PyImport_AddModule("Server");
	py_module = Py_InitModule("Server", Server_methods);
	
/*
 * Prepare the error object to catch python exceptions
 *
 * Add some symbolic constants to the module 
 */
	py_dict = PyModule_GetDict (py_module);
	
/* 
 * Add the variable taco_error to the exception object 
 */
	py_exception_dict = PyDict_New();
	PyDict_SetItemString (py_exception_dict, "taco_error", Py_BuildValue ("i", 0));		      

	ErrorObject = PyErr_NewException ("Server.error", NULL, py_exception_dict);
    	PyDict_SetItemString (py_dict, "error", ErrorObject);				  	 	 		
}

/** 
 * Startup function as needed to start TACO device servers 
 *
 * @param svc_name the name of the server consisting of executeable name and personal name
 * @param error pointer to the error code. It will set in a case of error
 *
 * @return DS_OK in case of no failure otherwise DS_NOTOK
 */
long startup (char *svc_name, DevLong *error)
{ 
	DevMethodFunction	fn;
	Python	 		ds_python[MAX_PY_DEVICE];
	PyObject 		*Py_device,
				*py_name;
	char			*dev_name;
	int			nu_of_devices,
				exported_devices = 0,
				i;
/*		
	debug_flag = (DEBUG_ON_OFF | DBG_ERROR | DBG_STARTUP | DBG_COMMANDS | DBG_METHODS);
*/
   	dev_printdebug (DBG_TRACE | DBG_STARTUP, "startup() : executing\n");

/*
 * Loop over all devices to create
 */
	nu_of_devices = PySequence_Size (Py_devices);	
	for (i = 0; i < nu_of_devices; i++)
	{
		Py_device = PySequence_GetItem(Py_devices, i);
/*
 * Read the device name
 */
		py_name   = PyObject_CallMethod(Py_device, "get_dev_name", NULL);
		dev_name = PyString_AsString(py_name);
/*
 * Create the device
 */
		if (ds__create(dev_name, pythonClass, &(ds_python[i]), error) != DS_OK)
		{
			dev_printerror(SEND, "create failed (%d): %s\n", *error, dev_error_str (*error));
//	      		return (DS_NOTOK);
			continue;
		}
		else 
			dev_printdebug(DBG_STARTUP, "\t\t- Created\n");
	   
	   /*
	    * Store the pointer to the python object to use
	    */
	    
	   ds_python[i]->python.Py_device = Py_device; 
		
/*
 * Initialise the device
 */
		fn = ds__method_finder(ds_python[i], DevMethodInitialise);
		if (fn != NULL)
		{
			if ((fn)(ds_python[i], error) != DS_OK)
			{
				dev_printerror(SEND, "initialise failed (%d): %s\n", *error, dev_error_str (*error));
				continue;
			}
			else 
				dev_printdebug(DBG_STARTUP, "\t\t- Initialised\n");
		}
	   
/*
 * Export the device on the network
 */
		if (ds__method_finder(ds_python[i], DevMethodDevExport)(dev_name, ds_python[i], error) != DS_OK)
		{
			dev_printerror(SEND, "export failed (%d): %s\n", *error, dev_error_str (*error));
			continue;
		}
		else 
			dev_printdebug(DBG_STARTUP, "\t\t- Exported\n\n");
		exported_devices++;
	}
	
/*
 * release the global interpreter lock and store the thread state!
 */
	thread_state = PyEval_SaveThread();
	return exported_devices ? (DS_OK) : DS_NOTOK;
}

