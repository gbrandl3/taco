/*+*******************************************************************
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
 *		$Author: jkrueger1 $
 *
 * Original   :	December 1999
 *
 * Version    :	$Revision: 1.3 $
 *
 * Date	      : $Date: 2004-03-09 17:02:49 $
 *
 * Copyleft (c) 1999 by European Synchrotron Radiation Facility,
 *                      Grenoble, France
 *
 *********************************************************************-*/

#include <tango.h>
#include <OB/CORBA.h> 

#include <API.h>
#include <private/ApiP.h>
#include <cstdio>
#include <cassert>
#include <iostream>
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

static CORBA::ORB 			*orb;
static Tango::Database 			*tango_dbase;
static long 				tango_dbase_init = 0;
typedef struct _tango_device {
	long flag; 
	std::vector<std::string> 	cmd_name;
	std::vector<long> 		cmd_value;
	std::vector<long> 		argin_type;
	std::vector<long> 		argout_type;
	Tango::Device_var 		object;
	} _tango_device;
static std::vector<_tango_device> 	tango_device;
static long 				n_tango_device=0;
static int 				tango_port = 10000;
static char 				*tango_host_c_str = NULL;

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
 * @param db_name name of the database
 * @param db_host database host
 * @param db_port database port
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK
 */
long tango_db_import(char *db_name, char *db_host, long db_port, long *error)
{
	if (db_name == NULL) 
	{
		*error = DevErr_DbImportFailed;
		return(DS_NOTOK);
	}

// Connect to TANGO database
	try 
	{
//
// create a CORBA orb manually. This is needed for the string_to_object 
// call to connect to devices. This can be removed when the TANGO Device
// api is used and all this CORBA stuff is hidden.
//
                int 		_argc;
                char		**_argv;
		std::string 	db_host_str(db_host);
 
// Pass dummy arguments to init() because we don't have access to
// argc and argv
                _argc = 1;
                _argv = (char**)malloc(sizeof(char*));
                _argv[0] = (char*)"taco_tango";
 
                orb = CORBA::ORB_init(_argc, _argv);                            
	
		tango_dbase = new Tango::Database(db_host_str, db_port, orb);
	}
	catch (CORBA::Exception  &corba_exception) 
	{
		if (debug_flag & DBG_TANGO) 
			Tango::Util::print_exception(corba_exception); 
		*error = DevErr_DbImportFailed;
		return(DS_NOTOK);
	}
	
	if (CORBA::is_nil(tango_dbase->get_dbase()))
	{
		*error = DevErr_DbImportFailed;
		return(DS_NOTOK);
	}

	tango_dbase_init = 1;

	return(DS_OK);
}

/**@ingroup tangoAPI
 * import TANGO device
 *
 * @param dev_name name of device
 * @param access
 * @param ds_ptr
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return  DS_OK or DS_NOTOK
 */
