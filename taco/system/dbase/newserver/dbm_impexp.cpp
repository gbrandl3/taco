#include <DevErrors.h>
#include <NdbmServer.h>
#ifdef HAVE_SSTREAM
#	include <sstream>
#else
#	include <strstream>
#	define	stringstream	strstream
#endif

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
DevLong *NdbmServer::db_devexp_1_svc(tab_dbdev *rece)
{
    int 	db_err;
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
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database
//
    for (int i = 0; i < num_dev; i++)
	if ((db_err = db_store(rece->tab_dbdev_val[i])) != 0)
	{
	    errcode = db_err;
	    break;
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
DevLong  *NdbmServer::db_devexp_2_svc(tab_dbdev_2 *rece)
{
    int 	db_err;
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
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database */
//
    for (int i = 0; i < num_dev; i++)
    {
	if ((db_err = db_store_2(rece->tab_dbdev_val[i])) != 0)
	{
	    errcode = db_err;
	    break;
	}
    }
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
DevLong *NdbmServer::db_devexp_3_svc(tab_dbdev_3 *rece)
{
    int 	db_err;
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

    if (!dbgen.connected)
    {
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Store values in database */
//
    for (int i = 0; i < num_dev; i++)
    {
	if ((db_err = db_store_3(rece->tab_dbdev_val[i])) != 0)
	{
	    errcode = db_err;
	    break;
	}
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
db_resimp *NdbmServer::db_devimp_1_svc(arr1 *de_name)
{
    int 			num_dev = de_name->arr1_len;
    datum 			key,
				content;
//    device 			ret;
    register db_devinfo 	*stu_addr,
				*stu_addr1;	
    string 			ret_dev_name,
				ret_host_name,
				ret_dev_type,
				ret_dev_class;
    int 			ret_pn,
				ret_vn;
#ifdef DEBUG
    for (int i = 0; i < num_dev; i++)
	cout << "Device name (in import function) : " << de_name->arr1_val[i] << endl;
#endif
//
// Initialize error code sended back to client 
//
	back.db_imperr = 0;
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	back.db_imperr = DbErr_DatabaseNotConnected;
	back.imp_dev.tab_dbdev_len = 0;
	back.imp_dev.tab_dbdev_val = NULL;
	return(&back);
    }
//
// Allocate memory for the array of db_devinfo structures
//
    try
    {
	back.imp_dev.tab_dbdev_val = new db_devinfo[num_dev];
    }
    catch(const bad_alloc &)
    {
	back.db_imperr = DbErr_ServerMemoryAllocation;
	back.imp_dev.tab_dbdev_len = 0;
	return(&back);
    }
//
// A loop on every device to import
//
    for (int i = 0; i < num_dev; i++)
    {
//
// Try to retrieve the tuple in the NAMES table 
//
	key = gdbm_firstkey(dbgen.tid[0]);
	if (!key.dptr)
	{
	    delete [] back.imp_dev.tab_dbdev_val;
	    back.db_imperr = DbErr_DatabaseAccess;
	    back.imp_dev.tab_dbdev_len = 0;
	    return(&back);
	}

	do
	{
	    content = gdbm_fetch(dbgen.tid[0], key);
	    if (content.dptr)
	    {
		string 			temp = content.dptr;
		string::size_type	pos = temp.find('|');
		if (pos != string::npos)
		{
		    ret_dev_name = temp.substr(0, pos);
		    if (ret_dev_name == de_name->arr1_val[i])
		    {
//
// Unpack the content 
//
			temp.erase(0, pos + 1);
			pos = temp.find('|');
			ret_host_name = temp.substr(0, pos);
			temp.erase(0, pos + 1);

			pos = temp.find('|');
			ret_pn = atoi(temp.substr(0, pos).c_str());
			temp.erase(0, pos + 1);

			pos = temp.find('|');
			ret_vn = atoi(temp.substr(0, pos).c_str());
			temp.erase(0, pos + 1);

			pos = temp.find('|');
			ret_dev_type = temp.substr(0, pos);
			temp.erase(0, pos + 1);

			pos = temp.find('|');
			if (pos == string::npos) 
			    ret_dev_class = temp;
			else
			    ret_dev_class = temp.substr(0, pos);
			break;
		    } 
		} 
	    } 
	}while (key.dptr);
//
// In case of error 
//
	if (!content.dptr || !key.dptr || !ret_pn)
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
	    if (!key.dptr)
		back.db_imperr = DbErr_DeviceNotDefined;
	    else if (!content.dptr)
		back.db_imperr = DbErr_DatabaseAccess;
	    else
		back.db_imperr = DbErr_DeviceNotExported;
	    return(&back);
	}
	stu_addr1 = &(back.imp_dev.tab_dbdev_val[i]);
	try
	{
//
// Allocate memory for the host_name string 
//
	    stu_addr1->host_name = new char[ret_host_name.length() + 1];
//
// Allocate memory for the device name string 
//
	    stu_addr1->dev_name = new char[strlen(de_name->arr1_val[i]) + 1];
//
// Allocate memory for the device type string 
//
	    stu_addr1->dev_type = new char[ret_dev_type.length() + 1];
//
// Allocate memory for the device class string 
//
	    stu_addr1->dev_class = new char[ret_dev_class.length() + 1];
	}
	catch(const bad_alloc &)
	{
	    for (int j = 0; j <= i; j++)
	    {
		stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
		delete [] stu_addr->dev_name;
		delete [] stu_addr->host_name;
		delete [] stu_addr->dev_class;
		delete [] stu_addr->dev_type;
	    }
	    back.imp_dev.tab_dbdev_len = 0;
	    back.db_imperr = DbErr_ServerMemoryAllocation;
	    return(&back);
	}
//
// Initialize structure sended back to client 
//
	strcpy(stu_addr1->host_name, ret_host_name.c_str());
	strcpy(stu_addr1->dev_name, de_name->arr1_val[i]);
	stu_addr1->p_num = ret_pn;
	stu_addr1->v_num = ret_vn;
	strcpy(stu_addr1->dev_class, ret_dev_class.c_str());
	strcpy(stu_addr1->dev_type, ret_dev_type.c_str());

    } // end of for for devices 

    back.imp_dev.tab_dbdev_len = num_dev;
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
DevLong *NdbmServer::db_svcunr_1_svc(nam *dsn_name)
{
//    device 	dev;
    datum 	key,
		content, 
		dev1;
    string 	dev_str,
		proc_str,
		pers_str,
		dev_class,
		key_str;
    int 	d_num = 0,
   		dev_numb = 1,
		old_d_num;

#ifdef DEBUG
	cout << "Device server name (unregister function) : " << *dsn_name << endl;
#endif
//
// Miscallaneous initialization
//
    errcode = 0;
    string	sto(*dsn_name);
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	errcode = DbErr_DatabaseNotConnected;
	return(&errcode);
    }
//
// Get device server class */
//
    string::size_type	pos = sto.find('/');
    string	ds_class = sto.substr(0, pos);
//
// Get device server name */
//
    string	ds_name  = sto.substr(pos + 1);

#ifdef DEBUG
    cout << "Device server class (unreg) : " << ds_class << endl;
    cout << "Device server name (unreg) : " << ds_name << endl;
#endif
//
// Try to retrieve devices in database assuming that the input device server
// name is the device server PROCESS name. As there is no key build on
// the device server process name, do a full traversal of the database 
//
    do
    {
	old_d_num = d_num;
	for (key = gdbm_firstkey(dbgen.tid[0]); key.dptr != NULL; key = gdbm_nextkey(dbgen.tid[0], key))
	{
//
// Extract personal name and sequence field from key 
//
	    string	temp = key.dptr;
	    pos = temp.find('|');
	    temp.erase(0, pos + 1);
	    pos = temp.find('|');
	    pers_str = temp.substr(0, pos);
	    if (ds_name != pers_str)
		continue;

	    string key_str(key.dptr, key.dsize);
//
// Get db content 
//
	    content = gdbm_fetch(dbgen.tid[0], key);
	    if (!content.dptr)
		if (gdbm_error(dbgen.tid[0]) != 0)
		{
		    errcode = DbErr_DatabaseAccess;
		    return(&errcode);
		}
	    temp = content.dptr;
	    pos = temp.find('|');
	    pos = temp.find('|', pos + 1);
	    dev_str = temp.substr(0, pos);
//
// Extract program number 
//
	    temp.erase(0, pos + 1);
	    pos = temp.find('|');
	    if (temp.substr(0, pos) == "0")
		continue;
//
// Extract device class 
//
	    for (int i = 0; i < 2; i++)
	    	pos = temp.find('|', pos + 1);
	    temp.erase(0, pos + 1);
	    pos = temp.find('|');
	    dev_class = temp.substr(0, pos);
//
// Extract process name 
//
	    for (int i = 0; i < 2; i++)
	    	pos = temp.find('|', pos + 1);
	    temp.erase(0, pos + 1);
	    pos = temp.find('|');
	    proc_str = temp.substr(0, pos);
	    if (ds_class != proc_str)
		continue;
//
// A device to be unregistered has been found, build the new database content 
//
	    dev_str += ("|0|0|unknown|" + dev_class + "|0|" + proc_str + "|");
//
// Update database 
//
	    dev1.dptr = const_cast<char *>(dev_str.data());
	    dev1.dsize = dev_str.length();
	    key.dptr = const_cast<char *>(key_str.data());
	    key.dsize = key_str.length();

	    if (gdbm_store(dbgen.tid[0], key, dev1, GDBM_REPLACE))
	    {
		errcode = DbErr_DatabaseAccess;
		return(&errcode);
	    }
	    d_num++;
	    break;
	}
	if (old_d_num == d_num)
	    break;
    }
    while (true);
//
// Initialization needed to retrieve the right tuples in the NAMES table
// and to update the tuples (program and version number) assuming the input
// name is a device server name
//
    if (d_num == 0)
    {
	do
	{
	    stringstream	s;
#if !HAVE_SSTREAM
            s.seekp(0, ios::beg);
#endif
	    s << ds_class << "|" << ds_name << "|" << dev_numb << "|" << ends;
#if !HAVE_SSTREAM
            strcpy(key.dptr, s.str());
            s.freeze(false);
#else
            strcpy(key.dptr, s.str().c_str());
#endif
	    key.dsize = strlen(key.dptr);
//
// Try to retrieve the tuples 
//
	    content = gdbm_fetch(dbgen.tid[0], key); 
	    if (content.dptr)
	    {
		d_num++;
//
// Extract device class 
//
		string	temp = content.dptr;
	        pos = temp.find('|');
	        pos = temp.find('|', pos + 1);
		dev_str = temp.substr(0, pos);
	     	for (int i = 0; i < 3; i++)
	    	    pos = temp.find('|', pos + 1);
	    	temp.erase(0, pos + 1);
	        pos = temp.find('|');
		dev_class = temp.substr(0, pos);
//
// Build the new database content 
//
		temp.erase(0, pos + 1);
	     	for (int i = 0; i < 2; i++)
	    	    pos = temp.find('|', pos + 1);
	    	temp.erase(0, pos + 1);
	        pos = temp.find('|');
		proc_str = temp.substr(0, pos);
	        dev_str += ("|0|0|unknown|" + dev_class + "|0|" + proc_str + "|");
//
// Update database 
//
		dev1.dptr = const_cast<char *>(dev_str.data());
		dev1.dsize = dev_str.length();

		if (gdbm_store(dbgen.tid[0], key, dev1, GDBM_REPLACE))
		{
		    errcode = DbErr_DatabaseAccess;
		    return(&errcode);
		}
		dev_numb++;
	    }
	    else
		break;
	} 
	while (true);
    }
//
// In case of trouble 
//
    if ((content.dptr == NULL) && (d_num == 0))
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
svc_inf *NdbmServer::db_svcchk_1_svc(nam *dsn_nam)
{
    datum 	key,
    		content;
    string	ret_host_name,
    		ret_dev_name,
    		ret_dev_type,
    		ret_dev_class;
    string	sto(*dsn_nam);
#ifdef DEBUG
    cout << "Device server name (check function) : " << sto << endl;
#endif /* DEBUG */
//
// Miscalaneous initialization 
//
    svc_info.p_num = 1;
    svc_info.v_num = 1;
    svc_info.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	svc_info.db_err = DbErr_DatabaseNotConnected;
	return(&svc_info);
    }
//
// Get device server class
//
    string::size_type	pos = sto.find('/');
    string	ds_class = sto.substr(0, pos);
//
// Get device server name 
//
    string	ds_name = sto.substr(pos + 1);

#ifdef DEBUG
    cout << "Device server class (check) : " << ds_class << endl;
    cout << "Device server name (check) : " << ds_name << endl;
#endif
//
// Initialization needed to retrieve the right tuples in the NAMES table 
//
    string s = ds_class + "|" + ds_name + "|1|"; 
    key.dptr = const_cast<char *>(s.data());
    key.dsize = s.length();
//
// Try to retrieve the tuples 
//
    content = gdbm_fetch(dbgen.tid[0], key);                                

    if (!content.dptr)
    {
	string	temp = content.dptr;
	pos = temp.find('|');
	ret_dev_name = temp.substr(0, pos);
	temp.erase(0, pos + 1);		

	pos = temp.find('|');
	ret_host_name = temp.substr(0, pos);
	temp.erase(0, pos + 1);		

	pos = temp.find('|');
	svc_info.p_num = atoi(temp.substr(0, pos).c_str());
	temp.erase(0, pos + 1);		

	pos = temp.find('|');
	svc_info.v_num = atoi(temp.substr(0, pos).c_str());
	temp.erase(0, pos + 1);		

	pos = temp.find('|');
	ret_dev_type = temp.substr(0, pos);
	temp.erase(0, pos + 1);		

	if ((pos = temp.find('|')) == string::npos)
	    ret_dev_class = temp;
	else
	    ret_dev_class = temp.substr(0, pos);
        try
	{
    	    svc_info.ho_name = new char[ret_host_name.length() + 1];
	    strcpy(svc_info.ho_name, ret_host_name.c_str());
	}
	catch(const bad_alloc &)
	{
	    svc_info.db_err = DbErr_ServerMemoryAllocation;
	}
    }
    else
	svc_info.db_err = DbErr_DeviceServerNotDefined;
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
int NdbmServer::db_store(db_devinfo &dev_stu)
{
    datum 	key, 
		key_sto,
		content, 
		cont_sto;
    int		errcode = 0;

    try
    {
//
// Try to retrieve the right tuple in the NAMES table
//
	key_sto.dptr = new char[MAX_KEY];
	cont_sto.dptr = new char[MAX_CONT];

    	key = gdbm_firstkey(dbgen.tid[0]);
    	if (!key.dptr)
	    throw int(DbErr_DatabaseAccess);
    	do
    	{
            strncpy(key_sto.dptr, key.dptr, key.dsize);
            key_sto.dptr[key.dsize] = '\0';
            key_sto.dsize = key.dsize;
	    content = gdbm_fetch(dbgen.tid[0], key);
	    if (content.dptr)
	    {
	    	string temp = content.dptr;
	    	string::size_type	pos = temp.find('|');
	    	if (pos != string::npos)
		    if (temp.substr(0, pos) ==  dev_stu.dev_name)
		    	break;
	    } 
	    key = gdbm_nextkey(dbgen.tid[0], key);
    	} while(key.dptr); 
//
// Different result cases 
//
    	if (!key.dptr || !key_sto.dptr)
	    throw int(DbErr_DeviceNotDefined);
//
// Test to see if host name in database is the same.
// Finally, update the tuple in database
//
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num << "|"
	  <<  dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class 
	  << "|0|unknown" << ends;
#if !HAVE_SSTREAM
        strcpy(cont_sto.dptr, s.str());
        s.freeze(false);
#else
        strcpy(cont_sto.dptr, s.str().c_str());
#endif
	cont_sto.dsize = strlen(cont_sto.dptr);
	if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, GDBM_REPLACE))
	    throw int(DbErr_DatabaseAccess);
    } 
    catch(const int err)
    {
        errcode = err;
    }
    catch(const bad_alloc &)
    {
	errcode = int(DbErr_ServerMemoryAllocation);
    }
    delete [] cont_sto.dptr;
    delete [] key_sto.dptr;
    return(errcode);
}

/****************************************************************************
*                                                                           *
*		Code for db_store_2 function                                *
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
int NdbmServer::db_store_2(db_devinfo_2 &dev_stu)
{
    datum 		key, 
			key_sto,
			content, 
			cont_sto;
    int			errcode = 0;

//
// Try to retrieve the right tuple in the NAMES table */
//
    try
    {
	key_sto.dptr = new char[MAX_KEY];
	cont_sto.dptr = new char[MAX_CONT];
	
	key = gdbm_firstkey(dbgen.tid[0]);
	if (!key.dptr)
	    throw int(DbErr_DatabaseAccess);

	do
	{
	    strncpy(key_sto.dptr, key.dptr, key.dsize);
	    key_sto.dptr[key.dsize] = '\0';
	    key_sto.dsize = key.dsize;
	    content = gdbm_fetch(dbgen.tid[0], key);

	    if (content.dptr)
	    {
		string 		temp = content.dptr;
		string::size_type	pos = temp.find('|');
		if (pos != string::npos)
		    if (temp.substr(0, pos) == dev_stu.dev_name)
			break;
	    } 
	    key = gdbm_nextkey(dbgen.tid[0], key);
	} while (key.dptr );
//
// Different result cases 
//
	if (!key.dptr || !key_sto.dptr)
	    throw int(DbErr_DeviceNotDefined);
//
// Test to see if host name in database is the same.
// Finally, update the tuple in database 
//
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num <<  "|"
	  << dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class << "|" 
	  << dev_stu.pid << "|unknown|" << ends;
#if !HAVE_SSTREAM
        strcpy(cont_sto.dptr, s.str());
        s.freeze(false);
#else
        strcpy(cont_sto.dptr, s.str().c_str());
#endif
	cont_sto.dsize = strlen(cont_sto.dptr);
	if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, GDBM_REPLACE))
	    throw int(DbErr_DatabaseAccess);
    } 
    catch(const int err)
    {
        errcode = err;
    }
    catch(const bad_alloc &)
    {
	errcode = int(DbErr_ServerMemoryAllocation);
    }
    delete [] cont_sto.dptr;
    delete [] key_sto.dptr;
    return(errcode);
}

