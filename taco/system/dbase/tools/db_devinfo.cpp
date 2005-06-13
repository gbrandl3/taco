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

void usage(const char *cmd)
{
	std::cerr << "usage : " << cmd << " [options] <device name>" << std::endl;
	std::cerr << " display informations of the device" << std::endl;
	std::cerr << "          options: -h display this message" << std::endl;
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for db_devinfo command                                 *
*                        ----------                                         *
*                                                                           *
*    Command rule : To get device information from the static database.     *
*                                                                           *
*    Synopsis : db_devinfo [ device name ]               		    *
*                                                                           *
****************************************************************************/

int main(int argc,char *argv[])
{
	long 		error;
	db_devinfo_call info;
	char **dev_list;
	unsigned int dev_nb;
        extern int      optopt;
        extern int      optind;
        int             c;

//
// Argument test and device name structure
//
        while ((c = getopt(argc, argv, "h")) != -1)
                switch (c)
                {
                	case 'h':
                	case '?':
				usage(argv[0]);
                }

	if (optind != argc - 1)
		usage(argv[0]);

	string dev_name(argv[optind]);

#ifdef DEBUG
	cout  << "Device name : " << dev_name << endl;
#endif 
#ifndef _solaris
	if (std::count(dev_name.begin(), dev_name.end(), '/') != 2)
#else
	if (_sol::count(dev_name.begin(), dev_name.end(), '/') != 2)
#endif /* _solaris */
	{
		cerr << "db_devinfo : Bad device name" << endl;
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
// Check if there is any * used in the device name
//
	bool devexp = (dev_name.find('*') != string::npos);
	if (!devexp)
	{
//
// Ask device info to database server
// Display error message if the call fails
//
		if (db_deviceinfo(dev_name.c_str(), &info, &error) == -1)
		{
			if (error == DbErr_DeviceNotDefined)
			{
				cout << "The device " << dev_name << " is not defined in the database" << endl;
				exit(-1);
			}
			else
			{
				cerr << "The call to database server failed with error " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
				exit(-1);
			}
		}

		if (info.device_type == DB_Device)
		{
//
// Display message if the device is not exported
//
			if (info.device_exported == False)
			{
				cout << "The device " << dev_name << " is not actually exported" << endl;
				exit(-1);
			}
			else
			{
//
// Print results
//
				cout << "Device " << dev_name << " belongs to class : " << info.device_class << endl;
				cout << "It is monitored by the server : " << info.server_name <<"/" << info.personal_name << " version " << info.server_version << endl;
				if (strcmp(info.process_name,"unknown") != 0)
				{
					cout << "The device server process name is : " << info.process_name;
					if (info.program_num != 0)
						cout << " with program number : " << info.program_num;
					cout << endl;
				}
				cout << "The process is running on the computer : " << info.host_name;
				if (info.pid != 0)
					cout << " with process ID : " << info.pid;
				cout << endl;
			}
		}
		else
		{
//
// Display pseudo-device info
//
			cout << "Device " << dev_name << " is a pseudo device" << endl;
			cout << "It has been created by a process with PID : " << info.pid << " running on host : " << info.host_name << endl;
		}
	}
	else
	{
//
// Get a list of devices from database
// Display error message if the call fails
//
		if (db_getdevexp(const_cast< char *>(dev_name.c_str()), &dev_list, &dev_nb, &error) == -1)
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
			exit(-1);
		}
//
// Display device list
//
		for (int i = 0; i < dev_nb; i++)
			cout << dev_list[i] << endl;
	}
	return 0;
}
