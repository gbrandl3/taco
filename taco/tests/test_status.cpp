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
			std::cerr << "usage: ps_menu [device name]" << std::endl;
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

