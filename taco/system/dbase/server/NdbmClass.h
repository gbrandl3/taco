#ifndef _NdbmClass_h_
#define _NdbmClass_h_

#include <API.h>
#include <db_xdr.h>

#include "config.h"

#include <gdbm.h>

#define gdbm_error(dbm)	(0)
#define gdbm_clearerr(dbm)

#include <vector>
#include <string>

using namespace std;

class NdbmNamesKey
{
public:
	NdbmNamesKey();
	NdbmNamesKey(datum);
	NdbmNamesKey(string &,string &,long = 1);

	~NdbmNamesKey();

	char operator[] (long);

	long length() {return str->size();}
	void build_datum();
	datum get_key() {return key;};
	void upd_indi(long);
	void get_ds_name(string &);
	void get_ds_pers_name(string &);
	void get_dev_indi(long &);
	
	void get_devinfo(db_devinfo_svc &);
	void get_devinfo(db_poller_svc &);

private:
	string *str;
	string inter_str;
	datum key;
};



class NdbmNamesCont
{
public:
	NdbmNamesCont();
	NdbmNamesCont(GDBM_FILE, datum);

	~NdbmNamesCont();

	char operator[] (long);

	long length() {return str->size();}
	void build_datum();
	datum get_datum() {return dat;};
	void get_device_name(string &) const;
	void get_host_name(string &);
	void get_p_num(unsigned long &) const;
	void get_v_num(unsigned long &) const;
	void get_device_type(string &);
	void get_device_class(string &);
	void get_pid(unsigned long &);
	void get_process_name(string &);

	void get_dev_domain_name(string &);
	void get_dev_fam_name(string &);
	void get_dev_memb_name(string &);
	
	void get_devinfo(db_devinfo_svc &);
	void get_devinfo(db_poller_svc &);
	void unreg();

private:
	string *str;
	datum dat;
};



class NdbmPSNamesKey
{
public:
	NdbmPSNamesKey();
	NdbmPSNamesKey(datum);

	~NdbmPSNamesKey();

	char operator[] (long);

	long length() {return str->size();}
	void get_psdev_domain_name(string &);
	void get_psdev_fam_name(string &);
	void get_psdev_memb_name(string &);

private:
	string *str;
};



class NdbmPSNamesCont
{
public:
	NdbmPSNamesCont();
	NdbmPSNamesCont(GDBM_FILE, datum);

	~NdbmPSNamesCont();

	char operator[] (long);

	long length() {return str->size();}
	void get_host_name(string &);
	void get_pid(unsigned long &);
	void get_refresh(long &);
	
	void get_devinfo(db_devinfo_svc &);

private:
	string *str;
};


class NdbmResKey
{
public:
	NdbmResKey();
	NdbmResKey(datum);
	NdbmResKey(string &,string &,string &,long);
	NdbmResKey(string &);

	~NdbmResKey();

	char operator[] (long);

	long length() {return str->size();};
	datum get_key() {return key;};
	void build_datum();
	void get_res_fam_name(string &);
	void get_res_memb_name(string &);
	void get_res_name(string &);
	void get_res_indi(long &);
	
	void upd_indi(long);

private:
	string *str;
	string inter_str;
	datum key;
};


class NdbmResCont
{
public:
	NdbmResCont();
	NdbmResCont(GDBM_FILE, datum);

	~NdbmResCont();

	char operator[] (long);

	long length() {return str->size();};
	void get_res_value(string &);

private:
	string *str;
};


class NdbmNameList
{
public:
	NdbmNameList();
	~NdbmNameList();

	string &operator[] (long);
	
	long length() {return name_list.size();}	
	void add_if_new(string &);
	void sort_name();
	long copy_to_C(char **&);
	
private:
	vector<string> name_list;
};


class NdbmDoubleNameList
{
public:
	NdbmDoubleNameList();
	~NdbmDoubleNameList();
	
	long first_name_length() {return first_list.size();}
	long sec_name_length(long);	
	void add(string &,string &);
	
	void get_record(long,long,string &,string &);
	
private:
	vector<string> first_list;
	vector<NdbmNameList> sec_list;
};



class NdbmDomain
{
public:
	NdbmDomain();
	NdbmDomain(const string &);
	NdbmDomain(const string &,const long &);
	~NdbmDomain();
	
	friend bool operator== (const NdbmDomain &, const NdbmDomain &);
	friend bool operator< (const NdbmDomain &, const NdbmDomain &);
		
	void inc_nb() {nb++;}
	long get_nb() {return nb;}
	const char *get_name() {return name.c_str();}	
private:
	string name;
	long nb;
};

class NdbmDomDev
{
public:
	NdbmDomDev();
	NdbmDomDev(const string &,const string &);
	~NdbmDomDev();
	
	friend bool operator== (const NdbmDomDev &,const NdbmDomDev &);

	void add_dev(const string &);	
	string &get_domain();
	string &get_fm(long);
	long find_in_list(const string &);
private:
	string domain;
	vector<string> fm;
};

class NdbmSvcDev
{
public:
	NdbmSvcDev();
	NdbmSvcDev(const NdbmNamesCont &);
	~NdbmSvcDev();
	
	long get_flag() {return flag;}
	const char *get_name() {return name.c_str();}
	long get_name_length() {return (name.size() + 1);}
private:
	string name;
	long flag;
};


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

class NdbmError
{
public:
	NdbmError(long, char *);
	
	long get_err_code();
	char *get_err_message();
	void display_err_message();
	
private:
	long errcode;
	char errmess[256];
};


/*#ifdef sun
class bad_alloc
{
public:
	bad_alloc() {};
	~bad_alloc() {};
};
#endif*/

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

long cmp_nocase(string &,string &);


// Define some structures

typedef struct _NdbmInfo {
	GDBM_FILE	tid[MAXDOMAIN];
	string		TblName[MAXDOMAIN];
	long		TblNum;
	long		ps_names_index;
	long		connected;
}NdbmInfo;



#endif
