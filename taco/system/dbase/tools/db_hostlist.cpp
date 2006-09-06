/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2005 Jens Krüger <jkrueger1@users.sf.net>
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
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-06 18:34:15 $
 */

#include "config.h"
/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <cstdlib>
void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options]" << std::endl;
	std::cerr << " displays the list of all host used by a device server" << std::endl;
	std::cerr << "         options: -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
        exit(1);
}

int main(int argc, char **argv)
{
	long 		error;
        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
        int             c;
//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"hn:")) != -1)
        {
                switch (c)
                {
			case 'n':
				{
					char s[160];
					snprintf(s, sizeof(s), "NETHOST=%s", optarg);
					putenv(s);
				}
			break;
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

