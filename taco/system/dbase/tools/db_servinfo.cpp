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
 * File:	db_servinfo.cpp
 *
 * Description: To display device server info
 *		Synopsis : db_servinfo [ full device server name ]
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
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <full device server name>" << endl;
	cerr << "  Diplay device server informations" << std::endl;
	cerr << "        options : -h display this message" << std::endl;
	cerr << "                  -n nethost" << std::endl;
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

	string full_ds_name(argv[optind]);

#ifdef DEBUG
	cout  << "Server name : " << full_ds_name << endl;
#endif /* DEBUG */
#ifndef _solaris
	if (std::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		cerr << "db_servinfo : Bad full device server name" << endl;
		exit(-1);
	}

//
// Extract device server and personal name from full device server
// name
//

	string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == string::npos)
	{
		cout << "db_servinfo : Can't split full device server" << endl;
		exit(-1);
	}
	
	string ds_name(full_ds_name,0,pos);
    string pers_name(full_ds_name.substr(pos + 1)); 
#ifdef DEBUG
	cout << "DS name : " << ds_name << endl;
	cout << "Pers. name : " << pers_name << endl;
#endif /* DEBUG */
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_servinfo : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Ask database server for server info
// Display error message if the call fails
//
	db_svcinfo_call info;
	if (db_servinfo(ds_name.c_str(), pers_name.c_str(), &info, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			cout << "The device server " << full_ds_name << " is not defined in database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
//
// Display info sent back by server
//

	string pro(info.process_name);
	if (info.pid != 0)
	{
		if (pro != "unknown")
			cout << "Device server " << full_ds_name << " is part of the process " << info.process_name << endl;
		cout << "The process is running on host " << info.host_name << " with PID " << info.pid 
	     		<< " (Program number = " << info.program_num << ")" << endl;
	}
	
	for (int i = 0; i < info.embedded_server_nb; i++)
	{
		if (info.embedded_server_nb != 1)
		{
			cout << endl;
			cout << "Device server class : " << info.server[i].server_name << endl;
		}
		for (int j = 0; j < info.server[i].device_nb; j++)
		{
			if (info.server[i].device[j].exported_flag == True)
				cout << "Device number " << j << " : " << info.server[i].device[j].dev_name 
					<< " exported from host " << info.host_name << endl;
			else
				cout << "The device " << info.server[i].device[j].dev_name 
					<< " is defined for this server but is not exported" << endl;
		}
	}
	return 0;
}


