#include <DevErrors.h>
#include <MySqlServer.h>



/****************************************************************************
*                                                                           *
*	Server code for the devdomainlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To device domain list for all the device name defined    *
*		   in the NAMES and PS_NAMES tables			    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::devdomainlist_1_svc(void)
{
    vector<string>	dom_list;
	
#ifdef DEBUG
    cout << "In devdomainlist_1_svc function" << endl;
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
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the domain name list from the NAMES table
//
    string query = "SELECT DISTINCT DOMAIN FROM NAMES ORDER BY DOMAIN ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
    while((row = mysql_fetch_row(result)) != NULL)
    	dom_list.push_back(row[0]);
    mysql_free_result(result);

    query = "SELECT DISTINCT DOMAIN FROM PS_NAMES ORDER BY DOMAIN ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    result = mysql_store_result(mysql_conn);
    while((row = mysql_fetch_row(result)) != NULL)
    	dom_list.push_back(row[0]);
    mysql_free_result(result);

//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = dom_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
	int	k = dom_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        dom_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#ifdef DEBUG
    cout << "Found the following domains" << endl;
    for (vector<string>::iterator it = dom_list.begin(); it != dom_list.end(); ++it)
	cout << *it << endl;
    cout << endl;
#endif
//
// Return data
//
    return(&browse_back);
}

/****************************************************************************
*                                                                           *
*	Server code for the devfamilylist_1_svc function    	    	    *
*                           -------------------                    	    *
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
db_res *MySQLServer::devfamilylist_1_svc(nam * domain)
{
    string		user_domain(*domain);
    vector<string>	fam_list;
	
#ifdef DEBUG
    cout << "In devfamilylist_1_svc function for domain " << *domain << endl;
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
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the family name list for the wanted domain in the NAMES table
//
    string query = "SELECT DISTINCT FAMILY FROM NAMES WHERE DOMAIN = '" + user_domain + "' ORDER BY FAMILY ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
    while((row = mysql_fetch_row(result)) != NULL)
    	fam_list.push_back(row[0]);
    mysql_free_result(result);
//
// Add family name list for the wanted domain in from the PS_NAMES table
//
    query = "SELECT DISTINCT FAMILY FROM PS_NAMES WHERE DOMAIN = '" + user_domain + "' ORDER BY FAMILY ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    result = mysql_store_result(mysql_conn);
    while((row = mysql_fetch_row(result)) != NULL)
    	fam_list.push_back(row[0]);
    mysql_free_result(result);
//
// Sort family name list
//

//
// Build the sequence returned to caller
//
    int length = browse_back.res_val.arr1_len = fam_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];

    for (int i = 0;i < length;i++)
    {
	int	k = fam_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        fam_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#ifdef DEBUG
    cout << "Found the following families in the domain " << user_domain << endl;
    for (vector<string>::iterator it = fam_list.begin(); it != fam_list.end(); ++it)
	cout << *it << endl;
    cout << endl;
#endif
//
// Return data
//
    return(&browse_back);	
}

/****************************************************************************
*                                                                           *
*	Server code for the devmemberlist_1_svc fucntion 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To device member list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain and family  *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::devmemberlist_1_svc(db_res *recev)
{
    vector<string> memb_list;
//
// Build strings from input names
//
    string user_domain(recev->res_val.arr1_val[0]);
    string user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
    cout << "In devmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << endl;
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
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the member name list for the wanted domain and family from NAMES table
//
    string query = "SELECT DISTINCT MEMBER FROM NAMES WHERE DOMAIN = '" + user_domain;
    query += "' AND FAMILY = '" + user_family + "' ORDER BY MEMBER ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
    while((row = mysql_fetch_row(result)) != NULL)
    	memb_list.push_back(row[0]);
    mysql_free_result(result);
//
// Add member name for the wanted domain and family from PS_NAMES table
// Sort member name list
//
    query = "SELECT DISTINCT MEMBER FROM PS_NAMES WHERE DOMAIN = '" + user_domain;
    query += "' AND FAMILY = '" + user_family + "' ORDER BY MEMBER ASC";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    result = mysql_store_result(mysql_conn);
  
    while((row = mysql_fetch_row(result)) != NULL)
    	memb_list.push_back(row[0]);
    mysql_free_result(result);
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = memb_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];

    for (int i = 0; i < length; i++)
    {
	int	k = memb_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        memb_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
//
// Return data
//
    return(&browse_back);	
}
