#include <DevErrors.h>
#include <NdbmServer.h>
#include <algorithm>

/* File global variables, should be removed */
// static string 	domain;	
// static string 	family;	
// static string 	member;	
//static string 	domain_tup;	
// static string 	family_tup;	
static int  	dev_num;


/****************************************************************************
*                                                                           *
*		Server code for db_getdevexp function                       *
*                               -------------                               *
*                                                                           *
*    Function rule : To retrieve the names of the exported devices          *
*                    device server.                                         *
*                                                                           *
*    Argin : A string to filter special device names in the whole list of   *
*	     the exported device					    *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;  A structure of the arr1 type with the device names    *
*      int db_err;    The database error code (0 if no error)               *
*            }                                                              *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::db_getdevexp_1_svc(nam *fil_name,struct svc_req *rqstp)
{
    string		device_name(*fil_name);
    int 		i,err,ret;
    register char 	*tmp,*temp;
    unsigned int 	diff;
    device 		dev;
    int 		k1 = 1;
    struct sockaddr_in 	so;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
    socklen_t  		so_size;   /* from POSIX draft - already used by GLIBC */
#else
    int 		so_size;
#endif
    datum 		key, key_sto;
    datum 		content;
    char 		*tbeg, *tend;
    string 		ret_host_name,
     			ret_pn,
    			prgnr;
    int 		exit = 0;
    int 		flags=O_RDWR;
    int			prot;
//
// If the server is not connected to the database, return an error 
//
    if (!dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

#ifdef sun
    if (rqstp->rq_xprt->xp_port == udp_port)
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#else
//
// Retrieve the protocol used to send this request to the server
//
	so_size = sizeof(so);
	if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so, (socklen_t *)&so_size) == -1)
	{
		browse_back.db_err = DbErr_MaxDeviceForUDP;
		browse_back.res_val.arr1_len = 0;
		return(&browse_back);
	}
    	if (so.sin_port == getUDPPort())
		prot = IPPROTO_UDP;
	else
		prot = IPPROTO_TCP;
#endif
//
// Extract from filter string each part of the filter (domain, family and 
// member). If two / characters can be retrieved in the filter string, this
// means that the domain, family and member part of the filter are initialized.
// If only one / can be retrieved, only the domain and family part are
// initialized and if there is no / in the filter string, just the domain
// is initialized. 
//
    string::size_type	pos;
    string	member,
		family,
    		domain;
    switch(count(device_name.begin(), device_name.end(), '/'))
    {
	case 2 : pos =  device_name.find('/');
		 domain = device_name.substr(0, pos);	// potential error
		 device_name.erase(0, pos + 1);
		 pos =  device_name.find('/');
		 family	= device_name.substr(0, pos);	// potential error	
		 member = device_name.substr(pos + 1);
		 break;

	case 1 : pos =  device_name.find('/');
		 domain = device_name.substr(0, pos);	// potential error
		 family	= device_name.substr(pos + 1);	// potential error	
		 member = "*";
		 break;

	case 0 : domain = device_name;			// potential error
		 family = "*";
		 member = "*";
		 break;
    }

#ifdef DEBUG
    cout << "filter domain : " << domain << endl;
    cout << "filter family : " << family << endl;
    cout << "filter member : " << member << endl;
#endif /* DEBUG */
//
// Allocate memory for the pointer's array */
//
    dev_num = 0;
    vector<nam>	ptra;