long tango_dev_import(char *dev_name, long access, devserver *ds_ptr, long *error)
{
	_tango_device 	tango_device_tmp;
	long 		tango_device_free;

	if (!tango_dbase_init) 
	{
		try
		{
			char *tango_host_env_c_str = getenv("TANGO_HOST");
			if (tango_host_env_c_str == NULL)
			{
				std::cout <<"TANGO_HOST environment variable not set, set it and retry (e.g. TANGO_HOST=tango:10000)" << std::endl;
				return(DS_NOTOK);
			}
			std::string 		tango_host_env(tango_host_env_c_str);
						tango_host;
			std::string::size_type 	separator;
			
			separator = tango_host_env.find(":");
			if (separator != std::string::npos)
			{
				tango_port = ::atoi((tango_host_env.substr(separator+1)).c_str());
			}
			tango_host = tango_host_env.substr(0,separator);
			
			tango_host_c_str = (char*)malloc(strlen(tango_host.c_str())+1);
			strcpy(tango_host_c_str, tango_host.c_str());
			tango_db_import("database", tango_host_c_str, tango_port,error);
        	}
		catch (CORBA::Exception  &corba_exception)
		{
                	if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(corba_exception);
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
		Tango::DbDevImportInfo 	dev_info;
		std::string 		dev_name_str(dev_name);

		dev_info = tango_dbase->import_device(dev_name_str);

//
// device defined but not exported - case for stateless import
//
                if (dev_info.exported == 0)
                {
			if (dev_notimported_init(dev_name,access,0,ds_ptr,error) == DS_NOTOK)
				return(DS_NOTOK);
			(*ds_ptr)->ds_id = -1;
			(*ds_ptr)->dev_access = access;
			(*ds_ptr)->rpc_protocol = D_IIOP;
                        return(DS_OK);
                }
                CORBA::Object_var obj = orb->string_to_object(dev_info.ior.c_str());
                CORBA::PolicyList policy;
                CORBA::Any any;
                CORBA::Object_var policy_obj;
		CORBA::ULong timeout=3000;
 
                policy.length(1);
 
                any <<= timeout;
 
// set timeout to 3 seconds
 
                policy[0] = orb->create_policy(OB::TIMEOUT_POLICY_ID,any);
                policy_obj = obj->_set_policy_overrides(policy,CORBA::ADD_OVERRIDE);
                tango_device_tmp.object = Tango::Device::_narrow(policy_obj);
                tango_device_tmp.flag = TANGO_DEV_IMPORTED;
        }
//
// failed to import - device does not exist ?
//
        catch (CORBA::Exception  &corba_exception)
	{
                if (debug_flag & DBG_TANGO) 
			Tango::Util::print_exception(corba_exception);
		*error = DevErr_DeviceOfThisNameNotServed;
                return(DS_NOTOK);
        }
//
// device defined but could not be narrowed - case for stateless import
//
        if (CORBA::is_nil(tango_device_tmp.object))
        {
		if (dev_notimported_init(dev_name,access,0,ds_ptr,error) == DS_NOTOK)
			return(DS_NOTOK);
                (*ds_ptr)->ds_id = -1;
                (*ds_ptr)->rpc_protocol = D_IIOP;
                return(DS_OK);
        }
//
// initialise command list for device
//
	try
	{
		Tango::DevCmdInfoList_var 	cmd_query = tango_device_tmp.object->command_list_query();
		Tango::DevInfo_var 		dev_info = tango_device_tmp.object->info();

		tango_device_tmp.cmd_name = std::vector<std::string>(cmd_query->length());
		tango_device_tmp.cmd_value = std::vector<long>(cmd_query->length());
		tango_device_tmp.argin_type = std::vector<long>(cmd_query->length());
		tango_device_tmp.argout_type = std::vector<long>(cmd_query->length());
		for (long i = 0;i < cmd_query->length();i++)
		{
			long error;
			tango_device_tmp.cmd_name[i] = std::string((*cmd_query)[i].cmd_name);
			get_cmd_value((char*)dev_info->dev_class,
			              (char*)tango_device_tmp.cmd_name[i].c_str(),
			              &tango_device_tmp.cmd_value[i],
			              &error);
			tango_device_tmp.argin_type[i] = (*cmd_query)[i].in_type;
			tango_device_tmp.argout_type[i] = (*cmd_query)[i].out_type;
		}
        	if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
        	{
                	*error = DevErr_InsufficientMemory;
                	return(DS_NOTOK);
        	}
		tango_device_free = -1;
		for (long i = 0; i < tango_device.size(); i++)
			if (tango_device[i].flag == TANGO_DEV_FREE)
				tango_device_free = i;
		if (tango_device_free < 0)
		{
			tango_device.push_back(tango_device_tmp);
			n_tango_device = tango_device.size() - 1;
		}
		else
		{
			tango_device[tango_device_free] = tango_device_tmp;
			n_tango_device = tango_device_free;
		}
        	strncpy((*ds_ptr)->device_name, dev_name, sizeof((*ds_ptr)->device_name));
        	strncpy((*ds_ptr)->device_class, dev_info->dev_class, sizeof((*ds_ptr)->device_class));
        	strncpy((*ds_ptr)->device_type, "TANGO_Device", sizeof((*ds_ptr)->device_type));
        	strncpy((*ds_ptr)->server_name, "Unknown", sizeof((*ds_ptr)->server_name));
        	strncpy((*ds_ptr)->server_host, "Unknown", sizeof((*ds_ptr)->server_host));
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
                if (debug_flag & DBG_TANGO) 
			Tango::Util::print_exception(e);
//
// device command query failed - case for stateless import
// 
                if (dev_notimported_init(dev_name,access,0,ds_ptr,error) == DS_NOTOK)
			return(DS_NOTOK);
                (*ds_ptr)->ds_id = -1;
                (*ds_ptr)->rpc_protocol = D_IIOP; 
                return(DS_OK);
        }
	n_tango_device++;

	return(DS_OK);
}

/**@ingroup tangoAPI
 * free a TANGO device
 * 
 * @param ds  device server structure
 * @param error	  contains the error code if function returns DS_NOTOK
 *
 * @return DS_OK 
 */
long tango_dev_free(devserver ds, long *error)
{
//	_tango_device tango_device_tmp;

//	delete tango_device[ds->ds_id].cmd_name;
//	delete tango_device[ds->ds_id].cmd_value;
//	delete tango_device[ds->ds_id].argin_type;
//	delete tango_device[ds->ds_id].argout_type;
	tango_device[ds->ds_id].flag = TANGO_DEV_FREE;
	free(ds);

	return(DS_OK);
}

/**@ingroup tangoAPI
 * reimport TANGO device
 *
 * @param ds  device server structure
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
			tango_db_import("database",tango_host_c_str, tango_port,error);
        	}
		catch (CORBA::Exception  &corba_exception)
		{
                	if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(corba_exception);
			*error = DevErr_DbImportFailed;
                	return(DS_NOTOK);
        	}
	}

// Get new IOR from TANGO database (server might have been restarted)

	try 
	{
		Tango::DbDevImportInfo 	dev_info;
		std::string 		device_name_str(ds->device_name);

                dev_info = tango_dbase->import_device(device_name_str);

                if (dev_info.exported == 0)
                {
			*error = DevErr_BadServerConnection;
                        return(DS_NOTOK);
                }
                CORBA::Object_var obj = orb->string_to_object(dev_info.ior.c_str());

        	CORBA::PolicyList policy;
        	CORBA::Any any;
        	CORBA::Object_var policy_obj;
 		CORBA::ULong timeout=3000;
 
        	policy.length(1);
	 
        	any <<= timeout;
 
// set timeout to 3 seconds

        	policy[0] = orb->create_policy(OB::TIMEOUT_POLICY_ID,any);
        	policy_obj = obj->_set_policy_overrides(policy,CORBA::ADD_OVERRIDE);
                tango_device[ds->ds_id].object = Tango::Device::_narrow(policy_obj);
                tango_device[ds->ds_id].flag = TANGO_DEV_IMPORTED;
        }
        catch (CORBA::Exception  &corba_exception)
	{
                if (debug_flag & DBG_TANGO) 
			Tango::Util::print_exception(corba_exception);
		*error = DevErr_BadServerConnection;
                return(DS_NOTOK);
        }

        if (CORBA::is_nil(tango_device[ds->ds_id].object))
        {
		*error = DevErr_BadServerConnection;
                return(DS_NOTOK);
        }

	return(DS_OK);
}

/**@ingroup tangoAPI
 * execute a command on a TANGO device
 * 
 * @param ds  		device server structure
 * @param error	  	contains the error code if function returns DS_NOTOK
 * @param cmd 		command to execute
 * @param argin 	pointer to argin
 * @param argin_type	argin type
 * @param argout	pointer to argout
 * @param argout_type	argout type
 *
 * @return DS_OK or DS_NOTOK
 */
long tango_dev_putget(devserver ds, long cmd, void *argin, long argin_type,
                      void *argout, long argout_type, long *error)
{
	long	i_cmd = -1, 
		dev_id;
	char 	*cmd_name = NULL;

	if (tango_dev_check(ds, error) != DS_OK) 
		return(DS_NOTOK);

	dev_id = ds->ds_id;
//
// check if command has already been executed, if not then
// read command string using its value from TACO database
//
	for (long i=0; i<tango_device[dev_id].cmd_value.size(); i++)
		if (tango_device[dev_id].cmd_value[i] == cmd)
		{
			i_cmd = i;
			break;
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
			CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd], argin, error);

// execute command using command_inout() method
                	CORBA::Any_var received = tango_device[dev_id].object->command_inout(tango_device[dev_id].cmd_name[i_cmd].c_str(),send);

// extract output argument from Any
			tango_any_to_argout(argout_type, tango_device[dev_id].argout_type[i_cmd], received, argout, error);
        	}
        	catch (Tango::DevFailed  &tango_exception)
		{
                	if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(tango_exception);
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
                        if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(e);
                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
			if (tango_dev_check(ds,error) != DS_OK)
			{
		        	*error = DevErr_BadServerConnection;
                        	return(DS_NOTOK);
			}
//
// if import works then execute the command again i.e. immediate reconnection
//
			try 
			{
//		
// insert input argument into Any type
				CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd], argin, error);
		
