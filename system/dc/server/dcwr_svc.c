/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File         : dcwr_svc.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author(s)    : E. Taurel
 *                $Author: andy_gotz $
 *
 * Original     : February 1993
 *
 * Version      : $Revision: 1.14 $
 *
 * Date         : $Date: 2009-09-23 11:42:34 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#if HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#elif HAVE_RPC_H
#	include <rpc.h>
#endif
#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif
#if HAVE_SYS_SEM_H
#	include <sys/sem.h>
#endif
#ifdef _solaris
#define PORTMAP
#endif

#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>

#if HAVE_SIGNAL_H
#	include <signal.h>
#endif
#include <errno.h>
#if HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#	include <netinet/in.h>
#endif
#if HAVE_NETDB_H
#	include <netdb.h>
#endif
#include <string.h>
#if HAVE_SYS_TYPE_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_IPC_H
#	include <sys/ipc.h>
#endif
#if HAVE_SYS_SHM_H
#	include <sys/shm.h>
#endif
#if HAVE_SYS_SEM_H
#	include <sys/sem.h>
#endif
#if HAVE_SYS_RESOURCE_H
#	include <sys/resource.h>
#endif
#define DC_SERVER 1
#include <dc_xdr.h>
#include <dcP.h>

#include <errno.h>

/* Function definition */

static void dc_prog_1(struct svc_req *, SVCXPRT *);
static void leave(int);
static void register_dc(char *, char *, u_long, u_long);
static void one_more_request(void);
static int db_register(char *, unsigned int, unsigned int, char *, char *);
static int shm_size(char *);

/* Some static variables */
static SVCXPRT *transp_sta;

/* Some external variables */
extern dc_devallx_back ret_devall;

/* Added definition */
u_long		pgnum;
char		d_name[DEV_NAME_LENGTH],
		ds_name[DEV_NAME_LENGTH],
		psd_name[DEV_NAME_LENGTH],
		*addr_alloc,
		*addr_ptr,
		*addr_data;
upd_reqnb 	req;
hash_info 	mem;
DevLong		ctr;
int		req_call,
		time_out = False,
		shmid_alloc,
		shmid_ptr,
		shmid_data,
 		ptr_size,
		dat_size,
		alloc_size,
 		semid,
		semid1;

db_resource 	res_serv_put,
	 	res_serv_get[] = {
			{"start_req", D_LONG_TYPE},
			{"start_nb", D_LONG_TYPE},
			{"update", D_LONG_TYPE},
		},
		res1[] = {
			{"dev_number", D_LONG_TYPE},
			{"cellar_number", D_LONG_TYPE},
			{"data_size", D_LONG_TYPE},
		};
int 		res_serv_get_size = sizeof(res_serv_get)/sizeof(db_resource),
 		res1_size = sizeof(res1) / sizeof(db_resource);

#ifdef DC_DATAPORT
#include <dataport.h>
Dataport *dp_wr;		/* pointer on the dataport */
char dp_wr_name[15];	/* name of the shared memory (dataport) */
typedef	struct {
	int ctr;
} Sharedstruct_wr;
Sharedstruct_wr *shared_wr;
#endif /* DC_DATAPORT */
 
#define DC_SANITY 1
int *dc_sanity_bufsize;

/* Three differents signal routines */
void default_sig(int signo)
{
        time_t 		tps = time((time_t *)0);
        struct tm 	*time_tm = localtime(&tps);
        char 		*tps_str = asctime(time_tm);
	char		*p;
	p = strchr(tps_str, '\n');
	*p = '\0';
        fprintf(stderr,"%s : signal %d received !!!\n",tps_str,signo);
#ifndef _solaris
        fprintf(stderr,"Server requested from %x\n",transp_sta->xp_raddr.sin_addr.s_addr);
#endif
	fflush(stderr);
#ifdef DC_DATAPORT
	CloseDataport(dp_wr, dp_wr_name);
#endif /* DC_DATAPORT */
}

void un_register_prog(int signo)
{
	DevLong error;
	pmap_unset(pgnum, DC_VERS);
/* Added code to unregister the server from static db */
	db_svc_unreg(ds_name, &error);
#ifdef DC_DATAPORT
	CloseDataport(dp_wr, dp_wr_name);
#endif /* DC_DATAPORT */
	exit(1);
}