//
// Try to retrieve all tuples in the database NAMES table with the PN column
// different than "not_exp" 
//
    key_sto.dptr = new char[MAX_KEY];
    key = gdbm_firstkey(dbgen.tid[0]);
    if (key.dptr == NULL)
    {
	delete [] key_sto.dptr;
	browse_back.db_err = DbErr_DatabaseAccess;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

    do
    {
//
// Store the key if it is needed later 
//
	strncpy(key_sto.dptr, key.dptr, key.dsize);
	key_sto.dptr[key.dsize] = '\0';
	key_sto.dsize = key.dsize;
	content = gdbm_fetch(dbgen.tid[0], key);

	if (content.dptr != NULL)
	{
	    string temp(content.dptr, content.dsize);
		
	    string::size_type 	pos = temp.find('|');
	    strcpy(dev.d_name, temp.substr(0, pos).c_str());
	    temp.erase(0, pos + 1);

	    pos = temp.find('|');
	    ret_host_name = temp.substr(0, pos);
	    temp.erase(0, pos + 1);
		
	    pos = temp.find('|');
	    ret_pn = temp.substr(0, pos);

	    if (ret_pn != "0")
	    {
//
// Move the the rest of the content and key values to the structure of dev 
//
		strcpy(dev.h_name, ret_host_name.c_str());
		dev.pn = atoi(ret_pn.c_str());

	    	temp.erase(0, pos + 1);
	    	pos = temp.find('|');
		dev.vn = atoi(temp.substr(0, pos).c_str());
	    	temp.erase(0, pos + 1);

	    	pos = temp.find('|');
		strcpy(dev.d_type, temp.substr(0, pos).c_str());
	    	temp.erase(0, pos + 1);

	    	pos = temp.find('|');
		strcpy(dev.d_class, temp.substr(0, pos).c_str());
	    	temp.erase(0, pos + 1);

	    	pos = temp.find('|');
		dev.pid = atoi(temp.substr(0, pos).c_str());
//
// Extract device server class and name from the key 
//
		temp = key_sto.dptr;
		if ((pos = temp.find('|')) == string::npos)
		{
		    cerr << "No separator in db tuple" << endl;
		    browse_back.db_err = DbErr_DatabaseAccess;
		    browse_back.res_val.arr1_len = 0;
		    free(key_sto.dptr);
		    return(&browse_back);
		}
		strcpy(dev.ds_class, temp.substr(0, pos).c_str());
	    	temp.erase(0, pos + 1);

	    	pos = temp.find('|');
		strcpy(dev.ds_name, temp.substr(0, pos).c_str());
//
// Extract the domain part of the device name from the retrieved tuple */
//
		temp = dev.d_name;
		pos = temp.find('/');
		string domain_tup =  temp.substr(0, pos);
#ifdef DEBUG
		cout << "Domain part from DB: " << domain_tup << endl;
#endif 
//
// Call the stringOK function to verify that the retrieved devices device name is OK 
// If the domain part of the filter is *, directly call the fam_fil  function 
//
		if (!stringOK(domain, domain_tup))
		{
		    try
		    {
		    	ptra.push_back(fam_fil(&dev, family, member, prot));
		    }
		    catch(const int err)
		    {
			if (err)
			{
			    browse_back.res_val.arr1_val = &ptra.front();
			    for (int i = 0; i < dev_num; i++)
				delete [] browse_back.res_val.arr1_val[i];
			    browse_back.db_err = err;
			    browse_back.res_val.arr1_len = 0;
			    delete [] key_sto.dptr;
			    return(&browse_back);
			}
		    }
		} 
	    } 
	} 
	key = gdbm_nextkey(dbgen.tid[0], key);
    } 
    while (key.dptr != NULL);
//
// If a problem occurs during database function 
//
    if (content.dptr == 0)
    {
	browse_back.res_val.arr1_val = &ptra.front();
	for (int i = 0; i < dev_num; i++)
	    delete [] browse_back.res_val.arr1_val[i];
	browse_back.db_err = DbErr_DatabaseAccess;
	browse_back.res_val.arr1_len = 0;
	delete [] key_sto.dptr;
	return(&browse_back);
    }
//
// Initialize the structure sended back to client and leave the server 
//
    browse_back.res_val.arr1_val = new char *[dev_num];
    browse_back.res_val.arr1_len = dev_num;
    for (int i = 0; i < dev_num; ++i)
	browse_back.res_val.arr1_val[i] = ptra[i];
    browse_back.db_err = 0;
    delete [] key_sto.dptr;
    return(&browse_back);
}


