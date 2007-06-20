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
 * File:	db_devdel.cpp
 *
 * Description: To delete a device and its resources from the database. 
 *              The -r option is used if the user does not want device
 *		resources to be also deleted
 *              Synopsis : db_devdel [-r] <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2007-06-20 07:20:42 $
 */

/* TACO include file */
#include "config.h"
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
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " deletes a device and (optional) its resources from the database. " << std::endl;
	std::cerr << "        options : -r delete also resources of the device" << std::endl;
	std::cerr << "                  -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
	exit(1);
}

int main(int argc,char *argv[])
{
	long 		error;
	long 		del_res = True;
	extern int 	optopt;
	extern int	optind;
	extern char 	*optarg;
	int 		c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"hrn:")) != -1)
	{
		switch (c)
		{
		case 'r':
			del_res = False;
			break;
		case 'n':
			setenv("NETHOST", optarg, 1);
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			break;
		}
	}
    	if (optind != argc - 1)
		usage(argv[0]);

    	string dev_name(argv[optind]);
	if (dev_name.substr(0, 2) == "//")
	{
		std::string::size_type pos = dev_name.find("/", 2);
		setenv("NETHOST", dev_name.substr(2, pos - 2).c_str(), 1);
		dev_name.erase(0, pos + 1);
	}
#ifdef DEBUG
	cout  << "Device name : " << dev_name << endl;
#endif /* DEBUG */
#ifndef _solaris
	if (std::count(dev_name.begin(), dev_name.end(), '/') != 2)
#else
	if (_sol::count(dev_name.begin(), dev_name.end(), '/') != 2)
#endif /* _solaris */
	{
		cerr << "db_devdel : Bad device name" << endl;
		exit(-1);
	}

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_devdel : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Ask database server to remove device
// Display error message if the call fails
//
        if (db_devicedelete(dev_name.c_str(), &error) == -1)
	{
		if (error == DbErr_DeviceNotDefined)
			cerr << "Device " << dev_name << " is not defined in the database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}

//
// Delete device resources if it is wanted
//

	if (del_res == True)
	{
		char *tmp = const_cast<char *>(dev_name.c_str());
    		db_error 	err;
		if (db_devicedeleteres(1, &tmp, &err) == -1)
		{
			cerr << "The call to database server to delete device resources failed with error " << err.error_code << endl;
			cerr << "Error message : " << dev_error_str(err.error_code) << endl;
		}
	}
    	return 0;
}


