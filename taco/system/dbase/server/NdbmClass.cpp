#include <cstdlib>

#include <macros.h>
#include <db_setup.h>

// C++ include

#include <NdbmClass.h>
#include <new>
#include <string>
#include <algorithm>
#include <iostream>
#include <stdexcept>

//****************************************************************************
//
//		NdbmNamesKey class 
//		------------
//
//	This class is used to manage NAMES table key part. It is constructed
// 	from the NDBM key. Within the NAMES table, the key is :
//		Device server name
//		Device server personal name
//		Device sequence number in the device server device list
//
// 	The record content is :
//		Device name
//		Host name
//		Device server program number
//		Device server version number
//		Device type
//		Device class
//		Device server process PID
//		Device server process name
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmNamesKey::NdbmNamesKey()
{
    key.dptr = NULL;
    key.dsize = 0;
}

// The class destructor

NdbmNamesKey::~NdbmNamesKey()
{
    if (key.dsize != 0)
	delete [] key.dptr;
}

// Class constructor to be used with the record key
NdbmNamesKey::NdbmNamesKey(const datum &call_key)
{
    if (call_key.dptr != NULL)
	str = string(call_key.dptr, call_key.dsize);
    else
	throw NdbmError(DbErr_CantBuildKey, MessBuildKey);
		
    key.dsize = 0;
    key.dptr = NULL;
}

