/*********************************************************************
 * File:        taco.c
 *
 * Description: interface python - Taco
 *
 * Author(s):   MCD
 *		$Author: jkrueger1 $
 *
 * Original:    December 99
 * 
 * Date:	$Date: 2005-02-22 13:13:57 $
 *
 * Version:	$Revision: 1.4 $
 *
 *********************************************************************/

#include "config.h"
#include <Python.h>

#include <ctype.h>

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <dc.h>

#ifdef NUMPY
#	if HAVE_NUMARRAY_ARRAYOBJECT_H
#		include <numarray/arrayobject.h>
#	elif HAVE_NUMERIC_ARRAYOBJECT_H
#		include <Numeric/arrayobject.h>
#	endif
#endif

#include <taco.h>

extern long     debug_flag;
static long	flag=0;

static PyObject *ErrorObject;

#define onError(message) {PyErr_SetString(ErrorObject,message);return NULL;}

static short devstatus;
static long error;
static long readwrite = 0;
static long db_imported = 0;
static PyObject *glob_tuple;
static PyObject *glob_dict;
      				      		      
/**
 * frees a device 
 *
 * @param self	unused
 */
void  esrf_free(void *self)
{
	static devserver *ds;
   
	if (flag != 0)
		printf("-- esrf_free: enter\n");

	ds = (devserver *) self;
	devstatus = dev_free(*ds,&error);
	if (devstatus != 0) 
		printf("esrf_free failed ");
	else if (flag != 0)
		printf("-- esrf_free: OK\n");
}

/**
 * Imports a device 
 *
 * @param self	unused
 * @param args	list of arguments - device name (in)
 * 			
 * @returns device server C pointer no error NULL error	
 */
static PyObject* esrf_import(PyObject *self,PyObject *args)
{
	char 			*devname;
	static devserver	*ds_pt;

/* get the device name */
	if (!PyArg_ParseTuple(args,"s",&devname))
		onError("usage: esrf_import (<device_name>)")
      
	if (flag != 0)
		printf("-- esrf_import: devname = %s\n",devname); 

	ds_pt = malloc(sizeof(devserver));
	if (!ds_pt)
	{
		printf("Could not allocate memory for the device\n");
		onError("Could not allocate memory for the device");
	}
	devstatus = dev_import(devname, readwrite, ds_pt, &error);
	if (devstatus != 0)
	{
		free(ds_pt); 
		printf("%s\n",dev_error_str(error));
		onError("import failed in dev_import");
	}

	return PyCObject_FromVoidPtr((void *)ds_pt, (void *)esrf_free);   
}

/**
 * sets a debug flag
 *
 * @param self	unused
 * @param args	list of arguments - debug value
 *				   0 : no debug trace
 *				   else : debug trace
 *			
 * @returns None no error NULL error	
 */ 
static PyObject* esrf_debug(PyObject *self,PyObject *args)
{   
/* 
 * get the flag value 
 */
	if (!PyArg_ParseTuple(args, "d", &flag))
		onError("usage: esrf_debug (<flag>)")
   
	Py_INCREF(Py_None);
	return Py_None;   
}

/**
 * Asks for device command list 
 *
 * @param self unused
 * @param args	list of arguments C object pointer given by dev_import
 *
 * @returns a dictionary
 *		{ 'cmd': [cmd,in_type,out_type], ... }
 *		NULL:			error	
 */ 
static PyObject* esrf_query(PyObject *self, PyObject *args)

{
	int 			i;
	static DevVarCmdArray 	varcmdarr;
	PyObject 		*py_ds_pt,
				*mydict,
				*mylist,
				*item;
	static devserver 	*ds;

	if (flag != 0)
		printf("-- esrf_query: enter\n");
/* 
 * get pointer on device server 
 */
	if (!PyArg_ParseTuple(args,"O",&py_ds_pt))
		onError("usage: esrf_query(<c_object>)")

	ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
	devstatus = dev_cmd_query(*ds,&varcmdarr,&error);
	if (devstatus != 0) 
		onError("esrf_query failed ");

/*  
 * build the output dictionary  
 */
	if ( (mydict = PyDict_New()) == NULL)
		onError("cannot build commands dictionary");
       
	for (i = 0; i < varcmdarr.length; i++)
	{
/* 
 * build the output list of command descriptors 
 */
		if ( (mylist = PyList_New(3)) == NULL)
			onError("cannot build command descriptors list");
/* 
 * cmd 
 */
		if ( (item = PyInt_FromLong(varcmdarr.sequence[i].cmd)) == NULL)
			onError("cannot build command descriptors list");
		if (PyList_SetItem(mylist,0,item) == - 1)
			onError("cannot build command descriptors list");
/* 
 * in_type 
 */
		if ( (item = PyInt_FromLong(varcmdarr.sequence[i].in_type)) == NULL)
			onError("cannot build command descriptors list");
		if (PyList_SetItem(mylist,1,item) == -1)
			onError("cannot build command descriptors list");
/* 
 * out_type 
 */
		if ( (item = PyInt_FromLong(varcmdarr.sequence[i].out_type)) == NULL)
			onError("cannot build command descriptors list");
		if (PyList_SetItem(mylist,2,item) == -1)
			onError("cannot build command descriptors list");

		if (PyMapping_SetItemString(mydict,varcmdarr.sequence[i].cmd_name,mylist)==-1)
			onError("cannot build command descriptors list in dictionary");
	}   
      
	if (flag != 0)
		printf("-- esrf_query: ok\n");
   
	return mydict;
}

