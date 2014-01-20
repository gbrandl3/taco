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
 * File:
 *
 * Description:
 *
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.29 $
 *
 * Date:	$Date: 2008-04-06 09:07:41 $
 *
 */

#ifdef sun
#define PORTMAP
#endif
#if HAVE_CONFIG_H
#	include "config.h"
#endif

#ifdef DARWIN
#	include <rpc/types.h>
#	include <netinet/in.h>
#	include <rpc/xdr.h>
#	include <rpc/auth.h>
#	include <rpc/clnt.h>
#	if HAVE_SVC_H 
# 		include <svc.h>
#	elif !HAVE_RPC_SVC_H
#		include <rpc/svc.h>
#	else
#		include "svc.h"
#	endif
#endif

#include "dbClass.h"
#ifdef USE_MYSQL
#	include "MySqlServer.h"
#endif
#ifdef USE_GDBM
#	include "NdbmServer.h"
#endif
#ifdef USE_SQLITE3
#	include "Sqlite3Server.h"
#endif
#if HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#elif HAVE_RPC_H
#	include <rpc.h>
#endif
#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif

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
#if !HAVE_DECL_GET_MYADDRESS
	void get_myaddress(struct sockaddr_in *);
#endif
	int taco_gethostname(char *,size_t);
}

typedef char *(* DbRpcLocalFunc)(...);
//
// Local function 
//
static void register_db(const std::string, const std::string, const u_long, const u_long);
static void db_setupprog_1(struct svc_req *, SVCXPRT *);

extern	DBServer	*dbm;

log4cpp::Category       *logStream;

static void un_register_prog(int signo)
{
	if (signo == SIGHUP)
		return;

	logStream->noticeStream() << "signal " << signo << " received." << log4cpp::eol;
	logStream->noticeStream() << "unregister database server." << log4cpp::eol;
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
	logStream->noticeStream() << "close connection to database." << log4cpp::eol;
	delete dbm;
	logStream->noticeStream() << "exit server." << log4cpp::eol;
	exit(1);
}
//
// Default signal handler for strange UNIX signal which default action is to kill the process 
//
void default_sig(int signo)
{
	logStream->noticeStream() << "signal " << signo << " received! (ignored)." << log4cpp::eol;
}

