/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:          ManagerMain.c
 *
 * Project:       Device Servers with sun-rpc
 *
 * Description:   Main Programm for a Network Manager running under
 *            HPUX or SUN or without message server and with a
 *            dummy database under OS9.
 *
 * Author(s):     Jens Meyer
 *            $Author: jensmeyer $
 *
 * Original:  January 1991
 *
 * Version:   $Revision: 1.7 $
 *
 * Date:              $Date: 2011-10-24 13:11:23 $
 *
 */

#ifdef HAVE_CONFIG_H
#     include "config.h"
#endif

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

#include <API.h>
#include <private/ApiP.h>
#include <API_xdr_vers3.h>
#include <ManagerP.h>
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#	error Could not find signal.h
#endif
#ifdef HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#else
#	error Could not find sys/wait.h
#endif
#ifdef HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#endif
#ifdef HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif

#ifdef DARWIN
        typedef void (* MyRpcFuncPtr)();
#else
	typedef void (* MyRpcFuncPtr)(struct svc_req *, SVCXPRT *);
#endif

#include <cerrno>
#include <string>

#ifdef HAVE_PATHS_H
#	include <paths.h>
#else
#	define _PATH_DEVNULL	"/dev/null"
#endif
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_FCNTL_H
#	include <fcntl.h>
#endif
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif

static void 	network_manager_1(struct svc_req *rqstp, SVCXPRT *transp);
static void 	network_manager_4(struct svc_req *rqstp, SVCXPRT *transp);
static void 	startup_msg();

manager_config_flags	c_flags = {False,False,False,False,False,E_GDBM};
char 		*dshome  = NULL;
const char	*display = NULL;
char	 	nethost [HOST_NAME_LENGTH];

int		pid = 0;

log4cpp::Category	*logStream;

typedef char *(* DbRpcLocalFunc)(...);

#ifdef unix
pid_t	become_daemon(void);
#endif

/**
 * This function will initialise the log4cpp logging service.
 * The service will be configured via the ${LOGCONFIG} environment variable.
 * If this is not found it will use the default logging mechanism.
 * The instance will be the "taco.server" + serverName
 *
 * @param serverName the name of the server
 */
static void init_logstream(const std::string serverName)
{
        const char *logpath = getenv("LOGCONFIG");
        std::string tmp = serverName;
        std::string::size_type pos = tmp.find('/');
        tmp[pos] = '.';
        try
        {
                if (!logpath)
                        throw 0;
                log4cpp::PropertyConfigurator::configure(logpath);
        }
        catch (const log4cpp::ConfigureFailure &e)
        {
                std::cerr << e.what() << std::endl;
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
        }
        catch (...)
        {
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
        }
        logStream = &log4cpp::Category::getInstance("taco.system." + tmp);
        logStream->noticeStream() << "using " << logpath << " configuration file" << log4cpp::eol;
}