/****************************************************************************
*                                                                           *
*		Code for fam_fil function                                   *
*                        -------                                            *
*                                                                           *
*    Function rule : To check if the family part of the filter is the same  *
*		     than the family part of the device name retrieved from *
*		     the database					    *
*                                                                           *
*    Argin : - A pointer to the structure with the contents of one          *
*	       database tuple.						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
char* NdbmServer::fam_fil(device *dev1, const string &family, const string &member, int prot) throw (int)
{
    string 		tmp(dev1->d_name);
    unsigned int 	diff;
    int 		err,ret;
//
// If the family part of the filter is *, directly call the memb_fil
// function 
//
#ifdef DEBUG
    cout << "Arrived in fam_fil function" << endl;
#endif 
//
// Extract the family part of the device name in the retrieved tuple 
//
    if (count(tmp.begin(), tmp.end(), '/') != 2)
	throw int(1);
    string::size_type	first = tmp.find('/'),
			second = tmp.rfind('/');
    string 		family_tup = tmp.substr(first + 1, second - first);
#ifdef DEBUG
    cerr << "Family part from DB : " << family_tup << endl;
#endif
//
// Special case for the data collector pseudo devices which are not real 
// devices. It is impossible to execute command on them. So, don't return
// them to the caller. 
//
    if ("dc_rd" == family_tup.substr(0, 5))
	throw int(0);
    if ("dc_wr" == family_tup.substr(0, 5))
	throw int(0);
//
// Call the stringOK to verify that the family name is OK 
// Is this family part the same than in the filter, call the memb_fil function 
//
    if (!stringOK(family, family_tup))
	return memb_fil(dev1, member, prot);
    throw int(0);
}

/****************************************************************************
*                                                                           *
*		Code for memb_fil function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To check if the member part of the filter is the same  *
*		     than the member part of the device name retrieved from *
*		     the database. If yes, this device name must be         *
*		     returned to the caller				    *
*                                                                           *
*    Argin : - A pointer to the structure with the contents of one          *
*	       database tuple.						    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no errors occurs or the error code when     *
*    there is a problem.                                                    *
*                                                                           *
****************************************************************************/
char *NdbmServer::memb_fil(device *dev2, const string &member, int prot) throw (int)
{
    string 		tmp(dev2->d_name);
    string::size_type 	pos;
    char		*p;
    int 	tp,
		ret;
//
// If the member part of the filter is *, the device name must be sent back to
// the user. This means copy the device name in the special array (allocate
// memory for the string) and increment the device name counter 
//
#ifdef DEBUG
    cout << "Arrived in memb_fil function" << endl;
#endif 
//
// Extract the member part of the device name in the retrieved tuple */
//
    if (count(tmp.begin(), tmp.end(), '/') != 2)
	throw int(1);
    pos = tmp.rfind('/');
#ifdef DEBUG
    cout << "member part from DB : " << tmp.substr(pos + 1) << endl;
#endif 
//
// Call the stringOK function to verify that the member part of the
// retrieved device is OK 
// Is this member part the same than in the filter ? If yes, the device name
// must be sent back to the user: copy the device name in the special array
// (allocate memory for the string) and increment the device name counter. 
//
    if (!stringOK(member, tmp.substr(pos + 1)))
    {
	try
	{
	    p = new char[strlen(dev2->d_name) + 1];
	}
	catch(const bad_alloc &)
	{
	    throw int(DbErr_ServerMemoryAllocation);
	}
	strcpy(p, dev2->d_name);
	dev_num++;
	if (prot == IPPROTO_UDP && dev_num == MAXDEV_UDP)
	    throw int(DbErr_MaxDeviceForUDP);
#ifdef DEBUG
    	cout << "One more device name" << endl;
#endif 
    }
    return p;
}

/****************************************************************************
*                                                                           *
*		Code for stringOK function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To check if a string is the same than a wanted string  *
*		     This function allow the caller to have ONE wildcard '*'*
*		     in the wanted string.				    *
*                                                                           *
*    Argin : - A pointer to the wanted string				    *
*   	     - A pointer to the string to be compared			    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if the string in the same than the wanted one  *
*    Otherwise, this function returns 1					    *
*                                                                           *
****************************************************************************/
bool NdbmServer::stringOK(const string wanted, const string retrieved)
{
//
// If the wanted string is only the wild card, return 0 
//
    if (wanted == "*")
	return(0);
//
// If the wild card is in the wanted string, compute the number of
// characters before and after it 
//
    string::size_type	pos;
    if ((pos = wanted.find('*')) != string::npos)
    {
	if (pos >= retrieved.length())
	    return(1);
	if (wanted.length() - pos - 1 >= retrieved.length())
	    return(1);
    }
//
// Test to see if the string is stricly the same than the wanted one 
//
    else
	return (wanted == retrieved); 
//
// Test characters before the wild card
// 
    if (wanted.substr(0, pos) != retrieved.substr(0, pos))
	return(1);
//
// Test characters after the wild card 
//
    long l = wanted.length() - (pos + 1);
    if (wanted.substr(pos + 1) != retrieved.substr(0, retrieved.length() - l))
	return(1);
//
// Leave function 
//
    return(0);
}
