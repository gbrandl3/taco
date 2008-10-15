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
 * File:        taco.c
 *
 * Description: interface python - Taco
 *
 * Author(s):   MCD
 *		$Author: jkrueger1 $
 *
 * Original:    December 99
 * 
 * Date:	$Date: 2008-10-15 14:52:49 $
 *
 * Version:	$Revision: 1.11 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef HAVE_FSTAT
#undef HAVE_FSTAT
#endif

#include <Python.h>

#include <ctype.h>

/* for taco */
#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <atte_xdr.h>
#include <grp_xdr.h>
#include <maxe_xdr.h>

#ifdef NUMPY
#	if HAVE_NUMARRAY_ARRAYOBJECT_H
#		include <numarray/arrayobject.h>
#	elif HAVE_NUMERIC_ARRAYOBJECT_H
#		include <Numeric/arrayobject.h>
#	elif HAVE_NUMPY_ARRAYOBJECT_H
#		include <numpy/arrayobject.h>
#	endif
#endif

#include <taco.h>

extern long     debug_flag;
static long	flag=0;

static PyObject *ErrorObject;

#define onError(message) \
{PyErr_SetString(ErrorObject,message);return NULL;}

static short devstatus;
static DevLong error;
static long readwrite = 0;
static long db_imported = 0;
static PyObject *glob_tuple;
static PyObject *glob_dict;

extern long check_provided(long typenum,long ds_out);
      				      		      
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
   else
      if (flag != 0)
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
   char *devname;
   static devserver *ds_pt;

   /* get the device name */
   if (!PyArg_ParseTuple(args,"s",&devname))
      onError("usage: esrf_import (<device_name>)")
      
   if (flag != 0)
      printf("-- esrf_import: devname = %s\n",devname); 

   ds_pt = malloc(sizeof(devserver));
  devstatus = dev_import(devname,readwrite,ds_pt,&error);
/* for test */
   /* devstatus = dev_import(devname,10,ds_pt,&error);*/
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
   /* get the flag value */
   if (!PyArg_ParseTuple(args,"d",&flag))
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
   int i;
   PyObject *py_ds_pt;
   static DevVarCmdArray varcmdarr;
   PyObject* mydict;
   PyObject* mylist;
   PyObject* item;
   static devserver *ds;

   if (flag != 0)
       printf("-- esrf_query: enter\n");
   /* get pointer on device server */
   if (!PyArg_ParseTuple(args,"O",&py_ds_pt))
      onError("usage: esrf_query(<c_object>)")

   ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
   devstatus = dev_cmd_query(*ds,&varcmdarr,&error);
   if (devstatus != 0) 
      {
      printf ("Taco Error : %s", dev_error_str(error));
      onError("esrf_query failed ");
      }

/*  build the output dictionnary  */
   if ( (mydict = PyDict_New()) == NULL)
      onError("cannot build commands dictionnary");
       
   for (i=0; i<varcmdarr.length; i++)
   {
      /*  build the output list of command descriptors */
      if ( (mylist = PyList_New(3)) == NULL)
         onError("cannot build command descriptors list");
      /* cmd */
      if ( (item = PyInt_FromLong(varcmdarr.sequence[i].cmd)) == NULL)
            onError("cannot build command descriptors list");
      if (PyList_SetItem(mylist,0,item) == - 1)
            onError("cannot build command descriptors list");
      /* in_type */
      if ( (item = PyInt_FromLong(varcmdarr.sequence[i].in_type)) == NULL)
            onError("cannot build command descriptors list");
      if (PyList_SetItem(mylist,1,item) == -1)
            onError("cannot build command descriptors list");
      /* out_type */
      if ( (item = PyInt_FromLong(varcmdarr.sequence[i].out_type)) == NULL)
            onError("cannot build command descriptors list");
      if (PyList_SetItem(mylist,2,item) == -1)
            onError("cannot build command descriptors list");

      if
      (PyMapping_SetItemString(mydict,varcmdarr.sequence[i].cmd_name,mylist)==-1)
            onError("cannot build command descriptors list in dictionnary");
   }   
      
   if (flag != 0)
      printf("-- esrf_query: ok\n");
   
   return mydict;
}

