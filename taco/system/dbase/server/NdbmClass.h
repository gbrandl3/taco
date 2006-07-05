/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.11 $
 *
 * Date:	$Date: 2006-07-05 14:40:54 $
 *
 */

#ifndef _NdbmClass_h_
#define _NdbmClass_h_

#include <API.h>
#include <db_xdr.h>

#include <vector>
#include <string>

#ifdef USE_GDBM
#include <gdbm.h>

#define gdbm_error(dbf) 	(0)
#define gdbm_clearerr(dbf)

/**@ingroup dbServerClasses
 */
class NdbmNamesKey
{
public:
	NdbmNamesKey();
	NdbmNamesKey(const datum &);
	NdbmNamesKey(const std::string &, const std::string &, const long = 1);
	~NdbmNamesKey();
	char operator[] (long);

	long 		length() {return str.length();}
	void 		build_datum();
	datum 		get_key() {return key;};
	void 		upd_indi(long);
	std::string 	get_ds_name(void);
	std::string	get_ds_pers_name(void);
	long 		get_dev_indi(void);
	
	void 		get_devinfo(db_devinfo_svc &);
	void 		get_devinfo(db_poller_svc &);

private:
	std::string 	str;
	std::string 	inter_str;
	datum 		key;
};

/**@ingroup dbServerClasses
 */
class NdbmNamesCont
{
public:
	NdbmNamesCont();
	NdbmNamesCont(GDBM_FILE, datum);

	~NdbmNamesCont();

	char operator[] (long);

	long 		length() {return str.length();}
	datum 		get_datum() {return dat;};
	std::string	get_device_name(void) const;
	std::string 	get_host_name(void) const;
	unsigned long	get_p_num(void) const;
	unsigned long 	get_v_num(void) const;
	std::string 	get_device_type(void) const;
	std::string 	get_device_class(void) const;
	unsigned long 	get_pid(void) const;
	std::string	get_process_name(void) const;

	std::string 	get_dev_domain_name(void) const;
	std::string 	get_dev_fam_name(void) const;
	std::string 	get_dev_memb_name(void) const;
	
	void 		get_devinfo(db_devinfo_svc &);
	void 		get_devinfo(db_poller_svc &);
	void 		unreg(void);

private:
	std::string 	str;
	datum 		dat;
};

/**@ingroup dbServerClasses
 */
class NdbmPSNamesKey
{
public:
	NdbmPSNamesKey();
	NdbmPSNamesKey(datum);

	~NdbmPSNamesKey();

	char operator[] (long);

	long 		length() {return str.size();}
	std::string 	get_psdev_domain_name() const;
	std::string 	get_psdev_fam_name() const;
	std::string 	get_psdev_memb_name() const;

private:
	std::string 	str;
};

/**@ingroup dbServerClasses
 */
class NdbmPSNamesCont
{
public:
	NdbmPSNamesCont();
	NdbmPSNamesCont(GDBM_FILE, datum);

	~NdbmPSNamesCont();

	char operator[] (long);

	long 		length() {return str.size();}
	std::string 	get_host_name() const;
	unsigned long 	get_pid() const;
	long 		get_refresh() const;
	
	void 		get_devinfo(db_devinfo_svc &);
private:
	std::string str;
};

/**@ingroup dbServerClasses
 */
class NdbmResKey
{
public:
	NdbmResKey();
	NdbmResKey(datum);
	NdbmResKey(std::string &,std::string &,std::string &,long);
	NdbmResKey(std::string &);

	~NdbmResKey();

	char operator[] (long);

	long 		length() {return str.size();};
	datum 		get_key() {return key;};
	void 		build_datum();
	std::string 	get_res_fam_name() const;
	std::string 	get_res_memb_name() const;
	std::string 	get_res_name() const;
	long 		get_res_indi() const;
	
	void 		upd_indi(long);

private:
	std::string 	str;
	std::string 	inter_str;
	datum 		key;
};

/**@ingroup dbServerClasses
 */
class NdbmResCont
{
public:
	NdbmResCont();
	NdbmResCont(GDBM_FILE, datum);

	~NdbmResCont();

	char operator[] (long);

	long length() {return str.size();};
	std::string 	get_res_value() const;

private:
	std::string 	str;
};

/**@ingroup dbServerClasses
 */
class NdbmNameList
{
public:
	NdbmNameList();
	~NdbmNameList();

	std::string &operator[] (long);
	
	long size() {return name_list.size();}	
	void add_if_new(const std::string &);
	void sort_name();
	long copy_to_C(char **&);
	
private:
	std::vector<std::string> name_list;
};

/**@ingroup dbServerClasses
 */
class NdbmDoubleNameList
{
public:
	NdbmDoubleNameList();
	~NdbmDoubleNameList();
	
	long first_name_length() {return first_list.size();}
	long sec_name_length(long);	
	void add(std::string, std::string);
	
	void get_record(const long, const long, std::string &, std::string &);
	
private:
	std::vector<std::string> 	first_list;
	std::vector<NdbmNameList>	 sec_list;
};


class NdbmDomDev
{
public:
	NdbmDomDev();
	NdbmDomDev(const std::string &,const std::string &);
	~NdbmDomDev();
	
	friend bool operator== (const NdbmDomDev &,const NdbmDomDev &);

