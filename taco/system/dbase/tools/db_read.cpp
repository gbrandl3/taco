#include <cstdio>
#include <string>
#include <cstdlib>
#include <sys/wait.h>

#define _db_setup_h
#include <API.h>
#undef _db_setup_h
#include "db_setup.h"

/* For database only */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include "config.h"
#include <gdbm.h>

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

/* Function definitions */

int db_read(char *,char *);

/* Global variables definitions */

int line_ptr;


/****************************************************************************
*                                                                           *
*		Code for db_read command 				    *
*                        -------                                            *
*                                                                           *
*    Command rule : To read all device servers and resources of a domain    *
*		    (table) or all domains (tables)                         *
*                   in the order they are stored in dbm database.           *
*                   The database directory has to be given by the           *
*                   environment variable DBM_DIR.                           *
*                                                                           *
*    Synopsis : db_read [domain/all]                                        *
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
	return 1;
}

int db_read(char *dbm_file,char *TblName)
{
	int 		flags = GDBM_READER | GDBM_NOLOCK;
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
			strncpy(key_out.dptr, key.dptr, key.dsize);
			key_out.dptr[key.dsize] = '\0';
			cout << TblName << ": " << key_out.dptr << ": " << content_out.dptr << endl;
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
