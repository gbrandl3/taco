#include <DevErrors.h>
#include <NdbmServer.h>


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
DevLong *NdbmServer::db_clodb_1_svc()
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
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Disconnect server from database files 
//
    for (int i = 0; i < dbgen.TblNum; i++) 
	gdbm_close(dbgen.tid[i]);
    dbgen.connected = False;
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
DevLong *NdbmServer::db_reopendb_1_svc()
{
    char 	*ptr;

#ifdef DEBUG
    cout << "db_reopendb" << endl;
#endif
    errcode = 0;
//
// Find the dbm_database files 
//
    if ((ptr = (char *)getenv("DBM_DIR")) == NULL)
    {
	cerr << "dbm_server: Can't find environment variable DBM_DIR" << endl;
	errcode = DbErr_DatabaseAccess;
	return(&errcode);
    }

    string dir_name(ptr);
    if (dir_name[dir_name.size() - 1] != '/')
		dir_name.append(1,'/');
//
// Open database tables according to the definitions  
//
    int flags = O_RDWR;
    for (int i = 0;i < dbgen.TblNum;i++)
    {
	string dbm_file(dir_name);
	dbm_file.append(dbgen.TblName[i]);
	string uni_file(dbm_file);
	uni_file.append(".dir");
	ifstream fi(uni_file.c_str());
	if (!fi)
	{
	    cerr << "dbm_clo_op : Can't find file " << uni_file << endl;
	    errcode = DbErr_DatabaseAccess;
	    return(&errcode);
	}

	dbgen.tid[i] = gdbm_open((char *)dbm_file.c_str(), 0, flags, 0666, NULL);
	if (dbgen.tid[i] == NULL)
	{
	    cerr <<"dbm_server : Can't open " << dbgen.TblName[i] << " table"; 
	    errcode = DbErr_DatabaseAccess;
	    return(&errcode);
	}
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