/**
 * Asks for device resource 
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
   char *devname;
   char *resname;
   db_resource pt_res;
   static char mystring[200];
   unsigned int num_res;
   DevLong error;
   int ret;

   static char* res_string = "";
   DevVarStringArray	res_array;
   int i;
   PyObject *item;
   PyObject *mylist;

   res_array.length    = 0;
   res_array.sequence = NULL;
         

   if (flag != 0)
      printf("-- esrf_getresource: enter\n");
   if (!PyArg_ParseTuple(args,"ss",&devname,&resname))
      onError("usage: esrf_getresource(<devname>,<resname>)")
      
   if (flag != 0)
      printf("device name: %s\n",devname);
   if (flag != 0)
      printf("resource name: %s\n",resname);
   
   /*
    * Read resources always as string array!
    */

   pt_res.resource_name = resname;
   pt_res.resource_type = D_VAR_STRINGARR;
   pt_res.resource_adr  = &(res_array);

   num_res = 1;
   ret = db_getresource(devname,&pt_res,num_res,&error);
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

      if ( res_array.length == 0 )
	 {
         return(Py_BuildValue("s", res_string));
	 }
      
      /*
       * If only on value was found return it as a simple string
       */

      if (  res_array.length == 1 )
         {
         return(Py_BuildValue ("s", res_array.sequence[0]));

         /* free allocated memory */
	 free (res_array.sequence[0]);
	 free (res_array.sequence);
	 }
      else
         {
	 /*
          * If several resource values are read, return them as a
          * list of strings
  	  */

         if ( (mylist = PyList_New(res_array.length)) == NULL)
            {
            printf("error on db_getresource : cannot build list\n");
            onError("error on db_getresource");

            Py_INCREF(Py_None);
            return Py_None;
            }
         for (i=0; i<res_array.length; i++)
            {
            item = PyString_FromString( (char *) res_array.sequence[i]);
            PyList_SetItem (mylist,i,item);
            }

         /* free allocated memory */
         for (i=0; i<res_array.length; i++)
            {
	    free (res_array.sequence[i]);
	    }
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
   char *devname;
   char *resname;
   char *resval;
   db_resource pt_res;
   static char mystring[200];
   static char mystringval[200];
   static char *my_stringval;
   unsigned int num_res;
   DevLong error;
   int ret;
      
   if (flag != 0)
      printf("-- esrf_putresource: enter\n");
   if (!PyArg_ParseTuple(args,"sss",&devname,&resname,&resval))
      onError("usage: esrf_putresource(<devname>,<resname>,<value>)")
      
   if (flag != 0)
   {
      printf("device name: %s\n",devname);
      printf("resource name: %s\n",resname);
      printf("resource value: %s\n",resval);
   }
   
   pt_res.resource_name = mystring;
   strncpy(mystring, resname, sizeof(mystring) - 1);
   mystring[sizeof(mystring) - 1] = '\0';

   pt_res.resource_type = (short)D_STRING_TYPE;
   my_stringval = &(mystringval[0]);
   pt_res.resource_adr = (void *)(&(my_stringval));
   strncpy(mystringval, resval, sizeof(mystringval) - 1);
   mystringval[sizeof(mystringval) - 1] = '\0';

   num_res = 1;
   ret = db_putresource(devname,&pt_res,num_res,&error);
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
   char *devname;
   char *resname;
   int ret;
   unsigned int res_num;
   DevLong error;
         
   if (flag != 0)
      printf("-- esrf_delresource: enter\n");
   if (!PyArg_ParseTuple(args,"ss",&devname,&resname))
      onError("usage: esrf_delresource(<devname>,<resname>)")
      
   if (flag != 0)
   {
      printf("device name: %s\n",devname);
      printf("resource name: %s\n",resname);
   }

   res_num = 1;
   ret = db_delresource(devname,&resname,res_num,&error);
   
   if (ret != 0)
   {
      printf("error deleting resource for device %s: resource %s\n",
         devname,resname);
      onError("error deleting resource ")
   }
   else
   {
      Py_INCREF(Py_None);
      return(Py_None);
   }
   
}
/**************************************************

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
   PyObject *py_ds_pt;
   devserver *ds;
   char *mymode;
   long protocol,found;
   
   /* get pointer on device server and mode*/
   if (!PyArg_ParseTuple(args,"Os",&py_ds_pt,&mymode))
      onError("usage: esrf_tcpudp(<c_object>,\"tcp\"|\"udp\"");

   ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
   
   found = 0;
   if ( !strcmp(mymode,"tcp") )
   {
      if (flag != 0)
         printf("-- esrf_tcpudp: setting mode tcp \n");
      protocol = D_TCP;
      found = 1;
   }

   if ( !strcmp(mymode,"udp") )
   {
      if (flag != 0)
         printf("-- esrf_tcpudp: setting mode udp \n");
      protocol = D_UDP;
      found = 1;
   }
   
   if (found == 0)
      onError("usage: esrf_tcpudp(<c_object>,\"tcp\"|\"udp\"");
      
   if ( dev_rpc_protocol(*ds,protocol,&error) != 0)
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
   PyObject *py_ds_pt;
   devserver *ds;
   float mytime;
   struct timeval dev_timeout;
   float secval;

   mytime = -1;   
   /* get pointer on device server and timeout value*/
   if (!PyArg_ParseTuple(args,"O|f",&py_ds_pt,&mytime))
      onError("usage: esrf_timeout(<c_object>[,<time_in_sec>]");

   ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);

   if (mytime == -1)
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
      /* set mode choosen */
      dev_timeout.tv_sec = (unsigned long)(int)mytime;
      dev_timeout.tv_usec = (long) ((mytime - (int)mytime) * 1e6);  
      if (flag != 0)
         printf("-- esrf_timeout: set mode : %d %d\n",dev_timeout.tv_sec,
      						   dev_timeout.tv_usec);
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


   int i;
   PyObject* mylist;
   PyObject* item;
   char *server_name;
   char **tab;
   unsigned int dev_num;
   long ret;
	
   if (flag != 0)
      printf("-- esrf_getdevlist: enter\n");
		
	/* Parse the device server name */	
   if (!PyArg_ParseTuple(args,"s",&server_name))
		{
		printf("usage : esrf_getdevlist(<C_obj>,<server_name>)\n");
      onError("usage: esrf_getdevlist(<C_obj>,<server_name>)");		
		}
	
   if (flag != 0)
      printf("-- esrf_getdevlist: server = %s\n", server_name);
	
	
	/* import the database if its not yet done */
	if (db_imported == 0)
   	{
      if (db_import(&error) != 0)
      	{
         printf("-- esrf_getdevlist: error on db_import : \n%s\n", \
				dev_error_str(error));
	 		onError("-- esrf_getdevlist: error on db_import");
      	}
      db_imported = 1;
   	}
		
	/* Read the list of devices for the device server */				
	ret = db_getdevlist(server_name,&tab,&dev_num,&error);
   if (ret != 0)
   	{
      printf("-- esrf_getdevlist: error on db_getdevlist : \n%s\n",\
      	dev_error_str(error));
      onError("-- esrf_getdevlist: error on db_getdevlist");   
   	}
		
	/* Prepare the list to be returned to Python */			
   if ( ( mylist = PyList_New(dev_num) ) == NULL)
		{
      onError("cannot build list for devices");
		}
      
   for (i=0; i<dev_num; i++)
   	{
      if ( (item = PyString_FromString(tab[i]) ) == NULL)
            onError("cannot build string for device name");
      if ( PyList_SetItem(mylist,i,item ) == - 1)
            onError("cannot add device name string to the list");      
      }
  
   /* now free the allocated string array */
   db_freedevexp(tab);
           
       
   if (flag != 0)
      printf("-- esrf_getdevlist: Returning list of devices\n");
      
	return mylist;   	
}



