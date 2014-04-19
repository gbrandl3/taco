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
 * Authors:
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.20 $
 *
 * Date:	$Date: 2008-04-06 09:07:41 $
 *
 */

#include "config.h"
#include <DevErrors.h>
#include <algorithm>
#include <NdbmServer.h>


/**
 * To update device list(s)
 * 
 * @param dev_list The device name
 * 
 * @return a pointer to a structure with all device info and an error code which is set if needed
 *
 */
db_psdev_error *NdbmServer::upddev_1_svc(db_res *dev_list)
{
	long list_nb = dev_list->res_val.arr1_len;
		
	logStream->debugStream() << "In upddev_1_svc function for " << list_nb << " device list(s)" << log4cpp::eol;

//
// Initialize parameter sent back to client 
//
	psdev_back.error_code = 0;
	psdev_back.psdev_err = 0;
//
// A loop on each device list 
//
	for (long i = 0; i < list_nb; i++)
	{
//		
// Allocate memory for strtok pointers
//
		std::string		lin(dev_list->res_val.arr1_val[i]);
		std::string::size_type	pos = lin.find(":");
		if(pos == std::string::npos)
		{
			logStream->debugStream() << "upd_name : no ':' found : " << lin << log4cpp::eol;
			psdev_back.psdev_err = i + 1;
			psdev_back.error_code = ERR_DEVNAME;
			return(&psdev_back);
		}
		std::string		serv = lin.substr(0, pos);
		lin.erase(0, pos + 1);
		if (count(serv.begin(), serv.begin() + pos, '/') != 2)
		{
			logStream->debugStream() << "upd_name : 2 '/' in the name expected, " << lin.substr(0, pos) << log4cpp::eol;
			throw long();
		}
		pos = serv.rfind('/');
		serv.erase(pos);
		logStream->debugStream() << "Device list = " << lin << log4cpp::eol;
		try
		{
			upd_name(serv, lin, 1);
		}
		catch(const long err)
		{
			psdev_back.psdev_err = i + 1;
			psdev_back.error_code = err;
			return(&psdev_back);
		}
    	}
//
// return data
//
	return(&psdev_back);
}


/**
 * To update the NAMES table in the database
 * 
 * @param serv The server name. The first time this function is called for a device list, 
 *		this buffer also contains the DS name and personal name
 * @param dev_name The device name
 * @param ind The device index in the list (starting with 1)
 * 
 * @throw error code 
 * 
 * @return This function returns 0 if no errors occurs or the error code when there is a problem.
 */