// execute command using command_inout() method
		               	CORBA::Any_var received = tango_device[dev_id].object->command_inout(tango_device[dev_id].cmd_name[i_cmd].c_str(),send);
		
// extract output argument from Any
				tango_any_to_argout(argout_type, tango_device[dev_id].argout_type[i_cmd], received, argout, error);
		        }
		        catch (Tango::DevFailed  &tango_exception)
		        {
		        	if (debug_flag & DBG_TANGO) 
					Tango::Util::print_exception(tango_exception);
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
	                        if (debug_flag & DBG_TANGO) 
					Tango::Util::print_exception(corba_exception);
	                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
	                        *error = DevErr_BadServerConnection;
	                        return(DS_NOTOK);
	                }
		}
	}

	return(DS_OK);
}

/**@ingroup tangoAPI
 * execute a command on a TANGO device and return the data in DEV_OPAQUE_TYP
 * 
 * @param ds  		device server structure
 * @param error	  	contains the error code if function returns DS_NOTOK
 * @param cmd 		command to execute
 * @param argin 	pointer to argin
 * @param argin_type	argin type
 * @param argout	pointer to argout
 * @param argout_type	argout type
 *
 * @return DS_OK or NOTOK
 */ 
long tango_dev_putget_raw(devserver ds, long cmd, void *argin, long argin_type,
                      void *argout, long argout_type, long *error)
{
	long	i_cmd = -1, 
		dev_id;
	char 	*cmd_name = NULL;

	if (tango_dev_check(ds,error) != DS_OK) 
		return(DS_NOTOK);

	dev_id = ds->ds_id;
//
// check if command has already been executed, if not then
// read command string using its value from TACO database
//
	for (long i=0; i<tango_device[dev_id].cmd_value.size(); i++)
		if (tango_device[dev_id].cmd_value[i] == cmd)
		{
			i_cmd = i;
			break;
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
			CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd], argin, error);

