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
 * File:        deviceserver.cpp
 *
 * Project:     System test
 *
 * Description: implementation for DeviceServer class 
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original:    June 1996
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-12-02 13:02:08 $
 *
 +**********************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

// UNIX include files

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// A Trick to use my rpc include files

#include <rpc/rpc.h>
#define _sunrpc
#include <API.h>
#include <private/ApiP.h>

// GNU C++ classes include files

#include <string>
#include <iostream>
#include <algorithm>

// testcs classes include files

#include <DeviceServer.h>


/****************************************************************************
*                                                                           *
*		Code for the DeviceServer class 			    *
*                            ------------                                   *
*                                                                           *
*    Class rule : This class has been implemented for the testcs utility    *
* 	 	  It is used only when this utility is started with the     *
*		  -d option. The main function of this class is to test     *
*		  a ESRF control system device server with the RPC RPC_CHECK*
* 		  procedure which is implemented in each device server	    *
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
*		Code for default constructor function	                    *
*                        -------------------                                *
*                                                                           *
*    Function rule : This function is the default constructor for the       *
*		     class.						    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

DeviceServer::DeviceServer()
{
#ifdef DEBUG
	std::cout << "DeviceServer default constructor" << std::endl;
#endif // DEBUG
	pn = 0;
	pid = 0;
	vers_nb = 0;
	vers_array = NULL;
	host = "";
	DS_name = "";
	DS_pers_name = "";
	DS_proc_name = "";
	DS_check_answer = "";
}



/****************************************************************************
*                                                                           *
*		Code for constructor function	                            *
*                        -----------                                        *
*                                                                           *
*    Function rule : This function is a constructor function for the  	    *
*		     class when the object is constructed from the device   *
*		     server name and its personal name.			    *
*									    *
*    Argins : - ds_name : The device server name			    *
*             - ds_pers_name : The device server personal name		    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

DeviceServer::DeviceServer(const std::string &ds_name,const std::string &ds_pers_name)
{
	register int j;
	unsigned long tmp_vers[20];

#ifdef DEBUG
	std::cout << "DeviceServer class constructor" << std::endl;
#endif // DEBUG

// Init RPC_CHECK answer field

	DS_check_answer = "";

// Init DS name and personal name (in lowercase letters)

	DS_name = ds_name;
	DS_pers_name = ds_pers_name;

	std::transform(DS_name.begin(), DS_name.end(), DS_name.begin(), ::tolower);
	std::transform(DS_pers_name.begin(), DS_pers_name.end(), DS_pers_name.begin(), ::tolower);

// Call the function implemented in the testcs db library to retrieve host,
// program number and process name

	db_svcinfo_call svcinfo;
	DevLong error;

	if (db_servinfo(const_cast<char *>(DS_name.c_str()), 
			const_cast<char *>(DS_pers_name.c_str()),& svcinfo, &error) == DS_OK)
	{
		host = svcinfo.host_name;
		DS_proc_name = svcinfo.process_name;
		pid = svcinfo.pid;
		pn = svcinfo.program_num;
	}

// Do not try to get version number if the host is "not_exp" or the PN is 0

	if ((host == "not_exp") || (pn == 0))
	{
		vers_nb = 1;
		vers_array = new unsigned long [1];
		if (vers_array == NULL)
		{
			std::cout << "DeviceServer class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		vers_array[0] = 0;
		return;
	}

// Unfortunately, the OS-9 software is unable to support the pmap_getmaps
// call to the portmapper. It kills the portmapper process on the VME.
// That's why how device server versions are retrieved in this way.
// Create a connection to the server with version 4

	CLIENT *vers_clnt;
	enum clnt_stat clnt_stat;

	vers_clnt = clnt_create((char*)host.c_str(),pn,4,"udp");
	if (vers_clnt == NULL)
	{
		vers_nb = 1;
		tmp_vers[0] = 1;
	}
	else
	{
		clnt_stat = clnt_call(vers_clnt,NULLPROC,
				(xdrproc_t)xdr_void,NULL,
				(xdrproc_t)xdr_void,NULL,
				vers_tout);
		if (clnt_stat != RPC_SUCCESS)
		{
			vers_nb = 1;
			tmp_vers[0] = 1;
		}
		else
		{
			vers_nb = 2;
			tmp_vers[0] = 1;
			tmp_vers[1] = 4;
		}
		clnt_destroy(vers_clnt);
	}

// Copy version array in the object field

	if ((vers_array = new unsigned long [vers_nb]) == NULL)
	{
		std::cout << "DeviceServer class memory allocation error, exiting" << std::endl;
//		close_db();
		exit(-1);
	}
	for (j = 0;j < vers_nb;j++)
		vers_array[j] = tmp_vers[j];

#ifdef DEBUG
	std::cout << "DS param : " << pn << ", " << host << ", " << DS_proc_name << std::endl;
	std::cout << "version number : " << vers_nb << std::endl;
	std::cout << DS_name << ", " << DS_pers_name << std::endl;
#endif // DEBUG

}



/****************************************************************************
*                                                                           *
*		Code for copy constructor function	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : This functions is the copy constructor		    *
*									    *
*    Argins : - k : A reference to the other DeviceServer object	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

DeviceServer::DeviceServer(const DeviceServer &k)
{
#ifdef DEBUG
	std::cout << "DeviceServer copy constructor" << std::endl;
#endif // DEBUG
	pn = k.pn;
	pid = k.pid;
	vers_nb = k.vers_nb;
	if (vers_array != NULL)
	{
		vers_array = new unsigned long [vers_nb];
		if (vers_array == NULL)
		{
			std::cout << "DeviceServer class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < vers_nb;i++)
			vers_array[i] = k.vers_array[i];
	}
	else
		vers_array = NULL;
	host = k.host;
	DS_name = k.DS_name;
	DS_pers_name = k.DS_pers_name;
	DS_proc_name = k.DS_proc_name;
	DS_check_answer = k.DS_check_answer;
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

DeviceServer::~DeviceServer()
{
#ifdef DEBUG
	std::cout << "DeviceServer destructor" << std::endl;
#endif // DEBUG
	delete [] vers_array;
}



/****************************************************************************
*                                                                           *
*		Code for = operator overloading 	                    *
*                        ----------                                         *
*                                                                           *
*    Function rule : This functions is the = operator overloading. This     *
*		     allows a C++ program which uses this class to use the  *
*     		     = operator between two objects of the DeviceServer     *
*		     class						    *
*									    *
*    Argins : - k : A reference to the other DeviceServer object	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns a reference to a DeviceServer object		    *
*                                                                           *
****************************************************************************/

