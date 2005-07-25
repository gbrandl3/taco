/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 ESRF, www.esrf.fr
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
 * File:	db_dump.cpp
 *
 * Description: To dump all device servers and resources of a domain (table) or
 *		all domains (tables) in SQL statements to be able to be imported 
 *		into the tango MySQL database. The database directory has to be 
 *		given by the environment variable DBM_DIR. 
 * 		Synopsis : db_dump [domain/all] 
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2005-07-25 11:31:53 $
 */

#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <sys/wait.h>

#define _db_setup_h
#include <API.h>
#undef _db_setup_h
#include "db_setup.h"

// For database only 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <gdbm.h>

#include <iostream>
#include <algorithm>

// Function definitions 
static int db_dump_resources(std::string, std::string);
static int db_dump_names(std::string);
static int db_dump_psnames(std::string);

// Global variables definitions 
int line_ptr;

int main(int argc,char *argv[])
{
	char 	*ptr,
		*tbeg,
       		*tend;
	int 	diff;
	int 	flags;
	FILE 	*fop;
    	int 	i,
        	j,
        	l;
	int 	res_num;
	std::vector<std::string> tblname;
	int 	names = False;
	int 	ps_names = False;
	extern char 	*optarg;
	extern int 	optind, 
			opterr, 
			optopt;
	int 		c;


// Argument test and domain name modification 
	while((c = getopt(argc, argv, "h")) != -1)
	{
		switch(c)
		{
			case 'h':
			case '?':
				std::cerr << " usage: " << argv[0] << " [options] [domain name]" << std::endl;
				std::cerr << " dumps all servers and its devices and all resources" << std::endl;
				std::cerr << "         options : -h display this message" << std::endl;
				exit(-1);
		}
	}

	std::string	domain;
	if (argc > 1)
		domain = argv[optind];
	else
		domain = "all";

	transform(domain.begin(), domain.end(), domain.begin(), ::tolower);

// Find the dbm_database table names 
	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		std::cerr << "db_dump: Can't find environment variable DBTABLES" << std::endl;
		exit(-1);
	}

	std::string dbtables(ptr);
	transform(dbtables.begin(), dbtables.end(), dbtables.begin(), ::tolower);
	std::string::size_type pos;
	while ((pos = dbtables.find(',')) != std::string::npos)
	{
		tblname.push_back(dbtables.substr(0, pos));
		dbtables.erase(0, pos + 1);
	}
	tblname.push_back(dbtables);
	
// Change the database table names to lowercase letter names and check if there 
// is a names and ps_names tables defined 
	for (std::vector<std::string>::iterator i = tblname.begin();i != tblname.end(); ++i)
	{
		if (*i == "names")
			names = True;
		if (*i == "ps_names")
			ps_names = True;
	}

// If no names or ps_names tables are defined, add them to the list 
	if (names == False)
		tblname.push_back("names");
	if (ps_names == False)
		tblname.push_back("ps_names");
	
// Take the environment variable DBM_DIR 
	if ((ptr = getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "db_dump: Can't find environment variable DBM_DIR" << std::endl;
		exit(-1);
	}
	std::string dbm_dir(ptr);

	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += '/';

	unsigned int TblNum = tblname.size();
// Read the database tables of the database 
   	for (unsigned int i = 0; i < TblNum; i++)
		if ((domain == tblname[i]) || (domain == "all")) 
		{
			std::string dbm_file(dbm_dir);
			dbm_file +=  tblname[i];
			if (tblname[i] == "names")
				res_num = db_dump_names(dbm_file);
			else if (tblname[i] == "ps_names")
				res_num = db_dump_psnames(dbm_file);
			else
				res_num = db_dump_resources(dbm_file, tblname[i]);
			if (domain != "all") 
				return 0;
		}
	return 1;
}

int db_dump_names(std::string dbm_file)
{
	int 			flags = GDBM_READER | GDBM_NOLOCK;
	GDBM_FILE		tab_tid;
	datum 			key,
        			key_out;
	datum 			content,
        			content_out;
	int 			res_num = 0;
	long 			err;
	std::string		key_str, content_str;
	std::string		device, personal_name,server;
	std::string::size_type 	pos;

	std::map<std::string, std::vector<std::string> > ds; 

	key_out.dptr = new char[MAX_KEY];
	content_out.dptr = new char[MAX_CONT];
//
// Open database file 
//
	if ((tab_tid = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, (int)0666, NULL)) == NULL)
	{
		std::cerr << "db_dump: Can't open " << dbm_file << " table" << std::endl;
		exit(-1);
	}	
//
// Display table contents 
//
	for (key = gdbm_firstkey(tab_tid); key.dptr != NULL;key = gdbm_nextkey(tab_tid, key))
	{
		content = gdbm_fetch(tab_tid, key);
		if (content.dptr != NULL)
		{
			strncpy(content_out.dptr, content.dptr, content.dsize);
			content_out.dptr[content.dsize] = '\0';
			content_str = content_out.dptr;
			strncpy(key_out.dptr, key.dptr, key.dsize);
			key_out.dptr[key.dsize] = '\0';
			key_str = key_out.dptr;

			pos = key_str.find("|");
			server = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("|");
			personal_name = key_str.substr(0,pos);

			std::string server_name = server + "/" + personal_name;

			pos = content_str.find("|");
			device = content_str.substr(0,pos);

			if (ds.find(server_name) == ds.end())
			{
				std::vector<std::string> tmp;
				tmp.push_back(device);
				ds.insert(std::pair<std::string, std::vector<std::string> >(server_name, tmp));
			}			
			else
				ds[server_name].push_back(device);
		}
	}
	for (std::map<std::string, std::vector<std::string> >::iterator i = ds.begin(); i != ds.end(); ++i)
	{
		std::cout << i->first << ":\t";
		for (std::vector<std::string>::iterator j = i->second.begin(); j != i->second.end(); ++j)
		{
			if (j != i->second.begin())
//				std::cout << ",";
				std::cout << " \\" << std::endl << "\t\t";
			std::cout << *j;
		}
		std::cout << std::endl << std::endl;
	}
