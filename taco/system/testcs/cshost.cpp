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
 * File:        cshost.cpp
 *
 * Project:     System test
 *
 * Description: implementation for CSHost class 
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original:    June 1996
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2008-04-06 09:07:56 $
 *
 +**********************************************************************/

// UNIX include files

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#ifdef sun
#include <sys/types.h>
#endif

#include <cstdlib>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>

// ESRF include files

#include <API.h>

// GNU C++ classes include files

#include <string>
#include <iostream>
#include <strstream>
#include <algorithm>

// testcs classes include files

#include <DeviceServer.h>
#include <CSHost.h>

// functions declaration

long rpc_check_host (char *host_name, long *error);


/****************************************************************************
*                                                                           *
*		Code for the CSHost class       			    *
*                            ------                                         *
*                                                                           *
*    Class rule : This class has been implemented for the testcs utility    *
* 	 	  It is used when this utility is started with the -a and   *
*		  -h option. The main function of this class is to test     *
*		  all the device server running on a host with the RPC      *
*		  RPC_CHECK procedure which is implemented in each device   *
*		  server.						    *
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

CSHost::CSHost()
{
#ifdef DEBUG
	std::cout << "CSHost default constructor" << std::endl;
#endif // DEBUG
	ds_nb = 0;
	ds_array = NULL;
	host_name = "";
}



/****************************************************************************
*                                                                           *
*		Code for constructor function	                            *
*                        -----------                                        *
*                                                                           *
*    Function rule : This function is a constructor function for the  	    *
*		     class when the object is constructed from the host     *
*		     name.						    *
*									    *
*    Argins : - h_name : The host name					    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

CSHost::CSHost(const std::string &h_name)
{
	device *p_ds_from_db;
	long ds_db_nb;
	long tmp_embedded;
	int tmp_pn,tmp_pid;
	long error,res;

#ifdef DEBUG
	std::cout << "CSHost class constructor" << std::endl;
#endif // DEBUG

// Init host name class field

	host_name = h_name;

// Test if the host is running

	res = rpc_check_host(const_cast<char *>(h_name.c_str()),&error);
	if (res == DS_OK)
		host_alive = True;
	else
	{
		host_alive = False;
		ds_array = NULL;
		ds_nb = 0;
		return;
	}

// Get a unsorted list of device server registered in this host
//#warning implement get_ds_host
//	get_ds_host(const_cast<char *>(h_name.c_str()),&p_ds_from_db,&ds_db_nb);

// Leave constructor if no device server is defined in this host

	if (ds_db_nb == 0)
	{
		ds_array = NULL;
		ds_nb = 0;
		return;
	}

#ifdef DEBUG
	std::cout << "DB ds number : " << ds_db_nb << std::endl;
	for (int i = 0;i < ds_db_nb;i++)
		std::cout << p_ds_from_db[i].ds_class << "/" << p_ds_from_db[i].ds_name << std::endl;
#endif // DEBUG

// Allocate memory for the _HostDeviceServer array

	if ((ds_array = new _HostDeviceServer [ds_db_nb]) == NULL)
	{
		std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//		close_db();
		exit(-1);
	}

// Build the real device server array (if pn and pid are the same for different
// device server in database, this means that it is a device server with
// different embedded classes)

	ds_nb = 0;
	for (int i = 0;i < ds_db_nb; i++)
	{
		if (p_ds_from_db[i].h_name[0] != (char)0)
		{

// There is a real new device server, create a temporary DeviceServer
// object and copy it in the real ds array

			std::string ds_name(p_ds_from_db[i].ds_class);
			std::string pers_name(p_ds_from_db[i].ds_name);

			DeviceServer tmp_ds(ds_name,pers_name);
			ds_array[ds_nb].ds = tmp_ds;

			tmp_embedded = 1;
			tmp_pn = p_ds_from_db[i].pn;
			tmp_pid = p_ds_from_db[i].pid;

// Look if any other device server classes embedded in the new ds

			for(int j = i + 1;j < ds_db_nb;j++)
			{
				if ((p_ds_from_db[j].pn == tmp_pn) &&
				    (p_ds_from_db[j].pid == tmp_pid))
				{
					tmp_embedded++;
					p_ds_from_db[j].h_name[0] = (char)0;
				}
			}

// Allocate memory to store embedded classes name list

			ds_array[ds_nb].ds_embedded = tmp_embedded;
			if ((ds_array[ds_nb].ds_list = new std::string [tmp_embedded]) == NULL)
			{
				std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//				close_db();
				exit(-1);
			}

// Init the embedded classes name list

			int l = 0;
			for (int k = i;k < ds_db_nb;k++)
			{
				if ((p_ds_from_db[k].pn == tmp_pn) &&
				    (p_ds_from_db[k].pid == tmp_pid))
				{
					ds_array[ds_nb].ds_list[l] = p_ds_from_db[k].ds_class;
					ds_array[ds_nb].ds_list[l] += '/';
					ds_array[ds_nb].ds_list[l] += p_ds_from_db[k].ds_name;
					l++;
				}
			}
			ds_nb++;
		}
	}

#ifdef DEBUG
	std::cout << "REAL ds number : " << ds_nb << std::endl;
	for (int i = 0;i < ds_nb;i++)
	{
		std::cout << "DS number : " << i + 1 << std::endl;
		for (int j = 0;j < ds_array[i].ds_embedded;j++)
			std::cout << ds_array[i].ds_list[j] << std::endl;
	}
#endif // DEBUG


}



/****************************************************************************
*                                                                           *
*		Code for copy constructor function	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : This functions is the copy constructor		    *
*									    *
*    Argins : - k : A reference to the other CSHost object	    	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

CSHost::CSHost(const CSHost &k)
{
#ifdef DEBUG
	std::cout << "CSHost copy constructor" << std::endl;
#endif // DEBUG

	host_name = k.host_name;
	ds_nb = k.ds_nb;
	host_alive = k.host_alive;
	if (ds_array != NULL)
	{
		ds_array = new _HostDeviceServer [ds_nb];
		if (ds_array == NULL)
		{
			std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < ds_nb;i++)
		{
			ds_array[i] = k.ds_array[i];
			if (ds_array[i].ds_list != NULL)
			{
				if ((ds_array[i].ds_list = new std::string [ds_array[i].ds_embedded]) == NULL)
				{
					std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//					close_db();
					exit(-1);
				}
				for (int j = 0;j < ds_array[i].ds_embedded;j++)
					ds_array[i].ds_list[j] = k.ds_array[i].ds_list[j];
			}
		}
	}
	else
		ds_array = NULL;
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

CSHost::~CSHost()
{
#ifdef DEBUG
	std::cout << "CSHost destructor" << std::endl;
#endif // DEBUG
	if (ds_array != NULL)
	{
		for (int i = 0;i < ds_nb;i++)
			delete [] ds_array[i].ds_list;
	}
	delete [] ds_array;
}



/****************************************************************************
*                                                                           *
*		Code for = operator overloading 	                    *
*                        ----------                                         *
*                                                                           *
*    Function rule : This functions is the = operator overloading. This     *
*		     allows a C++ program which uses this class to use the  *
*     		     = operator between two objects of the CSHost     	    *
*		     class						    *
*									    *
*    Argins : - k : A reference to the other CSHost object	    	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns a reference to a CSHost object		    *
*                                                                           *
****************************************************************************/