void time_out_prog(int signo)
{
	time_out = True;
}


/**
 *
 * DATA COLLECTOR WRITE SERVER MAIN FUNCTION
 *
 */
int main(int argc, char **argv)
{
	SVCXPRT 		*transp_tcp,
				*transp_udp;
	struct sockaddr_in 	so;
	char 			hostna[HOST_NAME_LENGTH],
				full_name[1024];
	int			i;
#ifdef OBSOLETE_SUN
	int 			sig_mask;
	struct sigvec 		sighand;
#endif
#if defined( linux) || defined (FreeBSD)
	struct sigaction 	sigact;
#endif /* linux */

/* Added variables to manage transient program number */
	int 			sock_tcp,
				sock_udp;
	char 			*netmanhost,
				*serv_num;
	struct rlimit 		lim;

#ifndef ALONE
	if (argc != 3) 
	{
		fprintf(stderr,"%s usage: %s <network manager host name> <server number>\n",argv[0],argv[0]);
		exit(1);
	}
	netmanhost = argv[1];
	serv_num = argv[2];
#else
	if (argc != 2) 
	{
		fprintf(stderr,"%s usage: %s <server number>\n",argv[0],argv[0]);
		exit(1);
	}
	serv_num = argv[1];
#endif /* ALONE */

#ifdef DC_DATAPORT
/* Create the Dataport to have a diagnostic on the load of the server */
	strncpy(dp_wr_name,"dc_wr_alo_",11);
	strncat(dp_wr_name,argv[1],2);
/* 
 * If dataport exists, destroy it.
 **********************************
 */
	if((dp_wr = OpenDataport(dp_wr_name,sizeof(Sharedstruct_wr))) != NULL)
	{
		CloseDataport(dp_wr, dp_wr_name);
		printf("dataport %s already existed, destroyed !\n", dp_wr_name);
	}
/* 
 * create a new dataport
 **********************************
 */
	dp_wr = CreateDataport(dp_wr_name, sizeof(Sharedstruct_wr));
	if (dp_wr==NULL)
	{
		fprintf(stderr,"Dataport %s can't be created\n",dp_wr_name);
	}
	else
	{
		printf("Dataport %s created\n",dp_wr_name);
	}
	shared_wr = (Sharedstruct_wr *)&(dp_wr->body);
	if (AccessDataport(dp_wr) == -1)
	{
	   	fprintf(stderr,"Can't access Dataport %s, errno = %d\n", dp_wr_name,errno);
	}
/*
 * Initialize the shared values now 
 *
 */
	shared_wr->ctr	= 0;

	if (ReleaseDataport(dp_wr) == -1)
	{
		fprintf(stderr,"Can't release Dataport %s, errno = %d\n", dp_wr_name,errno);
	}
#endif /* DC_DATAPORT */

/* Install signal handlers */
#ifdef OBSOLETE_SUN
	sig_mask = sigmask(SIGHUP);
	sigsetmask(sig_mask);

	sighand.sv_handler = un_register_prog;
	sighand.sv_mask = 0;
	sighand.sv_flags = 0;

	sigvec(SIGINT,&sighand,NULL);
	sigvec(SIGQUIT,&sighand,NULL);
	sigvec(SIGTERM,&sighand,NULL);

	sighand.sv_handler = time_out_prog;
	sigvec(SIGALRM,&sighand,NULL);

	sighand.sv_handler = default_sig;
	sigvec(SIGPIPE,&sighand,NULL);
	sigvec(SIGVTALRM,&sighand,NULL);
	sigvec(SIGPROF,&sighand,NULL);
#ifndef _solaris
	sigvec(SIGLOST,&sighand,NULL);
#endif
	sigvec(SIGUSR1,&sighand,NULL);
	sigvec(SIGUSR2,&sighand,NULL);
	sigvec(SIGXCPU,&sighand,NULL);
	sigvec(SIGXFSZ,&sighand,NULL);
#else
#if !defined (linux) && !defined(FreeBSD)
	(void) sigignore(SIGHUP);
	(void) sigset(SIGINT, un_register_prog);
	(void) sigset(SIGQUIT, un_register_prog);
	(void) sigset(SIGTERM, un_register_prog);
	(void) sigset(SIGALRM, time_out_prog);
	(void) sigset(SIGPIPE, default_sig);
	(void) sigset(SIGUSR1, default_sig);
	(void) sigset(SIGUSR2, default_sig);
	(void) sigset(SIGVTALRM, default_sig);
	(void) sigset(SIGPROF, default_sig);
#if !defined( _solaris) 
	(void) sigset(SIGLOST, default_sig);
#endif /* _solaris */
#else
	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);
        sigact.sa_handler = SIG_IGN;
        sigaction(SIGHUP, &sigact, NULL);
        sigact.sa_handler = un_register_prog;
        sigaction(SIGINT, &sigact, NULL);
        sigact.sa_handler = un_register_prog;
        sigaction(SIGQUIT, &sigact, NULL);
        sigact.sa_handler = un_register_prog;
        sigaction(SIGTERM, &sigact, NULL);
        sigact.sa_handler = time_out_prog;
        sigaction(SIGALRM, &sigact, NULL);
        sigact.sa_handler = default_sig;
        sigaction(SIGPIPE, &sigact, NULL);
        sigact.sa_handler = default_sig;
        sigaction(SIGUSR1, &sigact, NULL);
        sigact.sa_handler = default_sig;
        sigaction(SIGUSR2, &sigact, NULL);
        sigact.sa_handler = default_sig;
        sigaction(SIGVTALRM, &sigact, NULL);
        sigact.sa_handler = default_sig;
        sigaction(SIGPROF, &sigact, NULL);
