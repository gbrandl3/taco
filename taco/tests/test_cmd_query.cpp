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

#ifdef EBUG
	debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);
#endif	
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

	if (status != 0) 
	{
		std::cout << "dev_import(" << cmd_string << ") returned " << status << " (error=" << error << std::endl;
		std::cerr << dev_error_str(error) << std::endl;
		exit(1);
	}

	DevVarCmdArray	varcmdarr = {0, NULL}; 
	if (dev_cmd_query(ps, &varcmdarr, &error) == DS_OK)
		for (int i = 0; i < varcmdarr.length; ++i)
			std::cout << std::setw(10) << varcmdarr.sequence[i].cmd 
				<< std::setw(25) << varcmdarr.sequence[i].cmd_name << std::endl; 
	return 0;
}

