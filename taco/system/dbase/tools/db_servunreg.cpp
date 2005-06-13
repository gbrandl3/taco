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
	cerr << "usage : " << cmd << " [options] <full device server name>" << endl;
	cerr << " Unregister a device server from the static database." << std::endl;
	cerr << "      options : -h display this message" << std::endl;
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for db_servunreg command                               *
*                        ------------                                       *
*                                                                           *
*    Command rule : To unregister a device server from the static database. *
*                                                                           *
*    Synopsis : db_servunreg [ full device server name ]                    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long i;
	long error;
	db_devinfo_call info;
        extern int      optopt;
        extern int      optind;
        int             c;

//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"h")) != -1)
                switch (c)
                {
                        case 'h':
                        case '?':
                                usage(argv[0]);
                                break;
                }
        if (optind != argc - 1)
                usage(argv[0]);

	string full_ds_name(argv[optind]);

#ifdef DEBUG
	cout  << "Server name : " << full_ds_name << endl;
#endif /* DEBUG */

#ifndef _solaris
	if (std::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		cerr << "db_servunreg : Bad full device server name" << endl;
		exit(-1);
	}

//
// Extract device server and personal name from full device server
// name
//
	string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == string::npos)
	{
		cout << "db_servunreg : Can't split full device server" << endl;
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
// Ask database server to unregister server
// Display error message if the call fails
//
	if (db_servunreg(ds_name.c_str(), pers_name.c_str(), &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			cout << "The device server " << full_ds_name << " does not have any device in the database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
	return 0;
}


