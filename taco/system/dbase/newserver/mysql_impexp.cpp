#include <MySqlServer.h>


/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 1 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number and the version      *
*                    number of the device server for a specific device      *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*         Zero means no error                                               * 
*                                                                           *
*****************************************************************************/
DevLong *MySQLServer::db_devexp_1_svc(tab_dbdev *rece)
{
    u_int 	num_dev = rece->tab_dbdev_len;;

#ifdef DEBUG
    for (int i=0;i<num_dev;i++)
    {
	cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
	cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
	cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
	cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
	cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
	cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
    }
#endif
//
// Initialize error code sended back to client 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database
//
    try
    {
    	for (int i = 0; i < num_dev; i++)
	    db_store(rece->tab_dbdev_val[i]);
    }
    catch (const long 	db_err)
    {
	errcode = db_err;
    }
    return(&errcode);
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 2 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number, the version         *
*                    number and the process ID of the device server for a   *
*                    specific device      				    *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*    Zero means no error                                                    * 
*                                                                           *
*****************************************************************************/
DevLong *MySQLServer::db_devexp_2_svc(tab_dbdev_2 *rece)
{
    u_int 	num_dev = rece->tab_dbdev_len;;

#ifdef DEBUG
    for (int i = 0; i < num_dev; i++)
    {
	cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
	cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
	cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
	cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
	cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
	cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
	cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << endl;
    }
#endif
//
// Initialize error code sended back to client 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database 
//
    try
    {
    	for (int i = 0; i < num_dev; i++)
	    db_store(rece->tab_dbdev_val[i]);
    }
    catch (const long 	db_err)
    {
	errcode = db_err;
    }
//
// Leave function
//
    return(&errcode);
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_export version 3 function            *
*                               -----------------------                     *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number, the version         *
*                    number and the process ID of the device server for a   *
*                    specific device      				    *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures bm_devinfo *
*              bm_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*    Zero means no error                                                    * 
*                                                                           *
*****************************************************************************/
DevLong *MySQLServer::db_devexp_3_svc(tab_dbdev_3 *rece)
{
    u_int 	num_dev = rece->tab_dbdev_len;

#ifdef DEBUG
    for (int i = 0; i < num_dev; i++)
    {
	cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << endl;
	cout << "Host name : " << rece->tab_dbdev_val[i].host_name << endl;
	cout << "Program number : " << rece->tab_dbdev_val[i].p_num << endl;
	cout << "Version number : " << rece->tab_dbdev_val[i].v_num << endl;
	cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << endl;
	cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << endl;
	cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << endl;
	cout << "Device server process name : " << rece->tab_dbdev_val[i].proc_name << endl;
    }
#endif
//
// Initialize error code sended back to client */
//
    errcode = 0;
//
// Return error code if the server is not connected to the database */
//
    if (dbgen.connected == False)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database */
//
    try
    {
    	for (int i = 0; i < num_dev; i++)
	    db_store(rece->tab_dbdev_val[i]);
    }
    catch (const long 	db_err)
    {
	errcode = db_err;
    }
    return(&errcode);
}

/****************************************************************************
*                                                                           *
*		Server code for db_dev_import function                      *
*                               -------------                               *
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
db_resimp *MySQLServer::db_devimp_1_svc(arr1 *de_name)
{
    int 	resu,
		num_dev = de_name->arr1_len;
    device 	ret;
    db_devinfo 	*stu_addr;
    string 	ret_host_name,
		ret_dev_name,
		ret_dev_type,
		ret_dev_class;
    int 	ret_pn,
		ret_vn;
	
#ifdef DEBUG
    for (int i = 0; i < num_dev; i++)
	cout << "Device name (in import function) : " << de_name->arr1_val[i] << endl;
#endif
//
// Initialize error code sended back to client */
//
    back.db_imperr = 0;
//
// Return error code if the server is not connected to the database */
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
	back.db_imperr = DbErr_DatabaseNotConnected;
	back.imp_dev.tab_dbdev_len = 0;
	back.imp_dev.tab_dbdev_val = NULL;
	return(&back);
    }
//
// Allocate memory for the array of db_devinfo structures */
//
    int i = 0;
    try
    {
    	back.imp_dev.tab_dbdev_val = new db_devinfo[num_dev];
//
// A loop on every device to import 
//
    	for (i = 0; i < num_dev; i++)
    	{
//
// Try to retrieve the tuple in the NAMES table 
//
	    string dev_name(de_name->arr1_val[i]);
	    string query = "SELECT HOSTNAME, PROGRAM_NUMBER, VERSION_NUMBER, DEVICE_TYPE, DEVICE_CLASS";
	    query += (" FROM NAMES WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" + dev_name + "'");
	    if (mysql_query(mysql_conn, query.c_str()) != 0)
	    {
		throw;
	    }
	    MYSQL_RES	*result = mysql_store_result(mysql_conn);
	    MYSQL_ROW	row;
	    int		n_rows;
            if ((n_rows = mysql_num_rows(result)) == 1)
	    	if ((row = mysql_fetch_row(result)) != NULL)
	    	{
//
// Unpack the content 
//
	    	    ret_host_name = row[0];
	    	    ret_pn = atoi(row[1]);
	    	    ret_vn = atoi(row[2]);
	    	    ret_dev_type = row[3];
	    	    ret_dev_class = row[4];
	    	}
//
// In case of error 
//
	    mysql_free_result(result); 
	    if (ret_pn == 0 || n_rows != 1)
	    {
	    	for (int j = 0; j < i; j++)
	    	{
		    stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
		    delete [] stu_addr->dev_name;
		    delete [] stu_addr->host_name;
		    delete [] stu_addr->dev_type;
		    delete [] stu_addr->dev_class;
	    	}
	    	back.imp_dev.tab_dbdev_len = 0;
	    	if (n_rows == 0)
		    back.db_imperr = DbErr_DeviceNotDefined;
	    	else if (n_rows != 1)
		    back.db_imperr = DbErr_DatabaseAccess;
	    	else
		    back.db_imperr = DbErr_DeviceNotExported;
	    	return(&back);
	    }
	    stu_addr = &(back.imp_dev.tab_dbdev_val[i]);
//
// Allocate memory for the host_name string 
//
	    stu_addr->host_name = new char[ret_host_name.length() + 1];
//
// Allocate memory for the device name string 
//
	    stu_addr->dev_name = new char[dev_name.length() + 1];
//
// Allocate memory for the device type string 
//
	    stu_addr->dev_type = new char[ret_dev_type.length() + 1];
//
// Allocate memory for the device class string 
//
	    stu_addr->dev_class = new char[ret_dev_class.length() + 1];
//
// Initialize structure sended back to client 
//
	    strcpy(stu_addr->host_name,ret_host_name.c_str());
	    strcpy(stu_addr->dev_name,dev_name.c_str());
	    stu_addr->p_num = ret_pn;
	    stu_addr->v_num = ret_vn;
	    strcpy(stu_addr->dev_class,ret_dev_class.c_str());
	    strcpy(stu_addr->dev_type,ret_dev_type.c_str());
    	} 
        back.imp_dev.tab_dbdev_len = num_dev;
    }
    catch(bad_alloc)
    {
	delete [] stu_addr->host_name;
	delete [] stu_addr->dev_name;
	delete [] stu_addr->dev_type;
	delete [] stu_addr->dev_class;
	for (int j = 0; j < i; j++)
	{
	    stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
	    delete [] stu_addr->dev_name;
	    delete [] stu_addr->host_name;
	    delete [] stu_addr->dev_class;
	    delete [] stu_addr->dev_type;
	}
	back.imp_dev.tab_dbdev_len = 0;
	back.db_imperr = DbErr_ServerMemoryAllocation;
    }
    return(&back);
}

