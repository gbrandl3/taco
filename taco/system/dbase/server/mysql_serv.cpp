#include <DevErrors.h>
#include <MySqlServer.h>


/****************************************************************************
*                                                                           *
*		Server code for db_getresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To retrieve from the database (builded from resources  *
*		     files) a resource value                                *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*              u_int arr1_len;     The number of strings                    *
*              char **arr1_val;    A pointer to the array of strings        *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     resources values information transferred as strings   *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*          }                                                                *
*                                                                           *
*****************************************************************************/
db_res *MySQLServer::db_getres_1_svc(arr1 *rece, struct svc_req *rqstp)
{
    int 		j,
    			k = 0;
    u_int 		num_res,
			err_db;
    string		tab_name,
    			rest;
    struct sockaddr_in 	so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
    socklen_t  		so_size;   /* from POSIX draft - already used by GLIBC */
#else
    int 		so_size;
#endif
    u_short 		prot;
    int 		k1 = 1;

#if DEBUG 
    std::cout << "In db_getres_1_svc " << std::endl;
#endif
//
// Return error code if the server is not connected to the database 
//
    if (dbgen.connected == False)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return(&browse_back);
    }
//
// Retrieve the protocol used to send this request to server 
//
    so_size = sizeof(so);
#ifdef sun
    if (rqstp->rq_xprt->xp_port == udp_port)
        prot = IPPROTO_UDP;
    else
        prot = IPPROTO_TCP;
#else
    so_size = sizeof(so);
    if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so, (socklen_t *)&so_size) == -1)
    {
	browse_back.db_err = DbErr_TooManyInfoForUDP;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

    if (so.sin_port == getUDPPort())
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#endif 
    num_res = rece->arr1_len;
#ifdef DEBUG
    for(int i = 0; i < num_res; i++)
	std::cout << "Resource name : " << rece->arr1_val[i] << std::endl;
#endif
//
// Initialize browse_back structure error code 
//
    browse_back.db_err = 0;
//
// Allocate memory for the array of string sended back to client 
//
    int i = 0;
    try
    {
    	browse_back.res_val.arr1_val = new nam[num_res];
//
// A loop on the resource's number to be looked for
//
    	for(i = 0; i < num_res; i++)
    	{
	    string ptrc = rece->arr1_val[i];
//
// Find the table name (DOMAIN) 
//
	    string::size_type pos = ptrc.find('/');
	    tab_name = ptrc.substr(0, pos);
	    rest = ptrc.substr(pos + 1);
//
// Try to find the resource value from database 
//
	    if((err_db = db_find(tab_name, rest, &browse_back.res_val.arr1_val[i], &k1)) != 0)
	    {
	    	for (int j = 0; j <= i; j++)
		    delete [] browse_back.res_val.arr1_val[j];
		delete [] browse_back.res_val.arr1_val;
	    	browse_back.db_err = err_db;
	    	browse_back.res_val.arr1_len = 0;
	    	return(&browse_back);
	    }
//
// Compute an estimation of the network packet size (Only if the UDP protocol
// has been used to send this request to the server) 
//
	    if (prot == IPPROTO_UDP)
	    {
	    	if ((k = strlen(browse_back.res_val.arr1_val[i]) + k) > SIZE - 1000)
	    	{
		    for (int j = 0; j <= i; j++)
		    	delete [] browse_back.res_val.arr1_val[j];
		    delete [] browse_back.res_val.arr1_val;
		    browse_back.db_err = DbErr_TooManyInfoForUDP;
		    browse_back.res_val.arr1_len = 0;
		    return(&browse_back);
	    	}
	    }
    	}
//
// Initialize the structure sended back to client 
//
    	browse_back.res_val.arr1_len = num_res;
    }
    catch(bad_alloc)
    {
	for (int j = 0; j <= i; j++)
	    delete [] browse_back.res_val.arr1_val[j];
	delete [] browse_back.res_val.arr1_val;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	browse_back.res_val.arr1_len = 0;
    }
//
// Exit server 
//
    return(&browse_back);
}


