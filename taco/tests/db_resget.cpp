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
 * File:        db_resget.cpp
 *
 * Description: Code for db_resget test program
 * 		Command rule : To get a resource from the static database.          
 *		Synopsis : db_resget <resource name> 
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.7 $
 *
 * Date:        $Date: 2008-10-15 15:57:44 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

#include <unistd.h>

#include <taco_utils.h>

void usage(const char *argv)
{
	std::cerr << "usage : " << argv << " [options] resource_name" << std::endl;
	std::cerr << "options : -h         display this information" << std::endl;
	std::cerr << "          -n nethost set nethost environment" << std::endl;
	std::cerr << "          -t         enable time measurment" << std::endl;
	exit(-1);
}


double elapsedTime(struct timeval first, struct timeval second)
{
        if (first.tv_usec > second.tv_usec)
        {
                second.tv_usec += 1000000;
                second.tv_sec--;
        }
        double lapsed = (second.tv_usec - first.tv_usec) / 1000.;
        lapsed += 1000. * (second.tv_sec - first.tv_sec);
	return lapsed;
}




int main(int argc, char *argv[])
{
	DevLong error;
	int 	c;
	bool	b_timing(false);
//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"hn:t")) != -1)
                switch (c)
                {
                        case 'n':
				taco_setenv("NETHOST", optarg, 1);
				break;
			case 't':
				++b_timing;
				break;
                        case 'h':
                        case '?':
                                usage(argv[0]);
                                break;
                }
        if (optind != argc - 1)
                usage(argv[0]);

	std::string full_res_name(argv[optind]);
	std::transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
//
// Test resource name syntax
//
	int  max_slashes = (full_res_name.substr(0, 2) == "//") ? 6 : 3;
	if (std::count(full_res_name.begin(), full_res_name.end(), '/') != max_slashes)
	{
		std::cerr << *argv << " : Bad resource name" << std::endl;
		exit(-1);
	}
	std::string nethost("");
	if (max_slashes == 6)
	{
		std::string::size_type pos = full_res_name.find('/', 2);
		if (pos != std::string::npos)
			nethost = full_res_name.substr(2, pos - 2);
	}
//
// Extract device name from full resource name
//
	std::string::size_type pos = full_res_name.rfind('/');
	std::string res_name(full_res_name, pos + 1);
	std::string dev_name(full_res_name, 0, pos);
#ifdef DEBUG
	std::cerr << "Full resource name : " << full_res_name << std::endl;
	std::cerr << "Device name : " << dev_name << std::endl;
	std::cerr << "Resource name : " << res_name << std::endl;
#endif /* DEBUG */
//
// Test domain name and exit if the resource belongs to the SEC domain
//
	pos = full_res_name.find('/');
	std::string domain(full_res_name, 0, pos);
	if (domain == "sec")
	{
		std::cerr << *argv << " : SEC is not a authorized domain name" << std::endl;
		exit(-1);
	}
//
// Connect to database server
//
	if (nethost.empty() && db_import(&error) == DS_NOTOK)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	unsigned int res_nb = 1;
	DevVarStringArray tmp = {0, NULL};
	db_resource resTable [] = {{ res_name.c_str(), D_VAR_STRINGARR, &tmp},};

        struct timeval  first,
                        second;
        struct timezone tzp;
        DevLong         err;

        gettimeofday (&first, &tzp);
	long res = db_getresource(const_cast<char *>(dev_name.c_str()), resTable, 1, &error);
        gettimeofday (&second, &tzp);
	if (b_timing)
        	printf("%.3f ms\n", elapsedTime(first, second));

	if (res == -1)
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
	for (unsigned int i = 0; i < tmp.length; ++i)
	{
		std::cout << tmp.sequence[i]; 
		if (i < tmp.length - 1)
			std::cout << ",";
	}
	std::cout << std::endl;
	return 0;
}
