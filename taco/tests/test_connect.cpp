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
 * File:        test_connect.cpp
 *
 * Description: Code for test_connect test program
 *              Synopsis : test_connect <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-09-18 22:34:41 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <iostream>
#include <iomanip>
#include <string>

extern long debug_flag;

int main(int argc,char **argv)
{
	devserver 		ps;
	long 			readwrite = ADMIN_ACCESS, 
				pid,
				error;
	int 			cmd, 
				status;
	char			*ch_ptr; 
	short 			devstatus;
	DevVarStringArray	cmdline = {0, NULL};
	std::string		cmd_string;

//	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);
	switch (argc)	
	{
		case 1:
			std::cout << "enter device name [\"sys/start/d\"]?";
			std::cin >> cmd_string;
			if (cmd_string.empty())
				cmd_string = "sys/start/d";
			break;
		case 2:
			cmd_string = argv[1];
			break;
		default:
			std::cerr << "usage: " << *argv << " [device name]" << std::endl;
			exit(1);
	}

	status = dev_import(const_cast<char *>(cmd_string.c_str()), readwrite, &ps, &error);

	if (status != DS_OK) 
	{
		std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}

	std::cout << "Set protocol to TCP" << std::endl;
	status = dev_rpc_protocol(ps, D_TCP, &error);
	if (status != DS_OK)
	{
		std::cout << "dev_rpc_protocol(D_TCP) returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}

	std::cout << "Query commands" << std::endl;
	DevVarCmdArray	cmds = {0, NULL};
	status = dev_cmd_query(ps, &cmds, &error);
	if (status != DS_OK)
	{
		std::cout << "dev_cmd_query() returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}
	for (int i = 0; i < cmds.length; ++i)
		std::cout << std::setw(14) << (unsigned)cmds.sequence[i].cmd
			<< std::setw(8) << (unsigned)cmds.sequence[i].in_type
			<< std::setw(8) << (unsigned)cmds.sequence[i].out_type
			<< std::setw(25) << "\"" << cmds.sequence[i].cmd_name << "\"" << std::endl;

	std::cout << "Query events" << std::endl;
	DevVarEventArray	events;
	status = dev_event_query(ps, &events, &error);
	if (status != DS_OK)
	{
		std::cout << "dev_event_query() returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}
	for (int i = 0; i < events.length; ++i)
		std::cout << std::setw(14) << (unsigned)events.sequence[i].event
			<< std::setw(12) << (unsigned)events.sequence[i].out_type
			<< std::setw(25) << "\"" << events.sequence[i].event_name << "\"" << std::endl;

	dev_free(ps,&error);
	return 0;
}