/**
 * Asks for commands 
 *	
 * @param self
 * @param args	list of arguments:
 *				- device name 
 *				- resource name
 *			
 * @returns resource value (string pack): OK
 *		NULL:		 error	
 */ 
static PyObject *esrf_getresource(PyObject *self, PyObject *args)
{
	char 		*devname,
			*resname;
	db_resource 	pt_res;
	static char 	mystring[200],
			*res_string = "";
	unsigned int 	num_res;
	long error;
	int 		ret,
			i;
	PyObject 	*item,
   			*mylist;
	DevVarStringArray	res_array = {0, NULL};

	if (flag != 0)
		printf("-- esrf_getresource: enter\n");
	if (!PyArg_ParseTuple(args,"ss",&devname,&resname))
		onError("usage: esrf_getresource(<devname>,<resname>)")
      
	if (flag != 0)
	{
      		printf("device name: %s\n",devname);
		printf("resource name: %s\n",resname);
	}
   
/*
 * Read resources always as string array!
 */
	pt_res.resource_name = resname;
	pt_res.resource_type = D_VAR_STRINGARR;
	pt_res.resource_adr  = &(res_array);

	num_res = 1;
	ret = db_getresource(devname, &pt_res, num_res, &error);
	if (ret != 0)
	{
		printf("error on db_getresource : (%d) %s\n",error,dev_error_str(error));
		onError("error on db_getresource");

		Py_INCREF(Py_None);
		return Py_None;
	}
	else 
	{
/*
 * If no resource value was found, return an empty string
 */
		if (res_array.length == 0)
		{
			return(Py_BuildValue("s", res_string));
		}
      
/*
 * If only on value was found return it as a simple string
 */
		if (res_array.length == 1)
		{
			item = Py_BuildValue ("s", res_array.sequence[0]);
/* 
 * free allocated memory 
 */
			free (res_array.sequence[0]);
			free (res_array.sequence);
			return item;
		}
		else
		{
/*
 * If several resource values are read, return them as a list of strings
 */
			if ( (mylist = PyList_New(res_array.length)) == NULL)
			{
				printf("error on db_getresource : cannot build list\n");
				onError("error on db_getresource");

				Py_INCREF(Py_None);
				return Py_None;
			}
			for (i = 0; i < res_array.length; i++)
			{
				item = PyString_FromString( (char *) res_array.sequence[i]);
				PyList_SetItem (mylist,i,item);
	    			free (res_array.sequence[i]);
			}

/* 
 * free allocated memory 
 */
			free (res_array.sequence);
			return (mylist);
		}
	}
}

/**
 * set a device resource value 
 *
 * @param self	unused
 * @param args	list of arguments:
 *				- devname: device name 
 *				- resname: resource name
 *				- object: containing the value packed as a string
 *
 * @returns Py_None: 	if OK
 *		NULL: 		if error	
 */ 
static PyObject* esrf_putresource(PyObject *self,PyObject *args)
{
	char 		*devname,
   			*resname,
			*resval;
	db_resource 	pt_res;
	static char 	mystring[200],
   			mystringval[200],
			*my_stringval;
	unsigned int 	num_res;
	long 		error;
	int 		ret;
      
	if (flag != 0)
		printf("-- esrf_putresource: enter\n");
	if (!PyArg_ParseTuple(args,"sss",&devname,&resname,&resval))
		onError("usage: esrf_getresource(<devname>,<resname>,<value>)")
      
	if (flag != 0)
	{
		printf("device name: %s\n",devname);
		printf("resource name: %s\n",resname);
		printf("resource value: %s\n",resval);
	}
   
	pt_res.resource_name = mystring;
	strcpy(mystring,resname);
	pt_res.resource_type = (short)D_STRING_TYPE;
	my_stringval = &(mystringval[0]);
	pt_res.resource_adr = (void *)(&(my_stringval));
	strcpy(mystringval,resval);

	num_res = 1;
	ret = db_putresource(devname,&pt_res,num_res,&error);
	free(my_stringval);
	if (ret != 0)
	{
		printf("error on db_putresource : (%d) %s\n",error,dev_error_str(error));
		onError("error on db_putresource")
	}
	Py_INCREF(Py_None);
	return Py_None;
}

/**
 * deletes a device resource 
 *
 * @param self	unused
 * @param args	list of arguments:
 *				- devname: device name 
 *				- resname: resource name
 *			
 * @return Py_None: 	if OK
 *		NULL: 		if error	
 */
static PyObject *esrf_delresource(PyObject *self,PyObject *args)
{
	char 		*devname,
			*resname;
	int 		ret;
	unsigned int 	res_num = 1;
	long 		error;
         
	if (flag != 0)
		printf("-- esrf_delresource: enter\n");
	if (!PyArg_ParseTuple(args,"ss",&devname,&resname))
		onError("usage: esrf_delresource(<devname>,<resname>)")
      
	if (flag != 0)
	{
		printf("device name: %s\n",devname);
		printf("resource name: %s\n",resname);
	}

	if (db_delresource(devname, &resname, res_num, &error) != 0)
	{
		printf("error deleting resource for device %s: resource %s\n", devname,resname);
		onError("error deleting resource ")
	}
	Py_INCREF(Py_None);
	return(Py_None);
}

