#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>

/****************************************************************************
*                                                                           *
*		Server code for dbm_close function                          *
*                               --------                                    *
*                                                                           *
*    Function rule : To close the database to be able to reload a new       *
*		     database.						    *
*                                                                           *
*    Argin :  No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to an integer. This integer is simply  *
*    an error code (0 if no error).					    * 
*                                                                           *
****************************************************************************/
DevLong *MySQLServer::db_clodb_1_svc(void)
{
    errcode = 0;
#ifdef DEBUG
    cout << "db_clodb()" << endl;
#endif 
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
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

/****************************************************************************
*                                                                           *
*		Server code for dbm_reopendb function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : Reopen the database after it has been updated by a     *
*		     dbm_update command in a single user mode or after      *
*		     the rebuilding from a backup file.			    *
*									    *
*                                                                           *
*    Argin :  No argin							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to an integer. This integer is simply  *
*    an error code (0 if no error).					    * 
*                                                                           *
****************************************************************************/
DevLong *MySQLServer::db_reopendb_1_svc(void)
{
#ifdef DEBUG
    cout << "db_reopendb" << endl;
#endif
    errcode = 0;
//
// Open database tables according to the definition
//
    if ((mysql_conn = mysql_connect(&mysql, "localhost", mysql_user.c_str(), mysql_passwd.c_str())) == NULL)
    {
        cerr << mysql_error(&mysql) << endl;
	errcode = DbErr_DatabaseAccess;
        return (&errcode);
    }
    if (mysql_select_db(mysql_conn, mysql_db.c_str()) != 0)
    {
        cerr << mysql_error(mysql_conn) << endl;
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
