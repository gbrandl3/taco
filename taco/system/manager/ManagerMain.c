/********************************************************************

 File:          ManagerMain.c

 Project:       Device Servers with sun-rpc

 Description:   Main Programm for a Network Manager running under
		HPUX or SUN or without message server and with a
		dummy database under OS9.

 Author(s):     Jens Meyer
 		$Author: jkrueger1 $

 Original: 	January 1991

 Version:	$Revision: 1.4 $

 Date:		$Date: 2003-12-09 13:31:45 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

			All Rights Reserved
**********************************************************************/

#include <API.h>
#include <private/ApiP.h>
#include <API_xdr_vers3.h>
#include <ManagerP.h>
#include <signal.h>
#include <sys/wait.h>

static void 	network_manager_1();
static void 	network_manager_4();
static void 	startup_msg();

config_flags	c_flags = {False,False,False,False,False,False,False};
char 		*dshome  = NULL;
char		*display = NULL;
char	 	nethost [SHORT_NAME_SIZE];
char		logfile [256];
FILE		*system_log = NULL;

int		pid = 0;

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
	char	rtdb_server_path [200];
	char	oracle_server_path [200];
	char	ld_path [200];
	char	db_path [200];
	char	homepath [200];
	char	homedir [200];
	char	dbase_env [200];
	char	db_start [256];
	char	mode [256];
	char    *time_string;
	time_t	clock;
	int	msg_pid = 0;
	int	db_pid = 0;
	int	i;
	int     res;


	pid = getpid ();

#ifdef unix
	if ( (nethost_env = (char *)getenv ("NETHOST")) == NULL )
	{
		printf ("Environment variable NETHOST not defined, using local host ...\n");
		gethostname (nethost, sizeof(nethost) - 1);
	}
	else
	{
		snprintf(nethost, sizeof(nethost) - 1, "%s",nethost_env);

/*		printf ("Environment variable NETHOST = %s\n",nethost);*/
	}
#endif /* unix */

	/*
	 *  read options for the manager startup.
	 */

	if (argc > 1)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp (argv[i],"-rtdb") == 0)
		 		c_flags.rtdb        = True;
			if (strcmp (argv[i],"-log") == 0)
		 		c_flags.request_log = True;
			if (strcmp (argv[i],"-security") == 0)
		 		c_flags.security    = True;
			if (strcmp (argv[i],"-oracle") == 0)
	         		c_flags.oracle	     = True;
		}
	}


	/*
	 *  get environment variables 
	 */

#ifdef unix
	if ( (dshome = (char *)getenv ("TACO_PATH")) == NULL )
	{
		printf ("Environment variable TACO_PATH not defined, assume database and message server are in path...\n");
	}

/*
	if ( (display = (char *)getenv ("DISPLAY")) == NULL )
	{
		display = "localhost:0.0";
	}
 */

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

	if (dshome != NULL)
	{
		snprintf (homepath, sizeof(homepath) - 1, "%s", dshome);
	}
	else
	{
		snprintf(homepath, sizeof(homepath) - 1, "./");
	}
#ifdef NEVER
#ifdef linuxx86
	snprintf (homepath, sizeof(homepath) - 1, "%s/system/bin/linux/x86", dshome);
#endif /* linuxx86 */
#ifdef linux68k
	snprintf (homepath, sizeof(homepath) - 1, "%s/system/bin/linux/68k", dshome);
#endif /* linux68k */


#ifdef sun
#ifdef solaris
	snprintf (homepath, sizeof(homepath) - 1, "%s/system/bin/solaris", dshome);
#else
	snprintf (homepath, sizeof(homepath) - 1, "%s/system/bin/sun4", dshome);
#endif /* solaris */
#endif /* sun */

#ifdef hpux10
	snprintf (homepath, sizeof(homepath) - 1, "%s/system/bin/hpux10.2", dshome);
#endif /* hpux10 */
#endif /* NEVER */

#ifdef unix
	if ( (fptr = fopen (homepath,"r")) == NULL )
