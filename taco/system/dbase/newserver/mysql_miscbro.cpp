#include <DevErrors.h>
#include <MySqlServer.h>


/****************************************************************************
*                                                                           *
*	Server code for the devserverlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To retrieve device server list for all the devices       *
*		   defined in the NAMES table			  	    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::devserverlist_1_svc()
{
    string 		server;
    vector<string> 	serv_list;
	
#ifdef DEBUG
    cout << "In devserverlist_1_svc function" << endl;
#endif

//
// Initialize structure sent back to client
//
    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the server name list from the NAMES table
//
//
// Sort device server name list
//
    string query = "SELECT DISTINCT DEVICE_SERVER_CLASS FROM NAMES ORDER BY DEVICE_SERVER_CLASS ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return(&browse_back);			
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
	serv_list.push_back(row[0]);		
    mysql_free_result(result);
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = serv_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = serv_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        serv_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                                                        
#if 0
    if (serv_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in devserverlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);
}


/****************************************************************************
*                                                                           *
*	Server code for the devpersnamelist_1_svc function    	    	    *
*                           ---------------------                    	    *
*                                                                           *
*    Method rule : To device family list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain		    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *MySQLServer::devpersnamelist_1_svc(nam *server)
{
    string 		user_server(*server);
    vector<string>	pers_list;
	
#ifdef DEBUG
    cout << "In devpersnamelist_1_svc function for server " << user_server << endl;
#endif

//
// Initialize structure sent back to client
//
    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the family name list for the wanted domain in the NAMES table
//
//
// Sort device server personal name list
//
    string query = "SELECT DISTINCT DEVICE_SERVER_NAME FROM NAMES WHERE DEVICE_SERVER_CLASS = '";
    query += (user_server + "' ORDER BY DEVICE_SERVER_NAME ASC");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return(&browse_back);			
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
	pers_list.push_back(row[0]);
    mysql_free_result(result);
//
// Build the sequence returned to caller
//
    int length = browse_back.res_val.arr1_len = pers_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = pers_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        pers_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                                                        
#if 0
    if (pers_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
		cerr << "Memory allocation error in devpersnamelist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);	
}



/****************************************************************************
*                                                                           *
*	Server code for the hostlist_1_svc function       	   	    *
*                           --------------                       	    *
*                                                                           *
*    Method rule : To device host list where device server should run       *		   in the NAMES table			  		    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::hostlist_1_svc()
{
    vector<string>	host_list;
	
#ifdef DEBUG
    cout << "In hostlist_1_svc function" << endl;
#endif

//
// Initialize structure sent back to client
//

    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
	
//
// If the server is not connected to the database, return error
//

    if (dbgen.connected == False)
    {
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
    }

//
// Get the host name list from the NAMES table for all exported devices (for DS with DB library release 4)
// and sort device server host list
//
    string query = "SELECT DISTINCT HOSTNAME FROM NAMES WHERE HOSTNAME != 'not_exp' ORDER HOSTNAME ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return(&browse_back);			
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
	host_list.push_back(row[0]);
    mysql_free_result(result);
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = host_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = host_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        host_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                                                        
#if 0
    if (host_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
		cerr << "Memory allocation error in hostlist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);
}
