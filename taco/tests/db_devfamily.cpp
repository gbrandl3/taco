/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_devfamily command                                  
*                                                                           
*    Command rule : To get a list of domains of the device from the static database.          
*                                                                           
*    Synopsis : db_devdomain
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
		std::cerr << "usage : " << *argv << " domain " << std::endl;
		exit(-1);
	}
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		exit(-1);
	}
//
// Ask database server to delete resource
// Display error message if the call fails
//
	long res_nb;
	char **tmp; 
	if (db_getdevfamilylist(argv[1], &res_nb, &tmp, &error) == -1)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
	}
	for (int i = 0; i < res_nb; i++)
    		std::cout << tmp[i] << std::endl;

	db_freedevexp(tmp);
	return 0;
}
