#include <DevErrors.h>
#include <algorithm>
#include <NdbmServer.h>

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
db_psdev_error *NdbmServer::upddev_1_svc(db_res *dev_list)
{
    long list_nb = dev_list->res_val.arr1_len;
		
#ifdef DEBUG
    cout << "In upddev_1_svc function for " << list_nb << " device list(s)" << endl;
#endif
//
// Initialize parameter sent back to client */
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// A loop on each device list */
//
    for (long i = 0; i < list_nb; i++)
    {
//		
// Allocate memory for strtok pointers
//
	string	lin(dev_list->res_val.arr1_val[i]);
#ifdef DEBUG
	cout << "Device list = " << lin << endl;
#endif 
//
// Find the last device in the list. If there is no , character in the line,
// this means that there is only one device in the list 
//
	string::size_type	pos = lin.rfind(",");
	int			last(False);
	if (pos == string::npos)
	{
	    pos = lin.find(":");
	    last = True;
	}
	string	last_dev = lin.substr(pos + 1);
//
// Extract each device from the list and update table each time 
//
	int	ind(1);
	do
	{
	    pos = lin.find(",");
	    string	dev = (pos == string::npos) ? lin : lin.substr(0, pos);
	    try
	    {
	    	upd_name(dev, lin, ind, last);
	    }
	    catch(const long err)
	    {
	    	psdev_back.psdev_err = i + 1;
		psdev_back.error_code = err;
	    	return(&psdev_back);
	    }
	    lin.erase(0, pos + 1);
	    last = (dev == last_dev);
	    ind++;	
	}while(pos != string::npos);
    }
//
// return data
//
    return(&psdev_back);
}


/****************************************************************************
*                                                                           *
*		Code for upd_name function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To update the NAMES table in the database              *
*                                                                           *
*    Argin : - lin : The device name. The first time this function is called*
*		     for a device list, this buffer also contains the DS    *
*		     name and personal name				    *
*	     - ptr : The complete device list				    *
*	     - ind : The device indice list in the list (starting with 1)   *
*	     - last : A flag set to true if the device is the last one in   *
*		      the list						    *
*                                                                           *
*    Argout : p_err : Pointer for error code                                *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long NdbmServer::upd_name(string lin, string ptr,int ind, long last) throw (long)
{
    static vector<dena>	tab_dena;
    device 		dev; 

    string::size_type	pos,
			last_pos = 0;
    char 		prgnr[20],
    			seqnr[4];
    DBM 		*tup;
    static datum 	key, 
			key_sto, 
			key_sto2, 
			key_2,
			content, 
			cont_sto;
    static int 		ndev;
    static long	 	key_sto_base_length;

//
// Allocate memory for the dbm-structures (key and content)
//
    if (ind == 1)
    {
	try
	{
	    key_sto.dptr = new char[MAX_KEY];
	    key_2.dptr = new char[MAX_KEY];	
	    content.dptr = new char[MAX_CONT];
	}
	catch(bad_alloc)
	{
	    delete [] key_sto.dptr;
	    delete [] key_2.dptr;
	    delete [] content.dptr;
	    throw long(DbErr_ServerMemoryAllocation);
	}
	if((pos = lin.find(':')) == string::npos)
	{
	    cerr << "upd_name : no ':' found" << endl;
	    throw long();
	}
	if (count(lin.begin(), lin.begin() + pos, '/') != 2)
	{
	    cerr << "upd_name : 2 '/' in the name expected, " << lin.substr(0, pos) << endl;
	    throw long();
	}
//
// Get device server class
//
	pos = lin.find('/');
	strcpy(dev.ds_class, lin.substr(last_pos, pos - last_pos).c_str());
	last_pos = pos;
	strcpy(key_sto.dptr, dev.ds_class);
	strcat(key_sto.dptr, "|");
//
// Get device server name 
//
	pos = lin.find('/', last_pos + 1);
	strcpy(dev.ds_name, lin.substr(last_pos, pos - last_pos).c_str());
	last_pos = pos;
	strcat(key_sto.dptr,dev.ds_name);
	strcat(key_sto.dptr, "|");
	key_sto.dsize = strlen(key_sto.dptr);
	key_sto_base_length = key_sto.dsize;
//
// Get device name
//
	pos = lin.find(':');
	strcpy(dev.d_name, lin.substr(pos + 1).c_str());
	strcpy(content.dptr, lin.substr(pos + 1).c_str());
	strcat(content.dptr, "|");
    }
    else
    {
	strcpy(dev.d_name, lin.c_str());
	strcpy(content.dptr, dev.d_name);
	strcat(content.dptr, "|");
    }
//
// Allocate memory for the dena structures array 
//
    if (ind == 1)
    {
	tab_dena.clear();
#ifdef DEBUG
	cout << "Memory allocated for the dena structures array" << endl;
#endif 
//
// Call the del_name function
//
	try
	{
	    del_name(dev, ndev, ptr, tab_dena);
	}
	catch(...)
	{
	    delete [] (content.dptr);
	    delete [] (key_2.dptr);
	    delete [] (key_sto.dptr);
	    return(-1);
	}
#ifdef DEBUG
	cout << "Device server class : " << dev.ds_class << endl;
	cout << "Device server name : " << dev.ds_name << endl;
	cout << "Device name : " << dev.d_name << endl;
	cout << "Device number (in device list) : " << ind << endl;
	cout << "Returned from the del_name function" << endl;
#endif /* DEBUG */
    }