// execute command using command_inout() method
                	CORBA::Any_var received = tango_device[dev_id].object->command_inout(tango_device[dev_id].cmd_name[i_cmd].c_str(),send);

// extract output argument from Any
			tango_any_to_argout_raw(argout_type, tango_device[dev_id].argout_type[i_cmd], received, argout, error);
        	}
                catch (Tango::DevFailed  &tango_exception)
                {
                        if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(tango_exception);
// TODO - improve TANGO to TACO error handling here
                        *error = DevErr_CommandFailed;
                        return(DS_NOTOK);
                }
//
// CORBA exception, assume server is down, try to reimport immediately
//
                catch (CORBA::Exception  &corba_exception)
                {
                        if (debug_flag & DBG_TANGO) 
				Tango::Util::print_exception(corba_exception);
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
				CORBA::Any send = tango_argin_to_any(argin_type, tango_device[dev_id].argin_type[i_cmd], argin, error);
		
// execute command using command_inout() method
		               	CORBA::Any_var received = tango_device[dev_id].object->command_inout(tango_device[dev_id].cmd_name[i_cmd].c_str(),send);
		
// extract output argument from Any
				tango_any_to_argout_raw(argout_type, tango_device[dev_id].argout_type[i_cmd], received, argout, error);
		        }
		        catch (Tango::DevFailed  &tango_exception)
		        {
		        	if (debug_flag & DBG_TANGO) 
					Tango::Util::print_exception(tango_exception);
// TODO - improve TANGO to TACO error handling here
	                        *error = DevErr_CommandFailed;
	                        return(DS_NOTOK);
	                }
	                catch (CORBA::Exception  &corba_exception)
	                {
	                        if (debug_flag & DBG_TANGO) 
					Tango::Util::print_exception(corba_exception);
	                        tango_device[dev_id].flag = TANGO_BAD_CONNECTION;
	                        *error = DevErr_BadServerConnection;
	                        return(DS_NOTOK);
	                }
		}
	}
	return(DS_OK);
}

/**@ingroup tangoAPI
 * Returns a sequence of structures containing all available commands, names, input 
 * and output data types for a TANGO device. 
 * 
 * Command names and data types are read from the command list in the device server 
 * by calling device->cmd_query.
 *
 * Command values are read from the TACO database
 *
 * Data type descriptions have to be specified as CLASS resources as:
 * 	- CLASS/class_name/cmd_name/IN_TYPE:
 * 	- CLASS/class_name/cmd_name/OUT_TYPE:
 * 
 * @param ds  		device server structure
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
 * @param varcmdarr 	sequence of DevCmdInfo structures.
 *		
 * @return DS_OK or NOTOK
 */ 
long tango_dev_cmd_query (devserver ds, DevVarCmdArray *varcmdarr, long *error)
{
	if (tango_dev_check(ds,error) != DS_OK) 
		return(DS_NOTOK);

	long dev_id = ds->ds_id;
	*error = 0;

/*
 * Allocate memory for a sequence of DevCmdInfo structures
 * returned with varcmdarr.
 */
	varcmdarr->length   = tango_device[dev_id].cmd_value.size();
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
		strncat(varcmdarr->sequence[i].cmd_name,tango_device[dev_id].cmd_name[i].c_str(),32);
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
 * Ping a TANGO device
 * 
 * @param ds  		device server structure
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
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
                tango_device[dev_id].object->ping();
        }
        catch (CORBA::Exception  &corba_exception)
	{
		if (debug_flag & DBG_TANGO) 
			Tango::Util::print_exception(corba_exception);
// TODO - improve TANGO to TACO error handling here
                *error = DevErr_CommandFailed;
                return(DS_NOTOK);
        } 

	return (DS_OK);
}

/**@ingroup tangoAPIintern
 * convert a TACO argin type to a TANGO type and return it in an Any
 *
 * @param argin_type 	TACO argin type to convert
 * @param argin		pointer to argin
 * @param tango_type	TANGO argin type to convert
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
 * 
 * @return argin converted to CORBA::Any 
 */         