/**
 * sets tcp/udp mode  
 *
 * @param self	unused
 * @param args	list of arguments:
 *			- C object pointer given by dev_import
 *			- "tcp" | "udp"
 *			
 * @return Py_None: 		OK
 *		NULL:			error	
 */
static PyObject* esrf_tcpudp(PyObject *self, PyObject *args)
{
	PyObject 	*py_ds_pt;
	devserver 	*ds;
	char 		*mymode;
	long 		protocol;
   
/* 
 * get pointer on device server and mode
 */
	if (!PyArg_ParseTuple(args,"Os",&py_ds_pt,&mymode))
		onError("usage: esrf_tcpudp(<c_object>,\"tcp\"|\"udp\"");

	ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
   
	if (!strcmp(mymode, "tcp"))
	{
		if (flag != 0)
			printf("-- esrf_tcpudp: setting mode tcp \n");
		protocol = D_TCP;
	}
	else if (!strcmp(mymode, "udp"))
	{
		if (flag != 0)
			printf("-- esrf_tcpudp: setting mode udp \n");
		protocol = D_UDP;
	}
	else 
		onError("usage: esrf_tcpudp(<c_object>,\"tcp\"|\"udp\"");
      
	if (dev_rpc_protocol(*ds, protocol, &error) != 0)
	{
		printf("error on dev_rpc_protocol\n");
		onError("error on dev_rpc_protocol");
	}

	if (flag != 0)
		printf("-- esrf_tcpudp ok\n");   
	Py_INCREF(Py_None);
	return Py_None;
}


/**
 * Sets timeout for device 
 *
 * @param self unused
 * @param args list of arguments:
 * 			- C object pointer given by dev_import
 *			- time for setting timeout (optional)
 *			
 * @return timeout (read or set) 	if ok
 *		NULL:			error	
 */ 
static PyObject *esrf_timeout(PyObject *self, PyObject *args)
{
	PyObject 	*py_ds_pt;
	devserver 	*ds;
	float 		mytime = -1;
	struct timeval 	dev_timeout;
	float 		secval;

/* 
 * get pointer on device server and timeout value
 */
	if (!PyArg_ParseTuple(args,"O|f",&py_ds_pt,&mytime))
		onError("usage: esrf_timeout(<c_object>[,<time_in_sec>]");

	ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);

	if (mytime < 0)
	{
		if (flag != 0)
			printf("-- esrf_timeout: read mode\n");
		if (dev_rpc_timeout(*ds,CLGET_TIMEOUT,&dev_timeout,&error) != 0)
		{
			printf("-- esrf_timeout: error dev_rpc_timeout\n");
			onError("-- esrf_timeout: error dev_rpc_timeout\n");
		} 
		else
		{
			secval = (float) dev_timeout.tv_sec + (float) dev_timeout.tv_usec*1e-6;
			if (flag != 0)
				printf("-- esrf_timeout: %f OK\n",secval);
			return Py_BuildValue("f",secval);
		}     
	} 
	else
	{
/* 
 * set mode choosen 
 */
		dev_timeout.tv_sec = (unsigned long)(int)mytime;
		dev_timeout.tv_usec = (long) ((mytime - (int)mytime) * 1e6);  
		if (flag != 0)
			printf("-- esrf_timeout: set mode : %d %d\n",dev_timeout.tv_sec, dev_timeout.tv_usec);
		if (dev_rpc_timeout(*ds,CLSET_TIMEOUT,&dev_timeout,&error) != 0)
		{
			printf("-- esrf_timeout: error dev_rpc_timeout\n");
			onError("-- esrf_timeout: error dev_rpc_timeout\n");
		} 
		else
		{
			if (flag != 0)
				printf("-- esrf_timeout: OK\n");
			return Py_BuildValue("f",mytime);
		}   
	}     
}

/**
 * Retrieve from database the list of devices. User may provide a filter
 *
 * @param self	unused
 * @param args	list of arguments:
 *				- filter (optional)
 *
 * @returns [list of devices] 	if ok
 *		NULL:			error	
 */
static PyObject *esrf_getdevlist(PyObject *self,PyObject *args)
{
	int 		i;
	PyObject	*mylist,
			*item;
	char 		*filter = "*/*/*",
			**tab;
	unsigned int 	dev_num;
	long 		ret;

	if (flag != 0)
		printf("-- esrf_getdevlist: enter\n");

	if (!PyArg_ParseTuple(args,"|s",&filter))
		onError("usage: esrf_getdevlist([**/**/**])")

	if (flag != 0)
		printf("-- esrf_getdevlist: %s\n",filter);

	if (db_imported == 0)
	{
		if (db_import(&error) != 0)
		{
			printf("-- esrf_getdevlist: error on db_import");
			onError("-- esrf_getdevlist: error on db_import");
		}
		db_imported = 1;
	}
   
	if (db_getdevexp(filter,&tab,&dev_num,&error) != 0)
	{
		printf("-- esrf_getdevlist: error on db_getdevexp : %s\n", dev_error_str(error));
		onError("-- esrf_getdevlist: error on db_getdevexp");   
	}

	if ( (mylist = PyList_New(dev_num)) == NULL)
	{
		db_freedevexp(tab); 
         	onError("cannot build list for devices");
	}
      
	for (i=0; i<dev_num; i++)
	{
		if ( (item = PyString_FromString(tab[i])) == NULL)
		{
			db_freedevexp(tab); 
			onError("cannot build list of devices");
		}
		if (PyList_SetItem(mylist,i,item) == - 1)
		{
			db_freedevexp(tab); 
			onError("cannot build command descriptors list"); 
		}
	}
      
/* 
 * now free the data 
 */
	db_freedevexp(tab); 
           
	if (flag != 0)
		printf("-- esrf_getdevlist: access to db OK\n");
	return mylist;
}

