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
 * File       :	tango_api.c
 *
 * Project    :	Interfacing TACO (based on SUN/RPC) to TANGO (based on CORBA)
 *
 * Description:	TACO to TANGO Application Programmer's Interface
 *
 *		Source code to implement the taco interface to tango via
 *		the Device Server C API (DSAPI) using the TANGO C++ interface. 
 *		This means it will be possible to use the TACO dev_putget() 
 *		call from TACO programs which have been linked with C++
 *		to talk to TANGO device servers.
 *	
 * Author(s)  :	Andy Goetz
 * 		$Author: andy_gotz $
 *
 * Original   :	December 1999
 *
 * Version    : $Revision: 1.4 $
 *
 * Date       : $Date: 2005-09-28 16:08:27 $
 *
 ********************************************************************-*/
static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/tango/tango_api.cpp,v 1.4 2005-09-28 16:08:27 andy_gotz Exp $";

#include <tango.h>
#define TANGO_API
#include "config.h"
#include <API.h>
#include <ApiP.h>
#include <dc.h>
#include <stdio.h>
#include <assert.h>
#include <iostream.h>
#include <vector>
#include <string>
#ifdef linux
using namespace std;
#endif

#define D_IIOP	1111		/* TANGO native protocol - should be in API.h */

static long get_cmd_value (char*, char*, long*, long *error);
static long tango_to_taco_type(long );
static CORBA::Any tango_argin_to_any(long, long, void *, long *);    
static void tango_any_to_argout(long, long, CORBA::Any, void *, long *);    
static void tango_any_to_argout_raw(long, long, CORBA::Any, void *, long *);    
static long tango_dev_check(devserver, long *);    
static long tango_dev_error_string(Tango::DevFailed tango_exception);

/*
 * TANGO/CORBA globals with file scope
 */

static CORBA::ORB *orb;
static Tango::Database *tango_dbase;
static long tango_dbase_init=0;
/*
typedef struct _tango_device {
	long flag; 
	vector<string> cmd_name;
	vector<long> cmd_value;
	vector<long> argin_type;
	vector<long> argout_type;
	Tango::Device_var object;} _tango_device;
static vector<_tango_device> tango_device;
 */
#define MAX_TANGO_DEVICE 1000
static _tango_device *tango_device=NULL;
static int max_tango_device=0;
static int tango_device_init=0;
static long n_tango_device=0;
static int tango_port=10000;
static char *tango_host_c_str=NULL;

/*
 * symbols used by TACO/TANGO interface
 */
#define TANGO_DEV_NOT_IMPORTED 	0
#define TANGO_DEV_IMPORTED 	1
#define TANGO_BAD_CONNECTION	2
#define TANGO_DEV_FREE		3

extern long debug_flag;
extern char *dev_error_string;

#undef DevState
#undef RUNNING

/**
 * @defgroup tangoAPI TANGO API for TACO
 * @ingroup clientAPI
 */

/**
 * @defgroup tangoAPIintern internal API for the TANGO API
 * @ingroup tangoAPI
 */

/**@ingroup tangoAPI
 * 
 * import TANGO database
 *
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */


long tango_db_import(long *error)
{


// Connect to TANGO database

	try {
		tango_dbase = new Tango::Database();
	}
	catch (CORBA::Exception  &corba_exception) {
#ifdef DEBUG
		if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception); 
#endif
		*error = DevErr_DbImportFailed;
		return(DS_NOTOK);
	}
	
	if (CORBA::is_nil(tango_dbase->get_dbase()))
	{
		*error = DevErr_DbImportFailed;
		return(DS_NOTOK);
	}

	tango_dbase_init = 1;

	int i;

	if (!tango_device_init)
	{
		if (tango_device == NULL)
		{
			tango_device = (_tango_device*)malloc(sizeof(_tango_device)*MAX_TANGO_DEVICE);
		}
		for (i=0; i< MAX_TANGO_DEVICE; i++)
		{
			tango_device[i].flag = TANGO_DEV_FREE;
		}
		max_tango_device = MAX_TANGO_DEVICE;
		tango_device_init =1;
	}

	return(DS_OK);
}