/**************************************************

	esrf_getdevexp

Retrieve from database the list of exported devices.
The user can provide a fliter with wildcards (*).

	arguments:
		self:	unused
		args:	The filter for the list of exported devices as string	  
			
   returns: 
		[list of devices] 	if ok
		NULL:			error	
***************************************************/
static PyObject* 
esrf_getdevexp(PyObject *self,PyObject *args)

{
   int i;
   PyObject* mylist;
   PyObject* item;
   char *filter;
   char **tab;
   unsigned int dev_num;
   long ret;

   if (flag != 0)
      printf("-- esrf_getdevexp: enter\n");

   filter = "*/*/*";
   if (!PyArg_ParseTuple(args,"|s",&filter))
      onError("usage: esrf_getdevlist([**/**/**])")

   if (flag != 0)
      printf("-- esrf_getdevexp: %s\n",filter);

	/* import the database if its not yet done */
   if (db_imported == 0)
   	{
      if (db_import(&error) != 0)
      	{
         printf("-- esrf_getdevexp: error on db_import");
	 		onError("-- esrf_getdevlist: error on db_import");
      	}
      db_imported = 1;
   	}

   ret = db_getdevexp(filter,&tab,&dev_num,&error);
   
   if (ret != 0)
   	{
      printf("-- esrf_getdevexp: error on db_getdevexp : %s\n",\
      	dev_error_str(error));
      onError("-- esrf_getdevexp: error on db_getdevexp");   
   	}
		
   if ( (mylist = PyList_New(dev_num)) == NULL)
         onError("cannot build list for devices");
      
   for (i=0; i<dev_num; i++)
      {
      if ( (item = PyString_FromString(tab[i])) == NULL)
            onError("cannot build list of devices");
      if (PyList_SetItem(mylist,i,item) == - 1)
            onError("cannot build command descriptors list");      
      }
      
   /* now free the data */
   db_freedevexp(tab); 
           
   if (flag != 0)
         printf("-- esrf_getdevexp: access to db OK\n");
      
	return mylist;
}



