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
 * File:	db_initcache.cpp
 *
 * Description: To initialize a database server resource cache. This command 
 *		clears the old cache and (re)initialize it with the contents 
 *		of the RES database table for the wanted domain
 *    		Synopsis : db_initcache [ domain name ] 
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
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <domain name>" << endl;
	cerr << " Initialize a database server resource cache" << endl;
	cerr << " This command clears the old cache and (re)initialize it" << std::endl;
	cerr << " with the contents of the RES database table for the" << std::endl;
	cerr << " wanted domain" << std::endl;
	cerr << "        options : -h display this message" << std::endl;
	cerr << "                  -n nethost" << std::endl;
	exit(1);
}

int main(int argc,char *argv[])
{
	long error;
        extern char     *optarg;
        extern int      optind,
                        opterr,
                        optopt;
        int             c;


// Argument test and domain name modification
        while((c = getopt(argc, argv, "hn:")) != -1)
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
				usage(argv[0]);
		}
//
// Argument test and device name structure
//
	if (optind != argc - 1)
		usage(argv[0]);

	string dom_name(argv[1]);

#ifdef DEBUG
	cout  << "Domain name : " << dom_name << endl;
#endif 
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_initcache : Impossible to connect to database server" << endl;
		exit(-1);
	}
//
// Display error message if the call fails
// Init. cache
//
	if (db_initcache(dom_name.c_str(), &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
		exit(-1);
	}
	return 0;
}


