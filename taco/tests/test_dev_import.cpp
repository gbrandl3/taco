/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 ESRF, www.esrf.fr
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
 * File:        test_dev_import.cpp
 *
 * Description: Code for test_dev_import test program
 *              Command rule : To import a device from the TACO system. 
 *              Synopsis : test_dev_import <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-18 22:34:41 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/* TACO include file */
#include <API.h>
#include <DevSec.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

int main(int argc, char *argv[])
{
	long error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		std::cerr << "usage : " << *argv << " device_name" << std::endl;
		exit(-1);
	}

	std::string full_name(argv[1]);
	std::transform(full_name.begin(), full_name.end(), full_name.begin(), ::tolower);
//
// Test resource name syntax
//
	int  max_slashes = (full_name.substr(0, 2) == "//") ? 5 : 2;
	if (std::count(full_name.begin(), full_name.end(), '/') != max_slashes)
	{
		std::cerr << *argv << " : Bad resource name" << std::endl;
		exit(-1);

	}
//
// Ask system to get the handle on specific device
// Display error message if the call fails
//
	devserver	ds;
	if (dev_import(const_cast<char *>(full_name.c_str()), WRITE_ACCESS, &ds, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	std::cout << ds->device_name << std::endl;
	std::cout << ds->device_class << std::endl;
	std::cout << ds->device_type << std::endl;
	if (dev_free(ds, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	return 0;
}