int main (int argc, char **argv)
{
	SVCXPRT *transp;
	FILE	*fptr;
	const char	*dbhost = NULL;
	const char	*dbhome = NULL;
	char 	*dbtables = NULL;
	char	*nethost_env = NULL;
	char	*ora_sid = NULL;
	char 	*ora_home = NULL;
	const char 	*cmd_argv [5];
	char	oracle_server_path [200];
	char	db_path [200];
	char	homepath [200];
	char	homedir [200];
	char	db_start [256];
	char    *time_string;
	time_t	clock;
	int	msg_pid = 0;
	int	db_pid = 0;
	int	i;
	int     res;
	const char 	*dbase_used="NDBM";
	
	char	*db_name_env = NULL;
	char    db_name[256];
	
#ifdef unix
static	int	fd_devnull = -1;
#endif

	pid = getpid ();

#ifdef unix
	if ( (nethost_env = (char *)getenv ("NETHOST")) == NULL ) /* Flawfinder: ignore */
	{
		printf ("Environment variable NETHOST not defined, using local host ...\n");
		taco_gethostname (nethost, sizeof(nethost) - 1);
	}
	else
		snprintf(nethost, sizeof(nethost), "%s", nethost_env);

#ifdef DEBUG
	printf ("Environment variable NETHOST = %s\n",nethost);
#endif		
		
	/*
	 * Get the name of the database schema to use.
	 */
	 
	if ( (db_name_env = (char *)getenv ("TACO_DB_NAME")) != NULL )
	{
		snprintf(db_name, sizeof(db_name), "%s", db_name_env);
	}
	else
	{
		snprintf(db_name, sizeof(db_name), "tango");
	}
	
#endif /* unix */

/*
 *  read options for the manager startup.
 */
	if (argc > 1)
	{
		for (i=1; i<argc; i++)
			if (*argv[i] == '-')
			{
				char *arg = argv[i] + 1;
		
				if (strcmp (arg, "oracle") == 0)
				{
	         			c_flags.dbm   	     = E_ORACLE;
					dbase_used = "ORACLE";
				}
				else if (strcmp (arg, "mysql") == 0)
				{
	         			c_flags.dbm  	     = E_MYSQL;
					dbase_used = "MYSQL";
					dbm_name = db_name;
				}
				else if (strcmp (arg, "sqlite3") == 0)
				{
	         			c_flags.dbm  	     = E_SQLITE;
					dbm_name = db_name;
				}
				else if (strcmp (arg, "log") == 0)
		 			c_flags.request_log = True;
				else if (strcmp (arg, "security") == 0)
		 			c_flags.security    = True;
				else if (*arg == 'v')
				{
					printf("TACO Manager version : %s\n", VERSION);
					exit(0); 
				}
				else if (*arg == 'h')
				{
		 			printf("usage: Manager [-help] [-version] [ -dbm | -mysql | -oracle ] [-log] [-security]\n");
					printf(" (note: default database is dbm)\n");
					exit(0);
				}
			}
	}
#ifdef unix
/*
 *  get environment variables 
 */
	if ( (dshome = (char *)getenv ("TACO_PATH")) == NULL )
	{
		printf ("Environment variable TACO_PATH not defined, assume database and message server are in path...\n");
		dshome = getcwd(homepath, sizeof(homepath));
	}

	if ( (display = (char *)getenv ("DISPLAY")) == NULL )
	{
		display = "localhost:0.0";
	}

        if ( (dbhost = (char *)getenv ("DBHOST")) == NULL )
        {
		dbhost = "localhost";
        }
#endif /* unix */

#ifdef _OSK
/*
 *  fixed path for a automatic os9 startup with 
 *  dummy database.
 */
	dshome = "/h0";
#endif /* _OSK */
	strncpy(homepath, dshome, sizeof(homepath) - 1);

#ifdef unix
	if ( (fptr = fopen (homepath,"r")) == NULL )
#endif /* unix */
#ifdef _OSK
	if ( (fptr = fopen (homepath,"d")) == NULL )
#endif /* _OSK */
	{
		fprintf (stderr, "\nTACO_PATH leads to a strange directory, exiting...\n");
		exit (-1);
	}
	else
		fclose (fptr);
/*
 *  delete old Manager.log and open a new
 *  Manager.log file for writing system information
 */
	if (c_flags.request_log)
	{       
		init_logstream("Manager");
	}

#ifdef unix
/*
 * Check the environment if DBM database is used !!
 */
	if (c_flags.dbm != E_ORACLE)
	{
		if ( (dbtables = (char *)getenv ("DBTABLES")) == NULL )
		{
	      		fprintf (stderr, "\nEnvironment variables for %s database not defined, exiting...\n", dbase_used);
			fprintf (stderr, "DBTABLES must be defined!\n");
			exit (-1);
		}
		if ( (dbtables = (char *)getenv ("RES_BASE_DIR")) == NULL )
		{
	      		fprintf (stderr, "\nEnvironment variables for %s database not defined, exiting...\n",dbase_used);
			fprintf (stderr, "RES_BASE_DIR must be defined!\n");
			exit (-1);
		}
		if (c_flags.dbm != E_MYSQL)
		{
			if ( (dbtables = (char *)getenv ("DBM_DIR")) == NULL )
			{
	      			fprintf (stderr, "\nEnvironment variables for NDBM database not defined, exiting...\n");
				fprintf (stderr, "DBM_DIR must be defined!\n");
				exit (-1);
			}
		}
	}
	else
/*
 * Check the environment if ORACLE is used 
 */
	{
		if ( (dbtables = (char *)getenv ("DBTABLES")) == NULL )
		{
	      		fprintf (stderr, "\nEnvironment variables for ORACLE database not defined, exiting...\n");
			fprintf (stderr, "DBTABLES must be defined!\n");
			exit (-1);
		}
		if ( (dbhome = (char *)getenv ("DBHOME")) == NULL )
		{
			fprintf (stderr, "\nEnvironment variable DBHOME not defined, exiting...\n");
			fprintf (stderr, "DBHOME must be defined!\n");
			exit (-1);
		}
		if ( (ora_sid = (char *)getenv("ORACLE_SID")) == NULL)
		{
			fprintf (stderr, "\nEnvironment variable ORACLE_SID not defined, exiting...\n");
			fprintf (stderr, "ORACLE_SID must be defined!\n");
			exit(-1);
		}
		if  ( (ora_home = (char *)getenv("ORACLE_HOME")) == NULL)
		{
			fprintf (stderr, "\nEnvironment variable ORACLE_HOME not defined, exiting...\n");
			fprintf (stderr, "ORACLE_HOME must be defined!\n");
			exit(-1);
		}
/* 
 * Set the pathes and check them. 
 */
		snprintf (oracle_server_path, sizeof(oracle_server_path), "%s/bin/solaris", dbhome);
		if ( (fptr = fopen (oracle_server_path,"r")) == NULL )
		{
			fprintf (stderr, "\nDBHOME leads to a strange directory, exiting...\n");
			exit (-1);
		}
		else
			fclose (fptr);
	}
#endif /* unix */


/*
 *  setup signal handling
 */
#ifdef unix
	(void) signal(SIGQUIT, unreg_server);
	(void) signal(SIGTERM, unreg_server);
	(void) signal(SIGINT,  unreg_server);
	(void) signal(SIGPIPE, SIG_IGN);
	(void) signal(SIGHUP,  SIG_IGN);
	(void) signal(SIGCHLD, SIG_IGN);
#endif /* unix */

#ifdef _OSK
	intercept (unreg_server);

/* 
 * because the normal kill command under OS9 can not
 * be caught it makes live easier to unregister the manager
 * from the portmapper every time before startup.
 * Safety will be neglected because now it is possible
 * to start several managers on the same host.
 */
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS);
	pmap_unset (NMSERVER_PROG, NMSERVER_VERS_1);