/**************************************************

	cmd2string

provides with the string corresponding to the command
number

	arguments:
			cmd number
		        string (allocated by caller)
				  
			
        returns: 
	        0		OK
		-1		error	
***************************************************/
int cmd2string(long cmd,char **mystring)
{
   char			res_path[LONG_NAME_SIZE];
   char			res_name[SHORT_NAME_SIZE];
   db_resource 		res_tab;
   unsigned short 	team;
   unsigned short 	server;
   unsigned short 	cmds_ident;
   unsigned long 	cmd_number_mask = 0x3ffff;
   DevLong		error;
   char			*ret_str = NULL;

/* Decode the command nuber into the fields:
 * team, server and cmds_ident.
 */
   team   = (unsigned short)(cmd >> DS_TEAM_SHIFT);
   team   = team & DS_TEAM_MASK;
   server = (unsigned short)(cmd >> DS_IDENT_SHIFT);
   server = server & DS_IDENT_MASK;
   cmds_ident = (unsigned short)(cmd & cmd_number_mask);

/*
 * use default nethost
*/
   sprintf(res_path, "CMDS/%d/%d", team, server);

   sprintf (res_name, "%d", cmds_ident);

   res_tab.resource_name = res_name;
   res_tab.resource_type = D_STRING_TYPE;
   res_tab.resource_adr  = mystring;

/*
 * Read the command name string from the database.
 */

   if (db_getresource (res_path, &res_tab, 1, &error) == DS_NOTOK)
   {
      printf("get_cmd_string() : db_getresource failed with error %d\n", error);
      return(-1);
   }
   else
   {
      if (flag != 0)
         printf("%s\n",*mystring);
      return(0);
   }

}



/**************************************************************
      	free_argin
      
	free memory for allocated input arguments
      
      	Arguments:
		- ds_in: argument type
      		- ds_argin: argument
		- is_in_an_array: array indication
      
      	returns:
      		0: OK
      		-1: error
**************************************************************/
long free_argin (long ds_in, DevArgument ds_argin, long is_in_an_array)
{   
   if (ds_in != D_VOID_TYPE)
      {
      if (is_in_an_array == 1 || ds_in == D_OPAQUE_TYPE)
         {
	 		free ( ((DevVarCharArray *)ds_argin)->sequence);
	 		}
      free (ds_argin); 		 
      }
   return (0);    
}      
      
          

