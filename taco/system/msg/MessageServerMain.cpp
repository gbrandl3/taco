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
 * File:        MessageServerMain.c
 *
 * Project:     Device Servers with sunrpc
 *
 * Description: Main Programm for a Message-Server
 *
 * Author(s):   Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-06-20 11:26:17 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

#include <API.h>
#include <private/ApiP.h>
#include <Message.h>

#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#	error Could not find signal.h
#endif

#if HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#elif HAVE_RPC_H
#	include <rpc.h>
#endif
#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#elif HAVE_WAIT_H
#include <wait.h>
#endif

extern "C" 
{
	u_long gettransient(const char *);
#if !HAVE_DECL_GET_MYADDRESS
	void get_myaddress(struct sockaddr_in *);
#endif
	int taco_gethostname(char *,size_t);
}

typedef char 	*(* DbRpcLocalFunc)(...);
static void 	msgserver_prog_1 (struct svc_req *, SVCXPRT *);
void 		register_msg (char *, char **);
void 		unreg_server (int);
extern "C" 	void exit_child (int);
_msg_out 	*rpc_msg_send_1(_msg_data *);

MessageServerPart 	msg;
int	 	  	pid = 0;
log4cpp::Category       *logStream;

void usage(const char *cmd)
{
	fprintf (stderr, "usage: %s [-h] [-v] nethost\n",cmd);
	fprintf (stderr, "  -h displays this information and exits\n");
        fprintf (stderr, "  -v displays the version number and exits\n");
	exit (1);
}

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
	char	*nethost;
	char    *dshome;

        int             c;
        extern int      optind;
        extern char     *optarg;

        while ((c = getopt(argc, argv, "hv")) != EOF)
                switch(c)
                {
			case 'v':
				printf("%s version : %s\n", argv[0], VERSION);
				exit(0);
                        case 'h' :
			case '?' :
				usage(argv[0]);
		}

	if (optind != argc - 1)
		usage(argv[0]);

        init_logstream("MessageServer");

	strncpy (msg.name, argv[0], sizeof(msg.name) - 1);
	nethost = argv[optind];

/*
 *  get process ID
 */
  	pid = getpid ();
/*
 * install signal handling
 */
	(void) signal(SIGHUP,  SIG_IGN);
	(void) signal(SIGINT,  unreg_server);
	(void) signal(SIGQUIT, unreg_server);
	(void) signal(SIGTERM, unreg_server);

#ifdef __hpux
        (void) signal (SIGCLD,SIG_IGN);
#endif /* __hpux */

#ifdef sun
        (void) signal (SIGCLD,exit_child);
#endif /* sun */

/*
 *  get transient progamm number and host name
 */

/* M. Diehl, 15.11.99
 * Use new gettransient() interface. Use some program description to create the hash value. This will 
 * prevent conflicts with database server. When both programms try to register numbers beginning at a 
 * common base leading to a race condition!
 */
  	msg.prog_number = gettransient("MessageServer");
  	taco_gethostname (msg.host_name, sizeof(msg.host_name) - 1);
	msg.host_name[sizeof(msg.host_name) - 1] = '\0';

	logStream->noticeStream() << "Version : " << VERSION << log4cpp::eol;
	logStream->noticeStream() << "Starting with program number " << msg.prog_number << " on host " << msg.host_name << log4cpp::eol;
/*
 *  register message-server to netwok manager
 */
	register_msg (nethost, &dshome);
	logStream->noticeStream() << "registered on NETHOST " << nethost << " with home=" << dshome << " and DISPLAY=" << msg.display << log4cpp::eol;
	
/*
 *  create server handle
 */

/* M. Diehl, 15.11.99
 * Since gettransient() does not bind sockets and pmap_set  prognums anymore, the patches required 
 * for Solaris and Linux/glibc 2.x (and probably for every other well-behaving system) have been removed.
 */
	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) 
	{
		logStream->fatalStream() << "Cannot create udp service, exiting..." << log4cpp::eol;
		kill (pid,SIGQUIT);
	}

	if (!svc_register(transp, msg.prog_number, MSGSERVER_VERS, msgserver_prog_1, IPPROTO_UDP)) 
	{
		logStream->fatalStream() << "Unable to register server, exiting..." << log4cpp::eol;
		kill (pid,SIGQUIT);
	}

/*
 *  startup message server
 */
        msg_initialise (dshome);
	logStream->noticeStream() << "initialized" << log4cpp::eol;