void usage(const char *argv)
{
	std::string	types("");
#ifdef USE_GDBM
	types += "dbm";
#endif
#ifdef USE_SQLITE3
	if (!types.empty())
		types += '|';
	types += "sqlite3";
#endif
#ifdef USE_MYSQL
	if (!types.empty())
		types += '|';
	types += "mysql";
#endif
	std::cerr << "usage: " << argv << " [-h] [-v] [-t " << types << "] "
#ifdef USE_MYSQL
		<< "[-u user] [-p password] " 
#endif
		<< "database_name network_manager_host_name" << std::endl;
	std::cerr << "       -h             - prints this message" << std::endl;
	std::cerr << "       -v             - prints the version and exits" << std::endl;
	std::cerr << "       -t " << types << " - gives the type of underlying database connect to" << std::endl;
#ifdef USE_GDBM
	std::cerr << "                        'dbm' stands for the dbm, ndbm, and gdbm" << std::endl;
#endif
#ifdef USE_SQLITE3
	std::cerr << "                        'sqlite3' stands for the SQLite3 database engine" << std::endl;
#endif
#ifdef USE_MYSQL
	std::cerr << "                        'mysql' stands for the MySQL database" << std::endl;
#endif
#if USE_MYSQL || USE_SQLITE3
	std::cerr << "                        database_name for "
#if USE_MYSQL
	"'MySQL' " 
#endif
#if USE_SQLITE3
	"'SQLite3' "
#endif
	"database should be 'tango'" << std::endl;
#endif
#ifdef USE_MYSQL
	std::cerr << "       -u user        - user for MySQL database" << std::endl;
	std::cerr << "       -p password    - password for MySQL database" << std::endl;
#endif
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
	SVCXPRT			*transp_udp,
				*transp_tcp;
//
// Added definition for transient program number 
//
	u_long 			pgnum;
	u_short 		udp_port,
				tcp_port;
	int 			i,
				j;
	char 			*ptr,
    				hostna[HOST_NAME_LENGTH];
	struct sigaction 	sighand;
	struct sockaddr_in 	so;
#ifdef sun
	struct hostent 		*host;
	unsigned long 		*ptmp_long;
#endif
	const char		*mysql_user="root";
	const char		*mysql_password="";
 
	const char 		*logpath = getenv("LOGCONFIG");

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
	logStream = &log4cpp::Category::getInstance("taco.system.Database");
	logStream->noticeStream() << "using " << logpath << " configuration file" << log4cpp::eol;
		
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
	int 		c;
	extern int	optint;
	extern char	*optarg;

	for (int i = 0; i< argc; i++) 
		logStream->debugStream() << "argv[" << i << "] = " << argv[i] << log4cpp::eol;

#ifdef USE_MYSQL
	if (getenv("MYSQL_USER") != NULL)
		mysql_user = getenv("MYSQL_USER");
	if (getenv("MYSQL_PASSWORD") != NULL)
		mysql_password = getenv("MYSQL_PASSWORD");
#endif

	std::string database_type;
	while ((c = getopt(argc, argv, "t:h:lp:u:v")) != EOF)
		switch(c)
		{
			case 'v' : 
				std::cout << argv[0] << " version " << VERSION << std::endl;
				exit(0);
			case 't' :	
				database_type = optarg;
				break;
#ifdef USE_MYSQL
			case 'u' :
				mysql_user = optarg;
				break;
			case 'p' :
				mysql_password = optarg;
				break;
#endif
			case '?' :
			case 'h' :	
			default  :
				usage(*argv);
				break;
		}

	if (optind != (argc - 2))
		usage(*argv); 			
	std::string 	netmanhost(argv[argc - 1]);
#endif

#ifdef USE_GDBM
	if (database_type == "dbm")
		dbm = new NdbmServer("", "", "");
	else
#endif
#ifdef USE_MYSQL
	if (database_type == "mysql")
	{
		logStream->debugStream() << "going to connect to mysql database with user = " << mysql_user
			<< ", password = " << mysql_password << log4cpp::eol;

		dbm = new MySQLServer(mysql_user, mysql_password, argv[optind]);
	}
	else
#endif
#ifdef USE_SQLITE3
	if (database_type == "sqlite3")
	{
		dbm = new SQLite3Server(argv[optind]);
	}
	else
#endif
		usage(*argv);

//	dbm->setLogstream(logStream);
//
// RPC business !!!!!!!!!!!!!!!!!!!! 
//
#ifdef ALONE
	transp_udp = svcudp_create(RPC_ANYSOCK);
	transp_tcp = svctcp_create(RPC_ANYSOCK,0,0);
	taco_gethostname(hostna, sizeof(hostna));
	std::cout << "Server host name : " << hostna << endl;
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
		logStream->fatalStream() << "Can't get transcient program number" << log4cpp::eol;
		exit(-1);
	}
	dbm->setPgNum(pgnum);

	logStream->noticeStream() << "Version : " << VERSION << log4cpp::eol;
	logStream->noticeStream() << "Program number : " << pgnum << log4cpp::eol;

	taco_gethostname(hostna, sizeof(hostna));
	logStream->noticeStream() << "Server host name : " << hostna << log4cpp::eol;

//
// Send these informations to network manager. Even if the server is now 
// supporting version one and two, register it in the Manager with version 1
// for compatibility with old release of device server. 
//
	register_db((char *)netmanhost.c_str(), hostna, pgnum, DB_SETUPVERS);
	logStream->noticeStream() << "registered on host : " << netmanhost << log4cpp::eol;
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
		logStream->fatalStream() << "cannot create udp service." << log4cpp::eol;
		exit(1);
	}
	if (transp_tcp == NULL)
	{
		logStream->fatalStream() << "cannot create tcp service." << log4cpp::eol;
		exit(1);
	}
//
// Register the server in TCP and UDP for version 1,2 and 3
//
#ifdef DARWIN
        typedef void (* MyRpcFuncPtr)();
#else
	typedef void (* MyRpcFuncPtr)(struct svc_req *, SVCXPRT *);
#endif
	MyRpcFuncPtr setup_prog;
        setup_prog = (MyRpcFuncPtr)db_setupprog_1;

#ifdef ALONE
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_SETUPVERS,setup_prog,IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_SETUPVERS,udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_SETUPVERS,setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_SETUPVERS,tcp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_2,setup_prog,IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_2,setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_udp,DB_SETUPPROG,DB_VERS_3,setup_prog,IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_VERS_2,udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,DB_SETUPPROG,DB_VERS_3, setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (DB_SETUPPROG,DB_VERS_2,tcp)" << log4cpp::eol;
		exit(1);
	}

