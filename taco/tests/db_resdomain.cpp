/* TACO include file */
#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/****************************************************************************
*                                                                           
*    Code for db_resdomain command                                  
*                        ---------                                          
*                                                                           
*    Command rule : To get a list of domains from the static database.          
*                                                                           
*    Synopsis : db_resdomain
*                                                                           
****************************************************************************/
int main(int argc, char *argv[])
{
    long error;
//
// Argument test and device name structure
//
    if (argc != 1)
    {
	cerr << "usage : " << *argv << endl;
	exit(-1);
    }
//
// Connect to database server
//
    if (db_import(&error) == -1)
    {
	cerr << "db_devinfo : Impossible to connect to database server" << endl;
	exit(-1);
    }
//
// Ask database server to delete resource
// Display error message if the call fails
//
    long res_nb;
    char **tmp; 
    if (db_getresdomainlist(&res_nb, &tmp, &error) == -1)
    {
	cerr << "The call to database server failed with error " << error << endl;
	cerr << "Error message : " << dev_error_str(error) << endl;
    }
    for (int i = 0; i < res_nb; i++)
    	cout << tmp[i] << endl;
    return 0;
}
