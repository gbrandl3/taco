/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        testcs.cpp
 *
 * Project:     System test
 *
 * Description: implementation for testcs program
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

#include <string>

// GNU C++ include file

#include <iostream>
#include <unistd.h>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

// testcs classes include files

#include <KernelServer.h>
#include <DeviceServer.h>
#include <CSHost.h>
#include <EsrfControlSystem.h>

// Miscellaneous include files

#include <API.h>
#include <testcs.h>


// Some global variables

/****************************************************************************
*                                                                           *
*		Code for testcs utility                                     *
*                        ------                                             *
*                                                                           *
*    Command rule : To test a ESRF style control system.		    *
*                   With options, it is possible to test :		    *
*                      - The kernel control system (-k)			    *
*		       - A device server (-d <full device server name>)	    *
*		       - All the device servers on a host (-h <host>)	    *
*		       - All a control system (-a)			    *
*		    A verbose option also exist to inform the user of what  *
*		    the software is doing				    *
*                                                                           *
*    Synopsis : testcs [kad<device server full name>h<host>] [-v]	    *
*                                                                           *
****************************************************************************/


int main(int argc,char **argv)
{
	std::string server_name;
	std::string host_name;
	int option_char,nb_char;
	long k_flag,a_flag,d_flag,h_flag;
	long verbose_flag;
	extern char *optarg;
	extern int optind, opterr, optopt;

// Miscellaneous init.

	k_flag = a_flag = d_flag = h_flag = verbose_flag = False;

// Test the utility option with the GetOpt class

	if (argc == 1)
		errprint(argv[0]);
       
	while ((option_char = getopt(argc, argv, "vkad:h:")) != EOF)
	{
		switch(option_char)
		{

// The kernel option

			case 'k' :
				if ((a_flag == True) || (h_flag == True) ||
				    (d_flag == True))
					errprint(argv[0]);
				else
					k_flag = True;
				break;

// The all control system option

			case 'a' :
				if ((k_flag == True) || (h_flag == True) ||
				    (d_flag == True))
					errprint(argv[0]);
				else
					a_flag = True;
				break;

// The device server option

			case 'd' :
				if ((k_flag == True) || (h_flag == True) ||
				    (a_flag == True))
					errprint(argv[0]);
				else
				{
					server_name = optarg;
					if (server_name[0] == '-')
						errprint(argv[0]);
#ifndef _solaris
					nb_char = std::count(server_name.begin(), server_name.end(), '/');
#else
					nb_char = _sol::count(server_name.begin(), server_name.end(), '/');
#endif
					if (nb_char != 1)
						errprint(argv[0]);
					else
						d_flag = True;
				}
				break;

// The host option

			case 'h' :
				if ((k_flag == True) || (d_flag == True) ||
				    (a_flag == True))
					errprint(argv[0]);
				else
				{
					host_name = optarg;
					if (host_name[0] == '-')
						errprint(argv[0]);
					else
						h_flag = True;
				}
				break;

// The verbose option

			case 'v' : verbose_flag = True;
				   break;

// Other options

			case '?' :
				errprint(argv[0]);
				break;
		}
	}

	if ((h_flag == False) && (d_flag == False) && (k_flag == false) &&
	    (a_flag == False) && (argc == 2))
		errprint(argv[0]);

// Open the database

//	open_db();
	DevLong error;
	db_import(&error);

// Call the right function according to parameter

	if (a_flag == True)
	{
		std::cout << "Testing control system kernel components" << std::endl;
		test_kernel(verbose_flag);
		test_all(verbose_flag);
	}
	else if (k_flag == True)
	{
		test_kernel(verbose_flag);
	}
	else if (h_flag == True)
	{
		std::cout << "Test host : " << host_name << std::endl;
		test_host(host_name,verbose_flag);
	}
	else if (d_flag == True)
	{
		test_ds(server_name,True,True);
	}

// Close database

//	close_db();

}



