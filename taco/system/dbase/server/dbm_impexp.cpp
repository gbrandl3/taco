#include "config.h"
#include <API.h>

#include <DevErrors.h>

#include <db_xdr.h>
#include <fcntl.h>

#include <iostream>
#include <NdbmClass.h>
#include <NdbmServer.h>
#ifdef HAVE_SSTREAM
#	include <sstream>
#else
#	include <strstream>
#	define	stringstream	strstream
#endif

#include <gdbm.h>

/**
 * To store in the database (built from resources files) the host_name, the 
 * program number and the version number of the device server for a specific device
 *
 * @param rece  A pointer to a structure of the tab_dbdev type
 * 
 * @return   an integer which is an error code, Zero means no error
 */
DevLong *NdbmServer::db_devexp_1_svc(tab_dbdev *rece)
{
	u_int 		num_dev = rece->tab_dbdev_len;;
	static DevLong 	errcode;

#ifdef DEBUG
	for (int i = 0; i < num_dev; i++)
	{
		std::cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << std::endl;
		std::cout << "Host name : " << rece->tab_dbdev_val[i].host_name << std::endl;
		std::cout << "Program number : " << rece->tab_dbdev_val[i].p_num << std::endl;
		std::cout << "Version number : " << rece->tab_dbdev_val[i].v_num << std::endl;
		std::cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << std::endl;
		std::cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << std::endl;
	}
#endif
//
// Initialize error code sent back to client 
//
	errcode = 0;
//
// Return error code if the server is not connected to the database 
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Store values in database 
//
	long	db_err;
	for (int i = 0; i < num_dev; i++)
		if ((db_err = db_store(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}

	return(&errcode);
}


/**
 * To store in the database (built from resources files) the host_name, the 
 * program number, the version number and the process ID of the device server 
 * for a specific device 
 * 
 * @param rece A pointer to a structure of the tab_dbdev_2 type
 *
 * @return an integer which is an error code, Zero means no error
 */
DevLong *NdbmServer::db_devexp_2_svc(tab_dbdev_2 *rece)
{
	int 		db_err;
	u_int 		num_dev = rece->tab_dbdev_len;
	static DevLong 	errcode;

#ifdef DEBUG
	for (int i = 0; i < num_dev; i++)
	{
		std::cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << std::endl;
		std::cout << "Host name : " << rece->tab_dbdev_val[i].host_name << std::endl;
		std::cout << "Program number : " << rece->tab_dbdev_val[i].p_num << std::endl;
		std::cout << "Version number : " << rece->tab_dbdev_val[i].v_num << std::endl;
		std::cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << std::endl;
		std::cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << std::endl;
		std::cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << std::endl;
	}
#endif
//
// Initialize error code sent back to client
//
	errcode = 0;
//
// Return error code if the server is not connected to the database
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Store values in database
//
	for (int i = 0; i < num_dev; i++)
		if ((db_err = db_store_2(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}
	return(&errcode);
}


/**
 * To store in the database (built from resources files) the host_name, the 
 * program number, the version number and the process ID of the device server 
 * for a specific device
 * 
 * @param rece A pointer to a structure of the tab_dbdev_3 type
 *
 * @return   an integer which is an error code. Zero means no error
 */
DevLong *NdbmServer::db_devexp_3_svc(tab_dbdev_3 *rece)
{
	int 		db_err;
	u_int 		num_dev = rece->tab_dbdev_len;;
	static DevLong 	errcode;

#ifdef DEBUG
	for (int i = 0; i < num_dev; i++)
	{
		std::cout << "Device name : " << rece->tab_dbdev_val[i].dev_name << std::endl;
		std::cout << "Host name : " << rece->tab_dbdev_val[i].host_name << std::endl;
		std::cout << "Program number : " << rece->tab_dbdev_val[i].p_num << std::endl;
		std::cout << "Version number : " << rece->tab_dbdev_val[i].v_num << std::endl;
		std::cout << "Device type : " << rece->tab_dbdev_val[i].dev_type << std::endl;
		std::cout << "Device class : " << rece->tab_dbdev_val[i].dev_class << std::endl;
		std::cout << "Device server PID : " << rece->tab_dbdev_val[i].pid << std::endl;
		std::cout << "Device server process name : " << rece->tab_dbdev_val[i].proc_name << std::endl;
	}
#endif
//
// Initialize error code sent back to client 
//
	errcode = 0;
//
// Return error code if the server is not connected to the database 
//
	if (dbgen.connected == False)
	{
		errcode = DbErr_DatabaseNotConnected;
		return(&errcode);
	}
//
// Store values in database 
//
	for (int i =0 ;i < num_dev; i++)
		if ((db_err = db_store_3(rece->tab_dbdev_val[i])) != 0)
		{
			errcode = db_err;
			return(&errcode);
		}

	return(&errcode);
}


/**
 * To retrieve (from database) the host_name, the program number and the version 
 * number for specific devices 
 * 
 * @param de_name A pointer to a structure of the arr1 type
 *
 * @return   This function returns a pointer to a structure of the db_resimp type 
 */
db_resimp *NdbmServer::db_devimp_1_svc(arr1 *de_name)
{
	int 		j,
			resu,
			num_dev = de_name->arr1_len;
	device 		ret;
	db_devinfo 	*stu_addr,
			*stu_addr1;	
	int 		ret_pn,
			ret_vn;
	
    	std::string	ret_dev_name,
			ret_host_name,
			ret_dev_type,
			ret_dev_class;

#ifdef DEBUG
	for (int i = 0; i < num_dev; i++)
		std::cout << "Device name (in import function) : " << de_name->arr1_val[i] << std::endl;
#endif
//
// Initialize error code sent back to client
//
	back.db_imperr = 0;
//
// Return error code if the server is not connected to the database
//
	if (dbgen.connected == False)
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
	catch (const std::bad_alloc)
	{
		back.db_imperr = DbErr_ServerMemoryAllocation;
		back.imp_dev.tab_dbdev_len = 0;
		back.imp_dev.tab_dbdev_val = NULL;
		return(&back);
	}
//
// A loop on every device to import 
//
	for (int i = 0; i < num_dev; i++)
	{
		datum		key;
		try
		{
//
// Try to retrieve the tuple in the NAMES table 
//
			datum	key2;
			for (key = gdbm_firstkey(dbgen.tid[0]);
				key.dptr != NULL;
				key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
			{
				NdbmNamesCont	cont(dbgen.tid[0], key);
				if (cont.get_device_name() == de_name->arr1_val[i])
				{
// Unpack the content 
					ret_dev_name = cont.get_device_name();
					ret_host_name = cont.get_host_name();	
					ret_pn = cont.get_p_num();
					ret_vn = cont.get_v_num();
					ret_dev_type = cont.get_device_type();
					ret_dev_class = cont.get_device_class();
					if (ret_pn == 0)
					{
						back.db_imperr = DbErr_DeviceNotExported;
						throw int(0);
					}
					stu_addr1 = &(back.imp_dev.tab_dbdev_val[i]);
//
// Allocate memory for the host_name string
//
					stu_addr1 = &(back.imp_dev.tab_dbdev_val[i]);
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
//
// Initialize structure sent back to client 
//
					strcpy(stu_addr1->host_name, ret_host_name.c_str());
					strcpy(stu_addr1->dev_name, de_name->arr1_val[i]);
					stu_addr1->p_num = ret_pn;
					stu_addr1->v_num = ret_vn;
					strcpy(stu_addr1->dev_class, ret_dev_class.c_str());
					strcpy(stu_addr1->dev_type, ret_dev_type.c_str());
					
					break;
				} 
			}
			if (key.dptr == NULL)
			{
				back.db_imperr = DbErr_DeviceNotDefined;
				throw int(1);
			}
			free(key.dptr);
		}
		catch(NdbmError & err)
		{
			free(key.dptr);
			delete [] stu_addr1->host_name;
			delete [] stu_addr1->dev_name;
			delete [] stu_addr1->dev_type;
			for (int j = 0; j < i; j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				delete [] stu_addr->dev_name;
				delete [] stu_addr->host_name;
				delete [] stu_addr->dev_type;
				delete [] stu_addr->dev_class;
			}
			std::cerr << err.get_err_message() << std::endl;
			back.db_imperr = err.get_err_code();
			back.imp_dev.tab_dbdev_len = 0;
			return (&back);
		}
		catch(const std::bad_alloc)
		{
			free(key.dptr);
			delete [] stu_addr1->host_name;
			delete [] stu_addr1->dev_name;
			delete [] stu_addr1->dev_type;
			for (int j = 0; j < i; j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				delete [] stu_addr->dev_name;
				delete [] stu_addr->host_name;
				delete [] stu_addr->dev_type;
				delete [] stu_addr->dev_class;
			}
			back.imp_dev.tab_dbdev_len = 0;
			std::cerr << "Memory allocation error in devinfo" << std::endl;
			back.db_imperr = DbErr_ServerMemoryAllocation;
			return (&back);
		}
		catch (const int)
		{
//
// In case of error 
//
			free(key.dptr);
//			delete [] stu_addr1->host_name;
//			delete [] stu_addr1->dev_name;
//			delete [] stu_addr1->dev_type;
			for (int j = 0; j < i; j++)
			{
				stu_addr = &(back.imp_dev.tab_dbdev_val[j]);
				delete [] stu_addr->dev_name;
				delete [] stu_addr->host_name;
				delete [] stu_addr->dev_type;
				delete [] stu_addr->dev_class;
			}
			back.imp_dev.tab_dbdev_len = 0;
			return(&back);
		}
	}
	back.imp_dev.tab_dbdev_len = num_dev;
	return(&back);
}


/**
 * To unregister from database all the devices driven by a device server 
 *
 * @param dsn_name The network device server name 
 * 
 * @return This function returns an int which is an error code Zero means no error
 */
DevLong *NdbmServer::db_svcunr_1_svc(nam *dsn_name)
{
	static DevLong 	mis;
	register char 	*tmp,
			*sto,
			*temp;
	register int 	i;
	device 		dev;
	unsigned int 	diff;
	int 		resu,
			d_num = 0;
	int 		dev_numb = 1;
	char 		seqnr[4];
	bool 		exit = false;
	int 		flags = GDBM_REPLACE;
	std::string	proc_str,
			pers_str,
			p_num,
			dev_class;
	int 		old_d_num;

	std::string	device_name(*dsn_name);
#ifdef DEBUG
	std::cout << "Device server name (unregister function) : " << device_name << std::endl;
#endif
//
// Miscallaneous initialization
//
	mis = DS_OK;
//
// Return error code if the server is not connected to the database */
//
	if (dbgen.connected == False)
	{
		mis = DbErr_DatabaseNotConnected;
		return(&mis);
	}
//
// Get device server class 
//
	std::string::size_type pos = device_name.find('/');
	strcpy(dev.ds_class, device_name.substr(0, pos).c_str());
//
// Get device server name 
//
	strcpy(dev.ds_name, device_name.substr(pos + 1).c_str());

#ifdef DEBUG
	std::cout << "Device server class (unreg) : " << dev.ds_class << std::endl;
	std::cout << "Device server name (unreg) : " << dev.ds_name << std::endl;
#endif
//
// Try to retrieve devices in database assuming that the input device server name is the
// device server PROCESS name. As there is no key build on the device server process name, 
// do a full traversal of the database 
//
	do
	{
		old_d_num = d_num;
		datum 	key, 
			key2;
		for (key = gdbm_firstkey(dbgen.tid[0]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
//
// Extract personal name and sequence field from key 
//
			NdbmNamesKey	namesKey(key);
			if (dev.ds_name != namesKey.get_ds_pers_name())
				continue;
//
// Get db content
//
			try
			{
				NdbmNamesCont cont(dbgen.tid[0], key);
//
// Extract program number 
//
				if (cont.get_p_num() == 0)
					continue;
//
// Extract device class 
//
				dev_class = cont.get_device_class();
//
// Extract process name 
//
				proc_str = cont.get_process_name();
				if (dev.ds_class != proc_str)
					continue;
//
// A device to be unregistered has been found, build the new database content 
// host name ?
// 
				std::string 	dev_str,
						key_str;
				dev_str = cont.get_device_name() + "|" + cont.get_host_name() + "|0|0|unknown|" + dev_class + "|0|" + proc_str + "|";
//
// Update database 
//
				datum	dev1;
				dev1.dptr = const_cast<char *>(dev_str.c_str());
				dev1.dsize = strlen(dev1.dptr);

				if (gdbm_store(dbgen.tid[0], key, dev1, flags))
				{
					mis = DbErr_DatabaseAccess;
					free(key.dptr);
					return(&mis);
				}
				d_num++;
				break;
			}
			catch ( ... )
			{
				if (gdbm_error(dbgen.tid[0]) != 0)
				{
					mis = DbErr_DatabaseAccess;
					free(key.dptr);
					return(&mis);
				}
			}
		}
	} while (old_d_num != d_num);
//
// Initialization needed to retrieve the right tuples in the NAMES table and to update the 
// tuples (program and version number) assuming the input name is a device server name
//
	if (d_num == 0)
	{
		do
		{
			datum	key3;
			key3.dptr = new char[MAX_KEY];
			std::stringstream        s;
#if !HAVE_SSTREAM
			s.seekp(0, ios::beg);
#endif
			s << dev.ds_class << "|" << dev.ds_name << "|" << dev_numb << "|" << std::ends;
#if !HAVE_SSTREAM
			strcpy(key3.dptr, s.str());
			s.freeze(false);
#else
			strcpy(key3.dptr, s.str().c_str());
#endif
			key3.dsize = strlen(key3.dptr);
//
// Try to retrieve the tuples 
//
			try
			{
				NdbmNamesCont cont(dbgen.tid[0], key3); 
				d_num++;
//
// Extract device class 
//
				dev_class = cont.get_device_class();
//
// Build the new database content
//
				datum	content = cont.get_datum();
				std::string 	dev_str;
			
				dev_str = cont.get_device_name() + "|" + cont.get_host_name() + "|0|0|unknown|" + dev_class + "|0|" + cont.get_process_name() + "|";
//
// Update database 
//
				datum	dev1;
				dev1.dptr = const_cast<char *>(dev_str.c_str());
				dev1.dsize = strlen(dev1.dptr);

				if (gdbm_store(dbgen.tid[0], key3, dev1, flags))
				{
					mis = DbErr_DatabaseAccess;
					return(&mis);
				}
				dev_numb++;
				delete [] key3.dptr;
			}
			catch ( ... )
			{
				exit = true;
			}
		} while (!exit);
	}
//
// No error 
//
	return(&mis);
}



/**
 * To retrieve (and send back to client) the program number and version number 
 * for a device server 
 * 
 * @param dsn_nam The network device server name 
 * 
 * @return a pointer to a stucture of the svc_inf type
 */
svc_inf *NdbmServer::db_svcchk_1_svc(nam *dsn_nam)
{
	datum		key,
			content;
	std::string	ret_host_name,
			ret_dev_name,
			ret_dev_type,
			ret_dev_class,
			sto(*dsn_nam);
#ifdef DEBUG
	std::cout << "Device server name (check function) : " << std::sto << endl;
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
	if (dbgen.connected == False)
	{
		svc_info.db_err = DbErr_DatabaseNotConnected;
		return(&svc_info);
	}
//
// Get device server class
//
	std::string::size_type	pos = sto.find('/');
	std::string		ds_class = sto.substr(0, pos);
//
// Get device server name
//
	std::string	ds_name = sto.substr(pos + 1);

#ifdef DEBUG
	std::cout << "Device server class (check) : " << ds_class << std::endl;
	std::cout << "Device server name (check) : " << ds_name << std::endl;
#endif
//
// Initialization needed to retrieve the right tuples in the NAMES table
//
	std::string	s = ds_class + "|" + ds_name + "|1|";
	key.dptr = const_cast<char *>(s.c_str());
	key.dsize = strlen(key.dptr);
//
// Try to retrieve the tuples
//
	try
	{
		NdbmNamesCont cont(dbgen.tid[0], key);

		ret_host_name = cont.get_host_name();
		svc_info.ho_name = new char[ret_host_name.length() + 1];
		strcpy(svc_info.ho_name, ret_host_name.c_str());
		svc_info.p_num = cont.get_p_num();
		svc_info.v_num = cont.get_v_num();
        }
	catch(const std::bad_alloc &)
	{
		svc_info.db_err = DbErr_ServerMemoryAllocation;
	}
	catch ( ... )
	{
		svc_info.db_err = DbErr_DeviceServerNotDefined;
	}
//
// Leave function
//
	return(&svc_info);
}



/**
 * To store in the NAMES table of the database the number and the version number 
 * of the device server in charge of a device 
 * 
 * @param A db_devinf structure (with device name, host name, program number and version number)
 * 
 * @returns 0 if no errors occurs or the error code when there is a problem.
 */
int NdbmServer::db_store(db_devinfo &dev_stu)
{
	datum 	key, 
		cont_sto;
	int	errcode = 0;

	try
	{
//
// Try to retrieve the right tuple in the NAMES table
//
		datum	key2;
		for(key = gdbm_firstkey(dbgen.tid[0]);
        		key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0], key);	
			if (cont.get_device_name() == dev_stu.dev_name)
			{
				free(key.dptr);
				break;
			}
		}
		if (key.dptr == NULL)
			throw int(DbErr_DeviceNotDefined);
//
// Test to see if host name in database is the same.
// Finally, update the tuple in database 
//
		std::stringstream    s;
#if !HAVE_SSTREAM
		s.seekp(0, ios::beg);
#endif
		s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num <<  "|"
			<< dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class << "|"
			<< "0|unknown|" << std::ends;
		cont_sto.dptr = new char[MAX_CONT];
#if !HAVE_SSTREAM
		strcpy(cont_sto.dptr, s.str());
		s.freeze(false);
#else
		strcpy(cont_sto.dptr, s.str().c_str());
#endif
		cont_sto.dsize = strlen(cont_sto.dptr);
		if (gdbm_store(dbgen.tid[0], key, cont_sto, GDBM_REPLACE))
			throw int(DbErr_DatabaseAccess);
	} 
	catch(NdbmError & err)
	{
		errcode = err.get_err_code();
	}
	catch(const int err)
	{
        	errcode = err;
	}
	catch (const std::bad_alloc)
	{
		errcode	= int(DbErr_ServerMemoryAllocation);
	}
	delete [] cont_sto.dptr;
	free(key.dptr);
	return errcode;
}


/**
 * To store in the NAMES table of the database the number and the version number 
 * of the device server in charge of a device
 * 
 * @param A db_devinf structure (with device name, host name, program number and version number)
 *
 * @return This function returns 0 if no errors occurs or the error code when
 *    there is a problem.
 */
int NdbmServer::db_store_2(db_devinfo_2 &dev_stu)
{
	datum 	key, 
		cont_sto;
	int	errcode = DS_OK;

	try
	{
		datum	key2;
		for(key = gdbm_firstkey(dbgen.tid[0]);
        		key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
			NdbmNamesCont cont(dbgen.tid[0], key);	
			if (cont.get_device_name() == dev_stu.dev_name)
			{
				free(key.dptr);
				break;
			}
		}
		if (key.dptr == NULL)
			throw int(DbErr_DeviceNotDefined);

//
// Test to see if host name in database is the same.
// Finally, update the tuple in database
//
		std::stringstream    s;
#if !HAVE_SSTREAM
		s.seekp(0, ios::beg);
#endif
		s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num <<  "|"
			<< dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class << "|"
			<< dev_stu.pid << "|unknown|" << std::ends;
		cont_sto.dptr = new char[MAX_CONT];
#if !HAVE_SSTREAM
		strcpy(cont_sto.dptr, s.str());
		s.freeze(false);
#else
		strcpy(cont_sto.dptr, s.str().c_str());
#endif
		cont_sto.dsize = strlen(cont_sto.dptr);
		if (gdbm_store(dbgen.tid[0], key, cont_sto, GDBM_REPLACE))
			throw int(DbErr_DatabaseAccess);
	}
	catch(NdbmError & err)
	{
		errcode = err.get_err_code();
	}
	catch(const int err)
	{
        	errcode = err;
	}
	catch (const std::bad_alloc)
	{
		errcode	= int(DbErr_ServerMemoryAllocation);
	}
	delete [] cont_sto.dptr;
	free(key.dptr);
	return errcode;
}


/**
 * To store in the NAMES table of the database the number and the version number 
 * of the device server in charge of a device. This function is for the version 3 
 * of the db_dev_export call
 * 
 * @param A db_devinf structure (with device name, host name, program
 *              number, version number and process name)
 * 
 * @return This function returns 0 if no errors occurs or the error code when
 *    there is a problem.
 */
int NdbmServer::db_store_3(db_devinfo_3 &dev_stu)
{
	int	errcode = DS_OK;
	datum 	cont_sto,
		key; 

	try
	{
		datum 	key2;
		for(key = gdbm_firstkey(dbgen.tid[0]);
        		key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
		{
			NdbmNamesKey  k(key);
			NdbmNamesCont cont(dbgen.tid[0], key);	
/*
 * WARNING !!!
 * This does not solve the problem of double device names in different servers
 */
			if (cont.get_device_name() == dev_stu.dev_name && k.get_ds_name() == dev_stu.proc_name)
				break;
		}
		if (key.dptr == NULL)
			throw int(DbErr_DeviceNotDefined);
//
// Finally, update the tuple in database
//
		std::stringstream    s;
#if !HAVE_SSTREAM
		s.seekp(0, ios::beg);
#endif
		s << dev_stu.dev_name << "|" << dev_stu.host_name << "|" << dev_stu.p_num <<  "|"
			<< dev_stu.v_num << "|" << dev_stu.dev_type << "|" << dev_stu.dev_class << "|"
			<< dev_stu.pid << "|" << dev_stu.proc_name << "|" << std::ends;
		cont_sto.dptr = new char[MAX_CONT];
#if !HAVE_SSTREAM
		strcpy(cont_sto.dptr, s.str());
		s.freeze(false);
#else
		strcpy(cont_sto.dptr, s.str().c_str());
#endif
		cont_sto.dsize = strlen(cont_sto.dptr);
		if (gdbm_store(dbgen.tid[0], key, cont_sto, GDBM_REPLACE))
			throw int(DbErr_DatabaseAccess);
		delete [] cont_sto.dptr;
		free(key.dptr);
	}
	catch(NdbmError & err)
	{
		errcode = err.get_err_code();
	}
	catch(const int err)
	{
        	errcode = err;
		if (err == DbErr_DatabaseAccess)
		{
			delete [] cont_sto.dptr;
			free(key.dptr);
		}
	}
	catch (const std::bad_alloc)
	{
		errcode	= int(DbErr_ServerMemoryAllocation);
	}
	return errcode;
}
