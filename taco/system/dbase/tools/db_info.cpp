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
 * File:	db_info.cpp
 *
 * Description: To get device and resources informations from the static database 
 *		Synopsis : db_info
 *
 * Author(s):
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.7 $
 *
 * Date:        $Date: 2008-10-13 19:41:24 $
 */
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
/* TACO include file */
#include <API.h>

/* Include files */
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options]" << std::endl;
        std::cerr << " displays statistics information about the use of the database" << std::endl;
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
	db_stat_call inf;
	time_t sec;
	char *ti;
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

	if (optind != argc)
		usage(argv[0]);
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << "db_info : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server for informations
// Display error message if the call fails
//
	if (db_stat(&inf, &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
//
// Get time
//
	sec = time((time_t *)0);
	ti = ctime(&sec);

//
// Convert domain name to upper case letter
//
	for (int i = 0; i < inf.dev_domain_nb; i++)
	{
		int l = strlen(inf.dev_domain[i].dom_name);
		for (int j = 0; j < l; j++)
			inf.dev_domain[i].dom_name[j] = toupper(inf.dev_domain[i].dom_name[j]);
	}
	for (int i = 0; i < inf.res_domain_nb; i++)
	{
		int l = strlen(inf.res_domain[i].dom_name);
		for (int j = 0; j < l; j++)
			inf.res_domain[i].dom_name[j] = toupper(inf.res_domain[i].dom_name[j]);
	}
//
// Displays info
//
	std::cout << "\t\t" << ti;
	std::cout << "\t\tDEVICE STATISTICS" << std::endl << std::endl;
	std::cout << inf.dev_defined << " devices are defined in database" << std::endl;
	std::cout << inf.dev_exported << " of the defined devices are actually exported:" << std::endl;
	for (int i = 0; i < inf.dev_domain_nb; i++)
		std::cout << "    " << inf.dev_domain[i].dom_elt << " for the " << inf.dev_domain[i].dom_name << " domain" << std::endl;
	std::cout << inf.psdev_defined << " pseudo devices are defined in the database" << std::endl;
	std::cout << std::endl;
	std::cout << "\t\tRESOURCE STATISTICS" << std::endl << std::endl;
	std::cout << inf.res_number << " resources are defined in database:" << std::endl;
	for (int i = 0; i < inf.res_domain_nb; i++)
		std::cout << "    " << inf.res_domain[i].dom_elt << " resources for the " << inf.res_domain[i].dom_name << " domain" << std::endl;
	return 0;
}


