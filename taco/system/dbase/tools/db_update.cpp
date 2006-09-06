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
 * File:	db_update.cpp
 *
 * Description: Read resource and device informations from the file and put 
 *		these into the database.
 *		Synopsis : db_update <file name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-09-06 18:34:15 $
 */

#include "config.h"
/* TACO include file */
#include <API.h>

/* Include files */
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <file name>" << endl;
	cerr << " Read resource and device informations from the file" << std::endl;
	cerr << " and put them into the database" << std::endl;
	cerr << "        options : -h display this message" << std::endl;
	cerr << "                  -n nethost" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	long 	error;
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
        while ((c = getopt(argc,argv,"hn:")) != -1)
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
                                break;
                }
        if (optind != argc - 1)
                usage(argv[0]);

	string file_name(argv[optind]);

//
// Get environment variable
//
	string base("");
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
	cout  << "File name : " << file_name << endl;
#endif /* DEBUG */
//
// Connect to database server
//
	if (db_import(&error) == DS_NOTOK)
	{
		cerr << "db_update : Impossible to connect to database server" << endl;
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
				cerr << "Error in device definition" << endl;
			else
			{
				cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
		}
		else
		{
			cerr << "File analysis failed with error " << error << endl;
	    		cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
#ifdef DEBUG
	cout << "File analysis is OK" << endl;

	cout << nb_dev << " device list defined" << endl;	
	for (int i = 0; i < nb_dev; i++)
		cout << dev_def[i] << endl;
	cout << nb_res << " resources defined" << endl;
	for (int i = 0; i < nb_res; i++)
		cout << res_def[i] << endl;
#endif /* DEBUG */

//
// Check if there is any security resources
//
	for (int i = 0; i < nb_res; i++)
	{
		string str(res_def[i]);
		string::size_type pos;

		if ((pos = str.find('/')) == string::npos)
		{
			cerr << "Wrong resource syntax !!!, exiting" << endl;
			exit(-1);
		}
	
		string domain(str,0,pos);					
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
				cerr << "Can't retrieve the database defined password, update not allowed" << endl;
				exit(-1);
			}
		}
		
		if (pass == True)
		{
			answer = (char *)getpass("Security passwd : ");
			if (strcmp(answer,pa) != 0)
			{
				cout << "Sorry, wrong password. Update not allowed" << endl;
				cout << "Hint : Remove security resources from file" << endl;
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
				cerr << "Error in device definition number " << dev_err << ", error = " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			else
			{
				cerr << "Device(s) update call failed with error " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			exit(-1);
		}
	}
	
#ifdef DEBUG
	cout << "Update device successfull" << endl;
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
				cerr << "Error in resource definition number " << dev_err << ", error = " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			else
			{
				cerr << "Resource(s) update call failed with error " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
		}
	}	
	return 0;
}
