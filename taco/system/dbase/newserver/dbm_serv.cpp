#include "config.h"
#include <DevErrors.h>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <algorithm>
#include <cctype>
/*
static int make_sec(int c)
{
    return (c == '|') ? SEC_SEP : c;
}

static int make_unsec(int c)
{
    return (c == SEC_SEP) ? '|' : c;
}
*/
NdbmServer::NdbmServer(const string user, const string password, const string db)
        : DBServer()
{
//
// Open database tables according to the definition
//
    if (*this->db_reopendb_1_svc() != 0)
        return;
    this->dbgen.connected = true;
    return;
}

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
db_res *NdbmServer::db_getres_1_svc(arr1 * rece, struct svc_req *rqstp)
{
    int 		k = 0;
    u_int 		num_res = rece->arr1_len;
    struct sockaddr_in 	so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
    socklen_t 		so_size;		/* from POSIX draft - already used by GLIBC */
#else
    int so_size;
#endif
    u_short prot;
//
// Return error code if the server is not connected to the database */
//
    if (dbgen.connected == False)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return (&browse_back);
    }
//
// Retrieve the protocol used to send this request to server */
//
    so_size = sizeof(so);

#ifdef sun
    if (rqstp->rq_xprt->xp_port == udp_port)
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#else
    so_size = sizeof(so);
    if (getsockname(rqstp->rq_xprt->xp_sock, (struct sockaddr *)&so, &so_size) == -1)
    {
	browse_back.db_err = DbErr_TooManyInfoForUDP;
	browse_back.res_val.arr1_len = 0;
	return (&browse_back);
    }

    if (so.sin_port == getUDPPort())
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#endif /*solaris */

#ifdef DEBUG
    for (int i = 0; i < num_res; i++)
	cout << "Resource name : " << rece->arr1_val[i] << endl;
#endif
//
// Initialize browse_back structure error code */
//
    browse_back.db_err = 0;
//
// Allocate memory for the array of string sended back to client 
//
    try
    {
    	browse_back.res_val.arr1_val = new nam[num_res];
    } 
    catch (const bad_alloc &)
    {
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	browse_back.res_val.arr1_len = 0;
	return (&browse_back);
    }
//
// A loop on the resource's number to be looked for */
//
    for (int i = 0; i < num_res; i++)
    {
	string ptrc(rece->arr1_val[i]);
//
// Find the table name (DOMAIN) 
//
	string::size_type 	pos = ptrc.find('/');
	string		  	tab_name(ptrc.substr(0, pos));
	string			rest(ptrc.substr(pos + 1));
//
// Try to find the resource value from database 
//
	try
	{
	    db_find(tab_name, rest, &browse_back.res_val.arr1_val[i]);
	}
	catch (const int err_db)
	{
	    for (int j = 0; j < i; j++)
		delete [] browse_back.res_val.arr1_val[j];
	    delete [] browse_back.res_val.arr1_val;
	    browse_back.db_err = err_db;
	    browse_back.res_val.arr1_len = 0;
	    return (&browse_back);
	}
//
// Compute an estimation of the network packet size (Only if the UDP protocol
// has been used to send this request to the server) */
//
	if (prot == IPPROTO_UDP)
	{
	    if ((k = strlen(browse_back.res_val.arr1_val[i]) + k) > SIZE - 1000)
	    {
		for (int j = 0; j <= i; j++)
		    delete [] (browse_back.res_val.arr1_val[j]);
		delete [] browse_back.res_val.arr1_val;
		browse_back.db_err = DbErr_TooManyInfoForUDP;
		browse_back.res_val.arr1_len = 0;
		return (&browse_back);
	    }

	}

    }
//
// Initialize the structure sended back to client */
//
    browse_back.res_val.arr1_len = num_res;
