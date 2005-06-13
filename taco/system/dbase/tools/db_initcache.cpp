/* TACO include file */

#include <API.h>

/* Include files */
#include <string>
#include <iostream>
#include <string>

using namespace std;

void usage(const char *cmd)
{
	cerr << "usage : " << cmd << " [options] <domain name>" << endl;
	cerr << " Initialize a database server resource cache" << endl;
	cerr << " This command clears the old cache and (re)initialize it" << std::endl;
	cerr << " with the contents of the RES database table for the" << std::endl;
	cerr << " wanted domain" << std::endl;
	cerr << "      options : -h display this message" << std::endl;
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for db_initcache command                               *
*                        ------------                                       *
*                                                                           *
*    Command rule : To initialize a database server resource cache          *
*		    This command clears the old cache and (re)initialize it *
*		    with the contents of the RES database table for the     *
*		    wanted domain					    *
*                                                                           *
*    Synopsis : db_initcache [ domain name ]               		    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long error;
        extern char     *optarg;
        extern int      optind,
                        opterr,
                        optopt;
        int             c;


// Argument test and domain name modification
        while((c = getopt(argc, argv, "h")) != -1)
                switch(c)
                {
                        case 'h':
                        case '?':
				usage(argv[0]);
		}
//
// Argument test and device name structure
//
	if (optind != argc - 1)
		usage(argv[0]);

	string dom_name(argv[1]);

#ifdef DEBUG
	cout  << "Domain name : " << dom_name << endl;
#endif 
//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_initcache : Impossible to connect to database server" << endl;
		exit(-1);
	}
//
// Display error message if the call fails
// Init. cache
//
	if (db_initcache(dom_name.c_str(), &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
		exit(-1);
	}
	return 0;
}


