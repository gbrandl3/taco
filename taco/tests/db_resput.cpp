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
*    Command rule : To get a resource from the static database.          
*                                                                           
*    Synopsis : db_resput resource_name resource_val
*                                                                           
****************************************************************************/
int main(int argc, char **argv)
{
    long error;
//
// Argument test and device name structure
//
    if (argc != 3)
    {
	cerr << "usage : " << *argv << " resource_name resource_val" << endl;
	exit(-1);
    }

    string full_res_name(argv[1]);
    transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), tolower);
//
// Test resource name syntax
//
    if (count(full_res_name.begin(), full_res_name.end(), '/') != 3)
    {
	cerr << "db_resput : Bad resource name" << endl;
	exit(-1);
    }
//
// Extract device name from full resource name
//
    string::size_type pos = full_res_name.rfind('/');
    string res_name(full_res_name, pos + 1);
    string dev_name(full_res_name, 0, pos);
#ifdef DEBUG
    cerr << "Full resource name : " << full_res_name << endl;
    cerr << "Device name : " << dev_name << endl;
    cerr << "Resource name : " << res_name << endl;
#endif /* DEBUG */
//
// Test domain name and exit if the resource belongs to the SEC domain
//
    pos = full_res_name.find('/');
    string domain(full_res_name, 0, pos);
    if (domain == "sec")
    {
	cerr << "db_resput : SEC is not a authorized domain name" << endl;
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
    unsigned int res_nb = 1;
    char *tmp = argv[2]; 
    db_resource resTable [] = {{ res_name.c_str(), D_STRING_TYPE, &tmp},};
    if (db_putresource(const_cast<char *>(dev_name.c_str()), resTable, 1, &error) == -1)
    {
	cerr << "The call to database server failed with error " << error << endl;
	cerr << "Error message : " << dev_error_str(error) << endl;
	exit(-1);
    }
    return 0;
}
