#include <cstdlib>

#include <macros.h>
#include <db_setup.h>

// C++ include

#include <NdbmClass.h>
#include <new>
#include <string>
#include <algorithm>
#include <string>
#include <iostream>
#include <strstream>


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
	str = NULL;
	key.dptr = NULL;
	key.dsize = 0;
}

// The class destructor

NdbmNamesKey::~NdbmNamesKey()
{
	if (str != NULL)
		delete str;
	if (key.dsize != 0)
		delete [] key.dptr;
}

// Class constructor to be used with the record key

NdbmNamesKey::NdbmNamesKey(datum call_key)
{
	if (call_key.dptr != NULL)
	{
		str = new string(call_key.dptr,call_key.dsize);
	}
	else
		throw NdbmError(DbErr_CantBuildKey,MessBuildKey);
		
	key.dsize = 0;
	key.dptr = NULL;
}

// Class constructor to be used from individual element

NdbmNamesKey::NdbmNamesKey(string &server,string &pers_name,long indi)
{

//
// Allocate memory to store key
//

	try
	{
		key.dptr = new char[MAX_KEY];
	}
	catch (bad_alloc)
	{
		throw;
	}

//
// Build intermediate key
//

	inter_str = server + '|' + pers_name + '|';
	
//
// Build key
//

	ostrstream to(key.dptr,MAX_KEY);
	
	to << inter_str << indi << '|' << ends;
	key.dsize = to.pcount() - 1;
		
	str = NULL;	
}

//
//		Class operator overloading
//		--------------------------

// [] operator overloading. It returns one key character

