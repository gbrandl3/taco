/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:
 *
 * Description:
 *
 * Author(s):
 *		$Author: jkrueger1 $
 * 
 * Version:	$Revision: 1.16 $
 *
 * Date:	$Date: 2005-12-15 14:39:56 $
 *
 */
#include <cstdlib>
#include "config.h"
#include <macros.h>
#include <db_setup.h>

// C++ include

#include <NdbmClass.h>
#include <new>
#include <string>
#include <algorithm>
#include <iostream>
#include <stdexcept>

/**@class NdbmNamesKey
// This class is used to manage NAMES table key part. It is constructed
// from the NDBM key. Within the NAMES table, the key is :
//	- Device server name
//	- Device server personal name
//	- Device sequence number in the device server device list
//
// The record content is :
//	- Device name
//	- Host name
//	- Device server program number
//	- Device server version number
//	- Device type
//	- Device class
//	- Device server process PID
//	- Device server process name
 */

//		Class constructor and destructor
//! The class default constuctor
NdbmNamesKey::NdbmNamesKey()
{
	key.dptr = NULL;
	key.dsize = 0;
}

//! The class destructor
NdbmNamesKey::~NdbmNamesKey()
{
	if (key.dsize != 0)
		delete [] key.dptr;
}

//! Class constructor to be used with the record key
NdbmNamesKey::NdbmNamesKey(const datum &call_key)
{
	key.dptr = NULL;
	key.dsize = 0;
	if (call_key.dptr != NULL)
		str = std::string(call_key.dptr, call_key.dsize);
	else
		throw NdbmError(DbErr_CantBuildKey, MessBuildKey);
	build_datum();	
}

//! Class constructor to be used from individual element
NdbmNamesKey::NdbmNamesKey(const std::string &server, const std::string &pers_name, const long indi)
{
//
// Allocate memory to store key
//
	key.dptr = NULL;
	key.dsize = 0;
	try
	{
//
// Build key
//
		std::stringstream to;
		to << server << '|' << pers_name << '|' << indi << '|' << std::ends;
		str = to.str();
#if !HAVE_SSTREAM
		to.freeze(false);
#endif
		build_datum();
	}
	catch (std::bad_alloc)
	{
		throw;
	}
}

//
//		Class operator overloading
//		--------------------------
//! [] operator overloading. It returns one key character
char NdbmNamesKey::operator [](long i)
{
	try
	{
		return(str.at(i));
	}
	catch(const std::out_of_range &)
	{
		throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
	}
}

//
//		Class methods
//		-------------
//! Method to build a datum data from the already stored content
void NdbmNamesKey::build_datum()
{
	long l = strlen(str.c_str());
	if (l)
	{
		try
		{
			if (key.dsize)
				delete [] key.dptr;
			key.dptr = new char[l + 1];
			strncpy(key.dptr,str.c_str(),l);
	                key.dptr[l] = 0; /* NULL terminate string - andy 23nov2005 */
			key.dsize = l;
		}
		catch (std::bad_alloc)
		{
			throw;
		}
	}
}

//! Method to return the device server name
std::string NdbmNamesKey::get_ds_name(void)
{
	std::string::size_type pos = str.find(SEP);
	if (pos == std::string::npos)
		throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
	return str.substr(0, pos);	
}

//! Method to return the DS personal name
std::string  NdbmNamesKey::get_ds_pers_name(void)
{
	std::string::size_type 	pos = 0,
				start;
	
	for (long i = 0; i < NB_SEP_DS_PERS; ++i)
	{
		if ((pos = str.find(SEP, pos)) == std::string::npos)
			throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
		if (i != (NB_SEP_DS_PERS - 1))
			pos++;
		if (i == (NB_SEP_DS_PERS - 2))
			start = pos;
	}
	return str.substr(start, pos - start);
}

//! Method to return the device index in the DS device list
long NdbmNamesKey::get_dev_indi(void)
{
	std::string::size_type pos = str.find_last_of(SEP, str.size() - 2);
	
	if (pos == std::string::npos)
		throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
	pos++;

	std::stringstream st;
	long	indi;
	st << str.substr(pos, (str.size() - 1) - pos);
	st >> indi;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return indi;
}

//! Method to increment the key index part
void NdbmNamesKey::upd_indi(long ind)
{
//
// Build the new key
//
	std::stringstream to;

	to << get_ds_name() << '|' << get_ds_pers_name() << '|' << ind << '|' << std::ends;
	str = to.str();
#if !HAVE_SSTREAM
	to.freeze(false);
#endif
	build_datum();
	return;
}

