#include <DevErrors.h>
#include <algorithm>
#include <MySqlServer.h>


/****************************************************************************
*                                                                           *
*	Server code for the upddev_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To update device list(s)				    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_psdev_error *MySQLServer::upddev_1_svc(db_res *dev_list)
{
    long 			list_nb = dev_list->res_val.arr1_len;
		
#ifdef DEBUG
    std::cout << "In upddev_1_svc function for " << list_nb << " device list(s)" << std::endl;
#endif
//
// Initialize parameter sent back to client */
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// A loop on each device list */
//
    for (long i = 0;i < list_nb;i++)
    {
//		
// Allocate memory for strtok pointers */
//
	std::string lin = dev_list->res_val.arr1_val[i];

#ifdef DEBUG
	std::cout << "Device list = " << lin << std::endl;
#endif 
//
// Find the last device in the list. If there is no , character in the line,
// this means that there is only one device in the list 
//
	std::string	ds_class,
		ds_name,
		tmp;
	std::string::size_type pos =  lin.rfind(':');
	if (pos == std::string::npos)
	{

	}
	tmp = lin.substr(0, pos);
	if (count(tmp.begin(), tmp.end(), '/') != 2)
	{

	}
	lin.erase(0, pos + 1);
	pos = tmp.find('/');
	ds_class = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
	pos = tmp.find('/');
	ds_name = tmp.substr(0,pos);

	std::vector<std::string>	dev_list;
	dev_list.clear();
	while((pos = lin.find(',')) != std::string::npos)
	{
#ifdef DEBUG
	    std::cout << "Line = " << lin << std::endl;
#endif
	    dev_list.push_back(lin.substr(0, pos));
	    lin.erase(0, pos + 1);
	}
	dev_list.push_back(lin);

	std::string query;
	if (mysql_db == "tango")
	{
	    query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) FROM device WHERE ";
	    query += (" SERVER LIKE '" + ds_class + "/" + ds_name + "'");
	}
	else
	{
	    query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER), INDEX_NUMBER  FROM NAMES WHERE DEVICE_SERVER_CLASS = '";
	    query += ( ds_class + "' AND DEVICE_SERVER_NAME = '" + ds_name + "' ORDER BY INDEX_NUMBER ASC");
	}
#ifdef DEBUG
        std::cout << "MySQLServer::upddev_1_svc(): query = " << query << std::endl;
#endif /* DEBUG */
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
	    std::cerr << mysql_error(mysql_conn) << std::endl;
	    psdev_back.error_code = DbErr_DatabaseAccess;
	    psdev_back.psdev_err = i + 1;
	    return (&psdev_back);
	}
	MYSQL_RES *result = mysql_store_result(mysql_conn);
	MYSQL_ROW row;

	std::vector<std::string>	db_dev_list;
	db_dev_list.clear();
	while ((row = mysql_fetch_row(result)) != NULL)
	    db_dev_list.push_back(row[0]);	
	int 	ind = 1;
#ifdef DEBUG
	std::cout << "Some devices deleted " << std::endl;
#endif
        for (std::vector<std::string>::iterator it = db_dev_list.begin(); it != db_dev_list.end(); ++it)
	{
	    std::cerr << " Device = " << *it; 
	    std::vector<std::string>::iterator	it2;  
	    if ((it2 = find(dev_list.begin(), dev_list.end(), *it)) != dev_list.end())
	    {
		std::cerr << " found." << std::endl;	
	        switch(psdev_back.error_code = db_update_names(ds_class, ds_name, ind, *it))
	    	{
		    case DbErr_BadResourceType: 	
		    case DbErr_DatabaseAccess :	psdev_back.error_code = DbErr_DatabaseAccess;
						psdev_back.psdev_err = i + 1;
						return (&psdev_back);
		    default		      :	dev_list.erase(it2);
					      	ind++;
						break;
	    	}
	    }
	    else
	    {
		std::cerr << " not found." << std::endl;
		switch(psdev_back.error_code = db_delete_names(ds_class, ds_name, ind, *it))
		{
		    case DbErr_BadResourceType: 	
		    case DbErr_DatabaseAccess :	psdev_back.error_code = DbErr_DatabaseAccess;
						psdev_back.psdev_err = i + 1;
						return (&psdev_back);
		}
	    }
	    std::cerr << " Return = " << psdev_back.error_code << std::endl;
	}
        for (std::vector<std::string>::iterator it = dev_list.begin(); it != dev_list.end(); ++it)
	    if ((psdev_back.error_code = db_insert_names(ds_class, ds_name, ind, *it)) != 0)
	    {
	    	psdev_back.psdev_err = i + 1;
	    	return (&psdev_back);
	    }
    }
//
// return data
//
    return(&psdev_back);
}

