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
 * File:	db_info.cpp
 *
 * Description: To get device and resources informations from the static database 
 *		Synopsis : db_info
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2006-09-06 18:34:15 $
 */

#include "config.h"
/* TACO include file */
#include <API.h>

/* Include files */
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options]" << endl;
        cerr << " displays statistics information about the use of the database" << std::endl;
	cerr << "        options : -h display this message" << std::endl;
	cerr << "                  -n nethost" << std::endl;
	exit(1);
}
int main(int argc,char *argv[])
{
	long error;
	db_stat_call inf;
	time_t sec;
	char *ti;
	extern char     *optarg;
        extern int      optind,
                        opterr,
                        optopt;
        int             c;


// Argument test and domain name modification
        while((c = getopt(argc, argv, "h")) != -1)
                switch(c)
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
				exit(-1);
		}

	if (optind != argc)
		usage(argv[0]);
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		cerr << "db_info : Impossible to connect to database server" << endl;
		exit(-1);
	}
//
// Ask database server for informations
// Display error message if the call fails
//
	if (db_stat(&inf, &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
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
	cout << "\t\t" << ti;
	cout << "\t\tDEVICE STATISTICS" << endl << endl;
	cout << inf.dev_defined << " devices are defined in database" << endl;
	cout << inf.dev_exported << " of the defined devices are actually exported:" << endl;
	for (int i = 0; i < inf.dev_domain_nb; i++)
		cout << "    " << inf.dev_domain[i].dom_elt << " for the " << inf.dev_domain[i].dom_name << " domain" << endl;
	cout << inf.psdev_defined << " pseudo devices are defined in the database" << endl;
	cout << endl;
	cout << "\t\tRESOURCE STATISTICS" << endl << endl;
	cout << inf.res_number << " resources are defined in database:" << endl;
	for (int i = 0; i < inf.res_domain_nb; i++)
		cout << "    " << inf.res_domain[i].dom_elt << " resources for the " << inf.res_domain[i].dom_name << " domain" << endl;
	return 0;
}