//
// Close database 
//
	gdbm_close(tab_tid);

	delete [] key_out.dptr;
	delete [] content_out.dptr;
	return(res_num);
}

int db_dump_psnames(std::string dbm_file)
{
	int 			flags = GDBM_READER | GDBM_NOLOCK;
	GDBM_FILE		tab_tid;
	datum 			key,
        			key_out;
	datum 			content,
        			content_out;
	int 			res_num = 0;
	long 			err;
	std::string		key_str, content_str, device_tmp;
	std::string		device, domain, family, member;
	std::string		host, pid, poll_rate;
	std::string::size_type 	pos;

	key_out.dptr = new char[MAX_KEY];
	content_out.dptr = new char[MAX_CONT];
//
// Open database file 
//
	if ((tab_tid = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, (int)0666, NULL)) == NULL)
	{
		std::cerr << "db_dump: Can't open " << dbm_file << " table" << std::endl;
		exit(-1);
	}	
//
// Display table contents 
//
	for (key = gdbm_firstkey(tab_tid); key.dptr != NULL;key = gdbm_nextkey(tab_tid, key))
	{
		content = gdbm_fetch(tab_tid, key);
		if (content.dptr != NULL)
		{
			strncpy(content_out.dptr, content.dptr, content.dsize);
			content_out.dptr[content.dsize] = '\0';
			content_str = content_out.dptr;
			strncpy(key_out.dptr, key.dptr, key.dsize);
			key_out.dptr[key.dsize] = '\0';
			key_str = key_out.dptr;
			device = key_str;

			pos = key_str.find("/");
			domain = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("/");
			family = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("/");
			member = key_str.substr(0,pos);

			pos = content_str.find("|");
			host = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			pid = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			poll_rate = content_str.substr(0,pos);

#if 0
			std::cout << "INSERT INTO device SET "
	     			<< "name='" << device << "',"
	     			<< "domain='" << domain << "',"
	     			<< "family='" << family << "',"
	     			<< "member='" << member << "',"
	     			<< "ior='DC:" << host << ":" << poll_rate << "',"
	     			<< "host='" << host << "',"
	     			<< "server='datacollector',"
	     			<< "pid='" << pid << "',"
	     			<< "class='PseudoDevice',"
	     			<< "version='1'"
	     			<< ", exported=1"
	     			<< ";" << std::endl;
#endif
		}
	}
//
// Close database 
//
	gdbm_close(tab_tid);

	delete [] key_out.dptr;
	delete [] content_out.dptr;
	return(res_num);
}

int db_dump_resources(std::string dbm_file, std::string domain)
{
	int 			flags = GDBM_READER | GDBM_NOLOCK;
	GDBM_FILE		tab_tid;
	datum 			key,
        			key_out;
	datum 			content,
        			content_out;
	int 			res_num = 0;
	long 			err;
	std::string		key_str, content_str;
	std::string		family, member, name, value;
	std::string::size_type pos;
	
	key_out.dptr = new char[MAX_KEY];
	content_out.dptr = new char[MAX_CONT];
//
// Open database file 
//
	if ((tab_tid = gdbm_open(const_cast<char *>(dbm_file.c_str()), 0, flags, (int)0666, NULL)) == NULL)
	{
		std::cerr << "db_dump: Can't open " << dbm_file << " table" << std::endl;
		exit(-1);
	}	
//
// Display table contents 
//
	for (key = gdbm_firstkey(tab_tid); key.dptr != NULL;key = gdbm_nextkey(tab_tid, key))
	{
		content = gdbm_fetch(tab_tid, key);
		if (content.dptr != NULL)
		{
			res_num++;
			strncpy(content_out.dptr, content.dptr, content.dsize);
			content_out.dptr[content.dsize] = '\0';
			content_str = content_out.dptr;
			strncpy(key_out.dptr, key.dptr, key.dsize);
			key_out.dptr[key.dsize] = '\0';
			key_str = key_out.dptr;
	
			pos = key_str.find("|");
			family = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("|");
			member = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("|");
			name = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
#if 0
			pos = key_str.find("|");
			count = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
#endif
			value = content_str;
			pos = value.find("'");
			while (pos != std::string::npos)
			{
				value.insert(pos,"\\");
				pos = value.find("'",pos+2);
			}
			std::cout << domain << "/" << family << "/" << member << "/" << name 
				<< ":	\"" << value << "\"" << std::endl;
		}
	}
	std::cout << std::endl;
//
// Close database 
//
	gdbm_close(tab_tid);

	delete [] key_out.dptr;
	delete [] content_out.dptr;
	return(res_num);
}
