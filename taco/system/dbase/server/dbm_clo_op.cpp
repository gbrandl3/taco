#include <API.h>

#include <DevErrors.h>

#include <db_xdr.h>

#include <fcntl.h>

/* Some C++ include files */

#include <string>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <iostream>
#include <fstream>
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif


/**
 * To close the database to be able to reload a new database.
 *
 * @return  This function returns a pointer to an integer. This integer is simply
 *    an error code (0 if no error).
 */
DevLong *NdbmServer::db_clodb_1_svc()
{
	static DevLong errcode;

#ifdef DEBUG
	std::cout << "db_clodb()" << std::endl;
#endif 
//
// Return error code if the server is not connected to the database files
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Disconnect server from database files
//
	for (int i = 0;i < dbgen.TblNum;i++)
		gdbm_close(dbgen.tid[i]);
	dbgen.connected = False;
//
// Leave server 
//
	errcode = 0;
	return(&errcode);
}



/**
 * Reopen the database after it has been updated by a dbm_update command in 
 * a single user mode or after the rebuilding from a backup file.
 *
 * @return This function returns a pointer to an integer. This integer is simply
 *    an error code (0 if no error).
 */
DevLong *NdbmServer::db_reopendb_1_svc()
{
	static DevLong 	errcode;
	int 		flags = O_RDWR;;
	char 		*ptr;

#ifdef DEBUG
	std::cout << "db_reopendb" << std::endl;
#endif 
//
// Find the dbm_database files
//
	if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "dbm_server: Can't find environment variable DBM_DIR" << std::endl;
		errcode = DbErr_DatabaseAccess;
		return(&errcode);
	}

	std::string dir_name(ptr);
	if (dir_name[dir_name.size() - 1] != '/')
		dir_name.append(1,'/');
//
// Open database tables according to the definitions
//
	for (int i = 0;i < dbgen.TblNum;i++)
	{
		std::string dbm_file(dir_name);
		dbm_file.append(dbgen.TblName[i]);
		
		std::ifstream fi(dbm_file.c_str());
		if (!fi)
		{
			umask(0);
			GDBM_FILE t = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, GDBM_WRCREAT, 0666, NULL);
                	if (t == NULL)
                	{
				std::cerr << "dbm_clo_op : Can't create file " << dbm_file << std::endl;
				errcode = DbErr_DatabaseAccess;
				return(&errcode);
                	}
			gdbm_close(t);
		}

		dbgen.tid[i] = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, 0666, NULL);
		if (dbgen.tid[i] == NULL)
		{
			std::cerr <<"dbm_server : Can't open " << dbgen.TblName[i] << " table" << std::endl; 
			errcode = DbErr_DatabaseAccess;
			return(&errcode);
		}
	} 
//
// Mark the server as connected to the database
//
	dbgen.connected = True;
//
// Leave server
//
	errcode = 0;
	return(&errcode);
}