long NdbmServer::upd_name(std::string serv, std::string dev_name, int ind) throw (long)
{
	std::vector<dena>	tab_dena;
	device 			dev; 

	std::string::size_type	pos;
	char 			prgnr[20],
    				seqnr[4];
	GDBM_FILE 		tup;
	static datum 		key = {NULL, 0}, 
				key_sto = {NULL, 0}, 
				key_sto2 = {NULL, 0}, 
				key_2 = {NULL, 0},
				content = {NULL, 0}, 
				cont_sto = {NULL, 0};
	static int 		ndev = 0;
	static long	 	key_sto_base_length = 0;

//
// Allocate memory for the dbm-structures (key and content)
//
	tab_dena.clear();
	try
	{
		if (key_sto.dptr == NULL)
                       key_sto.dptr = new char[MAX_KEY];
		if (key_2.dptr == NULL)
                       key_2.dptr = new char[MAX_KEY];
		if (content.dptr == NULL)
                       content.dptr = new char[MAX_CONT];
	}
	catch(std::bad_alloc)
	{
		delete [] key_sto.dptr;
		delete [] key_2.dptr;
		delete [] content.dptr;
		throw long(DbErr_ServerMemoryAllocation);
	}
//
// Get device server class
//
	pos = serv.find('/');
	strcpy(dev.ds_class, serv.substr(0, pos).c_str());
	strcpy(key_sto.dptr, dev.ds_class);
	strcat(key_sto.dptr, "|");
//
// Get device server name 
//
	strcpy(dev.ds_name, serv.substr(pos + 1).c_str());
	strcat(key_sto.dptr, serv.substr(pos + 1).c_str());
	strcat(key_sto.dptr, "|");
	key_sto.dsize = strlen(key_sto.dptr);
	key_sto_base_length = key_sto.dsize;
//
// Call the del_name function
//
	try
	{
		del_name(dev, ndev, dev_name, tab_dena);
	}
	catch(...)
	{
		logStream->errorStream() << "Could not delete " << dev_name << log4cpp::eol;
		return(-1);
	}

	logStream->debugStream() << "Device server class : " << serv.substr(0, pos) << log4cpp::eol;
	logStream->debugStream() << "Device server name : " << serv.substr(pos + 1) << log4cpp::eol;
//
// Check, if the only device server is to be removed  
//
	if (dev_name != "%")
	{
//
// Initialize the new tuple with the right pn and vn values 
//
		int                     i;
		int                     nb_dev = count(dev_name.begin(), dev_name.end(), ',') + 1;
		std::string             ptr1 = dev_name;
		for(int j = 0; j < nb_dev; ++j)
		{
			std::string::size_type  pos = ptr1.find(',');
			strcpy(content.dptr, ptr1.substr(0, pos).c_str());
			strcat(content.dptr, "|");
			ptr1.erase(0, pos + 1);
			for (i = 0; i < ndev && strcmp(dev.d_name,tab_dena[i].devina); ++i);
			if (i == ndev)
			{
//
// Initialize the content for dbm-database 
//
				strcat(content.dptr, "not_exp|0|0|unknown|unknown|0|unknown|");
			}
			else
			{
//
// Initialize the content for dbm-database 
//
				std::stringstream       s;
#if !HAVE_SSTREAM
				s.seekp(0, std::ios::beg);
#endif
				s << tab_dena[i].oh_name << "|" << tab_dena[i].opn << "|" << tab_dena[i].ovn << "|" 
					<< tab_dena[i].od_class << "|" << tab_dena[i].od_type << "|" << tab_dena[i].opid 
					<< "|" << tab_dena[i].od_proc << "|";

				logStream->debugStream() << " update_name " << s.str() << log4cpp::eol;

#if !HAVE_SSTREAM
				strcat(content.dptr, s.str());
        			s.freeze(false);
#else
				strcat(content.dptr, const_cast<char *>(s.str().c_str()));
#endif
			}
			content.dsize = strlen(content.dptr);
			snprintf(seqnr, sizeof(seqnr), "%d", j + 1);
			key_sto.dptr[key_sto_base_length] = '\0';
			strcat(key_sto.dptr, seqnr);
			strcat(key_sto.dptr, "|");
			key_sto.dsize = strlen(key_sto.dptr);
//
// Insert tuple in NAMES table
//
			logStream->debugStream() << "Insert tuple in NAMES table " << key_sto.dptr << " " << content.dptr << log4cpp::eol;

			key_sto2 = key_sto;
			cont_sto = content;

			if ((i = gdbm_store(dbgen.tid[0], key_sto2, cont_sto, GDBM_INSERT)) != 0)
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
	}
//
// Leave function 
//
	return(0);
}



/**
 * To delete (in the database) all the devices for a specific device server.
 * 
 * @param devi The device server network name
 * @param ptr
 * @param buf The address of a buffer where is saved all the device's names
 *            in charge of a device server.  The string follows this format :
 *            D.S. network name:device name 1,device name 2,...
 *
 * @param pndev The number of device that have been deleted 
 * 
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
long NdbmServer::del_name(device &devi, int &pndev, std::string ptr, std::vector<dena> &buf) throw (long)
{
	int 	j,
		l,
		tp;
	datum 	key;
	int 	seq = 1;
	long 	nb_dev;
//
// Remove all the devices already registered in the database with the same
// device server/personal name 
// 
	int 	i = 0;
	key.dptr = new char[MAX_KEY];
	do
	{
//
// Build key 
//
		std::stringstream	s;
#if !HAVE_SSTREAM
        	s.seekp(0, std::ios::beg);
#endif
		s << devi.ds_class << "|" << devi.ds_name << "|" << seq << "|" << std::ends;

		logStream->debugStream() << "del_name : " << s.str() << log4cpp::eol;

#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
        	s.freeze(false);
#else
		strcpy(key.dptr, const_cast<char *>(s.str().c_str()));
#endif
		key.dsize = strlen(key.dptr);

		logStream->debugStream() << key.dsize << " " << key.dptr << log4cpp::eol;
//
// Try to get data out of database
//
		try
		{
			NdbmNamesCont	cont(dbgen.tid[0], key);
//
// Copy all the database information in a "dena" structure 
//
			dena			_dena;

			strcpy(_dena.devina, cont.get_device_name().c_str());
			strcpy(_dena.oh_name, cont.get_host_name().c_str());
			_dena.opn = cont.get_p_num();
			_dena.ovn = cont.get_v_num();
			strcpy(_dena.od_type, cont.get_device_type().c_str());
			strcpy(_dena.od_class, cont.get_device_class().c_str());
			_dena.opid = cont.get_pid();
			strcpy(_dena.od_proc, cont.get_process_name().c_str());
			i++;
			buf.push_back(_dena);
//
// Delete database entry
//
			if (gdbm_delete(dbgen.tid[0], key))
				throw long(0);
			seq++;
		}
		catch ( ... )
		{
			if (gdbm_error(dbgen.tid[0]) == 0)
				break;
			else
			{
				delete [] key.dptr;
				throw long(DbErr_DatabaseAccess);
			}
		}
	}while (true);
	delete [] key.dptr;
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
	    	std::string ptr1 = ptr;
    		for(int j = 0; j < nb_dev; ++j)
    		{
			std::string::size_type  pos = ptr1.find(',');
			strcpy(ptr_dev[j].dev_name, ptr1.substr(0, pos).c_str());
			ptr_dev[j].found = False;
			ptr1.erase(0, pos + 1);
	    	}
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
					key.dsize = strlen(key.dptr);
					gdbm_delete(dbgen.tid[0], key);
					update_dev_list(ptr_dev[j].dev_info, ptr_dev[j].seq + 1);
	    			}
	}
	catch(const std::bad_alloc &)
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

/**
 * To check if a device is known in the database
 * 
 * @param ptr The device name
 * @param nb_dev
 * 
 * @return 0 if there is a device in the database
 *    If the device is not defined in the database, the function returns
 *    ERR_DEV_NOT_FD. In case of failure, the function returns ERR_DEVNAME
 */
long NdbmServer::is_dev_in_db(db_dev_in_db *ptr,long nb_dev) throw (long)
{
	device 		dev;
	datum		key2;

	for (datum key = gdbm_firstkey(dbgen.tid[0]); 
		key.dptr != NULL;
		key2 = key, key = gdbm_nextkey(dbgen.tid[0], key2), free(key2.dptr))
	{
		NdbmNamesKey	namesKey(key);
//
// Extract dserver name
//
		strcpy(dev.ds_class, namesKey.get_ds_name().c_str());
//
// Extract personal name
//
		strcpy(dev.ds_name, namesKey.get_ds_pers_name().c_str());
//
// Make a copy of the key
//
		std::string	key_buf(key.dptr, key.dsize);
//
// Get db content 
//
		try
		{
			NdbmNamesCont	cont(dbgen.tid[0], key);
			strcpy(dev.d_name, cont.get_device_name().c_str());
//
// Wanted device ? 
//
			for (long j = 0;j < nb_dev;j++)
				if (dev.d_name == ptr[j].dev_name)
				{
					ptr[j].found = True;
					ptr[j].seq = namesKey.get_dev_indi();
					strcpy(ptr[j].key_buf, key_buf.c_str());
					ptr[j].dev_info = dev;
				}
		}
		catch ( ... )
		{
			if (gdbm_error(dbgen.tid[0]) == 0)
				break;
			else
				throw long(ERR_DEV_NOT_FD);
		}
	}
	return(0);
}

/**
 * To update a device server device list when one of the device is removed from the list. 
 * This means to update the sequence field of all the remaining devices
 * 
 * @param p_ret The removed device structure with the device information
 * @param seq The sequence field of the first device to be updated
 *
 * @return 0 if there is no problem. Otherwise, it returns  ERR_DEVNAME
 */
long NdbmServer::update_dev_list(device &p_ret, long seq) throw (long)
{
	datum 	key;
//
// Miscellaneous init 
//
	logStream->debugStream() << p_ret.ds_class << " " << p_ret.ds_name << log4cpp::eol;
	std::string key_buf = std::string(p_ret.ds_class) + "|" + std::string(p_ret.ds_name) + "|";

	logStream->debugStream() << "before loop in update-dev_list function" << log4cpp::eol;

	key.dptr = new char [MAX_KEY];
	do
	{
//
// Build the key 
//
#if !HAVE_SSTREAM
		std::stringstream	s(const_cast<char *>(key_buf.c_str()), key_buf.length());
        	s.seekp(0, std::ios::beg);
#else
		std::stringstream	s(key_buf);
#endif
		s << seq << "|" <<  std::ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
        	s.freeze(false);
#else
		strcpy(key.dptr, const_cast<char *>(s.str().c_str()));
#endif
		key.dsize = strlen(key.dptr);
//
// Tried to get data from the database 
//
		try
		{
			NdbmNamesCont	cont(dbgen.tid[0],key);	
//
// Delete the entry and store a new one with a modifed sequence field 
//
			if (gdbm_delete(dbgen.tid[0], key) != 0)
				throw long(ERR_DEVNAME);
#if !HAVE_SSTREAM
			s.seekp(0, std::ios::beg);
			s << key_buf;
#else
			s.str(key_buf);
#endif
			s << (seq - 1) << "|" << std::ends;;
#if !HAVE_SSTREAM
			strcpy(key.dptr, s.str());
			s.freeze(false);
#else
			strcpy(key.dptr, const_cast<char *>(s.str().c_str()));
#endif
			key.dsize = strlen(key.dptr);

			if (gdbm_store(dbgen.tid[0], key, cont.get_datum(), GDBM_INSERT) != 0)
				throw long(ERR_DEVNAME);
			seq++;
		}
		catch ( ... )
		{
			if (gdbm_error(dbgen.tid[0]) == 0)
				break;
			else
			{
				delete key.dptr;
				throw long(ERR_DEV_NOT_FD);
			}
		}
	}while(true);
	delete [] key.dptr;

	logStream->debugStream() << "after loop in update-dev_list function" << log4cpp::eol;

	return(0);
}




/**
 * To update resource(s)
 * 
 * @param res_list The resource list 
 * 
 * @return a pointer to a structure with all device info and an error code which is set if needed
 */
db_psdev_error *NdbmServer::updres_1_svc(db_res *res_list)
{
	long 	list_nb = res_list->res_val.arr1_len,
		ind = 1,
		i;

	logStream->debugStream() << "In updres_1_svc function for " << list_nb << " resource(s)" << log4cpp::eol;
//
// Initialize parameter sent back to client 
//
	psdev_back.error_code = 0;
	psdev_back.psdev_err = 0;
//
// A loop on each resource 
//
	try
	{
    		for (i = 0; i < list_nb; i++)
    		{
//
// Allocate memory for strtok pointers 
//
			std::string lin = res_list->res_val.arr1_val[i];
			std::string::size_type	pos = 0;

			logStream->debugStream() << "Resource list = " << lin << log4cpp::eol;
//
// Only one update if the resource is a simple one 
//
			if ((pos = lin.find(SEP_ELT)) == std::string::npos)
				upd_res(lin, 1, false);
			else
			{		
				ind = 1;
				do 
				{
					upd_res(lin.substr(0, pos), ind, true); 
					lin.erase(0, pos + 1);
					ind++;	
				}while((pos = lin.find(SEP_ELT)) != std::string::npos);
				upd_res(lin, ind, true); 
	    		}
		}
	}
	catch(const std::bad_alloc &)
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


/**
 * To update a resource in the appropriate table in database 
 * 
 * @param lin A pointer to the modified resource definition (without space and tab characters)
 * @param numb The number of the resource in the array (one if the resource type is not an array)
 * @param array A flag to inform the function that this resource is a member of an array 
 *
 * @return 0 if no errors occurs or the error code when there is a problem.
 */
long NdbmServer::upd_res(const std::string &lin, const long numb, bool array) throw (long)
{
	static std::string 	t_name;
	std::string::size_type	pos,
				last_pos;
	int	 		i,
				l,
				indi,
				resu;
	static GDBM_FILE 	tab;
	static datum 		key = {NULL, 0},
     				content = {NULL, 0},
    				key_array = {NULL, 0};
	int 			res_numb;
	static bool 		sec_res;
	int 			ctr = 0;
	bool 			old_res_array;	
	static std::string	family,
				member,
				r_name,
				r_val;

	if (numb == 1)
	{
		if (key.dptr == NULL)
    			key.dptr = new char[MAX_KEY];
		if (content.dptr == NULL)
    			content.dptr = new char[MAX_CONT];
		if (key_array.dptr == NULL)
			key_array.dptr = new char[MAX_KEY];
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
			std::transform(r_name.begin(), r_name.end(), r_name.begin(), DBServer::make_sec);
//
// Select the right resource table in database 
//
		for (i = 1;i < dbgen.TblNum;i++)
			if (t_name == dbgen.TblName[i])
			{
				tab = dbgen.tid[i];
				break;
			}
		if (i == dbgen.TblNum)
			throw long(DbErr_DomainDefinition);

		logStream->debugStream() << "Table name : " << t_name << log4cpp::eol
				<< "Family name : " << family << log4cpp::eol
				<< "Number name : " << member << log4cpp::eol
				<< "Resource name : " << r_name << log4cpp::eol
				<< "Resource value : " << r_val << log4cpp::eol
				<< "Sequence number : " << indi << log4cpp::eol;
//
// Get resource value (resource values are stored in the database as case dependent strings)
//
		r_val = lin.substr(pos + 1);
	}
	else
		r_val = lin;
//
// For security domain, change every occurance of | by a ^ 
//
	if (sec_res)
		std::transform(r_val.begin(), r_val.end(), r_val.begin(), DBServer::make_sec);
//
// Initialise resource number 
//
	indi = numb;
//
// Try to retrieve the right tuple in table 
//
	try
	{
    		res_numb = numb;
		std::stringstream	s;
#if !HAVE_SSTREAM
        	s.seekp(0, std::ios::beg);
#endif
		s << family << "|" << member << "|" << r_name << "|" << numb << "|" << std::ends;
#if !HAVE_SSTREAM
        	strcpy(key.dptr, s.str());
        	s.freeze(false);
#else
        	strcpy(key.dptr, s.str().c_str());
#endif
    		key.dsize = strlen(key.dptr);
//
// If the resource value is %, remove all the resources.
// If this function is called for a normal resource, I must also 
// remove all the old resources with the old name. This is necessary if there
// is an update of a resource which was previously an array 
//
    		if (r_val == "%" || !array)
    		{
			while(1)
			{
#if !HAVE_SSTREAM
				s.seekp(0, std::ios::beg);
#else
				s.str("");
#endif
				s << family << "|" << member << "|" << r_name << "|" << res_numb << "|" << std::ends;
#if !HAVE_SSTREAM
				strcpy(key.dptr, s.str());
				s.freeze(false);
#else
				strcpy(key.dptr, s.str().c_str());
#endif
    				key.dsize = strlen(key.dptr);
				try
				{
					NdbmResCont	cont(tab, key);
					ctr++;
				}
				catch ( ... )
				{
					if (gdbm_error(tab) == 0)
						break;
					else
					{
						gdbm_clearerr(tab);
						throw long(DbErr_DatabaseAccess);
					}
				}
//
// The resource already exists in db. Check if one element with index 2 also
// exists. It it is the case, the resource is an array 
//
				if (ctr == 1)
				{
#if !HAVE_SSTREAM
					s.seekp(0, std::ios::beg);
#else
					s.str("");
#endif
					s << family << "|" << member << "|" << r_name << "|2|" << std::ends;
#if !HAVE_SSTREAM
					strcpy(key_array.dptr, s.str());
					s.freeze(false);
#else
					strcpy(key_array.dptr, s.str().c_str());
#endif
					key_array.dsize = strlen(key_array.dptr);

					try
					{
						NdbmResCont	cont(tab, key_array);
						old_res_array = true;
					}
					catch ( ... )
					{
						if (gdbm_error(tab) == 0)
							old_res_array = false;
						else
						{
							gdbm_clearerr(tab);
							throw long(DbErr_DatabaseAccess);
						}
					}
				}
				gdbm_delete(tab, key);
				res_numb++;
			}
			if (r_val == "%") 
				return(0);
    		}
//
// Insert a new tuple 
//
#if !HAVE_SSTREAM
		s.seekp(0, std::ios::beg);
#else
		s.str("");
#endif
		s << family << "|" << member << "|" << r_name << "|" << numb << "|" << std::ends;
#if !HAVE_SSTREAM
		strcpy(key.dptr, s.str());
		s.freeze(false);
#else
		strcpy(key.dptr, s.str().c_str());
#endif
		key.dsize = strlen(key.dptr);

		strcpy(content.dptr, r_val.c_str());
		content.dsize = r_val.length();

		switch(gdbm_store(tab, key, content, GDBM_REPLACE))
		{
			case 0 : break;
			case 1 : throw long (DbErr_DoubleTupleInRes);
			default: throw long (DbErr_DatabaseAccess);
		}
	}
	catch(const long err)
	{
		throw err;
	}
	catch(const std::bad_alloc &)
	{
		throw long(DbErr_ServerMemoryAllocation);
	}
	return(0);
}

