/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 ESRF, www.esrf.fr
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
 * File:        db_devperslist.cpp
 *
 * Description: Code for db_devperslist test command                                  
 *		Command rule : To get a list of personal names for a device server from 
 *		the static database.          
 *		Synopsis : db_devperslist <server name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:47 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

int main(int argc, char *argv[])
{
	DevLong error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		std::cerr << "usage : " << *argv << " server " << std::endl;
		exit(-1);
	}
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	long res_nb;
	char **tmp; 
	if (db_getdspersnamelist(argv[1], &res_nb, &tmp, &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		return 1;
	}
	for (int i = 0; i < res_nb; i++)
    		std::cout << tmp[i] << std::endl;

	db_freedevexp(tmp);
	return 0;
}
