/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_devexp command                                  
*                                                                           
*    Command rule : To export a device
*                                                                           
*    Synopsis : db_devexp
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
		std::cerr << "usage : " << *argv << " device name" << std::endl;
		exit(-1);
	}
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << "db_devinfo : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	Db_devinf	devinfo;

	devinfo->device_name = argv[1];
	devinfo->host_name = "localhost";
	devinfo->device_type = "test_device";
	devinfo->device_class = "test_class";
	devinfo->pn = 99999;
	devinfo->vn = 1;
	devinfo->proc_name = "taco_test_server";	

	if (db_dev_export(devinfo, 1, &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		return 1;
	}
	return 0;
}