static CORBA::Any tango_argin_to_any(long argin_type, long tango_type, void *argin, long *error)
{
	CORBA::Any send;

	if (argin == NULL && argin_type != D_VOID_TYPE)
	{
		Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
				(const char *)"argin is NULL ", (const char *)"tango_argin_to_any()");
	}

	switch(argin_type) 
	{
		case D_VOID_TYPE : 
			if (tango_type != Tango::DEV_VOID)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_VOID ", 
		                		(const char *)"tango_argin_to_any()");
			}
			break;
		case D_SHORT_TYPE :
			if (tango_type != Tango::DEV_SHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_SHORT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				short *argin_short = (short*)argin;
				send <<= *argin_short;
			}
			break;

		case D_USHORT_TYPE :
			if (tango_type != Tango::DEV_USHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_USHORT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				unsigned short *argin_short = (unsigned short*)argin;
				send <<= *argin_short;
			}
			break;
			
		case D_LONG_TYPE :
			if (tango_type != Tango::DEV_LONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_LONG ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				long *argin_long = (long*)argin;
				send <<= *argin_long;
			}
			break;
			
		case D_ULONG_TYPE :
			if (tango_type != Tango::DEV_ULONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_ULONG ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				unsigned long *argin_ulong = (unsigned long*)argin;
				send <<= *argin_ulong;
			}
			break;
			
		case D_FLOAT_TYPE :
			if (tango_type != Tango::DEV_FLOAT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_FLOAT ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				float *argin_float = (float*)argin;
				send <<= *argin_float;
			}
			break;
			
		case D_DOUBLE_TYPE :
			if (tango_type != Tango::DEV_DOUBLE)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEV_DOUBLE ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				double *argin_double = (double*)argin;
				send <<= *argin_double;
			}
			break;
			
		case D_STRING_TYPE :
			if (tango_type != Tango::DEV_STRING)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argin type is not Tango::DEV_STRING", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				char **argin_str = (char**)argin;
				send <<= CORBA::string_dup(*argin_str);
			}
			break;

		case D_VAR_STRINGARR :
			if (tango_type != Tango::DEVVAR_STRINGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_STRINGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarStringArray *argin_vsa = (DevVarStringArray*)argin;
				Tango::DevVarStringArray *tango_vsa = new Tango::DevVarStringArray;
				tango_vsa->length(argin_vsa->length);
				for (long i=0; i<argin_vsa->length; i++)
					(*tango_vsa)[i] = CORBA::string_dup(argin_vsa->sequence[i]);
				send <<= tango_vsa;
			}
			break;
			
		case D_VAR_SHORTARR :
			if (tango_type != Tango::DEVVAR_SHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_SHORTARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarShortArray *argin_vsha = (DevVarShortArray*)argin;
				Tango::DevVarShortArray *tango_vsha = new Tango::DevVarShortArray;
				tango_vsha->length(argin_vsha->length);
				for (long i=0; i<argin_vsha->length; i++)
					(*tango_vsha)[i] = argin_vsha->sequence[i];
				send <<= tango_vsha;
			}
			break;
			
		case D_VAR_USHORTARR :
			if (tango_type != Tango::DEVVAR_USHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_USHORTARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarUShortArray *argin_vusha = (DevVarUShortArray*)argin;
				Tango::DevVarUShortArray *tango_vusha = new Tango::DevVarUShortArray;
				tango_vusha->length(argin_vusha->length);
				for (long i=0; i<argin_vusha->length; i++)
					(*tango_vusha)[i] = argin_vusha->sequence[i];
				send <<= tango_vusha;
			}
			break;
			
		case D_VAR_LONGARR :
			if (tango_type != Tango::DEVVAR_LONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_LONGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarLongArray *argin_vla = (DevVarLongArray*)argin;
				Tango::DevVarLongArray *tango_vla = new Tango::DevVarLongArray;
				tango_vla->length(argin_vla->length);
				for (long i=0; i<argin_vla->length; i++)
					(*tango_vla)[i] = argin_vla->sequence[i];
				send <<= tango_vla;
			}
			break;
			
		case D_VAR_ULONGARR :
			if (tango_type != Tango::DEVVAR_ULONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_ULONGARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarULongArray *argin_vula = (DevVarULongArray*)argin;
				Tango::DevVarULongArray *tango_vula = new Tango::DevVarULongArray;
				tango_vula->length(argin_vula->length);
				for (long i=0; i<argin_vula->length; i++)
					(*tango_vula)[i] = argin_vula->sequence[i];
				send <<= tango_vula;
			}
			break;
			
		case D_VAR_FLOATARR :
			if (tango_type != Tango::DEVVAR_FLOATARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_FLOATARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarFloatArray *argin_vfa = (DevVarFloatArray*)argin;
				Tango::DevVarFloatArray *tango_vfa = new Tango::DevVarFloatArray;
				tango_vfa->length(argin_vfa->length);
				for (long i=0; i<argin_vfa->length; i++)
					(*tango_vfa)[i] = argin_vfa->sequence[i];
				send <<= tango_vfa;
			}
			break;
		}
			
		case D_VAR_DOUBLEARR :
			if (tango_type != Tango::DEVVAR_DOUBLEARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TANGO argin type not Tango::DEVVAR_DOUBLEARRAY ", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarDoubleArray *argin_vda = (DevVarDoubleArray*)argin;
				Tango::DevVarDoubleArray *tango_vda = new Tango::DevVarDoubleArray;
				tango_vda->length(argin_vda->length);
				for (long i=0; i<argin_vda->length; i++)
					(*tango_vda)[i] = argin_vda->sequence[i];
				send <<= tango_vda;
			}
			break;
		}
			
		default:
			Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"TACO type no supported", 
		                		(const char *)"tango_argin_to_any()");
	}

	return(send);
}