#endif /* !linux */                                                             
#endif

/* Change max number of open files */

	if (getrlimit(RLIMIT_NOFILE,&lim) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get max number of files\n");
		exit(-1);
	}
	lim.rlim_cur = DC_MAX_FILES;
	if (setrlimit(RLIMIT_NOFILE,&lim) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't change max number of files\n");
		exit(-1);
	}

	taco_gethostname(hostna,sizeof(hostna));

/* Added code to manage transient program number and to get host name*/

	sock_tcp = sock_udp = RPC_ANYSOCK;
	strcpy(full_name,argv[0]);
	strcat(full_name,"/");
	strcat(full_name,argv[1]);
	
	pgnum = gettransient(full_name);

	if (pgnum == 0)
	{
		fprintf(stderr,"dc_server_wr : Can't get transcient program number\n");
		leave(NO_UNREG);
	}

#ifdef DEBUG
	printf("Server host name : %s\n",hostna);
	printf("Program number : %x in hexa or %d in decimal\n", pgnum, pgnum);
#endif /* DEBUG */

	transp_tcp = svctcp_create(sock_tcp,0,0);
	if (transp_tcp == NULL)
	{
		fprintf(stderr, "cannot create TCP service.\n");
		leave(NO_UNREG);
	}

/* Register the server with the TCP protocol */
	if (svc_register(transp_tcp, pgnum, DC_VERS, dc_prog_1, IPPROTO_TCP) == 0)
	{
		fprintf(stderr,"unable to register (pgnum,DC_VERS,tcp). \n");
		leave(NO_UNREG);
	}

/* Register the server with the UDP protocol */
	transp_udp = svcudp_create(sock_udp);
	if (transp_udp == NULL)
	{
		fprintf(stderr, "cannot create UDP service.\n");
		leave(NO_UNREG);
	}
	if (svc_register(transp_udp, pgnum, DC_VERS, dc_prog_1, IPPROTO_UDP) == 0)
	{
		fprintf(stderr,"unable to register (pgnum,DC_VERS,udp). \n");
		leave(NO_UNREG);
	}

/* Register myself in the static database */
	if (db_register(serv_num, pgnum, DC_VERS, hostna, argv[0]))
		leave(NO_UNREG);

/* Retrieve shared memory segments size */
	if (shm_size(hostna))
		leave(UNREG);