/**@ingroup tangoAPI
 * 
 * import TANGO device
 *
 * @param dev_name name of device
 * @param access security access level
 * @param ds_ptr returned device server structure
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_import(char *dev_name, long access, devserver *ds_ptr, long *error)
{
	_tango_device tango_device_tmp;
	long tango_device_free;
	long i;

	if (!tango_dbase_init) 
	{
		try
		{
			tango_db_import(error);
        	}
		catch (CORBA::Exception  &corba_exception)
		{
#ifdef DEBUG
                	if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
			*error = DevErr_DbImportFailed;
                	return(DS_NOTOK);
        	}
	}

	if (dev_name == NULL) 
	{
		*error = DevErr_DeviceOfThisNameNotServed;
		return(DS_NOTOK);
	}

// Get IOR from TANGO database

	try 
	{
		string dev_name_str(dev_name);

                tango_device_tmp.device_proxy = new Tango::DeviceProxy(dev_name_str);
                tango_device_tmp.flag = TANGO_DEV_IMPORTED;
        }
//
// failed to import - device does not exist ?
//
        catch (CORBA::Exception  &corba_exception)
	{
#ifdef DEBUG
                if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
		*error = DevErr_DeviceOfThisNameNotServed;
                return(DS_NOTOK);
        }
//
// initialise command list for device
//
	try
	{
		Tango::CommandInfoList *cmd_query = tango_device_tmp.device_proxy->command_list_query();
		Tango::DeviceInfo dev_info = tango_device_tmp.device_proxy->info();

		//tango_device_tmp.cmd_name = vector<string>(cmd_query->size());
		tango_device_tmp.cmd_name = (char**)malloc(cmd_query->size()*sizeof(char*));
		tango_device_tmp.n_cmds = cmd_query->size();
		tango_device_tmp.cmd_value = (long*)malloc(cmd_query->size()*sizeof(long));
		tango_device_tmp.argin_type = (long*)malloc(cmd_query->size()*sizeof(long));
		tango_device_tmp.argout_type = (long*)malloc(cmd_query->size()*sizeof(long));
/*
 * the Solaris CC compiler does not support vector<long> as a member of a class array
 * replace it with an array - andy 4nov02
 *
		tango_device_tmp.cmd_value = vector<long>(cmd_query->size());
		tango_device_tmp.argin_type = vector<long>(cmd_query->size());
		tango_device_tmp.argout_type = vector<long>(cmd_query->size());
 */
		for (i = 0;i < cmd_query->size();i++)
		{
			long error, tango_cmd_value;
			//tango_device_tmp.cmd_name[i] = string((*cmd_query)[i].cmd_name);
//
// map TANGO's "State" and "Status" commands into "DevState" and "DevStatus"
// to make them compatible with TACO (on request of Laurent Farvacque)
//
// andy 08apr03
// 
// added following commands - Insert, Extract, On, Off, Reset, Open, Close
//
			if ((*cmd_query)[i].cmd_name == "State")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevState")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevState");
			}
			else if ((*cmd_query)[i].cmd_name == "Status")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevStatus")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevStatus");
			}
			else if ((*cmd_query)[i].cmd_name == "Insert")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevInsert")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevInsert");
			}
			else if ((*cmd_query)[i].cmd_name == "Extract")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevExtract")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevExtract");
			}
			else if ((*cmd_query)[i].cmd_name == "Open")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevOpen")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevOpen");
			}
			else if ((*cmd_query)[i].cmd_name == "Close")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevClose")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevClose");
			}
			else if ((*cmd_query)[i].cmd_name == "On")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevOn")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevOn");
			}
			else if ((*cmd_query)[i].cmd_name == "Off")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevOff")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevOff");
			}
			else if ((*cmd_query)[i].cmd_name == "Reset")
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen("DevReset")+1);
				sprintf(tango_device_tmp.cmd_name[i],"DevReset");
			}
			else
			{
				tango_device_tmp.cmd_name[i] = (char*)malloc(strlen((*cmd_query)[i].cmd_name.c_str())+1);
				sprintf(tango_device_tmp.cmd_name[i],"%s",(*cmd_query)[i].cmd_name.c_str());
			}
			get_cmd_value((char*)dev_info.dev_class.c_str(),
			              tango_device_tmp.cmd_name[i],
			              &tango_cmd_value,
			              &error);
			tango_device_tmp.cmd_value[i] = tango_cmd_value;
			tango_device_tmp.argin_type[i] = (*cmd_query)[i].in_type;
			tango_device_tmp.argout_type[i] = (*cmd_query)[i].out_type;
		}
		delete cmd_query;
        	if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
        	{
                	*error = DevErr_InsufficientMemory;
                	return(DS_NOTOK);
        	}
		tango_device_free = -1;
		/*for (i = 0; i < tango_device.size(); i++)*/
		for (i = 0; i < max_tango_device; i++)
		{
			if (tango_device[i].flag == TANGO_DEV_FREE)
			{
				tango_device_free = i;
				break;
			}
		}
		if (tango_device_free < 0)
		{
			tango_device = (_tango_device*)realloc(tango_device,sizeof(_tango_device)*(max_tango_device+MAX_TANGO_DEVICE));
			for (i=max_tango_device;i<max_tango_device+MAX_TANGO_DEVICE;i++)
			{
				tango_device[i].flag = TANGO_DEV_FREE;
			}
			tango_device_free = max_tango_device;
			max_tango_device = max_tango_device+MAX_TANGO_DEVICE;
		}
		tango_device[tango_device_free] = tango_device_tmp;
		n_tango_device = tango_device_free;
        	sprintf((*ds_ptr)->device_name,"%s",dev_name);
        	sprintf((*ds_ptr)->device_class,"%s",(char*)dev_info.dev_class.c_str());
        	sprintf((*ds_ptr)->device_type,"TANGO_Device");
        	sprintf((*ds_ptr)->server_name,"Unknown");
        	sprintf((*ds_ptr)->server_host,"Unknown");
        	(*ds_ptr)->clnt             = NULL;
        	(*ds_ptr)->ds_id            = n_tango_device;
        	(*ds_ptr)->no_svr_conn      = 0;
        	(*ds_ptr)->rpc_conn_counter = 0;
        	(*ds_ptr)->dev_access       = access;
        	(*ds_ptr)->i_nethost        = 0;
        	(*ds_ptr)->rpc_protocol     = D_IIOP;
        	(*ds_ptr)->rpc_timeout.tv_sec  = 0;
        	(*ds_ptr)->rpc_timeout.tv_usec = 0;

	}
        catch (CORBA::Exception  &corba_exception)
        {
#ifdef DEBUG
                if (debug_flag & DBG_TANGO) Tango::Util::print_exception(e);
#endif
//
// device command query failed - case for stateless import
// 
                if (dev_notimported_init(dev_name,access,0,ds_ptr,error) == DS_NOTOK)                                return(DS_NOTOK);
                (*ds_ptr)->ds_id = -1;
                (*ds_ptr)->rpc_protocol = D_IIOP; 
                return(DS_OK);
        }
	n_tango_device++;

	return(0);
}

/**@ingroup tangoAPI
 * 
 * free a TANGO device
 *
 * @param ds device server structure to free
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_free(devserver ds, long *error)
{
	if (ds->ds_id != -1)
	{
		for (int i=0; i<tango_device[ds->ds_id].n_cmds; i++)
		{
			free(tango_device[ds->ds_id].cmd_name[i]);
		}
		free(tango_device[ds->ds_id].cmd_name);
		free(tango_device[ds->ds_id].cmd_value);
		free(tango_device[ds->ds_id].argin_type);
		free(tango_device[ds->ds_id].argout_type);
		tango_device[ds->ds_id].flag = TANGO_DEV_FREE;
		delete (tango_device[ds->ds_id].device_proxy);
	}
	free(ds);

	return(0);
}

/**@ingroup tangoAPI
 * 
 * reimport TANGO device
 *
 * @param ds device server structure of device to reimport
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_reimport(devserver ds, long *error)
{

	if (!tango_dbase_init) 
	{
		try
		{
			tango_db_import(error);
        	}
		catch (CORBA::Exception  &corba_exception)
		{
#ifdef DEBUG
                	if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
			*error = DevErr_DbImportFailed;
                	return(DS_NOTOK);
        	}
	}

// Get new IOR from TANGO database (server might have been restarted)

	try 
	{
		string device_name_str(ds->device_name);

                tango_device[ds->ds_id].device_proxy = new Tango::DeviceProxy(device_name_str);
                tango_device[ds->ds_id].flag = TANGO_DEV_IMPORTED;
        }
        catch (CORBA::Exception  &corba_exception)
	{
#ifdef DEBUG
                if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
		*error = DevErr_BadServerConnection;
                return(DS_NOTOK);
        }

/*
        if (CORBA::is_nil(tango_device[ds->ds_id].device_proxy))
        {
		*error = DevErr_BadServerConnection;
                return(DS_NOTOK);
        }
 */

	return(0);
}

