#include <API.h>

#include <string.h>

#include <fcntl.h>
#include <db_xdr.h>

/* Some C++ include files */

#include <string>
#include <NdbmClass.h>
#include <iostream>
#include <NdbmServer.h>


/****************************************************************************
*                                                                           *
*		Server code for db_cmd_query function                       *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (from database) the command code           *
*		     associated to a command name (the string)		    *
*                                                                           *
*    Argin : A pointer to string which is the command name		    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "cmd_que" type   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      int db_err;    The database error code                               *
*                        0 if no error                                      *
*      unsigned int xcmd_code;	The command code.			    *
*				0 if the database query fails		    *
*            }                                                              *
*                                                                           *
****************************************************************************/
cmd_que *NdbmServer::db_cmd_query_1_svc(nam *pcmd_name)
{
	static cmd_que 	back;
	int 		i,
			found;
	datum 		key, 
			key2,
			content;
	char 		*tbeg,
			*tend;
	GDBM_FILE 	tab;
	unsigned int 	diff;
	int 		ds_num;
	int 		team;
	int 		cmd_num;
	char 		fam[40];
	char 		memb[40];
	char 		r_name[40];
	char 		cmd_str[50];

#ifdef DEBUG
	std::cout << "Command name : " << *pcmd_name << std::endl;
#endif

/* Initialize error code sended back to client */
	back.db_err = 0;
	found = False;

/* Return error code if the server is not connected to the database files */
	if (dbgen.connected == False)
	{
		back.db_err = DbErr_DatabaseNotConnected;
		back.xcmd_code = 0;
		return(&back);
	}

/* Retrieve the right "table" in the table array */

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == "cmds")
		{
			tab = dbgen.tid[i];
			break;
		}
	}

	if (i == dbgen.TblNum)
	{
		back.db_err = DbErr_DomainDefinition;
		back.xcmd_code = 0;
		return(&back);
	}

/* Try to retrieve a resource in the CMDS table with a resource value equal
   to the command name */

	for (key=gdbm_firstkey(tab);
		key.dptr!=NULL;
		key2 = key, key=gdbm_nextkey(tab, key), free(key2.dptr))
	{

		content = gdbm_fetch(tab,key);
		if (content.dptr == NULL)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
			return(&back);
		}

		strncpy(cmd_str,content.dptr,content.dsize);
		cmd_str[content.dsize] = 0;
		free(content.dptr);
		if (strcmp(cmd_str,*pcmd_name) == 0)
		{
			found = True;
			break;
		}
	}

	if (found == True)
	{

/* Get family from key */

		tbeg = key.dptr;
		tend = (char *)strchr(tbeg,'|');
		if (tend == NULL)
		{
			free(content.dptr);
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
			return(&back);
		}
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(fam,tbeg,diff);
		fam[diff] = 0;

/* Get member from key */

		tbeg = tend;

		tend = (char *)strchr(tbeg,'|');
		if (tend == NULL)
		{
			free(content.dptr);
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
			return(&back);
		}
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(memb,tbeg,diff);
		memb[diff] = 0;

/* Get resource name from key */

		tbeg = tend;

		tend = (char *)strchr(tbeg,'|');
		if (tend == NULL)
		{
			free(content.dptr);
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
			return(&back);
		}
		diff = (unsigned int)(tend++ - tbeg);
		strncpy(r_name,tbeg,diff);
		r_name[diff] = 0;

/* Build the command code */

		team = atoi(fam);
		ds_num = atoi(memb);
		cmd_num = atoi(r_name);
		back.xcmd_code = (team << DS_TEAM_SHIFT) + (ds_num << DS_IDENT_SHIFT) + cmd_num;

	}

/* If no command string founded */

	else
	{
		free(content.dptr);
		back.xcmd_code = 0;
		back.db_err = DbErr_ResourceNotDefined;
	}


/* Leave server */

	return(&back);
}