/*
 *  set server into wait status
 */
	logStream->noticeStream() << "ready to run" << log4cpp::eol;
	svc_run();
	logStream->fatalStream() << "svc_run returned. Exiting." << log4cpp::eol << log4cpp::eol;
	kill (pid,SIGQUIT);
}

static void msgserver_prog_1 (struct svc_req *rqstp, SVCXPRT *transp)
{
	union {
		_msg_data 	rpc_msg_send_1_arg;
	} argument;

	char	*result;
	DbRpcLocalFunc local;
	bool_t 	(*xdr_argument)(XDR*, _msg_data*), 
		(*xdr_result)(XDR*, _msg_out*);

/*
 *  call the right server routine
 */
	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
#if !defined (unix) 
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif 
			return;
		case RPC_MSG_SEND:
			xdr_argument = xdr__msg_data;
			xdr_result = xdr__msg_out;
			local = (DbRpcLocalFunc)rpc_msg_send_1;
			break;

		case RPC_STARTUP_MSG:
#if !defined (unix) 
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif 
			msg_alarm_handler (-1, "Network_Manager", msg.host_name, "NULL", msg.display);
			return;

		case RPC_QUIT_SERVER:
#if !defined (unix) 
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif 
			kill (pid,SIGQUIT);
			return;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, (xdrproc_t)xdr_argument, (char *)&argument)) 
	{
		msg_fault_handler("svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}

	result = (*local)(&argument, rqstp);
#if !defined (unix) 
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) 
#else
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t)xdr_result, result)) 
#endif 
	{
		msg_fault_handler("svcerr_systemerr : server couldn't send repply arguments");
		svcerr_systemerr(transp);
	}

	if (!svc_freeargs(transp, (xdrproc_t)xdr_argument, (char *)&argument)) 
	{
		msg_fault_handler("svc_freeargs : server couldn't free arguments !!");
		return;
	}
}

void register_msg (char *nethost, char **dshome)
{
	_register_data		 register_data;
	static _msg_manager_data msg_manager_data;
	CLIENT			 *clnt;
	enum clnt_stat  	 clnt_stat;
	int 			 pid = 0;


  	pid = getpid ();
       	memset (&msg_manager_data, 0, sizeof (msg_manager_data));

/*
 * Create network manager client handle to nethost 
 */
	clnt = clnt_create(nethost, NMSERVER_PROG,NMSERVER_VERS, "udp");
  	if (clnt == NULL)
     	{
		clnt_pcreateerror("register_msg");
		kill(pid, SIGQUIT);
	}
    	clnt_control(clnt, CLSET_RETRY_TIMEOUT, (char *)&retry_timeout);
    	clnt_control(clnt, CLSET_TIMEOUT, (char *)&timeout);


/*
 *  send server configuration to network manager and get back DSHOME and DISPLAY.
 */
	register_data.host_name   = msg.host_name;
	register_data.prog_number = msg.prog_number;
	register_data.vers_number = MSGSERVER_VERS;

#if !defined (unix) 
	clnt_stat = clnt_call(clnt, RPC_MSG_REGISTER,
			       xdr__register_data, &register_data,
			       xdr__msg_manager_data, &msg_manager_data,timeout);
#else
	clnt_stat = clnt_call (clnt, RPC_MSG_REGISTER,
			       (xdrproc_t)xdr__register_data, (char *)&register_data,
			       (xdrproc_t)xdr__msg_manager_data, (char *)&msg_manager_data,timeout);
#endif 
	if (clnt_stat != RPC_SUCCESS)
	{
		clnt_perror(clnt, "register_msg");
		kill (pid,SIGQUIT);
	}

	*dshome = msg_manager_data.dshome;
	strncpy(msg.display, msg_manager_data.display, sizeof(msg.display) - 1);
	msg.display[sizeof(msg.display) - 1] = '\0';
	clnt_destroy (clnt);
}

void unreg_server (int signo)
{
	pmap_unset(msg.prog_number, MSGSERVER_VERS);
	logStream->noticeStream() << "received signal " << signo << log4cpp::eol;
	logStream->noticeStream() << "unregistered." << log4cpp::eol;
	logStream->noticeStream() << "exited." << log4cpp::eol << log4cpp::eol;
	exit(1);
}


#ifdef sun
extern "C" void exit_child (int signo)
{
	int 	status;
	wait (&status);
}
#endif /* sun */



