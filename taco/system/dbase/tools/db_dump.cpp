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

#include <iostream>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

/* Function definitions */

int db_read(char *,char *);

/* Global variables definitions */

int line_ptr;
std::map<std::string, int> device_map;
std::map<std::string,int>::iterator ipos;


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

/* Argument test and domain name modification */

	if (argc != 2)
	{
		cerr <<  argv[0] << " usage: " << argv[0] << " <domain name|all>" << endl;
		exit(-1);
	}

	string	domain(argv[1]);
	transform(domain.begin(), domain.end(), domain.begin(), ::tolower);

/* Find the dbm_database table names */        

	if ((ptr = (char *)getenv("DBTABLES")) == NULL)
	{
		cerr << "db_read: Can't find environment variable DBTABLES" << endl;
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
		cerr << "db_read: Can't find environment variable DBM_DIR" << endl;
		exit(-1);
	}
	string dbm_dir(ptr);

	if (dbm_dir[dbm_dir.length() - 1] != '/')
		dbm_dir += '/';

/* Read the database tables of the database */
   	for (int i = 0; i < TblNum; i++)
	{
		if ((domain == string(tblname[i])) || (domain == "all")) 
		{
			string dbm_file(dbm_dir);
			dbm_file +=  string(tblname[i]);
			res_num[i] = db_read(const_cast<char *>(dbm_file.c_str()), tblname[i]);
			if (domain != "all") 
				return 0;
		}
	}
	for (ipos = device_map.begin(); ipos != device_map.end(); ipos++)
	{
		if (ipos->second > 1)
		{
			cout << "WARNING - the following device " << ipos->first << " has " << ipos->second << " entries in the NAMES table !\n";
		}
	}
	return 1;
}

int db_read(char *dbm_file,char *TblName)
{
	//int 		flags = GDBM_READER | GDBM_NOLOCK;
	int 		flags = 0666;
	GDBM_FILE	tab_tid;
	datum 		key,
        		key_out;
	datum 		content,
        		content_out;
	int 		res_num = 0;
	long 		err;
#ifdef linux
	static long 	connected = False;
#endif /* linux */
	string		key_str, content_str, device_tmp;
	string		domain, family, member, personal_name, process;
	string		device, host, program_no, pid, server;
	string		version, device_type, device_class, count;
	string		name, value, poll_rate;
	string::size_type pos;
	
	key_out.dptr = (char *)malloc(MAX_KEY);
	content_out.dptr = (char *)malloc(MAX_CONT);

#if 0 //def linux
//
// Connect process to db if it is not already done 
//
	if (connected == False)
	{
		if (db_import(&err) != 0)
		{
	    		cerr << "db_read: can't connect to database server" << endl;
			exit(-1);
		}
		connected = True;
	}
//
// Ask server to disconnect from DBM files 
//
	if (db_svc_close(&err) == -1)
	{
		cerr << "db_read: Server failed when tries to disconnect to DBM files" << endl;
		exit(-1);
	}	
#endif /* linux */
//
// Open database file 
//
	tab_tid = gdbm_open(dbm_file, 0, flags, (int)0666, NULL);

	if (tab_tid == NULL)
	{
		cerr << "db_read: Can't open " << dbm_file << " table" << endl;
#if 0 //def linux
//
// Ask server to disconnect from DBM files 
//
		if (db_svc_reopen(&err) == -1)
	   		cerr << "db_read: Server failed when tries to reconnect to DBM files" << endl;
#endif /* linux */		
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
//			cout << TblName << ": " << key_out.dptr << ": " << content_out.dptr << endl;
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
				cout << "INSERT INTO device SET "
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
				     	cout << ", exported=1";
				     }
				     else
				     {
				     	cout << ", exported=0";
				     }
				     cout << ";" << endl;
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


					cout << "INSERT INTO device SET "
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
				     	<< ";" << endl;
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
					while (pos != string::npos)
					{
						value.insert(pos,"\\");
						pos = value.find("'",pos+2);
					}
					cout << "INSERT INTO property_device SET "
					     << "device='" << domain <<"/" << family << "/" << member << "',"
					     << "domain='" << domain << "',"
					     << "family='" << family << "',"
					     << "member='" << member << "',"
					     << "name='" << name << "',"
					     << "count='" << count << "',"
					     << "value='" << value << "'" 
					     << ";" << endl;
				}
			}
		}
	}
//
// Close database 
//
	gdbm_close(tab_tid);

	free(key_out.dptr);
	free(content_out.dptr);
#if 0 //def linux
//
// Ask server to reconnect to DBM files 
//
	if (db_svc_reopen(&err) == -1)
	{
		cerr << "db_read: Server failed when tries to reconnect to DBM files" << endl;
		exit(-1);
	}	
#endif /* linux */
	return(res_num);
}
