#include <DevErrors.h>
#include <algorithm>
#include <MySqlServer.h>


/****************************************************************************
*                                                                           *
*	Server code for the resdomainlist_1_svc function     	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrive resource domain list for all the resources    *
*		   defined in the database				    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *                                                          *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::resdomainlist_1_svc()
{
    vector<string> 	dom_list;
	
#ifdef DEBUG
    cout << "In resdomainlist_1_svc function" << endl;
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
// Find each domain
//
    for (long i = 0;i < dbgen.TblNum;i++)
    {
	if ((dbgen.TblName[i] != "names") &&
	    (dbgen.TblName[i] != "ps_names"))
	    dom_list.push_back(dbgen.TblName[i]);
    }
//
// Sort domain name list
//
    sort(dom_list.begin(), dom_list.end());
//
// Bild the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = dom_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = dom_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        dom_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in resdomainlist" << endl;
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
*	Server code for the resfamilylist_1_svc function     	   	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific domain				    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *MySQLServer::resfamilylist_1_svc(nam* domain)
{
    vector<string> 	fam_list;
	
#ifdef DEBUG
    cout << "In resfamilylist_1_svc function for domain " << *domain << endl;
#endif

    string user_domain(*domain);
//
// Initialize structure sent back to client
//
    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, throw exception
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources family name defined in this table
//
    string query;
    if (mysql_db == "tango")
    {
    	query = "SELECT DISTINCT family FROM property_device WHERE domain = '";
    	query += (user_domain + "' ORDER BY family ASC");
    }
    else
    {
    	query = "SELECT DISTINCT FAMILY FROM RESOURCE WHERE DOMAIN = '";
    	query += (user_domain + "' ORDER BY FAMILY ASC");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES   *result = mysql_store_result(mysql_conn);
    MYSQL_ROW   row;
 
    while((row = mysql_fetch_row(result)) != NULL)
	fam_list.push_back(row[0]);
    mysql_free_result(result);                                                                            
//
// Sort family name list
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = fam_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = fam_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        fam_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                     
#if 0
    if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in resfamilylist" << endl;
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
*	Server code for the resmemberlist_1_svc function 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific couple domain,family			    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::resmemberlist_1_svc(db_res *recev)
{
    vector<string> 	memb_list;
    string 		user_domain(recev->res_val.arr1_val[0]),
     			user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
    cout << "In resmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << endl;
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
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources family name defined in this table
//
    string query;
    if (mysql_db == "tango")
    {
    	query = "SELECT DISTINCT member FROM property_device WHERE domain = '";
    	query += (user_domain + "' AND family = '" + user_family + "' ORDER BY member ASC");
    }
    else
    {
    	query = "SELECT DISTINCT MEMBER FROM RESOURCE WHERE DOMAIN = '";
    	query += (user_domain + "' AND FAMILY = '" + user_family + "' ORDER BY MEMBER ASC");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES   *result = mysql_store_result(mysql_conn);
    MYSQL_ROW   row;
 
    while((row = mysql_fetch_row(result)) != NULL)
	memb_list.push_back(row[0]);
    mysql_free_result(result);                                                                            
//
// Sort member name list
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = memb_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = memb_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        memb_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }                                                                                                     
#if 0
    if (memb_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
		cerr << "Memory allocation error in resmemberlist" << endl;
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
*	Server code for the resresolist_1_svc function 	    	    	    *
*                           -----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a list of resources for a given device	    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::resresolist_1_svc(db_res *recev)
{
    vector<string> 	reso_list;
    string 		user_domain(recev->res_val.arr1_val[0]),
    			user_family(recev->res_val.arr1_val[1]),
    			user_member(recev->res_val.arr1_val[2]);
	
#ifdef DEBUG
    cout << "In resresolist_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << endl;
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
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//
    string query;

    if (mysql_db == "tango")
    {
    	query = "SELECT DISTINCT name FROM property_device WHERE domain = '";
    	query += (user_domain + "' AND family = '" + user_family + "' AND member = '");
    }
    else
    {
    	query = "SELECT DISTINCT NAME FROM RESOURCE WHERE DOMAIN = '";
    	query += (user_domain + "' AND FAMILY = '" + user_family + "' AND MEMBER = '");
    }
    query += (user_member + "' ORDER BY NAME ASC");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES   *result = mysql_store_result(mysql_conn);
    MYSQL_ROW   row;
 
    while((row = mysql_fetch_row(result)) != NULL)
	reso_list.push_back(row[0]);
    mysql_free_result(result);                                                                            
//
// Sort resource name list
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = reso_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = reso_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        reso_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (reso_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in resresolist" << endl;
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
*	Server code for the resresoval_1_svc function     	    	    *
*                           ----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a resource value (as strings)		    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*	     - resource : The resource name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::resresoval_1_svc(db_res *recev)
{
    string 		res_val;
    vector<string> 	reso_list;
    string 		user_domain(recev->res_val.arr1_val[0]),
    			user_family(recev->res_val.arr1_val[1]),
    			user_member(recev->res_val.arr1_val[2]),
    			user_reso(recev->res_val.arr1_val[3]);
		
#ifdef DEBUG
    cout << "In resresoval_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << "/" << user_reso << endl;
#endif
	
//
// Initialize structure sent back to client
//
    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
//
// If the server is not connected to the database, throw exception
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to the database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get a list of all members and resource name
//
    string query;

    if (mysql_db == "tango")
    {
    	query = "SELECT member, name, count, value FROM property_device WHERE ";
    	query += (" domain = '" + user_domain + "' AND family = '" + user_family + "'");
        if (user_member != "*")
    	    query += (" AND member = '" + user_member + "'");
        if (user_reso != "*")
	    query += (" AND name = '" + user_reso + "'");
        query += (" ORDER BY member ASC, name ASC, count ASC");
    }
    else
    {
    	query = "SELECT MEMBER, NAME, INDEX_RES, RESVAL FROM RESOURCE WHERE ";
    	query += (" DOMAIN = '" + user_domain + "' AND FAMILY = '" + user_family + "'");
        if (user_member != "*")
    	    query += (" AND MEMBER = '" + user_member + "'");
        if (user_reso != "*")
	    query += (" AND NAME = '" + user_reso + "'");
        query += (" ORDER BY MEMBER ASC, NAME ASC, INDEX_RES ASC");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES   *result = mysql_store_result(mysql_conn);
    MYSQL_ROW   row;
    if (mysql_num_rows(result) == 0)
    { 
	mysql_free_result(result);
	browse_back.db_err = DbErr_ResourceNotDefined;
	return(&browse_back);
    }
    res_val = "";
    while((row = mysql_fetch_row(result)) != NULL)
    {
//	MEMBER, NAME, INDEX_RES, RESVAL
//
// Build the first field of the complete resource name
//
	if (atoi(row[2]) == 1)
	{
	    if (res_val.length() != 0)
		reso_list.push_back(res_val);
	    res_val = user_domain + "/" + user_family + "/" + row[0] + "/" + row[1] + ":" + row[3];
	}
	else
	    res_val += ("," + string(row[3]));
    }
    mysql_free_result(result);                                                                            
//
// Sort the resource array
//
//
// Build the structure returned to caller
//
    int length = browse_back.res_val.arr1_len = reso_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];
 
    for (int i = 0;i < length;i++)
    {
        int     k = reso_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        reso_list[i].copy(browse_back.res_val.arr1_val[i],string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#if 0
    if (reso_val.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in resresoval" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
#endif
//
// Return data
//
    return(&browse_back);
}
