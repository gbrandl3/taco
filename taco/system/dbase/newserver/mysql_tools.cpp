#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>


/****************************************************************************
*                                                                           *
*	Server code for the devinfo_1_svc function       	   	    *
*                           -------------                       	    *
*                                                                           *
*    Method rule : To return device information 			    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns a pointer to a structure with all device info    *
*    and an error code which is set if needed				    *
*                                                                           *
****************************************************************************/
db_devinfo_svc *MySQLServer::devinfo_1_svc(nam *dev)
{
    string user_device(*dev);
#ifdef DEBUG
    cout << "In devinfo_1_svc function for device " << user_device << endl;
#endif
//
// Initialize parameter sent back to client and allocate memory for string
// (bloody XDR)
//
    sent_back.db_err = 0;
    sent_back.device_class = NULL;
    sent_back.server_name = NULL;
    sent_back.personal_name = NULL;
    sent_back.process_name = NULL;
    sent_back.host_name = NULL;
	
    try
    {	
	sent_back.device_class = new char[DEV_CLASS_LENGTH];
	sent_back.device_class[0] = '\0';

	sent_back.server_name = new char[DS_NAME_LENGTH];
	sent_back.server_name[0] = '\0';
	
	sent_back.personal_name = new char[DSPERS_NAME_LENGTH];
	sent_back.personal_name[0] = '\0';
	
	sent_back.process_name = new char[PROC_NAME_LENGTH];
	sent_back.process_name[0] = '\0';

	sent_back.host_name = new char[HOST_NAME_LENGTH];	
	sent_back.host_name[0] = '\0';
    }
    catch (bad_alloc)
    {
	cerr << "Memory allocation error in devinfo" << endl;
	sent_back.db_err = DbErr_ServerMemoryAllocation;
	return(&sent_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database" << endl;
	sent_back.db_err = DbErr_DatabaseNotConnected;
	return(&sent_back);
    }
//
// Search for device name in the NAMES table
//
    long found = False;

    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT SUBSTRING_INDEX(server,'/',1), SUBSTRING_INDEX(server,'/',-1), host, ior,";
        query += ("version, class, pid, SUBSTRING_INDEX(server,'/',1), exported FROM device");
        query += (" WHERE CONCAT(domain, '/', family, '/', member) = '" + user_device + "'"); 
    }
    else
    {
        query = "SELECT DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, HOSTNAME, PROGRAM_NUMBER,";
        query += ("VERSION_NUMBER, DEVICE_CLASS, PROCESS_ID, PROCESS_NAME FROM NAMES");
        query += (" WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + user_device + "'"); 
    }
    if (mysql_query(mysql_conn, query.c_str()) == 0)
    {
	MYSQL_RES *result = mysql_store_result(mysql_conn);
	MYSQL_ROW row;
	if((row = mysql_fetch_row(result)) != NULL)
	{
	    sent_back.device_type = DB_Device;				
	    if (row[0] != NULL) strcpy(sent_back.server_name, row[0]);
	    if (row[1] != NULL) strcpy(sent_back.personal_name, row[1]);
	    if (row[2] != NULL) strcpy(sent_back.host_name, row[2]);
	    if (mysql_db == "tango")
	    {
		if (row[3] != NULL)
		{
                    string ior(row[3]);
                    string pgm_no;
		    pgm_no = ior.substr(ior.rfind(':')+1);
                    sent_back.program_num = atoi(pgm_no.c_str());         
		}
	    }
	    else
	    {
	        sent_back.program_num = atoi(row[3]);
	    }
	    if (row[4] != NULL) sent_back.server_version = atoi(row[4]);
	    if (row[5] != NULL) strcpy(sent_back.device_class, row[5]);
	    if (row[6] != NULL) sent_back.pid = atoi(row[6]);
	    if (row[7] != NULL) strcpy(sent_back.process_name, row[7]);
	    if (mysql_db == "tango")
	    {
	        if (row[8] != NULL) sent_back.device_exported = atoi(row[8]);
	    }
	    else
	    {
	        sent_back.device_exported = (sent_back.program_num != 0 || sent_back.server_version != 0); 
	    }
	}
	else
	{
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
	    mysql_free_result(result);
	    if (mysql_db != "tango")
	    {
	        query = "SELECT HOST, PROCESS_ID FROM PS_NAMES";
    	        query += (" WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + user_device + "'"); 
    	        if (mysql_query(mysql_conn, query.c_str()) == 0)
    	        {
		    result = mysql_store_result(mysql_conn);
		    if((row = mysql_fetch_row(result)) != NULL)
		    {
    		        sent_back.device_type = DB_Pseudo_Device;
	    	        *sent_back.server_name = '\0';
	    	        *sent_back.personal_name = '\0';
	    	        *sent_back.host_name = '\0';
	    	        sent_back.program_num = 0;
	    	        sent_back.server_version = 0;
	    	        *sent_back.device_class = '\0';
	    	        sent_back.pid = atoi(row[1]);
	    	        *sent_back.process_name = '\0';
	    	        sent_back.device_exported = False;
		    }
//
// Return error if the device is not found
//
   		    else 
    		    {
		        sent_back.db_err = DbErr_DeviceNotDefined;
		        return(&sent_back);
    		    }
                }
	    }
	    else
	    {		
	    	sent_back.db_err = DbErr_DeviceNotDefined;
	    	return(&sent_back);
	    }
	}
	mysql_free_result(result);
    }
    else
    {			
	sent_back.db_err = DbErr_DatabaseAccess;
	return(&sent_back);			
    }
//
// Return data
//
    return(&sent_back);
}




/****************************************************************************
*                                                                           *
*	Server code for the devres_1_svc function 	    	    	    *
*                           ------------                     	    	    *
*                                                                           *
*    Method rule : To retrieve all resources belonging to a device	    *
*                                                                           *
*    Argin : - name : The device name				    	    *
*                                                                           *
*    Argout : No argout							    *
*                                                                    	    *
*    This function returns the resource list and an error code which is set *
*    if needed								    *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::devres_1_svc(db_res *recev)
{
    string 		fam,
    			memb,
			resource,
			tmp_res,
			r_name;
    string::size_type 	pos;
	
    res_list_dev.clear();

#ifdef DEBUG
    cout << "In devres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << endl;
    for (long i = 0; i < recev->res_val.arr1_len; i++)
	cout << " Device = " << recev->res_val.arr1_val[i] << endl;
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
	cerr << "I'm not connected to database" << endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }

    for (long i = 0; i < recev->res_val.arr1_len; i++)
    {
	string in_dev(recev->res_val.arr1_val[i]);
#ifdef DEBUG
	cout << " Device = " << in_dev << endl;
#endif
	if (count(in_dev.begin(), in_dev.end(), SEP_DEV) != 2)
	{
	    browse_back.db_err = DbErr_BadDevSyntax;
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
	    query = "SELECT domain, family, member, name, count, value FROM property_device WHERE ";
	    query += (" device = '" + in_dev);
	    query += ( + "' ORDER BY domain ASC, family ASC, member ASC, name ASC, count ASC");
	}
	else
	{
	    query = "SELECT DOMAIN, FAMILY, MEMBER, NAME, INDEX_RES, RESVAL FROM RESOURCE WHERE ";
	    query += (" CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + in_dev);
	    query += ( + "' ORDER BY DOMAIN ASC, FAMILY ASC, MEMBER ASC, NAME ASC, INDEX_RES ASC");
	}
//
// Get resource value for each element in the tmp_res_list list 
//
	if (mysql_query(mysql_conn, query.c_str()) == 0)
	{
	    MYSQL_RES *result = mysql_store_result(mysql_conn);
	    MYSQL_ROW row;

	    string resource("");
	    while ((row = mysql_fetch_row(result)) != NULL)
	    {
		if (atoi(row[4]) == 1)
		{
		    if (!resource.empty())
		   	res_list_dev.push_back(resource);
		    resource = string(row[0]) + "/" + row[1] + "/" + row[2] + "/" + row[3] + ": " + row[5];
		}
		else
		    resource += ("," + string(row[5]));
	    }
	    if (!resource.empty())
		res_list_dev.push_back(resource);
	    mysql_free_result(result);
	}
	else
	{
	    cerr << mysql_error(mysql_conn) << endl;
	    browse_back.db_err = DbErr_DatabaseAccess;
	    return (&browse_back);
	}
    }
//
// Sort resource name list
//
    sort(res_list_dev.begin(),res_list_dev.end());
// 
// Build the structure returned to caller
//
    long res_nb = res_list_dev.size(),
	 i;
    try
    {		
	browse_back.res_val.arr1_val = new char * [res_nb];
	for (i = 0;i < res_list_dev.size();i++)
	{
	    browse_back.res_val.arr1_val[i] = new char [res_list_dev[i].size() + 1];
	    strcpy(browse_back.res_val.arr1_val[i], res_list_dev[i].c_str());
	}
	browse_back.res_val.arr1_len = res_list_dev.size();
    }
    catch (bad_alloc)
    {
	if (browse_back.res_val.arr1_val != NULL)
	{
	    for (int j = 0; j < i; j++)
		delete [] browse_back.res_val.arr1_val;
	    delete [] browse_back.res_val.arr1_val;
			
	    cerr << "Memory allocation error in devres_svc" << endl;
	    browse_back.db_err = DbErr_ServerMemoryAllocation;
	    return(&browse_back);
	}
    }
//
// Return data
//
    return(&browse_back);	
}



/****************************************************************************
*                                                                           *
*	Server code for the devdel_1_svc function       	   	    *
*                           ------------                       	    	    *
*                                                                           *
*    Method rule : To delete a device from the database			    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
DevLong *MySQLServer::devdel_1_svc(nam *dev)
{
    string 	user_device(*dev);
#ifdef DEBUG
    cout << "In devdel_1_svc function for device " << user_device << endl;
#endif

//
// Initialize parameter sent back to client
//
    errcode = 0;
//
// If the server is not connected to the database, return error
//

    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
    if (count(user_device.begin(), user_device.end(), SEP_DEV) != 2)
    {
	errcode = DbErr_BadDevSyntax;
	return(&errcode);
    }
//
// Search for device name in the NAMES table
//
    string query, where;
    if (mysql_db == "tango")
    {
        query = "SELECT class, server FROM device ",
        where = "WHERE CONCAT(domain, '/', family, '/', member) = '" + user_device + "'";   
    }
    else
    {
        query = "SELECT DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, INDEX_RES FROM NAMES ",
        where = "WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + user_device + "'";   
    }
    
    if (mysql_query(mysql_conn, (query + where).c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row = mysql_fetch_row(result);

    if(row != NULL)
    {
    	string	ds_name(row[0]),
		ds_pers_name(row[1]),
    		ind(row[2]);
		
    	mysql_free_result(result);
	if (mysql_db == "tango")
	{
    	    query = "DELETE FROM device "; 
	}
	else
	{
    	    query = "DELETE FROM NAMES "; 
	}
    	if (mysql_query(mysql_conn, (query + where).c_str()) != 0)
    	{
	    cerr << mysql_error(mysql_conn) << endl;
	    errcode = DbErr_DatabaseAccess;
	    return (&errcode);
    	}
//
// Update device server device list (decrement all device index in device list
// for all devices above the deleted one)
//
	if (mysql_db != "tango")
	{
    	    query = "UPDATE NAMES SET INDEX_RES = INDEX_RES - 1 WHERE DEVICE_SERVER_CLASS = '" + ds_name + "' AND ";
    	    query += ("DEVICE_SERVER_NAME = '" + ds_pers_name + "' AND INDEX_RES > " + ind);
    	    if (mysql_query(mysql_conn, query.c_str()) != 0)
    	    {
	        cerr << mysql_error(mysql_conn) << endl;
	        errcode = DbErr_DatabaseAccess;
	        return (&errcode);
	    }
        }
    }
//
// If device not found in the NAMES table, search for it in the PS_NAMES table
//
    else
    {
    	mysql_free_result(result);
	if (mysql_db != "tango")
	{
	    query = "DELETE FROM PS_NAMES ";
    	    if (mysql_query(mysql_conn, (query + where).c_str()) != 0)
    	    {
	        cerr << mysql_error(mysql_conn) << endl;
	        errcode = DbErr_DatabaseAccess;
	        return (&errcode);
            }
//
// Return error if the device is not found
//
	    if (mysql_affected_rows(mysql_conn) == 0)
	    {
	        errcode = DbErr_DeviceNotDefined;
	        return(&errcode);
	    }
        }
    }
//
// Return data
//
    return(&errcode);
}



/****************************************************************************
*                                                                           *
*	Server code for the devdelres_1_svc function       	   	    *
*                           ---------------                       	    *
*                                                                           *
*    Method rule : To delete all the resources belonging to a device	    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
db_psdev_error *MySQLServer::devdelres_1_svc(db_res *recev)
{
#ifdef DEBUG
    cout << "In devdelres_1_svc function for " << recev->res_val.arr1_len << " device(s)" << endl;
#endif

//
// Initialize parameter sent back to client
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// Build a vector of object from the NdbmDomDev class. Each object in this class
// has a domain name and a list of family/member for this domain
//
    for (long i = 0;i < recev->res_val.arr1_len;i++)
    {
	string in_dev(recev->res_val.arr1_val[i]);
		
	if (count(in_dev.begin(), in_dev.end(), SEP_DEV) != 2)
	{
	    psdev_back.error_code = DbErr_BadDevSyntax;
	    psdev_back.psdev_err = i;
	    return(&psdev_back);
	}
	string query;
	if (mysql_db == "tango")
	{
	    query = "DELETE FROM property_device WHERE device = '" + in_dev + "'";
	}
	else
	{
	    query = "DELETE FROM RESOURCE WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + in_dev + "'";
	}
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
	    cerr << mysql_error(mysql_conn) << endl;
	    psdev_back.error_code = DbErr_DatabaseAccess;
	    psdev_back.psdev_err = i;
	    return (&psdev_back);
	}
	if (mysql_affected_rows(mysql_conn) == 0)
	{
	    psdev_back.error_code = DbErr_DeviceNotDefined;
	    psdev_back.psdev_err = i;
	    return(&psdev_back);
	}
    }
//
// leave fucntion
//
    return(&psdev_back);
}

class NameCount	
{
public:
    string	name;
    long	count;
    friend bool operator== (const NameCount &a, const NameCount &b){return (a.name == b.name);};
    friend bool operator< (const NameCount &a, const NameCount &b) {return (a.name < b.name);};
};


/****************************************************************************
*                                                                           *
*	Server code for the info_1_svc function       	   	    	    *
*                           ----------                       	    	    *
*                                                                           *
*    Method rule : To get global information on the database		    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to a structure with all the database   *
*    info								    *
*                                                                           *
****************************************************************************/
db_info_svc *MySQLServer::info_1_svc()
{
    long 			dev_defined = 0,
				dev_exported = 0,
				psdev_defined = 0,
				res_num = 0,
    				length;
    static vector<NameCount>	dom_list,
				res_list;

#ifdef DEBUG
    cout << "In info_1_svc function" << endl;
#endif 

    dom_list.clear();
    res_list.clear();
//
// Initialize parameter sent back to client
//
    info_back.db_err = 0;
    info_back.dev_defined = 0;
    info_back.dev_exported = 0;
    info_back.psdev_defined = 0;
    info_back.res_number = 0;
    info_back.dev.dom_len = 0;
    info_back.res.dom_len = 0;
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	info_back.db_err = DbErr_DatabaseNotConnected;
	return(&info_back);
    }
//
// First, count exported devices in the domains
//
    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT domain, COUNT(*) FROM device WHERE exported != 0 GROUP BY domain";
    }
    else
    {
        query = "SELECT DOMAIN, COUNT(*) FROM NAMES WHERE PROGRAM_NUMBER != 0 GROUP BY DOMAIN";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = atoi(row[1]);
	dev_defined += nc.count;
	dev_exported += nc.count;
	dom_list.push_back(nc);
    }
    mysql_free_result(result); 
//
// And now,  count not exported devices in the domains
//
    if (mysql_db == "tango")
    {
        query = "SELECT domain, COUNT(*) FROM device WHERE exported = 0 GROUP BY domain";
    }
    else
    {
        query = "SELECT DOMAIN, COUNT(*) FROM NAMES WHERE PROGRAM_NUMBER = 0 GROUP BY DOMAIN";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = atoi(row[1]);
	dev_defined += nc.count;
	vector<NameCount>::iterator p;
	for (p = dom_list.begin(); p != dom_list.end(); ++p)
	    if ((*p).name == nc.name)
		break;
	if (p == dom_list.end())
	    dom_list.push_back(nc);
	else
	    (*p).count += nc.count;
    }
//
// Now, count pseudo_devices
//
    if (mysql_db == "tango")
    {
        query = "SELECT COUNT(*) FROM device where ior like 'DC:%'";
    }
    else
    {
        query = "SELECT COUNT(*) FROM PS_NAMES";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    { 
	cerr << mysql_error(mysql_conn) << endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    if ((row = mysql_fetch_row(result)) != NULL)
    	psdev_defined = atoi(row[0]);
    mysql_free_result(result);
//
// Then, count resources in each domain
//
    if (mysql_db == "tango")
    {
        query = "SELECT domain, COUNT(*) FROM property_device GROUP BY domain";
    }
    else
    {
        query = "SELECT DOMAIN, COUNT(*) FROM RESOURCE GROUP BY DOMAIN";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	info_back.db_err = DbErr_DatabaseAccess;
	return (&info_back);
    }
    result = mysql_store_result(mysql_conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
	NameCount	nc;
	nc.name = row[0];
	nc.count = atoi(row[1]);
	res_list.push_back(nc);
	res_num += nc.count;
    }
    mysql_free_result(result);
//
// Sort exported devices domain list and resource domain list
//
    sort(dom_list.begin(),dom_list.end());
    sort(res_list.begin(),res_list.end());
//
// Return data
//
    info_back.dev_defined = dev_defined;
    info_back.dev_exported = dev_exported;
    info_back.psdev_defined = psdev_defined;
    info_back.res_number = res_num;

    length = dom_list.size();
    info_back.dev.dom_val = new db_info_dom_svc [length];
    info_back.dev.dom_len = length;	
    for (int i = 0;i < length;i++)
    {
	info_back.dev.dom_val[i].dom_elt = dom_list[i].count;
	info_back.dev.dom_val[i].dom_name = (char *)dom_list[i].name.c_str();
    }
	
    length = res_list.size();
    info_back.res.dom_val = new db_info_dom_svc [length];
    info_back.res.dom_len = length;	
    for (int i = 0;i < length;i++)
    {
	info_back.res.dom_val[i].dom_elt = res_list[i].count;
	info_back.res.dom_val[i].dom_name = (char *)res_list[i].name.c_str();
    }
    return(&info_back);
}



/****************************************************************************
*                                                                           *
*	Server code for the unreg_1_svc function     	    	    	    *
*                           -----------                     	    	    *
*                                                                           *
*    Method rule : To unregister a server from the database (to mark all its*
*		   devices as not exported)				    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                    	    *
*    This function returns a pointer to a long which will be set in case of *
*    error								    *
*                                                                           *
****************************************************************************/
long *MySQLServer::unreg_1_svc(db_res *recev)
{
    string 		user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
    cout << "In unreg_1_svc function for " << user_ds_name << "/" << user_pers_name << endl;
#endif
	
//
// Initialize structure sent back to client
//
    errcode = 0;
//
// If the server is not connected to the database, returns error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT DISTINCT class FROM device";
        query += (" WHERE server = '" + user_ds_name + "/" + user_pers_name + "'");
    }
    else
    {
        query = "SELECT DISTINCT DEVICE_SERVER_CLASS FROM NAMES";
        query += (" WHERE PROCESS_NAME = '" + user_ds_name + "' AND DEVICE_SERVER_NAME = '" + user_pers_name + "'");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
   	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    string	class_list("");
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	if (class_list.empty())
	    class_list += ("'" + string(row[0]) + "'");
	else
	    class_list += (",'" + string(row[0]) + "'");
    }
    mysql_free_result(result);
//
// If the ds_name was not a process name, init the class list with the user ds name
//		
    if (class_list.empty())
	class_list = "'" + user_ds_name + "'";
//
// Unregister every devices for each ds name in the list
//
    if (mysql_db == "tango")
    {
        query = "UPDATE device SET exported = 0 WHERE class IN (";
        query += (class_list + ")");
    }
    else
    {
        query = "UPDATE NAMES SET HOSTNAME = 'not_exp', PROGRAM_NUMBER = 0, VERSION_NUMBER = 0, DEVICE_TYPE = 'unknown'";
        query += (", DEVICE_CLASS = 'unknown', PROCESS_ID = 0, PROCESS_NAME = 'unknown' WHERE DEVICE_SERVER_CLASS IN (");
        query += (class_list + ")");
    }

    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
//
// Set error code if no device have been found
//
    if (mysql_affected_rows(mysql_conn) == 0)
	errcode = DbErr_DeviceServerNotDefined;
//
// Return data
//
    return(&errcode);
}



typedef struct
{
    string	name;
    int		flag;
}SvcDev;
/****************************************************************************
*                                                                           *
*	Server code for the svcinfo_1_svc function     	    	    	    *
*                           -------------                     	    	    *
*                                                                           *
*    Method rule : To return device server info to client		    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    argout : - no argout						    *
*                                                                    	    *
*    this function returns a pointer to a structure with all the device     *
*    server information							    *
*                                                                           *
****************************************************************************/
svcinfo_svc *MySQLServer::svcinfo_1_svc(db_res *recev)
{
    long 		indi,
			i,
			j,
			dev_length,
			name_length;
    vector<SvcDev> 	dev_list;
	
    string 		user_ds_name(recev->res_val.arr1_val[0]),
    			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
    cout << "in svcinfo_1_svc function for " << user_ds_name << "/" << user_pers_name << endl;
#endif
	
//
// initialize structure sent back to client
//
    svcinfo_back.db_err = 0;
    svcinfo_back.pid = 0;
    svcinfo_back.program_num = 0;
    svcinfo_back.embedded_len = 0;
    svcinfo_back.embedded_val = NULL;
    svcinfo_back.process_name = NULL;
    svcinfo_back.host_name = NULL;
	
    try
    {
	svcinfo_back.process_name = new char [PROC_NAME_LENGTH];
	svcinfo_back.process_name[0] = '\0';
	svcinfo_back.host_name = new char [HOST_NAME_LENGTH];
	svcinfo_back.host_name[0] = '\0';
    }
    catch (bad_alloc)
    {
	cerr << "memory allocation error in svc_info" << endl;
	svcinfo_back.db_err = DbErr_ServerMemoryAllocation;
	return(&svcinfo_back);
    }
//
// if the server is not connected to the database, returns error
//
    if (dbgen.connected == false)
    {
	cerr << "I'm not connected to database." << endl;
	svcinfo_back.db_err = DbErr_DatabaseNotConnected;
	return(&svcinfo_back);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT DISTINCT class FROM device";
        query += (" WHERE server = '" + user_ds_name + "/" + user_pers_name + "'");
    }
    else
    {
        query = "SELECT DISTINCT DEVICE_SERVER_CLASS FROM NAMES";
        query += (" WHERE DEVICE_SERVER_NAME = '" + user_pers_name + "' AND PROCESS_NAME = '" + user_ds_name + "'");
    }

    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	svcinfo_back.db_err = DbErr_DatabaseAccess;
	return (&svcinfo_back);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    
    string 	class_list("");
    int		nb_class = 0;
    while ((row = mysql_fetch_row(result)) != NULL)
    {
	if (class_list.empty())
	    class_list = "'" + string(row[0]) + "'";
	else
	    class_list += (", '" + string(row[0]) + "'");
	++nb_class;
    }
    mysql_free_result(result);
//
// If the user ds name was not a process name, init the class list with it
//
    if (nb_class == 0)
    {
	nb_class = 1;
	class_list = "'" + user_ds_name + "'";
    }
//
// Get all device for each class in the list
//
    if (mysql_db == "tango")
    {
        query = "SELECT SUBSTRING_INDEX(server,'/',1), CONCAT(domain, '/', family, '/', member), host, ior, exported, ";
        query += ("pid, server FROM device WHERE ");
        query += ("class IN (" + class_list + ") AND server LIKE '%/" + user_pers_name + "'");
    }
    else
    {
        query = "SELECT DEVICE_SERVER_CLASS, CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER), HOSTNAME, PROGRAM_NUMBER, VERSION_NUMBER, ";
        query += ("PROCESS_ID, PROCESS_NAME FROM NAMES WHERE ");
        query += ("DEVICE_SERVER_CLASS IN (" + class_list + ") AND DEVICE_SERVER_NAME = '" + user_pers_name + "'");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	svcinfo_back.db_err = DbErr_DatabaseAccess;
	return (&svcinfo_back);
    }
    result = mysql_store_result(mysql_conn);
    if (mysql_num_rows(result) == 0)
    {
//
// Set error code if no device have been found
//
	mysql_free_result(result);
	svcinfo_back.db_err = DbErr_DeviceServerNotDefined;
	return(&svcinfo_back);
    } 
//
// Allocate vector to store each class device list
//
    if ((row = mysql_fetch_row(result)) != NULL)
    {
//
// Initialize structure sent-back to client
//
	string	ds_class("");
	svcinfo_back.embedded_len = nb_class;
	svcinfo_back.embedded_val = new svcinfo_server[nb_class];
	for (long i = 0;i < nb_class;i++)
	{
	    svcinfo_back.embedded_val[i].server_name = NULL;
	    svcinfo_back.embedded_val[i].dev_val = NULL;
	}
   	if (row[2] != NULL) strcpy(svcinfo_back.host_name, row[2]);
	if (mysql_db == "tango")
	{
	    if (row[0] != NULL) strcpy(svcinfo_back.process_name, row[0]);
	}
	else
	{
	    if (row[6] != NULL) strcpy(svcinfo_back.process_name, row[6]);
	}
  	if (row[5] != NULL) svcinfo_back.pid = atoi(row[5]);
        if (mysql_db == "tango")
        {
	    if (row[3] != NULL)
	    {
                string ior(row[3]);
                string pgm_no;
                pgm_no = ior.substr(ior.rfind(':')+1);
                svcinfo_back.program_num = atoi(pgm_no.c_str());
            }
        }
        else
        {
   	    svcinfo_back.program_num = atoi(row[3]);
        }
	long i = 0;
        dev_list.clear();
	do
        {
	    if (ds_class != row[0])
	    {
		ds_class = row[0];
	    	name_length = ds_class.length();
	    	svcinfo_back.embedded_val[i].server_name = new char [name_length + 1];
	    	strcpy(svcinfo_back.embedded_val[i].server_name, ds_class.c_str());

	    	dev_length = dev_list.size();
		if (dev_length != 0)
		{
	    	    svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
	    	    for (long j = 0; j < dev_length; j++)
		    {
		    	svcinfo_back.embedded_val[i].dev_val[j].name = NULL;
		    	svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_list[j].name.length() + 1];
		    	strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_list[j].name.c_str());
		    	svcinfo_back.embedded_val[i].dev_val[j].exported_flag = dev_list[j].flag;
		    }
		    svcinfo_back.embedded_val[i].dev_len = dev_length;
		    i++;
		}
		dev_list.clear();
	    }
	    SvcDev	dev;
	    dev.name = row[1];
	    if (mysql_db == "tango")
	    {
	        if (row[4] != NULL) dev.flag = atoi(row[4]);
	    }
	    else
	    {
	        dev.flag = (atoi(row[3]) && atoi(row[4]));
	    }
	    dev_list.push_back(dev);	    
    	}while ((row = mysql_fetch_row(result)) != NULL);
	dev_length = dev_list.size();
	svcinfo_back.embedded_val[i].dev_val = new svcinfo_dev [dev_length];
	for (long j = 0; j < dev_length; j++)
	{
	    svcinfo_back.embedded_val[i].dev_val[j].name = NULL;
	    svcinfo_back.embedded_val[i].dev_val[j].name = new char [dev_list[j].name.length() + 1];
	    strcpy(svcinfo_back.embedded_val[i].dev_val[j].name, dev_list[j].name.c_str());
	    svcinfo_back.embedded_val[i].dev_val[j].exported_flag = dev_list[j].flag;
	}
	svcinfo_back.embedded_val[i].dev_len = dev_length;
    }
    else
    {
	mysql_free_result(result);
        return(&svcinfo_back);
    }
    mysql_free_result(result);
