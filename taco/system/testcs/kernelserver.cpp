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
 * File:        kernelserver.cpp
 *
 * Project:     System test
 *
 * Description: implementation for KernelServer class 
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original:    June 1996
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2008-12-02 13:02:08 $
 *
 +**********************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// A Trick to use my rpc include files

#include <rpc/rpc.h>
#define _sunrpc
#include <API.h>
#include <private/ApiP.h>


#include <string.h>
#include <iostream>
#include <sstream>
#include <KernelServer.h>
#ifdef _solaris
#	include "_count.h"
#endif
#include <algorithm>

void get_database_param(unsigned int &,std::string &);
void get_dc_param(int,int,char *,unsigned int &,std::string &);

extern bool_t xdr__manager_data(XDR *,struct _manager_data *);

#ifndef HAVE_DECL_GETHOSTNAME
#ifdef sun
extern "C" {int gethostname(char *,int);}
#endif
#endif


/****************************************************************************
*                                                                           *
*		Code for the KernelServer class 			    *
*                            ------------                                   *
*                                                                           *
*    Class rule : This class has been implemented for the testcs utility    *
* 	 	  It is used only when this utility is started with the     *
*		  -k option. The main function of this class is to test     *
*		  a ESRF control system kernel server (Manager, database    *
*		  server and data collector) with the classical RPC NULLPROC*
* 		  procedure	    					    *
*									    *
*    Remark : This class has been compiled with the GNU GCC compiler.       *
*	      Unfortunately, the C++ wonderful exception mechnism is not    *
*	      implemented for the HP harware and of course, I can't use it. *
*	      That's why, if function fails in the constructor, I print a   *
*	      message and then exit.					    *
*                                                                           *
****************************************************************************/




/****************************************************************************
*                                                                           *
*		Code for constructor function	                            *
*                        -----------                                        *
*                                                                           *
*    Function rule : These functions are the constructors function for the  *
*		     class. As the three function parameters have default   *
*		     values, this function is also the default constructor  *
*									    *
*    Argins : - server_type : The server type to be tested. It should be the*
*                             manager, database server, dc read or write    *
*			      server					    *
*			      default value = 0				    *
*             - server_nb : The server number. Only used for the dc test    *
*			    default value = 0				    *
*	      - dc_host : The dc host name. Only used for dc server test    *
*			  default value = 0 				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

KernelServer::KernelServer(const int server_type,const int server_nb,char *dc_host)
{
	char *tmp;
	switch (server_type)
	{
		case 0 :
#ifdef DEBUG
			std::cout << "KernelServer default constructor" << std::endl;
#endif // DEBUG
			pn = 0;
			host = "";
			break;

		case MANAGER :
#ifdef DEBUG
			std::cout << "KernelServer constructor for Manager" << std::endl;
#endif // DEBUG
			pn = NMSERVER_PROG;
			if ((tmp = getenv("NETHOST")) == NULL)
			{
				std::cout << "NETHOST environment variable not defined" << std::endl;
				exit(-1);
			}
			host = tmp;
			break;

		case DATABASE :
#ifdef DEBUG
			std::cout << "KernelServer constructor for database" << std::endl;
#endif // DEBUG
			get_database_param(pn,host);
			break;

		case DC_RD :
#ifdef DEBUG
			std::cout << "KernelServer constructor for dc read server" << std::endl;
#endif // DEBUG
			get_dc_param(server_nb,READ,dc_host,pn,host);
			break;

		case DC_WR :
#ifdef DEBUG
			std::cout << "KernelServer constructor for dc write server" << std::endl;
#endif // DEBUG
			get_dc_param(server_nb,WRITE,dc_host,pn,host);
			break;
	}
}



/****************************************************************************
*                                                                           *
*		Code for get_database_param function  	                    *
*                        ------------------                                 *
*                                                                           *
*    Function rule : This function initialise the KernelServer object       *
*                    private data for a database server (program number and *
*		     host). These information are asked to the manager via  *
*		     a RPC call to it.					    *
*									    *
*    Argins : No argin							    *
*									    *
*    Argout : - pn : A reference to the pn object private field 	    *
*	      - host : A reference to the host object private field	    *
*                                                                           *
*    This function does not returns anything				    *
*									    *
****************************************************************************/

