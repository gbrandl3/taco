#include <DevErrors.h>
#include <cctype>
#include <MySqlServer.h>
#include <algorithm>


/****************************************************************************
*                                                                           *
*		Server code for db_psdev_register function                  *
*                               -----------------            	            *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number and the version      *
*                    number of the device server for a specific device      *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures db_devinfo *
*              db_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*         Zero means no error                                               * 
*                                                                           *
*****************************************************************************/
db_psdev_error *MySQLServer::db_psdev_reg_1_svc(psdev_reg_x *rece)
{
    int 			num_psdev = rece->psdev_arr.psdev_arr_len;;
    psdev_elt 			*tmp;
//
// Miscellaneous init 
//
    psdev_back.error_code = psdev_back.psdev_err = 0;

#ifdef DEBUG
    std::cout << "Begin db_psdev_register" << std::endl;
    std::cout << "Host name : " << rece->h_name << std::endl;
    std::cout << "PID = " << rece->pid << std::endl;
    for (long i = 0; i < num_psdev; i++)
    {
	tmp = &(rece->psdev_arr.psdev_arr_val[i]);
	std::cout << "Pseudo device name : " << tmp->psdev_name << std::endl;
	std::cout << "Refresh period : " << tmp->poll << std::endl;
    }
#endif
//
// Return error code if the server is not connected 
//
    if (dbgen.connected == False)
    {
	std::cerr << "I'm not connected to database." << std::endl;
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// For each pseudo device, register it in the PS_NAMES table 
//
    for (long i = 0;i < num_psdev;i++)
    {
	long	error;
	tmp = &(rece->psdev_arr.psdev_arr_val[i]);
	if (reg_ps(rece->h_name,rece->pid,tmp->psdev_name, tmp->poll,&error) == -1)
	{
	    psdev_back.error_code =  error;
	    psdev_back.psdev_err = i + 1;
	    return(&psdev_back);
	}
    }
//
// Leave server 
//
#ifdef DEBUG
    std::cout << "End db_psdev_register" << std::endl;
#endif 
    return(&psdev_back);
}



/****************************************************************************
*                                                                           *
*		Code for reg_ps function                		    *
*                        ------                         		    *
*                                                                           *
*    Function rule : To register a pseuo device in the PS_NAMES database    *
*		     table						    *
*									    *
*    Argin(s) : - h_name : The host name				    *
*		- pid : The process PID					    *
*		- ps_name : The pseudo device name			    *
*		- poll : The polling period				    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/
long MySQLServer::reg_ps(std::string h_name, long pid, std::string ps_name, long poll, long *p_error)
{
    std::string	ps_name_low(ps_name);
//
// Make a copy of the pseudo device name in lowercase letter 
//
    transform(ps_name_low.begin(), ps_name_low.end(), ps_name_low.begin(), ::tolower);
    if (count(ps_name_low.begin(), ps_name_low.end(), '/') != 2)
    {
	*p_error = DbErr_BadDevSyntax;
	return (-1);
    }
//
// First, check that the name used for the pseudo device is not already used
// for a real device 
//
    std::string query = "SELECT COUNT(*) FROM NAMES WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + ps_name_low + "'";
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl; 
	*p_error = DbErr_DatabaseAccess;
	return(-1);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
    {
	if (atoi(row[0]))
    	{
	    *p_error = DbErr_NameAlreadyUsed;
	    return(-1);
    	}
    }
    else
    {
	mysql_free_result(result);
	*p_error = DbErr_DatabaseAccess;
	return (-1);
    }
    mysql_free_result(result);
//
// Now, try to retrieve a tuple in the PS_NAMES table with the same pseudo
// device name 
//
//
// Update database information 
//
    std::stringstream strquery;
    try
    {
	if (mysql_db == "tango")
        {
    	    strquery << "UPDATE DEVICE SET HOST = '" << h_name << "', PID = " << pid << " IOR = 'DC:" << poll << "'"
	      << " WHERE DEVICE = '" << ps_name_low << "'" << std::ends;
	}
	else
	{
    	    strquery << "UPDATE PS_NAMES SET HOST = '" << h_name << "', PROCESS_ID = " << pid << " POLL = " << poll
	      << " WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" << ps_name_low << "'" << std::ends;
	}
#if !HAVE_SSTREAM
    	if (mysql_query(mysql_conn, strquery.str()) != 0)
#else
    	if (mysql_query(mysql_conn, strquery.str().c_str()) != 0)
#endif
    	{
//
// In case of error 
//
	    std::cerr << mysql_error(mysql_conn) << std::endl;
	    throw long(DbErr_DatabaseAccess);
        }
#if !HAVE_SSTREAM
      	strquery.freeze(false);
#endif
    	if (mysql_affected_rows(mysql_conn) == 0)
        {
//
// Insert a new record in database 
//
	    std::string::size_type	pos,
				last_pos;
	    std::string		domain,
				family,
				member;

	    strquery.seekp(0);
	    if (mysql_db == "tango")
	    {
    	       strquery << "INSERT INTO DEVICE (DOMAIN, FAMILY, MEMBER, HOST, PID, IOR, EXPORTED, SERVER, CLASS, VERSION) VALUES('"
	  	   << domain << "', '" << family << "', '" << member << "', '" << h_name << "', " 
		   << pid << ", 'DC:" << poll << "',1,'DataCollector','PseudoDevice',1)" << std::ends;
	    }
	    else
	    {
    	       strquery << "INSERT INTO PS_NAMES (DOMAIN, FAMILY, MEMBER, HOST, PROCESS_ID, POLL) VALUES('"
	  	   << domain << "', '" << family << "', '" << member << "', '" << h_name << "', " 
		   << pid << ", " << poll << ")" << std::ends;
	    }
#if !HAVE_SSTREAM
	    if (mysql_query(mysql_conn, strquery.str()) != 0)
	    	throw long(DbErr_DatabaseAccess);
	    strquery.freeze(false);
#else
	    if (mysql_query(mysql_conn, strquery.str().c_str()) != 0)
	    	throw long(DbErr_DatabaseAccess);
#endif
	    if (mysql_affected_rows(mysql_conn) == 0)
	    	throw long(DbErr_DatabaseAccess);
	}
    }
    catch (const long err)
    {
//	strquery.freeze(false);
	*p_error = err;
	return -1;
    }
//
// leave function 
//
    return(0);
}


/****************************************************************************
*                                                                           *
*		Server code for db_psdev_unregister function                *
*                               -------------------                         *
*                                                                           *
*    Function rule : To retrieve (from database) the host_name, the program *
*                    number and the version number for specific devices     *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*               u_int arr1_len;      The number of strings                  *
*               char **arr1_val;     A pointer to the array of strings      *
*                   }                                                       *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_resimp" type *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      tab_dbdev imp_dev;   A structure of the tab_dbdev type (see above)   *
*                           with the informations needed (host_name,        *
*                           program number and version number)              *
*      int db_imperr;    The database error code                            *
*                        0 if no error                                      *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_psdev_error *MySQLServer::db_psdev_unreg_1_svc(arr1 *rece)
{
    u_int 			num_psdev = rece->arr1_len;
//
// Miscellaneous init 
//
    psdev_back.error_code = psdev_back.psdev_err = 0;

#ifdef DEBUG
    std::cout << "Begin db_psdev_unregister" << std::endl;
    for (long i = 0;i < num_psdev;i++)
	std::cout << "Pseudo device name : " << rece->arr1_val[i] << std::endl;
#endif
//
// Return error code if the server is not connected 
//
    if (dbgen.connected == False)
    {
	std::cerr << "I'm not connected to database." << std::endl;
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// For each pseudo device, unregister it in the PS_NAMES table 
//
    for (long i = 0;i < num_psdev;i++)
    {
	long error;
	if (unreg_ps(rece->arr1_val[i],&error) == -1)
	{
	    psdev_back.error_code =  error;
	    psdev_back.psdev_err = i + 1;
	    return(&psdev_back);
	}
    }
//
// Leave server */
//
#ifdef DEBUG
    std::cout << "End db_psdev_unregister" << std::endl;
#endif 
    return(&psdev_back);
}