#endif /* _OSK */

/*
 *   create server transport with udp protocol
 */
	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) 
	{
		fprintf(stderr, "\ncannot create udp service for manager.\n");
	        exit (-1);
	}

	MyRpcFuncPtr network_manager;
        network_manager = (MyRpcFuncPtr)network_manager_1;
	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS_1, network_manager, IPPROTO_UDP)) 	
	{
		fprintf(stderr, "\nUnable to register network manager.\n");
		fprintf(stderr, "Program number 100 already in use?\n");
	        exit (-1);
	}

        network_manager = (MyRpcFuncPtr)network_manager_4;
	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS, network_manager, IPPROTO_UDP)) 	
	{
		fprintf(stderr, "\nunable to register network manager\n");
		fprintf(stderr, "Program number 100 already in use?\n");
		pmap_unset (NMSERVER_PROG, NMSERVER_VERS_1);
	        exit (-1);
	}

/*
 *  Start the database server on a remote host, if the
 *  DBHOST environment variable is set and 
 *  specifies not the local host.
 */
	if (c_flags.request_log)
	{
		logStream->noticeStream() << "dbhost : " << dbhost << ", nethost : " << nethost << log4cpp::eol;
	}
	if (strcmp (dbhost, nethost) && strcmp (dbhost, "localhost"))
	{
#ifdef unix
/*
 *  startup database server on a remote host
 */
		if ( c_flags.dbm == E_ORACLE )
	      	{
			if (c_flags.dbm == E_MYSQL)
			{
/* 
 * DBM startup sequence 
 */
      				snprintf (db_start, sizeof(db_start), 
#ifdef __hpux
      					"remsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#else
      					"rsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#endif
      					dbhost, homepath, dbm_server, dbm_name, nethost);
			}
			else
			{
/* 
 * MYSQL startup sequence 
 */
      				snprintf (db_start, sizeof(db_start), 
#ifdef __hpux
      					"remsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#else
      					"rsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#endif
      					dbhost, homepath, dbm_server, dbm_name, nethost);
			}
		}
		else
      		{
/* 
 * ORACLE startup sequence 
 */
      			snprintf (db_start, sizeof(db_start),
#ifdef __hpux
      				"remsh %s -l dserver -n \"export DBTABLES=%s;"
				"export ORACLE_SID=%s;export ORACLE_HOME=%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#else
      				"rsh %s -l dserver -n \"export DBTABLES=%s;"
				"export ORACLE_SID=%s;export ORACLE_HOME=%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#endif
				dbhost, dbtables, ora_sid, ora_home, oracle_server_path, ora_server, nethost, ora_name);
		}
	}
	else
	{
		if (c_flags.request_log)
		{
			logStream->noticeStream() << "try to start database server" << log4cpp::eol;
		}
        	if ((db_pid = fork ()) < 0)
		{
			fprintf (stderr,"\ndatabase server startup failed, exiting...\n");
			if (c_flags.request_log)
			{
				logStream->fatalStream() << "Database server startup failed, exiting..." << log4cpp::eol;
			}
			kill (pid,SIGQUIT);
		}
		if (c_flags.request_log)
		{
			logStream->noticeStream() << "fork returned pid = " << db_pid << log4cpp::eol;
		}
		if (!db_pid)
		{
/* 
 * Set path to DBM server 
 */
			snprintf (homedir, sizeof(homedir), "%s/%s", homepath, dbm_server);
			if (c_flags.request_log)
			{
				logStream->noticeStream() << "try to start : " << homedir << log4cpp::eol;
			}
/* 
 * Set arguments for execv 
 */
			i = 0;
			cmd_argv[i++] = dbm_server; 
			cmd_argv[i++] = "-t";
			switch (c_flags.dbm)
			{
				case E_MYSQL :
					cmd_argv[i++] = "mysql";
					break;
				case E_GDBM :
					cmd_argv[i++] = "dbm";
					break;
				case E_SQLITE :
					cmd_argv[i++] = "sqlite3";
					break;
				default:
					break;
			}
			cmd_argv[i++] = dbm_name; 
			cmd_argv[i++] = nethost; 
			cmd_argv[i] = 0;

			if (c_flags.request_log)
			{
				logStream->noticeStream() << "Manager execvp arguments for database : " << log4cpp::eol;
				logStream->noticeStream() << homedir << " " << cmd_argv[0] << " " << cmd_argv[1] << " " 
					<< cmd_argv[2] << " " << cmd_argv[3] << " " << cmd_argv[4] << log4cpp::eol;
			}
			if (transp)
			{
				if (c_flags.request_log)
					logStream->noticeStream() << "svc_destroy for database : " << transp << log4cpp::eol;
				svc_destroy(transp);
			}
			execvp (homedir, const_cast<char **>(cmd_argv));
			
			std::cerr << "execvp failed, database_server not started" << std::endl;
			if (c_flags.request_log)
			{
				logStream->fatalStream() << "execv failed, database_server not started : " << log4cpp::eol;
				logStream->fatalStream() << "Error = " << errno << " (" << strerror(errno) << ")" << log4cpp::eol; 
			}
			kill (pid,SIGQUIT);
		}
	}
	