//
// Leave function
//
    return(&svcinfo_back);
}



/****************************************************************************
*                                                                           *
*	Server code for the svcdelete_1_svc function     	    	    *
*                           ---------------                     	    *
*                                                                           *
*    Method rule : To return delete all the device belonging to a device    *
*		   server and if necessary also their resources		    *
*                                                                           *
*    Argin : - recev : A pointer to a structure where the first element     *
*		       is the device server name and the second element     *
*		       is the device server personal name.		    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                    	    *
*    This function returns a pointer to a structure with all the device     *
*    server information							    *
*                                                                           *
****************************************************************************/
long *MySQLServer::svcdelete_1_svc(db_res *recev)
{
    string 		user_ds_name(recev->res_val.arr1_val[0]),
			user_pers_name(recev->res_val.arr1_val[1]);
		
#ifdef DEBUG
    cout << "In svcdelete_1_svc function for " << user_ds_name << "/" << user_pers_name << endl;
#endif
	
//
// Initialize structure sent back to client
//
    errcode = 0;
//
// If the server is not connected to the database, returns error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// First, suppose that the ds_name is a PROCESS name
//
    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT DISTINCT class FROM device WHERE server = '" + user_ds_name + "/" + user_pers_name;
    }
    else
    {
        query = "SELECT DISTINCT DEVICE_SERVER_CLASS FROM NAMES WHERE DEVICE_SERVER_NAME = '" + user_pers_name + "'";
        query += (" AND PROCESS_NAME = '" + user_ds_name + "'");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    string class_list("");

    while((row = mysql_fetch_row(result)) != NULL)
    {
	if (class_list.empty())
	    class_list = "'" + string(row[0]) + "'";
	else
	    class_list += (", '" + string(row[0]) + "'");
    }
    mysql_free_result(result);
//
// If the ds_name was not a process name, init the class list with the user ds name
//
    if (class_list.empty())
	class_list = "'" + user_ds_name + "'";
    if (mysql_db == "tango")
    {
        query = "SELECT CONCAT(domain, '/', family, '/', member) FROM device WHERE class IN (" + class_list + ")";
    }
    else
    {
        query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) FROM NAMES WHERE DEVICE_SERVER_CLASS IN (" + class_list + ")";
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    string	dev_list("");
    result = mysql_store_result(mysql_conn);
    while((row = mysql_fetch_row(result)) != NULL)
    {
	if (dev_list.empty())
	    dev_list = "'" + string(row[0]) + "'";
	else
	    dev_list += ", '" + string(row[0]) + "'";
    }
    mysql_free_result(result);
    if (dev_list.empty())
    {
	errcode = DbErr_DeviceServerNotDefined;
	return (&errcode);
    }
//
// Delete every resource for all devices in the ds name list
//
    if (mysql_db != "tango")
    {
	if (mysql_db == "tango")
	{
	    query = "DELETE FROM property_device WHERE device IN (" + dev_list + ")";
	}
	else
	{
            query = "DELETE FROM RESOURCE WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) IN (" + dev_list + ")";
	}
        if (mysql_query(mysql_conn, query.c_str()) != 0)
        {
	    cerr << mysql_error(mysql_conn) << endl;
	    errcode = DbErr_DatabaseAccess;
	    return (&errcode);
        }
    }
//
// Delete every devices for each ds name in the list
//
    if (mysql_db == "tango")
    {
        query = "DELETE FROM device WHERE CONCAT(domain, '/', family, '/', member) IN (" + dev_list + ")"; 
    }
    else
    {
        query = "DELETE FROM NAMES WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) IN (" + dev_list + ")"; 
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
//
// Leave call
//				
    return(&errcode);
}


/****************************************************************************
*                                                                           *
*	Server code for the getpoller_1_svc function       	   	    *
*                           ---------------                       	    *
*                                                                           *
*    Method rule : To retrieve poller info from a device name	    	    *
*                                                                           *
*    Argin : - dev : The device name					    *
*                                                                           *
*    Argout : No argout			    				    *
*                                                                           *
*    This function returns a pointer to an error code			    *
*                                                                           *
****************************************************************************/
db_poller_svc *MySQLServer::getpoller_1_svc(nam *dev)
{
    string 	poller_name,
    		user_device(*dev);
#ifdef DEBUG
    cout << "In getpoller_1_svc function for device " << user_device << endl;
#endif


//
// Initialize parameter sent back to client
//
    poll_back.db_err = 0;
    poll_back.pid = 0;
    poll_back.server_name = NULL;
    poll_back.personal_name = NULL;
    poll_back.host_name = NULL;
    poll_back.process_name = NULL;
		
    try
    {	
	poll_back.server_name = new char[DS_NAME_LENGTH];
	poll_back.server_name[0] = '\0';

	poll_back.personal_name = new char[DSPERS_NAME_LENGTH];
	poll_back.personal_name[0] = '\0';

	poll_back.process_name = new char[PROC_NAME_LENGTH];
	poll_back.process_name[0] = '\0';

	poll_back.host_name = new char[HOST_NAME_LENGTH];	
	poll_back.host_name[0] = '\0';
    }
    catch (bad_alloc)
    {
	cerr << "Memory allocation error in devinfo" << endl;
	poll_back.db_err = DbErr_ServerMemoryAllocation;
	return(&poll_back);
    }
//
// If the server is not connected to the database, return error
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	poll_back.db_err = DbErr_DatabaseNotConnected;
	return(&poll_back);
    }
//
// Find the db table for the sys domain
//
    long i;
    for (i = 0;i < dbgen.TblNum;i++)
	if (dbgen.TblName[i] == "sys")		// DOMAIN = 'sys'
	    break;
    if (i == dbgen.TblNum)
    {
	poll_back.db_err = DbErr_DomainDefinition;
	return(&poll_back);
    }
//
// Search for a resource "ud_poll_list" with its value set to caller device name
//
    string query;

    if (mysql_db == "tango")
    {
        query = "SELECT device FROM property_device WHERE domain = 'sys' AND";
        query += ("name = '" + string(POLL_RES) + "' AND UPPER(value) = UPPER('" + user_device + "')");
    }
    else
    {
        query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) FROM RESOURCE WHERE DOMAIN = 'sys' AND";
        query += ("NAME = '" + string(POLL_RES) + "' AND UPPER(RESVAL) = UPPER('" + user_device + "')");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	poll_back.db_err = DbErr_DatabaseAccess;
	return (&poll_back);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
    {
//				
// A poller has been found, build its device name
//
	poller_name = row[0];
    }			
    else
    {
//
// Return error if no poller has been found
//
	mysql_free_result(result);
	poll_back.db_err = DbErr_NoPollerFound;
	return(&poll_back);
    }
    mysql_free_result(result);
	
//
// get poller device info from the NAMES table
//
    query = "SELECT DEVICE_SERVER_CLASS, DEVICE_SERVER_NAME, HOSTNAME, PROCESS_NAME, PROCESS_ID FROM NAMES";
    query += (" WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + poller_name + "'");
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	cerr << mysql_error(mysql_conn) << endl;
	poll_back.db_err = DbErr_DatabaseAccess;
	return (&poll_back);
    }
    result = mysql_store_result(mysql_conn);
    if ((row = mysql_fetch_row(result)) != NULL)
    {
	strcpy(poll_back.server_name, row[0]);
	strcpy(poll_back.personal_name, row[1]);
	strcpy(poll_back.host_name, row[2]);
	strcpy(poll_back.process_name, row[3]);
	poll_back.pid = atoi(row[4]);
    }
    else
    {
//
// Return error if the device is not found
//
	mysql_free_result(result);
	poll_back.db_err = DbErr_DeviceNotDefined;
	return(&poll_back);			
    }
//
// Return data
//
    return(&poll_back);
}
