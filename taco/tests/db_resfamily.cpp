/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_resfamily command                                  
*                        ---------                                          
*                                                                           
*    Command rule : To get a resource from the static database.          
*                                                                           
*    Synopsis : db_resfamily domain_name 
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
		std::cerr << "usage : " << *argv << " domain_name" << std::endl;
		exit(-1);
	}

	std::string full_res_name(argv[1]);
	std::transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
	std::string domain(argv[1]);
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
	long res_nb;
	char **tmp; 
	if (db_getresfamilylist(const_cast<char *>(domain.c_str()), &res_nb, &tmp,  &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(-1);
	}
	for (int i = 0; i < res_nb; i++)
		std::cout << tmp[i] << std::endl;
	return 0;
}