/****************************************************************************
*                                                                           *
*		Code for db_update_names function                           *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To update the index in the NAMES table in the database *
*                                                                           *
*    Argin : - ds_class : The device class name	contains the DS    	    *
*	     - ds_name : The personal name				    *
*	     - ind : The device indice list in the list (starting with 1)   *
*	     - dev_name : the device name 				    *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long MySQLServer::db_update_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
    if (count(dev_name.begin(), dev_name.end(), '/') != 2)
	return DbErr_BadResourceType;
    std::stringstream query;

    if (mysql_db == "tango")
    {

// the TANGO database does not have an index number in the device table therefore do nothing

	return 0;
    }
    else
    {
        query << "UPDATE NAMES SET INDEX_NUMBER = " << ind 
	      << " WHERE DEVICE_SERVER_CLASS = '" << ds_class << "' AND DEVICE_SERVER_NAME = '" << ds_name
	      << "' AND CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" << dev_name <<"'" << std::ends;

#if !HAVE_SSTREAM    
        if (mysql_query(mysql_conn, query.str()) != 0)
#else
        if (mysql_query(mysql_conn, query.str().c_str()) != 0)
#endif
        {
	    std::cerr << __LINE__ << mysql_error(mysql_conn) << std::endl;
#if !HAVE_SSTREAM
	    query.freeze(false);
#endif
	    return DbErr_DatabaseAccess;
        }
#if !HAVE_SSTREAM    
        query.freeze(false);
#endif
        if (mysql_affected_rows(mysql_conn) == 1)
	    return 0;
        else
	    return DbErr_DeviceNotDefined;
    }
}

/****************************************************************************
*                                                                           *
*		Code for db_delete_names function                           *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To delete (in the database) a device for a      	    *
*                    specific device server.                                *
*                                                                           *
*    Argin : - ds_class : The device class name	contains the DS    	    *
*	     - ds_name : The personal name				    *
*	     - ind : The device indice list in the list (starting with 1)   *
*	     - dev_name : the device name 				    *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long MySQLServer::db_delete_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
    if (count(dev_name.begin(), dev_name.end(), '/') != 2)
	return DbErr_BadResourceType;

    std::string query;
    if (mysql_db == "tango")
    {
        query = "DELETE FROM device WHERE CLASS = '" + ds_class + "' AND SERVER LIKE  '"
	       + ds_name + "/%' AND CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + dev_name + "'";
    }
    else
    {
        query = "DELETE FROM NAMES WHERE DEVICE_SERVER_CLASS = '" + ds_class + "' AND DEVICE_SERVER_NAME = '"
	       + ds_name + "' AND CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + dev_name + "'";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << __LINE__ << mysql_error(mysql_conn) << std::endl;
	return DbErr_DatabaseAccess;
    }
    if (mysql_affected_rows(mysql_conn) == 1)
	return 0;
    else
	return DbErr_DeviceNotDefined;
}

/****************************************************************************
*                                                                           *
*		Code for db_insert_names function                           *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To insert a new device in the NAMES table              *
*                                                                           *
*    Argin : - ds_class : The device class name	contains the DS    	    *
*	     - ds_name : The personal name				    *
*	     - ind : The device indice list in the list (starting with 1)   *
*	     - dev_name : the device name 				    *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long MySQLServer::db_insert_names(const std::string ds_class, const std::string ds_name, const int ind, const std::string dev_name)
{
    std::stringstream query;

    if (count(dev_name.begin(), dev_name.end(), '/') != 2)
	return DbErr_BadResourceType;
    std::string	domain,
		family,
		member;
    std::string::size_type	pos,
			last_pos = 0;

    pos = dev_name.find('/');
    domain = dev_name.substr(0, pos);
    pos = dev_name.find('/', (last_pos = pos + 1));
    family = dev_name.substr(last_pos, (pos - last_pos));
    pos = dev_name.find('/', (last_pos = pos + 1));
    member = dev_name.substr(last_pos, (pos - last_pos));

    if (mysql_db == "tango")
    {
        query << "INSERT INTO device(NAME, CLASS, SERVER, DOMAIN, FAMILY, MEMBER, IOR, PID, VERSION, EXPORTED)"
	      << " VALUES('" << domain << "/" << family << "/" << member << "','" 
	      << ds_class << "', '" << ds_class << "/" << ds_name << "', '" << domain << "', '" << family
	      << "', '" << member << "', 'rpc::0', 0, 0, 0)" << std::ends;
        std::cout << query.str() << std::endl;
    }
    else
    {
        query << "INSERT INTO NAMES(DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, INDEX_NUMBER, DOMAIN, FAMILY, MEMBER)"
	      << " VALUES('" << ds_class << "', '" << ds_name << "', " << ind << ", '" << domain << "', '" << family
	      << "', '" << member << "')" << std::ends;
    }
#if !HAVE_SSTREAM
    if (mysql_query(mysql_conn, query.str()) != 0)
#else
    if (mysql_query(mysql_conn, query.str().c_str()) != 0)
#endif
    {
	std::cerr << __LINE__ << mysql_error(mysql_conn) << std::endl;
#if !HAVE_SSTREAM
	query.freeze(false);
#endif
	return DbErr_DatabaseAccess;
    }
#if !HAVE_SSTREAM
    query.freeze(false);
#endif
    if (mysql_affected_rows(mysql_conn) == 1)
	return 0;
    else
	return DbErr_DatabaseAccess;
}


/****************************************************************************
*                                                                           *
*	Server code for the updres_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To update resource(s)				    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_psdev_error *MySQLServer::updres_1_svc(db_res *res_list)
{
    long 			list_nb = res_list->res_val.arr1_len,
				last,
				ind;
    char 			*ptr,
				*ptr_cp,
				*tmp,
				last_dev[DEV_NAME_LENGTH],
				pat[2] = {SEP_ELT, '\0'};

#ifdef DEBUG
    std::cout << "In updres_1_svc function for " << list_nb << " resource(s)" << std::endl;
#endif
//
// Initialize parameter sent back to client */
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// A loop on each resources */
//
    for (long i = 0; i < list_nb; i++)
    {
//
// Allocate memory for strtok pointers */
//
	std::string lin(res_list->res_val.arr1_val[i]);

#ifdef DEBUG
	std::cout << "Resource list = " << lin << std::endl;
#endif
//
// Only one update if the resource is a simple one */
//
	if (lin.find(SEP_ELT) == std::string::npos)
	{
	    if (upd_res(lin,1,False,&psdev_back.error_code) == -1)
	    {
		psdev_back.psdev_err = i + 1;
		return(&psdev_back);
	    }
	}
	else
	{		
	    try
	    {
	    	ptr_cp = new char[lin.length() + 1];
	    }
	    catch(std::bad_alloc)
	    {
		psdev_back.psdev_err = i + 1;
		psdev_back.error_code = DbErr_ClientMemoryAllocation;
		return(&psdev_back);
	    }
//		
// Extract each resource from the list and update table each time 
//
	    strcpy(ptr_cp, lin.c_str());
	    ind = 1;
	    ptr = strtok(ptr_cp,pat);
	    if (upd_res(ptr, ind, False, &psdev_back.error_code) == -1)
	    {
		delete [] ptr_cp;
		psdev_back.psdev_err = i + 1;
		return(&psdev_back);
	    }
	    strcpy(ptr_cp, lin.c_str());
	    ptr = strtok(ptr_cp,pat); /* Reinit. strtok internal pointer */
	    while((ptr = strtok(NULL,pat)) != NULL)
	    {
		ind++;	
		if (upd_res(ptr, ind, True, &psdev_back.error_code) == -1)
		{
		    delete [] ptr_cp;
		    psdev_back.psdev_err = i + 1;
		    return(&psdev_back);
		}
	    }
	    delete [] ptr_cp;
	}
    }