CSHost &CSHost::operator=(const CSHost &k)
{
#ifdef DEBUG
	std::cout << "CSHost = operator overloading function" << std::endl;
#endif // DEBUG

	this->host_name = k.host_name;
	this->ds_nb = k.ds_nb;
	this->host_alive = k.host_alive;
	if (k.ds_array != NULL)
	{
		this->ds_array = new _HostDeviceServer [this->ds_nb];	
		if (this->ds_array == NULL)
		{
			std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < this->ds_nb;i++)
		{
			this->ds_array[i] = k.ds_array[i];
			if (this->ds_array[i].ds_list != NULL)
			{
				if ((this->ds_array[i].ds_list = new std::string [this->ds_array[i].ds_embedded]) == NULL)
				{
					std::cout << "CSHost class memory allocation error, exiting" << std::endl;
//					close_db();
					exit(-1);
				}
				for (int j = 0;j < this->ds_array[i].ds_embedded;j++)
					this->ds_array[i].ds_list[j] = k.ds_array[i].ds_list[j];
			}
		}
	}
	else
		this->ds_array = NULL;
	return(*this);
}



/****************************************************************************
*                                                                           *
*		Code for get_ds_nb method	 	                    *
*                        ---------                                          *
*                                                                           *
*    Function rule : To returns the number of server registered for a host  *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the device server array size		    	    *
*                                                                           *
****************************************************************************/