/**
 * provides with the string corresponding to the command number
 *
 * @param cmd cmd number
 * @param mystring string (allocated by caller)
 *
 * @return DS_OK or DS_NOTOK in case of error
 */
int cmd2string(long cmd, char **mystring)
{
	char		res_path[LONG_NAME_SIZE],
   			res_name[SHORT_NAME_SIZE];
	db_resource 	res_tab;
	unsigned short 	team,
   			server,
			cmds_ident;
	unsigned long 	cmd_number_mask = 0x3ffff;
	long		error;
	char		*ret_str = NULL;

/* 
 * Decode the command nuber into the fields: team, server and cmds_ident.
 */
	team   = (unsigned short)(cmd >> DS_TEAM_SHIFT);
	team   = team & DS_TEAM_MASK;
	server = (unsigned short)(cmd >> DS_IDENT_SHIFT);
	server = server & DS_IDENT_MASK;
	cmds_ident = (unsigned short)(cmd & cmd_number_mask);

/*
 * use default nethost
*/
	snprintf(res_path, sizeof(res_path), "CMDS/%d/%d", team, server);
	snprintf(res_name, sizeof(res_name), "%d", cmds_ident);

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = mystring;

/*
 * Read the command name string from the database.
 */
	if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
	{
		printf("get_cmd_string() : db_getresource failed with error %d\n", error);
		return(DS_NOTOK);
	}
	if (flag != 0)
		printf("%s\n",*mystring);
	return(DS_OK);
}

/**
 * free memory for allocated input arguments
 *
 * @param ds_in argument type
 * @param ds_argin argument
 * @param is_in_an_array array indication
 *
 * @return DS_OK or DS_NOTOK in case of error
 */
long free_argin (long ds_in, DevArgument ds_argin, long is_in_an_array)
{   
	if (ds_in != D_VOID_TYPE)
	{
		if (is_in_an_array == 1)
		{
			free ( ((DevVarCharArray *)ds_argin)->sequence);
		}
		free (ds_argin); 		 
	}
	return (DS_OK);    
}      
      
          

/**
 * call mydev_xdrfree if not void and free allocated pointer
 * 
 * @param ds_in argument type
 * @param ds_argin argument
 */
void free_argout (long ds_in,DevArgument ds_argin)
{
	long error;

	if (ds_in != D_VOID_TYPE) 
	{   
		if (flag != 0)
			printf("   calling dev_xdrfree with %d\n",ds_in);
		dev_xdrfree((DevType)ds_in, ds_argin, &error);
		free (ds_argin);
	}
}

/**
 * execute commands 
 * 
 * @param self	unused
 * @param args	list of arguments:
 *				- C_device_pointer
 *				- command string
 *				- command number
 *				- input type
 *				- output_type
 *				- input parameter
 * @param kwarg	dictionary (for output with keywords)
 *			
 * @return Python object according to type:  if ok
 *		NULL:				 error	
 */