//! Method to return all the parameters from the record key necessary for the devinfo call
void NdbmNamesKey::get_devinfo(db_devinfo_svc &data)
{
	std::string s = this->get_ds_name();
	strcpy(data.server_name, s.c_str());
	s = this->get_ds_pers_name();
	strcpy(data.personal_name,s.c_str());	
	return;
}

//! Method to return all the parameters from the record key necessary for the devinfo call
void NdbmNamesKey::get_devinfo(db_poller_svc &data)
{
	std::string s = this->get_ds_name();
	strcpy(data.server_name,s.c_str());
	s = this->get_ds_pers_name();
	strcpy(data.personal_name,s.c_str());	
	return;
}

/**@class NdbmNamesCont
// This class is used to manage NAMES table content part. It is constructed
// from the NDBM key. Within the pseudo NAMES table, the key is :
//	- Device server name
//	- Device server personal name
//	- Device sequence number in the device server device list
//
// The record content is :
//	- Device name
//	- Host name
//	- Device server program number
//	- Device server version number
//	- Device type
//	- Device class
//	- Device server process PID
//	- Device server process name
 */

//		Class constructor and destructor
//! The class default constructor
NdbmNamesCont::NdbmNamesCont()
{
	dat.dptr = NULL;
	dat.dsize = 0;
}

//! The class destructor
NdbmNamesCont::~NdbmNamesCont()
{
	if (dat.dsize != 0)
		delete [] dat.dptr;
}

//! Class constructor to be used with the record key
NdbmNamesCont::NdbmNamesCont(GDBM_FILE db, datum call_key)
{
	dat = gdbm_fetch(db, call_key);
	if (dat.dptr != NULL)
		str = std::string(dat.dptr, dat.dsize);
	else
		throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//! [] operator overloading. It returns one content character
char NdbmNamesCont::operator [] (long i)
{
	try
	{
		return(str.at(i));
	}
	catch (const std::out_of_range &)
	{
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	}
}

//! Method to return the device name from the record content
std::string NdbmNamesCont::get_device_name(void) const
{
	std::string::size_type pos = str.find(SEP);

	if (pos == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax, MessContSyntax);
	return str.substr(0, pos);	
}

//! Method to return the host name from the record content
std::string NdbmNamesCont::get_host_name(void) const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_HOST;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadContSyntax, MessContSyntax);
		if (i != (NB_SEP_HOST - 1))
			pos++;
		if (i == (NB_SEP_HOST - 2))
			start = pos;
	}
	return str.substr(start, pos - start);
}

//! Method to return the device server program number from the record content
unsigned long NdbmNamesCont::get_p_num(void) const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_PN;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
	    		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PN - 1))
	    		pos++;
		if (i == (NB_SEP_PN - 2))
	    		start = pos;
	}
	std::stringstream 		st;
	st << str.substr(start, pos - start);
	unsigned long 	pn;
	st >> pn;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return pn;
}

//! Method to return the device server version number from the record content
unsigned long NdbmNamesCont::get_v_num(void) const
{
	std::string::size_type 	pos = 0,
				start;
	
	for (long i = 0;i < NB_SEP_VN;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_VN - 1))
			pos++;
		if (i == (NB_SEP_VN - 2))
			start = pos;
	}
	
	std::stringstream 	st;
	st << str.substr(start, pos - start);
	long 	vn;
	st >> vn;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return vn;
}

//! Method to return the device type from the record content
std::string NdbmNamesCont::get_device_type(void) const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_TYPE;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
	    		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_TYPE - 1))
	    		pos++;
		if (i == (NB_SEP_TYPE - 2))
	    		start = pos;
	}
	return str.substr(start, pos - start);
}

//! Method to return the device class from the record content
std::string NdbmNamesCont::get_device_class() const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_CLASS;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_CLASS - 1))
			pos++;
		if (i == (NB_SEP_CLASS - 2))
	    		start = pos;
	}
	return  str.substr(start, pos - start);
}

//! Method to return the device server process PID from the record content
unsigned long NdbmNamesCont::get_pid() const
{
	std::string::size_type 	pos = 0,
			start;
	for (long i = 0;i < NB_SEP_PID;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PID - 1))
			pos++;
		if (i == (NB_SEP_PID - 2))
			start = pos;
	}
	std::stringstream 	st;
	unsigned long	p;
	st << str.substr(start, pos - start);
	st >> p;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return p;
}

