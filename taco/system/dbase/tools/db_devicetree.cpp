/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2004-2013 Jens Krüger <jkrueger1@users.sf.net>
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
 * File:	db_devicetree.cpp
 *
 * Description: To display at tree of devices on the different hosts
 *		and device types 
 *              Synopsis : db_devicetree
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
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include <taco_utils.h>

long Db_getdsonhost(char *, long *, db_svc **, long *);

typedef	struct _Device
{
	std::string	domain;
	std::string	family;
	std::string	member;
	bool		exported;
} Tdevice;

typedef struct _server
{
	std::string		name;
	std::string		personal_name;
	pid_t			pid;
	unsigned long		program_number;
	std::vector<Tdevice>	devices;
}Tserver;

typedef struct _host
{
	std::string		name;
	std::vector<Tserver>	servers;
}Thost;

typedef std::vector<Thost>	Tnethost;

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " displays all devices in a tree according to their host and servers. " << std::endl;
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
	Tnethost	nethost;
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
	{
		Thost	host;
		host.name = host_list[i];
		nethost.push_back(host);
	}
	db_freedevexp(host_list);

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
					db_svcinfo_call	info;
					if (db_servinfo(server_names[i], pers_names[j], &info, &error) == DS_OK)
					{
						Tserver	server;
						server.name = server_names[i];
						server.personal_name = pers_names[j];
						server.pid = info.pid;
						server.program_number = info.program_num;

						Tnethost::iterator it;
						for (it = nethost.begin(); it != nethost.end(); ++it)
							if (it->name == std::string(info.host_name))
								break;
						if (it == nethost.end())
						{
							Thost host;
							host.name = info.host_name;
							it = nethost.insert(it, host);
						}

						for (int k = 0; k < info.embedded_server_nb; ++k)
						{
							for (int l = 0; l < info.server[k].device_nb; ++l)
							{	
								Tdevice	device;
								device.exported = info.server[k].device[l].exported_flag == 1;
								std::string	dev_name(info.server[k].device[l].dev_name);
								std::string::size_type	pos = dev_name.find('/');

								device.domain = dev_name.substr(0, pos);
								dev_name.erase(0, pos + 1);
								pos = dev_name.find('/');
								device.family = dev_name.substr(0, pos);
								device.member = dev_name.substr(pos + 1);
								server.devices.push_back(device);
							}
						}
						it->servers.push_back(server);
					}
				}
				db_freedevexp(pers_names);
			}
		}
		db_freedevexp(server_names);
	}
	for (Tnethost::iterator it = nethost.begin(); it != nethost.end(); ++it)
	{
		std::cout << it->name << std::endl;
		for (std::vector<Tserver>::iterator s = it->servers.begin(); s != it->servers.end(); ++s)
		{
			std::cout << "\t" << s->name + "/" + s->personal_name << " : " << s->pid << std::endl;
			for (std::vector<Tdevice>::iterator d = s->devices.begin(); d != s->devices.end(); ++d)
				std::cout << "\t\t" << d->domain + "/" + d->family + "/" + d->member << 
					(d->exported ? " : exported" : " : not exported") << std::endl;
		}
	}
	return 0;
}