/****************************************************************************
*                                                                           *
*		Server code for db_getdevlist function                      *
*                               -------------                               *
*                                                                           *
*    Function rule : To retrieve all the names of the devices driven by a   *
*                    device server.                                         *
*                                                                           *
*    Argin : The name of the device server                                  *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     devices names                                         *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_res *MySQLServer::db_getdev_1_svc(nam *dev_name)
{
    int 	dev_num,
		err_db;

#ifdef DEBUG
    std::cout << "Device server name (getdevlist) : " << *dev_name << std::endl;
#endif
//
// Initialize error code sended back to client 
//
    browse_back.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
    if (dbgen.connected == False)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return(&browse_back);
    }
//
// Call database function 
//
    if ((err_db = db_devlist(*dev_name, &dev_num, &browse_back)) != 0)
    {
	browse_back.db_err = err_db;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }
#ifdef DEBUG
    for (int i = 0; i < dev_num; i++)
	std::cout << "Device name : " << browse_back.res_val.arr1_val[i] << std::endl;
#endif
//
// Exit server
//
    return(&browse_back);
}



/****************************************************************************
*                                                                           *
*		Code for db_find function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To retrieve a resource value in the database           *
*                                                                           *
*    Argin : - The table name where the ressource can be retrieved          *
*            - A part of the resource name (FAMILY/MEMBER/RES.)             *
*            - The adress where to put the resource value (as a string)     *
*	     - The buffer's address used to store temporary results         *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_find(string tab_name, string p_res_name, char **out, int *k1)
{
    string		family,
			member,
			r_name,
    			adr_tmp1;
    string::size_type	pos,
			last_pos;
    int 		k,
			sec_res,
    			i;

#ifdef DEBUG
    std::cout << "Table name : " << tab_name << std::endl;
#endif
//
// Set a flag if the resource belongs to security domain 
//
    sec_res = (tab_name == "sec"); 
//
// Get family name 
//
    pos = p_res_name.find('/');
    family = p_res_name.substr(0, pos);
//
// Get member name
//
    pos = p_res_name.find('/', (last_pos = pos + 1));
    member = p_res_name.substr(last_pos, pos - last_pos);
//
// Get resource name
//
    r_name = p_res_name.substr(pos + 1);
    if ((pos = r_name.find('/')) != string::npos)
	r_name.erase(pos,1);
//
// For security domain, change all occurances of | by ^ (| is the field
// separator in NDBM !) 
// 
    if (sec_res == True)
    {
	k = r_name.length();
	for (int i = 0; i < k; i++)
	    if (r_name[i] == '|')
		r_name[i] = SEC_SEP;
    }

#ifdef NEVER
    std::cout << "Family name : " << family << std::endl;
    std::cout << "Member name : " << member << std::endl;
    std::cout << "Resource name : " << r_name << std::endl;
#endif
//
// Select the right resource table in the right database
//
    for (i = 0;i < dbgen.TblNum;i++)
	if (tab_name == dbgen.TblName[i])
	    break;
    if (i >= dbgen.TblNum)
	return(DbErr_DomainDefinition);
//
// Try to retrieve the resource in table and loop in the case of an
// array of resources 
//
    string query;
    if (mysql_db == "tango")
    {
        query = "SELECT count, value FROM property_device ";
        query += ("WHERE device = '" + tab_name + "/" + family + "/" + member + "' AND name = '" + r_name);
        query += "' ORDER BY count ASC";
    }
    else
    {
        query = "SELECT INDEX_RES, RESVAL FROM RESOURCE WHERE DOMAIN = '" + tab_name ;
        query += ("' AND FAMILY = '" + family + "' AND MEMBER = '" + member + "' AND NAME = '" + r_name);
        query += "' ORDER BY INDEX_RES ASC";
    }
#ifdef DEBUG
    cout << "MySQLServer::db_find(): query = " << query << endl;
#endif /* DEBUG */
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    MYSQL_RES	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
    *k1 = mysql_num_rows(result);
    for (i = 0; i < *k1 && ((row = mysql_fetch_row(result)) != NULL); i++)
    {
	if (i)
	{
	    adr_tmp1 += SEP_ELT;
	    adr_tmp1 += row[1];
	}
	else
	    adr_tmp1 = row[1];
    }
    try
    {
    	switch (i)
    	{
//
// If it is a normal resource,so copy the resource value to the result buffer 
//
	    case 1 : *out = new char[adr_tmp1.length() + 1];
		     strcpy(*out, adr_tmp1.c_str());
		     break;
//
// Initialize resource value to N_DEF if the resource is not defined in the
// database 
//
    	    case 0 : *out = new char[10];
		     strcpy(*out,"N_DEF");
		     break;
//
// For an array of resource 
//
    	    default: *out = new char[adr_tmp1.length() + 10];
		     (*out)[0] = INIT_ARRAY;
		     sprintf(&((*out)[1]),"%d", i);
		     k = strlen(*out);
		     (*out)[k] = SEP_ELT;
		     (*out)[k + 1] = 0;
		     strcat(*out, adr_tmp1.c_str());
		     break;
    	}
    }
    catch(const bad_alloc &e)
    {
	std::cerr << "Error in malloc for out" << std::endl;
	throw e;
    }
