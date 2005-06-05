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
*		Code for db_servinfo command                                *
*                        -----------                                        *
*                                                                           *
*    Command rule : To display device server info			    *
*                                                                           *
*    Synopsis : db_servinfo [ full device server name ]                     *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long error;

/* Argument test and device name structure*/

	if (argc != 2)
	{
		cerr << "db_servinfo usage : db_servinfo <full device server name>" << endl;
		exit(-1);
	}

	string full_ds_name(argv[1]);

#ifdef DEBUG
	cout  << "Server name : " << full_ds_name << endl;
#endif /* DEBUG */
#ifndef _solaris 
    if (count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
    if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		cerr << "db_servinfo : Bad full device server name" << endl;
		exit(-1);
	}

//
// Extract device server and personal name from full device server
// name
//

	string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == string::npos)
	{
		cout << "db_servinfo : Can't split full device server" << endl;
		exit(-1);
	}
	
	string ds_name(full_ds_name,0,pos);
    string pers_name(full_ds_name.substr(pos + 1)); 
#ifdef DEBUG
	cout << "DS name : " << ds_name << endl;
	cout << "Pers. name : " << pers_name << endl;
#endif /* DEBUG */
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_devinfo : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Ask database server for server info
// Display error message if the call fails
//
	db_svcinfo_call info;
	if (db_servinfo(ds_name.c_str(), pers_name.c_str(), &info, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			cout << "The device server " << full_ds_name << " is not defined in database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
//
// Display info sent back by server
//

	string pro(info.process_name);
	if (info.pid != 0)
	{
		if (pro != "unknown")
			cout << "Device server " << full_ds_name << " is part of the process " << info.process_name << endl;
		cout << "The process is running on host " << info.host_name << " with PID " << info.pid 
	     		<< " (Program number = " << info.program_num << ")" << endl;
	}
	
	for (int i = 0; i < info.embedded_server_nb; i++)
	{
		if (info.embedded_server_nb != 1)
		{
			cout << endl;
			cout << "Device server class : " << info.server[i].server_name << endl;
		}
		for (int j = 0; j < info.server[i].device_nb; j++)
		{
			if (info.server[i].device[j].exported_flag == True)
				cout << "Device number " << j << " : " << info.server[i].device[j].dev_name 
					<< " exported from host " << info.host_name << endl;
			else
				cout << "The device " << info.server[i].device[j].dev_name 
					<< " is defined for this server but is not exported" << endl;
		}
	}
	return 0;
}