//
// Check, if the only device server is to be removed  
//
    if (dev.d_name != string("%"))
    {
//
// Initialize the new tuple with the right pn and vn values 
//
    	int 		i;
	stringstream	s;
	for (i = 0;i < ndev && strcmp(dev.d_name,tab_dena[i].devina);i++);
	if (i == ndev)
	{
//
// Initialize the content for dbm-database 
//
	    strcat(content.dptr, "not_exp|0|0|unknown|unknown|0|unknown|");
	    content.dsize = strlen(content.dptr);
	}
	else
	{
	    dev.pn = tab_dena[i].opn;
	    dev.vn = tab_dena[i].ovn;
	    dev.pid = tab_dena[i].opid;
	    strcpy(dev.h_name,tab_dena[i].oh_name);
	    strcpy(dev.d_class,tab_dena[i].od_class);
	    strcpy(dev.d_type,tab_dena[i].od_type);
	    strcpy(dev.proc_name,tab_dena[i].od_proc);
//
// Initialize the content for dbm-database 
//
#if !HAVE_SSTREAM
             s.seekp(0, ios::beg);
#endif
	    s << tab_dena[i].oh_name << "|" << tab_dena[i].opn << "|" << tab_dena[i].ovn << "|" << tab_dena[i].od_class << "|"
	      << tab_dena[i].od_type << "|" << tab_dena[i].opid << "|" << tab_dena[i].od_proc << "|";
#if !HAVE_SSTREAM
            content.dptr = s.str();
	    content.dsize = strlen(s.str());
            s.freeze(false);
#else
	    content.dptr = const_cast<char *>(s.str().data());
	    content.dsize = s.str().size();
#endif
	}
	dev.indi = ind;
	sprintf(seqnr, "%d", ind);
	key_sto.dptr[key_sto_base_length] = '\0';
	strcat(key_sto.dptr, seqnr);
	strcat(key_sto.dptr, "|");
	key_sto.dsize = strlen(key_sto.dptr);
//
// Insert tuple in NAMES table
//
#ifdef DEBUG
	cout << "Insert tuple in NAMES table" << endl;
#endif 
	key_sto2 = key_sto;
	cont_sto = content;

	if ((i = dbm_store(dbgen.tid[0], key_sto2, cont_sto, DBM_INSERT)) != 0)
	{
	    delete [] content.dptr;
	    delete [] key_2.dptr;
	    delete [] key_sto.dptr;
	    tab_dena.clear();
	    if (i == 1) 
		throw long(DbErr_DoubleTupleInNames);
	    else
		throw long(DbErr_DatabaseAccess);
	    return(-1);
	}
    }
//
// Free memory if it is the last device in the list 
//
    if (last)
    {
	delete [] content.dptr;
	delete [] key_2.dptr;
	delete [] key_sto.dptr;
	tab_dena.clear();
    }
//
// Leave function 
//
    return(0);
}



