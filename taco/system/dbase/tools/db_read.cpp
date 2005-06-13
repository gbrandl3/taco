#include <cstdio>
#include <string>
#include <cstdlib>
#include <sys/wait.h>

#define _db_setup_h
#include <API.h>
#undef _db_setup_h
#include "db_setup.h"

/* For database only */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include "config.h"
#include <gdbm.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/* Function definitions */
int db_read(std::string , std::string);

/* Global variables definitions */
int line_ptr;

void usage(const char *cmd)
{
	std::cerr << " usage: " << cmd << " [options] <domain name|all>" << std::endl;
	std::cerr << " Read all resource from specified or all domains" << std::endl;
	std::cerr << "     options : -h display this message" << std::endl;
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for db_read command 				    *
*                        -------                                            *
*                                                                           *
*    Command rule : To read all device servers and resources of a domain    *
*		    (table) or all domains (tables)                         *
*                   in the order they are stored in dbm database.           *
*                   The database directory has to be given by the           *
*                   environment variable DBM_DIR.                           *
*                                                                           *
*    Synopsis : db_read [domain/all]                                        *
*                                                                           *
****************************************************************************/
int main(int argc,char **argv)
{
	char				*ptr;
	std::vector<int> 		res_num;
	std::vector<std::string> 	TblName;
	bool 				names(False),
					ps_names(False);
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

	std::string	domain(argv[1]);
	std::transform(domain.begin(), domain.end(), domain.begin(), ::tolower);
//
// Find the dbm_database table names and
// change the database table names to lowercase letter names and check if there 
// is a names and ps_names tables defined 
//
	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		std::cerr << "db_read: Can't find environment variable DBTABLES" << std::endl;
		exit(-1);
	}

	std::string		dbtables(ptr);
	std::string::size_type	pos;
	while ((pos = dbtables.find(',')) != std::string::npos)
	{
		std::string	tblname = dbtables.substr(0, pos);
		std::transform(tblname.begin(), tblname.end(), tblname.begin(), ::tolower);
		TblName.push_back(tblname);
		dbtables.erase(0, pos + 1);
	}
	std::transform(dbtables.begin(), dbtables.end(), dbtables.begin(), ::tolower);
	TblName.push_back(dbtables);
	for (std::vector<std::string>::iterator it = TblName.begin(); it != TblName.end(); ++it)
	{
		if (*it == "names")
			names = true;
		if (*it == "ps_names")
			ps_names = true;
	}
//
// If no names or ps_names tables are defined, add them to the list 
//
	if (!names)
		TblName.push_back(std::string("names"));
	if (!ps_names)
		TblName.push_back(std::string("ps_names"));
//
// Take the environment variable DBM_DIR 
//
	if ((ptr = getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "db_read: Can't find environment variable DBM_DIR" << std::endl;
		exit(-1);
	}
	std::string dbm_dir(ptr);

	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += '/';
//
// Read the database tables of the database 
//
	for (std::vector<std::string>::iterator it = TblName.begin(); it != TblName.end(); ++it)
		if ((domain == *it) || (domain == "all")) 
		{
			std::string dbm_file = dbm_dir + *it;
			res_num.push_back(db_read(dbm_file, *it));
			if (domain != "all") 
				break;
		}
	return 0;
}

int db_read(std::string dbm_file, std::string tblname)
{
	int 		flags = GDBM_READER | GDBM_NOLOCK;
	GDBM_FILE	tab_tid;
	datum 		content;
	int 		res_num = 0;
	std::string	TblName(tblname);
	
	std::transform(tblname.begin(), tblname.end(), TblName.begin(), ::toupper);
//
// Open database file 
//
	if ((tab_tid = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, (int)0666, NULL)) == NULL)
	{
		std::cerr << "db_read: Can't open " << dbm_file << " table" << std::endl;
		exit(-1);
	}	
//
// Display table contents 
//
	datum	key2;
	for (datum key = gdbm_firstkey(tab_tid); 
		key.dptr != NULL;
		key2 = key, key = gdbm_nextkey(tab_tid, key2), free(key2.dptr))
	{
		content = gdbm_fetch(tab_tid, key);
		if (content.dptr != NULL)
		{
			std::string	content_out(content.dptr, content.dsize),
					key_out(key.dptr, key.dsize);
			res_num++;
			std::cout << TblName << ": " << key_out << ": " << content_out << std::endl;
		}
	}
//
// Close database 
//
	gdbm_close(tab_tid);
	return(res_num);
}