#endif /* unix */
#ifdef _OSK
	if ( (fptr = fopen (homepath,"d")) == NULL )
#endif /* _OSK */
	{
		printf ("TACO_PATH leads to a strange directory, exiting...\n");
		exit (-1);
	}
	else
		fclose (fptr);

#ifdef unix
/*
 * Check the environment if DBM database is used !!
 */

	if (c_flags.rtdb == False )
	{
		if (c_flags.oracle == False )
	   	{
			if ( ((char *)getenv ("DBTABLES")) == NULL ||
				((char *)getenv ("DBM_DIR")) == NULL ||
				((char *)getenv ("RES_BASE_DIR")) == NULL )
			{
	      			fprintf (stderr, 
	    				"\nEnvironment variables for DBM database not defined, exiting...\n");
	      			fprintf (stderr, "Use %s/dbm_env to set the environment variables!\n", homepath);
	      			exit (-1);
	      		}
		}
	   	else
/*
 * Check the environment if ORACLE is used 
 */
		{
			if ( (dbtables = (char *)getenv ("DBTABLES")) == NULL )
			{
	      			fprintf (stderr, 
	    				"\nEnvironment variables for ORACLE database not defined, exiting...\n");
				fprintf (stderr, "DBTABLES must be defined!\n");
				exit (-1);
			}
			if ( (dbhome = (char *)getenv ("DBHOME")) == NULL )
			{
				fprintf (stderr, "Environment variable DBHOME not defined, exiting...\n");
				exit (-1);
			}
			if ( (ora_sid = (char *)getenv("ORACLE_SID")) == NULL)
			{
				fprintf (stderr, "Environment variable ORACLE_SID not defined, exiting...\n");
				exit(-1);
			}
			if  ( (ora_home = (char *)getenv("ORACLE_HOME")) == NULL)
			{
				fprintf (stderr, "Environment variable ORACLE_HOME not defined, exiting...\n");
				exit(-1);
			}
/* 
 * Set the pathes and check them. 
 */
			snprintf (oracle_server_path, sizeof(oracle_server_path) - 1, "%s/bin/solaris", dbhome);
			if ( (fptr = fopen (oracle_server_path,"r")) == NULL )
			{
				fprintf (stderr, "DBHOME leads to a strange directory, exiting...\n");
				exit (-1);
			}
			else
				fclose (fptr);
		}
	}
	else
