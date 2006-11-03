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
 * File:        test_client2.cpp
 *
 * Description: Code for test_client2 test program
 *              Synopsis : test_client2 <device name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-11-03 16:06:22 $
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
				pid,
				error;
	int 			cmd, 
				status;
	char			*ch_ptr; 
	short 			devstatus;
	DevVarStringArray	cmdline = {0, NULL};
	std::string		cmd_string;

/*	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/
	
	if (argc == 1)	
	{
		std::cerr << "usage: " << argv[0] << " [device name(s)]" << std::endl;
		exit(1);
	}
	
	if (db_import(&error) != DS_OK)
		return 3;

	for (int i = 1; i < argc; ++i)
	{
		db_devinfo_call devinfo;
		
		std::cout << i << std::endl;
		if (db_deviceinfo(argv[i], &devinfo, &error) != DS_OK)
		{
			std::cout << "Db_deviceinfo : " << dev_error_str(error) << std::endl;
			continue;
		}
		std::cout << "Exported " << devinfo.device_exported << std::endl
			<< "Type " << devinfo.device_type << std::endl
			<< "Server " << devinfo.server_name << std::endl
			<< "Personal " << devinfo.personal_name << std::endl
			<< "Class " << devinfo.device_class << std::endl
			<< "Process " << devinfo.process_name << std::endl
			<< "Version " << devinfo.server_version << std::endl;

		if ((status = dev_import(argv[i], readwrite, &ps, &error)) != DS_OK) 
		{
			std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << std::endl;
			std::cout << dev_error_str(error) << std::endl;
			continue;
		}
		std::cout << argv[i] << " imported." << std::endl;
		DevVarStringArray	resList;
		if (db_deviceres(1, &argv[i], (long *)(&resList.length), &resList.sequence, &error) != DS_OK)
		{
			std::cout << "Db_deviceres : " << dev_error_str(error) << std::endl;
			std::cout << "Ping : " << argv[i] << " - " << dev_ping(ps, &error) << std::endl;
			continue;
		}
		for (unsigned int j = 0; j < resList.length; ++j)
			std::cout << "Resource[" << j << "] = " << resList.sequence[j] << std::endl;
		if (dev_rpc_protocol(ps, D_TCP, &error) != DS_OK)
		{
			std::cout << "Dev_rpc_protocol : " << dev_error_str(error) << std::endl;
			std::cout << "Ping : " << argv[i] << " - " << dev_ping(ps, &error) << std::endl;
			continue;
		}

		DevVarCmdArray	cmdList;
		if (dev_cmd_query(ps, &cmdList, &error) != DS_OK)
		{
			std::cout << "Dev_cmd_query : " << dev_error_str(error) << std::endl;
			std::cout << "Ping : " << argv[i] << " - " << dev_ping(ps, &error) << std::endl;
			continue;
		}

		for (unsigned int j = 0; j < cmdList.length; ++j)
			std::cout << "CMD[" << j << "] = " << cmdList.sequence[j].cmd_name << std::endl;		
//		dev_event_query
		std::cout << "Ping : " << argv[i] << " - " << dev_ping(ps, &error) << std::endl;
		dev_free(ps, &error);
	}
	if (error != 0)
	{
		std::cout << "Failed : error " << error << std::endl;
		dev_printerror_no(SEND,NULL,error);
	}
	return 0;
}

