#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>


/**
 * close the database to be able to reload a new database.
 *
 * @return a pointer to an integer. This integer is simply an error code (0 if no error).
 */
DevLong *MySQLServer::db_clodb_1_svc(void)
{
    errcode = 0;
#ifdef DEBUG
    std::cout << "db_clodb()" << std::endl;
#endif 
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	std::cerr << "I'm not connected to database." << std::endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Disconnect server from database files 
//
    mysql_close(mysql_conn);
    dbgen.connected = false;
//
// Leave server 
//
    return(&errcode);
}

/**
 * Reopen the database after it has been updated by a dbm_update command in a 
 * single user mode or after the rebuilding from a backup file.
 * 
 * @returns a pointer to an integer. This integer is simply an error code (0 if no error).
 */
DevLong *MySQLServer::db_reopendb_1_svc(void)
{
#ifdef DEBUG
    std::cout << "db_reopendb" << std::endl;
#endif
    errcode = 0;
//
// Open database tables according to the definition
//
    if ((mysql_conn = mysql_real_connect(&mysql, "localhost", mysql_user.c_str(), mysql_passwd.c_str(), mysql_db.c_str(), 0, NULL, 0)) == NULL)
    {
        std::cerr << mysql_error(&mysql) << std::endl;
	errcode = DbErr_DatabaseAccess;
        return (&errcode);
    }
//
// Mark the server as connected to the database 
//
    dbgen.connected = true;
//
// Leave server 
//
    return(&errcode);
}