void get_database_param(unsigned int &pn,std::string &host)
{
	CLIENT *man_clnt;
	char man_host[40];
	char *tmp;
	_manager_data manager_data;
	_register_data register_data;
	static char host_name[SHORT_NAME_SIZE];
	enum clnt_stat clnt_stat;

// Get manager host name

	tmp = getenv("NETHOST");
	if (tmp == NULL)
	{
		std::cout << "NETHOST environment variable not defined" << std::endl;
		exit(-1);
	}
	strcpy(man_host,tmp);

// Clear Manager data structure

	memset(&manager_data,0,sizeof(manager_data));

// Init register data structure

	gethostname(host_name,32);

	register_data.host_name = host_name;
	register_data.prog_number = getpid();
	register_data.vers_number = 0;

// Create a RPC connection to the Manager

	man_clnt = clnt_create(man_host,NMSERVER_PROG,NMSERVER_VERS,"udp");
	if (man_clnt == NULL)
	{
		std::cout << "Unable to create connection to network manager !!" << std::endl;
		exit(-1);
	}

	clnt_control(man_clnt,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
	clnt_control(man_clnt,CLSET_TIMEOUT,(char *)&timeout);

// Get database server info from Manager

	clnt_stat = clnt_call(man_clnt,RPC_GET_CONFIG,
			(xdrproc_t)xdr__register_data, (caddr_t)&register_data,
			(xdrproc_t)xdr__manager_data, (caddr_t)&manager_data,
			timeout);
	if (clnt_stat != RPC_SUCCESS)
	{
		std::cout << "Unable to get database server information from Manager" << std::endl;
		clnt_destroy(man_clnt);
		exit(-1);
	}

// Init object data

	host = manager_data.db_info.host_name;
	pn = manager_data.db_info.prog_number;
#ifdef DEBUG
	std::cout << "Database : " << host << ", " << pn << std::endl;
#endif // DEBUG 

// Close RPC connection to Manager

	clnt_destroy(man_clnt);

}



/****************************************************************************
*                                                                           *
*		Code for get_dc_param function  	                    *
*                        ------------                                       *
*                                                                           *
*    Function rule : This function initialise the KernelServer object       *
*                    private data for a data collector server (program      *
*                    number and host). These        			    *
*		     information are fetched from the static database with  *
*		     the pseudo-device associated to each data collector    *
*		     server						    *
*									    *
*    Argins : - server_nb : The server number				    *
*	      - dc_type : The data collector server type (READ or WRITE)    *
*	      -	dc_host : The data collector host			    *
*									    *
*    Argout : - pn : A reference to the pn object private field 	    *
*	      - host : A reference to the host object private field	    *
*                                                                           *
*    This function does not returns anything				    *
*                                                                           *
****************************************************************************/

void get_dc_param(int server_nb,int dc_type,char *dc_host,unsigned int &pn,std::string &host)
{
	struct hostent *p_host;
	unsigned int tmp =  0;
	char *tmp_dev;
	db_devinf_imp *serv_net;
	int nb_dash;
	DevLong error;

// Build dc server pseudo device name

	std::ostringstream misc;
	std::string dc_name = "sys/dc_";

	if ((p_host = gethostbyname(dc_host)) == NULL)
	{
		std::cout << "Cant get IP address for " << dc_host << std::endl;
		exit(-1);
	}
	tmp = tmp + (unsigned char)p_host->h_addr[3];

	if (dc_type == READ)
		dc_name = dc_name + "rd_";
	else
		dc_name = dc_name + "wr_";
	misc << tmp << std::ends;
	dc_name = dc_name + misc.str();
	dc_name = dc_name + '/';
	misc.seekp(0);
	misc << server_nb << std::ends ;
	dc_name = dc_name + misc.str();

// Get dc server network parameter from database

	tmp_dev = const_cast<char *>(dc_name.c_str());

	if (db_dev_import(&tmp_dev,&serv_net,1,&error))
	{
		std::cout << "Cant get dc server pseudo device information from database" << std::endl;
		std::cout << "Error code : " << error << " for device : " << dc_name << std::endl;
		exit(-1);
	}

// Remove the .esrf.fr at the end of host name (if any) and init the host
// field of the kernel server object

	std::string tmp_host = serv_net[0].host_name;
#ifndef _solaris
	nb_dash = std::count(tmp_host.begin(), tmp_host.end(), '.');
#else
	nb_dash = _sol::count(tmp_host.begin(), tmp_host.end(), '.');
#endif
	if (nb_dash != 0)
		host = tmp_host.substr(0, tmp_host.find('.'));
	else
		host = tmp_host;
		
// Init program number and free memory

	pn = serv_net[0].pn;
	free(serv_net);
#ifdef DEBUG
	std::cout << "DC : " << host << ", " << pn << std::endl;
#endif // DEBUG 

}



/****************************************************************************
*                                                                           *
*		Code for copy constructor function	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : This functions is the copy constructor		    *
*									    *
*    Argins : - k : A reference to the other KernelServer object	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

KernelServer::KernelServer(const KernelServer &k)
{
#ifdef DEBUG
	std::cout << "KernelServer copy constructor" << std::endl;
#endif // DEBUG
	pn = k.pn;
	host = k.host;
}



/****************************************************************************
*                                                                           *
*		Code for destructor function	 	                    *
*                        ----------                                         *
*                                                                           *
*    Function rule : This functions is the object destructor		    *
*									    *
*    Argins : No argin							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a destructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

KernelServer::~KernelServer()
{
#ifdef DEBUG
	std::cout << "KernelServer destructor" << std::endl;
#endif // DEBUG
}



/****************************************************************************
*                                                                           *
*		Code for = operator overloading 	                    *
*                        ----------                                         *
*                                                                           *
*    Function rule : This functions is the = operator overloading. This     *
*		     allows a C++ program which uses this class to use the  *
*     		     = operator between two objects of the KernelServer     *
*		     class						    *
*									    *
*    Argins : - k : A reference to the other KernelServer object	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns a reference to a KernelServer object		    *
*                                                                           *
****************************************************************************/

KernelServer &KernelServer::operator=(const KernelServer &k)
{
#ifdef DEBUG
	std::cout << "KernelServer = operator overloading function" << std::endl;
#endif // DEBUG
	this->pn = k.pn;
	this->host = k.host;
	return(*this);
}



/****************************************************************************
*                                                                           *
*		Code for test_server_udp function 	                    *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To test a server using the UDP protocol. This is done  *
*                    by creating a connection to the server and trying the  *
*		     server NULL_PROC procedure				    *
*		     The RPC timeout is set to 6 seconds with a retry every *
*		     2 seconds						    *
*									    *
*    Argins : - vers : The server version to be tested			    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns 0 if the server answers correctly. Otherwise, it *
*    returns -1.							    *
*                                                                           *
****************************************************************************/

int KernelServer::test_server_udp(const int vers) const
{
	CLIENT *serv_clnt;
	enum clnt_stat clnt_stat;

// Create a RPC connection to the server

	serv_clnt = clnt_create(const_cast<char *>(this->host.c_str()),this->pn,vers,"udp");
	if (serv_clnt == NULL)
		return(-1);

	clnt_control(serv_clnt,CLSET_RETRY_TIMEOUT,(char *)&retry_tout);
	clnt_control(serv_clnt,CLSET_TIMEOUT,(char *)&tout);

// Test server with the RPC NULLPROC procedure

	clnt_stat = clnt_call(serv_clnt,NULLPROC,
			(xdrproc_t)xdr_void,NULL,
			(xdrproc_t)xdr_void,NULL,
			      tout);
	if (clnt_stat != RPC_SUCCESS )
	{
		clnt_destroy(serv_clnt);
		return(-1);
	}

// Server answers correctly

	clnt_destroy(serv_clnt);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for test_server_tcp function 	                    *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To test a server using the TCP protocol. This is done  *
*                    by creating a connection to the server and trying the  *
*		     server NULL_PROC procedure				    *
*		     The RPC timeout is set to 6 seconds with a retry every *
*		     2 seconds						    *
*									    *
*    Argins : - vers : The server version to be tested			    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns 0 if the server answers correctly. Otherwise, it *
*    returns -1.							    *
*                                                                           *
****************************************************************************/

int KernelServer::test_server_tcp(const int vers) const
{
	CLIENT *serv_clnt;
	enum clnt_stat clnt_stat;

// Create a RPC connection to the server

	serv_clnt = clnt_create(const_cast<char *>(this->host.c_str()),this->pn,vers,"tcp");
	if (serv_clnt == NULL)
		return(-1);

	clnt_control(serv_clnt,CLSET_RETRY_TIMEOUT,(char *)&retry_tout);
	clnt_control(serv_clnt,CLSET_TIMEOUT,(char *)&tout);

// Test server with the RPC NULLPROC procedure

	clnt_stat = clnt_call(serv_clnt,NULLPROC,
			(xdrproc_t)xdr_void,NULL,
			(xdrproc_t)xdr_void,NULL,
			      tout);
	if (clnt_stat != RPC_SUCCESS )
	{
		clnt_destroy(serv_clnt);
		return(-1);
	}

// Server answers correctly

	clnt_destroy(serv_clnt);
	return(0);

}
