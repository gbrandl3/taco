#ifdef sun
#define PORTMAP
#endif

#include "config.h"

#include <dbClass.h>
#ifdef HAVE_MYSQL_MYSQL_H
#include <MySqlServer.h>
#endif
#include <NdbmServer.h>

#include <rpc/clnt.h>            /* for get_myaddress() */
//
// RPC function not defined within rpc include files !!
// M. Diehl, 15.11.99
// Not correct for GLIBC 2.x: get_myaddress() is defined
// and gettransient() is one of our private functions!
// Can we drop this by including DevServer.h?
//

extern "C" 
{
    u_long gettransient(const char *);
#if !defined( FreeBSD)
    void get_myaddress(struct sockaddr_in *);
#endif
//  int gethostname(char *,size_t);
}

typedef char * (* DbRpcLocalFunc)(...);
//
// Local function 
//
static void register_db(const string, const string, const u_long, const u_long);
static void db_setupprog_1(struct svc_req *, SVCXPRT *);

extern	DBServer	*dbm;
static void un_register_prog(int signo)
{
#ifdef ALONE
    pmap_unset(DB_SETUPPROG, DB_SETUPVERS);
    pmap_unset(DB_SETUPPROG, DB_VERS_2);
    pmap_unset(DB_SETUPPROG, DB_VERS_3);
#else
    pmap_unset(dbm->getPgNum(), DB_SETUPVERS);
    pmap_unset(dbm->getPgNum(), DB_VERS_2);
    pmap_unset(dbm->getPgNum(), DB_VERS_3);
#endif /* ALONE */
//
// Added code to close database 
//
    delete dbm;
    exit(1);
}

/* Default signal handler for strange UNIX signal which default action is to
   kill the process */

void default_sig(int signo)
{
    time_t 	tps;
    char 	*tps_str;
    struct tm 	*time_tm;

    tps = time((time_t *)0);
    time_tm = localtime(&tps);
    tps_str = asctime(time_tm);
    tps_str[24] = '\0';
    cout << tps_str << " : signal " << signo << " received !!!" << endl;
}

void usage(const char *argv)
{
    string	types("dbm");
#if HAVE_MYSQL_MYSQL_H
    types += "|mysql";
#endif
    cerr << "usage: " << argv << " [-h] [-t (" << types << ")] database_name network_manager_host_name" << endl;
    cerr << "       -h             - prints this message" << endl;
    cerr << "       -t (dbm|mysql) - gives the type of underlying database connect to" << endl;
    cerr << "                        'dbm' stands for the dbm, ndbm, and gdbm" << endl;
    cerr << "                        'mysql' stands for the MySQL database" << endl;
    cerr << "                        database_name for MySQL database should be tango" << endl;
    cerr << "       -u user        - user for MySQL database" << endl;
    cerr << "       -p password    - password for MySQL database" << endl;
    exit(1);
}


/****************************************************************************
 *                                                                          *
 *									    *
 *              The MySQL static database server main function               *
 *		---------------------------------------------		    *
 *									    *
 *                                                                          *
 ****************************************************************************/