//
// For resource of the SEC domain, change all occurences of the ^ character
// to the | character 
//
    if (sec_res)
    {
	k = strlen(*out);   
   	for (int i = 0;i < k;i++)
	    if ((*out)[i] == SEC_SEP)
		(*out)[i] = '|';
    }
    return(0);
}


/****************************************************************************
*                                                                           *
*		Code for db_devlist function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To retrieve all the devices name for a particular      *
*                    device server                                          *
*                                                                           *
*    Argin : - The  device server name                                      *
*            - The adress  of the db_res structure to be initialized with    *
*              the devices names                                            *
*              (see the definition of the db_res structure above)           *
*                                                                           *
*    Argout : - The number of devices managed by the devices server         *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_devlist(string dev_na, int *dev_num, db_res *back)
{
    register char 	**ptra;
    int 		dev_numb = 1;
    string::size_type	pos;
    unsigned int 	diff;
    string		ds_class,
			ds_name;

//
// Get device server type 
//
    pos = dev_na.find('/'); 
    ds_class = dev_na.substr(0, pos);
//
// Get device type 
//
    ds_name = dev_na.substr(pos + 1);
//
#ifdef DEBUG
    std::cout << "Device server class (getdevlist) : " << ds_class << std::endl;
    std::cout << "Device server name (getdevlist) : " << ds_name << std::endl;
#endif 
//
// Try to retrieve the right tuple in NAMES table 
//
//  
    string query;
    if (mysql_db == "tango")
    {
    	query = "SELECT name FROM device WHERE server = '" + ds_class + "/" + ds_name + "'";
    }
    else
    {
    	query = "SELECT DEVICENAME FROM NAMES WHERE DEVICE_SERVER_CLASS = '" + ds_class + "' AND DEVICE_SERVER_NAME = '" + ds_name + "'";
    }
#ifdef DEBUG
    std::cout << "MySQLServer::db_devlist(): query " << query << std::endl;
#endif /* DEBUG */
    
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    
    MYSQL_RES	*result = mysql_store_result(mysql_conn);
    MYSQL_ROW	row;
    int 	d_num = mysql_num_rows(result);
    if (d_num == 0)
	return(DbErr_DeviceServerNotDefined);
//
// Allocate memory for the pointer's array 
//
    int i = 0;
    try
    {
    	ptra = new nam[d_num];
    
    	for (i = 0; i < d_num && ((row = mysql_fetch_row(result)) != NULL); i++)
    	{
//
// Allocate memory for device name 
//
	    ptra[i] = new char[strlen(row[0]) + 1];
//
// Copy the device name 
//
	   strcpy(ptra[i], row[0]);
    	}
//
// Initialize the structure 
//
    	back->res_val.arr1_val = ptra;
    	back->res_val.arr1_len = *dev_num = d_num;
        mysql_free_result(result); 
    }
    catch(bad_alloc)
    {
    	mysql_free_result(result); 
        for (int j = 0; j < i; j++)
	    delete [] ptra[j];
	delete [] ptra;
	back->res_val.arr1_val = NULL;
    	back->res_val.arr1_len = *dev_num = 0;
	return(DbErr_ServerMemoryAllocation);
    }
    return 0;
}



/****************************************************************************
*                                                                           *
*		Server code for db_putresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To insert or update resources    		    	    *
*                                                                           *
*    Argin : A pointer to a structure of the tab_putres type		    *
*            The definition of the tab_putres type is :                     *
*	     struct {							    *
*		u_int tab_putres_len;	The number of resources to be       *
*					updated or inserted		    *
*		putres tab_putres_val;	A pointer to an array of putres     *
*					structure. Each putres structure    *
*					contains the resource name and      *
*                                       the resource value		    *
*		    }							    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This funtion returns 0 if no error occurs. Otherwise an error code is  *
*    returned								    *
*                                                                           *
****************************************************************************/
DevLong *MySQLServer::db_putres_1_svc(tab_putres *rece)
{
    int 		res_num = rece->tab_putres_len,
			res_numb = 1;
    string::size_type	pos,
			last_pos;
    string		res_name,
			res_val;
    register putres 	*tmp_ptr;
    unsigned int 	ctr;
    char 		indnr[9];


#ifdef DEBUG
    for (int i = 0; i < res_num; i++)
	std::cout << "Resource name : " << rece->tab_putres_val[i].res_name << std::endl;
#endif 
//
// Initialize sent back error code 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database 
//
    if (dbgen.connected == False)
    {
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode); 
    }
