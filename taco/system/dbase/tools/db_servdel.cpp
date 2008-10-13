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
 * File:	db_servdel.cpp
 *
 * Description: To delete all the devices and their resources from the database.
 *              The -r option is used if the user does not want device resources 
 *		to be also deleted
 *		Synopsis : db_servdel [-r] <full server name>
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
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <full device server name>" << std::endl;
	std::cerr << "  Delete a server, all its devices and optional their resources" << std::endl;
	std::cerr << "        options : -r all resources of the devices deleted" << std::endl;
	std::cerr << "                  -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
	std::cerr << "                  -v display the current version" << std::endl;
	exit(-1);
}

void version(const char *cmd)
{
	std::cerr << cmd << " version " << VERSION << std::endl;
	exit(0);
}

int main(int argc,char *argv[])
{
	DevLong error;
	long del_res = True;
	long opt = False;
	extern int optind;
	int c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"hrvn:")) != -1)
		switch (c)
		{
			case 'n':
                                taco_setenv("NETHOST", optarg, 1);
				break;
			case 'r':
				del_res = False;
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
	std::cout  << "Full device server name : " << full_ds_name << std::endl;
#endif 
#ifndef _solaris
        if (std::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		std::cerr << "db_servdel : Bad full device server name" << std::endl;
		exit(-1);
	}
//
// Extract device server and personal name from full device server name
//
	std::string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == std::string::npos)
	{
		std::cout << "db_servdel : Can't split full device server" << std::endl;
		exit(-1);
	}
	
	std::string ds_name(full_ds_name,0,pos);
	std::string pers_name(full_ds_name.substr(pos + 1 ));
	
#ifdef DEBUG
	std::cout << "DS name : " << ds_name << std::endl;
	std::cout << "Pers. name : " << pers_name << std::endl;
#endif /* DEBUG */

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		std::cerr << "db_servdel : Impossible to connect to database server" << std::endl;
		exit(-1);
	}

//
// Ask database server to remove device
// Display error message if the call fails
//
	if (db_servdelete(ds_name.c_str(), pers_name.c_str(), del_res, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			std::cerr << "Server " << full_ds_name << " does not have any device in database" << std::endl;
		else
		{
			std::cerr << "The call to database server failed with error " << error << std::endl;
			std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		}
		exit(-1);
	}
	return 0;
}