// Class constructor to be used from individual element
NdbmNamesKey::NdbmNamesKey(const string &server, const string &pers_name, const long indi)
{
//
// Allocate memory to store key
//
    try
    {
	key.dptr = new char[MAX_KEY];
//
// Build intermediate key
//
    	inter_str = server + '|' + pers_name + '|';
//
// Build key
//
#if !HAVE_SSTREAM
    	stringstream to(key.dptr, MAX_KEY);
#else
    	stringstream to(string(key.dptr, MAX_KEY));
#endif
    	to << inter_str << indi << '|' << ends;
#if !HAVE_SSTREAM
    	key.dsize = strlen(to.str());
	to.freeze(false);
#else
    	key.dsize = strlen(to.str().c_str());
#endif
    }
    catch (bad_alloc)
    {
	throw;
    }
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one key character
char NdbmNamesKey::operator [](long i)
{
    try
    {
	return(str.at(i));
    }
    catch(const out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
    }
}

//
//		Class methods
//		-------------
// Method to build a datum data from the already stored content
void NdbmNamesKey::build_datum()
{
    long l = str.size();
    if (str.size())
    {
	try
	{
	    key.dptr = new char[l + 1];
	    strcpy(key.dptr,str.c_str());
	    key.dsize = l;
	}
	catch (bad_alloc)
	{
			throw;
	}
    }
}

// Method to return the device server name
string NdbmNamesKey::get_ds_name(void)
{
    string::size_type pos;

    if ((pos = str.find(SEP)) == string::npos)
	throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
    return str.substr(0, pos);	
}

// Method to return the DS personal name
string  NdbmNamesKey::get_ds_pers_name(void)
{
    string::size_type pos,start;
	
    pos = 0;
    for (long i = 0; i < NB_SEP_DS_PERS; ++i)
    {
	if ((pos = str.find(SEP, pos)) == string::npos)
	    throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
	if (i != (NB_SEP_DS_PERS - 1))
	    pos++;
	if (i == (NB_SEP_DS_PERS - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to return the device index in the DS device list

long NdbmNamesKey::get_dev_indi(void)
{
    string::size_type pos;
	
    if ((pos = str.find_last_of(SEP, str.size() - 2)) == string::npos)
	throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
    pos++;

    stringstream st;
    long	indi;
    st << str.substr(pos, (str.size() - 1) - pos);
    st >> indi;
    return indi;
}

// Method to increment the key index part
void NdbmNamesKey::upd_indi(long ind)
{
//
// Build the new key
//
#if !HAVE_SSTREAM
    stringstream to(key.dptr, MAX_KEY);
#else
    stringstream to(string(key.dptr, MAX_KEY));
#endif
    to << inter_str << ind << '|' << ends;

#if !HAVE_SSTREAM
    key.dsize = strlen(to.str()) - 1;
    to.freeze(false);
#else
    key.dsize = strlen(to.str().c_str());
#endif
    return;
}

// Method to return all the parameters from the record key necessary for
// the devinfo call
void NdbmNamesKey::get_devinfo(db_devinfo_svc &data)
{
    string s = this->get_ds_name();
    strcpy(data.server_name, s.c_str());
    s = this->get_ds_pers_name();
    strcpy(data.personal_name,s.c_str());	
    return;
}

void NdbmNamesKey::get_devinfo(db_poller_svc &data)
{
    string s = this->get_ds_name();
    strcpy(data.server_name,s.c_str());
    s = this->get_ds_pers_name();
    strcpy(data.personal_name,s.c_str());	
    return;
}

//****************************************************************************
//
//		NdbmNamesCont class 
//		-------------
//
//	This class is used to manage NAMES table content part. It is constructed
// 	from the NDBM key. Within the pseudo NAMES table, the key is :
//		Device server name
//		Device server personal name
//		Device sequence number in the device server device list
//
// 	The record content is :
//		Device name
//		Host name
//		Device server program number
//		Device server version number
//		Device type
//		Device class
//		Device server process PID
//		Device server process name
//
//****************************************************************************

//
//		Class constructor and destructor
//		--------------------------------
// The class default constructor
NdbmNamesCont::NdbmNamesCont()
{
    dat.dptr = NULL;
    dat.dsize = 0;
}

// The class destructor
NdbmNamesCont::~NdbmNamesCont()
{
    if (dat.dsize != 0)
	delete [] dat.dptr;
}

// Class constructor to be used with the record key
NdbmNamesCont::NdbmNamesCont(GDBM_FILE db, datum key)
{
    datum content;

    content = gdbm_fetch(db,key);
    if (content.dptr != NULL)
	str = string(content.dptr, content.dsize);
    else
	throw NdbmError(DbErr_CantGetContent,MessGetContent);
		
    dat.dsize = 0;
    dat.dptr = NULL;
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one content character
char NdbmNamesCont::operator [] (long i)
{
   try
   {
	return(str.at(i));
   }
   catch (const out_of_range &)
   {
	throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
   }
}

//
//		Class methods
//		-------------
// Method to build a datum data from the already stored content
void NdbmNamesCont::build_datum()
{
    try
    {
        long l = str.size();
        if (l != 0)
	{
	    dat.dptr = new char[l + 1];
	    strcpy(dat.dptr,str.c_str());
	    dat.dsize = l;
	}
    }
    catch (bad_alloc)
    {
	throw;
    }
}

// Method to return the device name from the record content
string NdbmNamesCont::get_device_name(void) const
{
    string::size_type pos;

    if ((pos = str.find(SEP)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax, MessContSyntax);
	
    return str.substr(0, pos);	
}

// Method to return the host name from the record content
string NdbmNamesCont::get_host_name(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_HOST;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax, MessContSyntax);
	if (i != (NB_SEP_HOST - 1))
	    pos++;
	if (i == (NB_SEP_HOST - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to return the device server program number from the record content
unsigned long NdbmNamesCont::get_p_num(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_PN;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_PN - 1))
	    pos++;
	if (i == (NB_SEP_PN - 2))
	    start = pos;
    }
    stringstream 		st;
    st << str.substr(start, pos - start);
    unsigned long 	pn;
    st >> pn;
    return pn;
}

// Method to return the device server version number from the record content

unsigned long NdbmNamesCont::get_v_num(void) const
{
    string::size_type 	pos = 0,
			start;
	
    for (long i = 0;i < NB_SEP_VN;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_VN - 1))
	    pos++;
	if (i == (NB_SEP_VN - 2))
	    start = pos;
    }
	
    stringstream 	st;
    st << str.substr(start, pos - start);
    long 	vn;
    st >> vn;
    return vn;
}

// Method to return the device type from the record content
string NdbmNamesCont::get_device_type(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_TYPE;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_TYPE - 1))
	    pos++;
	if (i == (NB_SEP_TYPE - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to return the device class from the record content
string NdbmNamesCont::get_device_class() const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_CLASS;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_CLASS - 1))
	    pos++;
	if (i == (NB_SEP_CLASS - 2))
	    start = pos;
    }
    return  str.substr(start, pos - start);
}

// Method to return the device server process PID from the record content
unsigned long NdbmNamesCont::get_pid() const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_PID;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_PID - 1))
	    pos++;
	if (i == (NB_SEP_PID - 2))
	    start = pos;
    }
    stringstream 	st;
    unsigned long	p;
    st << str.substr(start, pos - start);
    st >> p;
}

