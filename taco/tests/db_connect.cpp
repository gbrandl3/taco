/* TACO include file */
#include <API.h>

/* Include files */
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>

/*
 ***************************************************************************
 * Code for db_connect command. 
 * Synopsis : db_connect 
 ***************************************************************************
 */
int main(int argc, char *argv[])
{
	long 	error;
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << "db_devinfo : Impossible to connect to database server" << std::endl;
		std::cerr << dev_error_str(errno) << std::endl;
		exit(-1);
	}
	return 0;
}
