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
 * File:          Manager.c
 * 
 * Project:       Device Servers with sun-rpc
 *
 * Description:   Source code for implementing a Network Manager
 *
 * Author(s):     Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:		$Date: 2008-06-20 11:26:01 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <log4cpp/Category.hh>

#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <ManagerP.h>
#ifdef HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#	error Could not find signal.h
#endif

void 			quit_server (char *, long, long);

extern manager_config_flags 	c_flags;
extern char		*dshome;
extern char		*display;
extern char		nethost [];

static server_conf	db_conf  = {0,0,0};
static server_conf	msg_conf = {0,0,0};

// static dbserver_info   	db_info;

extern log4cpp::Category	*logStream;

/**
  *  register message server to manager 
  */
_msg_manager_data *rpc_msg_register_1 (_register_data *register_data)
{
	static _msg_manager_data	msg_manager_data;
	FILE   				*system_log = NULL;
	char   				*time_string;
	time_t   			clock;

/*
 *  get message server info
 */
	snprintf (msg_conf.host_name, sizeof(msg_conf.host_name), "%s", register_data->host_name);
	msg_conf.prog_number = register_data->prog_number;
	msg_conf.vers_number = register_data->vers_number;

/*
 *  write information to System.log file
 */
	if (c_flags.request_log)
	{
		logStream->noticeStream() << "Message Server registered." << log4cpp::eol;
		logStream->noticeStream() << "(msg_host = " << msg_conf.host_name << ", prog_nu = " << msg_conf.prog_number 
					<< ", vers_nu = " << msg_conf.vers_number << ")" << log4cpp::eol;
	}
/*
 *  set manager return values
 */
	msg_manager_data.dshome  = dshome;
	msg_manager_data.display = display;

/*
 *  set configuration status
 */
	c_flags.msg_server = True;
	return (&msg_manager_data);
}

/**
 * register static database server to manager 
 */
int *rpc_db_register_1 (_register_data *register_data)
{
	FILE   		*system_log = NULL;
	static int	status = 0;
	char   		*time_string;
	time_t 		clock;

/*
 *  get database server info
 */
	snprintf (db_conf.host_name, sizeof(db_conf.host_name), "%s",register_data->host_name);
	db_conf.prog_number = register_data->prog_number;
	db_conf.vers_number = register_data->vers_number;

/*
 *  write information to System.log file
 */
	if ( c_flags.request_log == True )
	{
		logStream->noticeStream() << "Database Server registered." << log4cpp::eol;
		logStream->noticeStream() << "(db_host = " << db_conf.host_name 
					<< ", prog_nu = " << db_conf.prog_number 
					<< ", vers_nu = " << db_conf.vers_number << ")" << log4cpp::eol;
	}
/*
 *  set configuration status
 */
	c_flags.db_server = True;
	return (&status);
}

/**
  *  Send static database server and message server 
  *  configuration back to the requesting process. 
  */
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
		logStream->noticeStream() << "Configuration request." << log4cpp::eol;
		logStream->noticeStream() << "(Requesting process is running on " << register_data->host_name << " with pid = " 
					<< register_data->prog_number << ")" << log4cpp::eol;
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


/**
  * Quit Network Manager and its related applications 
  * like Message Server and Database Server 
  */
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
		logStream->noticeStream() << "Received signal " << signo << log4cpp::eol;
		logStream->noticeStream() << "System shutdown." << log4cpp::eol << log4cpp::eol;
	}

/*
 *  quit database and message servers
 */
	if (c_flags.db_server)
		quit_server ( db_conf.host_name, db_conf.prog_number, db_conf.vers_number );
#ifdef unix
	if (c_flags.msg_server)
		quit_server ( msg_conf.host_name, msg_conf.prog_number, msg_conf.vers_number );
#endif /* unix */
/*
 *  unregister manager from portmapper
 */
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS);
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS_1);
	exit (-1);
}

/**
  * Quit remote server by RPC call 
  */
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
	

