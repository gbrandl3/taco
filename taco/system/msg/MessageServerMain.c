
/*********************************************************************

 File:          MessageServerMain.c

 Project:       Device Servers with sunrpc

 Description:   Main Programm for a Message-Server

 Author(s):    	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.3 $

 Date:		$Date: 2003-11-28 13:14:55 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

			All Rights Reserved

*********************************************************************/

#include <API.h>
#include <private/ApiP.h>
#include <Message.h>
#include <signal.h>


static void 	msgserver_prog_1();
void 		register_msg ();
void 		unreg_server ();
void 		exit_child ();

MessageServerPart msg;
int	 	  pid = 0;

main (int argc, char **argv)
{
	SVCXPRT *transp;
	char	*nethost;
	char    *dshome;


	if (argc < 2)
	{
		fprintf (stderr, "usage: %s nethost\n",argv[0]);
		exit (1);
	}

	snprintf (msg.name, sizeof(msg.name), "%s",argv[0]);
	nethost = argv[1]; 
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
 * Use new gettransient() interface. Use some program description
 * to create the hash value. This will prevent conflicts with
 * database server. When both programms try to register numbers
 * beginning at a common base leading to a race condition!
 */
  	msg.prog_number = gettransient("MessageServer");
  	gethostname (msg.host_name, 32);

/*
 *  register message-server to netwok manager
 */
	register_msg (nethost,&dshome);
	
/*
 *  create server handle
 */

/* M. Diehl, 15.11.99
 * Since gettransient() does not bind sockets and pmap_set
 * prognums anymore, the patches required for Solaris and
 * Linux/glibc 2.x (and probably for every other well-behaving
 * system) have been removed.
 */
	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) 
	{
		fprintf(stderr, "Cannot create udp service, exiting...\n");
		kill (pid,SIGQUIT);
	}

	if (!svc_register(transp, msg.prog_number, MSGSERVER_VERS, 
			  msgserver_prog_1, IPPROTO_UDP)) 
	{
		fprintf(stderr, "Unable to register server, exiting...\n"); 
		kill (pid,SIGQUIT);
	}

/*
 *  startup message server
 */
        msg_initialise (dshome);

/*
 *  set server into wait status
 */
	svc_run();
	fprintf(stderr, "svc_run returned\n");
	kill (pid,SIGQUIT);
}

static void msgserver_prog_1 (struct svc_req *rqstp, SVCXPRT *transp)
{
	union {
		_msg_data 	rpc_msg_send_1_arg;
	} argument;

	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();

	/*
	 *  call the right server routine
	 */

	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
#if !defined (linux) && !defined(FreeBSD)
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* !linux */
			return;
		case RPC_MSG_SEND:
			xdr_argument = xdr__msg_data;
			xdr_result = xdr__msg_out;
			local = (char *(*)()) rpc_msg_send_1;
			break;
		case RPC_STARTUP_MSG:
#if !defined (linux) && !defined(FreeBSD)
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* !linux */
			msg_alarm_handler (-1,"Network_Manager",
				   msg.host_name,"NULL",msg.display);
			return;

		case RPC_QUIT_SERVER:
#if !defined (linux) && !defined(FreeBSD)
			svc_sendreply(transp, xdr_void, NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* !linux */
			kill (pid,SIGQUIT);
			return;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, (xdrproc_t)xdr_argument, (char *)&argument)) 
	{
		msg_fault_handler 
		("svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}

	result = (*local)(&argument, rqstp);
#if !defined (linux) && !defined(FreeBSD)
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) 
#else
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t)xdr_result, result)) 
#endif /* !linux */
	{
		msg_fault_handler 
		("svcerr_systemerr : server couldn't send repply arguments");
		svcerr_systemerr(transp);
	}

	if (!svc_freeargs(transp, (xdrproc_t)xdr_argument, (char *)&argument)) 
	{
		msg_fault_handler 
		("svc_freeargs : server couldn't free arguments !!");
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

	clnt = clnt_create ( nethost,NMSERVER_PROG,NMSERVER_VERS,"udp");
  	if (clnt == NULL)
     	{
		clnt_pcreateerror ("register_msg");
		kill (pid,SIGQUIT);
	}

    	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *)&retry_timeout);
    	clnt_control (clnt, CLSET_TIMEOUT, (char *)&timeout);


/*
 *  send server configuration to network manager
 *  and get back DSHOME and DISPLAY.
 */
	register_data.host_name   = msg.host_name;
	register_data.prog_number = msg.prog_number;
	register_data.vers_number = MSGSERVER_VERS;

#if !defined (linux) && !defined(FreeBSD)
	clnt_stat = clnt_call (clnt, RPC_MSG_REGISTER,
			       xdr__register_data,&register_data,
			       xdr__msg_manager_data,&msg_manager_data,timeout);
#else
	clnt_stat = clnt_call (clnt, RPC_MSG_REGISTER,
			       (xdrproc_t)xdr__register_data, (char *)&register_data,
			       (xdrproc_t)xdr__msg_manager_data, (char *)&msg_manager_data,timeout);
#endif /* !linux */
	if (clnt_stat != RPC_SUCCESS)
	{
		clnt_perror (clnt,"register_msg");
		kill (pid,SIGQUIT);
	}

	*dshome = msg_manager_data.dshome;
	snprintf (msg.display, sizeof(msg.display), "%s", msg_manager_data.display);
	clnt_destroy (clnt);
}

void unreg_server (int signo)
{
	pmap_unset(msg.prog_number, MSGSERVER_VERS);
	exit(1);
}


#ifdef sun
void exit_child (int signo)
{
	int 	status;
	wait (&status);
}
#endif /* sun */



