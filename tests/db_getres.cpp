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
 * File:        db_resget.cpp
 *
 * Description: Code for db_resget test program
 * 		Command rule : To get a resource from the static database.          
 *		Synopsis : db_resget <resource name> 
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:47 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <TACOExtensions.h>

int main(int argc, char *argv[])
{
	DevLong error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		std::cerr << "usage : " << *argv << " resource_name" << std::endl;
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
	if (db_import(&error) == DS_NOTOK)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	try
	{
		DevLong tmp = ::TACO::queryResource<DevLong>(dev_name, res_name); // , 255);
		std::cout << tmp << std::endl;
	}
	catch ( ... )
	{
		std::cerr << "The call to database server failed" << std::endl;
		exit(-1);
	}
	return 0;
}
