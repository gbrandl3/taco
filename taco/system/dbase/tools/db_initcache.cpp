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
 * File:	db_initcache.cpp
 *
 * Description: To initialize a database server resource cache. This command 
 *		clears the old cache and (re)initialize it with the contents 
 *		of the RES database table for the wanted domain
 *    		Synopsis : db_initcache [ domain name ] 
 *
 * Author(s):
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2008-10-13 19:01:46 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
/* TACO include file */
#include <API.h>

/* Include files */
#include <string>
#include <iostream>
#include <cstdlib>
#ifdef _solaris
#include <taco_utils.h>
#endif

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <domain name>" << std::endl;
	std::cerr << " Initialize a database server resource cache" << std::endl;
	std::cerr << " This command clears the old cache and (re)initialize it" << std::endl;
	std::cerr << " with the contents of the RES database table for the" << std::endl;
	std::cerr << " wanted domain" << std::endl;
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
        extern char     *optarg;
        extern int      optind,
                        opterr,
                        optopt;
        int             c;


// Argument test and domain name modification
        while((c = getopt(argc, argv, "hvn:")) != -1)
                switch(c)
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
//
// Argument test and device name structure
//
	if (optind != argc - 1)
		usage(argv[0]);

	std::string dom_name(argv[1]);

#ifdef DEBUG
	std::cout  << "Domain name : " << dom_name << std::endl;
#endif 
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		std::cerr << "db_initcache : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Display error message if the call fails
// Init. cache
//
	if (db_initcache(dom_name.c_str(), &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	return 0;
}