char NdbmNamesKey::operator [] (long i)
{
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//		-------------

// Method to build a datum data from the already stored content

void NdbmNamesKey::build_datum()
{
	long l = str->size();
	if (l != 0)
	{
		try
		{
			key.dptr = new char[l + 1];
		}
		catch (bad_alloc)
		{
			throw;
		}

		strcpy(key.dptr,str->c_str());
		key.dsize = l;
	}
}

// Method to return the device server name

void NdbmNamesKey::get_ds_name(string &dsname)
{
	string::size_type pos;

	if ((pos = str->find(SEP)) == string::npos)
		throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
	
	string tmp_str(*str,0,pos);	
	dsname = tmp_str;
}

// Method to return the DS personal name

void NdbmNamesKey::get_ds_pers_name(string &persname)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_DS_PERS;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
		if (i != (NB_SEP_DS_PERS - 1))
			pos++;
		if (i == (NB_SEP_DS_PERS - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	persname = tmp_str;
}

// Method to return the device index in the DS device list

void NdbmNamesKey::get_dev_indi(long &indi)
{
	string::size_type pos;
	
	if ((pos = str->find_last_of(SEP,str->size() - 2)) == string::npos)
		throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
	pos++;
	
	string tmp_str(*str,pos,(str->size() - 1) - pos);
		
	strstream st;
	st << tmp_str;
	st >> indi;
}

// Method to increment the key index part

void NdbmNamesKey::upd_indi(long ind)
{

//
// Build the new key
//

	ostrstream to(key.dptr,MAX_KEY);
	
	to << inter_str << ind << '|' << ends;
	key.dsize = to.pcount() - 1;
}

// Method to return all the parameters from the record key necessary for
// the devinfo call

void NdbmNamesKey::get_devinfo(db_devinfo_svc &data)
{
	string s;
	
	this->get_ds_name(s);
	strcpy(data.server_name,s.c_str());
	
	this->get_ds_pers_name(s);
	strcpy(data.personal_name,s.c_str());	
}

void NdbmNamesKey::get_devinfo(db_poller_svc &data)
{
	string s;
	
	this->get_ds_name(s);
	strcpy(data.server_name,s.c_str());
	
	this->get_ds_pers_name(s);
	strcpy(data.personal_name,s.c_str());	
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
	str = NULL;
	dat.dptr = NULL;
	dat.dsize = 0;
}

// The class destructor

NdbmNamesCont::~NdbmNamesCont()
{
	if (str != NULL)
		delete str;
		
	if (dat.dsize != 0)
		delete [] dat.dptr;
}

// Class constructor to be used with the record key

NdbmNamesCont::NdbmNamesCont(GDBM_FILE db, datum key)
{
	datum content;

	content = gdbm_fetch(db,key);
	if (content.dptr != NULL)
	{
		str = new string(content.dptr,content.dsize);
	}
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
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//		-------------

// Method to build a datum data from the already stored content

void NdbmNamesCont::build_datum()
{
	long l = str->size();
	if (l != 0)
	{
		try
		{
			dat.dptr = new char[l + 1];
		}
		catch (bad_alloc)
		{
			throw;
		}

		strcpy(dat.dptr,str->c_str());
		dat.dsize = l;
	}
}

// Method to return the device name from the record content

void NdbmNamesCont::get_device_name(string &devname) const
{
	string::size_type pos;

	if ((pos = str->find(SEP)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	
	string tmp_str(*str,0,pos);	
	devname = tmp_str;
}

// Method to return the host name from the record content

void NdbmNamesCont::get_host_name(string &host)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_HOST;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_HOST - 1))
			pos++;
		if (i == (NB_SEP_HOST - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	host = tmp_str;
}

// Method to return the device server program number from the record content

void NdbmNamesCont::get_p_num(unsigned long &pn) const
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_PN;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PN - 1))
			pos++;
		if (i == (NB_SEP_PN - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	
	strstream st;
	st << tmp_str;
	st >> pn;
}

// Method to return the device server version number from the record content

void NdbmNamesCont::get_v_num(unsigned long &vn) const
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_VN;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_VN - 1))
			pos++;
		if (i == (NB_SEP_VN - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	
	strstream st;
	st << tmp_str;
	st >> vn;
}

// Method to return the device type from the record content

void NdbmNamesCont::get_device_type(string &type)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_TYPE;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_TYPE - 1))
			pos++;
		if (i == (NB_SEP_TYPE - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	type = tmp_str;
}

// Method to return the device class from the record content

void NdbmNamesCont::get_device_class(string &clas)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_CLASS;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_CLASS - 1))
			pos++;
		if (i == (NB_SEP_CLASS - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	clas = tmp_str;
}

// Method to return the device server process PID from the record content

void NdbmNamesCont::get_pid(unsigned long &p)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_PID;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PID - 1))
			pos++;
		if (i == (NB_SEP_PID - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	
	strstream st;
	st << tmp_str;
	st >> p;
}

// Method to return the device server process name from the record content

void NdbmNamesCont::get_process_name(string &p_name)
{
	string::size_type pos;
	
	if ((pos = str->find_last_of(SEP,str->size() - 2)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	
	string tmp_str(*str,pos,(str->size() - 1) - pos);
	p_name = tmp_str;
}

// Method to return the device name domain part from the record content

void NdbmNamesCont::get_dev_domain_name(string &domain)
{
	string s;
	string::size_type pos;

	try
	{
		this->get_device_name(s);
	}
	catch (NdbmError &err)
	{
		throw;
	}

	if ((pos = s.find(SEP_DEV)) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	
	string tmp_str(s,0,pos);	
	domain = tmp_str;
}

// Method to return the device name family part from the record content

void NdbmNamesCont::get_dev_fam_name(string &family)
{
	string s;
	string::size_type pos,start;
	long i;

	try
	{
		this->get_device_name(s);
	}
	catch (NdbmError &err)
	{
		throw;
	}

	pos = 0;
	for (i = 0;i < NB_SEP_DEV_FAM;i++)
	{
		if ((pos = s.find(SEP_DEV,pos)) == string::npos)
			throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
		if (i != (NB_SEP_DEV_FAM - 1))
			pos++;
		if (i == (NB_SEP_DEV_FAM - 2))
			start = pos;
	}
	
	string tmp_str(s,start,pos - start);
	family = tmp_str;
}

// Method to return the device name member part from the record content

void NdbmNamesCont::get_dev_memb_name(string &member)
{
	string s;
	string::size_type pos;

	try
	{
		this->get_device_name(s);
	}
	catch (NdbmError &err)
	{
		throw;
	}
	
	if ((pos = s.find_last_of(SEP_DEV,s.size())) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	pos++;
	
	string tmp_str(s,pos,(s.size() - pos));
	member = tmp_str;
}

// Method to return all the parameters from the record content necessary for
// the devinfo call

void NdbmNamesCont::get_devinfo(db_devinfo_svc &data)
{
	string s;
	unsigned long p,v;
	
	this->get_host_name(s);
	strcpy(data.host_name,s.c_str());
	
	this->get_device_class(s);
	strcpy(data.device_class,s.c_str());
	
	this->get_process_name(s);
	strcpy(data.process_name,s.c_str());	
	
	this->get_pid(data.pid);

	this->get_v_num(data.server_version);
	
	this->get_p_num(data.program_num);
	this->get_v_num(v);
	if ((data.program_num == 0) && (v == 0))
		data.device_exported = False;
	else
		data.device_exported = True;	
}

void NdbmNamesCont::get_devinfo(db_poller_svc &data)
{
	string s;
	unsigned long p,v;
	
	this->get_host_name(s);
	strcpy(data.host_name,s.c_str());
	
	this->get_process_name(s);
	strcpy(data.process_name,s.c_str());	
	
	this->get_pid(data.pid);	
}

// Method to update the already stored content as a unregister device

void NdbmNamesCont::unreg()
{
	string::size_type end,start;
	
	if ((start = str->find(SEP)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	start++;
	
	str->replace(start,str->size() - start,"not_exp|0|0|unknown|unknown|0|unknown|");	
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
	str = NULL;
}

// The class destructor

NdbmPSNamesKey::~NdbmPSNamesKey()
{
	if (str != NULL)
		delete str;
}

// Class constructor to be used with the record key

NdbmPSNamesKey::NdbmPSNamesKey(datum key)
{
	if (key.dptr != NULL)
	{
		str = new string(key.dptr,key.dsize);
	}
	else
		throw NdbmError(DbErr_CantBuildKey,MessBuildKey);
}

//
//		Class operator overloading
//		--------------------------

// [] operator overloading. It returns one key character

char NdbmPSNamesKey::operator [] (long i)
{
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//		-------------

// Method to return the pseudo device name domain part from the record key

void NdbmPSNamesKey::get_psdev_domain_name(string &domain)
{
	string::size_type pos;
	
	if ((pos = str->find(SEP_DEV)) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
		
	string tmp_str(*str,0,pos);
	domain = tmp_str;
}

// Method to return the pseudo device name family part from the key content

void NdbmPSNamesKey::get_psdev_fam_name(string &family)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_DEV_FAM;i++)
	{
		if ((pos = str->find(SEP_DEV,pos)) == string::npos)
			throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
		if (i != (NB_SEP_DEV_FAM - 1))
			pos++;
		if (i == (NB_SEP_DEV_FAM - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	family = tmp_str;
}

// Method to return the pseudo device name member part from the key content

void NdbmPSNamesKey::get_psdev_memb_name(string &member)
{
	string::size_type pos;
	
	if ((pos = str->find_last_of(SEP_DEV,str->size())) == string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	pos++;
	
	string tmp_str(*str,pos,(str->size() - pos));
	member = tmp_str;
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
	str = NULL;
}

// The class destructor

NdbmPSNamesCont::~NdbmPSNamesCont()
{
	if (str != NULL)
		delete str;
}

// Class constructor to be used with the record key

NdbmPSNamesCont::NdbmPSNamesCont(GDBM_FILE db, datum key)
{
	datum content;

	content = gdbm_fetch(db,key);
	if (content.dptr != NULL)
	{
		str = new string(content.dptr,content.dsize);
	}
	else
		throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//
//		Class operator overloading
//		--------------------------

// [] operator overloading. It returns one content character
char NdbmPSNamesCont::operator [] (long i)
{
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//		-------------

// Method to return the host name from the record content

void NdbmPSNamesCont::get_host_name(string &host)
{
	string::size_type pos;

	if ((pos = str->find(SEP)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	
	string tmp_str(*str,0,pos);	
	host = tmp_str;
}

// Method to return the process PID from the record content

void NdbmPSNamesCont::get_pid(unsigned long &p)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_PS_PID;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PS_PID - 1))
			pos++;
		if (i == (NB_SEP_PS_PID - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	
	strstream st;
	st << tmp_str;
	st >> p;
}

// Method to return the refresh period from the record content

void NdbmPSNamesCont::get_refresh(long &p)
{
	string::size_type pos;
	
	if ((pos = str->find_last_of(SEP,str->size() - 2)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	
	string tmp_str(*str,pos,(str->size() - 1) - pos);
		
	strstream st;
	st << tmp_str;
	st >> p;
}

// Method to return all the parameters from the record content necessary for
// the devinfo call

void NdbmPSNamesCont::get_devinfo(db_devinfo_svc &data)
{
	string s;
	
	this->get_host_name(s);
	strcpy(data.host_name,s.c_str());	
	
	this->get_pid(data.pid);
	
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
	str = NULL;
	
	key.dptr = NULL;
	key.dsize = 0;
}

// The class destructor

NdbmResKey::~NdbmResKey()
{
	if (str != NULL)
		delete str;
	if (key.dsize != 0)
		delete [] key.dptr;
}

// Class constructor to be used from individual element

NdbmResKey::NdbmResKey(string &family,string &member,string &r_name,long indi=1)
{

//
// Allocate memory to store key
//

	try
	{
		key.dptr = new char[MAX_KEY];
	}
	catch (bad_alloc)
	{
		throw;
	}

//
// Build intermediate key
//

	inter_str = family + '|' + member + '|' + r_name + '|';
	
//
// Build key
//

	ostrstream to(key.dptr,MAX_KEY);
	
	to << inter_str << indi << '|' << ends;
	key.dsize = to.pcount() - 1;
		
	str = NULL;	
}


// Class constructor to be used from a already build key as a string

NdbmResKey::NdbmResKey(string &key_str)
{
	string::size_type pos;

//
// Allocate memory to store key
//

	try
	{
		key.dptr = new char[MAX_KEY];
	}
	catch (bad_alloc)
	{
		throw;
	}

//
// Build intermediate key
//

	pos = key_str.find_last_of('|',key_str.size() - 2);
	string tmp(key_str,0,pos + 1);
	inter_str = tmp;
	
//
// Build key
//

	ostrstream to(key.dptr,MAX_KEY);
	
	to << key_str << ends;
	key.dsize = to.pcount() - 1;
		
	str = new string(key_str);	
}


// Class constructor to be used with the record key

NdbmResKey::NdbmResKey(datum user_key)
{
	if (user_key.dptr != NULL)
	{
		str = new string(user_key.dptr,user_key.dsize);
	}
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
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//
		
// Method to build a datum data from the already stored content

void NdbmResKey::build_datum()
{
	long l = str->size();
	if (l != 0)
	{
		try
		{
			key.dptr = new char[l + 1];
		}
		catch (bad_alloc)
		{
			throw;
		}

		strcpy(key.dptr,str->c_str());
		key.dsize = l;
	}
}


// Method to increment the key index part

void NdbmResKey::upd_indi(long ind)
{

//
// Build the new key
//

	ostrstream to(key.dptr,MAX_KEY);
	
	to << inter_str << ind << '|' << ends;
	key.dsize = to.pcount() - 1;
}

// Method to retrieve resource family name

void NdbmResKey::get_res_fam_name(string &family)
{
	string::size_type pos;

	if ((pos = str->find(SEP)) == string::npos)
		throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
	
	string tmp_str(*str,0,pos);	
	family = tmp_str;
}

// Method to retrieve resource member name

void NdbmResKey::get_res_memb_name(string &member)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_RES_MEMB;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
		if (i != (NB_SEP_RES_MEMB - 1))
			pos++;
		if (i == (NB_SEP_RES_MEMB - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	member = tmp_str;
}

// Method to retrieve resource name

void NdbmResKey::get_res_name(string &r_name)
{
	string::size_type pos,start;
	long i;
	
	pos = 0;
	for (i = 0;i < NB_SEP_RES_NAME;i++)
	{
		if ((pos = str->find(SEP,pos)) == string::npos)
			throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
		if (i != (NB_SEP_RES_NAME - 1))
			pos++;
		if (i == (NB_SEP_RES_NAME - 2))
			start = pos;
	}
	
	string tmp_str(*str,start,pos - start);
	r_name = tmp_str;
}

// Method to retrieve resource index (in case of resource from the array type)

void NdbmResKey::get_res_indi(long &indi)
{
	string::size_type pos;
	
	if ((pos = str->find_last_of(SEP,str->size() - 2)) == string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	
	string tmp_str(*str,pos,(str->size() - 1) - pos);
		
	strstream st;
	st << tmp_str;
	st >> indi;
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
	str = NULL;
}

// The class destructor

NdbmResCont::~NdbmResCont()
{
	if (str != NULL)
		delete str;
}

// Class constructor to be used with the record key

NdbmResCont::NdbmResCont(GDBM_FILE db, datum key)
{
	datum content;

	content = gdbm_fetch(db,key);
	if (content.dptr != NULL)
	{
		str = new string(content.dptr,content.dsize);
	}
	else
		throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//
//		Class operator overloading
//		--------------------------

// [] operator overloading. It returns one content character

char NdbmResCont::operator [] (long i)
{
	if (i >= str->size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return((*str)[i]);
}

//
//		Class methods
//		-------------

// Method to return the resource value

void NdbmResCont::get_res_value(string &r_value)
{
	r_value = *str;
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
	if (i >= name_list.size())
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	else
		return(name_list[i]);
}

//
//		Class methods
//		-------------

// This method add a name to the list if it is not already in the list

void NdbmNameList::add_if_new(string &na)
{
	vector<string>::iterator p = find(name_list.begin(),name_list.end(),na);
	
	if (p == name_list.end())
	{
		name_list.push_back(na);
	}
}


// This method sorts the list (alphabetically)

void NdbmNameList::sort_name()
{
	sort(name_list.begin(),name_list.end());
}

// This method copy the vector to a C array allocated in this method

long NdbmNameList::copy_to_C(char **&buf)
{
	long length,i,j;

	i = j = 0;
	length = name_list.size();
	
	try
	{
		buf = new char *[length];
		
		for (i = 0;i < length;i++)
		{
			buf[i] = new char [name_list[i].length() + 1];
			name_list[i].copy(buf[i],string::npos);
			(buf[i])[name_list[i].length()] = '\0';
			
		}
	}
	catch (bad_alloc)
	{
		for (j = 0;j < i;j++)
			delete [] buf[j];		
		if (*buf != NULL)
			delete [] buf;
		return(-1);
	}
	
	return(0);	
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

void NdbmDoubleNameList::get_record(long first,long second,string &f_str,string &s_str)
{
	f_str = first_list[first];
	s_str = sec_list[first][second];
}

long NdbmDoubleNameList::sec_name_length(long first)
{
	return(sec_list[first].length());
}

// This method add a name to the list if it is not already in the list

void NdbmDoubleNameList::add(string &first,string &second)
{
	long i;
	long si = first_list.size();
		
	for (i = 0;i < si;i++)
	{
		if (first_list[i] == first)
			break;
	}
	
	if (i == si)
	{
		first_list.push_back(first);
		
		NdbmNameList tmp_list;		
		tmp_list.add_if_new(second);
		
		sec_list.push_back(tmp_list);
	}
	else
	{
		sec_list[i].add_if_new(second);
	}
		
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

NdbmDomain::NdbmDomain(const string &str,const long &n) : name(str),nb(n)
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
	if (a.name == b.name)
		return(True);
	else
		return(False);
}


// < operator overloading. Used by the standard sort algorithms


bool operator< (const NdbmDomain &a, const NdbmDomain &b)
{
	if (a.name < b.name)
		return(True);
	else
		return(False);
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

string &NdbmDomDev::get_domain()
{
	return(domain);
}

string &NdbmDomDev::get_fm(long ind)
{
	return(fm[ind]);
}

long NdbmDomDev::find_in_list(const string &str)
{
	vector<string>::iterator p = find(fm.begin(),fm.end(),str);
				
	if (p == fm.end())
		return(False);
	else
		return(True);
}

//
//		Class operator overloading
//		--------------------------

// == operator overloading. Used by the standard find algorithms

bool operator== (const NdbmDomDev &a, const NdbmDomDev &b)
{
	if (a.domain == b.domain)
		return(True);
	else
		return(False);
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
{
	flag = False;
}

// Another constructor from a NAMES table content

NdbmSvcDev::NdbmSvcDev(const NdbmNamesCont &co)
{
	unsigned long pn,vn;
	
	co.get_device_name(name);
	
	co.get_p_num(pn);
	co.get_v_num(vn);
	
	if ((pn != 0) && (vn != 0))
		flag = True;
	else
		flag = False;
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

NdbmError::NdbmError(long err,char *mess = "No error message defined")
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
