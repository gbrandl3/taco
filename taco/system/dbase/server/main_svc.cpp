#ifdef sun
#define PORTMAP
#endif

#include <API.h>
#include <stdlib.h>
#include <private/ApiP.h>
#include "db_xdr.h"
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <rpc/pmap_clnt.h>

// C++ include

#include <iostream>
#include <fstream>
#include <NdbmClass.h>
#include <string>
#include <vector>

#ifdef sun
#include <new>
#endif

/* Special for database */


#include <fcntl.h>

/* Some global variables */

NdbmInfo dbgen;

u_short udp_port;
u_short tcp_port;

/* Extern varaibles used in this file */

extern db_res browse_back;
extern db_res send_back;
extern db_res dev_back;
extern db_res res_back;
extern db_resimp back;
extern db_devinfo_svc sent_back;
extern db_info_svc info_back;
extern vector<NdbmDomain> dom_list;
extern vector<NdbmDomain> res_list;
extern svcinfo_svc svcinfo_back;
extern vector<string> res_list_dev;
extern db_poller_svc poll_back;

extern int errno;

/* RPC function not defined within rpc include files !!
 * M. Diehl, 15.11.99
 * Not correct for GLIBC 2.x: get_myaddress() is defined
 * and gettransient() is one of our private functions!
 * Can we drop this by including DevServer.h?
 */

#include <rpc/clnt.h>            /* for get_myaddress() */

extern "C" 
{
  u_long gettransient(const char *);
}

typedef char * (* DbRpcLocalFunc)(...);

/* Added definition for transient program number */

u_long pgnum;

/* Local function */

static void register_db(char *,char *,u_long,u_long);
static void db_setupprog_1(struct svc_req *,SVCXPRT *);

void un_register_prog(int signo)
{
#ifdef ALONE
	pmap_unset(DB_SETUPPROG,DB_SETUPVERS);
	pmap_unset(DB_SETUPPROG,DB_VERS_2);
	pmap_unset(DB_SETUPPROG,DB_VERS_3);
#else
	pmap_unset(pgnum, DB_SETUPVERS);
	pmap_unset(pgnum,DB_VERS_2);
	pmap_unset(pgnum,DB_VERS_3);
#endif /* ALONE */

/* Added code to close database */

	for (int i=0;i<dbgen.TblNum;i++) 
		gdbm_close(dbgen.tid[i]);

	exit(1);
}

/* Default signal handler for strange UNIX signal which default action is to
   kill the process */

void default_sig(int signo)
{
	time_t tps;
	char *tps_str;
	struct tm *time_tm;

	tps = time((time_t *)0);
	time_tm = localtime(&tps);
	tps_str = asctime(time_tm);
	tps_str[24] = 0;
	cout << tps_str << " : signal " << signo << " received !!!" << endl;
}



/****************************************************************************
 *                                                                          *
 *									    *
 *              The NDBM static database server main function               *
 *		---------------------------------------------		    *
 *									    *
 *                                                                          *
 ****************************************************************************/

int main(int argc,char **argv)
{
	SVCXPRT 		*transp_udp;
	SVCXPRT 		*transp_tcp;
	struct sockaddr_in 	so;
	int 			flags = GDBM_WRCREAT; // O_RDWR; 
	char 			*ptr;
	int 			i,
				j;
	char 			hostna[32];
	struct sigaction 	sighand;
#ifdef sun
	struct hostent 		*host;
	unsigned long 		*ptmp_long;
#endif
 
#ifndef ALONE
	if (argc != 3)
	{
		cerr << "dbm_server usage: dbm_server <database name> <network manager host name>" << endl;
		exit(1);
	}
	string netmanhost(argv[2]);
#endif
		
//
// Install signal handler
//

	sighand.sa_handler = un_register_prog;
	sighand.sa_flags = 0;
	
	sigaction(SIGHUP,&sighand,NULL);
	sigaction(SIGINT,&sighand,NULL);
	sigaction(SIGQUIT,&sighand,NULL);
	sigaction(SIGTERM,&sighand,NULL);
	
	sighand.sa_handler = default_sig;
	
	sigaction(SIGALRM,&sighand,NULL);
	sigaction(SIGPIPE,&sighand,NULL);
	sigaction(SIGUSR1,&sighand,NULL);
	sigaction(SIGUSR2,&sighand,NULL);	
	sigaction(SIGVTALRM,&sighand,NULL);
	sigaction(SIGPROF,&sighand,NULL);
	
/* Find the dbm_database table names */        
	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		cerr << "dbm_server: Can't find environment variable DBTABLES" << endl;
		exit(-1);
	}
	