//
// Exit server */
//
    return (&browse_back);
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
db_res *NdbmServer::db_getdev_1_svc(nam * dev_name)
{
    int dev_num,
	err_db;

#ifdef DEBUG
    cout << "Device server name (getdevlist) : " << *dev_name << endl;
#endif
//
// Initialize error code sended back to client 
//
    browse_back.db_err = 0;
//
// Return error code if the server is not connected to the database 
//
    if (!dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	return (&browse_back);
    }
//
// Call database function 
//
    try
    {
	dev_num = db_devlist(*dev_name, &browse_back);
#ifdef DEBUG
    	for (int i = 0; i < dev_num; i++)
	    cout << "Device name : " << browse_back.res_val.arr1_val[i] << endl;
#endif
    }
    catch (const int err_db)
    {
	browse_back.db_err = err_db;
	for (int i = 0;;)
	browse_back.res_val.arr1_len = 0;
    }
//
// Exit server 
//
    return (&browse_back);
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
int NdbmServer::db_find(const string tab_name, const string res_name, char **out) throw (int)
{
    string		p_res_name(res_name),
    			adr_tmp1;
    int 		k,
			ctr = 0;
    bool		sec_res = (tab_name == "sec");
    DBM 		*tab;
    datum 		key,
			resu;
    int 		res_numb = 1;
    int 		i;

#ifdef DEBUG
    cout << "Table name : " << tab_name << endl;
#endif

// Get family name 
    string::size_type	pos = p_res_name.find("/");
    string		family = p_res_name.substr(0, pos);
    p_res_name.erase(0, pos + 1);

// Get member name 
    pos = p_res_name.find("/");
    string	member = p_res_name.substr(0, pos);
// Get resource name 
    string	r_name = p_res_name.substr(pos + 1);
//
// For security domain, change all occurances of | by ^ (| is the field
// separator in NDBM !) 
//
    if (sec_res) 
	transform(r_name.begin(), r_name.end(), r_name.begin(), DBServer::make_sec);

#ifdef DEBUG
    cout << "Family name : " << family << endl;
    cout << "Member name : " << member << endl;
    cout << "Resource name : " << r_name << endl;
#endif
//
// Select the right resource table in the right database */
//
    for (i = 0; i < dbgen.TblNum; i++)
	if (tab_name == dbgen.TblName[i])
	{
	    tab = dbgen.tid[i];
	    break;
	}
    if (i == dbgen.TblNum)
	throw int(DbErr_DomainDefinition);
//
// Try to retrieve the right tuple in table and loop in the case of an
// array of resources */
//
    try 
    {
	key.dptr = new char[MAX_KEY];
    }
    catch(const bad_alloc &)
    {
	cerr << "Error in malloc for key" << endl;
	throw int (DbErr_ServerMemoryAllocation);
    }

    string	outstring;
    do
    {
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
	strcpy(key.dptr, s.str());
        s.freeze(false);
#else
	strcpy(key.dptr, s.str().c_str());
#endif
	key.dsize = strlen(key.dptr);

	resu = dbm_fetch(tab, key);
	if (resu.dptr != NULL)
	{
	    string	resu_out(resu.dptr, resu.dsize);
	    if (ctr)
	    {
//
// Copy the new array element in the result buffer. If the temporary buffer
// is full, realloc memory for it. */
//
		adr_tmp1 += SEP_ELT;
		adr_tmp1 += resu_out;
	    }
	    else
		adr_tmp1 = resu_out;
	    ctr++;
	    res_numb++;
	}
	else
	    break;
    }
    while (true);
//
// Reset the temporary buffer 
//
    delete [] key.dptr;

    try
    {
	switch(ctr)
	{
//
// If it is a normal resource,so copy the resource value to the result buffer 
//
	    case 1 : 	*out = new char[adr_tmp1.length() + 1];
	    		strcpy(*out, adr_tmp1.c_str());
			break;
//
// Initialize resource value to N_DEF if the resource is not defined in the database 
//
	    case 0 : 	if (resu.dptr == NULL)
			{
			    *out = new char[10];
			    strcpy(*out, "N_DEF");
			}
			break;
	    default : 	*out = new char[adr_tmp1.length() + 10];
	    		(*out)[0] = INIT_ARRAY;
	    		sprintf(&((*out)[1]), "%d", ctr);
	    		k = strlen(*out);
	    		(*out)[k] = SEP_ELT;
	    		(*out)[k + 1] = '\0';
	    		strcat(*out, adr_tmp1.c_str());
			break;
	}
    }
    catch(const bad_alloc &)
    {
	delete [] key.dptr;
	cerr << "Error in malloc for out" << endl;
	throw int (DbErr_ServerMemoryAllocation);
    }
//
// For resource of the SEC domain, change all occurences of the ^ character
// to the | character 
//
    if (sec_res)
	transform(*out, *out + strlen(*out), *out, &DBServer::make_unsec);
    return (0);
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
*            - The adress  of the db_res structure tobe initialized with    *
*              the devices names                                            *
*              (see the definition of the db_res structure above)           *
*                                                                           *
*    Argout : - The number of devices managed by the devices server         *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int NdbmServer::db_devlist(const string dev_name, db_res * back)
{
    string		dev_na(dev_name);
    vector<nam> 	ptra;
    register int 	d_num = 0;
    int 		i,
			j;
    datum 		key,
    			resu;
    int 		dev_numb = 1;
    char 		indnr[4];
//
// Get device server type 
//
    string::size_type	pos = dev_na.find("/");
    string		ds_class = dev_na.substr(0, pos);
//
// Get device type 
//
    string		ds_name = dev_na.substr(pos + 1);

#ifdef DEBUG
    cout << "Device server class (getdevlist) : " << dev.ds_class << endl;
    cout << "Device server name (getdevlist) : " << dev.ds_name << endl;
#endif /* DEBUG */
//
// Try to retrieve the right tuple in NAMES table */
//
    key.dptr = new char[MAX_KEY];
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

	resu = dbm_fetch(dbgen.tid[0], key);
	if (resu.dptr != NULL)
	{
//
// Unpack the retrieved content 
//
	    string	dev_name = resu.dptr;
	    pos = dev_name.find('|');
	    if (pos == string::npos)
	    {
		cerr << "No separator in the content." << endl;
		delete [] key.dptr;
		throw int (ERR_DEVNAME);
	    }
	    try
	    {
	    	string	d_name = dev_name.substr(0, pos);
//
// Allocate memory for device name */
//
	    	char *p = new char[d_name.length() + 1];
//
// Copy the device name 
//
	    	strcpy(p, d_name.c_str());
	    	dev_numb++;
	    	d_num++;
	    	ptra.push_back(p);
	    }
	    catch(const bad_alloc &)
	    {
		for (vector<nam>::iterator j = ptra.begin(); j != ptra.end(); ++j)
		    delete [] *j;
		delete [] key.dptr;
		return (DbErr_ServerMemoryAllocation);
	    }
	}
	else
	    break;
    }
    while (true);
    delete [] key.dptr;
//
// Initialize the structure 
//
    try
    {
    	back->res_val.arr1_val = new nam[ptra.size()];
    	back->res_val.arr1_len = ptra.size();
	for (int j = 0; j < back->res_val.arr1_len; ++j)
	    back->res_val.arr1_val[j] = ptra[j];
    	if (resu.dptr == NULL && back->res_val.arr1_len == 0)
	    throw int (DbErr_DeviceServerNotDefined);
    	return (back->res_val.arr1_len);
    }
    catch(const bad_alloc &)
    {
	return (DbErr_ServerMemoryAllocation);
    }	
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
DevLong *NdbmServer::db_putres_1_svc(tab_putres * rece)
{
    int 	res_num = rece->tab_putres_len;;
    int 	i,
		ret_res = 0;
    string 	domain,
    		family,
    		member,
    		r_name;
    unsigned int ctr;
    DBM 	*tab;
    datum 	key,
    		content;
    int 	res_numb = 1;

#ifdef DEBUG
    for (i = 0; i < res_num; i++)
	cout << "Resource name : " << rece->tab_putres_val[i].res_name << endl;
#endif /* DEBUG */
//
// Initialize sent back error code */
//
    errcode = 0;
//
// Return error code if the server is not connected to the database */
//
    if (!dbgen.connected)
    {
	errcode = DbErr_DatabaseNotConnected;
	return (&errcode);
    }
//
// Allocate memory for key and content pointers */
//
    try
    {
    	key.dptr = new char[MAX_KEY];
    }
    catch (const bad_alloc &)
    {
	errcode = DbErr_ServerMemoryAllocation;
	return (&errcode);
    }
    try
    {
    	content.dptr = new char[MAX_CONT];
    }
    catch (const bad_alloc &)
    {
	delete [] key.dptr;
	errcode = DbErr_ServerMemoryAllocation;
	return (&errcode);
    }
//
// A loop for every resource 
//
    try
    {
    	for (int ires = 0; ires < res_num; ires++)
    	{
	    putres 	*tmp_ptr = &(rece->tab_putres_val[ires]);
	    res_numb = 1;
//
// Extract domain, family, member and resource name from the full resource name 
//
	    string	temp = tmp_ptr->res_name;
	    string::size_type	pos = temp.find('/');
	    domain = temp.substr(0, pos);
	    temp.erase(0, pos + 1);

	    pos = temp.find('/');
	    family = temp.substr(0, pos);
	    temp.erase(0, pos + 1);

	    pos = temp.find('/');
	    member = temp.substr(0, pos + 1);
	    r_name = temp.substr(pos + 1);

#ifdef DEBUG
	    cout << "Domain name : " << domain << endl;
	    cout << "Family name : " << family << endl;
	    cout << "Member name : " << member << endl;
	    cout << "Resource name : " << r_name << endl;
#endif 
//
// Select the right resource table in CS database 
//
	    for (i = 0; i < dbgen.TblNum; i++)
	        if (domain == dbgen.TblName[i])
	        {
		    tab = dbgen.tid[i];
		    break;
	        }
	    if (i == dbgen.TblNum)
	    	throw long(DbErr_DomainDefinition);
//
// Delete the old information (single or array) if the array already exists
//
	    for(res_numb = 1;;)
	    {
	        stringstream 	s;
#if !HAVE_SSTREAM
        	s.seekp(0, ios::beg);
#endif
	        s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
       	 	s.freeze(false);
#else
        	strcpy(key.dptr, s.str().c_str());
#endif
	        key.dsize = strlen(key.dptr);
	        if (dbm_delete(tab, key))
		    break;  
	        res_numb++;
	        ret_res = 1;
	    } 
//
// If the new update is for an array 
//
	    if (tmp_ptr->res_val[0] == INIT_ARRAY)
	    {
//
// Retrieve the number of element in the array.
// Initialize the loop counter "ctr" to number of element minus one because
// it is not necessary to look for the element separator to extract the last
// element value from the string. 
//
	        string 	tmp = tmp_ptr->res_val;
	        pos = tmp.find(SEP_ELT);
	        int ctr = (unsigned int)atoi(tmp.substr(1, pos - 1).c_str()) - 1;
	        tmp.erase(0, pos + 1);
	        res_numb = 1;
	        for (int l = 0; l < ctr; l++)
	        {
//
// Initialize database information 
//
	    	    stringstream 	s;
#if !HAVE_SSTREAM
        	    s.seekp(0, ios::beg);
#endif
	    	    s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        	    strcpy(key.dptr, s.str());
        	    s.freeze(false);
#else
        	    strcpy(key.dptr, s.str().c_str());
#endif
	    	    key.dsize = strlen(key.dptr);
//
// Add one array element in the database 
//
		    pos = tmp.find(SEP_ELT);
		    strcpy(content.dptr, tmp.substr(0, pos).c_str());
		    content.dsize = strlen(content.dptr);
		    res_numb++;
		    if (dbm_store(tab, key, content, DBM_INSERT))
		    	throw long(DbErr_DatabaseAccess);
	    	}
//
// For the last element value 
//
	    	strcpy(content.dptr, tmp.c_str());
	    	content.dsize = strlen(content.dptr);
	    	stringstream	s;
#if !HAVE_SSTREAM
        	s.seekp(0, ios::beg);
#endif
	    	s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
        	s.freeze(false);
#else
        	strcpy(key.dptr, s.str().c_str());
#endif
	    	key.dsize = strlen(key.dptr);
	    	res_numb++;
	    	if (dbm_store(tab, key, content, DBM_INSERT))
		    throw long(DbErr_DatabaseAccess);
	    }
	    else
	    {
		res_numb = 1;
	    	stringstream	s;
#if !HAVE_SSTREAM
        	s.seekp(0, ios::beg);
#endif
	    	s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
        	s.freeze(false);
#else
        	strcpy(key.dptr, s.str().c_str());
#endif
	    	key.dsize = strlen(key.dptr);
		strcpy(content.dptr, tmp_ptr->res_val);
		content.dsize = strlen(content.dptr);
//
// If the resource is already defined in the database, just update the tuple or
// insert a new tuple 
//
		if (dbm_store(tab, key, content, ret_res ? DBM_REPLACE : DBM_INSERT))
		    throw long(DbErr_DatabaseAccess);
	    }			
	}			// end of for for every resource 
    }				
    catch (const long err)
    {
	errcode = err;
    }
//
// Leave server 
//
    delete [] key.dptr;
    delete [] content.dptr;
    return (&errcode);
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
DevLong *NdbmServer::db_delres_1_svc(arr1 * rece /* , struct svc_req *rqstp */)
{
    u_int 	num_res = rece->arr1_len;
    char 	**old_res;

#ifdef DEBUG
    for (int i = 0; i < num_res; i++)
	cout << "Resource to delete : " << rece->arr1_val[i] << endl;
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
	return (&errcode);
    }
//
// Mark the server as not connected. This will prevent dbm_update to
// add/modify resources during this call 
//
    dbgen.connected = false;
//
// Allocate array for pointers to store deleted resources value 
//
    try
    {
    	old_res = new char*[num_res];
	int	err_db;
//
// A loop on the resource's number to be deleted 
//
    	for (int i = 0; i < num_res; i++)
    	{
	    string ptrc = rece->arr1_val[i];
//
// Try to delete the resource from database
//
	    if ((err_db = db_del(ptrc, &(old_res[i]))) != 0)
	    {
	    	if (i != 0)
		    db_reinsert(rece, old_res, i);
		throw long(err_db);
	    }
	}
    }
    catch(const bad_alloc &)
    {
	errcode = DbErr_ServerMemoryAllocation;
    }
    catch(const long err_db)
    {
	errcode = err_db;
    }
//
// Free memory and exit server 
//
    for (int i = 0; i < num_res; i++)
	delete [] old_res[i];
    delete [] old_res;
    dbgen.connected = true;
    return (&errcode);
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
int NdbmServer::db_del(string res_name, char **p_oldres)
{
    string 		t_name,
			family,
			member,
			r_name;
    string::size_type 	pos, 
			last_pos;
    int 		i,
			k,
			k1,
			ctr = 0,
			res_numb = 1,
			err;
    static string	tmp_buf;
    datum 		key;
    DBM 		*tab;
//
// Get table name 
//
    if ((pos = res_name.find('/')) == string::npos)
    {
	cerr << "db_del : Error in resource name " << res_name << endl;
	return (DbErr_BadResourceType);
    }
    t_name = res_name.substr(0, pos);
//
// Get family name 
//
    if ((pos = res_name.find('/', (last_pos = pos + 1))) == string::npos)
    {
	cerr << "db_del : Error in resource name " << res_name << endl;
	return (DbErr_BadResourceType);
    }
    family = res_name.substr(last_pos, pos - last_pos);
//
// Get member name 
//
    if ((pos = res_name.find('/', (last_pos = pos + 1))) == string::npos)
    {
	cerr << "db_del : Error in resource name " << res_name << endl;
	return (DbErr_BadResourceType);
    }
    member = res_name.substr(last_pos, pos - last_pos);
//
// Get resource name 
//
    r_name = res_name.substr(pos + 1);

#ifdef DEBUG
    cout << "Family name : " << family << endl;
    cout << "Number name : " << member << endl;
    cout << "Resource name : " << r_name << endl;
#endif
//
// Select the right resource table in database
//
    for (i = 0; i < dbgen.TblNum; i++)
    {
	if (t_name == dbgen.TblName[i])
	{
	    tab = dbgen.tid[i];
	    break;
	}
    }
    if (i == dbgen.TblNum)
	return (DbErr_DomainDefinition);
//
// Allocate memory to store the old resource value (to reinsert it in case
// of problem) and for the key
//
    try
    {
    	key.dptr = new char[MAX_KEY];
    }
    catch(const bad_alloc &)
    {
	return (DbErr_ServerMemoryAllocation);
    }
//
// Try to retrieve the right tuple in table and loop for the case of an
// array of resource 
//
    do
    {
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << family <<"|" << member << "|" << r_name << "|" << res_numb << "|" << ends;
#if !HAVE_SSTREAM
        strcpy(key.dptr, s.str());
        s.freeze(false);
#else
        strcpy(key.dptr, s.str().c_str());
#endif
	key.dsize = strlen(key.dptr);

	datum resu = dbm_fetch(tab, key);
	if (resu.dptr != NULL)
	{
	    if (ctr)
	    {
//
// Copy the new element in the temporary buffer. If it is full, reallocate
// memory for it. 
//
		tmp_buf += SEP_ELT;
		tmp_buf += string(resu.dptr, resu.dsize);
	    }
	    else
//
// It is the first element, just copy it in the temporary buffer/
//
		tmp_buf = string(resu.dptr, resu.dsize);
//
// Remove the tuple from database 
//
	    dbm_delete(tab, key);
	    ctr++;
	    res_numb++;
	}
	else
	{
//
// Is it an error or simply the data does not exist in the database 
//
	    if ((err = dbm_error(tab)) != 0)
	    {
		dbm_clearerr(tab);
		return (DbErr_DatabaseAccess);
	    }
	    break;
	}
    }
    while (true);
    delete [] key.dptr;
//
// If it is a classical resource, copy the res. value in the real old res value buffer 
//
    try
    {
    	switch (ctr)
    	{
    	    case 1: 	*p_oldres = new char[tmp_buf.length() + 1];
			strcpy(*p_oldres, tmp_buf.c_str());
			break;
	    case 0: 	return (DbErr_ResourceNotDefined);
			break;
	    default:
//
// For an array of resource, add the number of resources at the beginning of the string 
//
			*p_oldres = new char[tmp_buf.length() + 10];
			(*p_oldres)[0] = INIT_ARRAY;
			sprintf(&((*p_oldres)[1]), "%d", ctr);
			k = strlen(*p_oldres);
			(*p_oldres)[k] = SEP_ELT;
			(*p_oldres)[k + 1] = 0;
			strcat(*p_oldres, tmp_buf.c_str());
    	}
    }
    catch (const bad_alloc &)
    {
	return (DbErr_ServerMemoryAllocation);
    }
//
// Return if the resource is not found 
//
    return (0);
}

/****************************************************************************
*                                                                           *
*		Code for db_reinsert function                               *
*                        -----------                                        *
*                                                                           *
*    Function rule : To reinsert a resource in the datbase                  *
*		     This function is called only if the db_del function    *
*		     returns a error					    *
*                                                                           *
*    Argin : - The array passed to the db_delresource server part	    *
*	     - A array with the resource value				    *
*	     - The number of resource to be reinserted			    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
int NdbmServer::db_reinsert(arr1 * rece, char **res_value, int num_res)
{
    tab_putres 	tmp;
    int 	j,
		num = 0;
//
// Find out how many devices are really to be reinserted */
//
    for (int i = 0; i < num_res; i++)
	if (res_value[i] != NULL)
	    num++;
//
// If the error happens after several try to delte only resources which don't
// exist, it  is not necessary to reinsert them ! 
//
    if (num == 0)
	return (0);
    tmp.tab_putres_len = num;
//
// Allocate a array of putres structure (one structure for each res.)
//
    try
    {
    	tmp.tab_putres_val = new putres[num];
    }
    catch (const bad_alloc &)
    {
	return (DbErr_ServerMemoryAllocation);
    }
//
// Initialise the array of putres structure with the resource name and resource
// value 
//
    j = 0;
    for (int i = 0; i < num_res; i++)
	if (res_value[i] != NULL)
	{
	    tmp.tab_putres_val[j].res_name = rece->arr1_val[i];
	    tmp.tab_putres_val[j].res_val = res_value[i];
	    j++;
	}
//
// Call the putresource function */
//
    DevLong *p = db_putres_1_svc(&tmp);
//
// Leave function 
//
    delete [] tmp.tab_putres_val;
    return (0);
}

void NdbmServer::leave(void)
{
//
// Close database 
//
    for (int i = 0; i < dbgen.TblNum; i++)
	dbm_close(dbgen.tid[i]);
//
// Exit now 
//
    exit(-1);
}