#else
	if (!svc_register(transp_udp,pgnum, DB_SETUPVERS, setup_prog, IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ", DB_SETUPVERS, udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,pgnum,DB_SETUPVERS, setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ",DB_SETUPVERS,tcp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_udp,pgnum, DB_VERS_2, setup_prog, IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ", DB_VERS_2, udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,pgnum,DB_VERS_2,setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ",DB_VERS_2,tcp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_udp,pgnum, DB_VERS_3, setup_prog, IPPROTO_UDP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ", DB_VERS_2, udp)" << log4cpp::eol;
		exit(1);
	}
	if (!svc_register(transp_tcp,pgnum,DB_VERS_3,setup_prog,IPPROTO_TCP))
	{
		logStream->fatalStream() << "unable to register (" << pgnum << ",DB_VERS_2,tcp)" << log4cpp::eol;
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
		logStream->fatalStream() << "Unable to get my IP address" << log4cpp::eol;
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
		logStream->fatalStream() << "unable to retrieve udp port number" << log4cpp::eol;
		exit(1);
	}

#ifdef ALONE
	if ((tcp_port = pmap_getport(&so,DB_SETUPPROG,DB_SETUPVERS,IPPROTO_TCP)) == 0)
#else
	if ((tcp_port = pmap_getport(&so,pgnum,DB_SETUPVERS,IPPROTO_TCP)) == 0)
#endif 
	{
		logStream->fatalStream() << "unable to retrieve tcp port number" << log4cpp::eol;
		exit(1);
	}
	dbm->setUDPPort(udp_port);
	dbm->setTCPPort(tcp_port);
	
	logStream->noticeStream() << "ready to run" << log4cpp::eol;
	svc_run();
	logStream->fatalStream() << "svc_run returned" << log4cpp::eol;
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
		DevString 	db_getdev_1_arg;
		DevString 	db_svcunr_1_arg;
		DevString 	db_svcchk_1_arg;
		DevString 	db_getdevexp_1_arg;
		DevString 	db_event_query_1_arg;
		DevString 	db_cmd_query_1_arg;
		int	 	fill;
		tab_putres 	db_putres_1_arg;
		psdev_reg_x 	db_psdev_reg_1_arg;
	} argument;

	xdrproc_t 	xdr_argument, 
			xdr_result;
	DbRpcLocalFunc 	local;
//
// Added variables 
//
	int 		i, 
			j, 
			num, 
			num_dev,
			pid;
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
			logStream->noticeStream() << " RPC_QUIT_SERVER : " << log4cpp::eol;
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
		logStream->fatalStream() << "unable to free arguments" << log4cpp::eol;
		exit(1);
	}
//
// Added code to free memory allocated in server function 
//
	switch (rqstp->rq_proc)
	{
		case DB_DEVIMP: 	
			num = reinterpret_cast<db_resimp *>(result)->imp_dev.tab_dbdev_len;
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
		case DB_SECPASS: 	
			if (reinterpret_cast<db_res *>(result)->db_err == 0)
			{
				num = reinterpret_cast<db_res *>(result)->res_val.arr1_len;
				for (int i = 0; i < num;i++)
				{
					delete [] reinterpret_cast<db_res *>(result)->res_val.arr1_val[i];
					reinterpret_cast<db_res *>(result)->res_val.arr1_val[i] = NULL;
				}
				delete [] reinterpret_cast<db_res *>(result)->res_val.arr1_val;
				reinterpret_cast<db_res *>(result)->res_val.arr1_val = NULL;
				reinterpret_cast<db_res *>(result)->res_val.arr1_len = 0;
			}
			break;
		case DB_DEVINFO: 	
			delete [] reinterpret_cast<db_devinfo_svc *>(result)->device_class;
			delete [] reinterpret_cast<db_devinfo_svc *>(result)->server_name;
			delete [] reinterpret_cast<db_devinfo_svc *>(result)->personal_name;
			delete [] reinterpret_cast<db_devinfo_svc *>(result)->process_name;
			delete [] reinterpret_cast<db_devinfo_svc *>(result)->host_name;
			break;
		case DB_INFO:
			if (reinterpret_cast<db_info_svc *>(result)->dev.dom_val != 0)
			{
				delete [] reinterpret_cast<db_info_svc *>(result)->dev.dom_val;
			}
			if (reinterpret_cast<db_info_svc *>(result)->res.dom_val != 0)
			{
				delete [] reinterpret_cast<db_info_svc *>(result)->res.dom_val;
			}
			break;
		
		case DB_SVCINFO: 
			delete [] reinterpret_cast<svcinfo_svc *>(result)->process_name;
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
		
		case DB_GETPOLLER:
		 	delete [] reinterpret_cast<db_poller_svc *>(result)->server_name;
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
static void register_db(const std::string netman_host,const std::string host, const u_long prog, const u_long vers)
{
	_register_data	register_data;
	CLIENT 		*netman_clnt;
	enum clnt_stat 	clnt_stat;
	static int 	res;
//
// Create an RPC connection to network manager 
//
	netman_clnt = clnt_create(const_cast<char *>(netman_host.c_str()), NMSERVER_PROG, NMSERVER_VERS, "udp");
	if (netman_clnt == NULL)
	{
		logStream->fatalStream() << "Unable to create connection to network manager." << log4cpp::eol;
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
		logStream->fatalStream() << "register_db failed !!!" << log4cpp::eol;
		exit(1);
	}
//
// Exit function 
//
	clnt_destroy(netman_clnt);
}
#endif 
