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
 * File:        test_cmd_query.cpp
 *
 * Description: Code for test_cmd_query test program
 *              Synopsis : test_cmd_query <resource name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2008-04-06 09:08:06 $
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
	long 			readwrite = READ_ACCESS, 
				pid,
				error;
	int 			cmd, 
				status;
	char			*ch_ptr; 
	short 			devstatus;
	DevVarStringArray	cmdline = {0, NULL};
	std::string		cmd_string;

#ifdef EBUG
	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);
#endif	
	switch (argc)	
	{
		case 2:
			cmd_string = argv[1];
			break;
		default:
			std::cerr << "usage: " << *argv << " [device name]" << std::endl;
			exit(1);
	}

	status = dev_import(const_cast<char *>(cmd_string.c_str()), readwrite, &ps, &error);
	if (status != 0) 
	{
		std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}
	DevVarCmdArray	varcmdarr = {0, NULL}; 
	if ((status = dev_cmd_query(ps, &varcmdarr, &error)) == DS_OK)
		for (int i = 0; i < varcmdarr.length; ++i)
			std::cout << std::setw(10) << varcmdarr.sequence[i].cmd 
				<< std::setw(25) << varcmdarr.sequence[i].cmd_name  
				<< std::setw(10) << varcmdarr.sequence[i].in_type  
				<< std::setw(10) << varcmdarr.sequence[i].out_type  
				<< std::endl; 
	else
	{
		std::cout << "dev_cmd_query(" << cmd_string << ") returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		return 1;
	}
	return 0;
}