	void 		add_dev(const std::string &);	
	std::string 	get_domain();
	std::string 	get_fm(long);
	long 		find_in_list(const std::string &);
private:
	std::string 		domain;
	std::vector<std::string> fm;
};

/**@ingroup dbServerClasses
 */
class NdbmSvcDev
{
public:
	NdbmSvcDev();
	NdbmSvcDev(const NdbmNamesCont &);
	~NdbmSvcDev();
	
	long 		get_flag() {return flag;}
	const char 	*get_name() {return name.c_str();}
	long 		get_name_length() {return (name.size() + 1);}
private:
	std::string 	name;
	long 		flag;
};

#endif

/**@class NdbmDomain
   @ingroup dbServerClasses
 *
 * This class is used to manage a simple name list. This is used for all
 * the browsing facilities implemented in the TACO static database server.
 *
 */
class NdbmDomain
{
public:
    //! The default constructor of the class
	NdbmDomain() : nb(0) {}
    //! Another constructor from the domain name
	NdbmDomain(const std::string &str) : name(str), nb(1) {}
    //! The last constructor from the domain name and the elt number
	NdbmDomain(const std::string &str,const long n): name(str), nb(n) {}
    //! The class destructor
	~NdbmDomain(){}
	
    //! == operator overloading. Used by the standard find algorithms
	friend bool operator== (const NdbmDomain &a, const NdbmDomain &b) {return (a.name == b.name);}
    //! < operator overloading. Used by the standard sort algorithms
	friend bool operator< (const NdbmDomain &a, const NdbmDomain &b) {return (a.name < b.name);}
		
	void 		inc_nb() {nb++;}
	long 		get_nb() {return nb;}
	const char 	*get_name() {return name.c_str();}	
private:
	std::string 	name;
	long 		nb;
};

/**@ingroup dbServerClasses
 */

#define	DbErr_CantGetContent	440
#define	DbErr_IndTooLarge	441
#define	DbErr_BadContSyntax	442
#define	DbErr_BadDevSyntax	443
#define	DbErr_CantBuildKey	444
#define	DbErr_BadKeySyntax	445
#define DbErr_BadResSyntax	446
#define DbErr_CantOpenResFile	447

#define	MessGetContent 	"Error when trying to get content"
#define MessTooLarge	"Indice above content size"
#define MessContSyntax	"Bad syntax in db content (Can't retrieve field separator)"
#define MessDevSyntax	"Bad device name syntax stored in database"
#define	MessBuildKey	"The key pointer is NULL!!!"
#define MessKeySyntax	"Bad syntax in database key record"

/**@ingroup dbServerClasses
 */
class NdbmError
{
public:
	NdbmError(long, const std::string mess = "No error message defined");
	
	long 	get_err_code();
	char 	*get_err_message();
	void 	display_err_message();
	
private:
	long 		errcode;
	std::string 	errmess;
};

// Define some constant

#define	SEP		'|'
#define	SEP_DEV		'/'
#define NB_SEP_HOST	2
#define NB_SEP_PN	3
#define NB_SEP_VN	4
#define NB_SEP_TYPE	5
#define NB_SEP_CLASS	6
#define NB_SEP_PID	7

#define NB_SEP_DEV_FAM	2

#define NB_SEP_DS_PERS	2
#define	NB_SEP_PS_PID	2

#define NB_SEP_RES_MEMB	2
#define NB_SEP_RES_NAME	3

#define POLL_RES	"ud_poll_list"

long cmp_nocase(std::string &,std::string &);


// Define some structures

typedef struct _NdbmInfo {
#if USE_GDBM
	GDBM_FILE			tid[MAXDOMAIN];
#endif
	std::vector<std::string>	TblName;
	long				TblNum;
	long				ps_names_index;
	long				connected;
}NdbmInfo;

#include "config.h"

#ifdef HAVE_SSTREAM
#       include <sstream>
#else
#       include <strstream>
#       define stringstream strstream
#endif

#ifdef USE_GDBM

/**@ingroup dbServerClasses
 */
struct DATUM : public datum
{
	DATUM()	{dptr = NULL; dsize = 0;};
	DATUM(const DATUM &a) { dptr = NULL; dsize = 0; copy(a); };
	DATUM(const datum &a) { dptr = NULL; dsize = 0; copy(a); }
	~DATUM() { clear(); };
#ifndef _solaris
	DATUM & operator =(const DATUM &a) { copy(a); };
	DATUM & operator =(const datum &a) { copy(a); };
#else
	DATUM & DATUM::operator =(const DATUM &a) { copy(a); return *this;};
	DATUM & DATUM::operator =(const datum &a) { copy(a); return *this;};
#endif /* !_solaris */
private:
	void clear(void)
	{
		if (dptr != NULL)
			delete [] dptr;
		dptr = NULL;
		dsize = 0;
	};
	void copy(const datum &a)
	{
		clear();
		dsize = a.dsize;
		dptr = new char[dsize + 1];
		strcpy(dptr, a.dptr);
	};
	void copy(const DATUM &a)
	{
		clear();
		dsize = a.dsize;
		dptr = new char[dsize + 1];
		strcpy(dptr, a.dptr);
	};
}; 

datum createKey(std::string family, std::string member, std::string name, long number);

#endif

#endif
