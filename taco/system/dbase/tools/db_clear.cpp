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
 * Version:     $Revision: 1.3 $
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
	std::cerr << " deletes all entries in the database. " << std::endl;
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

void delete_allresources()
{
	long 	domain_nb;
	char	**domain_list;
	DevLong	error;

	if (db_getresdomainlist(&domain_nb, &domain_list, &error) == DS_OK)
	{
		long	family_nb(0);
		char 	**family_list(NULL);
		char 	*domain;
		for (int i = 0; i < domain_nb; ++i)
		{
			domain = domain_list[i];
			if (db_getresfamilylist(domain, &family_nb, &family_list, &error) == DS_OK)
			{
				long member_nb(0);
				char **member_list(NULL);
				char *family;
				for (int j = 0; j < family_nb; ++j)
				{
					family = family_list[j];
					if (db_getresmemberlist(domain, family, &member_nb, &member_list, &error) == DS_OK)	
					{
						long reso_nb(0);
						char **reso_list(NULL);
						char *member;
						for (int k = 0; k < member_nb; ++k)
						{
							member = member_list[k];
							std::string dev_name = domain;
							dev_name += '/';
							dev_name += family;
							dev_name += '/';
							dev_name += member;

							db_devinfo_call info;
							if (db_getresresolist(domain, family, member, &reso_nb, 
								&reso_list, &error) == DS_OK)
							{
								std::cout << dev_name << std::endl;
								for (int n = 0; n < reso_nb; ++n)
									if (db_delresource(const_cast<char *>(dev_name.c_str()), 
										&reso_list[n], 1, &error) == DS_NOTOK)
										std::cout << "ERROR : " << dev_error_str(error) << std::endl;
								db_freedevexp(reso_list);
							}
						}
						db_freedevexp(member_list);
					}
				}
				db_freedevexp(family_list);
			}
		}
		db_freedevexp(domain_list);
	}
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
					std::cout << server_names[i] << "/" << pers_names[j];
					if (db_servdelete(server_names[i], pers_names[j], true, &error) == DS_NOTOK)
						std::cout << "not"; 
					std::cout << " deleted" << std::endl;
				}
				db_freedevexp(pers_names);
			}
		}
		db_freedevexp(server_names);
	}

	delete_allresources();
	return 0;
}

