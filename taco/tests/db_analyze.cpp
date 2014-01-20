/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 ESRF, www.esrf.fr
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
 * File:	db_analyze.cpp
 *
 * Description: Read resource and device informations from the file and put 
 *		these into the database.
 *		Synopsis : db_analyze <file name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:46 $
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
		cerr << *argv << " : Impossible to connect to database server" << endl;
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
			cerr << "Error message : " << dev_error_str(error) << endl;
			cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
		}
		else
		{
			cerr << "File analysis failed with error " << error << endl;
	    		cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
	cout << "File analysis is OK" << endl;

	cout << nb_dev << " device list defined" << endl;	
	for (int i = 0; i < nb_dev; i++)
		cout << dev_def[i] << endl;
	cout << nb_res << " resources defined" << endl;
	for (int i = 0; i < nb_res; i++)
	{
		std::string tmp = res_def[i];
		for(std::string::iterator it = tmp.begin(); it != tmp.end(); ++it)
		{
			if (iscntrl(*it))
				cout << '^' << char('@' + *it);
			else
				cout << *it;
		}
		cout << endl;
	}
	return 0;
}