static PyObject* esrf_io(PyObject *self, PyObject *args, PyObject *kwarg)
{
	int 		i,
   			dimension[1];
	PyObject 	*mytuple,
			*mylist,
			*item,
			*py_ds_pt = NULL;	/* device server pointer */
	static devserver *ds;
	static datco 	*dc;
	static char 	*ds_command = NULL,	/* command string */
   			*myouttype = NULL;	/* argument outtype= */
	static long 	ds_cmd = 0,		/* command number */
			ds_in = 0,		/* input type number */
			ds_out;			/* output type number */
#ifdef NUMPY
	PyArrayObject 	*myarray = NULL;	/* argument out= */
#else
	PyObject 	*myarray = NULL;
#endif
	char 		*array_2py = NULL,	/* to generate numeric for python */
			mymsg[100];
	long 		ret, 
			error;
	static DevDataListEntry 	data_type;
	DevArgument 	ds_argin,
			ds_argout;
	long 		is_in_an_array = 0,
			is_in_a_single = 0,
			is_in_a_special = 0,
			is_out_an_array = 0,
			is_out_a_single = 0,
			is_out_a_special = 0,
   			out_provided = 0,
   			type_num,
			mylen,
			flag_dc;
   
/* 
 * defining names for the function arguments 
*/  
	static char *kwlist[] = {"cds", "cmds", "cmd", "typein", "typeout", "dc", "parin", "out", "outtype", NULL};
	     	
/* 
 * init optional param to default values 
 */
	mytuple  = glob_tuple;

	py_ds_pt = NULL;
	ds_command = NULL;
	ds_cmd = 0;
	ds_in = 0;
	ds_out = 0;
	myarray = NULL;
	myarray = NULL;
	myouttype = NULL;
	flag_dc = 0;
   
	if (flag != 0)
		printf("-- esrf_io: enter\n");
      
/* 
 * get input arguments
 */
	if (!PyArg_ParseTupleAndKeywords(args, kwarg, "OsiiiiO|Os", kwlist,
		&py_ds_pt,&ds_command,&ds_cmd,&ds_in,&ds_out,&flag_dc, &mytuple,&myarray,&myouttype))
	{
		printf("usage:esrf_io(<C_obj>,<cmd_string>,<cmd>,<in_type>,<out_type>,<dc_flag>,<input_param>[,<out=xx>][,<outtype=xx>]\n");
		onError("usage: esrf_io(<C_obj>,<cmd_string>,<cmd>,<in_type>,<out_type>,<dc_flag>,<input_param>[,<out=xx>][,<outtype=xx>]");
	}

#ifdef NUMPY
/* 
 * out=xx or outtype=xx 
 */
	if ( (myarray != NULL) && (myouttype != NULL) )
	{
		printf("cannot use out= AND outtype= at same time\n");
		onError("cannot use out= AND outtype= at same time");
	}   
	if (myarray != NULL) 
	{
		if (flag != 0)
			printf("   key 'out' found\n");
		out_provided = 1;
	}
	if (myouttype != NULL)
	{
		for (i = 0; i < strlen(myouttype); i++)
			tolower(myouttype[i]);
		if (flag != 0)
			printf("   key 'outtype' found :%s\n",myouttype);
		out_provided = 2;
	}
#else
	if ( (myarray != NULL) || (myouttype != NULL) )
		printf("Not working with NUMERIC types: out= and outtype= ignored\n");
#endif

/* 
 * get device server pointer 
 */ 
	if (flag_dc == 0)
		ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
	else
		dc = (datco *) PyCObject_AsVoidPtr(py_ds_pt);
      
	if (flag != 0)
		printf("   server command: %s : cmd = %d, in = %d, out = %d\n", ds_command,ds_cmd,ds_in,ds_out); 

/* 
 * the auxilliary arguments should be in a tuple, maybe empty check this type 
 */
	if (PyTuple_Check(mytuple) != 1)
	{
		printf("auxilliary arguments should be in a tuple.\n");
		onError("auxilliary arguments should be in a tuple.")
	}
      
/* 
 * check the VOID case. we should have an argument if not VOID 
 */
	if ( (ds_in != D_VOID_TYPE ) && (PyTuple_Size(mytuple) == 0))
	{
		printf("missing input parameter(s)\n");
		onError("missing input parameter(s)");
	} 
   
	if ( (ds_in == D_VOID_TYPE ) && (PyTuple_Size(mytuple) !=0))
	{
		printf("got input parameters when expecting no\n");
		onError("got input parameters when expecting no");
	}

/* 
 * check on argin type 
 */
	if (check_type(ds_in,&is_in_a_single,&is_in_an_array,&is_in_a_special) != 0)
	{
		printf("unknown input argument type\n");
		onError("unknown input argument type")   
	}
   
/* 
 * check on argout type 
 */
	if (check_type(ds_out,&is_out_a_single,&is_out_an_array,&is_out_a_special) != 0)
	{
		printf("unknown input argument type\n");
		onError("unknown input argument type")   
	} 
	if ((is_out_an_array == 0) && (out_provided !=0)) 
	{
/* 
 * argout can't be but in a numeric array 
 */
		printf("Only taco argout arrays can be stored in python Numeric arrays !!\n");
		onError("Only taco argout arrays can be stored in python Numeric arrays !!")
	} 

/* 
 * prepare empty input argin for dev_putget
 */
	if (ds_in != D_VOID_TYPE)
	{
/* 
 * get the information structure for the input 
 */
		if (xdr_get_type(ds_in,&data_type,&error) == DS_NOTOK)
		{
			printf("   xdr_get_type error : (%d) for type IN %d\n", error,ds_in);
			onError("xdr_get_type error");
		}
   
/* 
 * allocate memory, then fill it
 */
		ds_argin = (char*) malloc((unsigned int)data_type.size);
		if (ds_argin == NULL)
		{
			printf("   malloc: insufficient memory for argin\n");
			onError("malloc: insufficient memory for argin");
		}
		memset(ds_argin,0,(size_t)data_type.size);
	}
	else
		ds_argin = NULL;
      
/* 
 * Now, fill the argin 
 *
 *  "single" type 
 */
	if (is_in_a_single == 1)
	{ 
/* 
 * check that only one item in the tuple 
 */    
		if (PyTuple_Size(mytuple) != 1)
		{
			printf("one single item requested as argin\n");
/* 
 * free malloc 
 */
			free (ds_argin);
			onError("one single item requested as argin");
		}
	
/* 
 * retreive this item 
 */ 
		if ( (item = PyTuple_GetItem(mytuple,0)) == NULL)
		{
			printf("cannot get item argin\n");
/* 
 * free malloc 
 */
			free (ds_argin);
			onError("cannot get item argin");
		}	
       	
/* 
 * convert python object in ds_argin 
 */ 
		if ( (ret = get_argin_single(ds_argin,ds_in,item,mymsg)) != 0)
		{
			free (ds_argin);	    
			onError(mymsg);
		}
		if (flag != 0)
			display_single(ds_argin,ds_in,"IN");      
	}    

/* 
 * "array" type 
 */
	if (is_in_an_array == 1)
	{
/* 
 * convert python object in ds_argin 
 */ 
		if ( (ret = get_argin_array(ds_argin,ds_in,mytuple,mymsg)) != 0)
		{
			printf(mymsg);
			printf("\n");
			free (ds_argin);		   
			onError(mymsg); 
		}     
		if (flag != 0)
			display_array(ds_argin,ds_in,"IN");
	}
   
/* 
 * special type 
 */
	if (is_in_a_special == 1)
	{
/* 
 * convert python object in ds_argin
 */ 
		if ( (ret = get_argin_special(ds_argin,ds_in,mytuple,mymsg)) != 0)
		{
			printf(mymsg);
			printf("\n");
			free (ds_argin);
			onError(mymsg); 
		}  
		if (flag != 0)
			display_special(ds_argin,ds_in,"IN");
	}
   
/* 
 * prepare empty output argin for dev_putget 
 */
	if (ds_out != D_VOID_TYPE)
	{
/* 
 * get the information structure for the output 
 */
		if (xdr_get_type(ds_out,&data_type,&error) == DS_NOTOK )
		{
			printf("   xdr_get_type error : (%d) for type OUT %d\n", error,ds_out);
			free_argin ((DevType)ds_in, ds_argin, is_in_an_array);
			onError("xdr_get_type error");
		}
   
/*
 * allocate memory, then fill it
 */
		ds_argout = (char*) malloc((unsigned int)data_type.size);
		if (ds_argout == NULL)
		{
			printf("   malloc: insufficient memory for argout\n");
			free_argin ((DevType)ds_in, ds_argin, is_in_an_array);
			onError("malloc: insufficient memory for argout");
		}
		memset(ds_argout,0,(size_t)data_type.size);
	}
	else
		ds_argout = NULL;
      
/* 
 * if out_provided, check that argout is an array 
 */
	if (((out_provided == 1) || (out_provided == 2)) && (is_out_an_array == 0))
	{
/* 
 * ignore out ? or deliver an error ? 
 */
		printf("   argout type is not an array\n");
		onError("   argout type is not an array");
	}
   
#ifdef NUMPY   
/* 
 * if out_provided with out=xx, check type 
 */
	if (out_provided == 1)
	{
/* 
 * first check that array type is correct 
 */
		if (check_provided(myarray->descr->type_num,ds_out) != 0)
		{
			printf("array type and server type not compatible\n");
			onError("array type and server type not compatible")
		}
		((DevVarCharArray*)(ds_argout))->sequence = (char *)(myarray->data);
	}    
#endif
   
	if (flag_dc == 0)
	{
/* 
 * dev_putget call 
 */
		if (flag != 0)
			printf("   calling dev_putget\n");
		ret = dev_putget(*ds,ds_cmd,ds_argin,ds_in,ds_argout,ds_out,&error);
		if (flag != 0)
			printf("   from dev_putget (%d)\n",ret);
	}
	else
	{
/* 
 * data collector call 
 */
		if (flag != 0)
			printf("   calling dc_devget\n");
		ret = dc_devget(dc,ds_cmd,ds_argout,ds_out,&error);
		if (flag != 0)
			printf("   from dc_devget (%d)\n",ret);   
	}
   
/* 
 * free argin 
 */
	free_argin ((DevType)ds_in, ds_argin, is_in_an_array);
        
	if (ret != 0)
	{
		printf("-- esrf_io: error: (%d) %s\n",error,dev_error_str(error));    
		free_argout ((DevType)ds_out, ds_argout);
		onError("esrf_io: error on dev_putget");
	}
       
/* 
 * Now, get the argout 
 */
	if (out_provided == 0)
	{
/*
 * "single" type
 */
		if (is_out_a_single == 1)
		{ 
/* 
 * convert python object in ds_argout 
 */ 
			if ( (ret = get_argout_single(ds_argout,ds_out,&item,mymsg)) != 0)
			{
				free_argout ((DevType)ds_out, ds_argout);
				onError(mymsg);
			}
			if (flag != 0)
				display_single(ds_argout,ds_out,"OUT");	 
	
/* 
 * free argout 
 */
			free_argout ((DevType)ds_out, ds_argout);

			if (flag != 0)
				printf("-- esrf_io: OK\n");
			return item;      
		}    

/*
 * "array" type
 */
		if (is_out_an_array == 1)
		{
/*
 * convert python object in ds_argout
 */ 
			if ((ret = get_argout_array(ds_argout,ds_out,&mylist,mymsg)) != 0)
			{
				free_argout ((DevType)ds_out, ds_argout);
				onError(mymsg);      
			}
			if (flag != 0)
				display_array(ds_argout,ds_out,"OUT");      

/* 
 * free argout 
 */
			free_argout ((DevType)ds_out, ds_argout);
			if (flag != 0)
				printf("-- esrf_io: OK\n");
			return mylist;     
		}
   
/* 
 * "special" type 
 */
		if (is_out_a_special == 1)
		{
/* 
 * convert python object in ds_argout 
 */ 
			if ((ret = get_argout_special(ds_argout,ds_out,&mylist,mymsg)) != 0)
			{
				free_argout ((DevType)ds_out, ds_argout);
				onError(mymsg);      
			}
			if (flag != 0)
				display_special(ds_argout,ds_out,"OUT");
/* 
 * free argout
 */
			free_argout ((DevType)ds_out, ds_argout);
			if (flag != 0)
				printf("-- esrf_io: OK\n");
			return mylist; 
		}   
  
/* 
 * in case of D_VOID_TYPE 
*/         
		if (flag != 0)
			printf("-- esrf_io: OK\n");

		Py_INCREF(Py_None);      
		return Py_None;
	}

#ifdef NUMPY   
/* 
 * out=xx 
 */
	else if (out_provided == 1)
	{
/* 
 * myarray->data has been filled. we return the number of elements set in the array
 */
		if (flag != 0)
		{
			display_array(ds_argout,ds_out,"OUT");
			printf("-- esrf_io: OK\n");
		}
		return(Py_BuildValue("(l)",((DevVarCharArray*)(ds_argout))->length));
	} 
   
/* 
 * outtype=numeric 
 */
	else
	{
/* 
 * determine numeric type for this taco type 
 */
		type_num = Ctype2numeric(ds_out); 
		if (type_num == -1)
		{
			printf("cannot create Numeric type from taco type %d\n",ds_out);
			free_argout ((DevType)ds_out, ds_argout);
			onError("cannot create Numeric type from taco type")
		}
            
/* 
 * copy the argout->sequence to another array 
 *
 * first allocate 
 */
		if ( (mylen = lenoftype(ds_out)) == -1)
		{
			printf("lenoftype: cannot calculate output single element size\n");
			free_argout ((DevType)ds_out, ds_argout);
			onError("lenoftype: cannot calculate output single element size")      
		}
		array_2py = (char *) malloc( ((DevVarCharArray*)(ds_argout))->length*mylen);
/* 
 * then copy
 */
		memcpy((void *)array_2py, (void *)(((DevVarCharArray*)(ds_argout))->sequence), ((DevVarCharArray*)(ds_argout))->length*mylen);
      
/* 
 * now, generate the Numeric array
 */
		dimension[0]= (int)(((DevVarCharArray*)(ds_argout))->length);
		myarray = (PyArrayObject *) PyArray_FromDimsAndData(1, dimension, (int) type_num, array_2py);

		free_argout ((DevType)ds_out, ds_argout);
		free (array_2py);
		if (flag != 0)
			printf("-- esrf_io: OK\n");
	 	 	       
		return PyArray_Return(myarray);
	}
#endif
}

