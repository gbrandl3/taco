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
 * File:	db_devicelist.cpp
 *
 * Description: To get list of devices from a special nethost
 *              Synopsis : db_devicelist
 *
 * Author(s):	Jens Krüger
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
#include <cstdlib>
#include <iostream>
#include <string>
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " displays all devices according to their servers. " << std::endl;
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

int main(int argc, char **argv)
{
	DevLong 	error;
	extern int 	optopt;
	extern int	optind;
	extern char 	*optarg;
	int 		c;

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
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			break;
		}
	}
    	if (optind != argc)
		usage(argv[0]);

//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << *argv << " : Impossible to connect to database server " << error << std::endl;
		exit(-1);
	}

	long	n_server;
	char	**server_names;
	if (db_getdsserverlist(&n_server, &server_names, &error) == DS_OK)
	{
		for (int i = 0; i < n_server; ++i)
		{
			long	n_pers;
			char	**pers_names;
			if (db_getdspersnamelist(server_names[i], &n_pers, &pers_names, &error) == DS_OK)
			{
				for (int j = 0; j < n_pers; ++j)
				{
					std::cout << server_names[i] << "/" << pers_names[j] << " : ";
					db_svcinfo_call	info;

					if (db_servinfo(server_names[i], pers_names[j], &info, &error) == DS_OK)
						for (int k = 0; k < info.embedded_server_nb; ++k)
							for (int l = 0; l < info.server[k].device_nb; ++l)
								std::cout << std::endl << "\t" << info.server[k].device[l].dev_name;
					std::cout << std::endl;
				}
				db_freedevexp(pers_names);
			}
		}
		db_freedevexp(server_names);
	}
	return 0;
}

