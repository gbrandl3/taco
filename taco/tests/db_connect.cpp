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
 *    Command rule : To delete a device and its resources from the database. *
 *                   The -r option is used if the user does not want device  *
 *		    resources to be also deleted			    *
 *                                                                           *
 *    Synopsis : db_devdel [-r] <device name>               		    *
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
		exit(-1);
	}
	return 0;
}