/****************************************************************************
*                                                                           *
*		Code for db_store_3 function                                *
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
int NdbmServer::db_store_3(db_devinfo_3 &dev_stu)
{
    datum 	key, 
		key_sto,
		content, 
		cont_sto;

    try
    {
//
// Try to retrieve the right tuple in the NAMES table 
//
	key_sto.dptr = new char[MAX_KEY];
	cont_sto.dptr = new char[MAX_CONT];
	
	key = gdbm_firstkey(dbgen.tid[0]);
	if (!key.dptr)
	    throw int(DbErr_DatabaseAccess);

	do
	{
	    strncpy(key_sto.dptr, key.dptr, key.dsize);
	    key_sto.dptr[key.dsize] = '\0';
	    key_sto.dsize = key.dsize;
	    content = gdbm_fetch(dbgen.tid[0], key);
	    if (content.dptr)
	    {
		string 		temp = content.dptr;
		string::size_type	pos = temp.find('|');
		if (pos != string::npos)
		    if (temp.substr(0, pos) == dev_stu.dev_name)
			break;
	    }
	    key = gdbm_nextkey(dbgen.tid[0], key);
	} while (key.dptr);
//
// Different result cases 
//
	if (!key.dptr || !key_sto.dptr)
	    throw int(DbErr_DeviceNotDefined);
//
// Finally, update the tuple in database 
//
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num <<  "|"
	  << dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class << "|" 
	  << dev_stu.pid << "|" << dev_stu.proc_name << "|" << ends;
#if !HAVE_SSTREAM
        strcpy(cont_sto.dptr, s.str());
        s.freeze(false);
#else
        strcpy(cont_sto.dptr, s.str().c_str());
#endif
	cont_sto.dsize = strlen(cont_sto.dptr);
	if (gdbm_store(dbgen.tid[0], key_sto, cont_sto, GDBM_REPLACE))
	    throw int(DbErr_DatabaseAccess);
    }
    catch(const int err)
    {
        errcode = err;
    }
    catch(const bad_alloc &)
    {
	errcode = int(DbErr_ServerMemoryAllocation);
    }

    delete [] (cont_sto.dptr);
    delete [] (key_sto.dptr);
    return(errcode);
}