/*
 * Check the environment if RTDB database is used !!
 */
	{
		if ( (dbhome = (char *)getenv ("DBHOME")) == NULL )
		{
			fprintf (stderr, "Environment variable DBHOME not defined, exiting...\n");
			exit (-1);
		}
/* 
 * Set the pathes and check them. 
 */
		snprintf (rtdb_server_path, sizeof(rtdb_server_path) - 1, "%s/bin", dbhome);
		if ( (fptr = fopen (rtdb_server_path,"r")) == NULL )
		{
			fprintf (stderr, "DBHOME leads to a strange directory, exiting...\n");
			exit (-1);
		}
		else
			fclose (fptr);
		
		snprintf (db_path, sizeof(db_path) - 1, "%s/setup", dbhome);
		if ( (fptr = fopen (rtdb_server_path,"r")) == NULL )
		{
			fprintf (stderr, "DBHOME leads to a strange directory, exiting...\n");
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
#endif /* _OSK */

#ifdef _OSK
/* because the normal kill command under OS9 can not
 * be caugth it makes live easier to unregister the manager
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
		fprintf(stderr, "cannot create udp service for manager.\n");
	        exit (-1);
	}

	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS_1, 
			  network_manager_1, IPPROTO_UDP)) 	
	{
		fprintf(stderr, "Unable to register network manager.\n");
		fprintf(stderr, "Program number 100 already in use?\n");
	        exit (-1);
	}

	if (!svc_register(transp, NMSERVER_PROG, NMSERVER_VERS, 
			  network_manager_4, IPPROTO_UDP)) 	
	{
		fprintf(stderr, "unable to register network manager\n");
		fprintf(stderr, "Program number 100 already in use?\n");
		pmap_unset (NMSERVER_PROG, NMSERVER_VERS_1);
	        exit (-1);
	}

/*
 *  delete old System.log and open a new
 *  System.log file for writing system information
 */
	if (c_flags.request_log)
	{
		char *logpath = getenv("LOGPATH");
		snprintf (logfile, sizeof(logfile) - 1, "%s/System.log", logpath ? logpath : homepath);
		if ( (system_log = fopen (logfile, "w")) == NULL )
		{
			fprintf (stderr,"cannot open System.log file, exiting...\n");
			kill (pid,SIGQUIT);
		}
	}
/*
 *  Start the database server on a remote host, if the
 *  DBHOST environment variable is set and 
 *  specifies not the local host.
 */
	if (strcmp (dbhost, nethost) && strcmp (dbhost, "localhost"))
	{
#ifdef unix
/*
 *  startup database server on a remote host
 */
		if ( c_flags.rtdb == False )
		{
			if ( c_flags.oracle == False )
	      		{
/* 
 * DBM startup sequence 
 */
	      			snprintf (db_start, sizeof(db_start) - 1, 
#ifdef __hpux
	      				"remsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#endif
#ifdef sun
	      				"rsh %s -l dserver -n \"export %s/%s %s %s 1>&- 2>&- &\" ", 
#endif
	      				dbhost, homepath, dbm_server, dbm_name, nethost);
			}
			else
	      		{
/* 
 * ORACLE startup sequence 
 */
	      			snprintf (db_start, sizeof(db_start) - 1,
#ifdef __hpux
	      				"remsh %s -l dserver -n \"export DBTABLES=%s;"
					"export ORACLE_SID=%s;export ORACLE_HOME=%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#endif
#ifdef sun
	      				"rsh %s -l dserver -n \"export DBTABLES=%s;"
					"export ORACLE_SID=%s;export ORACLE_HOME=%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#endif
					dbhost, dbtables, ora_sid, ora_home, oracle_server_path, ora_server, nethost, ora_name);
			}
		}
		else
		{
/* 
 * RTDB startup sequence 
 */
	      		snprintf (db_start, sizeof(db_start) - 1, 
#ifdef __hpux
	      			"remsh %s -l dbase -n \"export %s=%s:%s/%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#endif
#ifdef sun
	      			"rsh %s -l dbase -n \"export %s=%s:%s/%s;%s/%s %s %s 1>&- 2>&- &\" ", 
#endif
				dbhost, db_name, db_name, db_path, db_name, rtdb_server_path, db_server, db_name, nethost);
		}

		res = system(db_start);
		if ( res != 0 )
		{
			fprintf (stderr,"database server startup failed, exiting...\n");
			if (c_flags.request_log)
				fprintf (system_log, "database server startup failed, exiting...\n");
			kill (pid,SIGQUIT);
		}
	}
	else
	{
        	if (( db_pid = fork () ) < 0 )
		{
			fprintf (stderr,"database server startup failed, exiting...\n");
			if (c_flags.request_log)
				fprintf (system_log, "database server startup failed, exiting...\n");
			kill (pid,SIGQUIT);
		}

		if (!db_pid)
		{
			if ( c_flags.rtdb == True )
			{
/* 
 * Set environment 
 */
				snprintf (dbase_env, sizeof(dbase_env) - 1, "%s=%s:%s/%s", 
						db_name, db_name, db_path, db_name );
				if ( putenv (dbase_env) != 0 )
				{
					fprintf (stderr, "Cannot set environment variable %s, exiting...\n", db_name);
					if (c_flags.request_log)
						fprintf (system_log, "Cannot set environment variable %s, exiting...\n", db_name);
					kill (pid,SIGQUIT);
				}
/* 
 * Set path to executable 
 */
				snprintf (homedir, sizeof(homedir) - 1, "%s/%s", rtdb_server_path, db_server);
/* 
 * Set arguments for execv 
 */
				i = 0;
				cmd_argv[i++] = db_server; 
				cmd_argv[i++] = db_name; 
				cmd_argv[i++] = nethost; 
				cmd_argv[i] = 0;
			}
			else
			{
/* 
 * Set path to DBM server 
 */
				if (dshome != NULL)
					snprintf (homedir, sizeof(homedir) -1, "%s/%s", homepath, dbm_server);
				else
					snprintf (homedir, sizeof(homedir) - 1, "%s", dbm_server);
/* 
 * Set arguments for execv 
 */
				i = 0;
				cmd_argv[i++] = dbm_server; 
				cmd_argv[i++] = dbm_name; 
				cmd_argv[i++] = nethost; 
				cmd_argv[i] = 0;
			}

			svc_destroy(transp); 
			execvp (homedir, cmd_argv);
		
			fprintf (stderr,"execvp failed, database_server not started\n");
			if (c_flags.request_log)
				fprintf (system_log, "execv failed, database_server not started\n");
			kill (pid,SIGQUIT);
		}
	}

/*
 *  startup message server
 */
        if (( msg_pid = fork () ) < 0 )
	{
		fprintf (stderr,"message server startup failed, exiting...\n");
		if (c_flags.request_log)
			fprintf (system_log, "message server startup failed, exiting...\n");
		kill (pid,SIGQUIT);
	}

	if (!msg_pid)
	{
		if (dshome != NULL)
			snprintf (homedir, sizeof(homedir) - 1, "%s/MessageServer", homepath);
		else
			snprintf (homedir, sizeof(homedir) - 1, "MessageServer", homepath);

		i = 0;
		cmd_argv[i++] = "MessageServer"; 
		cmd_argv[i++] = nethost; 
		cmd_argv[i] = 0;

		svc_destroy(transp); 

		execvp (homedir,cmd_argv);

		fprintf (stderr,"execvp failed, message server not started\n");
		if (c_flags.request_log)
			fprintf (system_log,"execv failed, message server not started\n");
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

		snprintf (homedir, sizeof(homedir) - 1, "%s/CMDS/os9_dbsu_server", dshome);
		i = 0;
		cmd_argv[i++] = homedir; 
		cmd_argv[i] = 0;

		if ((db_pid = os9exec(os9forkc, cmd_argv[0], 
			      cmd_argv, environ,0,0,3)) <= 0)
		{
			fprintf (stderr,"os9exec failed, os9_dbsu-server not started\n");
			if (c_flags.request_log)
				fprintf (system_log, "os9exec failed, os9_dbsu-server not started\n");
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
 * print network manager starttime to System.log
 */
	time (&clock);
	time_string = ctime (&clock);
	if (c_flags.request_log)
	{
      		fprintf (system_log,"Network Manager started subprocesses at : %s", time_string);
      		fprintf (system_log,"NETHOST = %s   PID = %d\n\n",nethost,pid);
		fclose (system_log);
	}
	
/*
 *  point of no return
 */
	svc_run();
	fprintf(stderr, "svc_run returned\n");
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
		fprintf(stderr, "unable to free arguments\n");
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
		fprintf(stderr, "unable to free arguments\n");
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
	char    *time_string;
	time_t	clock;


	if ( c_flags.startup == False )
	{
		if ( c_flags.msg_server == True && c_flags.db_server == True) 
		{
			c_flags.startup = True;
#ifdef unix
/*
 *  Open the System.log file for writing system information
 */
			if (c_flags.request_log)
			{
				if ((system_log = fopen (logfile, "a")) != NULL )
	   			{
					time (&clock);
					time_string = ctime (&clock);
      					fprintf (system_log, "Network Manager startup finished at : %s\n", time_string);
					fclose (system_log);
	   			}
				else
           				fprintf (stderr,"cannot open System.log file.\n");
			}
#endif /* unix */
#ifdef _OSK
			printf ("Database Dummy Server OK !\nNetwork Manager OK !\n");
#endif /* _OSK */
		}
	}
}