/* Change database table names to lowercase letter names */
	for (i = 0;i < strlen(ptr);i++)
		ptr[i] = tolower(ptr[i]);

/* Automatically add a names and a ps_names tables */
	dbgen.TblName[0] = "names";
	dbgen.TblName[1] = "ps_names";
	dbgen.ps_names_index = 1;
	dbgen.TblNum = 2;
		
/* Extract each table name */
	string dbtables(ptr);
	string::size_type pos,start;

	start = 0;
	pos = 0;	
	while ((pos = dbtables.find(',',pos)) != string::npos)
	{
		string tmp_str(dbtables,start,pos - start);
		if ((tmp_str == "names") || (tmp_str == "ps_names"))
		{
			pos++;
			start = pos;
			continue;
		}
		dbgen.TblName[dbgen.TblNum] = tmp_str;
		dbgen.TblNum++;
		pos++;
		start = pos;
	}
	string tmp_str(dbtables,start,dbtables.size() - start);
	dbgen.TblName[dbgen.TblNum] = tmp_str;
	dbgen.TblNum++;

/* Find the dbm_database files */        
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		cerr << "dbm_server: Can't find environment variable DBM_DIR" << endl;
		exit(-1);
	}
	
	string dir_name(ptr);

	if (dir_name[dir_name.size() - 1] != '/')
		dir_name.append(1,'/');

/* Open database tables according to the definition */

	for (i = 0;i < dbgen.TblNum;i++)
	{
		string dbm_file(dir_name);
		
		dbm_file.append(dbgen.TblName[i]);

		string uni_file(dbm_file);
//		uni_file.append(".dir");
/*
		ifstream fi(uni_file.c_str());
		if (!fi)
		{
			cerr << "dbm_server : Can't find file " << uni_file << endl;
			exit(-1);	
		}
*/
		dbgen.tid[i] = gdbm_open((char *)dbm_file.c_str(), 0, flags, 0666, NULL);
		if (dbgen.tid[i] == NULL)
		{
			cerr <<"dbm_server : Can't open " << dbgen.TblName[i] << " table" << endl; 
			for (long j = 0;j < i;j++)
				gdbm_close(dbgen.tid[i]);
			exit(-1);	
		}		
	}

/* Mark the server as connected to the database */
	dbgen.connected = True;


/* RPC business !!!!!!!!!!!!!!!!!!!! */
#ifdef ALONE
	transp_udp = svcudp_create(RPC_ANYSOCK);

	transp_tcp = svctcp_create(RPC_ANYSOCK,0,0);

	gethostname(hostna,sizeof(hostna));
	cout << "Server host name : " << hostna << endl;
#else
/* Added code to manage transient program number and to get host name*/

/* M. Diehl, 15.11.99
 * Use new gettransient() interface. Use some program description
 * to create the hash value. This will prevent conflicts with
 * message server. When both programms try to register numbers
 * beginning at a common base leading to a race condition!
 */
         
  	pgnum = gettransient("DatabaseServer");
  	
	if (pgnum == 0)
	{
		cerr << "dbm_server : Can't get transcient program number" << endl;
		exit(-1);
	}

	cout << "Program number : " << pgnum << endl;
	gethostname(hostna,sizeof(hostna));
	cout << "Server host name : " << hostna << endl;

/* Send these informations to network manager. Even if the server is now 
   supporting version one and two, register it in the Manager with version 1
   for compatibility with old release of device server. */
	
 	register_db((char *)netmanhost.c_str(),hostna,pgnum,DB_SETUPVERS);

/* End of added code */

        /* M. Diehl, 15.11.99
         * Since gettransient() does not bind sockets and pmap_set
         * prognums anymore, the patches required for Solaris and
         * Linux/glibc 2.x (and probably for every other well-behaving
         * system) have been removed.
         */

	transp_udp = svcudp_create(RPC_ANYSOCK);

	transp_tcp = svctcp_create(RPC_ANYSOCK,0,0);
