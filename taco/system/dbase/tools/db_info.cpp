/* TACO include file */

#include <API.h>

/* Include files */
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

/****************************************************************************
*                                                                           *
*		Code for db_info command                                    *
*                        -------                                            *
*                                                                           *
*    Command rule : To get device and resources informations from the static*
*		    database using ndbm package				    *
*                                                                           *
*    Synopsis : db_info              		   			    *
*                                                                           *
****************************************************************************/
int main(int argc,char *argv[])
{
	long error;
	db_stat_call inf;
	time_t sec;
	char *ti;

//
// Connect to database server
//

	if (db_import(&error) == -1)
	{
		cerr << "db_devinfo : Impossible to connect to database server" << endl;
		exit(-1);
	}
//
// Ask database server for informations
// Display error message if the call fails
//
	if (db_stat(&inf, &error) == -1)
	{
		cerr << "The call to database server failed with error " << error << endl;
		cerr << "Error message : " << dev_error_str(error) << endl;
		exit(-1);
	}
//
// Get time
//
	sec = time((time_t *)0);
	ti = ctime(&sec);

//
// Convert domain name to upper case letter
//
	for (int i = 0; i < inf.dev_domain_nb; i++)
	{
		int l = strlen(inf.dev_domain[i].dom_name);
		for (int j = 0; j < l; j++)
			inf.dev_domain[i].dom_name[j] = toupper(inf.dev_domain[i].dom_name[j]);
	}
	for (int i = 0; i < inf.res_domain_nb; i++)
	{
		int l = strlen(inf.res_domain[i].dom_name);
		for (int j = 0; j < l; j++)
			inf.res_domain[i].dom_name[j] = toupper(inf.res_domain[i].dom_name[j]);
	}
//
// Displays info
//
	cout << "\t\t" << ti;
	cout << "\t\tDEVICE STATISTICS" << endl << endl;
	cout << inf.dev_defined << " devices are defined in database" << endl;
	cout << inf.dev_exported << " of the defined devices are actually exported:" << endl;
	for (int i = 0; i < inf.dev_domain_nb; i++)
		cout << "    " << inf.dev_domain[i].dom_elt << " for the " << inf.dev_domain[i].dom_name << " domain" << endl;
	cout << inf.psdev_defined << " pseudo devices are defined in the database" << endl;
	cout << endl;
	cout << "\t\tRESOURCE STATISTICS" << endl << endl;
	cout << inf.res_number << " resources are defined in database:" << endl;
	for (int i = 0; i < inf.res_domain_nb; i++)
		cout << "    " << inf.res_domain[i].dom_elt << " resources for the " << inf.res_domain[i].dom_name << " domain" << endl;
	return 0;
}


