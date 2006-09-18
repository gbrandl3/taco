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
 * File:        test_start.cpp
 *
 * Description: Code for test_server test program
 *              Synopsis : test_server <personal name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.4 $
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
#include <string>

extern long debug_flag;

DevVarStringArray read_cmdparameters(void)
{
	DevVarStringArray	cmd;

	std::string	process_name,
			pers_name;

	std::cin >> process_name;
	std::cin >> pers_name;
	cmd.length = 2;
	cmd.sequence = new DevString[2];
	cmd.sequence[0] = const_cast<DevString>(process_name.c_str());
	cmd.sequence[1] = const_cast<DevString>(pers_name.c_str());
	return cmd;
}

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

#ifdef EBUG
	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);
#endif	
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
			std::cerr << "usage: test_start [device name]" << std::endl;
			exit(1);
	}

	status = dev_import(const_cast<char *>(cmd_string.c_str()), readwrite, &ps, &error);

	if (status != 0) 
	{
		std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << ")" << std::endl
			<< dev_error_str(error) << std::endl;
		exit(1);
	}

	while (1)
	{
		std::cout << "Select one of the following commands:" << std::endl << std::endl;
		std::cout << "0. Quit" << std::endl
			<< "1. On          2. Off          3. Reset" << std::endl
			<< "4. State       5. Status" << std::endl
			<< "6. Run         7. Stop         8. Restart" << std::endl;

		std::cout << "cmd ?";
		std::cin >> cmd;
		switch (cmd) 
		{
			case 0 : 
				dev_free(ps,&error);
				exit(0);
	   		case 1 : 
				status = dev_put(ps, DevOn, NULL, D_VOID_TYPE, &error);
				std::cout << std::endl << "DevOn dev_put() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				break;
	   		case 2 : 
				status = dev_put(ps, DevOff, NULL, D_VOID_TYPE, &error);
				std::cout << std::endl << "DevOff dev_put() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				break;
			case 4 : 
				status = dev_putget(ps, DevState, NULL, D_VOID_TYPE, &devstatus, D_SHORT_TYPE, &error);
				std::cout << std::endl << "DevState dev_putget() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND,NULL,error); 
				else	
					std::cout << "status read " << devstatus << ", " << DEVSTATES[devstatus] << std::endl;
				break;
			case 5 : 
				ch_ptr=NULL; 
				status = dev_putget(ps, DevStatus, NULL, D_VOID_TYPE, &ch_ptr, D_STRING_TYPE, &error);
				std::cout << std::endl << "DevStatus dev_putget() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				else  
					std::cout << ch_ptr << std::endl;
				break;
			case 3 : 
				status = dev_put(ps, DevReset, NULL, D_VOID_TYPE, &error);
				std::cout << std::endl << "DevReset dev_put() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				break;
			case 6 : 
				cmdline = read_cmdparameters();
				status = dev_put(ps, DevRun, &cmdline, D_VAR_STRINGARR, &error);
				std::cout << "DevRun dev_put() returned " << status << std::endl;
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				delete [] cmdline.sequence;
				break;
			case 7 : 
				cmdline = read_cmdparameters();
				status = dev_put(ps, DevStop, &cmdline, D_VAR_STRINGARR, &error);
				std::cout << std::endl << "DevStop devput() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				delete [] cmdline.sequence;
				break;
			case 8 : 
				cmdline = read_cmdparameters();
				status = dev_put(ps, DevRestore, &cmdline, D_VAR_STRINGARR, &error);
				std::cout << std::endl << "DevRestart dev_put() returned " << status << std::endl;
				if (status != DS_OK)
					dev_printerror_no(SEND,NULL,error); 
				delete [] cmdline.sequence;
				break;
			default : 
				break;
		}
	}
	return 0;
}

