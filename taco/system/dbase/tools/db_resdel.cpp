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
 * File:	db_resdel.cpp
 *
 * Description: To delete a resource from the static database.
 *		Synopsis : db_resdel [ resource name ]
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
#include <cstdlib>
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
#include <taco_utils.h>


void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <resource name>" << std::endl;
	std::cerr << " Delete a resource from the static database." << std::endl;
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
	extern char 	*optarg;
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
	
	std::string full_res_name(argv[optind]);
	std::transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
//
// Test resource name syntax
//
#ifndef _solaris
	if (std::count(full_res_name.begin(), full_res_name.end(), '/') != 3)
#else
	if (_sol::count(full_res_name.begin(), full_res_name.end(), '/') != 3)
#endif /* _solaris */
	{
		std::cerr << "db_resdel : Bad resource name" << std::endl;
		exit(-1);
	}
//
// Extract device name from full resource name
//
	std::string::size_type pos = full_res_name.rfind('/');
	std::string res_name(full_res_name,pos + 1);
	std::string dev_name(full_res_name,0,pos);
#ifdef DEBUG
	std::cout << "Full resource name : " << full_res_name << std::endl;
	std::cout << "Device name : " << dev_name << std::endl;
	std::cout << "Resource name : " << res_name << std::endl;
#endif /* DEBUG */

//
// Test domain name and exit if the resource belongs to the SEC domain
//

	pos = full_res_name.find('/');
	std::string domain(full_res_name,0,pos);
	if (domain == "sec")
	{
		std::cout << "db_resdel : SEC is not a authorized domain name" << std::endl;
		exit(-1);
	}
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		std::cerr << "db_resdel : Impossible to connect to database server" << std::endl;
		exit(-1);
	}

//
// Ask database server to delete resource
// Display error message if the call fails
//
	unsigned int res_nb = 1;
	char *tmp = const_cast<char *>(res_name.c_str());
	if (db_delresource(const_cast<char *>(dev_name.c_str()), &tmp, 1, &error) == -1)
	{
		if (error == DbErr_ResourceNotDefined)
		{
			std::cerr << "The resource " << full_res_name << " does not exist in the database" << std::endl;
			exit(-1);
		}
		else
		{
			std::cerr << "The call to database server failed with error " << error << std::endl;
			std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			exit(-1);
		}
	}
	return 0;
}


