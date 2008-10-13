/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2007 Jens Krüger <jens.krueger@frm2.tum.de>
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
 * File:	dbm_sec_objinfo.cpp
 *
 * Description: To display the security informations for objects like domain
 *		family, or device.
 *
 * Author(s):	Jens Krüger
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2008-10-13 19:41:24 $
 */

#include <iostream>
#include <string>
#include <taco_utils.h>
#include <cstdlib>

void usage(const std::string cmd)
{
	std::cerr << "usage : " << cmd << " [options] " << std::endl;
        std::cerr << " displays all accesses specified for a user and/or a group" << std::endl;
	std::cerr << " this object may be a domain, family, or device" << std::endl;
	std::cerr << "        options : -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
	std::cerr << "                  -u user_name" << std::endl;
	std::cerr << "                  -g group_name" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	long 		error;
	extern int 	optopt;
	extern int	optind;
	extern char 	*optarg;
	int 		c;
	
	std::string	user_name(""),
			group_name("");

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"hn:u:g:")) != -1)
	{
		switch (c)
		{
			case 'n':
				taco_setenv("NETHOST", optarg, 1);
				break;
			case 'u':
				user_name = optarg;
				break;
			case 'g':
				group_name = optarg;
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				break;
		}
	}
    	if (optind != argc)
		usage(argv[0]);

	return 0;
}
