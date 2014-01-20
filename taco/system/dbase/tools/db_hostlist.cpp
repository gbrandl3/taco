/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2005-2014 Jens Krüger <jkrueger1@users.sf.net>
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
 * File:	db_hostlist.cpp
 *
 * Description: To get list of all host running TACO servers 
 *              Synopsis : db_hostlist 
 *
 * Author(s):
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.6 $
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
#include <cstdlib>
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options]" << std::endl;
	std::cerr << " displays the list of all host used by a device server" << std::endl;
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

int main(int argc, char **argv)
{
	DevLong 	error;
        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
        int             c;
//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"hvn:")) != -1)
        {
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
        }
    	if (optind != argc)
		usage(argv[0]);

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
	long n_host;
	char **host_list; 
	if (db_gethostlist(&n_host, &host_list, &error) == DS_NOTOK)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
	}
	for (int i = 0; i < n_host; i++)
		std::cout << host_list[i] << std::endl;
	db_freedevexp(host_list);
	return 0;
}