/**@ingroup tangoAPI
 * 
 * execute a command on a TANGO device
 *
 * @param cmd command to execute
 * @param argin - pointer to argin
 * @param argin_type - argin type
 * @param argout pointer to argout
 * @param argout_type argout type
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_putget(devserver ds, long cmd, void *argin, long argin_type,
                      void *argout, long argout_type, long *error)
{

	long i_cmd = -1, dev_id;
	const char *cmd_name = NULL;

	if (tango_dev_check(ds,error) != DS_OK) return(DS_NOTOK);

	dev_id = ds->ds_id;
//
// check if command has already been executed, if not then
// read command string using its value from TACO database
//
	//for (long i=0; i<tango_device[dev_id].cmd_value.size(); i++)
	for (long i=0; i<tango_device[dev_id].n_cmds; i++)
	{
		if (tango_device[dev_id].cmd_value[i] == cmd)
		{
			i_cmd = i;
			cmd_name = tango_device[dev_id].cmd_name[i_cmd];
			if (strcmp(cmd_name,"DevState") == 0)
			{
				cmd_name = "State";
			}
			else if (strcmp(cmd_name,"DevStatus") == 0)
			{
				cmd_name = "Status";
			}
// added following commands - Insert, Extract, On, Off, Reset, Open, Close
//
			else if (strcmp(cmd_name,"DevOn") == 0)
			{
				cmd_name = "On";
			}
			else if (strcmp(cmd_name,"DevOff") == 0)
			{
				cmd_name = "Off";
			}
			else if (strcmp(cmd_name,"DevInsert") == 0)
			{
				cmd_name = "Insert";
			}
			else if (strcmp(cmd_name,"DevExtract") == 0)
			{
				cmd_name = "Extract";
			}
			else if (strcmp(cmd_name,"DevOpen") == 0)
			{
				cmd_name = "Open";
			}
			else if (strcmp(cmd_name,"DevClose") == 0)
			{
				cmd_name = "Close";
			}
			else if (strcmp(cmd_name,"DevReset") == 0)
			{
				cmd_name = "Reset";
			}
			break;
		}
	}
	if (i_cmd == -1)
	{
		*error = DevErr_CommandNotImplemented;
		return(DS_NOTOK);
	}

	if (tango_device[dev_id].flag == TANGO_DEV_IMPORTED) 
	{
		try 
		{

// insert input argument into Any type

			CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd],
			                                    argin, error);

// execute command using command_inout() method

//                	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(tango_device[dev_id].cmd_name[i_cmd],send);
                	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(cmd_name,send);

// extract output argument from Any

			tango_any_to_argout(argout_type, tango_device[dev_id].argout_type[i_cmd],
		                    	received, argout, error);
        	}
        	catch (Tango::DevFailed  &tango_exception)
		{
#ifdef DEBUG
                	if (debug_flag & DBG_TANGO) Tango::Util::print_exception(tango_exception);
#endif
//
// recover TANGO error string and save it in global error string so that
// it can be printed out with dev_error_str()
//
			tango_dev_error_string(tango_exception);
			*error = DevErr_CommandFailed;
                	return(DS_NOTOK);
        	}
//
// CORBA exception, assume server is down, try to reimport immediately
//
                catch (CORBA::Exception  &e)
                {
#ifdef DEBUG
                        if (debug_flag & DBG_TANGO) Tango::Util::print_exception(e);
#endif
                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
			if (tango_dev_check(ds,error) != DS_OK)
			{
		        	*error = DevErr_BadServerConnection;
                        	return(DS_NOTOK);
			}
//
// if import works then execute the command again i.e. immediate reconnection

			try 
			{
		
		// insert input argument into Any type
		
				CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd],
			                                    	argin, error);
		
// execute command using command_inout() method
		
//		               	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(tango_device[dev_id].cmd_name[i_cmd],send);
		               	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(cmd_name,send);
		
// extract output argument from Any
		
				tango_any_to_argout(argout_type, tango_device[dev_id].argout_type[i_cmd],
			                    	received, argout, error);
		        }
		        catch (Tango::DevFailed  &tango_exception)
		        {
#ifdef DEBUG
		        	if (debug_flag & DBG_TANGO) Tango::Util::print_exception(tango_exception);
#endif
//
// recover TANGO error string and save it in global error string so that
// it can be printed out with dev_error_str()
//
				tango_dev_error_string(tango_exception);
	                        *error = DevErr_CommandFailed;
	                        return(DS_NOTOK);
	                }
	                catch (CORBA::Exception  &corba_exception)
	                {
#ifdef DEBUG
	                        if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
	                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
	                        *error = DevErr_BadServerConnection;
	                        return(DS_NOTOK);
	                }
		}
	}

	return(0);
}

/**@ingroup tangoAPI
 * 
 * execute a command on a TANGO device and return the data in DEV_OPAQUE_TYPE
 *
 * @param id device id
 * @param cmd command to execute
 * @param argin pointer to argin
 * @param argin_type argin type
 * @param argout pointer to argout
 * @param argout_type argout type
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_putget_raw(devserver ds, long cmd, void *argin, long argin_type,
                      void *argout, long argout_type, long *error)
{

	long i_cmd = -1, dev_id;
	const char *cmd_name = NULL;

	if (tango_dev_check(ds,error) != DS_OK) return(DS_NOTOK);

	dev_id = ds->ds_id;
//
// check if command has already been executed, if not then
// read command string using its value from TACO database
//
	//for (long i=0; i<tango_device[dev_id].cmd_value.size(); i++)
	for (long i=0; i<tango_device[dev_id].n_cmds; i++)
	{
		if (tango_device[dev_id].cmd_value[i] == cmd)
		{
			i_cmd = i;
			cmd_name = tango_device[dev_id].cmd_name[i_cmd];
			if (strcmp(cmd_name,"DevState") == 0)
			{
				cmd_name = "State";
			}
			if (strcmp(cmd_name,"DevStatus") == 0)
			{
				cmd_name = "Status";
			}
			break;
			break;
		}
	}
	if (i_cmd == -1)
	{
		*error = DevErr_CommandNotImplemented;
		return(DS_NOTOK);
	}

        if (tango_device[dev_id].flag == TANGO_DEV_IMPORTED)
        {
		try 
		{

// insert input argument into Any type

			CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd],
		                                    	argin, error);

// execute command using command_inout() method

//                	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(tango_device[dev_id].cmd_name[i_cmd],send);
                	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(cmd_name,send);

// extract output argument from Any

			tango_any_to_argout_raw(argout_type, tango_device[dev_id].argout_type[i_cmd],
		                    	received, argout, error);
        	}
                catch (Tango::DevFailed  &tango_exception)
                {
#ifdef DEBUG
                        if (debug_flag & DBG_TANGO) Tango::Util::print_exception(tango_exception);
#endif
// TODO - improve TANGO to TACO error handling here
                        *error = DevErr_CommandFailed;
                        return(DS_NOTOK);
                }
//
// CORBA exception, assume server is down, try to reimport immediately
//
                catch (CORBA::Exception  &corba_exception)
                {
#ifdef DEBUG
                        if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
			if (tango_dev_check(ds,error) != DS_OK)
			{
		        	*error = DevErr_BadServerConnection;
                        	return(DS_NOTOK);
			}
//
// if import works then execute the command again i.e. immediate reconnection

			try 
			{
		
		// insert input argument into Any type
		
				CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd],
			                                    	argin, error);
		
// execute command using command_inout() method
		
//		               	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(tango_device[dev_id].cmd_name[i_cmd],send);
		               	CORBA::Any_var received = tango_device[dev_id].device_proxy->command_inout(cmd_name,send);
		
// extract output argument from Any
		
				tango_any_to_argout_raw(argout_type, tango_device[dev_id].argout_type[i_cmd],
			                    	received, argout, error);
		        }
		        catch (Tango::DevFailed  &tango_exception)
		        {
#ifdef DEBUG
		        	if (debug_flag & DBG_TANGO) Tango::Util::print_exception(tango_exception);
#endif
// TODO - improve TANGO to TACO error handling here
	                        *error = DevErr_CommandFailed;
	                        return(DS_NOTOK);
	                }
	                catch (CORBA::Exception  &corba_exception)
	                {
#ifdef DEBUG
	                        if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
	                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
	                        *error = DevErr_BadServerConnection;
	                        return(DS_NOTOK);
	                }
		}
	}

	return(DS_OK);
}

/**@ingroup tangoAPI
 * 
 * Returns a sequence of structures containing all available commands, names, input and output data types
 * for a TANGO device
 *
 * @param ds device server structuer
 * @param varcmdarr returned list of commands and their types
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_cmd_query (devserver ds, DevVarCmdArray *varcmdarr, long *error)
{
	long dev_id;

	if (tango_dev_check(ds,error) != DS_OK) return(DS_NOTOK);

	dev_id = ds->ds_id;

	*error = 0;

/*
 * Allocate memory for a sequence of DevCmdInfo structures
 * returned with varcmdarr.
 */

	//varcmdarr->length   = tango_device[dev_id].cmd_value.size();
	varcmdarr->length   = tango_device[dev_id].n_cmds;
	varcmdarr->sequence = (DevCmdInfo*)malloc(varcmdarr->length * sizeof (DevCmdInfo));
	if ( varcmdarr->sequence == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}
	memset ((char *)varcmdarr->sequence,0,(varcmdarr->length * sizeof (DevCmdInfo)));

	for (long i = 0; i < varcmdarr->length; i++)
	{
		varcmdarr->sequence[i].cmd      = tango_device[dev_id].cmd_value[i];
		strncat(varcmdarr->sequence[i].cmd_name,tango_device[dev_id].cmd_name[i],32);
		varcmdarr->sequence[i].in_type  = tango_to_taco_type(tango_device[dev_id].argin_type[i]);
// leave in_name == NULL
//		varcmdarr->sequence[i].in_name = (char*)" "; 
		varcmdarr->sequence[i].out_type = tango_to_taco_type(tango_device[dev_id].argout_type[i]);
// leave out_name == NULL
//		varcmdarr->sequence[i].out_name = (char*)" ";
	}

	return (DS_OK);
}

