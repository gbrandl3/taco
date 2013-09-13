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
 * File:	db_update.cpp
 *
 * Description: Read resource and device informations from the file and put 
 *		these into the database.
 *		Synopsis : db_update <file name>
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
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <taco_utils.h>

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <file name>" << std::endl;
	std::cerr << " Read resource and device informations from the file" << std::endl;
	std::cerr << " and put them into the database" << std::endl;
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
	DevLong error;
	char 	*ptr;
	long 	nb_dev,
        	nb_res,
        	err_line,
        	dev_err;
	char 	**dev_def,
		**res_def;
	long 	sec = False;
	char 	*pa;
	long 	pass = True;
	char 	*answer;
        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
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

	std::string file_name(argv[optind]);

//
// Get environment variable
//
	std::string base("");
	if ((ptr = (char *)getenv("RES_BASE_DIR")) != NULL)
	{
		base = ptr;
		base += '/';
	}
//
// Build real file name path
//
	file_name.insert(0, base);
#ifdef DEBUG
	std::cout  << "File name : " << file_name << std::endl;
#endif /* DEBUG */
//
// Connect to database server
//
	if (db_import(&error) == DS_NOTOK)
	{
		std::cerr << "db_update : Impossible to connect to database server" << std::endl;
		exit(-1);
	}

//
// Analyse resource file
// Display error message if the analysis fails
//
	if (db_analyze_data(Db_File, file_name.c_str(), &nb_dev, &dev_def, &nb_res, &res_def, &err_line, &error) == DS_NOTOK)
	{
		if (err_line != 0)
		{
			if (error == DbErr_BadDevSyntax)
				std::cerr << "Error in device definition" << std::endl;
			else
			{
				std::cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << std::endl;
				std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			}
		}
		else
		{
			std::cerr << "File analysis failed with error " << error << std::endl;
	    		std::cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << std::endl;
			std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		}
		exit(-1);
	}
#ifdef DEBUG
	std::cout << "File analysis is OK" << std::endl;

	std::cout << nb_dev << " device list defined" << std::endl;	
	for (int i = 0; i < nb_dev; i++)
		std::cout << dev_def[i] << std::endl;
	std::cout << nb_res << " resources defined" << std::endl;
	for (int i = 0; i < nb_res; i++)
		std::cout << res_def[i] << std::endl;
#endif /* DEBUG */

//
// Check if there is any security resources
//
	for (int i = 0; i < nb_res; i++)
	{
		std::string str(res_def[i]);
		std::string::size_type pos;

		if ((pos = str.find('/')) == std::string::npos)
		{
			std::cerr << "Wrong resource syntax !!!, exiting" << std::endl;
			exit(-1);
		}
	
		std::string domain(str,0,pos);					
		if (domain == "sec")
		{
			sec = True;
			break;
		}
	}
//
// If some security resources are defined, check if a password is used
//

	if (sec == True)
	{
		if (db_secpass(&pa, &error) == DS_NOTOK)
		{
			if (error == DbErr_NoPassword)
				pass = False;
			else
			{
				std::cerr << "Can't retrieve the database defined password, update not allowed" << std::endl;
				exit(-1);
			}
		}
		
		if (pass == True)
		{
			answer = (char *)getpass("Security passwd : ");
			if (strcmp(answer,pa) != 0)
			{
				std::cout << "Sorry, wrong password. Update not allowed" << std::endl;
				std::cout << "Hint : Remove security resources from file" << std::endl;
				exit(-1);
			}
		}
	}
//
// Update device list in db
//

	if (nb_dev != 0)
	{
//
// Display message if this call fails
//
		if (db_upddev(nb_dev, dev_def, &dev_err, &error) == DS_NOTOK)
		{
			if (dev_err != 0)
			{
				std::cerr << "Error in device definition number " << dev_err << ", error = " << error << std::endl;
				std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			}
			else
			{
				std::cerr << "Device(s) update call failed with error " << error << std::endl;
				std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			}
			exit(-1);
		}
	}
	
#ifdef DEBUG
	std::cout << "Update device successfull" << std::endl;
#endif /* DEBUG */

//
// Update resources in db
//

	if (nb_res != 0)
	{
//
// Display message if this call fails
//
		if (db_updres(nb_res, res_def, &dev_err, &error) == DS_NOTOK)
		{
			if (dev_err != 0)
			{
				std::cerr << "Error in resource definition number " << dev_err << ", error = " << error << std::endl;
				std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			}
			else
			{
				std::cerr << "Resource(s) update call failed with error " << error << std::endl;
				std::cerr << "Error message : " << dev_error_str(error) << std::endl;
			}
		}
	}	
	return 0;
}
