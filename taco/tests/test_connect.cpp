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
			std::cerr << "usage: test_connect [device name]" << std::endl;
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
		std::cout << "dev_cmd_query() returned " << status << " (error=" << error << ")" << std::endl
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