/**@ingroup tangoAPIintern
 * extract a TANGO type from an Any and convert it to a TACO argout
 *
 * @param argout_type 	TACO argout type to convert
 * @param argout	pointer to argout
 * @param tango_type	TANGO argout type to convert
 * @param received 	Any returned by device server
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
 * 
 * @return argin converted to CORBA::Any 
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
			if (tango_type != Tango::DEV_VOID)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_VOID", 
				                (const char *)"tango_any_to_argout()");
			}
			break;

		case D_SHORT_TYPE :
			if ((tango_type != Tango::DEV_SHORT) &&
			    (tango_type != Tango::DEV_STATE))
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_SHORT", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				short tango_short;
				short *argout_short = (short*)argout;;
				Tango::DevState tango_state;

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
						case (Tango::ON) : 
							*argout_short = DEVON; 
					        	break;
						case (Tango::OFF) : 
							*argout_short = DEVOFF; 
					        	break;
						case (Tango::CLOSE) : 
							*argout_short = DEVCLOSE; 
					        	break;
						case (Tango::OPEN) : 
							*argout_short = DEVOPEN; 
					        	break;
						case (Tango::INSERT) : 
							*argout_short = DEVINSERTED; 
					        	break;
						case (Tango::EXTRACT) : 
							*argout_short = DEVEXTRACTED; 
					        	break;
						case (Tango::MOVING) : 
							*argout_short = DEVMOVING; 
					        	break;
						case (Tango::STANDBY) : 
							*argout_short = DEVSTANDBY; 
					        	break;
						case (Tango::FAULT) : 
							*argout_short = DEVFAULT; 
					        	break;
						case (Tango::INIT) : 
							*argout_short = DEVINIT; 
					        	break;
						case (Tango::RUNNING) : 
							*argout_short = DEVRUN; 
					        	break;
						case (Tango::ALARM) : 
							*argout_short = DEVALARM; 
					        	break;
						case (Tango::DISABLE) : 
							*argout_short = DEVDISABLED; 
					        	break;
						default : 
							*argout_short = DEVUNKNOWN; 
					        	break;
					}
				}
			}
			break

		case D_USHORT_TYPE :
			if (tango_type != Tango::DEV_USHORT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_USHORT", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				unsigned short tango_ushort;
				received >>= tango_ushort;
				unsigned short *argout_ushort = (unsigned short*)argout;
				*argout_ushort = tango_ushort;
			}
			break;

		case D_LONG_TYPE :
			if (tango_type != Tango::DEV_LONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_LONG", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				long tango_long;
				long *argout_long = (long*)argout;

				received >>= tango_long;
				*argout_long = tango_long;
			}
			break;

		case D_ULONG_TYPE :
			if (tango_type != Tango::DEV_ULONG)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_ULONG", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				unsigned long tango_ulong;
				unsigned long *argout_ulong = (unsigned long*)argout;

				received >>= tango_ulong;
				*argout_ulong = tango_ulong;
			}
			break;

		case D_FLOAT_TYPE :
			if (tango_type != Tango::DEV_FLOAT)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_FLOAT", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				float tango_float;
				float *argout_float = (float*)argout;;
				received >>= tango_float;
				*argout_float = tango_float;
			}
			break;

		case D_DOUBLE_TYPE :
			if (tango_type != Tango::DEV_DOUBLE)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_DOUBLE", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				double tango_double;
				double *argout_double = (double*)argout;;

				received >>= tango_double;
				*argout_double = tango_double;
			}
			break;

		case D_STRING_TYPE :
			if (tango_type != Tango::DEV_STRING)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEV_STRING", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				const char *tango_str;
				char **argout_str = (char**)argout;;

				received >>= tango_str;
				*argout_str = (char*)malloc(strlen(tango_str)+1);
				strcpy(*argout_str,tango_str);
			}
			break;

		case D_VAR_STRINGARR :
			if (tango_type != Tango::DEVVAR_STRINGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_STRINGARRAY", 
		                		(const char *)"tango_argin_to_any()");
			}
			{
				DevVarStringArray *argout_vsa = (DevVarStringArray*)argout;;
				const Tango::DevVarStringArray *tango_vsa;

				received >>= tango_vsa;
				if (argout_vsa->sequence == NULL)
					argout_vsa->sequence = (char**)malloc(tango_vsa->length()*sizeof(char*));
				argout_vsa->length = tango_vsa->length();
				for (long i=0; i< tango_vsa->length(); i++)
				{
					argout_vsa->sequence[i] = (char*)malloc(strlen((*tango_vsa)[i])+1);
					strcpy(argout_vsa->sequence[i],(*tango_vsa)[i]);
				}
			}
			break;

		case D_VAR_SHORTARR :
			if (tango_type != Tango::DEVVAR_SHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_SHORTARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarShortArray *argout_vsha = (DevVarShortArray*)argout;;
				const Tango::DevVarShortArray *tango_vsha;

				received >>= tango_vsha;
				if (argout_vsha->sequence == NULL)
					argout_vsha->sequence = (short*)malloc(tango_vsha->length()*sizeof(short));
				argout_vsha->length = tango_vsha->length();
				for (long i=0; i< tango_vsha->length(); i++)
					argout_vsha->sequence[i] = (*tango_vsha)[i];
			}
			break;

		case D_VAR_USHORTARR :
			if (tango_type != Tango::DEVVAR_USHORTARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_USHORTARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarUShortArray *argout_vusha = (DevVarUShortArray*)argout;
				const Tango::DevVarUShortArray *tango_vusha;

				received >>= tango_vusha;
				if (argout_vusha->sequence == NULL)
					argout_vusha->sequence = (unsigned short*)malloc(tango_vusha->length()*sizeof(unsigned short));
				argout_vusha->length = tango_vusha->length();
				for (long i=0; i< tango_vusha->length(); i++)
					argout_vusha->sequence[i] = (*tango_vusha)[i];
			}
			break;

		case D_VAR_LONGARR :
			if ((tango_type != Tango::DEVVAR_LONGARRAY) &&
			    (tango_type != Tango::DEVVAR_ULONGARRAY))
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_LONGARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarLongArray *argout_vla = (DevVarLongArray*)argout;;
				const Tango::DevVarLongArray *tango_vla;

				received >>= tango_vla;
				if (argout_vla->sequence == NULL)
					argout_vla->sequence = (long*)malloc(tango_vla->length()*sizeof(long));
				argout_vla->length = tango_vla->length();
				for (long i=0; i< tango_vla->length(); i++)
					argout_vla->sequence[i] = (*tango_vla)[i];
			}
			break;

		case D_VAR_ULONGARR :
			if (tango_type != Tango::DEVVAR_ULONGARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_ULONGARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarULongArray *argout_vula = (DevVarULongArray*)argout;;
				const Tango::DevVarULongArray *tango_vula;

				received >>= tango_vula;
				if (argout_vula->sequence == NULL)
					argout_vula->sequence = (unsigned long*)malloc(tango_vula->length()*sizeof(unsigned long));
				argout_vula->length = tango_vula->length();
				for (long i=0; i< tango_vula->length(); i++)
					argout_vula->sequence[i] = (*tango_vula)[i];
			}
			break;

		case D_VAR_FLOATARR :
			if (tango_type != Tango::DEVVAR_FLOATARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_FLOATARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarFloatArray *argout_vfa = (DevVarFloatArray*)argout;;
				const Tango::DevVarFloatArray *tango_vfa;

				received >>= tango_vfa;
				if (argout_vfa->sequence == NULL)
					argout_vfa->sequence = (float*)malloc(tango_vfa->length()*sizeof(float));
				argout_vfa->length = tango_vfa->length();
				for (long i=0; i< tango_vfa->length(); i++)
					argout_vfa->sequence[i] = (*tango_vfa)[i];
			}
			break;

		case D_VAR_DOUBLEARR :
			if (tango_type != Tango::DEVVAR_DOUBLEARRAY)
			{
				Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
						(const char*)"tango argout type is not Tango::DEVVAR_DOUBLEARRAY", 
				                (const char *)"tango_any_to_argout()");
			}
			{
				DevVarDoubleArray *argout_vda = (DevVarDoubleArray*)argout;;
				const Tango::DevVarDoubleArray *tango_vda;

				received >>= tango_vda;
				if (argout_vda->sequence == NULL)
					argout_vda->sequence = (double*)malloc(tango_vda->length()*sizeof(double));
				argout_vda->length = tango_vda->length();
				for (long i=0; i< tango_vda->length(); i++)
					argout_vda->sequence[i] = (*tango_vda)[i];
			}
			break;

		default:
			Tango::Except::throw_exception((const char*)"DSAPI_IncorrectArguments", 
					(const char*)"taco argout type not supported", 
			                (const char *)"tango_any_to_argout()");
	}
	return;
}
	
/**@ingroup tangoAPIintern
 * extract a TANGO type from an Any and convert it to a TACO argout of DEV_OPAQUE_TYPE
 *
 * @param argout_type 	TACO argout type to convert
 * @param argout	pointer to argout
 * @param tango_type	TANGO argout type to convert
 * @param received 	Any returned by device server
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
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
	
/**@ingroup tangoAPIintern
 * convert a TANGO type code to a TACO type code
 * 
 * @param tango_type - TANGo type code
 *
 * @return  TACO type code
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

/**@ingroup tangoAPIintern
 * Read the command value corresponding to the command string from the TACO resource 
 * database. This is the reverse of the function get_cmd_string() in util_api.c. 
 * The resource name must follow the convention :
 * 
 * CLASS/class_name/CMD/cmd_string: value
 *
 * Example:
 * @verbatim
 * class/database/cmd/dbinfo: 10000
 * @endverbatim
 *
 * FIRST : try to get the command from the global table of commands DevCmdNameList[]. If it is 
 * not there then try the database. This new algorithm means that TANGO servers which use 
 * "standard" TACO commands will not need to define anything in the database.
 *
 * @param class_name 	class name
 * @param cmd_name   	command name
 * @param cmd_value  	command value
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
 *
 * @return  DS_OK or DS_NOTOK or DS_WARNING is returned, if the function was executed correctly, 
 *		but no command name value was found in the database.
 */
