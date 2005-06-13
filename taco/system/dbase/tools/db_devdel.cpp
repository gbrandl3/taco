/* TACO include file */

#include <API.h>

/* Include files */
#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " deletes a device and (optional) its resources from the database. " << std::endl;
	std::cerr << "        options : -r delete also resources of the device" << std::endl;
	std::cerr << "                  -h display this message" << std::endl;
	exit(1);
}

/****************************************************************************
*                                                                           *
*		Code for db_devdel command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To delete a device and its resources from the database. *
*                   The -r option is used if the user does not want device  *
*		    resources to be also deleted			    *
*                                                                           *
*    Synopsis : db_devdel [-r] <device name>               		    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long 		error;
	long 		del_res = True;
	extern int 	optopt;
	extern int	optind;
	int 		c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"rh")) != -1)
	{
		switch (c)
		{
		case 'r':
			del_res = False;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			break;
		}
	}
    	if (optind != argc - 1)
		usage(argv[0]);

    	string dev_name(argv[optind]);
#ifdef DEBUG
	cout  << "Device name : " << dev_name << endl;
#endif /* DEBUG */
#ifndef _solaris
	if (std::count(dev_name.begin(), dev_name.end(), '/') != 2)
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
// Ask database server to remove device
// Display error message if the call fails
//
    if (db_devicedelete(dev_name.c_str(), &error) == -1)
	{
		if (error == DbErr_DeviceNotDefined)
			cerr << "Device " << dev_name << " is not defined in the database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}

//
// Delete device resources if it is wanted
//

	if (del_res == True)
	{
	char *tmp = const_cast<char *>(dev_name.c_str());
    	db_error 	err;
	if (db_devicedeleteres(1, &tmp, &err) == -1)
		{
			cerr << "The call to database server to delete device resources failed with error " << err.error_code << endl;
			cerr << "Error message : " << dev_error_str(err.error_code) << endl;
		}
	}
    return 0;
}


