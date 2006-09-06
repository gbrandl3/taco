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
 * File:	db_devres.cpp
 *
 * Description: To get device resources from the static database.
 *		Synopsis : db_devres [ device name ]
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-09-06 18:34:15 $
 */

/* TACO include file */

#include "config.h"
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " displays the resources of the device" << std::endl;
	std::cerr << "         options: -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
        exit(1);
}

int main(int argc,char *argv[])
{
	long error;
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
    	if (optind != argc - 1)
		usage(argv[0]);

	string dev_name(argv[optind]);

#ifdef DEBUG
	cout  << "Device name : " << dev_name << endl;
#endif 
#ifndef _solaris
	if (std::count(dev_name.begin(), dev_name.end(), '/') != 2)
#else
	if (_sol::count(dev_name.begin(), dev_name.end(), '/') != 2)
#endif /* _solaris */
	{
		cerr << "db_devres : Bad device name" << endl;
		exit(-1);
	}

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_devres : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Ask device resources to database server
// Display error message if the call fails
//
    char *tmp = const_cast<char *>(dev_name.c_str());
    char **res_list;
    long res_nb;
    if (db_deviceres(1, &tmp, &res_nb, &res_list, &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
		exit(-1);
	}

//
// Print device resources
//

	if (res_nb == 0)
		cout << "The device " << dev_name << " does not have any resource defined in the database"<< endl;
	else
	for (int i = 0; i < res_nb; i++)
			cout << res_list[i] << endl;
    	return 0;
}
