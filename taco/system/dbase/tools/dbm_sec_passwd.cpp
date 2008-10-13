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
 * File:	dbm_sec_passwd.cpp
 *
 * Description: To define or change the password for the SEC domain 
 *
 * Author(s):	Jens Krüger
 *              $Author: andy_gotz $
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-10-13 19:01:46 $
 */

#include <iostream>
#include <string>
#ifdef _solaris
#	include <taco_utils.h>
#endif /* _solaris */
#include <cstdlib>

void usage(const std::string cmd)
{
	std::cerr << "usage : " << cmd << " [options]" << std::endl;
        std::cerr << " defines or changes the password for the SEC domain" << std::endl;
	std::cerr << "        options : -h display this message" << std::endl;
	std::cerr << "                  -n nethost" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	long 		error;
	extern int 	optopt;
	extern int	optind;
	extern char 	*optarg;
	int 		c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"hn:")) != -1)
	{
		switch (c)
		{
			case 'n':
				taco_setenv("NETHOST", optarg, 1);
				break;
			case 'h':
			case '?':
				usage(argv[0]);
				break;
		}
	}
    	if (optind != argc - 1)
		usage(argv[0]);

	return 0;
}