#endif /* ALONE */

	if (transp_udp == NULL)
	{
		cerr << "cannot create udp service." << endl;
		exit(1);
	}
	if (transp_tcp == NULL)
	{
		cerr << "cannot create tcp service." << endl;
		exit(1);
	}

/* Register the server in TCP and UDP for version 1,2 and 3*/

#ifdef ALONE
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_SETUPVERS,db_setupprog_1,IPPROTO_UDP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_SETUPVERS,udp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_SETUPVERS,db_setupprog_1,IPPROTO_TCP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_SETUPVERS,tcp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_2,db_setupprog_1,IPPROTO_UDP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_2,db_setupprog_1,IPPROTO_TCP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_3,db_setupprog_1,IPPROTO_UDP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_3,db_setupprog_1,IPPROTO_TCP))
	{
		cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << endl;
		exit(1);
	}

#else

 	if (!svc_register(transp_udp,pgnum, DB_SETUPVERS, db_setupprog_1, IPPROTO_UDP))
 	{
 		cerr << "unable to register (pgnum, DB_SETUPVERS, udp)" << endl;
 		exit(1);
 	}
 	if (!svc_register(transp_tcp,pgnum,DB_SETUPVERS,db_setupprog_1,IPPROTO_TCP))
 	{
 		cerr << "unable to register (pgnum,DB_SETUPVERS,tcp)" << endl;
 		exit(1);
 	}
	if (!svc_register(transp_udp,pgnum, DB_VERS_2, db_setupprog_1, IPPROTO_UDP))
	{
		cerr << "unable to register (pgnum, DB_VERS_2, udp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_tcp,pgnum,DB_VERS_2,db_setupprog_1,IPPROTO_TCP))
	{
		cerr << "unable to register (pgnum,DB_VERS_2,tcp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_udp,pgnum, DB_VERS_3, db_setupprog_1, IPPROTO_UDP))
	{
		cerr << "unable to register (pgnum, DB_VERS_2, udp)" << endl;
		exit(1);
	}
	if (!svc_register(transp_tcp,pgnum,DB_VERS_3,db_setupprog_1,IPPROTO_TCP))
	{
		cerr << "unable to register (pgnum,DB_VERS_2,tcp)" << endl;
		exit(1);
	}
#endif /* ALONE */

/* Added code to ask portmapper for the port numbers */

	get_myaddress(&so);

#ifdef sun
	host = gethostbyname(hostna);
	if (host == NULL)
	{
		cerr << "Unable to get my IP address" << endl;
		exit(1);
	}
	ptmp_long = (unsigned long *)host->h_addr_list[0];
	so.sin_addr.S_un.S_addr = *ptmp_long;
#endif /* _solaris */

#ifdef ALONE
	if ((udp_port = pmap_getport(&so,DB_SETUPPROG,DB_SETUPVERS,IPPROTO_UDP)) == 0)
	{
#else
	if ((udp_port = pmap_getport(&so,pgnum,DB_SETUPVERS,IPPROTO_UDP)) == 0)
	{
#endif /* ALONE */
		cerr << "unable to retrieve udp port number" << endl;
		exit(1);
	}

#ifdef ALONE
	if ((tcp_port = pmap_getport(&so,DB_SETUPPROG,DB_SETUPVERS,IPPROTO_TCP)) == 0)
	{
#else
	if ((tcp_port = pmap_getport(&so,pgnum,DB_SETUPVERS,IPPROTO_TCP)) == 0)
	{
#endif /* ALONE */
		cerr << "unable to retrieve tcp port number" << endl;
		exit(1);
	}

/* End of added code */

	svc_run();
	cerr << "svc_run returned" << endl;
	exit(1);
}

static void
db_setupprog_1(struct svc_req *rqstp,SVCXPRT *transp)
{
	union {
		arr1 db_getres_1_arg;
		nam db_getdev_1_arg;
		tab_dbdev db_devexp_1_arg;
		tab_dbdev_2 db_devexp_2_arg;
		tab_dbdev_3 db_devexp_3_arg;
		arr1 db_devimp_1_arg;
		nam db_svcunr_1_arg;
		nam db_svcchk_1_arg;
		nam db_getdevexp_1_arg;
		int fill;
		tab_putres db_putres_1_arg;
		arr1 db_delres_1_arg;
		nam db_cmd_query_1_arg;
		psdev_reg_x db_psdev_reg_1_arg;
		arr1 db_psdev_unreg_1_arg;
		nam db_event_query_1_arg;
	} argument;
	char *result;
	xdrproc_t xdr_argument, xdr_result;
	DbRpcLocalFunc local;
	
/* Added variables */

	int i,j,num,num_dev;
	int pid;

	switch (rqstp->rq_proc)
	{
	case NULLPROC:
		svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
		return;

	case DB_GETRES:
		xdr_argument = (xdrproc_t)xdr_arr1;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) db_getres_1_svc;
		break;

	case DB_GETDEV:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) db_getdev_1_svc;
		break;

	case DB_DEVEXP:
		switch(rqstp->rq_vers)
		{
		case DB_SETUPVERS :
			xdr_argument = (xdrproc_t)xdr_tab_dbdev;
			xdr_result = (xdrproc_t)xdr_int;
			local = (DbRpcLocalFunc) db_devexp_1_svc;
			break;

		case DB_VERS_2 :
			xdr_argument = (xdrproc_t)xdr_tab_dbdev_2;
			xdr_result = (xdrproc_t)xdr_int;
			local = (DbRpcLocalFunc) db_devexp_2_svc;
			break;

		case DB_VERS_3 :
			xdr_argument = (xdrproc_t)xdr_tab_dbdev_3;
			xdr_result = (xdrproc_t)xdr_int;
			local = (DbRpcLocalFunc) db_devexp_3_svc;
			break;
		}
		break;

	case DB_DEVIMP:
		xdr_argument = (xdrproc_t)xdr_arr1;
		xdr_result = (xdrproc_t)xdr_db_resimp;
		local = (DbRpcLocalFunc) db_devimp_1_svc;
		break;

	case DB_SVCUNR:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_int;
		local = (DbRpcLocalFunc) db_svcunr_1_svc;
		break;

	case DB_SVCCHK:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_svc_inf;
		local = (DbRpcLocalFunc) db_svcchk_1_svc;
		break;

	case DB_GETDEVEXP:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) db_getdevexp_1_svc;
		break;

	case DB_CLODB:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_int;
		local = (DbRpcLocalFunc) db_clodb_1_svc;
		break;

	case DB_REOPENDB:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_int;
		local = (DbRpcLocalFunc) db_reopendb_1_svc;
		break;

	case DB_PUTRES:
		xdr_argument = (xdrproc_t)xdr_tab_putres;
		xdr_result = (xdrproc_t)xdr_int;
		local = (DbRpcLocalFunc) db_putres_1_svc;
		break;

	case DB_DELRES:
		xdr_argument = (xdrproc_t)xdr_arr1;
		xdr_result = (xdrproc_t)xdr_int;
		local = (DbRpcLocalFunc) db_delres_1_svc;
		break;

	case DB_CMDQUERY:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_cmd_que;
		local = (DbRpcLocalFunc) db_cmd_query_1_svc;
		break;
		/* event query */
	case DB_EVENTQUERY:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_event_que;
		local = (DbRpcLocalFunc) db_event_query_1_svc;
		break;

	case DB_PSDEV_REG:
		xdr_argument = (xdrproc_t)xdr_psdev_reg_x;
		xdr_result = (xdrproc_t)xdr_db_psdev_error;
		local = (DbRpcLocalFunc) db_psdev_reg_1_svc;
		break;

	case DB_PSDEV_UNREG:
		xdr_argument = (xdrproc_t)xdr_arr1;
		xdr_result = (xdrproc_t)xdr_db_psdev_error;
		local = (DbRpcLocalFunc) db_psdev_unreg_1_svc;
		break;
		
	case DB_GETDEVDOMAIN:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devdomainlist_1_svc;
		break;
		
	case DB_GETDEVFAMILY:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devfamilylist_1_svc;
		break;
		
	case DB_GETDEVMEMBER:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devmemberlist_1_svc;
		break;
		
	case DB_GETRESDOMAIN:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) resdomainlist_1_svc;
		break;
		
	case DB_GETRESFAMILY:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) resfamilylist_1_svc;
		break;

	case DB_GETRESMEMBER:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) resmemberlist_1_svc;
		break;
		
	case DB_GETRESRESO:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) resresolist_1_svc;
		break;
		
	case DB_GETRESRESOVAL:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) resresoval_1_svc;
		break;
		
	case DB_GETSERVER:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devserverlist_1_svc;
		break;
		
	case DB_GETPERS:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devpersnamelist_1_svc;
		break;
		
	case DB_GETHOST:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) hostlist_1_svc;
		break;
		
	case DB_DEVINFO:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_devinfo_svc;
		local = (DbRpcLocalFunc) devinfo_1_svc;
		break;
		
	case DB_DEVRES:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) devres_1_svc;
		break;

	case DB_DEVDEL:
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_long;
		local = (DbRpcLocalFunc) devdel_1_svc;
		break;

	case DB_DEVDELALLRES:
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_psdev_error;
		local = (DbRpcLocalFunc) devdelres_1_svc;
		break;

	case DB_INFO:
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_info_svc;
		local = (DbRpcLocalFunc) info_1_svc;
		break;

	case DB_SVCUNREG :
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_long;
		local = (DbRpcLocalFunc) unreg_1_svc;
		break;

	case DB_SVCINFO :
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_svcinfo_svc;
		local = (DbRpcLocalFunc) svcinfo_1_svc;
		break;
		
	case DB_SVCDELETE :
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_long;
		local = (DbRpcLocalFunc) svcdelete_1_svc;
		break;

	case DB_UPDDEV :
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_psdev_error;
		local = (DbRpcLocalFunc) upddev_1_svc;
		break;

	case DB_UPDRES :
		xdr_argument = (xdrproc_t)xdr_db_res;
		xdr_result = (xdrproc_t)xdr_db_psdev_error;
		local = (DbRpcLocalFunc) updres_1_svc;
		break;
		
	case DB_SECPASS :
		xdr_argument = (xdrproc_t)xdr_void;
		xdr_result = (xdrproc_t)xdr_db_res;
		local = (DbRpcLocalFunc) secpass_1_svc;
		break;
		
	case DB_GETPOLLER :
		xdr_argument = (xdrproc_t)xdr_nam;
		xdr_result = (xdrproc_t)xdr_db_poller_svc;
		local = (DbRpcLocalFunc) getpoller_1_svc;
		break;
																					