/*
 *  startup message server
 */
        if (( msg_pid = fork () ) < 0 )
	{
		std::cerr << "message server startup failed, exiting..." << std::endl;
		if (c_flags.request_log)
		{
			logStream->fatalStream() << "Message server startup failed, exiting..." << log4cpp::eol;
		}
		kill (pid,SIGQUIT);
	}

	if (!msg_pid)
	{
		snprintf (homedir, sizeof(homedir), "%s/MessageServer", homepath);

		i = 0;
		cmd_argv[i++] = "MessageServer"; 
		cmd_argv[i++] = nethost; 
		cmd_argv[i] = 0;

		if (c_flags.request_log)
		{
			logStream->noticeStream() << "Manager execvp arguments for message server :" << log4cpp::eol;
			logStream->noticeStream() << homedir << " " << cmd_argv[0] << " " << cmd_argv[1] << log4cpp::eol;
		}
		if (transp)
		{
			if (c_flags.request_log)
				logStream->noticeStream() << "svc_destroy for message server : " << transp << log4cpp::eol;
			svc_destroy(transp); 
		}
		execvp (homedir, const_cast<char **>(cmd_argv));

		std::cerr << "execvp failed, message server not started" << std::endl;
		if (c_flags.request_log)
		{
			logStream->fatalStream() << "execv failed, message server not started." << log4cpp::eol;
			logStream->fatalStream() << "Error = " << errno << " (" << strerror(errno) << ")" << log4cpp::eol;
		}
		kill (pid,SIGQUIT);
	}
