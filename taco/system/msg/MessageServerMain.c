/*********************************************************************

 File:          MessageServerMain.c

 Project:       Device Servers with sunrpc

 Description:   Main Programm for a Message-Server

 Author(s):    	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.10 $

 Date:		$Date: 2005-04-11 15:54:56 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

			All Rights Reserved

*********************************************************************/
#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <Message.h>

#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#error Could not find signal.h
#endif

static void 	msgserver_prog_1();
void 		register_msg ();
void 		unreg_server ();
void 		exit_child ();

MessageServerPart 	msg;
int	 	  	pid = 0;
FILE			*logFile;

char *getTimeString(const char *name)
{
	time_t          tps = time((time_t *)NULL);
	struct tm       *time_tm = localtime(&tps);
	static char     message[80];
	char            *tps_str = asctime(time_tm);
	tps_str[24] = '\0';

	snprintf(message, sizeof(message), "%s : %s : ", name, tps_str);
	return message;
}

int main (int argc, char **argv)
{
	SVCXPRT *transp;
	char	*nethost;
	char    *dshome;
	char	*logpath = getenv("LOGPATH");

	if (argc < 2)
	{
		fprintf (stderr, "usage: %s nethost\n",argv[0]);
		exit (1);
	}

	strncpy (msg.name, argv[0], sizeof(msg.name) - 1);
	nethost = argv[1];
	if (!logpath)
		logpath = getenv("DSHOME");
	if (!logpath)
		logpath = "/tmp";

	snprintf(msg.logfile, sizeof(msg.logfile), "%s/MessageServer.log", logpath); 
	logFile = fopen(msg.logfile, "w");

	if (!logFile)
		logFile = stderr;
/*
 *  get process ID
 */
  	pid = getpid ();
/*
 * install signal handling
 */
	(void) signal(SIGHUP,  unreg_server);
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
  	gethostname (msg.host_name, sizeof(msg.host_name) - 1);
	msg.host_name[sizeof(msg.host_name) - 1] = '\0';

	fprintf(logFile, "\n%s Starting with program number %d on host %s", getTimeString("MessageServer"), msg.prog_number, msg.host_name);
	fflush(logFile);
/*
 *  register message-server to netwok manager
 */
	register_msg (nethost, &dshome);
	fprintf(logFile, "\n%s registered on NETHOST %s with home=%s and DISPLAY=%s", getTimeString("MessageServer"), nethost, dshome, msg.display);
	fflush(logFile);
	
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
		fprintf(logFile, "\n%s Cannot create udp service, exiting...", getTimeString("MessageServer"));
		fflush(logFile);
		kill (pid,SIGQUIT);
	}

	if (!svc_register(transp, msg.prog_number, MSGSERVER_VERS, msgserver_prog_1, IPPROTO_UDP)) 
	{
		fprintf(logFile, "\n%s Unable to register server, exiting...", getTimeString("MessageServer")); 
		fflush(logFile);
		kill (pid,SIGQUIT);
	}

/*
 *  startup message server
 */
        msg_initialise (dshome);
	fprintf(logFile, "\n%s initialized", getTimeString("MessageServer"));
	fflush(logFile);

/*
 *  set server into wait status
 */
	fprintf(logFile, "\n%s ready to run", getTimeString("MessageServer"));
	fflush(logFile);
	svc_run();
	fprintf(logFile, "\n%s svc_run returned. Exiting.", getTimeString("MessageServer"));
	fflush(logFile);
	kill (pid,SIGQUIT);
}

static void msgserver_prog_1 (struct svc_req *rqstp, SVCXPRT *transp)
{
	union {
		_msg_data 	rpc_msg_send_1_arg;
	} argument;

	char	*result,
		*(*local)();
	bool_t 	(*xdr_argument)(), (*xdr_result)();

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
			local = (char *(*)()) rpc_msg_send_1;
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
	fprintf(logFile, "\n%s received signal %d.", getTimeString("MessageServer"), signo);
	fprintf(logFile, "\n%s unregistered.", getTimeString("MessageServer"));
	fprintf(logFile, "\n%s exited.", getTimeString("MessageServer"));
	fflush(logFile);
	fclose(logFile);
	exit(1);
}


#ifdef sun
void exit_child (int signo)
{
	int 	status;
	wait (&status);
}
#endif /* sun */