/* Attach the alloc area (it's a shared memory segment) to this process data area */
	if ((shmid_alloc = shmget((key_t)KEY_ALLOC,(size_t)alloc_size, 0666)) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get the allocation table\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		fprintf(stderr,"dc_server_wr : Error code : %s\n", strerror(errno));
		leave(UNREG);
	}

	if ((addr_alloc = (char *)shmat(shmid_alloc,(char *)0,0)) == (char *)-1)
	{
		fprintf(stderr,"dc_server_wr : Can't attach to the allocation area shred memory segment\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		leave(UNREG);
	}
#ifdef DEBUG
	fprintf(stderr, "ALLOC : SHMID = %d (%d), ptr = %p\n", shmid_alloc, alloc_size, addr_alloc);
#endif

/* Attach the data buffer (it's a shared memory segment) to this process data area */
	if ((shmid_data = shmget((key_t)KEY_DATBUF,(size_t)dat_size, 0666)) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get the data buffer\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		leave(UNREG);
	}

	if ((addr_data = (char *)shmat(shmid_data,(char *)0,0)) == (char *)-1)
	{
		fprintf(stderr,"dc_server_wr : Can't attach to the data buffer shred memory segment\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		leave(UNREG);
	}
#ifdef DEBUG
	fprintf(stderr, "DATA : SHMID = %d (%d), ptr = %p\n", shmid_data, dat_size, addr_data);
#endif

/* Attach the pointers buffer (it's a shared memory segment) to this process data area */
	if ((shmid_ptr = shmget((key_t)KEY_PTR,(size_t)ptr_size, 0666)) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get the pointers buffer\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		leave(UNREG);
	}

	if ((addr_ptr = (char *)shmat(shmid_ptr,(char *)0,0)) == (char *)-1)
	{
		fprintf(stderr,"dc_server_wr : Can't attach to the pointers buffer shred memory segment\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",errno);
		leave(UNREG);
	}
#ifdef DEBUG
	fprintf(stderr, "PTR : SHMID = %d (%d), ptr = %p\n", shmid_ptr, ptr_size, addr_ptr);
#endif

/* Get the semaphore id used to protect the alloc area */
	if ((semid = semget(SEM_KEY,1, 0666)) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get the alloc semaphore\n");
		perror("dc_server_wr ");
		leave(UNREG);
	}

/* Get the semaphore set id used to protect the pointers area */
	if ((semid1 = semget(SEMPTR_KEY,2, 0666)) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't get the ptrs semaphore\n");
		perror("dc_server_wr ");
		leave(UNREG);
	}
#ifdef DEBUG
	fprintf(stderr, "SEM : SHMID = %d (%d)\n", semid1, 2);
#endif
			
/* End of the hash_info structure initialisation */
	mem.sem_id = semid1;
	mem.parray = (dc_dev_param *)addr_ptr;

#ifndef ALONE
/* Send program number,host name and server version to network manager */
 	register_dc(netmanhost,hostna,pgnum,DC_VERS);
#endif /* ALONE */

	svc_run();
	fprintf(stderr, "svc_run returned\n");
	exit(1);
}

static void dc_prog_1(struct svc_req *rqstp, SVCXPRT *transp)
{
	union {
		dc_open_in dc_open_1_arg;	
		name_arr dc_close_1_arg;
		dev_datarr dc_dataput_1_arg;
		int fill;
		name dc_devinfo_1_arg;
	} argument;
	char 		*result;
	bool_t 		(*xdr_argument)(), 
			(*xdr_result)();
	char 		*(*local)();
/* Added variables */
	int 		pid,
			i;
	int 		max;
	char 		*tmp_ch;

	transp_sta = transp;

	switch (rqstp->rq_proc)
	{
		case NULLPROC:
			svc_sendreply(transp, (const xdrproc_t)xdr_void, (caddr_t)NULL);
			return;
		case DC_OPEN:
			xdr_argument = xdr_dc_open_in;
			xdr_result = xdr_dc_xdr_error;
			local = (char *(*)()) dc_open_1;
			break;
		case DC_CLOSE:
			xdr_argument = xdr_name_arr;
			xdr_result = xdr_dc_xdr_error;
			local = (char *(*)()) dc_close_1;
			break;
		case DC_DATAPUT:
			xdr_argument = xdr_dev_datarr;
			xdr_result = xdr_dc_xdr_error;
			local = (char *(*)()) dc_dataput_1;
			break;
		case DC_INFO:
			xdr_argument = xdr_void;
			xdr_result = xdr_dc_infox_back;
			local = (char *(*)()) dc_info_1;
			break;
		case DC_DEVALL:
			xdr_argument = xdr_void;
			xdr_result = xdr_dc_devallx_back;
			local = (char *(*)()) dc_devall_1;
			break;
		case DC_DEVINFO:
			xdr_argument = xdr_name;
			xdr_result = xdr_dc_devinfx_back;
			local = (char *(*)()) dc_devinfo_1;
			break;
		case RPC_CHECK:
			tmp_ch = ds_name;
			svc_sendreply(transp,(const xdrproc_t)xdr_wrapstring,(caddr_t)&tmp_ch);
			return;
		case RPC_QUIT_SERVER:
			svc_sendreply(transp,(const xdrproc_t)xdr_void,(caddr_t)NULL);
			pid = getpid();
			kill(pid,SIGQUIT);
			return;
		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, (const xdrproc_t)xdr_argument, (caddr_t)&argument))
	{
		svcerr_decode(transp);
		return;
	}
	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (const xdrproc_t)xdr_result, (caddr_t)result))
	{
		svcerr_systemerr(transp);
	}
	if (!svc_freeargs(transp, (const xdrproc_t)xdr_argument, (caddr_t)&argument))
	{
		fprintf(stderr, "unable to free arguments\n");
		exit(1);
	}
	one_more_request();

