/********************************************************************

 File:          ManagerMain.c

 Project:       Device Servers with sun-rpc

 Description:   Main Programm for a Network Manager running under
		HPUX or SUN or without message server and with a
		dummy database under OS9.

 Author(s):     Jens Meyer
 		$Author: jkrueger1 $

 Original: 	January 1991

 Version:	$Revision: 1.24 $

 Date:		$Date: 2006-09-06 18:43:31 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

			All Rights Reserved
**********************************************************************/

#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <API_xdr_vers3.h>
#include <ManagerP.h>
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error Could not find signal.h
#endif
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


#ifdef unix
#	ifdef HAVE_PATHS_H
#		include <paths.h>
#	else
#		define _PATH_DEVNULL	"/dev/null"
#	endif
#	if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#	endif
#	include <fcntl.h>
#	include <unistd.h>
#endif

static void 	network_manager_1();
static void 	network_manager_4();
static void 	startup_msg();

config_flags	c_flags = {False,False,False,False,False,False,False,True};
char 		*dshome  = NULL;
char		*display = NULL;
char	 	nethost [HOST_NAME_LENGTH];
char		logfile [256];
FILE		*system_log = NULL;

int		pid = 0;

#ifdef unix
pid_t	become_daemon(void);
#endif

char *getTimeString(const char *name)
{
        time_t          tps = time((time_t *)NULL);
        struct tm       *time_tm = localtime(&tps);
	static char	message[80];
        char            *tps_str = asctime(time_tm);
        tps_str[24] = '\0';
	
	snprintf(message, sizeof(message), "%s : %s : ", name, tps_str);
	return message;
}

int main (int argc, char **argv)
{
	SVCXPRT *transp;
	FILE	*fptr;
	char	*dbhost = NULL;
	char	*dbhome = NULL;
	char 	*dbtables = NULL;
	char	*nethost_env = NULL;
	char	*ora_sid = NULL;
	char 	*ora_home = NULL;
	char 	*cmd_argv [5];
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
	char 	*dbase_used="NDBM";
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
	         			c_flags.oracle	     = True;
	         			c_flags.dbm   	     = False;
					dbase_used = "ORACLE";
				}
				else if (strcmp (arg, "mysql") == 0)
				{
	         			c_flags.mysql	     = True;
	         			c_flags.dbm  	     = False;
					dbase_used = "MYSQL";
					dbm_name = "tango";
				}
				else if (strcmp (arg, "log") == 0)
		 			c_flags.request_log = True;
				else if (strcmp (arg, "security") == 0)
		 			c_flags.security    = True;
				else if (*arg == 'h')
				{
		 			printf("usage: Manager [ -dbm | -mysql | -oracle ] [-log] [-security] [-help] \n");
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
		char *logpath = getenv("LOGPATH");
		snprintf (logfile, sizeof(logfile), "%s/Manager.log", logpath ? logpath : homepath);
		if ( (system_log = fopen (logfile, "a")) == NULL )
		{
			fprintf (stderr,"\ncannot open Manager.log file (%s), exiting...\n", logfile);
			fprintf(stderr, "LOGPATH or DSHOME path may be wrong\n");
			kill (pid,SIGQUIT);
		}
	}

#ifdef unix
/*
 * Check the environment if DBM database is used !!
 */
	if (c_flags.oracle == False )
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
		if (c_flags.mysql == False)
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

	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS_1, network_manager_1, IPPROTO_UDP)) 	
	{
		fprintf(stderr, "\nUnable to register network manager.\n");
		fprintf(stderr, "Program number 100 already in use?\n");
	        exit (-1);
	}

	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS, network_manager_4, IPPROTO_UDP)) 	
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
	fprintf(system_log, "%s dbhost : %s, nethost : %s\n", getTimeString("Manager"), dbhost, nethost);
	fflush(system_log);
	if (strcmp (dbhost, nethost) && strcmp (dbhost, "localhost"))
	{
#ifdef unix
/*
 *  startup database server on a remote host
 */
		if ( c_flags.oracle == False )
	      	{
			if (c_flags.mysql == False)
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
			fprintf(system_log, "%s try to start database server\n", getTimeString("Manager"));
			fflush(system_log);
		}
        	if ((db_pid = fork ()) < 0)
		{
			fprintf (stderr,"\ndatabase server startup failed, exiting...\n");
			if (c_flags.request_log)
			{
				fprintf (system_log, "%s : Database server startup failed, exiting...\n", getTimeString("Manager"));
				fflush(system_log);
			}
			kill (pid,SIGQUIT);
		}
		if (c_flags.request_log)
		{
			fprintf(system_log, "%s fork returned : %d\n", getTimeString("Manager"), db_pid);
			fflush(system_log);
		}
		if (!db_pid)
		{
/* 
 * Set path to DBM server 
 */
			snprintf (homedir, sizeof(homedir), "%s/%s", homepath, dbm_server);
			if (c_flags.request_log)
			{
				fprintf(system_log, "%s try to start : %s\n", getTimeString("Manager"), homedir);
				fflush(system_log);
			}
/* 
 * Set arguments for execv 
 */
			i = 0;
			cmd_argv[i++] = dbm_server; 
			cmd_argv[i++] = "-t";
			if (c_flags.mysql == True)
				cmd_argv[i++] = "mysql";
			else
				cmd_argv[i++] = "dbm";
			cmd_argv[i++] = dbm_name; 
			cmd_argv[i++] = nethost; 
			cmd_argv[i] = 0;

			if (c_flags.request_log)
			{
				fprintf(system_log, "%s Manager execvp arguments for database : ", getTimeString("Manager"));
				fprintf(system_log, "%s %s %s %s %s %s\n",homedir,cmd_argv[0],cmd_argv[1],cmd_argv[2],cmd_argv[3],cmd_argv[4]);
				fprintf(system_log, "%s svc_destroy for database : %p\n", getTimeString("Manager"), transp);
				fflush(system_log);
			}
			if (transp)
				svc_destroy(transp); 
			execvp (homedir, cmd_argv);
			
			fprintf (stderr,"\nexecvp failed, database_server not started\n");
			if (c_flags.request_log)
			{
				fprintf(system_log, "%s execv failed, database_server not started : ", getTimeString("Manager"));
				fprintf(system_log, "Error = %d (%s)\n", errno, strerror(errno));
				fflush(system_log);
			}
			kill (pid,SIGQUIT);
		}
	}
	
