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
 * File:        db_resput.cpp
 *
 * Description: Code for db_resput test program
 *		Command rule : To put a resource into the static database.          
 * 		Synopsis : db_resput <resource name> <resource value> [<resource value> [<resourcer value>] ... ]
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2008-04-30 13:42:47 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
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
	DevVarStringArray       server_list = {0, NULL};
	DevLong error;
//
// Argument test and device name structure
//
	if (argc < 3)
	{
		std::cerr << "usage : " << *argv << " resource_name resource_value [resource_value [resource_value] ... ]" << std::endl;
		exit(-1);
	}

	std::string full_res_name(argv[1]);
	std::transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
//
// Test resource name syntax
//
	int  max_slashes = (full_res_name.substr(0, 2) == "//") ? 6 : 3;
	if (std::count(full_res_name.begin(), full_res_name.end(), '/') != max_slashes)
	{
		std::cerr << *argv << " : Bad resource name" << std::endl;
		exit(-1);
	}
//
// Extract device name from full resource name
//
	std::string::size_type pos = full_res_name.rfind('/');
	std::string res_name(full_res_name, pos + 1);
	std::string dev_name(full_res_name, 0, pos);
#ifdef DEBUG
	std::cerr << "Full resource name : " << full_res_name << std::endl;
	std::cerr << "Device name : " << dev_name << std::endl;
	std::cerr << "Resource name : " << res_name << std::endl;
#endif /* DEBUG */
//
// Test domain name and exit if the resource belongs to the SEC domain
//
	pos = full_res_name.find('/');
	std::string domain(full_res_name, 0, pos);
	if (domain == "sec")
	{
		std::cerr << *argv << " : SEC is not a authorized domain name" << std::endl;
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
	db_resource             server = {res_name.c_str(), D_VAR_STRINGARR, &server_list};
	server_list.length = argc - 2;
	server_list.sequence = new char *[server_list.length];
	for (int i = 0; i < server_list.length; ++i)
	{
		server_list.sequence[i] = new char[strlen(argv[2 + i]) + 1];
		strcpy(server_list.sequence[i], argv[2 + i]);
	}
	if (db_putresource(const_cast<char *>(dev_name.c_str()), &server, 1, &error) != DS_OK)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	return 0;
}
