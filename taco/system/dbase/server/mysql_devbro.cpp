#include <DevErrors.h>
#include <MySqlServer.h>



/**
 * list the domains for all the devices defined in the database 
 *
 * @return The domain name list
 */
db_res *MySQLServer::devdomainlist_1_svc(void)
{
    std::vector<std::string>	dom_list;
	
#ifdef DEBUG
    std::cout << "In devdomainlist_1_svc function" << std::endl;
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
	std::cerr << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the domain name list from the NAMES table
//
    std::string query = "SELECT DISTINCT DOMAIN FROM device ORDER BY DOMAIN ASC";
#ifdef DEBUG
    std::cout << "MySQLServer::devdomainlist_1_svc(): query " << query << std::endl;
#endif /* DEBUG */
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
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
        dom_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#ifdef DEBUG
    std::cout << "Found the following domains" << std::endl;
    for (std::vector<std::string>::iterator it = dom_list.begin(); it != dom_list.end(); ++it)
	std::cout << *it << std::endl;
    std::cout << std::endl;
#endif
//
// Return data
//
    return(&browse_back);
}

/**
 * list all families for all the devices defined in the database for a given domain
 *
 * @param domain The domain name
 *
 * @return The family name list
 */
db_res *MySQLServer::devfamilylist_1_svc(nam * domain)
{
    std::string		user_domain(*domain);
    std::vector<std::string>	fam_list;
	
#ifdef DEBUG
    std::cout << "In devfamilylist_1_svc function for domain " << *domain << std::endl;
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
	std::cerr << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the family name list for the wanted domain in the NAMES table
//
    std::string query = "SELECT DISTINCT FAMILY FROM device WHERE DOMAIN = '" + user_domain + "' ORDER BY FAMILY ASC";
#ifdef DEBUG
    std::cout << "MySQLServer::devfamilylist_1_svc(): query " << query << std::endl;
#endif /* DEBUG */
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
    while((row = mysql_fetch_row(result)) != NULL)
    	fam_list.push_back(row[0]);
    mysql_free_result(result);
//
// Build the sequence returned to caller
//
    int length = browse_back.res_val.arr1_len = fam_list.size();
    browse_back.res_val.arr1_val = new char *[browse_back.res_val.arr1_len];

    for (int i = 0;i < length;i++)
    {
	int	k = fam_list[i].length();
        browse_back.res_val.arr1_val[i] = new char [k + 1];
        fam_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
#ifdef DEBUG
    std::cout << "Found the following families in the domain " << user_domain << std::endl;
    for (std::vector<std::string>::iterator it = fam_list.begin(); it != fam_list.end(); ++it)
	std::cout << *it << std::endl;
    std::cout << std::endl;
#endif
//
// Return data
//
    return(&browse_back);	
}

/**
 * list all members for all the devices defined in the database for a given domain and family 
 * 
 * @param recev The domain name, the family name
 * 
 * @return The member name list
 */
db_res *MySQLServer::devmemberlist_1_svc(db_res *recev)
{
    std::vector<std::string> memb_list;
//
// Build strings from input names
//
    std::string user_domain(recev->res_val.arr1_val[0]);
    std::string user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
    std::cout << "In devmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << std::endl;
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
	std::cerr << "I'm not connected to the database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the member name list for the wanted domain and family from NAMES table
//
    std::string query = "SELECT DISTINCT MEMBER FROM device WHERE DOMAIN = '" + user_domain;
    query += "' AND FAMILY = '" + user_family + "' ORDER BY MEMBER ASC";
#ifdef DEBUG
    std::cout << "MySQLServer::devfamilylist_1_svc(): query " << query << std::endl;
#endif /* DEBUG */
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	browse_back.db_err = DbErr_DatabaseAccess;
	return (&browse_back);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
  
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
        memb_list[i].copy(browse_back.res_val.arr1_val[i],std::string::npos);
        (browse_back.res_val.arr1_val[i])[k] = '\0';
    }
//
// Return data
//
    return(&browse_back);	
}
