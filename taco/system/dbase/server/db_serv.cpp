#include "config.h"
#include <dbClass.h>
#include <algorithm>
#include <string>
#include <unistd.h>


DBServer::DBServer()
{
//
// Find the dbm_database table names
//
	char 	*ptr;
	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		std::cerr << "dbm_server: Can't find environment variable DBTABLES" << std::endl;
		exit(-1);
	}
//
// Automatically add a names and a ps_names tables
//
	dbgen.TblName.clear();
	dbgen.TblName.push_back("names");
	dbgen.TblName.push_back("ps_names");
	dbgen.ps_names_index = 1;
//
// Extract each table name
//
	std::string dbtables(ptr);
//
// Change database table names to lowercase letter names
//
	std::transform(dbtables.begin(), dbtables.end(), dbtables.begin(), ::tolower);
	std::string::size_type 	pos(0),
				start(0);

	while ((pos = dbtables.find(',', pos)) != std::string::npos)
	{
		std::string tmp_str(dbtables, start, pos - start);
		start = ++pos;
		if ((tmp_str == "names") || (tmp_str == "ps_names"))
			continue;
		dbgen.TblName.push_back(tmp_str);
	}
	dbgen.TblName.push_back(dbtables.substr(start));
	dbgen.TblNum = dbgen.TblName.size();
	return;
}

/**
 * Read the password from the password file
 *
 * @return the password as a resource for the system
 */
db_res *DBServer::secpass_1_svc()
{
	char pass[80];
	
#ifdef DEBUG
	std::cout << "In secpass_1_svc function" << std::endl;
#endif

//
// Initialize structure sent back to client
//
	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	pass[0] = '\0';
//
// Build security file name
//
	std::string f_name;
        if (getenv("SEC_DIR") != NULL)
        {
        	f_name = (char *)getenv("SEC_DIR");
	}
        else
	{
		f_name = (char *)getenv("HOME");
	}

	f_name.append("/.sec_pass");	
//
// Try to open the file
//
	std::ifstream f(f_name.c_str());
	if (!f)
	{
		browse_back.db_err = DbErr_NoPassword;
		return(&browse_back);
	}
//
// Get password
//
	f.getline(pass, sizeof(pass) - 1);
	if (strlen(pass) == 0)
	{
		browse_back.db_err = DbErr_NoPassword;
		return(&browse_back);
	}
//
// Init data sent back to client 
//	
	try
	{
		browse_back.res_val.arr1_val = new nam[1];
		browse_back.res_val.arr1_val[0] = new char [strlen(pass) + 1];
		strcpy(browse_back.res_val.arr1_val[0], pass);
	}
	catch (std::bad_alloc)
	{
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	browse_back.res_val.arr1_len = 1;
//
// Return data
//
	return(&browse_back);
}