#endif /* unix */

#ifdef _OSK
/*
 *  Startup of the dummy database server
 */
	{
		int  	os9forkc();
		extern char	**environ;
		int		db_pid = 0;

		snprintf (homedir, sizeof(homedir), "%s/CMDS/os9_dbsu_server", dshome);
		i = 0;
		cmd_argv[i++] = homedir; 
		cmd_argv[i] = 0;

		if ((db_pid = os9exec(os9forkc, cmd_argv[0], 
			      cmd_argv, environ,0,0,3)) <= 0)
		{
			std::cerr << "os9exec failed, os9_dbsu-server not started");
			if (c_flags.request_log)
			{
				logStream->debugStream() << "os9exec failed, os9_dbsu-server not started" << log4cpp::eol;
			}
			kill (pid,SIGQUIT);
		}
 	}
/*
 *  Simulation of a normal message server startup
 */
	{
		_register_data		register_data;
		_msg_manager_data		*ret_data;

		register_data.host_name   = "NULL";
		register_data.prog_number = 0;
		register_data.vers_number = 0;

		ret_data = rpc_msg_register_1 (&register_data);
	}
#endif /* _OSK */

/*
 * print network manager starttime to Manager.log
 */
	if (c_flags.request_log)
	{
      		logStream->noticeStream() << "Network Manager started subprocesses : " << log4cpp::eol;
      		logStream->noticeStream() << "NETHOST = " << nethost << ", PID = " << pid << log4cpp::eol;
	}
	
/*
 *  point of no return
 */
#if unix
/*
 * Try to become a daemon
 */
	sleep(2);
	pid = become_daemon();
#endif
	svc_run();
	if (c_flags.request_log)
	{	
		logStream->fatalStream() << "svc_run returned: Manager stopped" << log4cpp::eol;
	}
	kill (pid,SIGQUIT);
}

static void network_manager_4(struct svc_req *rqstp, SVCXPRT *transp)
{
	union 	
	{
		_register_data  	rpc_msg_register_4_arg;
		_register_data  	rpc_db_register_4_arg;
		_register_data  	rpc_get_config_4_arg;
	} argument;

	char 		*result;
	xdrproc_t 	xdr_argument,
			xdr_result;
	DbRpcLocalFunc 	local;

	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply (transp, (xdrproc_t)xdr_void, NULL);
			return;
		case RPC_MSG_REGISTER:
			xdr_argument = (xdrproc_t)xdr__register_data;
			xdr_result = (xdrproc_t)xdr__msg_manager_data;
			local = (DbRpcLocalFunc)rpc_msg_register_1;
			break;
		case RPC_DB_REGISTER:
			xdr_argument = (xdrproc_t)xdr__register_data;
			xdr_result = (xdrproc_t)xdr_int;
			local = (DbRpcLocalFunc)rpc_db_register_1;
			break;
		case RPC_GET_CONFIG:
/*
 * Execute only if the db_server is exported !
 */
			if ( c_flags.db_server == True )
			{
				xdr_argument = (xdrproc_t)xdr__register_data;
				xdr_result = (xdrproc_t)xdr__manager_data;
				local = (DbRpcLocalFunc) rpc_get_config_4;
				break;
			}
			else
			{
				svcerr_noproc(transp);
				return;
			}
		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, (xdrproc_t)xdr_argument, (char *) &argument)) 
	{
		svcerr_decode(transp);
		return;
	}

	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply (transp, (xdrproc_t)xdr_result, result)) 
	{
		svcerr_systemerr(transp);
	}

	if (!svc_freeargs(transp, (xdrproc_t)xdr_argument, (char *) &argument)) 
	{
		fprintf(stderr, "\nunable to free server arguments\n");
		exit (-1);
	}
	startup_msg ();
}