/****************************************************************************
*                                                                           *
*		Code for del_name function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To delete (in the database) all the devices for a      *
*                    specific device server.                                *
*                                                                           *
*    Argin : - The device server network name                               *
*	     - The address of a buffer where is saved all the device's names*
*              in charge of a device server.                                *
*              The string follows this format :                             *
*              D.S. network name:device name 1,device name 2,...            *
*                                                                           *
*    Argout : - The number of device that have been deleted                 *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
long NdbmServer::del_name(device &devi, int &pndev, string ptr, vector<dena> &buf) throw (long)
{
    int 	j,
		l,
		tp;
    datum 	key,
		content;
//    device 	ret;
    int 	seq = 1;
    long 	nb_dev;
//
// Remove all the devices already registered in the database with the same
// device server/personal name 
// 
    int 	i = 0;
    do
    {
//
// Build key 
//
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << devi.ds_class << "|" << devi.ds_name << "|" << seq << "|" << ends;
#if !HAVE_SSTREAM
        key.dptr = s.str();
	key.dsize = strlen(s.str());
        s.freeze(false);
#else
	key.dptr = const_cast<char *>(s.str().data());
	key.dsize = s.str().size();
#endif
//
// Try to get data out of database
//
	content = dbm_fetch(dbgen.tid[0],key);
	if (!content.dptr)
	{
	    if (dbm_error(dbgen.tid[0]) == 0)
		break;
	    else
		throw long(DbErr_DatabaseAccess);
	}
	else
	{
//
// Copy all the database information in a "dena" structure 
//
	    string 		temp(content.dptr, content.dsize);
	    string::size_type 	pos = temp.find('|');
	    dena		_dena;

	    strcpy(_dena.devina, temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    strcpy(_dena.oh_name, temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    _dena.opn = atoi(temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    _dena.ovn = atoi(temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    strcpy(_dena.od_type,  temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    strcpy(_dena.od_class, temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    _dena.opid = atoi(temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    strcpy(_dena.od_proc, temp.substr(0, pos).c_str());
	    i++;
	    buf.push_back(_dena);
//
// Delete database entry
//
	    dbm_delete(dbgen.tid[0], key);
	    seq++;
	}
    }while (true);
//
// Compute how many devices are defined in the list
//
    nb_dev = count(ptr.begin(), ptr.end(), ',') + 1;
//
// Allocate memory for the structure array
//
    db_dev_in_db 	*ptr_dev; 
    try
    {
	ptr_dev = new db_dev_in_db[nb_dev];
//
// Init structure array 
//
    	string ptr1 = ptr;
	string::size_type	pos = ptr1.find(',');
    	j = 0;	
    	do
    	{
	    strcpy(ptr_dev[j].dev_name, ptr1.substr(0, pos).c_str());
	    ptr1.erase(0, pos + 1);
	    j++;
    	}while((pos = ptr1.find(',')) != string::npos);
	strcpy(ptr_dev[j].dev_name, ptr1.c_str());
//
// In case of device in the device list which was not previously member of
// this device server, try to retrieve a tuple in the database for each device
// of the list
// Delete remaining device(s) from db
//
    	if (!is_dev_in_db(ptr_dev, nb_dev))
	    for (long j = 0;j < nb_dev;j++)
	    	if (ptr_dev[j].found)
	    	{
		    key.dptr = ptr_dev[j].key_buf;
		    key.dsize = strlen(ptr_dev[j].key_buf);
		    dbm_delete(dbgen.tid[0], key);
		    update_dev_list(ptr_dev[j].dev_info, ptr_dev[j].seq + 1);
	    	}
    }
    catch(const bad_alloc &)
    {
	throw long(DbErr_ServerMemoryAllocation);
    }
    catch(const long err)
    {
	delete [] ptr_dev;
	throw long(DbErr_DatabaseAccess);
    }
//
// Leave function 
//
    delete [] ptr_dev;
    pndev = i;
    return(0);
}

/****************************************************************************
*                                                                           *
*		Code for is_dev_in_db function                              *
*                        ------------                                       *
*                                                                           *
*    Function rule : To check if a device is known in the database	    *
*                                                                           *
*    Argin : - devname : The device name				    *
*                                                                           *
*    Argout : - p_dev : Pointer to a device structure with fields set if the*
*			device is defined in the database		    *
*	      - p_seq : The device sequence database entry field	    *
*	      - p_key : The database entry key				    *
*                                                                           *
*    This function returns 0 if there is a device in the datbase	    *
*    If the device is not defined in the database, the function returns     *
*    ERR_DEV_NOT_FOUND. In case of failure, the function returns ERR_DEVNAME*
*                                                                           *
****************************************************************************/
long NdbmServer::is_dev_in_db(db_dev_in_db *ptr,long nb_dev) throw (long)
{
    device 		dev;
	
    for (datum key = dbm_firstkey(dbgen.tid[0]); key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
    {
	string	temp(key.dptr, key.dsize);
//
// Extract dserver name
//
	string::size_type	pos = temp.find('|');
	if (pos == string::npos)
	    throw long(ERR_DEVNAME);
	strcpy(dev.ds_class, temp.substr(0, pos).c_str());
	temp.erase(0, pos + 1);
//
// Extract personal name
//
	pos = temp.find('|');
	if (pos == string::npos)
	    throw long(ERR_DEVNAME);
	strcpy(dev.ds_name, temp.substr(0, pos).c_str());
	temp.erase(0, pos + 1);
//
// Extract sequence field
//
	pos = temp.find('|');
	if (pos == string::npos)
	    throw long(ERR_DEVNAME);
	string	seq_str = temp.substr(0, pos);
//
// Make a copy of the key
//
	string	key_buf(key.dptr, key.dsize);
//
// Get db content 
//
	datum content = dbm_fetch(dbgen.tid[0], key);
	if (content.dptr != NULL)
	{
	    temp = string(content.dptr, content.dsize);
	    pos = temp.find('|');
	    if (pos == string::npos)
	    	throw long(ERR_DEVNAME);
	    strcpy(dev.d_name, temp.substr(0, pos).c_str());
//
// Wanted device ? 
//
	    for (long j = 0;j < nb_dev;j++)
		if (dev.d_name == ptr[j].dev_name)
		{
		    ptr[j].found = true;
		    ptr[j].seq = atoi(seq_str.c_str());
		    strcpy(ptr[j].key_buf, key_buf.c_str());
		    ptr[j].dev_info = dev;
		}
	}
    }
    return(0);
}

/****************************************************************************
*                                                                           *
*		Code for update_dev_list function                           *
*                        ---------------                                    *
*                                                                           *
*    Function rule : To update a device server device list when one of the  *
*		     device is removed from the list. This means to update  *
*		     the sequence field of all the remaining devices	    *
*                                                                           *
*    Argin : - p_ret : The removed device structure with the device         *
*		       information					    *
*	     - seq : The sequence field of the first device to be updated   *
*                                                                           *
*    Argout : No argout							    *
*                                                                           *
*    This function returns 0 if there is no problem. Otherwise, it returns  *
*    ERR_DEVNAME							    *
*                                                                           *
*****************************************************************************/
long NdbmServer::update_dev_list(device &p_ret, int seq) throw (long)
{
    datum 	key;
//
// Miscellaneous init 
//
    string key_buf = string(p_ret.ds_class) + "|" + string(p_ret.ds_name) + "|";
#ifdef DEBUG
    cout << "before loop in update-dev_list function" << endl;
#endif
    do
    {
//
// Build the key 
//
#if !HAVE_SSTREAM
	stringstream	s(const_cast<char *>(key_buf.c_str()), key_buf.length());
        s.seekp(0, ios::beg);
#else
	stringstream	s(key_buf);
#endif
	s << seq << "|" <<  ends;
#if !HAVE_SSTREAM
        key.dptr = s.str();
	key.dsize = strlen(s.str());
        s.freeze(false);
#else
	key.dptr = const_cast<char *>(s.str().data());
	key.dsize = s.str().size();
        strcpy(key.dptr, s.str().c_str());
#endif
//
// Tried to get data from the database 
//
	datum content = dbm_fetch(dbgen.tid[0],key);
	if (content.dptr == NULL)
	{
	    if (dbm_error(dbgen.tid[0]) == 0)
		break;
	    else
	   	throw long(ERR_DEVNAME);
	}
	else
	{
//
// Copy the old database content 
//
	    string cont_sto(content.dptr, content.dsize);
//
// Delete the entry and store a new one with a modifed sequence field 
//
	    if (dbm_delete(dbgen.tid[0], key) != 0)
	   	throw long(ERR_DEVNAME);
#if !HAVE_SSTREAM
            s.seekp(0, ios::beg);
	    s << key_buf;
#else
	    s.str(key_buf);
#endif
	    s << (seq - 1) << "|" << ends;;
#if !HAVE_SSTREAM
            key.dptr = s.str();
	    key.dsize = strlen(s.str());
            s.freeze(false);
#else
	    key.dptr = const_cast<char *>(s.str().data());
	    key.dsize = s.str().size();
#endif
	    content.dptr = const_cast<char *>(cont_sto.data());
	    content.dsize = cont_sto.size();

	    if (dbm_store(dbgen.tid[0], key, content, DBM_INSERT) != 0)
	   	throw long(ERR_DEVNAME);
	    seq++;
	}
    }while(true);
#ifdef DEBUG
    cout << "after loop in update-dev_list function" << endl;	
#endif
    return(0);
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
db_psdev_error *NdbmServer::updres_1_svc(db_res *res_list)
{
    long 	list_nb = res_list->res_val.arr1_len,
		ind = 1,
		i;

#ifdef DEBUG
    cout << "In updres_1_svc function for " << list_nb << " resource(s)" << endl;
#endif
//
// Initialize parameter sent back to client */
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;
//
// A loop on each resources */
//
    try
    {
    	for (i = 0; i < list_nb; i++)
    	{
//
// Allocate memory for strtok pointers 
//
	    string lin = res_list->res_val.arr1_val[i];
	    string::size_type	pos = 0;

#ifdef DEBUG
	    cout << "Resource list = " << lin << endl;
#endif
//
// Only one update if the resource is a simple one 
//
	    if ((pos = lin.find(SEP_ELT)) == string::npos)
	    	upd_res(lin, ind, false);
	    else
	    {		
		do 
		{
		    upd_res(lin.substr(0, pos), ind, true); 
		    lin.erase(0, pos + 1);
		    ind++;	
		}while((pos = lin.find(SEP_ELT)) != string::npos);
		upd_res(lin, ind, true); 
	    }
	}
    }
    catch(const bad_alloc &)
    {
	psdev_back.psdev_err = i + 1;
	psdev_back.error_code = DbErr_ClientMemoryAllocation;
    }
    catch(const long err)
    {
	psdev_back.psdev_err = i + 1;
	psdev_back.error_code = err;
    }
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
long NdbmServer::upd_res(const string &lin, const long numb, bool array) throw (long)
{
    static string 	t_name;
//    static reso 	res;
//    reso 		ret;
    string::size_type	pos,
			last_pos;
    int 		i,
			l,
			indi,
			resu;
    static DBM 		*tab;
    datum 		key,
     			content,
          		cont,
    			key_array;
    int 		res_numb;
    static bool 	sec_res;
    int 		ctr = 0;
//  char 		res_db[MAX_RES];
    bool 		old_res_array;	
    static string	family,
			member,
			r_name,
			r_val;

    if (numb == 1)
    {
//
// Get table name 
//
	pos = lin.find('/');
	t_name = lin.substr(0, pos).c_str();
		
	sec_res = (t_name == "sec");
//
// Get family name 
//
	pos = lin.find('/', (last_pos = pos + 1));
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
	r_name = lin.substr(last_pos, pos - last_pos);
//
// If the resource belongs to Security domain, change every occurance of
// | by a ^ character 
// 
   	if (sec_res)
	    transform(r_name.begin(), r_name.end(), r_name.begin(), DBServer::make_sec);
//
// Get resource value (resource values are stored in the database as 
// case dependent strings 
//
	r_val = lin.substr(pos + 1);
    }
    else
	r_val = lin;
//
// For security domain, change every occurance of | by a ^ 
//
    if (sec_res)
	transform(r_val.begin(), r_val.end(), r_val.begin(), DBServer::make_sec);
//
// Initialise resource number 
//
    indi = numb;
#ifdef DEBUG
    cout << "Table name : " << t_name << endl;
    cout << "Family name : " << family << endl;
    cout << "Number name : " << member << endl;
    cout << "Resource name : " << r_name << endl;
    cout << "Resource value : " << r_val << endl;
    cout << "Sequence number : " << indi << endl << endl;
#endif /* DEBUG */
//
// Select the right resource table in database 
//
    if (numb == 1)
    {
	for (i = 1;i < dbgen.TblNum;i++)
	    if (t_name == dbgen.TblName[i])
	    {
		tab = dbgen.tid[i];
		break;
	    }
	if (i == dbgen.TblNum)
	    throw long(DbErr_DomainDefinition);
    }
//
// Try to retrieve the right tuple in table 
//
    try
    {
    	key.dptr = new char[MAX_KEY];
    	content.dptr = new char[MAX_CONT];
    	res_numb = numb;
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << family << "|" << member << "|" << r_name << "|" << numb << "|" << ends;
#if !HAVE_SSTREAM
        strcpy(key.dptr, s.str());
    	key.dsize = strlen(s.str());
        s.freeze(false);
#else
        strcpy(key.dptr, s.str().c_str());
    	key.dsize = s.str().size();
#endif
//
// If the resource value is %, remove all the resources.
// If this function is called for a normal resource, I must also 
// remove all the old resources with the old name. This is necessary if there
// is an update of a resource which was previously an array 
//
    	if (r_val == "%" || !array)
    	{
	    key_array.dptr = new char[MAX_KEY];
	    while(1)
	    {
#if !HAVE_SSTREAM
        	s.seekp(0, ios::beg);
#else
		s.str("");
#endif
		s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
    		key.dsize = strlen(s.str());
        	s.freeze(false);
#else
        	strcpy(key.dptr, s.str().c_str());
    		key.dsize = s.str().size();
#endif
	    	cont = dbm_fetch(tab,key);
	    	if (!cont.dptr)
	    	{
		    if (dbm_error(tab) == 0)
		    	break;
		    else
		    {
		    	dbm_clearerr(tab);
			delete [] key_array.dptr;
		    	throw long(DbErr_DatabaseAccess);
		    }
	    	}
	    	ctr++;
//	    	strncpy(res_db,cont.dptr,cont.dsize);
//	    	res_db[cont.dsize] = '\0';
//
// The resource already exists in db. Check if one element with indoce 2 also
// exists. It it is the case, the resource is an array */
//
	    	if (ctr == 1)
	    	{
#if !HAVE_SSTREAM
        	    s.seekp(0, ios::beg);
#else
		    s.str("");
#endif
		    s << family << "|" << member << "|" << r_name << "|2|" << ends;
#if !HAVE_SSTREAM
        	    strcpy(key_array.dptr, s.str());
    		    key_array.dsize = strlen(s.str());
        	    s.freeze(false);
#else
        	    strcpy(key_array.dptr, s.str().c_str());
    		    key_array.dsize = s.str().size();
#endif

		    cont = dbm_fetch(tab, key_array);
		    if (!cont.dptr)
		    {
		    	if (dbm_error(tab) == 0)
			    old_res_array = false;
		    	else
		    	{
			    dbm_clearerr(tab);
			    delete [] key_array.dptr;
			    throw long(DbErr_DatabaseAccess);
		    	}
		    }
		    else
		    	old_res_array = true;
	    	}
	    	dbm_delete(tab,key);
	    	res_numb++;
	    }
	    delete [] key_array.dptr;
	    if (r_val == "%") 
	    {
	    	delete [] key.dptr;
	        delete [] content.dptr;
	        return(0);
	    }
    	}
//
// Insert a new tuple 
//
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#else
	s.str("");
#endif
	s << family << "|" << member << "|" << r_name << "|" << numb << "|" << ends;
#if !HAVE_SSTREAM
        strcpy(key.dptr, s.str());
    	key.dsize = strlen(s.str());
        s.freeze(false);
#else
        strcpy(key.dptr, s.str().c_str());
    	key.dsize = s.str().size();
#endif

    	strcpy(content.dptr, r_val.c_str());
    	content.dsize = r_val.size();

    	switch(dbm_store(tab, key, content, DBM_REPLACE))
    	{
	    case 0 : break;
	    case 1 : throw long (DbErr_DoubleTupleInRes);
	    default: throw long (DbErr_DatabaseAccess);
        }
    }
    catch(const long err)
    {
    	delete [] key.dptr;
    	delete [] content.dptr;
	throw err;
    }
    catch(const bad_alloc &)
    {
    	delete [] key.dptr;
    	delete [] content.dptr;
	throw long(DbErr_ServerMemoryAllocation);
    }
    delete [] key.dptr;
    delete [] content.dptr;
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
db_res *NdbmServer::secpass_1_svc()
{
    char 		pass[80];
	
#ifdef DEBUG
    cout << "In secpass_1_svc function" << endl;
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
    string f_name((char *)getenv("DBM_DIR"));
    f_name.append("/.sec_pass");	
//
// Try to open the file
//
    ifstream f(f_name.c_str());
    if (!f)
    {
	browse_back.db_err = DbErr_NoPassword;
	return(&browse_back);
    }
//
// Get password
//
    f.getline(pass, sizeof(pass));
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
	browse_back.res_val.arr1_val = new char * [1];
	browse_back.res_val.arr1_val[0] = new char [strlen(pass) + 1];
	strcpy(browse_back.res_val.arr1_val[0],pass);
    }
    catch (bad_alloc)
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