/**@ingroup tangoAPI
 * 
 * ping a tango device
 *
 * @param ds device server structure
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

long tango_dev_ping (devserver ds, long *error)
{
	long dev_id;

	if (tango_dev_check(ds,error) != DS_OK) return(DS_NOTOK);

	dev_id = ds->ds_id;

	*error = 0;

        try
        {
                tango_device[dev_id].device_proxy->ping();
        }
        catch (CORBA::Exception  &corba_exception)
	{
#ifdef DEBUG
		if (debug_flag & DBG_TANGO) Tango::Util::print_exception(corba_exception);
#endif
// TODO - improve TANGO to TACO error handling here
                *error = DevErr_CommandFailed;
                return(DS_NOTOK);
        } 

	return (DS_OK);
}

/**@ingroup tangoAPI
 * 
 * convert a TACO argin type to a TANGO type and return it in an Any
 *
 * @param argin_type argin type to convert
 * @param tango_type equivalent tango type
 * @param pointer to argin
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */

static CORBA::Any tango_argin_to_any(long argin_type, long tango_type, void *argin, long *error)
{
	CORBA::Any send;

	if (argin == NULL && argin_type != D_VOID_TYPE)
	{
		Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
				(const char *)"argin is NULL ",
		                (const char *)"tango_argin_to_any()");
	}

	switch(argin_type) 
	{
		case D_VOID_TYPE : 
		{
			if (tango_type != Tango::DEV_VOID)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_VOID ", 
		                		(const char *)"tango_argin_to_any()");
			}
			break;
		}

		case D_SHORT_TYPE :
		{
			if (tango_type != Tango::DEV_SHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_SHORT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			short *argin_short;
			argin_short = (short*)argin;
			send <<= *argin_short;
			break;
		}
			

		case D_USHORT_TYPE :
		{
			if (tango_type != Tango::DEV_USHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_USHORT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			unsigned short *argin_short;
			argin_short = (unsigned short*)argin;
			send <<= *argin_short;
			break;
		}
			
		case D_LONG_TYPE :
		{
			if (tango_type != Tango::DEV_LONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_LONG ", 
		                		(const char *)"tango_argin_to_any()");
			}
			long *argin_long;
			argin_long = (long*)argin;
			send <<= *argin_long;
			break;
		}
			
		case D_ULONG_TYPE :
		{
			if (tango_type != Tango::DEV_ULONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_ULONG ", 
		                		(const char *)"tango_argin_to_any()");
			}
			unsigned long *argin_ulong;
			argin_ulong = (unsigned long*)argin;
			send <<= *argin_ulong;
			break;
		}
			
		case D_FLOAT_TYPE :
		{
			if (tango_type != Tango::DEV_FLOAT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_FLOAT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			float *argin_float;
			argin_float = (float*)argin;
			send <<= *argin_float;
			break;
		}
			
		case D_DOUBLE_TYPE :
		{
			if (tango_type != Tango::DEV_DOUBLE)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_DOUBLE ", 
		                		(const char *)"tango_argin_to_any()");
			}
			double *argin_double;
			argin_double = (double*)argin;
			send <<= *argin_double;
			break;
		}
			
		case D_STRING_TYPE :
		{
			char **argin_str;

			if (tango_type != Tango::DEV_STRING)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argin type is not Tango::DEV_STRING", 
		                		(const char *)"tango_argin_to_any()");
			}
			argin_str = (char**)argin;
			send <<= CORBA::string_dup(*argin_str);
			break;
		}

		case D_VAR_STRINGARR :
		{
			if (tango_type != Tango::DEVVAR_STRINGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_STRINGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarStringArray *argin_vsa;
			argin_vsa = (DevVarStringArray*)argin;
			Tango::DevVarStringArray *tango_vsa = new Tango::DevVarStringArray;
			tango_vsa->length(argin_vsa->length);
			for (long i=0; i<argin_vsa->length; i++)
			{
				(*tango_vsa)[i] = CORBA::string_dup(argin_vsa->sequence[i]);
			}
			send <<= tango_vsa;
			break;
		}
			
		case D_VAR_CHARARR :
		{
			if (tango_type != Tango::DEVVAR_CHARARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_CHARARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarCharArray *argin_vsha;
			argin_vsha = (DevVarCharArray*)argin;
			Tango::DevVarCharArray *tango_vsha = new Tango::DevVarCharArray;
			tango_vsha->length(argin_vsha->length);
			for (long i=0; i<argin_vsha->length; i++)
			{
				(*tango_vsha)[i] = argin_vsha->sequence[i];
			}
			send <<= tango_vsha;
			break;
		}
		
		case D_VAR_SHORTARR :
		{
			if (tango_type != Tango::DEVVAR_SHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_SHORTARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarShortArray *argin_vsha;
			argin_vsha = (DevVarShortArray*)argin;
			Tango::DevVarShortArray *tango_vsha = new Tango::DevVarShortArray;
			tango_vsha->length(argin_vsha->length);
			for (long i=0; i<argin_vsha->length; i++)
			{
				(*tango_vsha)[i] = argin_vsha->sequence[i];
			}
			send <<= tango_vsha;
			break;
		}
			
		case D_VAR_USHORTARR :
		{
			if (tango_type != Tango::DEVVAR_USHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_USHORTARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarUShortArray *argin_vusha;
			argin_vusha = (DevVarUShortArray*)argin;
			Tango::DevVarUShortArray *tango_vusha = new Tango::DevVarUShortArray;
			tango_vusha->length(argin_vusha->length);
			for (long i=0; i<argin_vusha->length; i++)
			{
				(*tango_vusha)[i] = argin_vusha->sequence[i];
			}
			send <<= tango_vusha;
			break;
		}
			
		case D_VAR_LONGARR :
		{
			if (tango_type != Tango::DEVVAR_LONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_LONGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarLongArray *argin_vla;
			argin_vla = (DevVarLongArray*)argin;
			Tango::DevVarLongArray *tango_vla = new Tango::DevVarLongArray;
			tango_vla->length(argin_vla->length);
			for (long i=0; i<argin_vla->length; i++)
			{
				(*tango_vla)[i] = argin_vla->sequence[i];
			}
			send <<= tango_vla;
			break;
		}
			
		case D_VAR_ULONGARR :
		{
			if (tango_type != Tango::DEVVAR_ULONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_ULONGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarULongArray *argin_vula;
			argin_vula = (DevVarULongArray*)argin;
			Tango::DevVarULongArray *tango_vula = new Tango::DevVarULongArray;
			tango_vula->length(argin_vula->length);
			for (long i=0; i<argin_vula->length; i++)
			{
				(*tango_vula)[i] = argin_vula->sequence[i];
			}
			send <<= tango_vula;
			break;
		}
			
		case D_VAR_FLOATARR :
		{
			if (tango_type != Tango::DEVVAR_FLOATARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_FLOATARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarFloatArray *argin_vfa;
			argin_vfa = (DevVarFloatArray*)argin;
			Tango::DevVarFloatArray *tango_vfa = new Tango::DevVarFloatArray;
			tango_vfa->length(argin_vfa->length);
			for (long i=0; i<argin_vfa->length; i++)
			{
				(*tango_vfa)[i] = argin_vfa->sequence[i];
			}
			send <<= tango_vfa;
			break;
		}
			
		case D_VAR_DOUBLEARR :
		{
			if (tango_type != Tango::DEVVAR_DOUBLEARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_DOUBLEARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			DevVarDoubleArray *argin_vda;
			argin_vda = (DevVarDoubleArray*)argin;
			Tango::DevVarDoubleArray *tango_vda = new Tango::DevVarDoubleArray;
			tango_vda->length(argin_vda->length);
			for (long i=0; i<argin_vda->length; i++)
			{
				(*tango_vda)[i] = argin_vda->sequence[i];
			}
			send <<= tango_vda;
			break;
		}
			
		default:
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TACO type no supported", 
		                		(const char *)"tango_argin_to_any()");
	}

	return(send);
}

/**@ingroup tangoAPI
 * 
 * extract a TANGO type from an Any and convert it to a TACO argout
 *
 * @param argout_type argout type to convert
 * @param tango_type tango type to extract
 * @param received Any returned by device server
 * @param pointer to argout
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */


static void tango_any_to_argout(long argout_type, long tango_type, CORBA::Any received, void *argout, long *error)
{
	if (argout == NULL && argout_type != D_VOID_TYPE)
	{
		Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
				(const char*)"argout is NULL ", 
		                (const char *)"tango_any_to_argout()");
	}

	switch(argout_type) 
	{
		case D_VOID_TYPE : 
		{
			if (tango_type != Tango::DEV_VOID)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_VOID", 
				                (const char *)"tango_any_to_argout()");
			}
			break;
		}

		case D_SHORT_TYPE :
		{
			short tango_short;
			short *argout_short;
			Tango::DevState tango_state;

			if ((tango_type != Tango::DEV_SHORT) &&
			    (tango_type != Tango::DEV_STATE))
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_SHORT", 
				                (const char *)"tango_any_to_argout()");
			}
			argout_short = (short*)argout;
			if (tango_type == Tango::DEV_SHORT)
			{
				received >>= tango_short;
				*argout_short = tango_short;
			}
			if (tango_type == Tango::DEV_STATE)
			{
				received >>= tango_state;
//
// convert TANGO:Devstate type to equivalent TACO state
//
				switch (tango_state) 
				{
					case (Tango::ON) : *argout_short = DEVON; 
					            break;
					case (Tango::OFF) : *argout_short = DEVOFF; 
					             break;
					case (Tango::CLOSE) : *argout_short = DEVCLOSE; 
					             break;
					case (Tango::OPEN) : *argout_short = DEVOPEN; 
					             break;
					case (Tango::INSERT) : *argout_short = DEVINSERTED; 
					             break;
					case (Tango::EXTRACT) : *argout_short = DEVEXTRACTED; 
					             break;
					case (Tango::MOVING) : *argout_short = DEVMOVING; 
					             break;
					case (Tango::STANDBY) : *argout_short = DEVSTANDBY; 
					             break;
					case (Tango::FAULT) : *argout_short = DEVFAULT; 
					             break;
					case (Tango::INIT) : *argout_short = DEVINIT; 
					             break;
					case (Tango::RUNNING) : *argout_short = DEVRUN; 
					             break;
					case (Tango::ALARM) : *argout_short = DEVALARM; 
					             break;
					case (Tango::DISABLE) : *argout_short = DEVDISABLED; 
					             break;
					default : *argout_short = DEVUNKNOWN; 
					             break;
				};
			}
			break;
		}

		case D_USHORT_TYPE :
		{
			unsigned short tango_ushort;
			unsigned short *argout_ushort;

			if (tango_type != Tango::DEV_USHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_USHORT", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_ushort;
			argout_ushort = (unsigned short*)argout;
			*argout_ushort = tango_ushort;
			break;
		}

		case D_LONG_TYPE :
		{
			long tango_long;
			long *argout_long;

			if (tango_type != Tango::DEV_LONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_LONG", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_long;
			argout_long = (long*)argout;
			*argout_long = tango_long;
			break;
		}

		case D_ULONG_TYPE :
		{
			unsigned long tango_ulong;
			unsigned long *argout_ulong;

			if (tango_type != Tango::DEV_ULONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_ULONG", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_ulong;
			argout_ulong = (unsigned long*)argout;
			*argout_ulong = tango_ulong;
			break;
		}

		case D_FLOAT_TYPE :
		{
			float tango_float;
			float *argout_float;

			if (tango_type != Tango::DEV_FLOAT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_FLOAT", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_float;
			argout_float = (float*)argout;
			*argout_float = tango_float;
			break;
		}

		case D_DOUBLE_TYPE :
		{
			double tango_double;
			double *argout_double;

			if (tango_type != Tango::DEV_DOUBLE)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_DOUBLE", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_double;
			argout_double = (double*)argout;
			*argout_double = tango_double;
			break;
		}

		case D_STRING_TYPE :
		{
			const char *tango_str;
			char **argout_str;

			if (tango_type != Tango::DEV_STRING)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_STRING", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_str;
			argout_str = (char**)argout;
			*argout_str = (char*)malloc(strlen(tango_str)+1);
			strcpy(*argout_str,tango_str);
			break;
		}

		case D_VAR_STRINGARR :
		{
			DevVarStringArray *argout_vsa;
			const Tango::DevVarStringArray *tango_vsa;

			if (tango_type != Tango::DEVVAR_STRINGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_STRINGARRAY", 
		                		(const char *)"tango_argin_to_any()");
			}
			received >>= tango_vsa;
			argout_vsa = (DevVarStringArray*)argout;
			if (argout_vsa->sequence == NULL)
			{
				argout_vsa->sequence = (char**)malloc(tango_vsa->length()*sizeof(char*));
			}
			argout_vsa->length = tango_vsa->length();
			for (long i=0; i< tango_vsa->length(); i++)
			{
				argout_vsa->sequence[i] = (char*)malloc(strlen((*tango_vsa)[i])+1);
				strcpy(argout_vsa->sequence[i],(*tango_vsa)[i]);
			}
			break;
		}

		case D_VAR_CHARARR :
		{
			DevVarCharArray *argout_vsha;
			const Tango::DevVarCharArray *tango_vsha;

			if (tango_type != Tango::DEVVAR_CHARARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_CHARARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vsha;
			argout_vsha = (DevVarCharArray*)argout;
			if (argout_vsha->sequence == NULL)
			{
				argout_vsha->sequence = (char*)malloc(tango_vsha->length()*sizeof(char));
			}
			argout_vsha->length = tango_vsha->length();
			for (long i=0; i< tango_vsha->length(); i++)
			{
				argout_vsha->sequence[i] = (*tango_vsha)[i];
			}
			break;
		}

		case D_VAR_SHORTARR :
		{
			DevVarShortArray *argout_vsha;
			const Tango::DevVarShortArray *tango_vsha;

			if (tango_type != Tango::DEVVAR_SHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_SHORTARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vsha;
			argout_vsha = (DevVarShortArray*)argout;
			if (argout_vsha->sequence == NULL)
			{
				argout_vsha->sequence = (short*)malloc(tango_vsha->length()*sizeof(short));
			}
			argout_vsha->length = tango_vsha->length();
			for (long i=0; i< tango_vsha->length(); i++)
			{
				argout_vsha->sequence[i] = (*tango_vsha)[i];
			}
			break;
		}

		case D_VAR_USHORTARR :
		{
			DevVarUShortArray *argout_vusha;
			const Tango::DevVarUShortArray *tango_vusha;

			if (tango_type != Tango::DEVVAR_USHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_USHORTARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vusha;
			argout_vusha = (DevVarUShortArray*)argout;
			if (argout_vusha->sequence == NULL)
			{
				argout_vusha->sequence = (unsigned short*)malloc(tango_vusha->length()*sizeof(unsigned short));
			}
			argout_vusha->length = tango_vusha->length();
			for (long i=0; i< tango_vusha->length(); i++)
			{
				argout_vusha->sequence[i] = (*tango_vusha)[i];
			}
			break;
		}

		case D_VAR_LONGARR :
		{
			DevVarLongArray *argout_vla;
			const Tango::DevVarLongArray *tango_vla;

			if ((tango_type != Tango::DEVVAR_LONGARRAY) &&
			    (tango_type != Tango::DEVVAR_ULONGARRAY))
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_LONGARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vla;
			argout_vla = (DevVarLongArray*)argout;
			if (argout_vla->sequence == NULL)
			{
				argout_vla->sequence = (long*)malloc(tango_vla->length()*sizeof(long));
			}
			argout_vla->length = tango_vla->length();
			for (long i=0; i< tango_vla->length(); i++)
			{
				argout_vla->sequence[i] = (*tango_vla)[i];
			}
			break;
		}

		case D_VAR_ULONGARR :
		{
			DevVarULongArray *argout_vula;
			const Tango::DevVarULongArray *tango_vula;

			if (tango_type != Tango::DEVVAR_ULONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_ULONGARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vula;
			argout_vula = (DevVarULongArray*)argout;
			if (argout_vula->sequence == NULL)
			{
				argout_vula->sequence = (unsigned long*)malloc(tango_vula->length()*sizeof(unsigned long));
			}
			argout_vula->length = tango_vula->length();
			for (long i=0; i< tango_vula->length(); i++)
			{
				argout_vula->sequence[i] = (*tango_vula)[i];
			}
			break;
		}

		case D_VAR_FLOATARR :
		{
			DevVarFloatArray *argout_vfa;
			const Tango::DevVarFloatArray *tango_vfa;

			if (tango_type != Tango::DEVVAR_FLOATARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_FLOATARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vfa;
			argout_vfa = (DevVarFloatArray*)argout;
			if (argout_vfa->sequence == NULL)
			{
				argout_vfa->sequence = (float*)malloc(tango_vfa->length()*sizeof(float));
			}
			argout_vfa->length = tango_vfa->length();
			for (long i=0; i< tango_vfa->length(); i++)
			{
				argout_vfa->sequence[i] = (*tango_vfa)[i];
			}
			break;
		}

		case D_VAR_DOUBLEARR :
		{
			DevVarDoubleArray *argout_vda;
			const Tango::DevVarDoubleArray *tango_vda;

			if (tango_type != Tango::DEVVAR_DOUBLEARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_DOUBLEARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			received >>= tango_vda;
			argout_vda = (DevVarDoubleArray*)argout;
			if (argout_vda->sequence == NULL)
			{
				argout_vda->sequence = (double*)malloc(tango_vda->length()*sizeof(double));
			}
			argout_vda->length = tango_vda->length();
			for (long i=0; i< tango_vda->length(); i++)
			{
				argout_vda->sequence[i] = (*tango_vda)[i];
			}
			break;
		}

		default:
			Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
					(const char*)"taco argout type not supported", 
			                (const char *)"tango_any_to_argout()");
	}

	return;
}

/**@ingroup tangoAPI
 * 
 * extract a TANGO type from an Any and convert it to a TACO argout of DEV_OPAQUE_TYPE
 *
 * @param argout_type argout type to convert
 * @param tango_type tango type to be extracted
 * @param received Any returned by device server
 * @param argout pointer to argout
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */
	
static void tango_any_to_argout_raw(long argout_type, long tango_type, CORBA::Any received, void *argout, long *error)
{
	dc_datacmd dc_datacmd;
	dc_error dc_error;
	DevOpaque *taco_opaque;
	long status;

	taco_opaque = (DevOpaque*)argout;

	if (argout == NULL && argout_type != D_VOID_TYPE)
	{
		Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
				(const char*)"argout is NULL ", 
		                (const char *)"tango_any_to_argout()");
	}

	switch(argout_type) 
	{
		case D_VOID_TYPE : 
		{
			void *taco_void = NULL;

			if (tango_type != Tango::DEV_VOID)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_VOID", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			dc_datacmd.argout_type = argout_type;
			dc_datacmd.argout = &taco_void;
			status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
			break;
		}

		case D_SHORT_TYPE :
		{
			short tango_short;
			Tango::DevState tango_state;
			short taco_short;

			if ((tango_type != Tango::DEV_SHORT) &&
			    (tango_type != Tango::DEV_STATE))
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_SHORT", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			if (tango_type == Tango::DEV_SHORT)
			{
				received >>= tango_short;
				taco_short = tango_short;
			}
			else
			{
				received >>= tango_state;
//
// convert TANGO:Devstate type to equivalent TACO state
//
				switch (tango_state) 
				{
					case (Tango::ON) : taco_short = DEVON; 
					            break;
					case (Tango::OFF) : taco_short = DEVOFF; 
					             break;
					case (Tango::CLOSE) : taco_short = DEVCLOSE; 
					             break;
					case (Tango::OPEN) : taco_short = DEVOPEN; 
					             break;
					case (Tango::INSERT) : taco_short = DEVINSERTED; 
					             break;
					case (Tango::EXTRACT) : taco_short = DEVEXTRACTED; 
					             break;
					case (Tango::MOVING) : taco_short = DEVMOVING; 
					             break;
					case (Tango::STANDBY) : taco_short = DEVSTANDBY; 
					             break;
					case (Tango::FAULT) : taco_short = DEVFAULT; 
					             break;
					case (Tango::INIT) : taco_short = DEVINIT; 
					             break;
					case (Tango::RUNNING) : taco_short = DEVRUN; 
					             break;
					case (Tango::ALARM) : taco_short = DEVALARM; 
					             break;
					case (Tango::DISABLE) : taco_short = DEVDISABLED; 
					             break;
					default : taco_short = DEVUNKNOWN; 
					             break;
				};
			}
			dc_datacmd.argout_type = argout_type;
			dc_datacmd.argout = &taco_short;
			status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
			break;
		}
		 
		case D_USHORT_TYPE :
		{
			unsigned short tango_ushort;
			unsigned short taco_ushort;

			if (tango_type != Tango::DEV_USHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_USHORT", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_ushort;
			taco_ushort = tango_ushort;
			dc_datacmd.argout_type = argout_type;
			dc_datacmd.argout = &taco_ushort;
			status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
			break;
		}
		 
		case D_LONG_TYPE :
		{
			long tango_long;
			long taco_long;

			if (tango_type != Tango::DEV_LONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_LONG", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_long;
			taco_long = tango_long;
			dc_datacmd.argout_type = argout_type;
			dc_datacmd.argout = &taco_long;
			status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
			break;
		}

		case D_ULONG_TYPE :
		{
			unsigned long tango_ulong;
			unsigned long taco_ulong;

			if (tango_type != Tango::DEV_ULONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_ULONG", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_ulong;
			taco_ulong = tango_ulong;
			dc_datacmd.argout_type = argout_type;
			dc_datacmd.argout = &taco_ulong;
			status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
			break;
		}

		case D_FLOAT_TYPE :
		{
			float tango_float;
			float taco_float;

			if (tango_type != Tango::DEV_FLOAT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_FLOAT", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_float;
			taco_float = tango_float;
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_float;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);       
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence; 
			break;
		}

		case D_DOUBLE_TYPE :
		{
			double tango_double;
			double taco_double;

			if (tango_type != Tango::DEV_DOUBLE)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_DOUBLE", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_double;
			taco_double = tango_double;
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_double;
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);       
			break;
		}

		case D_STRING_TYPE :
		{
			const char *tango_str;
			char *taco_str;

			if (tango_type != Tango::DEV_STRING)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_STRING", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_str;
			taco_str = (char*)malloc(strlen(tango_str)+1);
			strcpy(taco_str,tango_str);
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_str;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);       
			taco_opaque->length = dc_datacmd.length;
			taco_opaque->sequence = dc_datacmd.sequence;
			free(taco_str);
			break;
		}

		case D_VAR_STRINGARR :
		{
			DevVarStringArray taco_vsa;
			const Tango::DevVarStringArray *tango_vsa;
			long i;

			if (tango_type != Tango::DEVVAR_STRINGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_STRINGARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vsa;
			taco_vsa.sequence = (char**)malloc(tango_vsa->length()*sizeof(char*));
			taco_vsa.length = tango_vsa->length();
			for (i=0; i< tango_vsa->length(); i++)
			{
				taco_vsa.sequence[i] = (char*)malloc(strlen((*tango_vsa)[i])+1);
				strcpy(taco_vsa.sequence[i],(*tango_vsa)[i]);
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vsa;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			for (i=0; i< tango_vsa->length(); i++)
			{
				free(taco_vsa.sequence[i]);
			}
			free(taco_vsa.sequence);
			break;
		}

		case D_VAR_CHARARR :
		{
			DevVarCharArray taco_vsha;
			const Tango::DevVarCharArray *tango_vsha;

			if (tango_type != Tango::DEVVAR_CHARARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_CHARARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vsha;
			taco_vsha.sequence = (char*)malloc(tango_vsha->length()*sizeof(char));
			taco_vsha.length = tango_vsha->length();
			for (long i=0; i< tango_vsha->length(); i++)
			{
				taco_vsha.sequence[i] = (*tango_vsha)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vsha;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vsha.sequence);
			break;
		}
		
		case D_VAR_SHORTARR :
		{
			DevVarShortArray taco_vsha;
			const Tango::DevVarShortArray *tango_vsha;

			if (tango_type != Tango::DEVVAR_SHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_SHORTARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vsha;
			taco_vsha.sequence = (short*)malloc(tango_vsha->length()*sizeof(short));
			taco_vsha.length = tango_vsha->length();
			for (long i=0; i< tango_vsha->length(); i++)
			{
				taco_vsha.sequence[i] = (*tango_vsha)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vsha;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vsha.sequence);
			break;
		}

		case D_VAR_USHORTARR :
		{
			DevVarUShortArray taco_vusha;
			const Tango::DevVarUShortArray *tango_vusha;

			if (tango_type != Tango::DEVVAR_USHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_USHORTARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vusha;
			taco_vusha.sequence = (unsigned short*)malloc(tango_vusha->length()*sizeof(unsigned short));
			taco_vusha.length = tango_vusha->length();
			for (long i=0; i< tango_vusha->length(); i++)
			{
				taco_vusha.sequence[i] = (*tango_vusha)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vusha;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vusha.sequence);
			break;
		}

		case D_VAR_LONGARR :
		{
			DevVarLongArray taco_vla;
			const Tango::DevVarLongArray *tango_vla;

			if (tango_type != Tango::DEVVAR_LONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_LONGARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vla;
			taco_vla.sequence = (long*)malloc(tango_vla->length()*sizeof(long));
			taco_vla.length = tango_vla->length();
			for (long i=0; i< tango_vla->length(); i++)
			{
				taco_vla.sequence[i] = (*tango_vla)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vla;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vla.sequence);
			break;
		}

		case D_VAR_ULONGARR :
		{
			DevVarULongArray taco_vula;
			const Tango::DevVarULongArray *tango_vula;

			if (tango_type != Tango::DEVVAR_ULONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_ULONGARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vula;
			taco_vula.sequence = (unsigned long*)malloc(tango_vula->length()*sizeof(unsigned long));
			taco_vula.length = tango_vula->length();
			for (long i=0; i< tango_vula->length(); i++)
			{
				taco_vula.sequence[i] = (*tango_vula)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vula;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vula.sequence);
			break;
		}

		case D_VAR_FLOATARR :
		{
			DevVarFloatArray taco_vfa;
			const Tango::DevVarFloatArray *tango_vfa;

			if (tango_type != Tango::DEVVAR_FLOATARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_FLOATARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vfa;
			taco_vfa.sequence = (float*)malloc(tango_vfa->length()*sizeof(float));
			taco_vfa.length = tango_vfa->length();
			for (long i=0; i< tango_vfa->length(); i++)
			{
				taco_vfa.sequence[i] = (*tango_vfa)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vfa;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vfa.sequence);
			break;
		}

		case D_VAR_DOUBLEARR :
		{
			DevVarDoubleArray taco_vda;
			const Tango::DevVarDoubleArray *tango_vda;

			if (tango_type != Tango::DEVVAR_DOUBLEARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_DOUBLEARRAY", 
				                (const char *)"tango_any_to_argout_raw()");
			}
			received >>= tango_vda;
			taco_vda.sequence = (double*)malloc(tango_vda->length()*sizeof(double));
			taco_vda.length = tango_vda->length();
			for (long i=0; i< tango_vda->length(); i++)
			{
				taco_vda.sequence[i] = (*tango_vda)[i];
			}
                        dc_datacmd.argout_type = argout_type;
                        dc_datacmd.argout = &taco_vda;
                        status = dc_dataconvert(&dc_datacmd, 1, &dc_error);
                        taco_opaque->length = dc_datacmd.length;
                        taco_opaque->sequence = dc_datacmd.sequence;              
			free(taco_vda.sequence);
			break;
		}

		default:
			Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
					(const char*)"taco argout type not supported", 
			                (const char *)"tango_any_to_argout_raw()");
	}

	return;
}
	
/**@ingroup tangoAPI
 * convert a TANGO type code to a TACO type code
 *
 * @param tango_type TANGO type code
 * 
 * @return TACO type code
 */
static long tango_to_taco_type(long tango_type)
{
	long taco_type;

	switch (tango_type)
	{
		case Tango::DEV_VOID : 
			taco_type = D_VOID_TYPE;
			break;
		case Tango::DEV_BOOLEAN : 
			taco_type = D_BOOLEAN_TYPE;
			break;
		case Tango::DEV_STATE :
		case Tango::DEV_SHORT : 
			taco_type = D_SHORT_TYPE;
			break;
		case Tango::DEV_USHORT : 
			taco_type = D_USHORT_TYPE;
			break;
		case Tango::DEV_LONG : 
			taco_type = D_LONG_TYPE;
			break;
		case Tango::DEV_ULONG : 
			taco_type = D_ULONG_TYPE;
			break;
		case Tango::DEV_FLOAT : 
			taco_type = D_FLOAT_TYPE;
			break;
		case Tango::DEV_DOUBLE : 
			taco_type = D_DOUBLE_TYPE;
			break;
		case Tango::DEV_STRING : 
			taco_type = D_STRING_TYPE;
			break;
		case Tango::DEVVAR_CHARARRAY : 
			taco_type = D_VAR_CHARARR;
			break;
		case Tango::DEVVAR_SHORTARRAY : 
			taco_type = D_VAR_SHORTARR;
			break;
		case Tango::DEVVAR_USHORTARRAY : 
			taco_type = D_VAR_USHORTARR;
			break;
		case Tango::DEVVAR_LONGARRAY : 
			taco_type = D_VAR_LONGARR;
			break;
		case Tango::DEVVAR_ULONGARRAY : 
			taco_type = D_VAR_ULONGARR;
			break;
		case Tango::DEVVAR_FLOATARRAY : 
			taco_type = D_VAR_FLOATARR;
			break;
		case Tango::DEVVAR_DOUBLEARRAY : 
			taco_type = D_VAR_DOUBLEARR;
			break;
		case Tango::DEVVAR_STRINGARRAY : 
			taco_type = D_VAR_STRINGARR;
			break;
		default : taco_type = -1;
	}
	return(taco_type);
}

extern DevCmdNameListEntry DevCmdNameList[];
extern int max_cmds;

/**@ingroup tangoAPI
 * Read the command value corresponding to the command string from the TACO resource 
 * database. This is the reverse of the function get_cmd_string() in util_api.c. 
 *
 * The resource name must follow the convention :
 * <b>CLASS/class_name/CMD/cmd_string: value</b>
 *
 * e.g. class/database/cmd/dbinfo: 10000
 *
 * FIRST : try to get the command from the global table of commands DevCmdNameList[]. 
 * If it is not there then try the database. This new algorithm means that TANGO
 * servers which use "standard" TACO commands will not need to define anything in the database.
 *
 * @param class_name class name
 * @param cmd_name command name
 * @param cmd_value command value
 * @param error will contain an appropriate error code if the corresponding call
 *		    	        returns a non-zero value.
 *
 * @return DS_NOTOK in case of failure, DS_WARNING is returned, if the function was 
 *	executed correctly, but no command name value was found in the database, DS_OK 
 *	otherwise
 */
static long get_cmd_value (char *class_name, char *cmd_name, long *cmd_value, long *error)
{
	static unsigned int tango_auto_cmd = DevTangoBase;
	char		res_path[LONG_NAME_SIZE];
	char		res_name[SHORT_NAME_SIZE];
	char		*ret_str = NULL;
	db_resource 	res_tab;
	int		i;

	*error = 0;
	*cmd_value = 0;

	for (i=0; i<max_cmds; i++)
	{
		if (strcmp(DevCmdNameList[i].name, cmd_name) == 0)
		{
			*cmd_value = DevCmdNameList[i].value;
			return(DS_OK);
		}
	}
/*
 * Create the resource path and the resource structure.
 *
 * first check to see whether the device belongs to another
 * nethost domain i.e. i_nethost != 0
 */

/*
 * use default nethost
 */
	sprintf(res_path, "CLASS/%s/CMD", class_name);

	sprintf (res_name, "%s", cmd_name);

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the command name string from the database.
 */

	/* Ignore error here to be able to use use taco->tango
	   interface without having a full taco system */
	db_getresource (res_path, &res_tab, 1, error);
	*error=0;

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
	{
/*
 * if there is no command number defined for this command in the
 * the database then return an automatically generated command
 * number which is simply TangoBase incremented by on each time.
 *
 * NOTE : this could cause problems because the same command
 *        from the same device server will be assigned different
 *        command numbers depending on when it is called and
 *        by which client. However this problem is considered
 *        minor compared to the advantages for generic clients
 *        i.e. no need to define command numbers in the database
 *        for tango anymore 
 *
 * andy 30jun03
 */
		*cmd_value = tango_auto_cmd;
		tango_auto_cmd++;
		return (DS_OK);
	}

	sscanf(ret_str,"%d",cmd_value);
	free(ret_str);

	return (DS_OK);
}

/**@ingroup tangoAPI
 * Check TANGO device connection. If not imported then import it. If bad connection 
 * has been signalled then reimport.	
 *
 * @param ds  client handle, updated if device (re)imported correctly
 * 
 * @return DS_OK if device has been (re)imported correctly or DS_NOTOK
 */
static long tango_dev_check(devserver ds, long *error)
{
	long dev_id;

	dev_id = ds->ds_id;
//
// is device imported yet ?
//
        if (dev_id == -1)
        {
                devserver ds_ptr;
                if ( tango_dev_import(ds->device_name, ds->dev_access, &ds_ptr, error)
                     != DS_OK) return(DS_NOTOK);
//
// was device really imported ?
//
                if (ds_ptr->ds_id == -1)                                           
                {
                        free(ds_ptr);
                        *error = DevErr_DeviceNotImportedYet;
                        return(DS_NOTOK);
                }
//
// device really imported - update device info in client's copy of devserver structure
//
                sprintf(ds->device_name,"%s",ds_ptr->device_name);
                sprintf(ds->device_class,"%s",ds_ptr->device_class);
                sprintf(ds->device_type,"%s",ds_ptr->device_type);
                sprintf(ds->server_name,"%s",ds_ptr->server_name);
                sprintf(ds->server_host,"%s",ds_ptr->server_host);
                ds->clnt                = ds_ptr->clnt;
                ds->ds_id               = ds_ptr->ds_id;
                ds->no_svr_conn         = ds_ptr->no_svr_conn;
                ds->rpc_conn_counter    = ds_ptr->rpc_conn_counter;
                ds->dev_access          = ds_ptr->dev_access;
                ds->i_nethost           = ds_ptr->i_nethost;
                ds->rpc_protocol        = ds_ptr->rpc_protocol;
                ds->rpc_timeout         = ds_ptr->rpc_timeout;
                free(ds_ptr);
        }
        else
        {
//
// does device need to be reimported ?
//
                if (tango_device[dev_id].flag == TANGO_BAD_CONNECTION)
                {
                        if (tango_dev_reimport(ds, error) != DS_OK)
                        {
                                *error = DevErr_BadServerConnection;               
                                return(DS_NOTOK);                                  
                        }                                                          
                }                                                                  
        }                                                                          

	return(DS_OK);
}

/**@ingroup tangoAPI
 * Recover TANGO error string stack from DevFailed exception.
 *
 * @param e DevFailed exception
 * @param dev_error_string global pointer to error string
 *
 *  
 * @return DS_OK or DS_NOTOK
 */
static long tango_dev_error_string(Tango::DevFailed tango_exception)
{
	for (int i=0; i<tango_exception.errors.length(); i++)
	{
		if (i == 0)
		{
			dev_error_string = (char*)malloc(strlen(tango_exception.errors[i].desc.in())+1);
			sprintf(dev_error_string,"%s",tango_exception.errors[i].desc.in());
		}
		else
		{
			dev_error_string = (char*)realloc(dev_error_string,strlen(dev_error_string)+strlen(tango_exception.errors[i].desc.in())+1);
			sprintf(dev_error_string+strlen(dev_error_string),"%s",tango_exception.errors[i].desc.in());
		}
	}
	return(DS_OK);
}