/**
 * frees a data collector device 
 * 
 * @param self:	unused
 */ 
void esrf_dc_free(void *self)
{
	static dc_dev_free *dc_devfree;
   
	if (flag != 0)
	printf("-- esrf_dc_free: enter\n");

	dc_devfree = (dc_dev_free *)malloc(sizeof(dc_dev_free));
	dc_devfree[0].dc_ptr = (datco *) self;
	dc_devfree[0].dc_dev_error = (long*) malloc(sizeof(long));

	devstatus = dc_free(dc_devfree,1,&error);

	if (devstatus != 0) 
		printf("esrf_dc_free failed: %s\n", dev_error_str(*(dc_devfree[0].dc_dev_error)));
	else if (flag != 0)
		printf("-- esrf_dc_free: OK\n");

	if (dc_devfree[0].dc_dev_error != NULL)
		free(dc_devfree[0].dc_dev_error);
	if (dc_devfree != NULL)
		free(dc_devfree);	 
}

/**
 * Imports a device from data collector
 *
 * @param self unused
 * @param args list of arguments:
 *				- device name (in)
 *			
 * @return datco C pointer:	no error
 *		NULL:				error	
 */ 
static PyObject *esrf_dc_import(PyObject *self,PyObject *args)
{
	char 			*devname;
	dc_dev_imp 		*dc_devimp;
	static datco 		*mydc_ptr;
	static dc_dev_free	*dc_devfree;

/* 
 * get the device name
 */
	if (!PyArg_ParseTuple(args,"s",&devname))
		onError("usage: esrf_dc_import (<device_name>)")

	if (flag != 0)
		printf("-- esrf_dc_import: devname = %s\n",devname); 

/*
 * create structure for call
 */
	dc_devimp = (dc_dev_imp*)malloc(sizeof(dc_dev_imp));
	if (dc_devimp == NULL)
	{
		printf("esrf_dc_import: cannot allocate memory for dc access\n");
		onError("esrf_dc_import: cannot allocate memory for dc access");
	}
	dc_devimp[0].device_name = (char*)malloc(strlen(devname) + 1);
	strcpy(dc_devimp[0].device_name,devname);
	dc_devimp[0].dc_ptr = NULL;
	dc_devimp[0].dc_dev_error = (long*) malloc(sizeof(long));
   
	devstatus = dc_import((dc_dev_imp *)dc_devimp,1,&error);

	free(dc_devimp[0].device_name);
   
	if (devstatus != 0)
	{
	printf("%s\n",dev_error_str(error));
	printf("%s\n",dev_error_str(*(dc_devimp[0].dc_dev_error)));
/* 
 * if any 
 */
	if (dc_devimp[0].dc_ptr != NULL)
	{
		dc_devfree = (dc_dev_free *)malloc(sizeof(dc_dev_free));
		dc_devfree[0].dc_ptr = dc_devimp[0].dc_ptr;
		dc_free((dc_dev_free*)dc_devfree,1,&error);
		if (dc_devfree[0].dc_dev_error != NULL)
			free(dc_devfree[0].dc_dev_error);
		free(dc_devfree);
		}
		if (dc_devimp[0].dc_dev_error != NULL)
			free(dc_devimp[0].dc_dev_error);
		free(dc_devimp);
		onError("import failed in dc_import");
	}

/* 
 * save datco to give it to python 
 */
	mydc_ptr = dc_devimp[0].dc_ptr;

/* 
 * free everything but datco
 */
	if (dc_devimp[0].dc_dev_error != NULL) 
		free(dc_devimp[0].dc_dev_error);
	if(dc_devimp != 0)
		free(dc_devimp);   
   
	return PyCObject_FromVoidPtr((void *)mydc_ptr, (void *)esrf_dc_free);   
}