static long get_cmd_value (char *class_name, char *cmd_name, long *cmd_value, long *error)
{
	char		res_path[LONG_NAME_SIZE];
	char		res_name[SHORT_NAME_SIZE];
	char		*ret_str = NULL;
	db_resource 	res_tab;
	int		i;

	*error = 0;

	for (i=0; i<max_cmds; i++)
		if (strcmp(DevCmdNameList[i].name, cmd_name) == 0)
		{
			*cmd_value = DevCmdNameList[i].value;
			return(DS_OK);
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
	snprintf(res_path, sizeof(res_path), "CLASS/%s/CMD", class_name);
	strncpy(res_name, cmd_name, sizeof(res_name));

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the command name string from the database.
 */
	if (db_getresource (res_path, &res_tab, 1, error) == DS_NOTOK)
		return (DS_NOTOK);

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
		return (DS_WARNING);

	sscanf(ret_str, "%d", cmd_value);

	return (DS_OK);
}

/**@ingroup tangoAPIintern
 * Check TANGO device connection. If not imported then import it. If bad connection 
 * has been signalled then reimport. Returns DS_OK if device has been (re)imported correctly.
 * 
 * @param ds  		device server structure
 * @param error	  	Will contain an appropriate error code if the function returns DS_NOTOK
 *
 * @return  DS_OK or DS_NOTOK
 */
static long tango_dev_check(devserver ds, long *error)
{
	long dev_id = ds->ds_id;
//
// is device imported yet ?
//
	if (dev_id == -1)
	{
		devserver ds_ptr;
		if ( tango_dev_import(ds->device_name, ds->dev_access, &ds_ptr, error) != DS_OK) 
			return(DS_NOTOK);
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
		strncpy(ds->device_name, ds_ptr->device_name, sizeof(ds->device_name));
		strncpy(ds->device_class, ds_ptr->device_class, sizeof(ds->device_class));
		strncpy(ds->device_type, ds_ptr->device_type, sizeof(ds->device_type));
		strncpy(ds->server_name, ds_ptr->server_name, sizeof(ds->server_name));
		strncpy(ds->server_host, ds_ptr->server_host, sizeof(ds->server_host));
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
		if ((tango_device[dev_id].flag == TANGO_BAD_CONNECTION) && (tango_dev_reimport(ds, error) != DS_OK))
		{
			*error = DevErr_BadServerConnection;               
			return(DS_NOTOK);                                  
		}                                                                  
	}                                                                          
	return(DS_OK);
}

/**@ingroup tangoAPIintern
 * Recover TANGO error string stack from DevFailed exception.
 * 
 * @param tango_exception DevFailed exception
 * 
 * @return  DS_OK 
 */
static long tango_dev_error_string(Tango::DevFailed tango_exception)
{
	for (int i=0; i<tango_exception.errors.length(); i++)
	{
		if (i == 0)
		{
			dev_error_string = (char*)malloc(strlen(tango_exception.errors[i].desc.in())+1);
			strcpy(dev_error_string, tango_exception.errors[i].desc.in());
		}
		else
		{
			dev_error_string = (char*)realloc(dev_error_string,strlen(dev_error_string)+strlen(tango_exception.errors[i].desc.in())+1);
			strcat(dev_error_string, tango_exception.errors[i].desc.in());
		}
	}
	return(DS_OK);
}