//
// A loop for every resource 
//
    for (int ires = 0; ires < res_num; ires++)
    {
	string 	content;
	tmp_ptr = &(rece->tab_putres_val[ires]);
	res_numb = 1;
	res_name = tmp_ptr->res_name;

	if (db_del(res_name) != 0)
	    std::cerr << "Could not delete resource" << res_name << std::endl;
	res_val = tmp_ptr->res_val;
//
// Try to retrieve this resource from the database 
//
// If the new update is for an array 
//
	if (res_val[0] == INIT_ARRAY)
	{
//
// Retrieve the number of element in the array.
// Initialize the loop counter "ctr" to number of element minus one because
// it is not necessary to look for the element separator to extract the last
// element value from the string. 
//
	    if ((pos = res_val.find(SEP_ELT)) == string::npos)
	    {
		std::cerr << "Missing '" << SEP_ELT <<"' in resource value." << std::endl;
		errcode = DbErr_BadDevSyntax;
		return (&errcode);
	    }
	    ctr = (unsigned int)atoi(res_val.substr(1, pos - 1).c_str());
	    res_numb = 1;

	    for (int l = 0; l < ctr; l++)
	    {
//
// Initialize database information 
//
		sprintf(indnr,"%d", res_numb++);
//
// Add one array element in the database 
//
		pos = res_val.find(SEP_ELT, 1 + (last_pos = pos));
		content = res_val.substr(pos + 1, last_pos - pos);
		if (db_insert(res_name, indnr, content) != 0)
		{
		    errcode = DbErr_DatabaseAccess;
		    return(&errcode);
		}
	    } 
//
// For the last element value 

	    content = res_val.substr(pos + 1);
	    sprintf(indnr,"%d",res_numb++);
	    if (db_insert(res_name, indnr, content) != 0)
	    {
		errcode = DbErr_DatabaseAccess;
		return(&errcode);
	    }
	}
	else
	{
	    sprintf(indnr,"%d",res_numb++);
	    if (db_insert(res_name, indnr, res_val) != 0)
	    { 
		errcode = DbErr_DatabaseAccess;
		return(&errcode);
	    } 
	} 
    } 
//
// Leave server
//
    return(&errcode);
}


/****************************************************************************
*                                                                           *
*		Server code for db_delresource function                     *
*                               --------------                              *
*                                                                           *
*    Function rule : To delete resources from the database (builded from    *
*		     resource files)					    *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*              u_int arr1_len;     The number of strings                    *
*              char **arr1_val;    A pointer to the array of strings        *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a int. This int is the error code   *
*    It is set to 0 is everything is correct. Otherwise, it is initialised  *
*    with the error code.						    *
*                                                                           *
*****************************************************************************/
DevLong *MySQLServer::db_delres_1_svc(arr1 *rece/*, struct svc_req *rqstp*/)
{
    int 	j;
    u_int 	num_res = rece->arr1_len,
		err_db;
    register char *ptrc;

#ifdef DEBUG
    for(int i = 0; i < num_res; i++)
	std::cout << "Resource to delete : " << rece->arr1_val[i] << std::endl;
#endif
//
// Initialize error code 
//
    errcode = 0;
//
// Return error code if the server is not connected to the database files
//
    if (dbgen.connected == False)
    {
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Mark the server as not connected. This will prevent dbm_update to
// add/modify resources during this call
//
    dbgen.connected = False;
//
// Allocate array for pointers to store deleted resources value
//
//
// A loop on the resource's number to be deleted

    for (int i = 0; i < num_res; i++)
    {
//
// Try to delete the resource from database 
//
	if((errcode = db_del(rece->arr1_val[i])) != 0 )
	{
	    dbgen.connected = True;
	    std::cerr << "Could not delete resource " << rece->arr1_val[i] << std::endl;
	    return(&errcode);
    	}
    }
//
// Free memory and exit server 
//
    dbgen.connected = True;
    return(&errcode);
}

/****************************************************************************
*                                                                           *
*		Code for db_insert function                                 *
*                        ---------                                          *
*                                                                           *
*    Function rule : To add a resource from the database         	    *
*                                                                           *
*    Argin : - The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)	    *
*	     - The address where to store the string to memorize the deleted*
*	       resource value						    *
*  	     - The content of the resource				    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_insert(string res_name, string number, string content)
{
    string		domain,
    			family,
    			member,
    			r_name;
    string::size_type 	pos, 
			last_pos;
    int 		i;
//
// Get table name 
//
    if ((pos = res_name.find('/')) == string::npos)
    {
	std::cerr << "db_del : Error in resource name " << res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    domain = res_name.substr(0, pos);
//
// Get family name 
//
    if ((pos = res_name.find('/', (last_pos = pos + 1))) == string::npos)
    {
	std::cerr << "db_insert : Error in resource name " << res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    family = res_name.substr(last_pos, pos - last_pos);
//
// Get member name 
//
    if ((pos = res_name.find('/', (last_pos = pos + 1))) == string::npos)
    {
	std::cerr << "db_insert : Error in resource name " <<res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    member = res_name.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
    r_name = res_name.substr(pos + 1);

#ifdef NEVER
    std::cout << "Family name : " << family << std::endl;
    std::cout << "Number name : " << member << std::endl;
    std::cout << "Resource name : " << r_name << std::endl;
#endif
//
// Select the right resource table in database
//
    for (i = 0; i < dbgen.TblNum;i++)
	if (domain == dbgen.TblName[i])
	    break;
    if (i == dbgen.TblNum)
	return(DbErr_DomainDefinition);
//
    string query;
    if (mysql_db == "tango")
    {
        query = "INSERT INTO property_device(device,name,domain,family,member,count,value) VALUES('" + domain + "/" + family + "/" + member + "','" + r_name + "','"; 
	query += (domain + "','" + family +"','" + member + "','");
        query += (number + "','" + content + "')");
    }
    else
    {
        query = "INSERT INTO RESOURCE VALUES('" + domain + "','" + family + "','" + member + "','"; 
        query += (r_name + "','" + number + "','" + content + "')");
    }
#ifdef DEBUG
    cout << "MySQLServer::db_insert(): query = " << query;
#endif /* DEBUG */
    if (mysql_query(mysql_conn, query.c_str()))
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    if ((i = mysql_affected_rows(mysql_conn)) != 1)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    return 0;
}


