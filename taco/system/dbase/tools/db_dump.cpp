/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 ESRF, www.esrf.fr
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
 * Version:     $Revision: 1.8 $
 *
 * Date:        $Date: 2008-04-06 09:07:47 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <cstdio>
#include <string>
#include <cstdlib>
#include <sys/wait.h>

#include <API.h>

/* For database only */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <gdbm.h>
#include <ndbm.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <map>

/* Function definitions */

int db_read(char *,char *);
void analyse_content(char *, datum *, datum *);

/* Global variables definitions */

int line_ptr;
std::map<std::string, int> device_map;
std::map<std::string, int> property_map;
std::map<std::string,int>::iterator ipos;
int mysql_statements = 0;
const char *dbase = "gdbm";

void usage(const char *cmd)
{
	std::cerr <<  "usage: " << cmd << " [options] <domain name|all>" << std::endl;
	std::cerr << " dumps all resources of the specified or all domains." << std::endl; 
	std::cerr << "        options : -h display this message" << std::endl;
	std::cerr << "                  -v display the current version" << std::endl;
	exit(1);
}

void version(const char *cmd)
{
	std::cerr << cmd << " version " << VERSION << std::endl;
	exit(0);
}

/****************************************************************************
*                                                                           *
*		Code for db_dump command 				    *
*                        -------                                            *
*                                                                           *
*    Command rule : To dump all device servers and resources of a domain    *
*		    (table) or all domains (tables) in SQL statements to    *
*                   be able to be imported into the tango MySQL database    *
*                   The database directory has to be given by the           *
*                   environment variable DBM_DIR.                           *
*                                                                           *
*    Synopsis : db_dump [domain/all]                                        *
*                                                                           *
****************************************************************************/

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
	int 	res_num[MAXDOMAIN];
	char 	TblName[MAXDOMAIN][12];
	char 	tblname[MAXDOMAIN][14];
	int 	TblNum = 0;
	int 	names = False;
	int 	ps_names = False;

        extern int      optopt;
        extern int      optind;
	extern char	*optarg;
        int             c;
//
// Argument test and device name structure
//
        while ((c = getopt(argc,argv,"hv")) != -1)
        {
                switch (c)
                {
			case 'v':
				version(argv[0]);
				break;
                	case 'h':
                	case '?':
				usage(argv[0]);
                }
        }
    	if (optind != argc - 1)
		usage(argv[0]);

	std::string	domain(argv[optind]);
	std::transform(domain.begin(), domain.end(), domain.begin(), ::tolower);

/* Find the dbm_database table names */        
	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		std::cerr << "db_read: Can't find environment variable DBTABLES" << std::endl;
		exit(-1);
	}
	
	tbeg = ptr;
	while ((tend = (char *)strchr(tbeg,',')) != NULL)
	{
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(TblName[TblNum], tbeg, diff);
		TblName[TblNum][diff] = '\0';
		TblNum++;
		tbeg = tend;
	}
	strcpy (TblName[TblNum++], tbeg);
	
/* Change the database table names to lowercase letter names and check if there 
   is a names and ps_names tables defined */
	
	for (i = 0;i < TblNum;i++)
	{
		for (j = 0;j < (int)strlen(TblName[i]);j++)
			tblname[i][j] = tolower(TblName[i][j]);
		tblname[i][j] = '\0';
		if (strcmp(tblname[i],"names") == 0)
			names = True;
		if (strcmp(tblname[i],"ps_names") == 0)
			ps_names = True;
	}
	
	/* If no names or ps_names tables are defined, add them to the list */
	
	if (names == False)
	{
		strcpy(tblname[TblNum],"names");
		TblNum++;
	}
	if (ps_names == False)
	{
		strcpy(tblname[TblNum],"ps_names");
		TblNum++;
	}	
	
/* Take the environment variable DBM_DIR */

	if ((ptr = getenv("DBM_DIR")) == NULL)
	{
		std::cerr << "db_read: Can't find environment variable DBM_DIR" << std::endl;
		exit(-1);
	}
	std::string dbm_dir(ptr);

	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += '/';