/**
 * Provides the list of devices defined in a data collector
 *
 * @param self	unused
 * @param args	device name or nothing
 *				  
 * @return if OK:
 *		   [list of devices] 	if no arg
 *		   [[cmd, data_type],[cmd,data_type],...]  if arg
 *		NULL:			error	
 */
static PyObject *esrf_dc_info(PyObject *self, PyObject *args)
{
	PyObject	*mylist,
			*mydict,
			*mylistcmd,
			*item;
	long 		ret,
			error,
			i;
	int 		dev_n;
	char 		*mydev=NULL,
			*myhost=NULL,
   			*mystring,
			**devnametab;
	dc_devinf 	dc_dev_info;

	if (flag != 0)
		printf("-- esrf_dc_info: enter\n");

	if (!PyArg_ParseTuple(args,"|s",&mydev))
		onError("usage: esrf_dc_info([device])")

/* 
 * if no device provided, dc_devall required get the NETHOST env variable
 */
	if (mydev == NULL)
	{
		myhost = getenv("NETHOST");
		if (myhost == NULL) 	/* not possible */
			onError("no NETHOST variable set")
   
		if (flag != 0)
			printf("-- esrf_dc_devall on host %s\n",myhost);

		if (dc_devall(myhost,&devnametab,&dev_n,&error) != 0)
		{
			printf("esrf_dc_info: error in dc_devall (%d)\n",error);
			onError("esrf_dc_info: error in dc_devall")
		}
		else
		{
			if ( (mylist = PyList_New(dev_n)) == NULL)
				onError("cannot build list for devices");
      
			for (i = 0; i < dev_n; i++)
			{
				if ( (item = PyString_FromString(devnametab[i])) == NULL)
					onError("cannot build device name");
				if (PyList_SetItem(mylist,i,item) == - 1)
					onError("cannot build device name list");      
            			free(devnametab[i]); 
			}
/* 
 * now free
 */
			free(devnametab);
           
			if (flag != 0)
				printf("-- esrf_dc_info: OK\n");
			return mylist;   
		}
	}
	else
	{
/* 
 * dc_dinfo required for a device
 */
		if (dc_dinfo(mydev,&dc_dev_info,&error) != 0)
		{
			printf("esrf_dc_info: error in dc_dinfo (%d)\n",error);
			onError("esrf_dc_info: error in dc_dinfo")      
		}
		else
		{
			if ( (mydict = PyDict_New()) == NULL)
				onError("cannot build dict for commands");      
      
			for (i=0; i<dc_dev_info.devinf_nbcmd; i++)
			{
				if ( (mylistcmd = PyList_New(2)) == NULL)
					onError("cannot build list for [command,type]");  
				if ( (item = PyInt_FromLong((long)dc_dev_info.devcmd[i].devinf_cmd)) == NULL)
					onError("cannot build command item");
				if (PyList_SetItem(mylistcmd,0,item) == - 1)
					onError("cannot set command item in [command,type] list");   
				if ( (item = PyInt_FromLong((long)dc_dev_info.devcmd[i].devinf_argout)) == NULL)
					onError("cannot build argout type item");
				if (PyList_SetItem(mylistcmd,1,item) == - 1)
					onError("cannot set type item in [command,type] list");   	 

				cmd2string(dc_dev_info.devcmd[i].devinf_cmd,&mystring);
				if (PyMapping_SetItemString(mydict,mystring,mylistcmd) == - 1)
					onError("cannot set [command,type] item in the list");	 
				free(mystring);           
			}
			if (flag != 0)
				printf("-- esrf_dc_info: OK\n");
			return mydict;   	    
		}      
	}
}