DeviceServer &DeviceServer::operator=(const DeviceServer &k)
{
#ifdef DEBUG
	std::cout << "DeviceServer = operator overloading function" << std::endl;
#endif // DEBUG
	this->pn = k.pn;
	this->pid = k.pid;
	this->host = k.host;
	this->DS_name = k.DS_name;
	this->DS_pers_name = k.DS_pers_name;
	this->DS_proc_name = k.DS_proc_name;
	this->DS_check_answer = k.DS_check_answer;
	this->vers_nb = k.vers_nb;
	if (k.vers_array != NULL)
	{
		this->vers_array = new unsigned long [this->vers_nb];
		if (this->vers_array == NULL)
		{
			std::cout << "DeviceServer class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < this->vers_nb;i++)
			this->vers_array[i] = k.vers_array[i];
	}
	else
		this->vers_array = NULL;
	return(*this);
}



/****************************************************************************
*                                                                           *
*		Code for test_ds_udp method	   	                    *
*                        -----------                                        *
*                                                                           *
*    Function rule : To test a server using the UDP protocol. This is done  *
*                    by creating a connection to the server and trying the  *
*		     server RPC_CHECK procedure				    *
*		     The RPC timeout is set to 6 seconds with a retry every *
*		     2 seconds						    *
*									    *
*    Argins : - vers : The server version to be tested			    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns 0 if the server answers correctly. Otherwise, it *
*    returns 1 if the device server is defined in the database but with a   *
*    "not_exp" host or -1 if the test fails.				    *
*                                                                           *
****************************************************************************/

int DeviceServer::test_ds_udp(const long vers)
{
	CLIENT *serv_clnt;
	enum clnt_stat clnt_stat;
	char ds[80];
	char *ds_str;

// If there is only one version and this version is 0

	if ((vers_nb == 1) && (vers_array[0] == 0))
		return(1);

// Create a RPC connection to the server

	serv_clnt = clnt_create(const_cast<char *>(this->host.c_str()),this->pn,vers,"udp");
	if (serv_clnt == NULL)
		return(-1);

	clnt_control(serv_clnt,CLSET_RETRY_TIMEOUT,(char *)&ds_retry_tout);
	clnt_control(serv_clnt,CLSET_TIMEOUT,(char *)&ds_tout);

// Test server with the RPC NULLPROC procedure

	ds_str = ds;
	clnt_stat = clnt_call(serv_clnt,RPC_CHECK,
			(xdrproc_t)xdr_void,NULL,
			(xdrproc_t)xdr_wrapstring, (caddr_t)&ds_str,
			ds_tout);
	if (clnt_stat != RPC_SUCCESS )
	{
		clnt_destroy(serv_clnt);
		return(-1);
	}
#ifdef DEBUG
	std::cout << ds << std::endl;
#endif // DEBUG

// Copy RPC call answer into object filed

	DS_check_answer = ds_str;

// Server answers correctly

	clnt_destroy(serv_clnt);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for test_ds_tcp method	 	                    *
*                        -----------                                        *
*                                                                           *
*    Function rule : To test a server using the TCP protocol. This is done  *
*                    by creating a connection to the server and trying the  *
*		     server RPC_CHECK procedure				    *
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

int DeviceServer::test_ds_tcp(const long vers)
{
	CLIENT *serv_clnt;
	enum clnt_stat clnt_stat;
	char ds[80];
	char *ds_str;

// Create a RPC connection to the server

	serv_clnt = clnt_create(const_cast<char *>(this->host.c_str()),this->pn,vers,"tcp");
	if (serv_clnt == NULL)
		return(-1);

	clnt_control(serv_clnt,CLSET_RETRY_TIMEOUT,(char *)&ds_retry_tout);
	clnt_control(serv_clnt,CLSET_TIMEOUT,(char *)&ds_tout);

// Test server with the RPC NULLPROC procedure

	ds_str = ds;
	clnt_stat = clnt_call(serv_clnt,RPC_CHECK,
			(xdrproc_t)xdr_void,NULL,
			(xdrproc_t)xdr_wrapstring, (caddr_t)&ds_str,
			ds_tout);
	if (clnt_stat != RPC_SUCCESS )
	{
		clnt_destroy(serv_clnt);
		return(-1);
	}
#ifdef DEBUG
	std::cout << ds << std::endl;
#endif // DEBUG

// Copy RPC call answer into object field

	DS_check_answer = ds_str;

// Server answers correctly

	clnt_destroy(serv_clnt);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for get_vers_nb method	 	                    *
*                        -----------                                        *
*                                                                           *
*    Function rule : To returns the number of version registered in the     *
*		     portmapper.					    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the version number array size		    *
*                                                                           *
****************************************************************************/

long DeviceServer::get_vers_nb() const
{
	return(this->vers_nb);
}



/****************************************************************************
*                                                                           *
*		Code for get_vers method	 	                    *
*                        --------                                           *
*                                                                           *
*    Function rule : To get a version numer				    *
*									    *
*    Argins : - which : the version array index				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the version number or 0 is the version number    *
*    array is empty or -1 if the index in the array is out of bounds	    *
*                                                                           *
****************************************************************************/

long DeviceServer::get_vers(long which) const
{

// If the index is out of bounds

	if (which > this->vers_nb)
		return(-1);

// If the version number array is empty

	if (this->vers_nb == 0)
		return(0);
	else
		return((long)this->vers_array[which]);
}



/****************************************************************************
*                                                                           *
*		Code for get_check_answer method	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : To get the RPC_CHECK procedure answer. This proc. is   *
*		     called by the test_ds_xx method. If this method is     *
*		     called before a test_ds_xx, it returns a empty string  *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : p_answer : The returned string				    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void DeviceServer::get_check_answer(std::string &p_answer) const
{
	p_answer = DS_check_answer;
}



/****************************************************************************
*                                                                           *
*		Code for get_check_answer method	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : To get the RPC_CHECK procedure answer. This proc. is   *
*		     called by the test_ds_xx method. If this method is     *
*		     called before a test_ds_xx, it returns a empty string  *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : p_answer : The returned string				    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void DeviceServer::get_host(std::string &p_answer) const
{
	p_answer = host;
}



/****************************************************************************
*                                                                           *
*		Code for get_ds_process_name method	                    *
*                        -------------------                                *
*                                                                           *
*    Function rule : To get the device server process name		    *
*									    *
*    Argins : No argin(s)						    *
*                                                                           *
*    Argout : - p_answer : A reference to a string which will be initialised*
*			   with the device server process name		    *
*									    *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void DeviceServer::get_ds_process_name(std::string &p_answer) const
{
	p_answer = DS_proc_name;
}



/****************************************************************************
*                                                                           *
*		Code for get_pid method		 	                    *
*                        -------    	                                    *
*                                                                           *
*    Function rule : To returns the device server process PID as it is      *
*		     registered in the database				    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the device server process PID		    *
*                                                                           *
****************************************************************************/

long DeviceServer::get_pid() const
{
	return(this->pid);
}