/* Try both database formats gdbm + dbm */

	for (int i_dbase=0; i_dbase<2 && mysql_statements==0; i_dbase++)
	{
	
	/* Read the database tables of the database */
	   	for (int i = 0; i < TblNum; i++)
		{
			if ((domain == std::string(tblname[i])) || (domain == "all")) 
			{
				std::string dbm_file(dbm_dir);
				dbm_file +=  std::string(tblname[i]);
				res_num[i] = db_read(const_cast<char *>(dbm_file.c_str()), tblname[i]);
			}
		}
		for (ipos = device_map.begin(); ipos != device_map.end(); ipos++)
		{
			if (ipos->second > 1)
			{
				std::cerr << "WARNING - the following device " << ipos->first << " has " << ipos->second << " entries in the NAMES table !\n";
			}
		}
		dbase = "dbm";
	}
	if (mysql_statements == 0)
	{
		std::cerr << "WARNING - could not find any data in tables, are you sure the tables are correct ?\n";
	}
	else
	{
		std::cerr << "INFORMATION - a total of " << mysql_statements << " mysql statements were generated \n";
	}
	return 1;
}

int db_read(char *dbm_file,char *TblName)
{
	//int 		flags = GDBM_READER | GDBM_NOLOCK;
	int 		flags = 0666;
	GDBM_FILE	gdbm_tab_tid;
	DBM		*dbm_tab_tid;
	datum 		key,
        		key_out;
	datum 		content,
        		content_out;
	int 		res_num = 0;
	long 		err;
#ifdef linux
	static long 	connected = False;
#endif /* linux */
	std::string		key_str, content_str, device_tmp;
	std::string		domain, family, member, personal_name, process;
	std::string		device, host, program_no, pid, server, device_property;
	std::string		version, device_type, device_class, count;
	std::string		name, value, poll_rate;
	std::string::size_type pos;
	

#if 0 //def linux
//
// Connect process to db if it is not already done 
//
	if (connected == False)
	{
		if (db_import(&err) != 0)
		{
	    		std::cerr << "db_read: can't connect to database server" << std::endl;
			exit(-1);
		}
		connected = True;
	}
//
// Ask server to disconnect from DBM files 
//
	if (db_svc_close(&err) == -1)
	{
		std::cerr << "db_read: Server failed when tries to disconnect to DBM files" << std::endl;
		exit(-1);
	}	
#endif /* linux */
//
// Open database file 
//
	gdbm_tab_tid = NULL;
	dbm_tab_tid = NULL;

	if (strcmp(dbase,"gdbm") == 0)
	{
		gdbm_tab_tid = gdbm_open(dbm_file, 0, flags, (int)0666, NULL);
	}
	else
	{
		dbm_tab_tid = dbm_open(dbm_file, flags, (int)0666);
	}

	if (gdbm_tab_tid == NULL && dbm_tab_tid == NULL)
	{
#if 0 //def linux
//
// Ask server to disconnect from DBM files 
//
		if (db_svc_reopen(&err) == -1)
	   		std::cerr << "db_read: Server failed when tries to reconnect to DBM files" << std::endl;
#endif /* linux */		
		return 0;
	}
//
// Display table contents 
//
        if (strcmp(dbase,"gdbm") == 0)
	{
		for (key = gdbm_firstkey(gdbm_tab_tid); key.dptr != NULL;key = gdbm_nextkey(gdbm_tab_tid, key))
		{
			content = gdbm_fetch(gdbm_tab_tid, key);
			analyse_content(TblName, &key, &content);
		}
	}
	else
	{
		for (key = dbm_firstkey(dbm_tab_tid); key.dptr != NULL;key = dbm_nextkey(dbm_tab_tid))
		{
			content = dbm_fetch(dbm_tab_tid, key);
			analyse_content(TblName, &key, &content);
		}
	}
//
// Close database 
//
        if (strcmp(dbase,"gdbm") == 0)
	{
		gdbm_close(gdbm_tab_tid);
	}
	else
	{
		dbm_close(dbm_tab_tid);
	}

#if 0 //def linux
//
// Ask server to reconnect to DBM files 
//
	if (db_svc_reopen(&err) == -1)
	{
		std::cerr << "db_read: Server failed when tries to reconnect to DBM files" << std::endl;
		exit(-1);
	}	
#endif /* linux */
	return(res_num);
}