long CSHost::get_ds_nb() const
{
	return(this->ds_nb);
}



/****************************************************************************
*                                                                           *
*		Code for get_alive_flag method	 	                    *
*                        --------------                                     *
*                                                                           *
*    Function rule : To returns the host alive flag			    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the host alive flag				    *
*                                                                           *
****************************************************************************/

long CSHost::get_alive_flag() const
{
	return(this->host_alive);
}



/****************************************************************************
*                                                                           *
*		Code for get_host_name method	 	                    *
*                        -------------                                      *
*                                                                           *
*    Function rule : To returns the host name				    *
*									    *
*    Argins : No argin(s)						    *
*									    *
*    Argout : - r_host : Reference to the string where the host name will be*
*			 copied						    *
*                                                                           *
*    This methos does not return anything				    *
*                                                                           *
****************************************************************************/

void CSHost::get_host_name(std::string &r_host) const
{
	r_host = host_name;
}



/****************************************************************************
*                                                                           *
*		Code for test_ds_on_host method	 	                    *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To test a device server on the specific host. This     *
*		     method uses DeviceServer test method implemented in the*
*		     DeviceServer class					    *
*									    *
*    Argins : - which : The index in the device server array of the server  *
*			to be tested					    *
*	      - verbose : The verbose flag				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns 0 if all the RPC test are correct. Otherwise,    *
*    it returns -1							    *
*                                                                           *
****************************************************************************/

long CSHost::test_ds_on_host(const int which,const long verbose) const
{
	long nb_vers,vers;
	int i,ret,k;
	std::string full_ds_name;
	std::string answer,process;
	long tmp_pid;

// If index out of bounds, return -1

	if (which > ds_nb)
		return(-1);

// Get device server version(s)

	nb_vers = ds_array[which].ds.get_vers_nb();
	for (i = 0;i < ds_array[which].ds_embedded;i++)
	{
		full_ds_name += ds_array[which].ds_list[i];
		if (i != (ds_array[which].ds_embedded - 1))
			full_ds_name += " and ";
	}

	for (i = 0;i < nb_vers;i++)
	{

// Get a version number

		vers = ds_array[which].ds.get_vers(i);
		if (vers == -1)
		{
			std::cout << "Mismatch in " << full_ds_name << " version number" << std::endl;
			return(-1);
		}

// Test the device server UDP connection

		ret = ds_array[which].ds.test_ds_udp(vers);
		if (ret == 0)
		{

// More test are needed for the special case where two device servers are
// registered in the database with the same program number

			ds_array[which].ds.get_check_answer(answer);
			std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);
			ds_array[which].ds.get_ds_process_name(process);

#ifdef PRINT_ANSWER
			std::cout << "answer : " << answer << std::endl;
			std::cout << "process : " << process << std::endl;
#endif // PRINT_ANSWER

// For DS not recmpiled with db software release 5, the process name is 
// unknown, otherwise, the process name is correctly initialised.

			if (process != "unknown")
			{

// Check if the process name returned by the device server is the same than
// the process name stored in the database

				std::string proc_answer;
				std::string::size_type pos = answer.find('/');
				if (pos != std::string::npos)
					proc_answer = answer.substr(0, pos);
				if (proc_answer != process)
				{
					std::cout << "DS " << full_ds_name << " defined in database on host " << host_name << " but not started" << std::endl;
					return(-1);
				}
			}
			else
			{

// We don't have any information on process name, try to retrieve the name
// returned by the RPC_CHECK in the embedded class list

				for (k = 0;k < ds_array[which].ds_embedded;k++)
				{
					if (answer == ds_array[which].ds_list[k])
						break;
				}
				if (k == ds_array[which].ds_embedded)
				{
					std::cout << "DS " << full_ds_name << " defined in database on host " << host_name << " but not started" << std::endl;
					return(-1);
				}
			}

			if (verbose == True)
				std::cout << "DS " << full_ds_name << " : UDP version " << vers << " ==> OK" << std::endl;
		}
		else if (ret == 1)
		{
			std::cout << "DS " << full_ds_name << " defined in database on host " << host_name << " but not started" << std::endl;
			return(-1);
		}
		else
		{
			std::cout << "DS " << full_ds_name << " : UDP version " << vers << " ==> NOK !!!!!!!" << std::endl;
			tmp_pid = ds_array[which].ds.get_pid();
			std::cout << "Process ID " << tmp_pid << " found in database" << std::endl;
			return(-1);
		}

// Test the device server TCP connection

		ret = ds_array[which].ds.test_ds_tcp(vers);
		if (ret == 0)
		{
			if (verbose == True)
				std::cout << "DS " << full_ds_name << " : TCP version " << vers << " ==> OK" << std::endl;
		}
		else
		{
			std::cout << "DS " << full_ds_name << " : TCP version " << vers << " ==> NOK !!!!!!!" << std::endl;
			tmp_pid = ds_array[which].ds.get_pid();
			std::cout << "Process ID " << tmp_pid << " found in database" << std::endl;
			return(-1);
		}
	}

