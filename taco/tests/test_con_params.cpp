/* TACO include file */
#include <API.h>
#include <DevSec.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for dev_import command                                  
*                        ---------                                          
*                                                                           
*    Command rule : To import a device from the TACO system.          
*                                                                           
*    Synopsis : dev_resget device_name
*                                                                           
****************************************************************************/
int main(int argc, char *argv[])
{
	long error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		std::cerr << "usage : " << *argv << " device_name" << std::endl;
		exit(-1);
	}

	std::string full_name(argv[1]);
	std::transform(full_name.begin(), full_name.end(), full_name.begin(), ::tolower);
//
// Test resource name syntax
//
	int  max_slashes = (full_name.substr(0, 2) == "//") ? 5 : 2;
	if (std::count(full_name.begin(), full_name.end(), '/') != max_slashes)
	{
		std::cerr << *argv << " : Bad resource name" << std::endl;
		exit(-1);

	}
//
// Ask system to get the handle on specific device
// Display error message if the call fails
//
	devserver	ds;
	if (dev_import(const_cast<char *>(full_name.c_str()), WRITE_ACCESS, &ds, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	if (dev_rpc_protocol(ds, D_UDP, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	if (dev_rpc_protocol(ds, D_TCP, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	struct timeval	tm;
	if (dev_rpc_timeout(ds, CLGET_TIMEOUT, &tm, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	std::cout << tm.tv_sec + tm.tv_usec / 1000000.0 << std::endl;
	tm.tv_sec = 1;
	tm.tv_usec = 500000;
	if (dev_rpc_timeout(ds, CLSET_TIMEOUT, &tm, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	if (dev_rpc_timeout(ds, CLGET_TIMEOUT, &tm, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	std::cout << tm.tv_sec + tm.tv_usec / 1000000.0 << std::endl;
	if (dev_free(ds, &error) == -1)
	{
		std::cerr << "The call to system failed with error " << error << std::endl
			<< "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	return 0;
}
