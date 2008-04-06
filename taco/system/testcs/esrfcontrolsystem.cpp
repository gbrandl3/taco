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
 * File:        esrfcontrolsystem.cpp
 *
 * Project:     System test
 *
 * Description: implementation for EsrfControlSystem class 
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

#include <stdlib.h>

// ESRF include files

#include <API.h>

// GNU C++ classes include files

#include <string>
#include <iostream>

// testcs classes include files

#include <CSHost.h>
#include <EsrfControlSystem.h>

/****************************************************************************
*                                                                           *
*		Code for the EsrfControlSystem class  			    *
*                            -----------------                              *
*                                                                           *
*    Class rule : This class has been implemented for the testcs utility    *
* 	 	  It is used only when this utility is started with the     *
*		  -a option. The main function of this class is to test     *
*		  a whole ESRF control system.				    *
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

EsrfControlSystem::EsrfControlSystem()
{
	char **db_host_list;
	long tmp_nb_ds = 0;
	long alive;

#ifdef DEBUG
	std::cout << "EsrfControlSystem default constructor" << std::endl;
#endif // DEBUG

// Ask database for the host number and the host name list
	DevLong error;
	db_gethostlist(&host_nb, &db_host_list, &error);

#ifdef DEBUG
	std::cout << "Host number : " << host_nb << std::endl;
	for (int i = 0;i < host_nb;i++)
		std::cout << "Host name : " << db_host_list[i] << std::endl;
#endif // DEBUG

// Leave constructor if there is no host used in this control system !

	if (host_nb == 0)
	{
		host_array = NULL;
		ds_process = 0;
		return;
	}

// Sort the host name array

	sort_name(db_host_list,host_nb);

// Create the array of CSHost objects

	host_array = new CSHost [host_nb];
	if (host_array == NULL)
	{
		std::cout << "EsrfControlSystem class memory allocation error, exiting" << std::endl;
//		close_db();
		exit(-1);
	}

	host_dead = 0;
	for (int i = 0;i < host_nb;i++)
	{
		std::string h_name;

// Build a temporary CSHost object correctly constructed

		h_name = db_host_list[i];
		std::cout << "Getting information for : " << h_name << std::endl;
		CSHost tmp_host(h_name);

// Copy for object into the object array of CSHost

		host_array[i] = tmp_host;

		tmp_nb_ds = tmp_nb_ds + tmp_host.get_ds_nb();

// Count the number of host which does not answer to network request

		alive = tmp_host.get_alive_flag();
		if (alive == False)
			host_dead++;
	}

// Init the ds process field

	ds_process = tmp_nb_ds;

// Free the memory allocated by the database function

	for (int i = 0;i < host_nb;i++)
		free(db_host_list[i]);
	free(db_host_list);
}

void sort_name(char **tab,long array_size)
{
        int gap,i,j;
        char *temp;

	for (gap = array_size/2;gap > 0;gap /= 2)
	{
		for (i = gap;i < array_size;i++)
		{
			for (j = i - gap;j >= 0;j -= gap)
			{
				if (strcmp(tab[j],tab[j + gap]) <= 0)
					break;
				else
				{
					temp = tab[j];
					tab[j] = tab[j + gap];
					tab[j + gap] = temp;
				}
			}
		}
	}
}



/****************************************************************************
*                                                                           *
*		Code for copy constructor function	                    *
*                        ----------------                                   *
*                                                                           *
*    Function rule : This functions is the copy constructor		    *
*									    *
*    Argins : - k : A reference to the other EsrfControlSystem object  	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function is a constructor and does not returns anything	    *
*                                                                           *
****************************************************************************/