/**
 * Array for functions mapping 
 */
static struct PyMethodDef Taco_methods[] = {
	{"esrf_import",		esrf_import,	1},
	{"esrf_query",		esrf_query,	1},
	{"esrf_tcpudp",		esrf_tcpudp,	1},
	{"esrf_timeout",	esrf_timeout,	1},
	{"esrf_getdevlist",	esrf_getdevlist,1},
	{"esrf_getresource",	esrf_getresource,1},
	{"esrf_putresource",	esrf_putresource,1},
	{"esrf_delresource",	esrf_delresource,1},
	{"esrf_io",		(PyCFunction)esrf_io, 1|2},
	{"esrf_debug",		esrf_debug, 	1},
	{"esrf_dc_info",	esrf_dc_info, 1},
	{"esrf_dc_import",	esrf_dc_import, 1},
	{NULL,	NULL}
};


/**
 * init function
 */
void initTaco()
{
	PyObject *m, *d;
    
	m = Py_InitModule("Taco", Taco_methods);
#ifdef NUMPY
	import_array();
#endif
	d = PyModule_GetDict(m);
	ErrorObject = Py_BuildValue("s", "Taco.error");
	PyDict_SetItemString(d, "error", ErrorObject);
    
	if (PyErr_Occurred())
	Py_FatalError("Can't initialize module Taco");

/* 
 * we create here a global dictionary and a global tuple for  esrf_io further init parameters 
 */
	if ((glob_tuple = PyList_New(0)) == NULL)
		printf("initTaco: cannot create global tuple\n");
    
	if ( (glob_dict = PyDict_New()) == NULL)
		printf("Cannot create global dict\n");
	if (flag)
		printf("Taco module init\n");
}