/****************************************************************************
*                                                                           *
*		Code for db_del function                                    *
*                        ------                                             *
*                                                                           *
*    Function rule : To delete a resource from the database         	    *
*                                                                           *
*    Argin : - The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)	    *
*	     - The address where to store the string to memorize the deleted*
*	       resource value						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int MySQLServer::db_del(string res_name)
{
    string		t_name,
    			family,
    			member,
    			r_name;
    string::size_type 	pos, 
			last_pos;
    int			i;
//
// Get table name 
//
    if ((pos = res_name.find('/')) == string::npos)
    {
	std::cerr << "db_del : Error in resource name " << res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    t_name = res_name.substr(0, pos);
//
// Get family name 
//
    if ((pos = res_name.find('/', 1 + (last_pos = pos))) == string::npos)
    {
	std::cerr << "db_del : Error in resource name " << res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    family = res_name.substr(last_pos + 1, pos - last_pos - 1);
//
// Get member name 
//
    if ((pos = res_name.find('/', 1 + (last_pos = pos))) == string::npos)
    {
	std::cerr << "db_del : Error in resource name " <<res_name << std::endl;
	return(DbErr_BadResourceType);
    }
    member = res_name.substr(last_pos + 1, pos - last_pos - 1);
//
// Get resource name 
//
   last_pos = pos;
    r_name = res_name.substr(last_pos + 1);

#ifdef NEVER
    std::cout << "Family name : " << family << std::endl;
    std::cout << "Number name : " << member << std::endl;
    std::cout << "Resource name : " << r_name << std::endl;
#endif
//
// Select the right resource table in database
//
    for (i = 0; i < dbgen.TblNum;i++)
	if (t_name == dbgen.TblName[i])
	    break;
    if (i == dbgen.TblNum)
	return(DbErr_DomainDefinition);
//
// Try to retrieve the right tuple in table and loop for the case of an
// array of resource 
//
    string query;
    if (mysql_db == "tango")
    {
        query = "DELETE FROM property_device WHERE device = '" + t_name + "/" + family + "/" + member + "'";
	query += " AND name = '" + r_name + "'";
    }
    else
    {
        query = "DELETE FROM RESOURCE WHERE DOMAIN = " + t_name + " AND FAMILY = ";
        query += (family + " AND MEMBER = " + member + " NAME = " + r_name);
    }

#ifdef DEBUG
    cout << "db_del : query = " << query << endl;
#endif /* DEBUG */

    if (mysql_query(mysql_conn, query.c_str()))
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    if ((i = mysql_affected_rows(mysql_conn)) == -1)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	return (DbErr_DatabaseAccess);
    }
    return(0);
}