int main(int argc,char **argv)
{
    SVCXPRT 		*transp_udp,
    			*transp_tcp;
//
// Added definition for transient program number 
//
    u_long 		pgnum;
    u_short 		udp_port,
			tcp_port;
    struct sockaddr_in 	so;
    int 		i,
			j;
    char 		*ptr,
    			hostna[32];
    struct sigaction 	sighand;
#ifdef sun
    struct hostent 	*host;
    unsigned long 	*ptmp_long;
#endif
    char		*mysql_user="root";
    char		*mysql_password="";
 
		
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
//
// Open database tables according to the definition 
//
#ifndef ALONE
    int c;
    extern int	optint;
    extern char	*optarg;

    while ((c = getopt(argc, argv, "t:h:u:p")) != EOF)
    	switch(c)
	{
	    case 't' :	if (string(optarg) == "dbm")
			    dbm = new NdbmServer("", "", "");
#ifdef HAVE_MYSQL_MYSQL_H
			else if (string(optarg) == "mysql")
			{
			    if (getenv("MYSQL_USER") != NULL)
			    {
				mysql_user = getenv("MYSQL_USER");
			    }
			    if (getenv("MYSQL_PASSWORD") != NULL)
			    {
				mysql_password = getenv("MYSQL_PASSWORD");
			    }
			    cout << "going to connect to mysql database with user = " << mysql_user;
			    cout << ", password = " << mysql_password << endl;
    			    dbm = new MySQLServer(mysql_user, mysql_password, argv[optind]);
			}
#endif
			else
			    usage(*argv);
			break;
	    case 'h' :	usage(*argv);
			break;
    	}

    if (optind != (argc - 2))
	usage(*argv); 			
    string netmanhost(argv[argc - 1]);
#endif
    if (!dbm)
    	dbm = new NdbmServer("", "", "");
//
// RPC business !!!!!!!!!!!!!!!!!!!! 
//
#ifdef ALONE
    transp_udp = svcudp_create(RPC_ANYSOCK);
    transp_tcp = svctcp_create(RPC_ANYSOCK,0,0);
    gethostname(hostna, sizeof(hostna));
    cout << "Server host name : " << hostna << endl;
#else
//
// Added code to manage transient program number and to get host name
//
// M. Diehl, 15.11.99
// Use new gettransient() interface. Use some program description
// to create the hash value. This will prevent conflicts with
// message server. When both programms try to register numbers
// beginning at a common base leading to a race condition!
//
    if ((pgnum = gettransient("DatabaseServer")) == 0)
    {
	cerr << "dbm_server : Can't get transcient program number" << endl;
	exit(-1);
    }
    cout << "Program number : " << pgnum << endl;
    gethostname(hostna, sizeof(hostna));
    cout << "Server host name : " << hostna << endl;
//
// Send these informations to network manager. Even if the server is now 
// supporting version one and two, register it in the Manager with version 1
// for compatibility with old release of device server. */
//
    register_db((char *)netmanhost.c_str(),hostna,pgnum,DB_SETUPVERS);
//
// M. Diehl, 15.11.99
// Since gettransient() does not bind sockets and pmap_set
// prognums anymore, the patches required for Solaris and
// Linux/glibc 2.x (and probably for every other well-behaving
// system) have been removed.
//
    transp_udp = svcudp_create(RPC_ANYSOCK);
    transp_tcp = svctcp_create(RPC_ANYSOCK,0,0);
#endif 

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
//
// Register the server in TCP and UDP for version 1,2 and 3
//
    typedef void (* MyRpcFuncPtr)(struct svc_req *, SVCXPRT *);
    MyRpcFuncPtr setup_prog;
    setup_prog = db_setupprog_1; 
#ifdef ALONE
    if (!svc_register(transp_udp,DB_SETUPPROG,DB_SETUPVERS,setup_prog,IPPROTO_UDP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_SETUPVERS,udp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_tcp,DB_SETUPPROG,DB_SETUPVERS,setup_prog,IPPROTO_TCP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_SETUPVERS,tcp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_2,setup_prog,IPPROTO_UDP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_2,setup_prog,IPPROTO_TCP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_3,setup_prog,IPPROTO_UDP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_3, setup_prog,IPPROTO_TCP))
    {
	cerr << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << endl;
	exit(1);
    }

#else
    if (!svc_register(transp_udp,pgnum, DB_SETUPVERS, setup_prog, IPPROTO_UDP))
    {
 	cerr << "unable to register (pgnum, DB_SETUPVERS, udp)" << endl;
 	exit(1);
    }
    if (!svc_register(transp_tcp,pgnum,DB_SETUPVERS, setup_prog,IPPROTO_TCP))
    {
 	cerr << "unable to register (pgnum,DB_SETUPVERS,tcp)" << endl;
 	exit(1);
    }
    if (!svc_register(transp_udp,pgnum, DB_VERS_2, setup_prog, IPPROTO_UDP))
    {
	cerr << "unable to register (pgnum, DB_VERS_2, udp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_tcp,pgnum,DB_VERS_2,setup_prog,IPPROTO_TCP))
    {
	cerr << "unable to register (pgnum,DB_VERS_2,tcp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_udp,pgnum, DB_VERS_3, setup_prog, IPPROTO_UDP))
    {
	cerr << "unable to register (pgnum, DB_VERS_2, udp)" << endl;
	exit(1);
    }
    if (!svc_register(transp_tcp,pgnum,DB_VERS_3,setup_prog,IPPROTO_TCP))
    {
	cerr << "unable to register (pgnum,DB_VERS_2,tcp)" << endl;
	exit(1);
    }
#endif 
//
// Added code to ask portmapper for the port numbers 
//
    get_myaddress(&so);
#ifdef sun
    if ((host = gethostbyname(hostna)) == NULL)
    {
	cerr << "Unable to get my IP address" << endl;
	exit(1);
    }
    ptmp_long = (unsigned long *)host->h_addr_list[0];
    so.sin_addr.S_un.S_addr = *ptmp_long;
#endif 

#ifdef ALONE
    if ((udp_port = pmap_getport(&so,DB_SETUPPROG,DB_SETUPVERS,IPPROTO_UDP)) == 0)
#else
    if ((udp_port = pmap_getport(&so,pgnum,DB_SETUPVERS,IPPROTO_UDP)) == 0)
#endif 
    {
	cerr << "unable to retrieve udp port number" << endl;
	exit(1);
    }

#ifdef ALONE
    if ((tcp_port = pmap_getport(&so,DB_SETUPPROG,DB_SETUPVERS,IPPROTO_TCP)) == 0)
#else
    if ((tcp_port = pmap_getport(&so,pgnum,DB_SETUPVERS,IPPROTO_TCP)) == 0)
#endif 
    {
	cerr << "unable to retrieve tcp port number" << endl;
	exit(1);
    }
    svc_run();
    cerr << "svc_run returned" << endl;
    exit(1);
}


static void db_setupprog_1(struct svc_req *rqstp, SVCXPRT *transp)
{
    union 
    {
	arr1 		db_getres_1_arg;
	arr1 		db_devimp_1_arg;
	arr1 		db_delres_1_arg;
	arr1 		db_psdev_unreg_1_arg;
	tab_dbdev 	db_devexp_1_arg;
	tab_dbdev_2 	db_devexp_2_arg;
	tab_dbdev_3 	db_devexp_3_arg;
	nam 		db_getdev_1_arg;
	nam 		db_svcunr_1_arg;
	nam 		db_svcchk_1_arg;
	nam 		db_getdevexp_1_arg;
	nam 		db_event_query_1_arg;
	nam 		db_cmd_query_1_arg;
	int	 	fill;
	tab_putres 	db_putres_1_arg;
	psdev_reg_x 	db_psdev_reg_1_arg;
    } argument;


    xdrproc_t 		xdr_argument, 
			xdr_result;
    DbRpcLocalFunc 	local;
//
// Added variables 
//
    int 		i, j, num, num_dev,
			pid;
/*
    union
    {
    	db_resimp	*back;
	db_res          *browse_back;
	db_devinfo_svc  *sent_back;
    } return_value;
*/
    void 		*result;

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
#endif 

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
    if (result != NULL && !svc_sendreply(transp, xdr_result, reinterpret_cast<char *>(result)))
	svcerr_systemerr(transp);
    if (!svc_freeargs(transp, xdr_argument, (caddr_t)&argument))
    {
	cerr << "unable to free arguments" << endl;
	exit(1);
    }
//
// Added code to free memory allocated in server function 
//
    switch (rqstp->rq_proc)
    {
	case DB_DEVIMP: 	num = reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_len;
				for(int i = 0;i < num;i++)
				{
				    delete [] reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val[i].dev_name;
				    delete [] reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val[i].host_name;
				    delete [] reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val[i].dev_type;
				    delete [] reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val[i].dev_class;
				}
				if (reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val != NULL)
				    delete [] reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_val;
				break;
	case DB_GETDEV: 	
	case DB_GETDEVEXP: 	
	case DB_GETRES: 	
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
	case DB_DEVRES: 	
#warning Reslistdev must be implemented res_list_dev.clear();
	case DB_SECPASS: 	if (reinterpret_cast<db_res *>(result)->db_err == 0)
				{
				    num = reinterpret_cast<db_res *>(result)->res_val.arr1_len;
				    for (int i = 0; i < num;i++)
					delete [] reinterpret_cast<db_res *>(result)->res_val.arr1_val[i];
				    delete [] reinterpret_cast<db_res *>(result)->res_val.arr1_val;
				}
				break;
	case DB_DEVINFO: 	delete [] reinterpret_cast<db_devinfo_svc *>(result)->device_class;
				delete [] reinterpret_cast<db_devinfo_svc *>(result)->server_name;
				delete [] reinterpret_cast<db_devinfo_svc *>(result)->personal_name;
				delete [] reinterpret_cast<db_devinfo_svc *>(result)->process_name;
				delete [] reinterpret_cast<db_devinfo_svc *>(result)->host_name;
				break;
	case DB_INFO: 		if (reinterpret_cast<db_info_svc *>(result)->dev.dom_val != 0)
				{
#warning Domain list must be implemented
//				    dom_list.clear();
				    delete [] reinterpret_cast<db_info_svc *>(result)->dev.dom_val;
				}
				if (reinterpret_cast<db_info_svc *>(result)->res.dom_val != 0)
				{
#warning Resource list must be implemented
//				    res_list.clear();
				    delete [] reinterpret_cast<db_info_svc *>(result)->res.dom_val;
				}
				break;
		
	case DB_SVCINFO: 	delete [] reinterpret_cast<svcinfo_svc *>(result)->process_name;
				delete [] reinterpret_cast<svcinfo_svc *>(result)->host_name;
				if (reinterpret_cast<svcinfo_svc *>(result)->embedded_val != NULL)
				{
				    num = reinterpret_cast<svcinfo_svc *>(result)->embedded_len;
				    for (int i = 0; i < num; i++)
				    {
					delete [] reinterpret_cast<svcinfo_svc *>(result)->embedded_val[i].server_name;
					if (reinterpret_cast<svcinfo_svc *>(result)->embedded_val[i].dev_val != NULL)
					{
					    num_dev = reinterpret_cast<svcinfo_svc *>(result)->embedded_val[i].dev_len;
					    for (j = 0;j < num_dev;j++)
						delete [] reinterpret_cast<svcinfo_svc *>(result)->embedded_val[i].dev_val[j].name;
					    delete [] reinterpret_cast<svcinfo_svc *>(result)->embedded_val[i].dev_val;
					}
				    }
				    delete [] reinterpret_cast<svcinfo_svc *>(result)->embedded_val;
				}
				break;
		
	case DB_GETPOLLER: 	delete [] reinterpret_cast<db_poller_svc *>(result)->server_name;
				delete [] reinterpret_cast<db_poller_svc *>(result)->personal_name;
				delete [] reinterpret_cast<db_poller_svc *>(result)->process_name;
				delete [] reinterpret_cast<db_poller_svc *>(result)->host_name;
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
static void register_db(const string netman_host,const string host, const u_long prog, const u_long vers)
{
    _register_data register_data;
    CLIENT *netman_clnt;
    enum clnt_stat clnt_stat;
    static int res;
//
// Create an RPC connection to network manager 
//
    netman_clnt = clnt_create(netman_host.c_str(), NMSERVER_PROG, NMSERVER_VERS, "udp");
    if (netman_clnt == NULL)
    {
	cerr << "Unable to create connection to network manager." << endl;
	exit(1);
    }

    clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&retry_timeout);
    clnt_control(netman_clnt,CLSET_TIMEOUT,(char *)&timeout);
//
// Send informations to network manager 
//
    register_data.host_name = const_cast<char *>(host.c_str());
    register_data.prog_number = prog;
    register_data.vers_number = vers;

    clnt_stat = clnt_call(netman_clnt,RPC_DB_REGISTER,(xdrproc_t)xdr__register_data,
			(caddr_t)&register_data,(xdrproc_t)xdr_int,(caddr_t)&res,timeout);

    if (clnt_stat != RPC_SUCCESS)
    {
	cerr << "register_db failed !!!" << endl;
	exit(1);
    }
//
// Exit function 
//
    clnt_destroy(netman_clnt);
}
#endif 
