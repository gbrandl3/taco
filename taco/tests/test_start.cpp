#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <iostream>
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

	/*debug_flag = (DEBUG_ON_OFF | DBG_TRACE | DBG_API | DBG_SEC);*/
	
	cerr << DevRun << endl
		<< DevStop << endl
		<< DevRestore << endl << endl
		<< DevStatus << endl;

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

	while (1)
	{
		std::cout << "Select one of the following commands:" << std::endl << std::endl;
		std::cout << "0. Quit" << std::endl
			<< "1. On          2. Off          3. State" << std::endl
			<< "4. Status      5. Reset" << std::endl
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
			case 3 : 
				status = dev_putget(ps, DevState, NULL, D_VOID_TYPE, &devstatus, D_SHORT_TYPE, &error);
				std::cout << std::endl << "DevState dev_putget() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND,NULL,error); 
				else	
					std::cout << "status read " << devstatus << ", " << DEVSTATES[devstatus] << std::endl;
				break;
			case 4 : 
				ch_ptr=NULL; 
				status = dev_putget(ps, DevStatus, NULL, D_VOID_TYPE, &ch_ptr, D_STRING_TYPE, &error);
				std::cout << std::endl << "DevStatus dev_putget() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				else  
					std::cout << ch_ptr << std::endl;
				break;
			case 5 : 
				status = dev_put(ps, DevReset, NULL, D_VOID_TYPE, &error);
				std::cout << std::endl << "DevReset dev_put() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND, NULL, error); 
				break;
			case 6 : 
				status = dev_put(ps, DevRun, &cmdline, D_VAR_STRINGARR, &error);
				std::cout << "DevRun dev_put() returned " << status << std::endl;
				if (status < 0) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 7 : 
				status = dev_put(ps, DevStop, &pid, D_LONG_TYPE, &error);
				std::cout << std::endl << "DevStop devput() returned " << status << std::endl;
				if (status != DS_OK) 
					dev_printerror_no(SEND,NULL,error); 
				break;
			case 8 : 
				status = dev_put(ps, DevRestore, &cmdline, D_VAR_STRINGARR, &error);
				std::cout << std::endl << "DevRestor dev_put() returned " << status << std::endl;
				if (status != DS_OK)
					dev_printerror_no(SEND,NULL,error); 
				break;
			default : 
				break;
		}
	}
	return 0;
}