#ifndef ALONE
	case RPC_QUIT_SERVER:
		svc_sendreply(transp,(xdrproc_t)xdr_void,NULL);
		pid = getpid();
		kill(pid,SIGQUIT);
		return;
#endif /* ALONE */
		
	default:
		svcerr_noproc(transp);
		return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, xdr_argument, (caddr_t)&argument))
	{
		svcerr_decode(transp);
		return;
	}
	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, result))
	{
		svcerr_systemerr(transp);
	}
	if (!svc_freeargs(transp, xdr_argument, (caddr_t)&argument))
	{
		cerr << "unable to free arguments" << endl;
		exit(1);
	}
	
/* Added code to free memory allocated in server function */

	switch (rqstp->rq_proc)
	{
	case DB_GETRES:
		num = send_back.res_val.arr1_len;
		for (i = 0;i < num;i++)
			free(send_back.res_val.arr1_val[i]);
		if (send_back.res_val.arr1_val !=  NULL)
			free(send_back.res_val.arr1_val);
		break;

	case DB_GETDEV:
		num = dev_back.res_val.arr1_len;
		for (i = 0;i < num;i++)
			free(dev_back.res_val.arr1_val[i]);
		if (dev_back.res_val.arr1_val != NULL)
			free(dev_back.res_val.arr1_val);
		break;
	case DB_DEVIMP:
		num = back.imp_dev.tab_dbdev_len;
		for(i = 0;i < num;i++)
		{
			free(back.imp_dev.tab_dbdev_val[i].dev_name);
			free(back.imp_dev.tab_dbdev_val[i].host_name);
			free(back.imp_dev.tab_dbdev_val[i].dev_type);
			free(back.imp_dev.tab_dbdev_val[i].dev_class);
		}
		if (back.imp_dev.tab_dbdev_val != NULL)
			free(back.imp_dev.tab_dbdev_val);
		break;
	case DB_GETDEVEXP:
		num = dev_back.res_val.arr1_len;
		for (i = 0;i < num;i++)
			free(dev_back.res_val.arr1_val[i]);
		if (dev_back.db_err == 0)
			free(dev_back.res_val.arr1_val);
		break;
		
	case DB_GETDEVDOMAIN:
	case DB_GETDEVFAMILY:
	case DB_GETDEVMEMBER:
	case DB_GETRESDOMAIN:
	case DB_GETRESFAMILY:
	case DB_GETRESMEMBER:
	case DB_GETRESRESO:
	case DB_GETRESRESOVAL:
	case DB_GETSERVER:
	case DB_GETPERS:
	case DB_GETHOST:
	case DB_SECPASS:
		if (browse_back.db_err == 0)
		{
			num = browse_back.res_val.arr1_len;
			for (i = 0;i < num;i++)
				delete [] browse_back.res_val.arr1_val[i];
			delete [] browse_back.res_val.arr1_val;
		}
		break;
		
	case DB_DEVINFO:
		delete [] sent_back.device_class;
		delete [] sent_back.server_name;
		delete [] sent_back.personal_name;
		delete [] sent_back.process_name;
		delete [] sent_back.host_name;
		break;
		
	case DB_DEVRES:
		if (res_back.db_err == 0)
		{
			num = res_back.res_val.arr1_len;
			for (i = 0;i < num;i++)
				delete [] res_back.res_val.arr1_val[i];
			delete [] res_back.res_val.arr1_val;
		}
		res_list_dev.clear();
		break;
		
	case DB_INFO:
		if (info_back.dev.dom_val != 0)
		{
			dom_list.clear();
			delete [] info_back.dev.dom_val;
		}
		if (info_back.res.dom_val != 0)
		{
			res_list.clear();
			delete [] info_back.res.dom_val;
		}
		break;
		
	case DB_SVCINFO:
		delete [] svcinfo_back.process_name;
		delete [] svcinfo_back.host_name;
		if (svcinfo_back.embedded_val != NULL)
		{
			num = svcinfo_back.embedded_len;
			for (i = 0;i < num;i++)
			{
				delete [] svcinfo_back.embedded_val[i].server_name;
				if (svcinfo_back.embedded_val[i].dev_val != NULL)
				{
					num_dev = svcinfo_back.embedded_val[i].dev_len;
					for (j = 0;j < num_dev;j++)
						delete [] svcinfo_back.embedded_val[i].dev_val[j].name;
					delete [] svcinfo_back.embedded_val[i].dev_val;
				}
			}
			delete [] svcinfo_back.embedded_val;
		}
		break;
		
	case DB_GETPOLLER:
		delete [] poll_back.server_name;
		delete [] poll_back.personal_name;
		delete [] poll_back.process_name;
		delete [] poll_back.host_name;
		break;
	}
}