/****************************************************************************
*                                                                           *
*		Code for test_kernel function                               *
*                        -----------                                        *
*                                                                           *
*    Function rule : To test the kernel of a ESRF stylke control system	    *
*		     This means to test :				    *
*			- The manager					    *
*			- The database server				    *
*			- The data collector server (read and write) if the *
*			  control system  uses a data collector		    *
*		     If a server fails, the test exits			    *
*									    *
*    Argins : - verbose : The verbose flag				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void test_kernel(long verbose)
{
	int ret;
	int dc;
	dc_serv_info *dc_inf;
	int nb_dc_host;

// First, test the Manager.

	KernelServer manager(MANAGER);

	ret = manager.test_server_udp(1);
	if (ret == 0)
	{
		if (verbose == True)
			std::cout << "Manager : UDP version 1 ==> OK" << std::endl;
	}
	else
	{
		std::cout << "Manager : UDP version 1 ==> NOK, leaving test" << std::endl;
//		close_db();
		exit(-1);
	}
	ret = manager.test_server_udp(4);
	if (ret == 0)
	{
		if (verbose == True)
			std::cout << "Manager : UDP version 4 ==> OK" << std::endl;
	}
	else
	{
		std::cout << "Manager : UDP version 4 ==> NOK, leaving test" << std::endl;
//		close_db();
		exit(-1);
	}

// Now, The database server.

	KernelServer database(DATABASE);

// Test database server for UDP protocol for version 1, 2 and 3

	for (int i = 0;i < 3;i++)
	{
		ret = database.test_server_udp(1);
		if (ret == 0)
		{
			if (verbose == True)
				std::cout << "Database server : UDP version " << (i + 1) << " ==> OK" << std::endl;
		}
		else
		{
			if (i == 2)
			{
				std::cout << "Database server : UDP version 3 ==> NOK" << std::endl;
				std::cout << "Do not take care of the previous message if your db server is not release 5.x" << std::endl;
			}
			else
			{
				std::cout << "Database server : UDP version " << (i + 1) << " ==> NOK, leaving test" << std::endl;
//				close_db();
				exit(-1);
			}
		}
	}

// Test database server for TCP protocol for version 1 and 2

	for (int i = 0;i < 3;i++)
	{
		ret = database.test_server_tcp(1);
		if (ret == 0)
		{
			if (verbose == True)
				std::cout << "Database server : TCP version " << (i + 1) << " ==> OK" << std::endl;
		}
		else
		{
			if (i == 2)
			{
				std::cout << "Database server : TCP version 3 ==> NOK" << std::endl;
				std::cout << "Do not take care of the previous message if your db server is not release 5.x" << std::endl;
			}
			else
			{
				std::cout << "Database server : TCP version " << (i + 1) << " ==> NOK, leaving test" << std::endl;
//				close_db();
				exit(-1);
			}
		}
	}

// Database is running, ask it if a data collector is used in this control
// system.

	dc = 2;
	dc = is_there_a_dc(&nb_dc_host,&dc_inf);

	if (dc == True)
	{
		for (int i = 0;i < nb_dc_host;i++)
		{

// Test each dc read server on a dc host

			for (int j = 0;j < dc_inf[i].rd;j++)
			{
				KernelServer dc_rd(DC_RD,j + 1,dc_inf[i].host);

				ret = dc_rd.test_server_tcp(1);
				if (ret == 0)
				{
					if (verbose == True)
						std::cout << "Data collector read server " << (j + 1) << " on " << dc_inf[i].host << " : TCP version 1 ==> OK" << std::endl;
				}
				else
				{
					std::cout << "Data collector read server " << (j + 1) << " on " << dc_inf[i].host << " : TCP version 1 ==> NOK, leaving test" << std::endl;
//					close_db();
					exit(-1);
				}

				ret = dc_rd.test_server_udp(1);
				if (ret == 0)
				{
					if (verbose == True)
						std::cout << "Data collector read server " << (j + 1) << " on " << dc_inf[i].host << " : UDP version 1 ==> OK" << std::endl;
				}
				else
				{
					std::cout << "Data collector read server " << (j + 1) << " on " << dc_inf[i].host << " : UDP version 1 ==> NOK, leaving test" << std::endl;
//					close_db();
					exit(-1);
				}
			}

// Test each dc write server on a dc host
			
			for (int j = 0;j < dc_inf[i].wr;j++)
			{
				KernelServer dc_wr(DC_WR,j + 1,dc_inf[i].host);

				ret = dc_wr.test_server_tcp(1);
				if (ret == 0)
				{
					if (verbose == True)
						std::cout << "Data collector write server " << (j + 1) << " on " << dc_inf[i].host << " : TCP version 1 ==> OK" << std::endl;
				}
				else
				{
					std::cout << "Data collector write server " << (j + 1) << " on " << dc_inf[i].host << " : TCP version 1 ==> NOK, leaving test" << std::endl;
//					close_db();
					exit(-1);
				}

				ret = dc_wr.test_server_udp(1);
				if (ret == 0)
				{
					if (verbose == True)
						std::cout << "Data collector write server " << (j + 1) << " on " << dc_inf[i].host << " : UDP version 1 ==> OK" << std::endl;
				}
				else
				{
					std::cout << "Data collector write server " << (j + 1) << " on " << dc_inf[i].host << " : UDP version 1 ==> NOK, leaving test" << std::endl;
//					close_db();
					exit(-1);
				}
			}
		}

// Test all the system update daemons 

		test_ud(verbose);
		
		free(dc_inf);
	}

}



/****************************************************************************
*                                                                           *
*		Code for is_there_a_dc function                             *
*                        -------------                                      *
*                                                                           *
*    Function rule : To test if a data collector is used in this control    *
*		     system. To do this, we simply try to retrieve from the *
*		     database (which is already tested), some data collector*
*		     resources						    *
*		     These resources are returned to the caller because they*
*		     are used to test the data collector server (if any)    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : - p_nb_dc_host : The number of hosts where the data collector *
*			       is distributed				    *
*             - pp_dc_info : The address where the array of dc_serv_info    *
*			     structures must be stored. There is one struct.*
*			     for each dc host with the dc host name and the *
*			     number of read and write server on this host   *
*			     If there is no dc, the memory is not allocated *
*                                                                           *
*    This function returns True if there is a data collector. Otherwise, it *
*    returns False.							    *
*                                                                           *
****************************************************************************/