/****************************************************************************
*                                                                           *
*		Server code for db_svc_unreg function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To unregister from database all the devices driven by  *
*                    a device server                                        *
*                                                                           *
*    Argin : The network device server name                                 *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an int which is an error code                    *
*       Zero means no error                                                 *
*                                                                           *
****************************************************************************/
DevLong *MySQLServer::db_svcunr_1_svc(nam *dsn_name)
{
    int 		d_num = 0;
    string		ds_class,
			ds_name,
			device(*dsn_name);
    string::size_type	pos;

#ifdef DEBUG
    cout << "Device server name (unregister function) : " << device << endl;
#endif
//
// Miscallaneous initialization 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database */
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Get device server class */
//
    if ((pos = device.find('/')) == string::npos)
    {
	errcode = DbErr_BadDevSyntax;
	return (&errcode);
    }
    ds_class = device.substr(0, pos);
//
// Get device server name 
//
    ds_name = device.substr(pos + 1);

#ifdef DEBUG
    cout << "Device server class (unreg) : " << ds_class << endl;
    cout << "Device server name (unreg) : " << ds_name << endl;
#endif
//
// Try to retrieve devices in database assuming that the input device server
// name is the device server PROCESS name. As there is no key build on
// the device server process name, do a full traversal of the database 
//
    string query = "UPDATE NAMES SET PROGRAM_NUMBER = 0, VERSION_NUMBER = 0,";
    query += (" DEVICE_TYPE = 'unknown', PROCESS_ID = 0 WHERE");
    query += (" DEVICE_SERVER_CLASS = '" + ds_class + "' AND");
    query += (" PROCESS_NAME = '" + ds_name + "' AND PROCESS_ID != 0");

    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	errcode = DbErr_DatabaseAccess;
	return (&errcode);
    }
    