static void network_manager_1(struct svc_req *rqstp, SVCXPRT *transp)
{
	union 	
	{
		_register_data  	rpc_msg_register_1_arg;
		_register_data  	rpc_db_register_1_arg;
		_register_data  	rpc_get_config_1_arg;
	} argument;

	char 		*result;
	xdrproc_t 	xdr_argument,
			xdr_result;
	DbRpcLocalFunc 	local;

	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply (transp, (xdrproc_t)xdr_void, NULL);
			return;
		case RPC_MSG_REGISTER:
			xdr_argument = (xdrproc_t)xdr__register_data;
			xdr_result = (xdrproc_t)xdr__msg_manager_data;
			local = (DbRpcLocalFunc) rpc_msg_register_1;
			break;
		
		case RPC_DB_REGISTER:
			xdr_argument = (xdrproc_t)xdr__register_data;
			xdr_result = (xdrproc_t)xdr_int;
			local = (DbRpcLocalFunc) rpc_db_register_1;
			break;

		case RPC_GET_CONFIG:
/*
 * Execute only if the startup is finished!
 */
			if ( c_flags.startup == True )
			{
				xdr_argument = (xdrproc_t)xdr__register_data;
				xdr_result = (xdrproc_t)xdr__manager_data_3;
				local = (DbRpcLocalFunc) rpc_get_config_4;
				break;
			}
			else
			{
				svcerr_noproc(transp);
				return;
			}

		default:
			svcerr_noproc(transp);
			return;
		}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, (xdrproc_t)xdr_argument, (char *) &argument)) 
	{
		svcerr_decode(transp);
		return;
	}

	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply (transp, (xdrproc_t)xdr_result, result)) 
	{
		svcerr_systemerr(transp);
	}

	if (!svc_freeargs(transp, (xdrproc_t)xdr_argument, (char *) &argument)) 
	{
		fprintf(stderr, "\nunable to free server arguments\n");
		exit (-1);
	}

	startup_msg ();
}



/****************************************************************
 *     Network Manager startup message                          *
 ****************************************************************/

static void startup_msg (void)
{
	if ( c_flags.startup == False )
	{
		if ( c_flags.msg_server == True && c_flags.db_server == True) 
		{
			c_flags.startup = True;
#ifdef unix
/*
 *  Open the Manager.log file for writing system information
 */
			if (c_flags.request_log)
			{
      				logStream->noticeStream() << "startup finished." << log4cpp::eol;
			}
#endif /* unix */
#ifdef _OSK
			printf ("Database Dummy Server OK !\nNetwork Manager OK !\n");
#endif /* _OSK */
		}
	}
}


#ifdef unix
/**
 * This function makes the current process to a daemon.
 *
 * @return Process ID of the child if the process became a daemon otherwise the process ID of itself.
 */
pid_t	become_daemon(void)
{
	pid_t	pid = getpid();
 	int	fd_devnull = open(_PATH_DEVNULL, O_RDWR);
	if (fd_devnull < 0)
		return pid; 
	while (fd_devnull <= 2) 
	{
        	int i = dup(fd_devnull);
        	if (i < 0)
        		return pid;
        	fd_devnull = i;
    	}

      	if ((pid = fork ()) > 0)
/*
 * Stop the parent process
 */ 
		exit(0);
	else if (pid == 0)
	{
/* 
 * Child process and try to detach the terminal
 */
#if HAVE_SETSID
		setsid();
#elif defined (TIOCNOTTY)
		{
			int i = open("/dev/tty", O_RDWR, 0);
			if (i != -1)
			{
				ioctl(i, (int)TIOCNOTTY, (char *)NULL);
				close(i);
			}
		}
#endif
		dup2(fd_devnull, 0);
		dup2(fd_devnull, 1);
		dup2(fd_devnull, 2);
		pid = getpid();
	}
	return pid;
}
#endif
