/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_getresoval
*                                                                           
*    Command rule : To get the value for a specified resource
*                                                                           
*    Synopsis : db_resdomain resource
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
		std::cerr << "usage : " << *argv << " resource " << std::endl;
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
	std::string	resource(argv[1]);	
	if (count(resource.begin(), resource.end(), '/') != 3)
	{
		std::cerr << "Wrong resource name: Should be domain/family/member/name" << std::endl;
		exit(1);
	}
	std::string::size_type	pos = resource.find('/');
	std::string	domain = resource.substr(0, pos);
	resource.erase(0, pos + 1);
	pos = resource.find('/');
	std::string	family = resource.substr(0, pos);	
	resource.erase(0, pos + 1);
	pos = resource.find('/');
	std::string	member = resource.substr(0, pos);
	std::string	name = resource.substr(pos + 1);

	long res_nb;
	char **tmp; 
	if (db_getresresoval(const_cast<char *>(domain.c_str()), const_cast<char *>(family.c_str()), 
				const_cast<char *>(member.c_str()), const_cast<char *>(name.c_str()),
			&res_nb, &tmp, &error) != DS_OK)
	{
		std::cerr << "The call to database server failed with error " << error << std::endl;
		std::cerr << "Error message : " << dev_error_str(error) << std::endl;
		exit(1);
	}
	for (int i = 0; i < res_nb; i++)
    		std::cout << tmp[i] << std::endl;
	return 0;
}