//
// Initialization needed to retrieve the right tuples in the NAMES table
// and to update the tuples (program and version number) assuming the input
// name is a device server name
//
    if ((d_num = mysql_affected_rows(mysql_conn)) == 0)
    {
     	query = "UPDATE NAMES SET PROGRAM_NUMBER = 0, VERSION_NUMBER = 0,";
    	query += (" DEVICE_TYPE = 'unknown', PROCESS_ID = 0 WHERE");
	query += (" DEVICE_SERVER_CLASS = '" + ds_class + "' AND");
	query += (" DEVICE_SERVER_NAME = '" + ds_name + "'");
	if (mysql_query(mysql_conn, query.c_str()) != 0)
	{
	    errcode = DbErr_DatabaseAccess;
	    return (&errcode);
	}
	d_num = mysql_affected_rows(mysql_conn);
    }
//
// In case of trouble 
//
    if (d_num == 0)
    {
	errcode = DbErr_DeviceServerNotDefined;
	return(&errcode);
    }
//
// No error 
//
    return(&errcode);
}

/****************************************************************************
*                                                                           *
*		Server code for db_svc_check function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (and send back to client) the program      *
*                    number and version number for a device server          *
*                                                                           *
*    Argin : The network device server name                                 *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a stucture of the "svc_inf" type    *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*	char *ho_name;  host name					    *	
*   	u_int p_num;	the program number                                  *
*       u_int v_num;    the version number                                  *
*       int db_err;     an error code (0 if no error)                       *
*           }                                                               *
*                                                                           *
****************************************************************************/
svc_inf *MySQLServer::db_svcchk_1_svc(nam *dsn_name)
{
    static char 	host_name[HOST_NAME_LENGTH];
    string		ds_class,
			ds_name,
			device(*dsn_name);
    string::size_type	pos;

#ifdef DEBUG
    cout << "Device server name (check function) : " << device << endl;
#endif 
//
// Miscalaneous initialization */
//
    host_name[0] = '\0';
    svc_info.ho_name = host_name;
    svc_info.p_num = 1;
    svc_info.v_num = 1;
    svc_info.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	cerr << "I'm not connected to database." << endl;
	svc_info.db_err = DbErr_DatabaseNotConnected;
	return(&svc_info);
    }
//
// Get device server class */
//
    if ((pos = device.find('/')) == string::npos)
    {
    	svc_info.db_err = DbErr_BadDevSyntax; 
	return (&svc_info);
    }
    ds_class = device.substr(0, pos);
//
// Get device server name
//
    ds_name = device.substr(pos + 1);

#ifdef DEBUG
    cout << "Device server class (check) : " << ds_class << endl;
    cout << "Device server name (check) : " << ds_name << endl;
#endif
//
// Initialization needed to retrieve the right tuples in the NAMES table 
//
    string query = "SELECT HOSTNAME, PROGRAM_NUMBER, VERSION_NUMBER FROM NAMES WHERE";
    query += (" DEVICE_SERVER_CLASS = '" + ds_class + "' AND");
    query += (" DEVICE_SERVER_NAME = '" + ds_name + "' AND");
    query += (" INDEX_NUMBER = 1");
//
// Try to retrieve the tuples 
//
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	svc_info.db_err = DbErr_DatabaseAccess;
	return (&svc_info);
    }
    MYSQL_RES 	*result = mysql_store_result(mysql_conn);

    if (mysql_num_rows(result) == 1)
    {
    	MYSQL_ROW	row = mysql_fetch_row(result);
	    
	strcpy(host_name, row[0]);
	svc_info.ho_name = host_name;
    	svc_info.p_num = atoi(row[1]);
    	svc_info.v_num = atoi(row[2]);
    }
    else
	svc_info.db_err = DbErr_DeviceServerNotDefined;
    mysql_free_result(result);
//
// Leave function 
//
    return(&svc_info);
}

