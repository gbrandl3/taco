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
 * File:        db_devexp.cpp
 *
 * Description: Code for db_devexp command                                  
 *		Command rule : To export a device
 *		Synopsis : db_devexp <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:46 $
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

/****************************************************************************
*                                                                           
*                                                                           
****************************************************************************/
int main(int argc, char *argv[])
{
	DevLong error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		std::cerr << "usage : " << *argv << " device name" << std::endl;
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
	Db_devinf	devinfo;

	devinfo->device_name = argv[1];
	devinfo->host_name = "localhost";
	devinfo->device_type = "test_device";
	devinfo->device_class = "test_class";
	devinfo->pn = 99999;
	devinfo->vn = 1;
	devinfo->proc_name = "taco_test_server";	

	if (db_dev_export(devinfo, 1, &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		return 1;
	}
	return 0;
}