//! Method to return the device server process name from the record content
std::string NdbmNamesCont::get_process_name(void) const
{
	std::string::size_type pos = str.find_last_of(SEP, str.size() - 2);
	
	if (pos == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	return str.substr(pos, (str.size() - 1) - pos);
}

//! Method to return the device name domain part from the record content
std::string NdbmNamesCont::get_dev_domain_name(void) const
{
	try
	{
		std::string s = this->get_device_name();
        	std::string::size_type pos = s.find(SEP_DEV);
		if (pos == std::string::npos)
			throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
		return s.substr(0, pos);	
	}
	catch (NdbmError &err)
	{
		throw;
	}
}

//! Method to return the device name family part from the record content
std::string NdbmNamesCont::get_dev_fam_name(void) const
{
    try
    {
	std::string s = this->get_device_name();
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_DEV_FAM;i++)
	{
	    if ((pos = s.find(SEP_DEV,pos)) == std::string::npos)
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

//! Method to return the device name member part from the record content
std::string NdbmNamesCont::get_dev_memb_name(void) const
{
    try
    {
	std::string s = this->get_device_name();
	std::string::size_type pos;
	if ((pos = s.find_last_of(SEP_DEV,s.size())) == std::string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	pos++;
	return s.substr(pos, (s.size() - pos));
    }
    catch (NdbmError &err)
    {
	throw;
    }
}

//! Method to return all the parameters from the record content necessary for the devinfo call
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
//! Method to  return all the parameters from the record content necessary for the devinfo call
void NdbmNamesCont::get_devinfo(db_poller_svc &data)
{
    strcpy(data.host_name,this->get_host_name().c_str());
    strcpy(data.process_name,this->get_process_name().c_str());	
    data.pid = this->get_pid();	
    return;
}

//! Method to update the already stored content as a unregister device
void NdbmNamesCont::unreg()
{
	std::string::size_type 	start;
	if ((start = str.find(SEP)) == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	start++;
	str.replace(start, str.size() - start, "not_exp|0|0|unknown|unknown|0|unknown|");	
}

/**@class NdbmPSNamesKey
// This class is used to manage PSNAMES table key part. It is constructed
// from the NDBM key. Within the pseudo PSNAMES table, the key is :
//	- Pseudo device name
//
// The record content is :
//	- Host name
//	- Process PID
//	- Refresh period
 */

//! The class default constuctor
NdbmPSNamesKey::NdbmPSNamesKey()
{
}

//! The class destructor
NdbmPSNamesKey::~NdbmPSNamesKey()
{
}

//! Class constructor to be used with the record key
NdbmPSNamesKey::NdbmPSNamesKey(datum call_key)
{
    if (call_key.dptr != NULL)
	str = std::string(call_key.dptr, call_key.dsize);
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
    catch(const std::out_of_range &)
    {
	throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
    }
}

//! Method to return the pseudo device name domain part from the record key
std::string NdbmPSNamesKey::get_psdev_domain_name(void) const
{
	std::string::size_type pos = str.find(SEP_DEV);
	
	if (pos == std::string::npos)
		throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	return str.substr(0, pos);
}

//! Method to return the pseudo device name family part from the key content
std::string NdbmPSNamesKey::get_psdev_fam_name(void) const
{
    std::string::size_type 	pos = 0,
			start;
    for (long i = 0;i < NB_SEP_DEV_FAM; i++)
    {
	if ((pos = str.find(SEP_DEV,pos)) == std::string::npos)
	    throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
	if (i != (NB_SEP_DEV_FAM - 1))
	    pos++;
	if (i == (NB_SEP_DEV_FAM - 2))
	    start = pos;
    }
    return str.substr(start, pos - start);
}

//! Method to return the pseudo device name member part from the key content
std::string NdbmPSNamesKey::get_psdev_memb_name(void) const
{
    std::string::size_type pos;
	
    if ((pos = str.find_last_of(SEP_DEV,str.size())) == std::string::npos)
	throw NdbmError(DbErr_BadDevSyntax,MessDevSyntax);
    pos++;
    return str.substr(pos,(str.size() - pos));
}

/**@class NdbmPSNamesCont
// This class is used to manage PSNAMES table content part. It is 
// constructed from the NDBM key. Within the pseudo PSNAMES table, 
// the key is :
//	- Pseudo device name
//
// The record content is :
//	- Host name
//	- Process PID
//	- Refresh period
 */

//! The class default constuctor
NdbmPSNamesCont::NdbmPSNamesCont()
{
}

//! The class destructor
NdbmPSNamesCont::~NdbmPSNamesCont()
{
}

//! Class constructor to be used with the record key
NdbmPSNamesCont::NdbmPSNamesCont(GDBM_FILE db, datum call_key)
{
	datum content;

	content = gdbm_fetch(db,call_key);
	if (content.dptr != NULL)
		str = std::string(content.dptr, content.dsize);
	else
		throw NdbmError(DbErr_CantGetContent,MessGetContent);
	free(content.dptr);
}

//! [] operator overloading. It returns one content character
char NdbmPSNamesCont::operator [] (long i)
{
	try
	{
		return(str.at(i));
	}
	catch(const std::out_of_range &)
	{
		throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
	}
}

//! Method to return the host name from the record content
std::string NdbmPSNamesCont::get_host_name(void) const
{
	std::string::size_type pos;

	if ((pos = str.find(SEP)) == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	return str.substr(0, pos);	
}

// Method to return the process PID from the record content
unsigned long NdbmPSNamesCont::get_pid(void) const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_PS_PID;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
		if (i != (NB_SEP_PS_PID - 1))
			pos++;
		if (i == (NB_SEP_PS_PID - 2))
			start = pos;
	}
    
	std::stringstream st;
	st << str.substr(start, pos - start);
	unsigned long 	p;
	st >> p;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return p;
}

//! Method to return the refresh period from the record content
long NdbmPSNamesCont::get_refresh(void) const
{
	std::string::size_type pos;
	
	if ((pos = str.find_last_of(SEP,str.size() - 2)) == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	std::stringstream st;
	st << str.substr(pos, (str.size() - 1) - pos);
	long	p;
	st >> p;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return p;
}

//! Method to return all the parameters from the record content necessary for the devinfo call
void NdbmPSNamesCont::get_devinfo(db_devinfo_svc &data)
{
	strncpy(data.host_name,this->get_host_name().c_str(), HOST_NAME_LENGTH);	
	data.pid = this->get_pid();
	strncpy(data.device_class, "Pseudo device", DEV_CLASS_LENGTH); 
	data.server_name[0] = '\0';
	data.personal_name[0] = '\0';
	data.process_name[0] = '\0';
	data.server_version = 0;
	data.device_exported = true;
}

/**@class NdbmResKey
// This class is used to manage a resource table key part. It is 
// constructed from the NDBM key. Within a resource table, the key is :
//	- Resource family  name
//	- Resource member name
//	- Resource name
//	- Resource index
//
// The record content is :
//	- Resource value
 */

//! The class default constuctor
NdbmResKey::NdbmResKey()
	: inter_str(""),
	  str("")
{
	key.dptr = NULL;
	key.dsize = 0;
}

//! The class destructor
NdbmResKey::~NdbmResKey()
{
	if (key.dsize != 0)
		delete [] key.dptr;
}

//! Class constructor to be used from individual element
NdbmResKey::NdbmResKey(std::string &family,std::string &member,std::string &r_name,long indi=1)
{
	key.dptr = NULL;
	key.dsize = 0;
	try
	{
//
// Build intermediate key
//
		inter_str = family + '|' + member + '|' + r_name + '|';
//
// Build key
//
		std::stringstream to;
		to << inter_str << indi << '|' << std::ends;
		str = to.str();
#if !HAVE_SSTREAM
		to.freeze(false);
#endif
		build_datum();
	}
	catch (std::bad_alloc)
	{
		throw;
	}
}

//! Class constructor to be used from a already build key as a string
NdbmResKey::NdbmResKey(std::string &key_str)
{
	key.dptr = NULL;
	key.dsize = 0;
	try
	{
		str = key_str;	
//
// Build intermediate key
//
		std::string::size_type pos = key_str.find_last_of('|',key_str.size() - 2);
		inter_str = key_str.substr(0, pos + 1); 
		build_datum();
	}
	catch (std::bad_alloc)
	{
		throw;
	}
}

//! Class constructor to be used with the record key
NdbmResKey::NdbmResKey(datum user_key)
{
	key.dptr = NULL;
	key.dsize = 0;
	if (user_key.dptr != NULL)
	{
		str = std::string(user_key.dptr, user_key.dsize);
//
// Build intermediate key
//
		std::string::size_type pos = str.find_last_of('|',str.size() - 2);
		inter_str = str.substr(0, pos + 1); 
		build_datum();
	}
	else
		throw NdbmError(DbErr_CantBuildKey,MessBuildKey);
}

//! [] operator overloading. It returns one key character
char NdbmResKey::operator [] (long i)
{
	try
	{
		return(str.at(i));
	}
	catch(const std::out_of_range &)
	{
		throw NdbmError(DbErr_IndTooLarge, MessTooLarge);
	}
}

//! Method to build a datum data from the already stored content
void NdbmResKey::build_datum()
{
	try
	{
		if (str.length() >= MAX_KEY)
			str.erase(MAX_KEY - 1);
		long l = strlen(str.c_str());
		if (l != 0)
		{
			if (key.dsize)
				delete [] key.dptr;
			key.dptr = new char[l + 1];
			strcpy(key.dptr, str.c_str());
			key.dsize = l;
		}
	}
	catch (std::bad_alloc)
	{
		throw;
	}
}


//! Method to increment the key index part
void NdbmResKey::upd_indi(long ind)
{
//
// Build the new key
//
	std::stringstream to;
	to << inter_str << ind << '|' << std::ends;
	str = to.str();
#if !HAVE_SSTREAM
	to.freeze(false);
#endif
	build_datum();
}

//! Method to retrieve resource family name
std::string NdbmResKey::get_res_fam_name(void) const
{
	std::string::size_type pos;
	if ((pos = str.find(SEP)) == std::string::npos)
		throw NdbmError(DbErr_BadKeySyntax, MessKeySyntax);
	return str.substr(0, pos);	
}

//! Method to retrieve resource member name
std::string NdbmResKey::get_res_memb_name(void) const
{
	std::string::size_type 	pos = 0,
				start;
	for (long i = 0;i < NB_SEP_RES_MEMB;i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
		if (i != (NB_SEP_RES_MEMB - 1))
			pos++;
		if (i == (NB_SEP_RES_MEMB - 2))
			start = pos;
	}
	return str.substr(start, pos - start);
}

//! Method to retrieve resource name
std::string NdbmResKey::get_res_name(void) const
{
	std::string::size_type 	pos = 0,
				start;
	
	for (long i = 0; i < NB_SEP_RES_NAME; i++)
	{
		if ((pos = str.find(SEP,pos)) == std::string::npos)
			throw NdbmError(DbErr_BadKeySyntax,MessKeySyntax);
		if (i != (NB_SEP_RES_NAME - 1))
			pos++;
		if (i == (NB_SEP_RES_NAME - 2))
			start = pos;
	}
	return str.substr(start, pos - start);
}

//! Method to retrieve resource index (in case of resource from the array type)
long NdbmResKey::get_res_indi(void) const
{
	std::string::size_type pos;
	
	if ((pos = str.find_last_of(SEP,str.size() - 2)) == std::string::npos)
		throw NdbmError(DbErr_BadContSyntax,MessContSyntax);
	pos++;
	std::stringstream st;
	st << str.substr(pos, (str.size() - 1) - pos);
	unsigned long	indi;
	st >> indi;
#if !HAVE_SSTREAM
	st.freeze(false);
#endif
	return indi;
}

/**@class NdbmResCont 
// This class is used to manage a resource table content part. It is 
// constructed from the NDBM key. Within a resource table, the key is :
//	- Resource family  name
//	- Resource member name
//	- Resource name
//	- Resource index
//
// The record content is :
//	- Resource value
 */

//! The class default constuctor
NdbmResCont::NdbmResCont()
{
}

//! The class destructor
NdbmResCont::~NdbmResCont()
{
}

//! Class constructor to be used with the record key
NdbmResCont::NdbmResCont(GDBM_FILE db, datum call_key)
{
	datum content = gdbm_fetch(db, call_key);
	if (content.dptr != NULL)
	{
		str = std::string(content.dptr, content.dsize);
		free(content.dptr);
	}
	else
		throw NdbmError(DbErr_CantGetContent,MessGetContent);
}

//! [] operator overloading. It returns one content character
char NdbmResCont::operator [] (long i)
{
	try
	{
		return(str.at(i));
	}
	catch(const std::out_of_range &)
	{
		throw NdbmError(DbErr_IndTooLarge,MessTooLarge);
	}
}

//! Method to return the resource value
std::string NdbmResCont::get_res_value(void) const
{
	return str;
}

/**@class NdbmNameList 
// This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
 */

//! The class default constuctor
NdbmNameList::NdbmNameList()
{
	name_list.clear();
}

//! The class destructor
NdbmNameList::~NdbmNameList()
{
	name_list.clear();
}

//! [] operator overloading. It returns one element of the list
std::string &NdbmNameList::operator [] (long i)
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

//! This method add a name to the list if it is not already in the list
void NdbmNameList::add_if_new(const std::string &na)
{
    std::vector<std::string>::iterator p = find(name_list.begin(),name_list.end(),na);
    if (p == name_list.end())
	name_list.push_back(na);
    return;
}


//! This method sorts the list (alphabetically)
void NdbmNameList::sort_name()
{
    sort(name_list.begin(), name_list.end());
}

//! This method copy the vector to a C array allocated in this method
long NdbmNameList::copy_to_C(char **&buf)
{
    long length = name_list.size();
    try
    {
	buf = new char *[length];
		
	for (long i = 0;i < length;i++)
	{
	    buf[i] = new char [name_list[i].size() + 1];
	    name_list[i].copy(buf[i], std::string::npos);
	    (buf[i])[name_list[i].size()] = '\0';
	}
    	return(0);	
    }
    catch (std::bad_alloc)
    {
	for (long j = 0; j < length; j++)
	    delete [] buf[j];		
	delete [] buf;
	return(-1);
    }
}
/**@class NdbmDoubleNameList 
//
// This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
 */

//! The class default constuctor
NdbmDoubleNameList::NdbmDoubleNameList()
{
	first_list.clear();
	sec_list.clear();
}

//! The class destructor
NdbmDoubleNameList::~NdbmDoubleNameList()
{
	first_list.clear();
	sec_list.clear();
}

//! method to return ???
void NdbmDoubleNameList::get_record(const long first, const long second, std::string &f_str, std::string &s_str)
{
    f_str = first_list[first];
    s_str = sec_list[first][second];
}

//! method to return ???
long NdbmDoubleNameList::sec_name_length(long first)
{
    return(sec_list[first].size());
}

//! This method add a name to the list if it is not already in the list
void NdbmDoubleNameList::add(std::string first,std::string second)
{
	long i;
	long si = first_list.size();
		
	for (i = 0; i < si; i++)
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

/**@class NdbmDomDev 
//
// This class is used to manage a list of family/mmeber associated to a domain. 
// This is used by the server for the db_deviceres and db_deldeviceres calls.
//
 */

//! The class default constuctor
NdbmDomDev::NdbmDomDev()
{
}

//! The last constructor from the domain name and a family/member string
NdbmDomDev::NdbmDomDev(const std::string &dom,const std::string &str) : domain(dom)
{
    fm.push_back(str);
}

//! The class destructor
NdbmDomDev::~NdbmDomDev()
{
}

//! method to add a device name
void NdbmDomDev::add_dev(const std::string &dev)
{
    fm.push_back(dev);
}

//! method to return the domain
std::string NdbmDomDev::get_domain()
{
    return(domain);
}

//! method to return ???
std::string NdbmDomDev::get_fm(long ind)
{
    try
    {
	return(fm[ind]);
    }
    catch (...)
    {
	return std::string("");
    }
}

//! method to check if the device (in str) is in the list
long NdbmDomDev::find_in_list(const std::string &str)
{
    std::vector<std::string>::iterator p = std::find(fm.begin(), fm.end(), str);
    return (p != fm.end());
}

//! == operator overloading. Used by the standard find algorithms
bool operator== (const NdbmDomDev &a, const NdbmDomDev &b)
{
    return (a.domain == b.domain);
}



/**@class NdbmSvcDev 
//
// This class is used to manage a simple name list. This is used for all
// the browsing facilities implemented in the TACO static database server.
//
 */

//! The class default constuctor
NdbmSvcDev::NdbmSvcDev()
	: flag(false)
{
}

//! Another constructor from a NAMES table content
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

//! The class destructor
NdbmSvcDev::~NdbmSvcDev()
{
}

/**@class NdbmError 
//
// This class is used for error management.
//
 */

//! Contructor from the error code and error message (with default value)
NdbmError::NdbmError(long err, std::string mess)
 	: errcode(err), errmess(mess)
{
}

//! Method to retrieve the error code
long NdbmError::get_err_code()
{
	return errcode;
}

//! Method to retrieve the error message
char *NdbmError::get_err_message()
{
    	return const_cast<char *>(errmess.c_str());
}

//! Method to display the error message
void NdbmError::display_err_message()
{
    	std::cout << errmess << std::endl;
}
