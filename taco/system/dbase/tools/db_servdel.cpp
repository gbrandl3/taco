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
 * File:	db_servdel.cpp
 *
 * Description: To delete all the devices and their resources from the database.
 *              The -r option is used if the user does not want device resources 
 *		to be also deleted
 *		Synopsis : db_servdel [-r] <full server name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2006-09-06 18:34:15 $
 */

#include "config.h"
/* TACO include file */
#include <API.h>

/* Include files */
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <full device server name>" << endl;
	cerr << "  Delete a server, all its devices and optional their resources" << std::endl;
	cerr << "        options : -r all resources of the devices deleted" << std::endl;
	cerr << "                  -h display this message" << std::endl;
	cerr << "                  -n nethost" << std::endl;
	exit(-1);
}

int main(int argc,char *argv[])
{
	long error;
	long del_res = True;
	long opt = False;
	extern int optind;
	int c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"hn:r")) != -1)
		switch (c)
		{
			case 'n':
				{
					char s[160];
					snprintf(s, sizeof(s), "NETHOST=%s", optarg);
					putenv(s);
				}
			break;
			case 'r':
				del_res = False;
				break;
			case 'h':
			case '?':
				usage(argv[0]);
		}

	if (optind != argc - 1)
		usage(argv[0]);

	string full_ds_name(argv[optind]);

#ifdef DEBUG
	cout  << "Full device server name : " << full_ds_name << endl;
#endif 
#ifndef _solaris
        if (std::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		cerr << "db_servdel : Bad full device server name" << endl;
		exit(-1);
	}
//
// Extract device server and personal name from full device server name
//
	string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == string::npos)
	{
		cout << "db_servdel : Can't split full device server" << endl;
		exit(-1);
	}
	
	string ds_name(full_ds_name,0,pos);
	string pers_name(full_ds_name.substr(pos + 1 ));
	
#ifdef DEBUG
	cout << "DS name : " << ds_name << endl;
	cout << "Pers. name : " << pers_name << endl;
#endif /* DEBUG */

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_servdel : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Ask database server to remove device
// Display error message if the call fails
//
	if (db_servdelete(ds_name.c_str(), pers_name.c_str(), del_res, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			cerr << "Server " << full_ds_name << " does not have any device in database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
	return 0;
}
