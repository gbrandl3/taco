/* TACO include file */

#include <API.h>

/* Include files */
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#ifdef _solaris
#include "_count.h"
#endif /* _solaris */

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <resource name>" << endl;
	cerr << " Delete a resource from the static database." << std::endl;
	cerr << "     options : -h display this message" << std::endl;
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for db_resdel command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To delete a resource from the static database.          *
*                                                                           *
*    Synopsis : db_resdel [ resource name ]               		    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long error;
        extern int      optopt;
        extern int      optind;
        int             c;

//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"rh")) != -1)
                switch (c)
                {
                        case 'h':
                        case '?':
                                usage(argv[0]);
                                break;
                }
        if (optind != argc - 1)
                usage(argv[0]);
	
	string full_res_name(argv[optind]);
	transform(full_res_name.begin(), full_res_name.end(), full_res_name.begin(), ::tolower);
//
// Test resource name syntax
//
#ifndef _solaris
	if (std::count(full_res_name.begin(), full_res_name.end(), '/') != 3)
#else
	if (_sol::count(full_res_name.begin(), full_res_name.end(), '/') != 3)
#endif /* _solaris */
	{
		cerr << "db_resdel : Bad resource name" << endl;
		exit(-1);
	}
//
// Extract device name from full resource name
//
	string::size_type pos = full_res_name.rfind('/');
	string res_name(full_res_name,pos + 1);
	string dev_name(full_res_name,0,pos);
#ifdef DEBUG
	cout << "Full resource name : " << full_res_name << endl;
	cout << "Device name : " << dev_name << endl;
	cout << "Resource name : " << res_name << endl;
#endif /* DEBUG */

//
// Test domain name and exit if the resource belongs to the SEC domain
//

	pos = full_res_name.find('/');
	string domain(full_res_name,0,pos);
	if (domain == "sec")
	{
		cout << "db_resdel : SEC is not a authorized domain name" << endl;
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
    char *tmp = const_cast<char *>(res_name.c_str());
    if (db_delresource(const_cast<char *>(dev_name.c_str()), &tmp, 1, &error) == -1)
	{
		if (error == DbErr_ResourceNotDefined)
		{
			cout << "The resource " << full_res_name << " does not exist in the database" << endl;
			exit(-1);
		}
		else
		{
			cerr << "The call to database server failed with error " << error << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
			exit(-1);
		}
	}
	return 0;
}


