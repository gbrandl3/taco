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

/****************************************************************************
*                                                                           *
*		Code for db_servdel command                                 *
*                        ---------                                          *
*                                                                           *
*    Command rule : To delete all the devices and their resources from the  *
*		    database. 						    *
*                   The -r option is used if the user does not want device  *
*		    resources to be also deleted			    *
*                                                                           *
*    Synopsis : db_servdel [-r] <full server name>               	    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long error;
	long del_res = True;
	long opt = False;
	extern int optind;
	int c;

//
// Argument test and device name structure
//
	while ((c = getopt(argc,argv,"r")) != -1)
	{
		switch (c)
		{
		case 'r':
			del_res = False;
			break;
			
		case '?':
			cerr << "db_servdel usage : db_servdel [-r] <full device server name>" << endl;
			exit(-1);
			break;
		}
	}

	if (optind != argc - 1)
	{
		cerr << "db_servdel usage : db_servdel [-r] <full device server name>" << endl;
		exit(-1);
	}
	string full_ds_name(argv[optind]);

#ifdef DEBUG
	cout  << "Full device server name : " << full_ds_name << endl;
#endif 
#ifndef _solaris
	if (count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#else
	if (_sol::count(full_ds_name.begin(), full_ds_name.end(), '/') != 1)
#endif /* _solaris */
	{
		cerr << "db_servdel : Bad full device server name" << endl;
		exit(-1);
	}
//
// Extract device server and personal name from full device server
// name
//

	string::size_type pos,start;

	if ((pos = full_ds_name.find('/')) == string::npos)
	{
		cout << "db_servdel : Can't split full device server" << endl;
		exit(-1);
	}
	
	string ds_name(full_ds_name,0,pos);
	string pers_name(full_ds_name.substr(pos + 1 ));
	
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
// Ask database server to remove device
// Display error message if the call fails
//
	if (db_servdelete(ds_name.c_str(), pers_name.c_str(), del_res, &error) == -1)
	{
		if (error == DbErr_DeviceServerNotDefined)
			cerr << "Server " << full_ds_name << " does not have any device in database" << endl;
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
	return 0;
}