int is_there_a_dc(int *p_nb_dc_host,dc_serv_info **pp_dc_info)
{
	DevVarStringArray host_dc;
	db_resource res_host;
	db_resource res_nb[2];
	DevLong error;

// Miscellaneous init.

	host_dc.length = 0;
	host_dc.sequence = NULL;

// Retrieve on which host the dc is running

	res_host.resource_adr = (void *)&host_dc;
	res_host.resource_type = D_VAR_STRINGARR;
	res_host.resource_name = "host";

	if (db_getresource("CLASS/DC/1",&res_host,1,&error))
	{
		std::cout << "Can't get DC resource from database, exiting" << std::endl;
//		close_db();
		exit(-1);
	}

	if (host_dc.length == 0)
		return(False);
	
// Allocate memory to store each dc host info

	if ((*pp_dc_info = (dc_serv_info *)calloc(host_dc.length,sizeof(dc_serv_info))) == (dc_serv_info *)NULL)
	{
		std::cout << "Memory allocation failed (in is_there_a_dc function)" << std::endl;
//		close_db();
		exit(-1);
	}

// Retrieve dc server number on each host

	for (int i = 0;i < host_dc.length;i++)
	{
		int rd = 0;
		int wr = 0;
		std::string resname_rd = host_dc.sequence[i];
		resname_rd = resname_rd + "_rd";
		std::string resname_wr = host_dc.sequence[i];
		resname_wr = resname_wr + "_wr";

		res_nb[0].resource_name = const_cast<char *>(resname_rd.c_str());
		res_nb[0].resource_adr = (void *)&rd;
		res_nb[0].resource_type = D_LONG_TYPE;
		res_nb[1].resource_name = const_cast<char *>(resname_wr.c_str());
		res_nb[1].resource_adr = (void *)&wr;
		res_nb[1].resource_type = D_LONG_TYPE;

		if (db_getresource("CLASS/DC/server_nb",res_nb,2,&error))
		{
			free(*pp_dc_info);
			std::cout << "Can't get DC resource from database, exiting" << std::endl;
//			close_db();
			exit(-1);
		}

		if ((rd == 0) && (wr == 0))
		{
			free(*pp_dc_info);
			return(False);
		}
		else
		{
			strcpy((*pp_dc_info)[i].host,host_dc.sequence[i]);
			(*pp_dc_info)[i].rd = rd;
			(*pp_dc_info)[i].wr = wr;
		}
	}

// Leave function

	*p_nb_dc_host = host_dc.length;
	return(True);

}