//
// Free memory and return data
//
    return(&psdev_back);
}


/****************************************************************************
*                                                                           *
*		Code for upd_res function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To update a resource in the appropriate table in       *
*                    database                                               *
*                                                                           *
*    Argin : - A pointer to the modified resource definition (without space *
*              and tab characters)                                          *
*            - The number of the resource in the array (one if the resource *
*              type is not an array)					    *
*            - A flag to inform the function that this resource is a member *
*              of an array                                                  *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long MySQLServer::upd_res(std::string lin, long numb, char array, long *p_err)
{
    unsigned int 	diff;
    static std::string	domain,
			family,
			member,
			name;
    std::string		val;

    if (numb == 1)
    {
    	std::string::size_type	pos,
				last_pos;
//
// Get table name 
//
	pos = lin.find('/');
	domain = lin.substr(0, pos).c_str();
	errcode = (domain == "sec");
//
// Get family name 
//
	pos = lin.find('/',  (last_pos = pos + 1));
	family = lin.substr(last_pos, pos - last_pos);
//
// Get member name 
//
	pos = lin.find('/', (last_pos = pos + 1));
	member = lin.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
	pos = lin.find(':', (last_pos = pos + 1));
	name = lin.substr(last_pos, pos - last_pos);
//
// If the resource belongs to Security domain, change every occurance of
// | by a ^ character 
// 
   	if (errcode == True)
	{
	    long l = name.length();
	    for (int i = 0; i < l; i++)
		if (name[i] == '|')
			name[i] = SEC_SEP;
	}
//
// Get resource value (resource values are stored in the database as 
// case dependent std::strings 
//
	val = lin.substr(pos + 1);
    }
    else
	val = lin;
//
// For security domain, change every occurance of | by a ^ 
//
    if (errcode == True)
    {
	long l = val.length();
	for (int i = 0; i < l; i++)
	    if (val[i] == '|')
		val[i] = SEC_SEP;
    }
//
// Initialise resource number */
//
    numb;
