#include <DevErrors.h>
#include <NdbmServer.h>
#include <cctype>
#include <algorithm>
#ifdef HAVE_SSTREAM
#	include <sstream>
#else
#	include <strstream>
#	define	stringstream 	strstream
#endif

/****************************************************************************
*                                                                           *
*		Server code for db_psdev_register function                  *
*                               -----------------            	            *
*                                                                           *
*    Function rule : To store in the database (builded from resources files)*
*                    the host_name, the program number and the version      *
*                    number of the device server for a specific device      *
*                                                                           *
*    Argin : A pointer to a structure of the "tab_dbdev" type               *
*            The definition of the tab_dbdev type is :                      *
*            struct {                                                       *
*              u_int tab_dbdev_len;     The number of structures db_devinfo *
*              db_devinfo *tab_dbdev_val;    A pointer to the array of      *
*					     structures                     *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns an integer which is an error code                *
*         Zero means no error                                               * 
*                                                                           *
*****************************************************************************/
db_psdev_error *NdbmServer::db_psdev_reg_1_svc(psdev_reg_x *rece)
{
    int 	num_psdev;
    psdev_elt 	*tmp;
//
// Miscellaneous init 
//
    num_psdev = rece->psdev_arr.psdev_arr_len;
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;

#ifdef DEBUG
    cout << "Begin db_psdev_register" << endl;
    cout << "Host name : " << rece->h_name << endl;
    cout << "PID = " << rece->pid << endl;
    for (int i = 0;i < num_psdev;i++)
    {
	tmp = &(rece->psdev_arr.psdev_arr_val[i]);
	cout << "Pseudo device name : " << tmp->psdev_name << endl;
	cout << "Refresh period : " << tmp->poll << endl;
    }
#endif
//
// Return error code if the server is not connected 
//
    if (!dbgen.connected)
    {
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// For each pseudo device, register it in the PS_NAMES table 
//
    for (long i = 0;i < num_psdev;i++)
    {
	tmp = &(rece->psdev_arr.psdev_arr_val[i]);
	try
	{
	    reg_ps(rece->h_name, rece->pid, tmp->psdev_name, tmp->poll);
	}
	catch (const long error)
	{
	    psdev_back.error_code =  error;
	    psdev_back.psdev_err = i + 1;
	    return(&psdev_back);
	}
    }
//
// Leave server 
//
#ifdef DEBUG
    cout << "End db_psdev_register" << endl;
#endif 
    return(&psdev_back);
}

/****************************************************************************
*                                                                           *
*		Code for reg_ps function                		    *
*                        ------                         		    *
*                                                                           *
*    Function rule : To register a pseuo device in the PS_NAMES database    *
*		     table						    *
*									    *
*    Argin(s) : - h_name : The host name				    *
*		- pid : The process PID					    *
*		- ps_name : The pseudo device name			    *
*		- poll : The polling period				    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/
long NdbmServer::reg_ps(const string &h_name, long pid, const string &ps_name, long poll) throw (long)
{
    datum 	key,
		content;
    string	ps_name_low;
//
// Make a copy of the pseudo device name in lowercase letter 
//
    transform(ps_name.begin(), ps_name.end(), ps_name_low.begin(), ::tolower);
//
// First, check that the name used for the pseudo device is not already used
// for a real device 
//
    for (key = gdbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = gdbm_nextkey(dbgen.tid[0], key))
    {
	content = gdbm_fetch(dbgen.tid[0],key);
	if (!content.dptr)
	    throw long(DbErr_DatabaseAccess);
	string temp = content.dptr;
	string::size_type pos = temp.find('|');
	if (temp.substr(0, pos) == ps_name_low)
	    break;
    }

    if (key.dptr == NULL)
    {
	if (gdbm_error(dbgen.tid[0]) != 0)
	    throw long(DbErr_DatabaseAccess);
    }
    else
	throw long(DbErr_NameAlreadyUsed);
//
// Now, try to retrieve a tuple in the PS_NAMES table with the same pseudo
// device name 
//
    key.dptr = const_cast<char *>(ps_name_low.data());
    key.dsize = ps_name_low.length();

    content = gdbm_fetch(dbgen.tid[dbgen.ps_names_index],key);
    if (content.dptr == NULL)
    {
//
// In case of error 
//
	if (gdbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
	    throw long(DbErr_DatabaseAccess);
//
// Insert a new record in database 
//
	else
	{
	    stringstream	s;
#if !HAVE_SSTREAM
            s.seekp(0, ios::beg);
#endif
	    s << h_name << "|" << pid << "|" << poll << "|";
#if !HAVE_SSTREAM
            content.dptr = s.str();
            content.dsize = strlen(s.str());
            s.freeze(false);
#else
	    content.dptr = const_cast<char *>(s.str().data());
	    content.dsize = s.str().length();
#endif

	    if (gdbm_store(dbgen.tid[dbgen.ps_names_index],key,content,GDBM_INSERT))
		throw long(DbErr_DatabaseAccess);
	}
    }
    else
    {
//
// Update database information 
//
	stringstream	s;
#if !HAVE_SSTREAM
        s.seekp(0, ios::beg);
#endif
	s << h_name << "|" << pid << "|" << poll << "|";
#if !HAVE_SSTREAM
        content.dptr = s.str();
        content.dsize = strlen(s.str());
        s.freeze(false);
#else
	content.dptr = const_cast<char *>(s.str().data());
	content.dsize = s.str().length();
#endif
	if (gdbm_store(dbgen.tid[dbgen.ps_names_index],key,content,GDBM_REPLACE))
	    throw long(DbErr_DatabaseAccess);
    }
//
// leave function 
//
    return(0);
}

/****************************************************************************
*                                                                           *
*		Server code for db_psdev_unregister function                *
*                               -------------------                         *
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
db_psdev_error *NdbmServer::db_psdev_unreg_1_svc(arr1 *rece)
{
    u_int 	num_psdev = rece->arr1_len;
//
// Miscellaneous init 
//
    psdev_back.error_code = 0;
    psdev_back.psdev_err = 0;

#ifdef DEBUG
    cout << "Begin db_psdev_unregister" << endl;
    for (long i = 0;i < num_psdev;i++)
	cout << "Pseudo device name : " << rece->arr1_val[i] << endl;
#endif
//
// Return error code if the server is not connected 
//
    if (!dbgen.connected)
    {
	psdev_back.error_code = DbErr_DatabaseNotConnected;
	return(&psdev_back);
    }
//
// For each pseudo device, unregister it in the PS_NAMES table 
//
    for (long i = 0;i < num_psdev;i++)
	try
	{
	    unreg_ps(rece->arr1_val[i]);
	}
	catch (const long error)
	{
	    psdev_back.error_code =  error;
	    psdev_back.psdev_err = i + 1;
	    return(&psdev_back);
	}
//
// Leave server
//
#ifdef DEBUG
    cout << "End db_psdev_unregister" << endl;
#endif 
    return(&psdev_back);
}

/****************************************************************************
*                                                                           *
*		Code for unreg_ps function                		    *
*                        --------                         		    *
*                                                                           *
*    Function rule : To unregister pseuo device from the database PS_NAMES  *
*		     table						    *
*									    *
*    Argin(s) : - ps_name : The pseudo device name			    *
*									    *
*    Argout(s) : - p_error : Pointer for an error code			    *
*									    *
*    This function returns 0 is everything OK. Otherwise, the returned value*
*    is -1 and the error code is set to the appropiate error.		    *
*                                                                           *
****************************************************************************/
long NdbmServer::unreg_ps(const string &ps_name) throw (long)
{
    long i,
	l;
    string	ps_name_low;
    datum 	key,
		content;
    char 	key_buf[MAX_KEY];
//
// Make a copy of the pseudo device name in lowercase letter 
//
    transform(ps_name.begin(), ps_name.end(), ps_name_low.begin(), ::tolower);
//
// Retrieve a tuple in the PS_NAMES table with the same pseudo device name 
//
    key.dptr = const_cast<char *>(ps_name_low.data());
    key.dsize = ps_name_low.length();

    content = gdbm_fetch(dbgen.tid[dbgen.ps_names_index], key);
    if (!content.dptr)
    {
	if (gdbm_error(dbgen.tid[dbgen.ps_names_index]))
	    throw long(DbErr_DatabaseAccess);
	else
	    throw long(DbErr_DeviceNotDefined);
    }
//
// Remove pseudo device 
//
    else if (gdbm_delete(dbgen.tid[dbgen.ps_names_index], key) != 0)
	throw long(DbErr_DatabaseAccess);
//
// leave function 
//
    return(0);
}
