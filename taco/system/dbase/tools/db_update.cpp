/* TACO include file */

#include <API.h>

/* Include files */
#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

/****************************************************************************
*                                                                           *
*		Code for db_update command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To delete a device and its resources from the database. *
*                   The -r option is used if the user does not want device  *
*		    resources to be also deleted			    *
*                                                                           *
*    Synopsis : db_update <file name>               		    	    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	long 	error;
	char 	*ptr;
	long 	nb_dev,
        	nb_res,
        	err_line,
        	dev_err;
	char 	**dev_def,
		**res_def;
	long 	sec = False;
	char 	*pa;
	long 	pass = True;
	char 	*answer;
//
// Argument test and device name structure
//
	if (argc != 2)
	{
		cerr << "db_update usage : db_update <file name>" << endl;
		exit(-1);
	}

	string file_name(argv[1]);

//
// Get environment variable
//

	if ((ptr = (char *)getenv("RES_BASE_DIR")) == NULL)
	{
		cerr << "Can't find environment variable RES_BASE_DIR" << endl;
		exit(-1);
	}
	string base(ptr);
//
// Build real file name path
//

	base.append(1,'/');
	file_name.insert(0,base);
#ifdef DEBUG
	cout  << "File name : " << file_name << endl;
#endif /* DEBUG */
//
// Connect to database server
//

	if (db_import(&error) == DS_NOTOK)
	{
		cerr << "db_devinfo : Impossible to connect to database server" << endl;
		exit(-1);
	}

//
// Analyse resource file
// Display error message if the analysis fails
//
	if (db_analyze_data(Db_File, file_name.c_str(), &nb_dev, &dev_def, &nb_res, &res_def, &err_line, &error) == DS_NOTOK)
	{
		if (err_line != 0)
		{
			if (error == DbErr_BadDevSyntax)
				cerr << "Error in device definition" << endl;
			else
			{
				cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
		}
		else
		{
			cerr << "File analysis failed with error " << error << endl;
	    		cerr << "Error at line " << err_line << " in file " << file_name << " (" << error << ")" << endl;
			cerr << "Error message : " << dev_error_str(error) << endl;
		}
		exit(-1);
	}
#ifdef DEBUG
	cout << "File analysis is OK" << endl;

	cout << nb_dev << " device list defined" << endl;	
	for (int i = 0; i < nb_dev; i++)
		cout << dev_def[i] << endl;
	cout << nb_res << " resources defined" << endl;
	for (int i = 0; i < nb_res; i++)
		cout << res_def[i] << endl;
#endif /* DEBUG */

//
// Check if there is any security resources
//
	for (int i = 0; i < nb_res; i++)
	{
		string str(res_def[i]);
		string::size_type pos;

		if ((pos = str.find('/')) == string::npos)
		{
			cerr << "Wrong resource syntax !!!, exiting" << endl;
			exit(-1);
		}
	
		string domain(str,0,pos);					
		if (domain == "sec")
		{
			sec = True;
			break;
		}
	}
//
// If some security resources are defined, check if a password is used
//

	if (sec == True)
	{
		if (db_secpass(&pa, &error) == DS_NOTOK)
		{
			if (error == DbErr_NoPassword)
				pass = False;
			else
			{
				cerr << "Can't retrieve the database defined password, update not allowed" << endl;
				exit(-1);
			}
		}
		
		if (pass == True)
		{
			answer = (char *)getpass("Security passwd : ");
			if (strcmp(answer,pa) != 0)
			{
				cout << "Sorry, wrong password. Update not allowed" << endl;
				cout << "Hint : Remove security resources from file" << endl;
				exit(-1);
			}
		}
	}
//
// Update device list in db
//

	if (nb_dev != 0)
	{
//
// Display message if this call fails
//
		if (db_upddev(nb_dev, dev_def, &dev_err, &error) == DS_NOTOK)
		{
			if (dev_err != 0)
			{
				cerr << "Error in device definition number " << dev_err << ", error = " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			else
			{
				cerr << "Device(s) update call failed with error " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			exit(-1);
		}
	}
	
#ifdef DEBUG
	cout << "Update device successfull" << endl;
#endif /* DEBUG */

//
// Update resources in db
//

	if (nb_res != 0)
	{
//
// Display message if this call fails
//
		if (db_updres(nb_res, res_def, &dev_err, &error) == DS_NOTOK)
		{
			if (dev_err != 0)
			{
				cerr << "Error in resource definition number " << dev_err << ", error = " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
			else
			{
				cerr << "Resource(s) update call failed with error " << error << endl;
				cerr << "Error message : " << dev_error_str(error) << endl;
			}
		}
	}	
	return 0;
}