/****************************************************************************
*                                                                           *
*		Code for test_ud function                   	            *
*                        -------                                            *
*                                                                           *
*    Function rule : To test all the update daemons device server used on a *
*		     TACO control system. This function is called only with *
*		     the -k option and if the TACO control system use a dc  *
*									    *
*    Argins : - verbose : The verbose flag				    *
*									    *
*    Argout : No argout							    *
*                                                                           *
*    This function returns True if there is a data collector. Otherwise, it *
*    returns False.							    *
*                                                                           *
****************************************************************************/


void test_ud(long verbose)
{
	char **daemon_list;
	long daemon_nb;
	DevLong	error;

// Ask db for the list of update daemon

	if (db_getdspersnamelist("ud_daemon",&daemon_nb,&daemon_list, &error) == DS_OK)
	{
		if (verbose == True)
			std::cout << "Update daemon numbers : " << daemon_nb << std::endl;
		
// Test each update daemons

		for (int i = 0;i < daemon_nb;i++)
		{		
			std::string ds_str = "ud_daemon/";
			ds_str += daemon_list[i];
			test_ds(ds_str,False,verbose);
		}
	}
}




/****************************************************************************
*                                                                           *
*		Code for test_ds function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To test a device server				    *
*		     If it fails, the test exits			    *
*									    *
*    Argins : - full_ds_name : The full device server name		    *
*	      - leave : Flag set to True is program must exit is the ds does*
*		        not answer					    *
*	      - verb : Flag set to True if the verbose mode is requested    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void test_ds(std::string &full_ds_name,long leave,long verb)
{
	std::string ds_name;
	std::string pers_name;
	std::string host_name;
	int ret;
	long i;
	long vers,nb_vers;

// Extract server name and personal name from full device server name
	std::string::size_type pos = full_ds_name.find('/');
	if (pos != std::string::npos)
	{
		ds_name = full_ds_name.substr(0, pos);
		pers_name = full_ds_name.substr(pos + 1);
	}

// Create the object DeviceServer

	DeviceServer ds(ds_name,pers_name);

	nb_vers = ds.get_vers_nb();

// If there is only one version amd this version number is 0, this means
// that for the database point of view, the device server is not running
// (h_name = not_exp or pn = 0)

	if ((nb_vers == 1) && ((vers = ds.get_vers(0)) == 0))
	{
		std::cout << "Device server " << full_ds_name << " defined in database but not started" << std::endl;
		if (leave == True)
		{
//			close_db();
			exit(-1);
		}
		else
			return;
	}

	for (i = 0;i < nb_vers;i++)
	{

// Get a version number

		vers = ds.get_vers(i);
		if (vers == -1)
		{
			std::cout << "Mismatch in " << ds_name << "/" << pers_name << " version number" << std::endl;
			if (leave == True)
			{
//				close_db();
				exit(-1);
			}
			else
				return;
		}

// Test the device server UDP connection

		ret = ds.test_ds_udp(vers);
		if (ret == 0)
		{
			if (verb == True)
				std::cout << "DS " << ds_name << "/" << pers_name << " : UDP version " << vers << " ==> OK" << std::endl;
		}
		else if (ret == 1)
		{
			ds.get_host(host_name);
			if (host_name != "not_exp")
				std::cout << "DS " << ds_name << "/" << pers_name << " defined in database on host " << host_name << " but not started" << std::endl;
			if (leave == True)
			{
//				close_db();
				exit(0);
			}
			else
				return;
		}
		else
		{
			std::cout << "DS " << ds_name << "/" << pers_name << " : UDP version " << vers << " ==> NOK" << std::endl;
			std::cout << "Process ID " << ds.get_pid() << " found in database" << std::endl;
			if (leave == True)
			{
//				close_db();
				exit(-1);
			}
			else
				return;
		}

// Test the device server TCP connection

		ret = ds.test_ds_tcp(vers);
		if (ret == 0)
		{
			if (verb == True)
				std::cout << "DS " << ds_name << "/" << pers_name << " : TCP version " << vers << " ==> OK" << std::endl;
		}
		else
		{
			std::cout << "DS " << ds_name << "/" << pers_name << " : TCP version " << vers << " ==> NOK, leaving test" << std::endl;
			if (leave == True)
			{
//				close_db();
				exit(-1);
			}
			else
				return;
		}
	}
}



/****************************************************************************
*                                                                           *
*		Code for test_host function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To test all the device server processes running on a   *
* 		     host. This is done with a method implemented in the    *
*		     CSHost class					    *
*									    *
*    Argins : - host_name : The host name				    *
*	      - verbose : The verbose flag				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void test_host(std::string &host_name,long verbose)
{
	long ds_nb,alive;

// Create the CSHost object

	CSHost csh(host_name);

// Display message if the host does not answer

	alive = csh.get_alive_flag();
	if (alive == False)
		std::cout << "Host " << host_name << " does not answer to network request !!!" << std::endl;
		
// Get the real device server number on this host

	ds_nb = csh.get_ds_nb();

// Test each device server on this host

	for (int i = 0;i < ds_nb;i++)
		csh.test_ds_on_host(i,verbose);

}



/****************************************************************************
*                                                                           *
*		Code for test_all function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To test all the device server in a control system	    *
*									    *
*    Argins : - verbose : The verbose flag				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void test_all(long verbose)
{
	long ds_process;
	long dead_host;
	long host_nb;

	std::cout << "Getting information from the whole control system." << std::endl;
	std::cout << "On large control systems this may take time !" << std::endl;

// Create the EsrfControlSystem object

	EsrfControlSystem cs;

// Display host and ds process number

	ds_process = cs.get_ds_process_nb();
	host_nb = cs.get_host_nb();
	dead_host = cs.get_dead_nb();

	std::cout << "Control system with " << ds_process << " server process(s) distributed on " << host_nb << " host(s)" << std::endl;
	if (dead_host != 0)
		std::cout << dead_host << " host(s) dont answer to network request !!!!" << std::endl;

// Test each device server process

	cs.test_cs(verbose);

}



/****************************************************************************
*                                                                           *
*		Code for errprint function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To print a brief utility usage message and to exit test*
*									    *
*    Argins : - utility_name : The utility name (logical no !)		    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not returns anything				    *
*                                                                           *
****************************************************************************/


void errprint(const char *utility_name)
{

/* Display utility usage and exit */

	std::cout << "usage: " << utility_name << " [options]" << std::endl;
	std::cout << "       options : " << std::endl;
	std::cout << "                 -k                      : " << std::endl;
        std::cout << "                 -a                      : " << std::endl;
	std::cout << "                 -d full_device_server_name : " << std::endl;
	std::cout << "                 -h host_name               : " << std::endl;
        std::cout << "                 -v                         : verbose" << std::endl;
	exit(-1);

}
