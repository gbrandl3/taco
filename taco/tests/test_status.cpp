/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 ESRF, www.esrf.fr
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
 * File:        test_status.cpp
 *
 * Description: Code for test_status test program
 *              Synopsis : test_status <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-30 13:42:48 $
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

#include <test_server.h>

extern long debug_flag;

int main(int argc,char **argv)
{

	devserver 		ps;
	long 			readwrite = ADMIN_ACCESS, 
				pid;
	DevLong			error;
	int 			cmd, 
				status;
	char			*ch_ptr; 
	short 			devstatus;
	DevVarStringArray	cmdline = {0, NULL};
	std::string		cmd_string;

/*	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/
	
	switch (argc)	
	{
		case 1:
			std::cout << "enter device name [\"tl1/ps-d/d\"]?";
			std::cin >> cmd_string;
			if (cmd_string.empty())
				cmd_string = "tl1/ps-d/d";
			break;
		case 2:
			cmd_string = argv[1];
			break;
		default:
			std::cerr << "usage: " << *argv << " [device name]" << std::endl;
			exit(1);
	}

	status = dev_import(const_cast<char *>(cmd_string.c_str()), readwrite, &ps, &error);
	if (dev_rpc_protocol(ps, D_TCP, &error) != DS_OK)
	{
		std::cerr << dev_error_str(error) << std::endl;
		return 1;
	}

	if (status != 0) 
	{
		std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << std::endl;
		std::cerr << dev_error_str(error) << std::endl;
		exit(1);
	}

	DevVarCharArray	arr = {0, NULL};
	if (dev_putget(ps, DevStatus, NULL, D_VOID_TYPE, &arr, D_VAR_CHARARR, &error) == DS_OK)
	{
		std::cout << arr.length << " Elements" << std::endl;
		for (int i = 0; i < arr.length; ++i)
			std::cout << arr.sequence[i];
		std::cout << std::endl;
	}
	else
		std::cerr << dev_error_str(error) << std::endl;
	if (dev_rpc_protocol(ps, D_UDP, &error) != DS_OK)
		return 1;
	if (dev_putget(ps, DevStatus, NULL, D_VOID_TYPE, &arr, D_VAR_CHARARR, &error) == DS_OK)
	{
		std::cout << arr.length << " Elements" << std::endl;
		for (int i = 0; i < arr.length; ++i)
			std::cout << arr.sequence[i];
		std::cout << std::endl;
	}
	else
		std::cerr << dev_error_str(error) << std::endl;
	return 0;
}