/****************************************************************************
*                                                                           *
*		Code for unreg_ps function                		    *
*                        --------                         		    *
*                                                                           *
*    Function rule : To unregister pseuo device from the database PS_NAMES  *
*		     table						    *
*									    *
*    Argin(s) : - ps_name : The pseudo device name			    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/
long MySQLServer::unreg_ps(std::string ps_name, long *p_error)
{
    long i,
	l;
    std::string	ps_name_low(ps_name);
    datum 	key,
		content;
    char 	key_buf[MAX_KEY];
//
// Make a copy of the pseudo device name in lowercase letter 
//
    transform(ps_name_low.begin(), ps_name_low.end(), ps_name_low.begin(), ::tolower);
    if (count(ps_name_low.begin(), ps_name_low.end(), '/') != 2)
    {
	*p_error = DbErr_BadDevSyntax;
	return (-1);
    }
//
// Retrieve a tuple in the PS_NAMES table with the same pseudo device name 
//
    std::string query;
    if (mysql_db == "tango")
    {
        query = "DELETE FROM DEVICE WHERE NAME = '" + ps_name_low + "'";
    }
    else
    {
        query = "DELETE FROM PS_NAMES WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + ps_name_low + "'";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	*p_error = DbErr_DatabaseAccess;
	return (-1);
    }
    if (mysql_affected_rows(mysql_conn) == 0)
    {
	*p_error = DbErr_DeviceNotDefined;
	return -1;
    }
//
// leave function 
//
    return(0);
}
