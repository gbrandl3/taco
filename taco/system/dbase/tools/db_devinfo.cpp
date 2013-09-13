/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 ESRF, www.esrf.fr
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
 * File:	db_devinfo.cpp
 *
 * Description: To get device information from the static database.
 * 		Synopsis : db_devinfo [ device name ]
 *
 * Author(s):
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2008-10-13 19:41:24 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
#include <taco_utils.h>


void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " display informations of the device" << std::endl;
	std::cerr << "         options: -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
	std::cerr << "                  -v display the current version" << std::endl;
	exit(1);
}

void version(const char *cmd)
{
	std::cerr << cmd << " version " << VERSION << std::endl;
	exit(0);
}

int main(int argc,char *argv[])
{
	DevLong 	error;
	db_devinfo_call info;
	char **dev_list;
	unsigned int dev_nb;
        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
        int             c;

//
// Argument test and device name structure
//
        while ((c = getopt(argc, argv, "hvn:")) != -1)
                switch (c)
                {
			case 'n':
                                taco_setenv("NETHOST", optarg, 1);
				break;
			case 'v':
				version(argv[0]);
				break;
                	case 'h':
                	case '?':
				usage(argv[0]);
                }

	if (optind != argc - 1)
		usage(argv[0]);

	std::string dev_name(argv[optind]);
	if (dev_name.substr(0, 2) == "//")
	{
		std::string::size_type pos = dev_name.find("/", 2);
		taco_setenv("NETHOST", dev_name.substr(2, pos - 2).c_str(), 1);
		dev_name.erase(0, pos + 1);
	}

#ifdef DEBUG
	std::cout  << "Device name : " << dev_name << std::endl;
#endif 
#ifndef _solaris
	if (std::count(dev_name.begin(), dev_name.end(), '/') != 2)
#else
	if (_sol::count(dev_name.begin(), dev_name.end(), '/') != 2)
#endif /* _solaris */
	{
		std::cerr << "db_devinfo : Bad device name" << std::endl;
		exit(-1);
	}

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		std::cerr << "db_devinfo : Impossible to connect to database server" << std::endl;
		exit(-1);
	}

//
// Check if there is any * used in the device name
//
	bool devexp = (dev_name.find('*') != std::string::npos);
	if (!devexp)
	{
//
// Ask device info to database server
// Display error message if the call fails
//
		if (db_deviceinfo(dev_name.c_str(), &info, &error) == -1)
		{
			if (error == DbErr_DeviceNotDefined)
			{
				std::cerr << "The device " << dev_name << " is not defined in the database" << std::endl;
				exit(-1);
			}
			else
			{
				std::cerr << "The call to database server failed with error " << error << std::endl
					<< "Error message : " << dev_error_str(error) << std::endl;
				exit(-1);
			}
		}

		if (info.device_type == DB_Device)
		{
//
// Display message if the device is not exported
//
			if (info.device_exported == False)
			{
				std::cout << "The device " << dev_name << " is not actually exported" << std::endl;
				exit(-1);
			}
			else
			{
//
// Print results
//
				std::cout << "Device " << dev_name << " belongs to class : " << info.device_class << std::endl
					<< "It is monitored by the server : " << info.server_name <<"/" << info.personal_name 
					<< " version " << info.server_version << std::endl;
				if (strcmp(info.process_name,"unknown") != 0)
				{
					std::cout << "The device server process name is : " << info.process_name;
					if (info.program_num != 0)
						std::cout << " with program number : " << info.program_num;
					std::cout << std::endl;
				}
				std::cout << "The process is running on the computer : " << info.host_name;
				if (info.pid != 0)
					std::cout << " with process ID : " << info.pid;
				std::cout << std::endl;
			}
		}
		else
		{
//
// Display pseudo-device info
//
			std::cout << "Device " << dev_name << " is a pseudo device" << std::endl
				<< "It has been created by a process with PID : " << info.pid << " running on host : " 
				<< info.host_name << std::endl;
		}
	}
	else
	{
//
// Get a list of devices from database
// Display error message if the call fails
//
		if (db_getdevexp(const_cast< char *>(dev_name.c_str()), &dev_list, &dev_nb, &error) == -1)
		{
			std::cerr << "The call to database server failed with error " << error << std::endl
				<< "Error message : " << dev_error_str(error) << std::endl;
			exit(-1);
		}
//
// Display device list
//
		for (int i = 0; i < dev_nb; i++)
			std::cout << dev_list[i] << std::endl;
	}
	return 0;
}