/****************************************************************************
*                                                                           *
*		Code for register_db function                               *
*                        -----------                                        *
*                                                                           *
*    Function rule : To send server information (host_name,program number   *
*                    and version number) to the network manager             *
*                                                                           *
*    Argin : - The system host_name where the network manager is running    *
*            - The system host_name where the server is running             *
*            - The server program number                                    *
*            - The server version number                                    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function is used only when the database is a part of the device   *
*    server system                                                          *
*                                                                           *
****************************************************************************/

#ifndef ALONE
static void register_db(char *netman_host,char *host,u_long prog,u_long vers)
{
	_register_data register_data;
	CLIENT *netman_clnt;
	enum clnt_stat clnt_stat;
	static int res;

/* Create an RPC connection to network manager */

	netman_clnt = clnt_create(netman_host,NMSERVER_PROG,NMSERVER_VERS,"udp");
	if (netman_clnt == NULL)
	{
		cerr << "Unable to create connection to network manager" << endl;
		exit(1);
	}

	clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&retry_timeout);
	clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&timeout);

/* Send informations to network manager */

	register_data.host_name = host;
	register_data.prog_number = prog;
	register_data.vers_number = vers;

	clnt_stat = clnt_call(netman_clnt,RPC_DB_REGISTER,(xdrproc_t)xdr__register_data,
			(caddr_t)&register_data,(xdrproc_t)xdr_int,(caddr_t)&res,timeout);

	if (clnt_stat != RPC_SUCCESS)
	{
		cerr << "register_db failed !!!" << endl;
		exit(1);
	}
		
/* Exit function */

	clnt_destroy(netman_clnt);

}
#endif /* ALONE */