EsrfControlSystem::EsrfControlSystem(const EsrfControlSystem &k)
{
#ifdef DEBUG
	std::cout << "EsrfControlSystem copy constructor" << std::endl;
#endif // DEBUG

	ds_process = k.ds_process;
	host_nb = k.host_nb;
	if (host_array != NULL)
	{
		host_array = new CSHost [host_nb];
		if (host_array == NULL)
		{
			std::cout << "EsrfControlSystem class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < host_nb;i++)
			host_array[i] = k.host_array[i];
	}
	else
		host_array = NULL;
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

EsrfControlSystem::~EsrfControlSystem()
{
#ifdef DEBUG
	std::cout << "EsrfControlSystem destructor" << std::endl;
#endif // DEBUG
	delete [] host_array;
}



/****************************************************************************
*                                                                           *
*		Code for = operator overloading 	                    *
*                        ----------                                         *
*                                                                           *
*    Function rule : This functions is the = operator overloading. This     *
*		     allows a C++ program which uses this class to use the  *
*     		     = operator between two objects of the EsrfControlSystem*
*		     class						    *
*									    *
*    Argins : - k : A reference to the other EsrfControlSystem object	    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns a reference to a EsrfControlSystem object	    *
*                                                                           *
****************************************************************************/

EsrfControlSystem &EsrfControlSystem::operator=(const EsrfControlSystem &k)
{
#ifdef DEBUG
	std::cout << "EsrfControlSystem = operator overloading function" << std::endl;
#endif // DEBUG

	this->host_nb = k.host_nb;
	this->ds_process = k.ds_process;
	if (k.host_array != NULL)
	{
		this->host_array = new CSHost [this->host_nb];	
		if (this->host_array == NULL)
		{
			std::cout << "EsrfControlSystem class memory allocation error, exiting" << std::endl;
//			close_db();
			exit(-1);
		}
		for (int i = 0;i < this->host_nb;i++)
			this->host_array[i] = k.host_array[i];
	}
	else
		this->host_array = NULL;
	return(*this);
}



/****************************************************************************
*                                                                           *
*		Code for get_ds_process_nb method 	                    *
*                        -----------------                                  *
*                                                                           *
*    Function rule : To returns the number of device server processes       *
*		     registered in a control system			    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the number of device server registered in a      *
*    control system							    *
*                                                                           *
****************************************************************************/

long EsrfControlSystem::get_ds_process_nb() const
{
	return(this->ds_process);
}



/****************************************************************************
*                                                                           *
*		Code for get_host_nb method	 	                    *
*                        -----------                                        *
*                                                                           *
*    Function rule : To returns the host number in a control system	    *
*									    *
*    Argins : No argin(s)						    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This methos returns the number oh host involved in the control system  *
*                                                                           *
****************************************************************************/

long EsrfControlSystem::get_host_nb() const
{
	return(this->host_nb);
}



/****************************************************************************
*                                                                           *
*		Code for get_dead_nb method 	                            *
*                        -----------                                        *
*                                                                           *
*    Function rule : To returns the number of host which dont answer to     *
*		     network request					    *
*									    *
*    Argins : No argins							    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function returns the number of host(s) which dont answer to net.  *
*    request								    *
*                                                                           *
****************************************************************************/

long EsrfControlSystem::get_dead_nb() const
{
	return(this->host_dead);
}



/****************************************************************************
*                                                                           *
*		Code for test_cs method	 	     	                    *
*                        -------     	                                    *
*                                                                           *
*    Function rule : To test all the device server processes registered in  *
*		     a control system					    *
*									    *
*    Argins : - verbose : The verbose flag				    *
*									    *
*    Argout : No argouts						    *
*                                                                           *
*    This function does not return anything				    *
*                                                                           *
****************************************************************************/

void EsrfControlSystem::test_cs(const long verbose) const
{
	long nb_server_host;
	long alive;
	std::string host_name;

// A loop on each host in the control system


	for (int i = 0;i < host_nb;i++)
	{

// Get the host alive flag

		alive = host_array[i].get_alive_flag();
		if (alive == False)
		{
			std::cout << std::endl << "Host : " << host_name << " does not answer to network request" << std::endl << std::endl;
			continue;
		}

// Get the number of server running on this host and the host name

		nb_server_host = host_array[i].get_ds_nb();
		host_array[i].get_host_name(host_name);

		std::cout << "Testing device server(s) running on " << host_name << std::endl;

// Test a device server

		for (int j = 0;j < nb_server_host;j++)
			host_array[i].test_ds_on_host(j,verbose);

	}
}
