/*********************************************************************

 File:          Manager.c

 Project:       Device Servers with sun-rpc

 Description:   Source code for implementing a Network Manager

 Author(s):     Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.2 $

 Date:		$Date: 2003-05-09 15:26:24 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

			All Rights Reserved

**********************************************************************/

#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <ManagerP.h>
#include <signal.h>

/********************************
 *           GLOBALS            *
 ********************************/

void 		quit_server ();

extern config_flags 	c_flags;
extern char		*dshome;
extern char		*display;
extern char		nethost [SHORT_NAME_SIZE];
extern char		logfile [256];

server_conf		db_conf  = {0,0,0};
server_conf		msg_conf = {0,0,0};

dbserver_info   	db_info;


/***************************************
 *                                     *
 *  register message server to manager *
 *                                     *
 ***************************************/

_msg_manager_data *rpc_msg_register_1 (_register_data *register_data)
{
	static _msg_manager_data	msg_manager_data;
	FILE   *system_log = NULL;
	char   *time_string;
	time_t   clock;

/*
 *  get message server info
 */
	snprintf (msg_conf.host_name, sizeof(msg_conf.host_name) - 1, "%s",register_data->host_name);
	msg_conf.prog_number = register_data->prog_number;
	msg_conf.vers_number = register_data->vers_number;

/*
 *  write information to System.log file
 */
       	if ( (system_log = fopen (logfile, "a")) != NULL )
	{
		time (&clock);
		time_string = ctime (&clock);
		fprintf (system_log, "Message Server registered at : %s",time_string);
		fprintf (system_log, "msg_host = %s  prog_nu = %d  vers_nu = %d\n\n",
					    msg_conf.host_name,
					    msg_conf.prog_number,
					    msg_conf.vers_number);
		fclose (system_log);
	}
   	else
		fprintf (stderr,"cannot open System.log file.\n");
/*
 *  set manager return values
 */
	msg_manager_data.dshome  = dshome;
	msg_manager_data.display = "nada";

/*
 *  set configuration status
 */
	c_flags.msg_server = True;
	return (&msg_manager_data);
}

/**********************************************
 *                                            *
 * register static database server to manager *
 *                                            *
 **********************************************/

int *rpc_db_register_1 (_register_data *register_data)
{
	FILE   		*system_log = NULL;
	static int	status = 0;
	char   		*time_string;
	time_t 		clock;

/*
 *  get database server info
 */
	snprintf (db_conf.host_name, sizeof(db_conf.host_name) - 1, "%s",register_data->host_name);
	db_conf.prog_number = register_data->prog_number;
	db_conf.vers_number = register_data->vers_number;

/*
 *  write information to System.log file
 */
	if ( c_flags.request_log == True )
	{
		if ( (system_log = fopen (logfile, "a")) != NULL )
		{
			time (&clock);
			time_string = ctime (&clock);
			fprintf (system_log, "Database Server registered at : %s",time_string);
			fprintf (system_log, "db_host = %s  prog_nu = %d  vers_nu = %d\n\n",
					    db_conf.host_name,
					    db_conf.prog_number,
					    db_conf.vers_number);
			fclose (system_log);
		}
   		else
			fprintf (stderr,"cannot open System.log file.\n");
	}
/*
 *  set configuration status
 */
	c_flags.db_server = True;
	return (&status);
}

/***************************************************
 *                                                 *
 *  send static database server and message server *
 *  configuration back to the requesting process.  *
 *                                                 *
 ***************************************************/
_manager_data *rpc_get_config_4 (_register_data	*register_data)
{
	static _manager_data	manager_data;
	FILE   			*system_log = NULL;
	char   			*time_string;
	time_t 			clock;

	manager_data.error  = 0;
	manager_data.status = 0;
	manager_data.var_argument.length   = 0;
	manager_data.var_argument.sequence = NULL;

/*
 *  write information about the reading process
 *  (can be server or client) to the System.log file
 */
	if ( c_flags.request_log == True )
	{
       		if ( (system_log = fopen (logfile, "a")) != NULL )
		{
			time (&clock);
			time_string = ctime (&clock);
			fprintf (system_log, "Configuration request at : %s",time_string);
			fprintf (system_log, "Requesting process is running on %s with pid = %d\n",
					register_data->host_name, register_data->prog_number);
			fclose (system_log);
		}
		else
			fprintf (stderr,"cannot open System.log file.\n");
	}

/*
 *  set manager return values
 */
	manager_data.msg_info.host_name   = msg_conf.host_name;
	manager_data.msg_info.prog_number = msg_conf.prog_number;
	manager_data.msg_info.vers_number = msg_conf.vers_number;

	manager_data.db_info.host_name   = db_conf.host_name;
	manager_data.db_info.prog_number = db_conf.prog_number ;
	manager_data.db_info.vers_number = db_conf.vers_number ;

	manager_data.security            = c_flags.security;

	return (&manager_data);
}


/****************************************************************
 *     Quit Network Manager and its related applications        *
 *     like Message Server and Database Server                  *
 ****************************************************************/
void unreg_server (int signo)
{
	FILE    *system_log = NULL;
	char 	*time_string;
	time_t  clock;
	int	i;

#ifdef OSK
	if (signo != SIGQUIT && signo != SIGINT)
		return;
#endif /* OSK */
/*
 *  write system shutdown message to System.log file
 */
	if ( c_flags.request_log == True )
	{
		if ( (system_log = fopen (logfile, "a")) != NULL )
		{
			time (&clock);
			time_string = ctime (&clock);
			fprintf (system_log, "\nSystem shutdown at : %s",time_string);
			fclose (system_log);
		}
	}

/*
 *  quit database and message servers
 */
	if (c_flags.db_server)
		quit_server ( db_conf.host_name,
			      db_conf.prog_number,
			      db_conf.vers_number );
#ifdef unix
	if (c_flags.msg_server)
		quit_server ( msg_conf.host_name,
			      msg_conf.prog_number,
			      msg_conf.vers_number );
#endif /* unix */
/*
 *  unregister manager from portmapper
 */
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS);
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS_1);
	exit (-1);
}

/****************************************************************
 *     Quit remote server by RPC call                           *
 ****************************************************************/
void quit_server (char*host_name, long prog_number, long vers_number)
{
	CLIENT	*clnt = clnt_create (host_name, prog_number, vers_number, "udp");
	if (clnt != NULL)
	{
		clnt_control (clnt, CLSET_TIMEOUT, (char *) &timeout);
		clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &retry_timeout);

		clnt_call (clnt, RPC_QUIT_SERVER, (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL, timeout);

		clnt_destroy (clnt);
	}
}
	