#ifdef DEBUG
    std::cout << "Table name : " << domain << std::endl;
    std::cout << "Family name : " << family << std::endl;
    std::cout << "Number name : " << member << std::endl;
    std::cout << "Resource name : " << name << std::endl;
    std::cout << "Resource value : " << val << std::endl;
    std::cout << "Sequence number : " << numb << std::endl << std::endl;
#endif 
//
// Select the right resource table in database */
//
    if (numb == 1)
    {
	int i;
	for (i = 0; i < dbgen.TblNum; i++)
	    if (domain == dbgen.TblName[i])
		break;
	if (i == dbgen.TblNum)
	{
	    *p_err = DbErr_DomainDefinition;
	    return(-1);
	}
    }
//
// If the resource value is %, remove all the resources.
// If this function is called for a normal resource, I must also 
// remove all the old resources with the old name. This is necessary if there
// is an update of a resource which was previously an array 
//
    if (val == "%" || !array)
    {
	std::string query;
        if (mysql_db == "tango")
        {
            query = "DELETE FROM property_device WHERE DEVICE = '" + domain + "/" + family + "/" + member + "'";
            query += " AND NAME = '" + name + "'";
        }
        else
        {
	    std::string query = "DELETE FROM RESOURCE WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER, '/', NAME) = '";
	    query += (domain + '/' + family + '/' + member + '/' + name + "'");
        }                                                                
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
	    std::cerr << __LINE__ << mysql_error(mysql_conn) << std::endl;
	    std::cerr << query.c_str() << std::endl;
	    *p_err = DbErr_DatabaseAccess;
	    return(-1);
	}
    }
//
// Insert a new tuple 
//
    std::stringstream query;
    if (mysql_db == "tango")
    {
        query << "INSERT INTO property_device(DEVICE,NAME,DOMAIN,FAMILY,MEMBER,COUNT,VALUE) VALUES('"
	      << domain << "/" << family << "/" << member << "','" << name << "','"
	      << domain << "','" << family << "','" << member << "','" 
	      << numb << "',\"" << val << "\")" << std::ends;

    }
    else
    {
        query << "INSERT INTO RESOURCE(DOMAIN, FAMILY, MEMBER, NAME, INDEX_RES, RESVAL) VALUES('"
	      << domain << "', '" << family << "', '" << member << "', '" << name << "', " << numb
	      << ", '" << val << "')" << std::ends;
    }
#ifdef DEBUG
    std::cout << "MySQLServer::upd_res(): query = " << query.str() << std::endl;
#endif /* DEBUG */
#if !HAVE_SSTREAM
    if (mysql_query(mysql_conn, query.str()) != 0)
#else
    if (mysql_query(mysql_conn, query.str().c_str()) != 0)
#endif
    {
	std::cerr << __LINE__ << mysql_error(mysql_conn) << std::endl;
	std::cerr << query.str() << std::endl;
#if !HAVE_SSTREAM
	query.freeze(false);
#endif
	*p_err = DbErr_DatabaseAccess;
	return (-1);
    }
#if !HAVE_SSTREAM
    query.freeze(false);
#endif
    return(0);
}



/****************************************************************************
*                                                                           *
*	Server code for the secpass_1_svc function       	   	    *
*                           -------------                       	    *
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
db_res *MySQLServer::secpass_1_svc()
{
    char pass[80];
	
#ifdef DEBUG
    std::cout << "In secpass_1_svc function" << std::endl;
#endif

//
// Initialize structure sent back to client
//
    browse_back.db_err = 0;
    browse_back.res_val.arr1_len = 0;
    browse_back.res_val.arr1_val = NULL;
    pass[0] = '\0';
//
// Build security file name
//
    std::string f_name((char *)getenv("DBM_DIR"));
    f_name.append("/.sec_pass");	
//
// Try to open the file
//
    std::ifstream f(f_name.c_str());
    if (!f)
    {
	browse_back.db_err = DbErr_NoPassword;
	return(&browse_back);
    }
//
// Get password
//
    f.getline(pass, sizeof(pass) - 1);
    if (strlen(pass) == 0)
    {
	browse_back.db_err = DbErr_NoPassword;
	return(&browse_back);
    }
//
// Init data sent back to client 
//	
    try
    {
	browse_back.res_val.arr1_val = new nam[1];
	browse_back.res_val.arr1_val[0] = new char [strlen(pass) + 1];
	strcpy(browse_back.res_val.arr1_val[0], pass);
    }
    catch (std::bad_alloc)
    {
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
    browse_back.res_val.arr1_len = 1;
//
// Return data
//
    return(&browse_back);
}