/* Added code to free memory allocated in the server functions */
	switch(rqstp->rq_proc)
	{
		case DC_DEVALL :
			if (ret_devall.err_code == 0)
			{
				max = ret_devall.dev_name.name_arr_len;
				for (i = 0;i < max;i++)
					free(ret_devall.dev_name.name_arr_val[i]);
				free(ret_devall.dev_name.name_arr_val);
			}
			break;
	}
}


#ifndef ALONE
/**
 * To send server information (host_name,program number
 * and version number) to the network manager
 * 
 * This function is used only when the database is a part of the device
 * server system
 * 
 * @param netman_host 	The system host_name where the network manager is running
 * @param host		The system host_name where the server is running
 * @param prog		The server program number
 * @param vers		The server version number
 *                                                                           *
 */
static void register_dc(char *netman_host, char *host, u_long prog, u_long vers)
{
	_register_data 	register_data;
	CLIENT 		*netman_clnt;
	enum clnt_stat 	clnt_stat;
	static int 	res;

/* Create an RPC connection to network manager */
	netman_clnt = clnt_create(netman_host,NMSERVER_PROG,NMSERVER_VERS,"udp");
	if (netman_clnt == NULL)
	{
		fprintf(stderr,"Unable to create connection to network manager\n");
		exit(1);
	}

	clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&retry_timeout);
	clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&timeout);

/* Send informations to network manager */
	register_data.host_name = host;
	register_data.prog_number = prog;
	register_data.vers_number = vers;

	clnt_stat = clnt_call(netman_clnt,RPC_DB_REGISTER, (xdrproc_t)xdr__register_data, (char *)&register_data,
							(xdrproc_t)xdr_int, (char *)&res,timeout);

	if (clnt_stat != RPC_SUCCESS)
	{
		clnt_perror(netman_clnt,"register_db");
		exit(1);
	}
		
/* Exit function */
	clnt_destroy(netman_clnt);
}
#endif /* ALONE */


/**
 * To export the pseudo device associated with this
 * server, to request for server resources and to set the 
 * resource "request number" to 0
 * 
 * @param serv_num 	The server number
 * @param pn_serv	The server program number
 * @param vn_serv	The server version number
 * @param host_name	The server host name
 * @param c_proc_name	The process name
 * 
 * @return This function returns DS_NOTOK if an error occurs. Otherwise, it returns DS_OK
 *
 */
