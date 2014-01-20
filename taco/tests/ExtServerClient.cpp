/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2006-2014 by Jens Krüger <jens.krueger@frm2.tum.de>
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
 * File:   	ExtServerClient.cpp
 *
 * Project:   	Device Server Distribution 
 *
 * Description: An interactive main routine for test all the commands
 *      	of the Device Server TestExtServer
 *
 * Author(s):   Jens Krueger
 *              $Author: jkrueger1 $
 *
 * Original:   	September 2006
 *
 * Version:     $Revision: 1.1 $
 *
 * Date:        $Date: 2006-09-20 16:33:04 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <iostream>
#include <TACOClient.h>
#include <string>

int main(int argc, char **argv)
{
	long		readwrite = 0;
	TACO::Client	*client;

	if (argc < 2)
	{
		std::cerr << "usage: " << *argv << " <device name>" << std::endl;
		return 1;
	}

	try
	{
		client = new ::TACO::Client(argv[1], readwrite);
	}
	catch (const ::TACO::Exception &e)
	{
		std::cerr << "can't import " << argv[1] << " : " << e.what() << std::endl;
		return 2;
	}

	while (1)
	{
		char		cmd_string[80];
		DevShort	state = client->deviceState();
		std::string	status;
		int		cmd;

		std::cout << std::endl  
			<< "test_menu for " << argv[1] << " (" << "" << ")" << std::endl
			<< "____________________________________" << std::endl
			<< "1.State" << std::endl
			<< "2.Status" << std::endl
	   		<< "3.Reset" << std::endl <<  std::endl
			<< "Select one commands (0 to quit) : ?";
/*
 * to get around the strange effects of scanf() wait for something read 
 */
		for( ; fgets(cmd_string, sizeof(cmd_string), stdin) == (char *)0 ; );
		sscanf(cmd_string,"%d",&cmd);
		std::cout << cmd << std::endl ;
		switch (cmd) 
		{
			case 0 : 
				return 0;
			case 1 :
				try
				{
					state = client->deviceState();
					std::cout << "The state is " << DEVSTATES[state] << "(" << state << ")" << std::endl;
				}
				catch (const ::TACO::Exception &e)
				{
					std::cerr << "can't read state: " << e.what() << std::endl;
				}
				break;
			case 2 :
				try
				{
					status = client->deviceStatus();
					std::cout << "Status :  " << status << std::endl;
				}
				catch (const ::TACO::Exception &e)
				{
					std::cerr << "can't read status: " << e.what() << std::endl;
				}
				break;
		
			case 3 :
				try
				{
					client->deviceReset();
				}
				catch (const ::TACO::Exception &e)
				{
					std::cerr << "can't reset device: " << e.what() << std::endl;
				}
				break;
			default : 
				break;
		}
	}
}