// Leave function

	return(0);

}



/*+**********************************************************************
 Function   :	long rpc_check_host()

 Description:	Checks whether the remote host is responding
		or not. Opens a TCP connection to the portmapper
		and tests the write access on the socket.

 Arg(s) In  :	char *host_name - name of the remote host.

 Arg(s) Out :	error  - Will contain an appropriate error code if the
		         corresponding call returns a non-zero value.

 Return(s)  :	DS_OK or DS_NOTOK
***********************************************************************-*/

long rpc_check_host (char *host_name, long *error)
{
	int 			s;  
	struct hostent 		*hp;
	struct servent 		*sp;
	struct sockaddr_in 	peeraddr_in; 
	int 			nb;
	int 			nfds;
	int 			nfd;
	fd_set 			writemask;
	struct timeval		timeout_host;

	*error = 0;

/* clear out address structures */

	memset ((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));

/* Set up the peer address to which we will connect. */

	peeraddr_in.sin_family = AF_INET;

/* Get the host information for the hostname that the user passed in. */

	hp = gethostbyname (host_name);
	if (hp == NULL)
	{
		*error = DevErr_CannotConnectToHost;
		return (DS_NOTOK);
	}

	peeraddr_in.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;

/* Find the information for the "example" server in order to get the needed 
   port number.*/

#ifdef sun
	sp = getservbyname ("sunrpc", "tcp");
#else
	sp = getservbyname ("portmap", "tcp");
#endif /* sun */
	if (sp == NULL)
	{
		*error = DevErr_CannotConnectToHost;
		return (DS_NOTOK);
	}
	peeraddr_in.sin_port = sp->s_port;

/* Create the socket. */

	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s == DS_NOTOK)
	{
		*error = DevErr_CannotConnectToHost;
		return (DS_NOTOK);
	}

/* Set the socket into noblocking mode */

#if defined(sun) || defined(linux) || defined(FreeBSD)
	nb =1;
	ioctl (s, FIONBIO, &nb);
#else
	nb =1;
	ioctl (s, FIOSNBIO, &nb);
#endif /* sun */

/* try to connect to the remote host.*/

#if defined(sun) || defined(linux) || defined(FreeBSD)
	connect (s,(struct sockaddr *)&peeraddr_in,sizeof(struct sockaddr_in));
#else
	connect (s, (void *) &peeraddr_in, sizeof(struct sockaddr_in));
#endif

/* select the socket and wait until the connection is ready for write access. */

	FD_ZERO (&writemask);
	FD_SET  (s,&writemask);
	nfds = s+1;

	timeout_host.tv_sec = 2;
	timeout_host.tv_usec = 0;

#ifdef __hpux
	nfd = select (nfds, NULL, (int *)&writemask, NULL, &timeout_host);
#else
	nfd = select (nfds, NULL, (fd_set *)&writemask, NULL, &timeout_host);
#endif /* __hpux */

	if (nfd == DS_NOTOK)
	{
		*error = DevErr_CannotConnectToHost;
		close (s);
		return (DS_NOTOK);
	}

/* check whether the select call was timed out */

	if ( FD_ISSET (s, &writemask) == FALSE)
	{
		*error = DevErr_CannotConnectToHost;
		close (s);
		return (DS_NOTOK);
	}

	close (s);

	return (DS_OK);
}