/*
 *  startup message server
 */
        if (( msg_pid = fork () ) < 0 )
	{
		fprintf (stderr,"\nmessage server startup failed, exiting...\n");
		if (c_flags.request_log)
		{
			fprintf (system_log, "%s Message server startup failed, exiting...\n", getTimeString("Manager"));
			fflush(system_log);
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
			fprintf(system_log, "%s Manager execvp arguments for message server :", getTimeString("Manager"));
			fprintf(system_log, "%s %s %s\n",homedir,cmd_argv[0],cmd_argv[1]);
			fprintf(system_log, "%s svc_destroy for message server : %p\n", getTimeString("Manager"), transp);
			fflush(system_log);
		}
		if (transp)
			svc_destroy(transp); 
		execvp (homedir,cmd_argv);

		fprintf (stderr,"\nexecvp failed, message server not started\n");
		if (c_flags.request_log)
		{
			fprintf(system_log,"%s execv failed, message server not started : ", getTimeString("Manager"));
			fprintf(system_log, "Error = %d (%s)\n", errno, strerror(errno));
			fflush(system_log);
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
			fprintf (stderr,"\nos9exec failed, os9_dbsu-server not started\n");
			if (c_flags.request_log)
			{
				fprintf (system_log, "os9exec failed, os9_dbsu-server not started\n");
				fflush(system_log);
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
      		fprintf (system_log,"%s Network Manager started subprocesses : ", getTimeString("Manager"));
      		fprintf (system_log,"NETHOST = %s   PID = %d\n",nethost,pid);
		fclose (system_log);
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
	fprintf(system_log, "%s svc_run returned: Manager stopped\n", getTimeString("Manager"));
	fflush(system_log);
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

	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();

	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply (transp, (xdrproc_t)xdr_void, NULL);
			return;
		case RPC_MSG_REGISTER:
			xdr_argument = xdr__register_data;
			xdr_result = xdr__msg_manager_data;
			local = (char *(*)()) rpc_msg_register_1;
			break;
		case RPC_DB_REGISTER:
			xdr_argument = xdr__register_data;
			xdr_result = xdr_int;
			local = (char *(*)()) rpc_db_register_1;
			break;
		case RPC_GET_CONFIG:
/*
 * Execute only if the startup is finished!
 */
			if ( c_flags.startup == True )
			{
				xdr_argument = xdr__register_data;
				xdr_result = xdr__manager_data;
				local = (char *(*)()) rpc_get_config_4;
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

	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();

	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply (transp, (xdrproc_t)xdr_void, NULL);
			return;
		case RPC_MSG_REGISTER:
			xdr_argument = xdr__register_data;
			xdr_result = xdr__msg_manager_data;
			local = (char *(*)()) rpc_msg_register_1;
			break;
		
		case RPC_DB_REGISTER:
			xdr_argument = xdr__register_data;
			xdr_result = xdr_int;
			local = (char *(*)()) rpc_db_register_1;
			break;

		case RPC_GET_CONFIG:
/*
 * Execute only if the startup is finished!
 */
			if ( c_flags.startup == True )
			{
				xdr_argument = xdr__register_data;
				xdr_result = xdr__manager_data_3;
				local = (char *(*)()) rpc_get_config_4;
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
	FILE	*system_log = NULL;

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
				if ((system_log = fopen (logfile, "a")) != NULL )
	   			{
      					fprintf (system_log, "%s startup finished.\n", getTimeString("Manager"));
					fclose (system_log);
	   			}
				else
           				fprintf (stderr, "\ncannot open Manager.log file (%s).\n", logfile);
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