void analyse_content(char *TblName, datum *key, datum *content)
{
	datum 		key_out,
        		content_out;
	static int 	res_num = 0;
	long 		err;
#ifdef linux
	static long 	connected = False;
#endif /* linux */
	std::string		key_str, content_str, device_tmp;
	std::string		domain, family, member, personal_name, process;
	std::string		device, host, program_no, pid, server, device_property;
	std::string		version, device_type, device_class, count;
	std::string		name, value, poll_rate;
	std::string::size_type pos;

	key_out.dptr = (char *)malloc(MAX_KEY);
	content_out.dptr = (char *)malloc(MAX_CONT);
	if (content->dptr != NULL)
	{
		res_num++;
		strncpy((char*)content_out.dptr, (char*)content->dptr, content->dsize);
		((char*)content_out.dptr)[content->dsize] = '\0';
		content_str = (char*)content_out.dptr;
		strncpy((char*)key_out.dptr, (char*)key->dptr, key->dsize);
		((char*)key_out.dptr)[key->dsize] = '\0';
		key_str = (char*)key_out.dptr;
//		std::cout << TblName << ": " << key_out.dptr << ": " << content_out.dptr << std::endl;
		if (strcmp(TblName,"names") == 0)
		{

			pos = key_str.find("|");
			server = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("|");
			personal_name = key_str.substr(0,pos);
			key_str = key_str.substr(pos+1);
			pos = key_str.find("|");
			count = key_str.substr(0,pos);

			pos = content_str.find("|");
			device = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			host = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			program_no = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			version = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			device_type = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			device_class = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			pid = content_str.substr(0,pos);
			content_str = content_str.substr(pos+1);
			pos = content_str.find("|");
			process = content_str.substr(0,pos);

			device_tmp = device;
			pos = device_tmp.find("/");
			domain = device_tmp.substr(0,pos);
			device_tmp = device_tmp.substr(pos+1);
			pos = device_tmp.find("/");
			family = device_tmp.substr(0,pos);
			device_tmp = device_tmp.substr(pos+1);
			pos = device_tmp.find("/");
			member = device_tmp.substr(0,pos);

//
// count how many times each device is in the "names" table
//

			ipos = device_map.find(device);
			if (ipos == device_map.end())
			{
				device_map[device] = 1;
			}
			else
			{
				device_map[device] = device_map[device]+1;
			}

			std::cout << "DELETE FROM device where name='" << device << "';" << std::endl;
			std::cout << "INSERT INTO device SET "
			     << "name='" << device << "',"
			     << "domain='" << domain << "',"
			     << "family='" << family << "',"
			     << "member='" << member << "',"
			     << "ior='rpc:" << host << ":" << program_no << "',"
			     << "host='" << host << "',"
			     << "server='" << server << "/" << personal_name << "',"
			     << "pid='" << pid << "',"
			     << "class='" << device_class << "',"
			     << "version='" << version << "'";
			if (atoi(program_no.c_str()) != 0)
			{
			     std::cout << ", exported=1";
		     	}
		     	else
		     	{
			     	std::cout << ", exported=0";
		     	}
		    	std::cout << ";" << std::endl;
			mysql_statements++;
		}
		else
		{
			if (strcmp(TblName,"ps_names") == 0)
			{
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


				std::cout << "DELETE FROM device where name='" << device << "';" << std::endl;
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
				mysql_statements++;
			}
			else
			{
				domain = TblName;
				pos = key_str.find("|");
				family = key_str.substr(0,pos);
				key_str = key_str.substr(pos+1);
				pos = key_str.find("|");
				member = key_str.substr(0,pos);
				key_str = key_str.substr(pos+1);
				pos = key_str.find("|");
				name = key_str.substr(0,pos);
				key_str = key_str.substr(pos+1);
				pos = key_str.find("|");
				count = key_str.substr(0,pos);
				key_str = key_str.substr(pos+1);

				value = content_str;
				pos = value.find("'");
				while (pos != std::string::npos)
				{
					value.insert(pos,"\\");
					pos = value.find("'",pos+2);
				}
//
// count how many times this device property has been encountered 
//

				device = domain + "/" + family +  "/" +  member;
				device_property = device + "/" + name;
				ipos = property_map.find(device_property);
				if (ipos == property_map.end())
				{
					property_map[device_property] = 1;
					std::cout << "DELETE FROM property_device WHERE device='" << device << "' AND name='" << name << "';" << std::endl;
				}
				else
				{
					property_map[device_property] = property_map[device_property]+1;
				}
				std::cout << "INSERT INTO property_device SET "
				     << "device='" << domain <<"/" << family << "/" << member << "',"
				     << "domain='" << domain << "',"
				     << "family='" << family << "',"
				     << "member='" << member << "',"
				     << "name='" << name << "',"
				     << "count='" << count << "',"
				     << "value='" << value << "'" 
				     << ";" << std::endl;
				mysql_statements++;
			}
		}
	}
	free(key_out.dptr);
	free(content_out.dptr);
}
