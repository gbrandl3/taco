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
	cerr << "usage : " << *argv << " domain_name" << endl;
	exit(-1);
    }

    string full_res_name(argv[1]);
    transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), tolower);
    string domain(argv[1]);
    if (domain == "sec")
    {
	cerr << "db_resdel : SEC is not a authorized domain name" << endl;
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
    if (db_getresfamilylist(const_cast<char *>(domain.c_str()), &res_nb, &tmp,  &error) == -1)
    {
	cerr << "The call to database server failed with error " << error << endl;
	cerr << "Error message : " << dev_error_str(error) << endl;
	exit(-1);
    }
    for (int i = 0; i < res_nb; i++)
    	cout << tmp[i] << endl;
    return 0;
}
