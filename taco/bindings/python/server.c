static char RcsId[] = "$Header: /home/jkrueger1/sources/taco/backup/taco/bindings/python/server.c,v 1.1 2003-03-18 16:54:16 jkrueger1 Exp $";
/*********************************************************************
 * File:        Server.c
 *
 * Description: TACO server in Python
 *
 * Author(s):   J. Meyer
 *
 * Original:    June 2000
 *
 * $Log: not supported by cvs2svn $
 * Revision 2.7  2002/03/06  15:34:58  15:34:58  meyer (J.Meyer)
 * Corrected version numbering
 * 
 * Revision 2.6  2002/03/06  14:30:52  14:30:52  meyer (J.Meyer)
 * Version for server without database
 * 
 * Revision 2.5  2001/10/17  18:42:57  18:42:57  meyer (J.Meyer)
 * Corrected command ececution for LINUX
 * 
 * Revision 2.4  2001/07/18  16:13:58  16:13:58  meyer (J.Meyer)
 * Corrected dev_getresource().
 * 
 * Revision 2.2  2001/04/27  14:37:14  14:37:14  meyer (J.Meyer)
 * Corrected modules to work only with one shared library
 * 
 * Revision 2.1  2001/02/22  16:57:59  16:57:59  meyer (J.Meyer)
 * TACO server and client as one package!
 * 

 *
 * $Revision: 1.1 $
 *********************************************************************/
 
#include <API.h>
#include <Admin.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <python.h>
#include <pythonP.h>



extern long     debug_flag;

long		startup (char *svc_name, long *error);
void 		initServer();

PyObject 	*Py_devices;
PyObject	*ErrorObject;
PyThreadState	*thread_state;



/* A static c module to implement a device server startup */

static PyObject *
Server_startup (self, args)
        PyObject *self; /* Not used */
        PyObject *args;		
{
	char	*name;
	char	*pers_name;	
			
        if ( !PyArg_ParseTuple(args, "ssO", &name, &pers_name, &Py_devices) )
	   {
           return PyInt_FromLong(-1L);
	   }

	printf ("Got server name\n");
	printf ("name = %s, personal_name = %s\n", name, pers_name);
	 	
	/* 
	 * start the device server
	 */
	 
	device_server (name, pers_name, 0, 0, 0, NULL);
			
        return PyInt_FromLong(0L);
}

/* A static c module to implement a device server startup without database */

static PyObject *
Server_startup_nodb (self, args)
        PyObject *self; /* Not used */
        PyObject *args;		
{
	char	*name;
	char	*pers_name;	
	int	pn;	
	short	i;	
	PyObject *Py_device;
        PyObject *py_name;
	int	n_device;
	char	*device_list[MAX_PY_DEVICE];

			
        if ( !PyArg_ParseTuple(args, "ssOi", &name, &pers_name, &Py_devices, &pn) )
	   {
           return PyInt_FromLong(-1L);
	   }

	printf ("Got server name\n");
	printf ("name = %s, personal_name = %s\n", name, pers_name);
	printf ("pn = %d\n", pn);
	 	
	/*
         * Get all device names
	 */

	n_device = PyTuple_Size (Py_devices);
        for (i=0; i<n_device; i++)
           {
           Py_device = PyTuple_GetItem (Py_devices, i);

           /*
            * Read the device name
            */

	   py_name  = PyObject_CallMethod (Py_device, "get_dev_name", NULL);
           device_list[i] = PyString_AsString(py_name);
	   }

	/* 
	 * start the device server
	 */
	 
	device_server (name, pers_name, 1, pn, n_device, device_list);
			
        return PyInt_FromLong(0L);
}




static PyObject *
Server_cmd_string (self, args)
        PyObject *self; /* Not used */
        PyObject *args;		
{
	static char     *ret_str = NULL;
	long		cmd;
	long		error;
        char            res_path[256];
        char            res_name[80];
        db_resource     res_tab;
        unsigned long   cmd_number_mask = 0x3ffff;
        unsigned short  team;
        unsigned short  server;
        unsigned short  cmds_ident;
			
        if ( !PyArg_ParseTuple(args, "i", &cmd) )
	   {
           return Py_None;
	   }
/*
 * Decode the command nuber into the fields:
 * team, server and cmds_ident.
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
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value "None".
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

void
initServer()
{
	PyObject	*py_module;
	PyObject	*py_dict;
	PyObject	*py_exception_dict;
	
        PyImport_AddModule("Server");
        py_module = Py_InitModule("Server", Server_methods);
	
	/*
	 * Prepare the error object to catch python exceptions
	 */
	 
        /* Add some symbolic constants to the module */
        py_dict = PyModule_GetDict (py_module);
	
	/* Add the valriable taco_error to the exception object */
	py_exception_dict = PyDict_New();
	PyDict_SetItemString (py_exception_dict, "taco_error",
			      Py_BuildValue ("i", 0));		      

	ErrorObject = PyErr_NewException ("Server.error", NULL, 
	                                  py_exception_dict);
    	PyDict_SetItemString (py_dict, "error", ErrorObject);				  	 	 		
}




/* Startup function as needed to start TACO device servers */

long startup (char *svc_name, long *error)
{
	Python	 ds_python[MAX_PY_DEVICE];
	PyObject *Py_device;
	PyObject *py_name;
	char	 *dev_name;
	int	 nu_of_devices;
	int	 i;
		
	
	debug_flag = (DEBUG_ON_OFF | DBG_ERROR |
                      DBG_STARTUP | DBG_COMMANDS | DBG_METHODS);

   	dev_printdebug (DBG_TRACE | DBG_STARTUP,
                   "startup() : executing\n");

	/*
	 * Loop over all devices to create
	 */
	 
	nu_of_devices = PyTuple_Size (Py_devices);	
	for (i=0; i<nu_of_devices; i++)
	   {
	   Py_device = PyTuple_GetItem (Py_devices, i);
	   
	   /*
	    * Read the device name
	    */

	   py_name   = PyObject_CallMethod (Py_device, "get_dev_name", NULL);
	   dev_name = PyString_AsString(py_name);
		 	
	   /*
	    * Create the device
	    */
	 
     	   if (ds__create(dev_name, pythonClass, &(ds_python[i]), error) 
	                  != DS_OK)
              {
              printf ("create failed: %s\n", dev_error_str (*error));
	      return (DS_NOTOK);
              }
           else printf ("\t\t- Created\n");
	   
	   /*
	    * Store the pointer to the python object to use
	    */
	    
	   ds_python[i]->python.Py_device = Py_device; 
		
	   /*
	    * Initialise the device
	    */
	 	
           if (ds__method_finder (ds_python[i], DevMethodInitialise)
                                 (ds_python[i], error) != DS_OK)
              {
              printf("initialise failed: %s\n", dev_error_str (*error));
              return (DS_NOTOK);
              }
           else printf("\t\t- Initialised\n");
	   
	   /*
	    * Export the device on the network
	    */
	
           if (ds__method_finder (ds_python[i], DevMethodDevExport)
                                 (dev_name, ds_python[i], error) != DS_OK)
              {
              printf("export failed: %s\n", dev_error_str (*error));
 	      return (DS_NOTOK);
              }
           else printf("\t\t- Exported\n\n");
	   }
	
        /*
         * release the global interpreter lock and store the thread state!
	 */

	thread_state = PyEval_SaveThread();
   	   
	return (DS_OK);
}

