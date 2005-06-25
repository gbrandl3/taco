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

