/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_resput command                                  
*                        ---------                                          
*                                                                           
*    Command rule : To put a resource into the static database.          
*                                                                           
*    Synopsis : db_resput resource_name resource_value [resource_value [resource_value] ... ]
*                                                                           
****************************************************************************/
int main(int argc, char *argv[])
{
	DevVarStringArray       server_list = {0, NULL};
	long error;
//
// Argument test and device name structure
//
	if (argc < 3)
	{
		std::cerr << "usage : " << *argv << " resource_name resource_value [resource_value [resource_value] ... ]" << std::endl;
		exit(-1);
	}

	std::string full_res_name(argv[1]);
	std::transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
//
// Test resource name syntax
//
	int  max_slashes = (full_res_name.substr(0, 2) == "//") ? 6 : 3;
	if (std::count(full_res_name.begin(), full_res_name.end(), '/') != max_slashes)
	{
		std::cerr << "db_resdel : Bad resource name" << std::endl;
		exit(-1);

	}
//
// Extract device name from full resource name
//
	std::string::size_type pos = full_res_name.rfind('/');
	std::string res_name(full_res_name, pos + 1);
	std::string dev_name(full_res_name, 0, pos);
#ifdef DEBUG
	std::cerr << "Full resource name : " << full_res_name << std::endl;
	std::cerr << "Device name : " << dev_name << std::endl;
	std::cerr << "Resource name : " << res_name << std::endl;
#endif /* DEBUG */
//
// Test domain name and exit if the resource belongs to the SEC domain
//
	pos = full_res_name.find('/');
	std::string domain(full_res_name, 0, pos);
	if (domain == "sec")
	{
		std::cerr << "db_resdel : SEC is not a authorized domain name" << std::endl;
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
	db_resource             server = {res_name.c_str(), D_VAR_STRINGARR, &server_list};
	server_list.length = argc - 2;
	server_list.sequence = new char *[server_list.length];
	for (int i = 0; i < server_list.length; ++i)
	{
		server_list.sequence[i] = new char[strlen(argv[2 + i]) + 1];
		strcpy(server_list.sequence[i], argv[2 + i]);
	}
	if (db_putresource(const_cast<char *>(dev_name.c_str()), &server, 1, &error) != DS_OK)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	return 0;
}
