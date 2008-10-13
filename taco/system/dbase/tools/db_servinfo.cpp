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
 * File:	db_servinfo.cpp
 *
 * Description: To display device server info
 *		Synopsis : db_servinfo [ full device server name ]
 *
 * Author(s):
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.8 $
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
#include <cstdlib>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <full device server name>" << std::endl;
	std::cerr << "  Diplay device server informations" << std::endl;
	std::cerr << "        options : -h display this message" << std::endl;
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
	DevLong error;
        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
        int             c;

//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"hvn:")) != -1)
                switch (c)
                {
			case 'n':
                                taco_setenv("NETHOST", optarg, 1);
				break;
			case 'v':
				version(argv[0]);
                        case 'h':
                        case '?':
                                usage(argv[0]);
                }
        if (optind != argc - 1)
                usage(argv[0]);

	std::string full_ds_name(argv[optind]);

#ifdef DEBUG
	std::cout  << "Server name : " << full_ds_name << std::endl;
#endif /* DEBUG */
#ifndef _solaris
	if (std::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		std::cerr << "db_servinfo : Bad full device server name" << std::endl;
		exit(-1);
	}

//
// Extract device server and personal name from full device server
// name
//

	std::string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == std::string::npos)
	{
		std::cout << "db_servinfo : Can't split full device server" << std::endl;
		exit(-1);
	}
	
	std::string ds_name(full_ds_name,0,pos);
    	std::string pers_name(full_ds_name.substr(pos + 1)); 
#ifdef DEBUG
	std::cout << "DS name : " << ds_name << std::endl;
	std::cout << "Pers. name : " << pers_name << std::endl;
#endif /* DEBUG */
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		std::cerr << "db_servinfo : Impossible to connect to database server" << std::endl;
		exit(-1);
	}

//
// Ask database server for server info
// Display error message if the call fails
//
	db_svcinfo_call info;
	if (db_servinfo(ds_name.c_str(), pers_name.c_str(), &info, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			std::cerr << "The device server " << full_ds_name << " is not defined in database" << std::endl;
		else
		{
			std::cerr << "The call to database server failed with error " << error << std::endl;
			std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		}
		exit(-1);
	}
//
// Display info sent back by server
//

	std::string pro(info.process_name);
	if (info.pid != 0)
	{
		if (pro != "unknown")
			std::cout << "Device server " << full_ds_name << " is part of the process " << info.process_name << std::endl;
		std::cout << "The process is running on host " << info.host_name << " with PID " << info.pid 
	     		<< " (Program number = " << info.program_num << ")" << std::endl;
	}
	
	for (int i = 0; i < info.embedded_server_nb; i++)
	{
		if (info.embedded_server_nb != 1)
		{
			std::cout << std::endl;
			std::cout << "Device server class : " << info.server[i].server_name << std::endl;
		}
		for (int j = 0; j < info.server[i].device_nb; j++)
		{
			if (info.server[i].device[j].exported_flag == True)
				std::cout << "Device number " << j << " : " << info.server[i].device[j].dev_name 
					<< " exported from host " << info.host_name << std::endl;
			else
				std::cout << "The device " << info.server[i].device[j].dev_name 
					<< " is defined for this server but is not exported" << std::endl;
		}
	}
	return 0;
}


