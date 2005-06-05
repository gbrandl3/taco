/* TACO include file */

#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

/****************************************************************************
*                                                                           *
*		Code for db_devres command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To get device resources from the static database.       *
*                                                                           *
*    Synopsis : db_devres [ device name ]               		    *
*                                                                           *
****************************************************************************/


int main(int argc,char *argv[])
{
    long error;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		cerr << "db_devres usage : db_devres <device name>" << endl;
		exit(-1);
	}

	string dev_name(argv[1]);

#ifdef DEBUG
	cout  << "Device name : " << dev_name << endl;
#endif 
#ifndef _solaris
    if (count(dev_name.begin(), dev_name.end(), '/') != 2)
#else
    if (_sol::count(dev_name.begin(), dev_name.end(), '/') != 2)
#endif /* _solaris */
	{
		cerr << "db_devres : Bad device name" << endl;
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
// Ask device resources to database server
// Display error message if the call fails
//
    char *tmp = const_cast<char *>(dev_name.c_str());
    char **res_list;
    long res_nb;
    if (db_deviceres(1, &tmp, &res_nb, &res_list, &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
		exit(-1);
	}

//
// Print device resources
//

	if (res_nb == 0)
		cout << "The device " << dev_name << " does not have any resource defined in the database"<< endl;
	else
	for (int i = 0; i < res_nb; i++)
			cout << res_list[i] << endl;
    return 0;
	}