// Method to return the device server process name from the record content
string NdbmNamesCont::get_process_name(void) const
{
    string::size_type pos;
	
    if ((pos = str.find_last_of(SEP,str.size() - 2)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
    pos++;
    return str.substr(pos,( str.size() - 1) - pos);
}

// Method to return the device name domain part from the record content
string NdbmNamesCont::get_dev_domain_name(void) const
{
   try
   {
	string s = this->get_device_name();
        string::size_type pos;
	if ((pos = s.find(SEP_DEV)) == string::npos)
	    throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	return s.substr(0, pos);	
   }
   catch (NdbmError &err)
   {
	throw;
   }
}

// Method to return the device name family part from the record content
string NdbmNamesCont::get_dev_fam_name(void) const
{
    try
    {
	string s = this->get_device_name();
	string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_DEV_FAM;i++)
	{
	    if ((pos = s.find(SEP_DEV,pos)) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	    if (i != (NB_SEP_DEV_FAM - 1))
		pos++;
	    if (i == (NB_SEP_DEV_FAM - 2))
		start = pos;
	}
	return s.substr(start, pos - start);
    }
    catch (NdbmError &err)
    {
	throw;
    }
}

// Method to return the device name member part from the record content
string NdbmNamesCont::get_dev_memb_name(void) const
{
    try
    {
	string s = this->get_device_name();
	string::size_type pos;
	if ((pos = s.find_last_of(SEP_DEV,s.size())) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	pos++;
	return s.substr(pos, (s.size() - pos));
    }
    catch (NdbmError &err)
    {
	throw;
    }
}

// Method to return all the parameters from the record content necessary for
// the devinfo call
void NdbmNamesCont::get_devinfo(db_devinfo_svc &data)
{
    strcpy(data.host_name, this->get_host_name().c_str());
    strcpy(data.device_class, this->get_device_class().c_str());
    strcpy(data.process_name, this->get_process_name().c_str());	
    data.pid = this->get_pid();
    data.server_version = this->get_v_num();
    data.program_num = this->get_p_num();
    unsigned long v = this->get_v_num();
    if ((data.program_num == 0) && (v == 0))
	data.device_exported = false;
    else
	data.device_exported = true;	
    return;
}

void NdbmNamesCont::get_devinfo(db_poller_svc &data)
{
    strcpy(data.host_name,this->get_host_name().c_str());
    strcpy(data.process_name,this->get_process_name().c_str());	
    data.pid = this->get_pid();	
    return;
}

// Method to update the already stored content as a unregister device
void NdbmNamesCont::unreg()
{
    string::size_type 	start;
    if ((start = str.find(SEP)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
    start++;
    str.replace(start, str.size() - start, "not_exp|0|0|unknown|unknown|0|unknown|");	
}

//****************************************************************************
//
//		NdbmPSNamesKey class 
//		--------------
//
//	This class is used to manage PSNAMES table key part. It is constructed
// 	from the NDBM key. Within the pseudo PSNAMES table, the key is :
//		Pseudo device name
//
// 	The record content is :
//		Host name
//		Process PID
//		Refresh period
//
//****************************************************************************

//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmPSNamesKey::NdbmPSNamesKey()
{
}

// The class destructor

NdbmPSNamesKey::~NdbmPSNamesKey()
{
}

// Class constructor to be used with the record key

NdbmPSNamesKey::NdbmPSNamesKey(datum key)
{
    if (key.dptr != NULL)
	str = string(key.dptr, key.dsize);
    else
	throw NdbmError(DbErr_CantBuildKey,MessBuildKey);
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one key character
char NdbmPSNamesKey::operator [] (long i)
{
    try
    {
	return(str.at(i));
    }
    catch(const out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
    }
}

//
//		Class methods
//		-------------
// Method to return the pseudo device name domain part from the record key
string NdbmPSNamesKey::get_psdev_domain_name(void) const
{
    string::size_type pos;
	
    if ((pos = str.find(SEP_DEV)) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
    return str.substr(0, pos + 1);
}

// Method to return the pseudo device name family part from the key content
string NdbmPSNamesKey::get_psdev_fam_name(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_DEV_FAM; i++)
    {
	if ((pos = str.find(SEP_DEV,pos)) == string::npos)
	    throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	if (i != (NB_SEP_DEV_FAM - 1))
	    pos++;
	if (i == (NB_SEP_DEV_FAM - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to return the pseudo device name member part from the key content
string NdbmPSNamesKey::get_psdev_memb_name(void) const
{
    string::size_type pos;
	
    if ((pos = str.find_last_of(SEP_DEV,str.size())) == string::npos)
	throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
    pos++;
    return str.substr(pos,(str.size() - pos));
}

//****************************************************************************
//
//		NdbmPSNamesCont class
//		---------------
//
//	This class is used to manage PSNAMES table content part. It is 
//	constructed from the NDBM key. Within the pseudo PSNAMES table, 
//	the key is :
//		Pseudo device name
//
// 	The record content is :
//		Host name
//		Process PID
//		Refresh period
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmPSNamesCont::NdbmPSNamesCont()
{
}

// The class destructor

NdbmPSNamesCont::~NdbmPSNamesCont()
{
}

// Class constructor to be used with the record key

NdbmPSNamesCont::NdbmPSNamesCont(GDBM_FILE db, datum key)
{
    datum content;

    content = gdbm_fetch(db,key);
    if (content.dptr != NULL)
	str = string(content.dptr, content.dsize);
    else
	throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//
//		Class operator overloading
//		--------------------------

// [] operator overloading. It returns one content character
char NdbmPSNamesCont::operator [] (long i)
{
    try
    {
	return(str.at(i));
    }
    catch(const out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
    }
}

//
//		Class methods
//		-------------
// Method to return the host name from the record content
string NdbmPSNamesCont::get_host_name(void) const
{
    string::size_type pos;

    if ((pos = str.find(SEP)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
    return str.substr(0, pos);	
}

// Method to return the process PID from the record content
unsigned long NdbmPSNamesCont::get_pid(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_PS_PID;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	if (i != (NB_SEP_PS_PID - 1))
	    pos++;
	if (i == (NB_SEP_PS_PID - 2))
	    start = pos;
    }
    
    stringstream st;
    st << str.substr(start, pos - start);
    unsigned long 	p;
    st >> p;
    return p;
}

// Method to return the refresh period from the record content
long NdbmPSNamesCont::get_refresh(void) const
{
    string::size_type pos;
	
    if ((pos = str.find_last_of(SEP,str.size() - 2)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
    pos++;
    stringstream st;
    st << str.substr(pos, (str.size() - 1) - pos);
    long	p;
    st >> p;
    return p;
}

// Method to return all the parameters from the record content necessary for
// the devinfo call

void NdbmPSNamesCont::get_devinfo(db_devinfo_svc &data)
{
    strcpy(data.host_name,this->get_host_name().c_str());	
    data.pid = this->get_pid();
    data.device_class[0] = '\0';
    data.server_name[0] = '\0';
    data.personal_name[0] = '\0';
    data.process_name[0] = '\0';
    data.server_version = 0;
    data.device_exported = False;
}


//****************************************************************************
//
//		NdbmResKey class
//		----------
//
//	This class is used to manage a resource table key part. It is 
//      constructed from the NDBM key. Within a resource table, the key is :
//		Resource family  name
//		Resource member name
//		Resource name
//		Resource index
//
// 	The record content is :
//		Resource value
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor

NdbmResKey::NdbmResKey()
{
    key.dptr = NULL;
    key.dsize = 0;
}

// The class destructor

NdbmResKey::~NdbmResKey()
{
    if (key.dsize != 0)
	delete [] key.dptr;
}

// Class constructor to be used from individual element
NdbmResKey::NdbmResKey(string &family,string &member,string &r_name,long indi=1)
{
    try
    {
//
// Allocate memory to store key
//
	key.dptr = new char[MAX_KEY];
//
// Build intermediate key
//
	inter_str = family + '|' + member + '|' + r_name + '|';
//
// Build key
//
#if !HAVE_SSTREAM
	stringstream to(key.dptr, MAX_KEY);
#else
	stringstream to(string(key.dptr, MAX_KEY));
#endif
	to << inter_str << indi << '|' << ends;
#if !HAVE_SSTREAM
	key.dsize = strlen(to.str());
	to.freeze(false);
#else
	key.dsize = strlen(to.str().c_str());
#endif
    }
    catch (bad_alloc)
    {
	throw;
    }
}


// Class constructor to be used from a already build key as a string

NdbmResKey::NdbmResKey(string &key_str)
{

    try
    {
//
// Allocate memory to store key
//
	key.dptr = new char[MAX_KEY];
//
// Build intermediate key
//
	string::size_type pos = key_str.find_last_of('|',key_str.size() - 2);
	inter_str =key_str.substr(0, pos + 1); 
//
// Build key
//
#if !HAVE_SSTREAM
	stringstream to(key.dptr, MAX_KEY);
#else
	stringstream to(string(key.dptr, MAX_KEY));
#endif
	to << key_str << ends;
#if !HAVE_SSTREAM
	key.dsize = strlen(to.str());
	to.freeze(false);
#else
	key.dsize = strlen(to.str().c_str());
#endif
	str = key_str;	
    }
    catch (bad_alloc)
    {
	throw;
    }
}

	// Class constructor to be used with the record key
NdbmResKey::NdbmResKey(datum user_key)
{
    if (user_key.dptr != NULL)
	str = string(user_key.dptr, user_key.dsize);
    else
	throw NdbmError(DbErr_CantBuildKey,MessBuildKey);
			
    key.dsize = 0;
    key.dptr = NULL;
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one key character
char NdbmResKey::operator [] (long i)
{
    try
    {
	return(str.at(i));
    }
    catch(const out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
    }
}

//
//		Class methods
//
// Method to build a datum data from the already stored content
void NdbmResKey::build_datum()
{
    try
    {
	long l = str.size();
	if (l != 0)
	{
	    key.dptr = new char[l + 1];
	    strcpy(key.dptr, str.c_str());
	    key.dsize = l;
	}
    }
    catch (bad_alloc)
    {
	throw;
    }
}


// Method to increment the key index part

void NdbmResKey::upd_indi(long ind)
{
//
// Build the new key
//
#if !HAVE_SSTREAM
    stringstream to(key.dptr, MAX_KEY);
#else
    stringstream to(string(key.dptr, MAX_KEY));
#endif
    to << inter_str << ind << '|' << ends;
#if !HAVE_SSTREAM
    key.dsize = strlen(to.str());
    to.freeze(false);
#else
    key.dsize = strlen(to.str().c_str());
#endif
}

// Method to retrieve resource family name
string NdbmResKey::get_res_fam_name(void) const
{
    string::size_type pos;
    if ((pos = str.find(SEP)) == string::npos)
	throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
    return str.substr(0, pos);	
}

// Method to retrieve resource member name
string NdbmResKey::get_res_memb_name(void) const
{
    string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_RES_MEMB;i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	     throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
	if (i != (NB_SEP_RES_MEMB - 1))
	     pos++;
	if (i == (NB_SEP_RES_MEMB - 2))
	     start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to retrieve resource name
string NdbmResKey::get_res_name(void) const
{
    string::size_type 	pos = 0,
			start;
	
    for (long i = 0; i < NB_SEP_RES_NAME; i++)
    {
	if ((pos = str.find(SEP,pos)) == string::npos)
	    throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
	if (i != (NB_SEP_RES_NAME - 1))
	    pos++;
	if (i == (NB_SEP_RES_NAME - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

// Method to retrieve resource index (in case of resource from the array type)
long NdbmResKey::get_res_indi(void) const
{
    string::size_type pos;
	
    if ((pos = str.find_last_of(SEP,str.size() - 2)) == string::npos)
	throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
    pos++;
    stringstream st;
    st << str.substr(pos, (str.size() - 1) - pos);
    unsigned long	indi;
    st >> indi;
    return indi;
}


//****************************************************************************
//
//		NdbmResCont class
//		-----------
//
//	This class is used to manage a resource table content part. It is 
//      constructed from the NDBM key. Within a resource table, the key is :
//		Resource family  name
//		Resource member name
//		Resource name
//		Resource index
//
// 	The record content is :
//		Resource value
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmResCont::NdbmResCont()
{
}

// The class destructor

NdbmResCont::~NdbmResCont()
{
}

// Class constructor to be used with the record key

NdbmResCont::NdbmResCont(GDBM_FILE db, datum key)
{
    datum content;

    content = gdbm_fetch(db,key);
    if (content.dptr != NULL)
	str = string(content.dptr, content.dsize);
    else
	throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one content character
char NdbmResCont::operator [] (long i)
{
    try
    {
	return(str.at(i));
    }
    catch(const out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
    }
}

//
//		Class methods
//		-------------
// Method to return the resource value
string NdbmResCont::get_res_value(void) const
{
    return str;
}


//****************************************************************************
//		
//		NdbmNameList class
//		------------
//
//	This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmNameList::NdbmNameList()
{
}

// The class destructor
NdbmNameList::~NdbmNameList()
{
}

//
//		Class operator overloading
//		--------------------------
// [] operator overloading. It returns one element of the list
string &NdbmNameList::operator [] (long i)
{
    try
    {
	return(name_list[i]);
    }
    catch(...)
    {
	throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
    }
}

//
//		Class methods
//		-------------

// This method add a name to the list if it is not already in the list

void NdbmNameList::add_if_new(const string &na)
{
    vector<string>::iterator p = find(name_list.begin(),name_list.end(),na);
    if (p == name_list.end())
	name_list.push_back(na);
    return;
}


// This method sorts the list (alphabetically)

void NdbmNameList::sort_name()
{
    sort(name_list.begin(), name_list.end());
}

// This method copy the vector to a C array allocated in this method

long NdbmNameList::copy_to_C(char **&buf)
{
    long length = name_list.size();
    try
    {
	buf = new char *[length];
		
	for (long i = 0;i < length;i++)
	{
	    buf[i] = new char [name_list[i].size() + 1];
	    name_list[i].copy(buf[i], string::npos);
	    (buf[i])[name_list[i].size()] = '\0';
	}
    	return(0);	
    }
    catch (bad_alloc)
    {
	for (long j = 0; j < length; j++)
	    delete [] buf[j];		
	delete [] buf;
	return(-1);
    }
}
//****************************************************************************
//		
//		NdbmDoubleNameList class
//		------------------
//
//	This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmDoubleNameList::NdbmDoubleNameList()
{
}

// The class destructor
NdbmDoubleNameList::~NdbmDoubleNameList()
{
}

//
//		Class methods
//		-------------
void NdbmDoubleNameList::get_record(const long first, const long second, string &f_str, string &s_str)
{
    f_str = first_list[first];
    s_str = sec_list[first][second];
}

long NdbmDoubleNameList::sec_name_length(long first)
{
    return(sec_list[first].size());
}

// This method add a name to the list if it is not already in the list
void NdbmDoubleNameList::add(string &first,string &second)
{
    long i;
    long si = first_list.size();
		
    for (i = 0;i < si;i++)
	if (first_list[i] == first)
	    break;
	
    if (i == si)
    {
	first_list.push_back(first);
	NdbmNameList tmp_list;		
	tmp_list.add_if_new(second);
	sec_list.push_back(tmp_list);
    }
    else
	sec_list[i].add_if_new(second);
    return;		
}

//****************************************************************************
//		
//		NdbmDomain class
//		----------
//
//	This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmDomain::NdbmDomain()
{
    nb = 0;
}

// Another constructor from the domain name
NdbmDomain::NdbmDomain(const string &str)
{
    name = str;
    nb = 1;
}

// The last constructor from the domain name and the elt number
NdbmDomain::NdbmDomain(const string &str,const long n) 
	: name(str),
	  nb(n)
{
}

// The class destructor
NdbmDomain::~NdbmDomain()
{
}

//
//		Class operator overloading
//		--------------------------
// == operator overloading. Used by the standard find algorithms
bool operator== (const NdbmDomain &a, const NdbmDomain &b)
{
    return (a.name == b.name);
}


// < operator overloading. Used by the standard sort algorithms


bool operator< (const NdbmDomain &a, const NdbmDomain &b)
{
    return (a.name < b.name);
}

//****************************************************************************
//		
//		NdbmDomDev class
//		----------
//
//	This class is used to manage a list of family/mmeber associated to a
//  domain. This is used by the server for the db_deviceres and db_deldeviceres
//  calls.
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmDomDev::NdbmDomDev()
{
}

// The last constructor from the domain name and a family/member string
NdbmDomDev::NdbmDomDev(const string &dom,const string &str) : domain(dom)
{
    fm.push_back(str);
}

// The class destructor
NdbmDomDev::~NdbmDomDev()
{
}

void NdbmDomDev::add_dev(const string &dev)
{
    fm.push_back(dev);
}

string NdbmDomDev::get_domain()
{
    return(domain);
}

string NdbmDomDev::get_fm(long ind)
{
    try
    {
	return(fm[ind]);
    }
    catch (...)
    {
	return string("");
    }
}

long NdbmDomDev::find_in_list(const string &str)
{
    vector<string>::iterator p = find(fm.begin(), fm.end(), str);
    return (p != fm.end());
}

//
//		Class operator overloading
//		--------------------------
// == operator overloading. Used by the standard find algorithms
bool operator== (const NdbmDomDev &a, const NdbmDomDev &b)
{
    return (a.domain == b.domain);
}



//****************************************************************************
//		
//		NdbmSvcDev class
//		----------
//
//	This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
//****************************************************************************

//
//		Class constructor and destructor
//		--------------------------------
// The class default constuctor
NdbmSvcDev::NdbmSvcDev()
	: flag(false)
{
}

// Another constructor from a NAMES table content
NdbmSvcDev::NdbmSvcDev(const NdbmNamesCont &co)
{
    unsigned long 	pn = co.get_p_num(),
			vn = co.get_v_num();
	
    name = co.get_device_name();
    if ((pn != 0) && (vn != 0))
	flag = true;
    else
	flag = false;
}

// The class destructor
NdbmSvcDev::~NdbmSvcDev()
{
}

//****************************************************************************
//
//		NdbmError class
//		---------
//
//	This class is used for error management.
//
//****************************************************************************
//
//		Class constructor and destructor
//		--------------------------------
// Contructor from the error code and error message (with default value)

NdbmError::NdbmError(long err, char *mess = "No error message defined")
 :errcode(err)
{
    strcpy(errmess,mess);
}

//
//		Class methods
//		-------------
// Method to retrieve the error code
long NdbmError::get_err_code()
{
    return(errcode);
}

// Method to retrieve the error message
char *NdbmError::get_err_message()
{
    return(errmess);
}

// Method to display the error message
void NdbmError::display_err_message()
{
    cout << errmess << endl;
}