static int db_register(char *serv_num, unsigned int pn_serv, unsigned int vn_serv, char *host_name, char *c_proc_name)
{
	struct hostent 	*host;
	DevLong		error;
	long		dev_def_err;
	db_devinf 	devinfo;
	unsigned char 	tmp = 0;
	unsigned int 	diff;
	char 		*tmp1,
			dev_type[DEV_TYPE_LENGTH],
			dev_class[DEV_CLASS_LENGTH],
			h_name[HOST_NAME_LENGTH],
			proc_name[PROC_NAME_LENGTH],
			*ptr,
			dev_def[256],
			**dev_def_array;
	
/* Get host information */
	if ((host = gethostbyname(host_name)) == NULL)
	{
		fprintf(stderr,"dc_server_wr : Can't get host info, exiting...\n");
		return(-1);
	}
	tmp = (unsigned char)host->h_addr[3];

/* Build the pseudo device server name */
	/*strcpy(ds_name, "dc_server_wr/"); - problems exporting the dc server, try a shorter name*/
	strcpy(ds_name, "dc_wr_alo/");
	strcpy(h_name, host->h_name);
	if ((tmp1 = strchr(h_name,'.')) != NULL)
	{
		diff = (u_int)(tmp1 - h_name);
		h_name[diff] = 0;
	}
	strcat(ds_name,h_name);
	strcat(ds_name,"_");
	strcat(ds_name,serv_num);

	h_name[0] = 0;

/* Build the pseudo device name */

	strcpy(d_name,"sys/dc_wr_");
	sprintf(&(d_name[strlen(d_name)]),"%u",tmp);
	strcat(d_name,"/");
	strcat(d_name,serv_num);

#ifdef DEBUG
	printf("Pseudo device server name : %s\n",ds_name);
	printf("Pseudo device name : %s\n",d_name);
#endif /* DEBUG */

/* 
   Copy the host name in the devinfo structure before the db_import call
   because the db_import use the clnt_create call wich use also the
   gethostbyname function and the returned structure is static!!
   Read the UNIX documentation, there is a little warning at this subject 
*/
	strcpy(h_name,host->h_name);

/* Import the static database server */
	if (db_import(&error) == -1)
	{
		fprintf(stderr,"dc_server_wr : Can't import the static database server, exiting...\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
		return(-1);
	}

/* Build real process name */
	ptr = strrchr(c_proc_name,'/');
	if (ptr != NULL)
	{
		ptr++;
		strcpy(proc_name,ptr);
	}
	else
		strcpy(proc_name,c_proc_name);

/* Export me to the outside world */
	strcpy(dev_type, "DevType_Default");
	strcpy(dev_class, "DcWriteServerClass");
	devinfo.device_type = dev_type;
	devinfo.device_class = dev_class;
	devinfo.device_name = d_name;
	devinfo.host_name = h_name;
	devinfo.pn = pn_serv;
	devinfo.vn = vn_serv;
	devinfo.proc_name = proc_name;

	if (db_dev_export(&devinfo,1,&error))
	{
/*
		if (error != DbErr_DeviceNotDefined || error != DbErr_DatabaseAccess)
		{
			fprintf(stderr,"dc_server_wr : Can't export me to outside world, exiting...\n");
			fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
		}
		else
 */
		{
			fprintf(stderr,"dc_server_wr : I am not defined, try to define me in the database...\n");
/* Build the device definition string */
			strcpy(dev_def,ds_name);
			strcat(dev_def,"/device:");
			strcat(dev_def,d_name);
			fprintf(stderr,"dc_server_wr : device definition = %s\n",dev_def);

			dev_def_array = calloc(1,sizeof(char *));
			dev_def_array[0] = dev_def;
		
			if (db_upddev(1,dev_def_array,&dev_def_err,&error) == -1)
			{
				fprintf(stderr,"dc_server_wr: Pseudo device for dc server not defined in db\n");
				fprintf(stderr," and I can't define it. Error = %d\n",error);
				return(-1);
			}
/* Export me to the outside world */
			if (db_dev_export(&devinfo,1,&error) == -1)
			{
				fprintf(stderr,"dc_server_wr : Can't export me to outside world even after trying to define myself, exiting...\n");
				fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
				return(-1);
			}
		}
	}
/* Retrieve server resources. Assign some default value sto these resources in
   case they are not defined in database */
	req.start_req = 8;
	req.start_nb = 8;
	req.update = 256;
	
	res_serv_get[0].resource_adr = &req.start_req;
	res_serv_get[1].resource_adr = &req.start_nb;
	res_serv_get[2].resource_adr = &req.update;

	if (db_getresource("CLASS/DC_WR/1",res_serv_get,res_serv_get_size,&error))
	{
		fprintf(stderr,"dc_server_wr : Can't retrieve my resources, exiting...\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
		return(-1);
	}

/* Init. request mask according to resources */
	if (req.start_req < 2 || req.start_req > 128)
	{
		fprintf(stderr,"dc_server_wr : start_req resource out of bound, exiting...\n");
		return(-1);
	}
	if (req.update < 2 || req.update > 1024)
	{
		fprintf(stderr,"dc_server_wr : update resource out of bound, exiting...\n");
		return(-1);
	}

	req.start = 0;
	switch(req.start_req)
	{
		case 2 : 
			req.start_shift = 1;
			break;
		case 4 : 
			req.start_shift = 2;
			break;
		case 8 : 
			req.start_shift = 3;
			break;
		case 16 : 
			req.start_shift = 4;
			break;
		case 32 : 
			req.start_shift = 5;
			break;
		case 64 : 
			req.start_shift = 6;
			break;
		case 128 : 
			req.start_shift = 7;
			break;
	}

	switch(req.update)
	{
		case 2 : 
			req.shift = 1;
			break;
		case 4 : 
			req.shift = 2;
			break;
		case 8 : 
			req.shift = 3;
			break;
		case 16 : 
			req.shift = 4;
			break;
		case 32 : 
			req.shift = 5;
			break;
		case 64 : 
			req.shift = 6;
			break;
		case 128 : 
			req.shift = 7;
			break;
		case 256 : 
			req.shift = 8;
			break;
		case 512 : 
			req.shift = 9;
			break;
		case 1024 : 
			req.shift = 10;
			break;
	}

/* Update the "server request" resource to 0 */
	strcpy(psd_name,"sys/dc_wr_");
	sprintf(&(psd_name[strlen(psd_name)]),"%u",tmp);
	strcat(psd_name,"/request");

#ifdef DEBUG
	printf("\nUpdate resource device name : %s\n",psd_name);
#endif /* DEBUG */

	ctr = 0;
	res_serv_put.resource_name = serv_num;
	res_serv_put.resource_type = D_LONG_TYPE;
	res_serv_put.resource_adr = &ctr;

	if (db_putresource(psd_name,&res_serv_put,1,&error))
	{
		fprintf(stderr,"dc_server_wr : Can't update the request resource, exiting\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
		return(-1);
	}

/* Leave function */
	return(0);
}



/**
 * To retrieve from the static database the shared memory 
 * segment size and to build the hashing algorithm
 * information in the "mem" structure.
 * 
 * @param host_name The host name
 * 
 * @return This function returns DS_NOTOK if an error occurs. Otherwise, it returns DS_OK.
 *	
 */
static int shm_size(char *host_name)
{
/*
	static long 	dev_num,
			dat_size1,
			cell_num;
 */
/*
 * changed the default values from zero to the values I have on id11
 */
        static DevLong  dev_num = 100,
                        dat_size1 = 252488,
                        cell_num = 50;

	unsigned int 	diff;
	int 		nb_tot;
	char 		dev_name[DEV_NAME_LENGTH],
			hostna[HOST_NAME_LENGTH],
			*tmp;
	DevLong		error;

/* Build the device name which is a function of the host name */
	strcpy(hostna,host_name);
	if ((tmp = strchr(hostna,'.')) != NULL)
	{
		diff = (u_int)(tmp - hostna);
		hostna[diff] = 0;
	}
	strcpy(dev_name,"CLASS/DC/");
	strcat(dev_name,hostna);

/* Retrieve data collector memories size */
	res1[0].resource_adr = &dev_num;
	res1[1].resource_adr = &cell_num;
	res1[2].resource_adr = &dat_size1;
	if (db_getresource(dev_name,res1,res1_size,&error))
	{
		fprintf(stderr,"dc_server_wr : Can't retrieve resources\n");
		fprintf(stderr,"dc_server_wr : Error code : %d\n",error);
		return(-1);
	}
	if (dev_num == 0 || dat_size1 == 0 || cell_num == 0)
	{
		fprintf(stderr,"dc_server_wr : Resources dev_number, cellar_number or data_size not defined\n");
		return(-1);
	}

/* Compute real memories size */
	nb_tot = cell_num + dev_num;
	ptr_size = (int)(nb_tot * (sizeof(dc_dev_param) + sizeof(int_level)));
	dat_size = dat_size1;
	alloc_size = (int)(dat_size1 / 256);

#ifdef DC_SANITY
	dc_sanity_bufsize = (int*)malloc(nb_tot*HIST*sizeof(int));
	memset(dc_sanity_bufsize, 0, nb_tot*sizeof(int));
#endif /* DC_SANITY */

/* Initialise the hashing algorithm parameters in the mem structure */
	mem.hash_table_size = dev_num;
	mem.cellar_size = cell_num;

	return(0);
}




/**
 * To count the request which arrive to this server and, 
 * to update the request resources when it is needed.
 *
 */
void one_more_request(void)
{
	DevLong 	error;
	int 		ctr1;
	int 		ctr_mul,
			ctr_mul_st,
			delta;
	time_t 		tps;
	char 		*tps_str;
	struct tm 	*time_tm;

/* If we are in the startup phase, update resource only "req.start_nb" time
   and every "req.start_mask" request */

	ctr1 = ctr + req_call;
	if (req.start < req.start_nb)
	{
		ctr_mul = (ctr >> req.start_shift);
		delta = ((ctr_mul + 1) << req.start_shift) - ctr;
		if (req_call >= delta)
		{
			ctr_mul_st = ctr1 >> req.start_shift;
			ctr = ctr_mul_st << req.start_shift;
#ifdef DEBUG
			printf("Update request resource. ctr = %d\n",ctr);
#endif /* DEBUG */
			if (db_putresource(psd_name,&res_serv_put,1,&error))
			{
				char		*p;
				tps = time((time_t *)0);
				time_tm = localtime(&tps);
				tps_str = asctime(time_tm);
				p = strchr(tps_str, '\n');
				*p = '\0';
				fprintf(stderr,"%s dc_server_wr : Can't update request resources\n",tps_str);
			}
			req.start = ctr_mul_st;
		}
	}
/* After the starting phase, update request resources every "req.update" request */
	else
	{
		ctr_mul = (ctr >> req.shift);
		delta = ((ctr_mul + 1) << req.shift) - ctr;
		if (req_call >= delta)
		{
			ctr = (ctr1 >> req.shift) << req.shift;
#ifdef DEBUG
			printf("Update request resource. ctr = %d\n",ctr);
#endif /* DEBUG */
			if (db_putresource(psd_name,&res_serv_put,1,&error))
			{
				char	*p;
				tps = time((time_t *)0);
				time_tm = localtime(&tps);
				tps_str = asctime(time_tm);
				p = strchr(tps_str, '\n');
				*p = '\0';
				fprintf(stderr,"%s dc_server_wr : Can't update request resources\n",tps_str);
			}
		}
	}
#ifdef DC_DATAPORT
	ctr = ctr1;
	if (AccessDataport(dp_wr) == -1)
	{
	   	fprintf(stderr,"Can't access Dataport %s, errno = % d\n", dp_wr_name,errno);
		return;
	}
	shared_wr->ctr = ctr1;
	
	if (ReleaseDataport(dp_wr) == -1)
	{
	   	fprintf(stderr,"Can't release Dataport %s, errno = % d\n", dp_wr_name,errno);
		return;
	}
#endif /* DC_DATAPORT */
}


/**
 * 
 * To terminate the server in a proper way.
 * 
 * @param flag A flag to unregister (or not to unregister) the server from  the static database
 * 
 */
static void leave(int flag)
{
	DevLong error;
/* Unregister server from portmapper */
	pmap_unset(pgnum, DC_VERS);
/* Unregister server from database (if necessary) */
	if (flag == UNREG)
		if (db_svc_unreg(ds_name, &error))
			fprintf(stderr,"dc_server_wr : Error during server unregister...\n");
/* Exit server */
#ifdef DC_DATAPORT
	CloseDataport(dp_wr, dp_wr_name);
#endif /* DC_DATAPORT */
	exit(-1);
}