/**************************************************************
      	free_argout
      
	call mydev_xdrfree if not void and free allocated pointer
      
      	Arguments:
		- ds_in: argument type
      		- ds_argin: argument
      		- error: returned by dev_xdrfree
      
      	returns:
      		0: OK
      		-1: error
**************************************************************/
void free_argout (long ds_in,DevArgument ds_argin)
{
   DevLong error;

   if (ds_in != D_VOID_TYPE) {   
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
   int i;
   PyObject *mytuple;
   PyObject *mylist;
   PyObject *item;
   static devserver *ds;
   static datco *dc;
   PyObject *py_ds_pt = NULL;		/* device server pointer */
   static char *ds_command = NULL;	/* command string */
   static long ds_cmd = 0;		/* command number */
   static long ds_in = 0;		/* input type number */
   static long ds_out;			/* output type number */
#ifdef NUMPY
   PyArrayObject *myarray = NULL;	/* argument out= */
#else
   PyObject *myarray = NULL;
#endif
   static char *myouttype = NULL;	/* argument outtype= */
   char *array_2py=NULL;		/* to generate numeric for python */
   long ret;
   DevLong error;
   static DevDataListEntry data_type;
   DevArgument ds_argin,ds_argout;
   long is_in_an_array,is_in_a_single,is_in_a_special;
   long is_out_an_array,is_out_a_single,is_out_a_special;
   char mymsg[100];
   long out_provided=0;
   long type_num;
   int dimension[1];
   long mylen;
   long flag_dc;

   PyThreadState   *thread_state;
   
/* defining names for the function arguments */  

   static char *kwlist[] = 
        {"cds","cmds","cmd","typein","typeout","dc","parin","out","outtype",NULL};
	     	
   /* init optional param to default values */
   mytuple  = glob_tuple;
   is_in_an_array = 0;
   is_in_a_single = 0;
   is_in_a_special = 0;
   is_out_an_array = 0;
   is_out_a_single = 0;
   is_out_a_special = 0;

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
      
   /* get input arguments*/
   /*--------------------*/

   if (!PyArg_ParseTupleAndKeywords(args,kwarg,"OsiiiiO|Os",kwlist,
          &py_ds_pt,&ds_command,&ds_cmd,&ds_in,&ds_out,&flag_dc,
	  &mytuple,&myarray,&myouttype))
   {
      printf("usage:esrf_io(<C_obj>,<cmd_string>,<cmd>,<in_type>,<out_type>,<dc_flag>,<input_param>[,<out=xx>][,<outtype=xx>]\n");
      onError("usage: esrf_io(<C_obj>,<cmd_string>,<cmd>,<in_type>,<out_type>,<dc_flag>,<input_param>[,<out=xx>][,<outtype=xx>]");
   }

#ifdef NUMPY
   /* out=xx or outtype=xx */
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
      for (i=0; i<strlen(myouttype); i++)
         tolower(myouttype[i]);
      if (flag != 0)
	 printf("   key 'outtype' found :%s\n",myouttype);
      out_provided = 2;
   }
#else
   if ( (myarray != NULL) || (myouttype != NULL) )
      printf("Not working with NUMERIC types: out= and outtype= ignored\n");
#endif
   
   /* get device server pointer */ 
   if (flag_dc == 0)
      ds = (devserver *) PyCObject_AsVoidPtr(py_ds_pt);
   else
      dc = (datco *) PyCObject_AsVoidPtr(py_ds_pt);
      
   if (flag != 0)
      printf("   server command: %s : cmd = %d, in = %d, out = %d\n",\
   	   ds_command,ds_cmd,ds_in,ds_out); 

   /* the auxilliary arguments should be in a tuple, maybe empty */
   /* check this type */
   if (PyTuple_Check(mytuple) != 1)
   {
      printf("auxilliary arguments should be in a tuple.\n");
      onError("auxilliary arguments should be in a tuple.")
   }
      
   /* check the VOID case. we should have an argument if not VOID */
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

   /* check on argin type */
   if (check_type(ds_in,&is_in_a_single,&is_in_an_array,&is_in_a_special) != 0)
   {
      printf("unknown input argument type\n");
      onError("unknown input argument type")   
   }
   
   /* check on argout type */
   if (check_type(ds_out,&is_out_a_single,&is_out_an_array,&is_out_a_special) != 0)
   {
      printf("unknown input argument type\n");
      onError("unknown input argument type")   
   } 
   if ((is_out_an_array == 0) && (out_provided !=0)) 
   {
      /* argout can't be but in a numeric array */
      printf("Only taco argout arrays can be stored in python Numeric arrays !!\n");
      onError("Only taco argout arrays can be stored in python Numeric arrays !!")
   } 

   /* prepare empty input argin for dev_putget */
   /*------------------------------------------*/

   if (ds_in != D_VOID_TYPE)
   {
         /* get the information structure for the input */
         if ( xdr_get_type(ds_in,&data_type,&error) == DS_NOTOK )
         {
            printf("   xdr_get_type error : (%d) for type IN %d\n", error,ds_in);
            onError("xdr_get_type error");
         }
   
         /* allocate memory, then fill it */
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
      

   /* Now, fill the argin */
   /*---------------------*/
   /*  "single" type */
   /*----------------*/ 
   if (is_in_a_single == 1)
   { 
      /* check that only one item in the tuple */    
      if (PyTuple_Size(mytuple) != 1)
      {
	 printf("one single item requested as argin\n");
	 /* free malloc */
	 free (ds_argin);
	 onError("one single item requested as argin");
      }
	
      /* retreive this item */ 
      if ( (item = PyTuple_GetItem(mytuple,0)) == NULL)
      {
	 printf("cannot get item argin\n");
	 /* free malloc */
	 free (ds_argin);
	 onError("cannot get item argin");
      }	
       	
      /* convert python object in ds_argin */ 
      if ( (ret = get_argin_single(ds_argin,ds_in,item,mymsg)) != 0)
      {
         free (ds_argin);	    
	 onError(mymsg);
      }
      if (flag != 0)
         display_single(ds_argin,ds_in,"IN");      
   }    

   /* "array" type */
   /*--------------*/   
   if (is_in_an_array == 1)
   {
      /* convert python object in ds_argin */ 
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
   
   /* special type */
   /*--------------*/
   if (is_in_a_special == 1)
   {
      /* convert python object in ds_argin */ 
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
   
   /* prepare empty output argin for dev_putget */
   /*------------------------------------------*/

   if (ds_out != D_VOID_TYPE)
   {
      /* get the information structure for the input */
      if ( xdr_get_type(ds_out,&data_type,&error) == DS_NOTOK )
      {
         printf("   xdr_get_type error : (%d) for type OUT %d\n", error,ds_out);
	 free_argin ((DevType)ds_in, ds_argin, is_in_an_array);
	 onError("xdr_get_type error");
      }
   
      /* allocate memory, then fill it */
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
      
   /* if out_provided, check that argout is an array */
   /*------------------------------------------------*/
   if ( ( (out_provided == 1) || (out_provided == 2) )&& (is_out_an_array == 0) )
   {
      /* ignore out ? or deliver an error ? */
      printf("   argout type is not an array\n");
      onError("   argout type is not an array");
   }
   
#ifdef NUMPY   
   /* if out_provided with out=xx, check type */
   /*-----------------------------------------*/
   if (out_provided == 1)
   {
      /* first check that array type is correct */
      if (check_provided(myarray->descr->type_num,ds_out) != 0)
      {
         printf("array type and server type not compatible\n");
	 onError("array type and server type not compatible")
      }
      ((DevVarCharArray*)(ds_argout))->sequence = (char *)(myarray->data);
   }    
#endif
   
   /* dev_putget call */
   /*-----------------*/
   if (flag_dc == 0)
   {
      if (flag != 0)
         printf("   calling dev_putget\n");

      /*
       * release the global interpreter lock and store the thread state!
       */

      thread_state = PyEval_SaveThread();

      ret = dev_putget(*ds,ds_cmd,ds_argin,ds_in,ds_argout,ds_out,&error);

      /*
       * Acquire the global interpreter lock and restore the tread state
       */

      PyEval_RestoreThread (thread_state);

      if (flag != 0)
         printf("   from dev_putget (%d)\n",ret);
   }
   else
   {
      /* data collector call */
      if (flag != 0)
         printf("   calling dc_devget\n");
      ret = dc_devget(dc,ds_cmd,ds_argout,ds_out,&error);
      if (flag != 0)
         printf("   from dev_putget (%d)\n",ret);   
   }
   
   /* free argin */
   /*------------*/

   free_argin ((DevType)ds_in, ds_argin, is_in_an_array);
        
             
   if (ret != 0)
   {
      printf("-- esrf_io: error: (%d) %s\n",error,dev_error_str(error));    
      free_argout ((DevType)ds_out, ds_argout);
      onError("esrf_io: error on dev_putget");
   }
       
      
   /* Now, get the argout */
   /*---------------------*/
   if (out_provided == 0)
   {
      /*  "single" type */
      /*----------------*/ 
      if (is_out_a_single == 1)
      { 
         /* convert python object in ds_argout */ 
         if ( (ret = get_argout_single(ds_argout,ds_out,&item,mymsg)) != 0)
         {
	    free_argout ((DevType)ds_out, ds_argout);
	    onError(mymsg);
         }
         if (flag != 0)
            display_single(ds_argout,ds_out,"OUT");	 

         /* free argout */
	 free_argout ((DevType)ds_out, ds_argout);

         if (flag != 0)
            printf("-- esrf_io: OK\n");
         return item;      
      }    

      /* "array" type */
      /*--------------*/   
      if (is_out_an_array == 1)
      {
         /* convert python object in ds_argout */ 
         if ( (ret = get_argout_array(ds_argout,ds_out,&mylist,mymsg)) != 0)
         {
	    free_argout ((DevType)ds_out, ds_argout);
	    onError(mymsg);      
         }
         if (flag != 0)
            display_array(ds_argout,ds_out,"OUT");      

         /* free argout */
	 free_argout ((DevType)ds_out, ds_argout);
   
         if (flag != 0)
            printf("-- esrf_io: OK\n");
         return mylist;     
      }
   
      /* "special" type */
      /*----------------*/
      if (is_out_a_special == 1)
      {
         /* convert python object in ds_argout */ 
         if ( (ret = get_argout_special(ds_argout,ds_out,&mylist,mymsg)) != 0)
         {
	    free_argout ((DevType)ds_out, ds_argout);
	    onError(mymsg);      
         }
         if (flag != 0)
            display_special(ds_argout,ds_out,"OUT");
      
         /* free argout */
	 free_argout ((DevType)ds_out, ds_argout);
    
         if (flag != 0)
            printf("-- esrf_io: OK\n");
         return mylist; 
      }   
  
      /* in case of D_VOID_TYPE */         
      if (flag != 0)
         printf("-- esrf_io: OK\n");

      Py_INCREF(Py_None);      
      return Py_None;
   }

#ifdef NUMPY   
   /* out=xx */
   /*--------*/
   else if (out_provided == 1)
   {
      /* myarray->data has been filled. we return the number 
         of elements set in the array */
      if (flag != 0)
         display_array(ds_argout,ds_out,"OUT");
      if (flag != 0)
         printf("-- esrf_io: OK\n");
      return(Py_BuildValue("(l)",((DevVarCharArray*)(ds_argout))->length));
   } 
   
   /* outtype=numeric */
   /*-----------------*/
   else
   {
      /* determine numeric type for this taco type */
      type_num = Ctype2numeric(ds_out); 
      if (type_num == -1)
      {
	 printf("cannot create Numeric type from taco type %d\n",ds_out);
	 free_argout ((DevType)ds_out, ds_argout);
	 onError("cannot create Numeric type from taco type")
      }
            
      /* copy the argout->sequence to another array */
      /* first allocate */
      if ( (mylen = lenoftype(ds_out)) == -1)
      {
	 printf("lenoftype: cannot calculate output single element size\n");
	 free_argout ((DevType)ds_out, ds_argout);
	 onError("lenoftype: cannot calculate output single element size")      
      }
      array_2py = (char *) malloc( ((DevVarCharArray*)(ds_argout))->length*mylen);
      /* then copy */
      memcpy( (void *)array_2py,
              (void *)(((DevVarCharArray*)(ds_argout))->sequence),
	      ((DevVarCharArray*)(ds_argout))->length*mylen
	    );
      
      /* now, generate the Numeric array */

      dimension[0]= (int)(((DevVarCharArray*)(ds_argout))->length);
      myarray = (PyArrayObject *) PyArray_FromDimsAndData(1,
               dimension,
               (int) type_num,
	       array_2py);

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
   dc_devfree[0].dc_dev_error = (DevLong*) malloc(sizeof(DevLong));

   devstatus = dc_free(dc_devfree,1,&error);

   if (devstatus != 0) 
      printf("esrf_dc_free failed: %s\n",
               dev_error_str(*(dc_devfree[0].dc_dev_error)));
   else
      if (flag != 0)
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
   char *devname;
   dc_dev_imp *dc_devimp;
   static datco *mydc_ptr;
   static dc_dev_free *dc_devfree;

   /* get the device name */
   if (!PyArg_ParseTuple(args,"s",&devname))
      onError("usage: esrf_dc_import (<device_name>)")

   if (flag != 0)
      printf("-- esrf_dc_import: devname = %s\n",devname); 

   /* create structure for call */
   dc_devimp = (dc_dev_imp*)malloc(sizeof(dc_dev_imp));
   if (dc_devimp == NULL)
   {
      printf("esrf_dc_import: cannot allocate memory for dc access\n");
      onError("esrf_dc_import: cannot allocate memory for dc access");
   }
   dc_devimp[0].device_name = (DevString)malloc(sizeof(DevChar)*strlen(devname));
   strcpy(dc_devimp[0].device_name,devname);
   dc_devimp[0].dc_ptr = NULL;
   dc_devimp[0].dc_dev_error = (DevLong*) malloc(sizeof(DevLong));
   
   devstatus = dc_import((dc_dev_imp *)dc_devimp,1,&error);

   free(dc_devimp[0].device_name);
   
   if (devstatus != 0)
   {
      printf("%s\n",dev_error_str(error));
      printf("%s\n",dev_error_str(*(dc_devimp[0].dc_dev_error)));
      /* if any */
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

   /* save datco to give it to python */
   mydc_ptr = dc_devimp[0].dc_ptr;

   /* free everything but datco*/
   if (dc_devimp[0].dc_dev_error != NULL) 
      free(dc_devimp[0].dc_dev_error);
   if(dc_devimp != 0)
      free(dc_devimp);   
   
   return PyCObject_FromVoidPtr((void *)mydc_ptr, 
                                (void *)esrf_dc_free);   
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
   PyObject* mylist;
   PyObject* mydict;
   PyObject* mylistcmd;
   PyObject* item;
   long ret,i;
   int dev_n;
   char *mydev=NULL;
   char *myhost=NULL;
   char **devnametab;
   dc_devinf dc_dev_info;
   DevLong error;
   char *mystring;

   if (flag != 0)
      printf("-- esrf_dc_info: enter\n");

   if (!PyArg_ParseTuple(args,"|s",&mydev))
      onError("usage: esrf_dc_info([device])")

   /* if no device provided, dc_devall required
      get the NETHOST env variable */
   if (mydev == NULL)
   {
      myhost = getenv("NETHOST");
      if (myhost == NULL) 	/* not possible */
         onError("no NETHOST variable set")
   
      if (flag != 0)
         printf("-- esrf_dc_devall on host %s\n",myhost);

      ret = dc_devall(myhost,&devnametab,&dev_n,&error);
   
      if (ret != 0)
      {
         printf("esrf_dc_info: error in dc_devall (%d)\n",error);
         onError("esrf_dc_info: error in dc_devall")
      }
      else
      {
         if ( (mylist = PyList_New(dev_n)) == NULL)
            onError("cannot build list for devices");
      
         for (i=0; i<dev_n; i++)
         {
            if ( (item = PyString_FromString(devnametab[i])) == NULL)
               onError("cannot build device name");
            if (PyList_SetItem(mylist,i,item) == - 1)
               onError("cannot build device name list");      
         }
      
         /* now free */
         for (i=0; i<dev_n; i++)
            free(devnametab[i]); 
         free(devnametab);
           
         if (flag != 0)
            printf("-- esrf_dc_info: OK\n");

         return mylist;   
      }
   }
   else		/* dc_dinfo required for a device*/
   {
      ret = dc_dinfo(mydev,&dc_dev_info,&error);
      
      if (ret != 0)
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


/**************************************************************   

		Array for functions mapping 

***************************************************************/

static struct PyMethodDef Taco_methods[] = {
   {"esrf_import",	esrf_import,		1},
   {"esrf_query",	esrf_query,		1},
   {"esrf_tcpudp",	esrf_tcpudp,		1},
   {"esrf_timeout",	esrf_timeout,		1},
   {"esrf_getdevlist",	esrf_getdevlist,	1},
   {"esrf_getdevexp",	esrf_getdevexp, 	1},
   {"esrf_getresource",	esrf_getresource,	1},
   {"esrf_putresource",	esrf_putresource,	1},
   {"esrf_delresource",	esrf_delresource,	1},
   {"esrf_io",		(PyCFunction)esrf_io, 1|2},
   {"esrf_debug",	esrf_debug, 		1},
   {"esrf_dc_info",	esrf_dc_info, 		1},
   {"esrf_dc_import",	esrf_dc_import, 	1},
   {"db_getresource",	esrf_getresource,	1},
   {"db_putresource",	esrf_putresource,	1},
   {NULL,	NULL}
};


/**************************************************************

		init function

***************************************************************/
void initTaco()
{
    PyObject *m, *d;
    
    m = Py_InitModule("Taco",Taco_methods);
#ifdef NUMPY
    import_array();
#endif
    
    d = PyModule_GetDict(m);
    ErrorObject = Py_BuildValue("s","Taco.error");
    PyDict_SetItemString(d,"error",ErrorObject);
    
    if (PyErr_Occurred())
       Py_FatalError("Can't initialize module Taco");

    /* we create here a global dictionnary and a global tuple for
       esrf_io further init parameters */

    if ( (glob_tuple = PyList_New(0)) == NULL)
       printf("initTaco: cannot create global tuple\n");
    
    if ( (glob_dict = PyDict_New()) == NULL)
       printf("Cannot create global dict\n");
           
    /* printf("Taco module init\n"); */
}