/****************************************************************************
*                                                                           *
*		Code for db_store function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number and version number)                                   *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_store(db_devinfo &dev_stu)
{
    int errorcode = 0;
//
// Try to retrieve the right tuple in the NAMES table 
//
    stringstream query;
    query << "UPDATE NAMES SET HOSTNAME = '" << dev_stu.host_name <<  "',"
      << " PROGRAM_NUMBER = " << dev_stu.p_num << ","
      << " VERSION_NUMBER = " << dev_stu.v_num << ","
      << " DEVICE_TYPE = '" << dev_stu.dev_type << "',"
      << " DEVICE_CLASS = '" << dev_stu.dev_class << "',"
      << " PROCESS_ID = 0, PROCESS_NAME = 'unknown'"
      << " WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" << dev_stu.dev_name << "'" << ends; 	
    try
    {
#if !HAVE_SSTREAM
   	if (mysql_query(mysql_conn, query.str()) != 0)
	    throw int(DbErr_DatabaseAccess);
        query.freeze(false);
#else
   	if (mysql_query(mysql_conn, query.str().c_str()) != 0)
	    throw int(DbErr_DatabaseAccess);
#endif
   	if (mysql_affected_rows(mysql_conn) == 0)
	    throw int(DbErr_DeviceNotDefined);
    }
    catch(const int err)
    {
	errorcode = err;
    }
//
// Leave function
//
//  query.freeze(false);
    return(errcode);
}

/****************************************************************************
*                                                                           *
*		Code for db_store function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number and version number)                                   *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_store(db_devinfo_2 &dev_stu)
{
    int errorcode = 0;
//
// Try to retrieve the right tuple in the NAMES table 
//
    stringstream query;
    query << "UPDATE NAMES SET HOSTNAME = '" << dev_stu.host_name <<  "',"
      << " PROGRAM_NUMBER = " << dev_stu.p_num << ","
      << " VERSION_NUMBER = " << dev_stu.v_num << ","
      << " DEVICE_TYPE = '" << dev_stu.dev_type << "',"
      << " DEVICE_CLASS = '" << dev_stu.dev_class << "',"
      << " PROCESS_ID = " << dev_stu.pid << ", PROCESS_NAME = 'unknown'"
      << " WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" << dev_stu.dev_name << "'" << ends; 	

    try
    {
#if !HAVE_SSTREAM
   	if (mysql_query(mysql_conn, query.str()) != 0)
	    throw int(DbErr_DatabaseAccess);
        query.freeze(false);
#else
   	if (mysql_query(mysql_conn, query.str().c_str()) != 0)
	    throw int(DbErr_DatabaseAccess);
#endif
   	if (mysql_affected_rows(mysql_conn) == 0)
	    throw int(DbErr_DeviceNotDefined);
    }
    catch(const int err)
    {
	errorcode = err;
    }
//
// Leave function
//
//    query.freeze(false);
    return(errorcode);
}

/****************************************************************************
*                                                                           *
*		Code for db_store function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To store in the NAMES table of the database the        *
*                    number and the version number of the device server     *
*                    in charge of a device                                  *
*		     This function is for the version 3 of the db_dev_export*
*		     call						    *
*                                                                           *
*    Argin : - A db_devinf structure (with device name, host name, program  *
*              number, version number and process name)                     *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_store(db_devinfo_3 &dev_stu)
{
    int errorcode = 0;
//
// Try to retrieve the right tuple in the NAMES table 
//
    stringstream query;
    query << "UPDATE NAMES SET HOSTNAME = '" << dev_stu.host_name <<  "',"
      << " PROGRAM_NUMBER = " << dev_stu.p_num << ","
      << " VERSION_NUMBER = " << dev_stu.v_num << ","
      << " DEVICE_TYPE = '" << dev_stu.dev_type << "',"
      << " DEVICE_CLASS = '" << dev_stu.dev_class << "',"
      << " PROCESS_ID = " << dev_stu.pid << ", PROCESS_NAME = '" << dev_stu.proc_name << "'"
      << " WHERE CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) = '" << dev_stu.dev_name << "'" << ends; 	

    try
    {
#if !HAVE_SSTREAM
   	if (mysql_query(mysql_conn, query.str()) != 0)
	    throw int(DbErr_DatabaseAccess);
        query.freeze(false);
#else
   	if (mysql_query(mysql_conn, query.str().c_str()) != 0)
	    throw int(DbErr_DatabaseAccess);
#endif
   	if (mysql_affected_rows(mysql_conn) == 0)
	    throw int(DbErr_DeviceNotDefined);
    }
    catch(const int err)
    {
	errorcode = err;
    }
//
// Leave function
//
//    query.freeze(false);
    return(errorcode);
}
