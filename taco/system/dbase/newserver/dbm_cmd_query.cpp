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
    int 		i;
    bool		found;
    DBM 		*tab;
    string 		fam,
    			memb,
    			r_name,
     			cmd_str,
			req_cmd(*pcmd_name);

#ifdef DEBUG
    cout << "Command name : " << *pcmd_name << endl;
#endif
//
// Initialize error code sended cmd_queue to client 
//
    cmd_queue.db_err = 0;
    found = False;
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	cmd_queue.db_err = DbErr_DatabaseNotConnected;
	cmd_queue.xcmd_code = 0;
	return(&cmd_queue);
    }
//
// Retrieve the right "table" in the table array 
//
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
	cmd_queue.db_err = DbErr_DomainDefinition;
	cmd_queue.xcmd_code = 0;
	return(&cmd_queue);
    }
//
// Try to retrieve a resource in the CMDS table with a resource value equal
// to the command name 
//
    datum 	key;
    for (key = dbm_firstkey(tab); key.dptr != NULL; key = dbm_nextkey(tab))
    {
	datum	content = dbm_fetch(tab,key);
	if (content.dptr == NULL)
	{
	    cmd_queue.db_err = DbErr_DatabaseAccess;
	    cmd_queue.xcmd_code = 0;
	    return(&cmd_queue);
	}
	cmd_str = string(content.dptr, content.dsize);
	if (cmd_str == req_cmd)
	{
	    found = true;
	    break;
	}
    }

    if (found)
    {
//
// Get family from key 
//
	string tmp = key.dptr;
	string::size_type pos = tmp.find('|');
	if (pos == string::npos)
	{
	    cmd_queue.db_err = DbErr_DatabaseAccess;
	    cmd_queue.xcmd_code = 0;
	    return(&cmd_queue);
	}
	fam = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
//
// Get member from key 
//
	if ((pos = tmp.find('|')) == string::npos)
	{
	    cmd_queue.db_err = DbErr_DatabaseAccess;
	    cmd_queue.xcmd_code = 0;
	    return(&cmd_queue);
	}
	memb = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
//
// Get resource name from key 
//
	if ((pos = tmp.find('|')) == string::npos)
	{
	    cmd_queue.db_err = DbErr_DatabaseAccess;
	    cmd_queue.xcmd_code = 0;
	    return(&cmd_queue);
	}
	r_name = tmp.substr(0, pos);
//
// Build the command code 
//
        int 		ds_num = atoi(memb.c_str()),
     			team = atoi(fam.c_str()),
     			cmd_num = atoi(r_name.c_str());
	cmd_queue.xcmd_code = (team << DS_TEAM_SHIFT) + (ds_num << DS_IDENT_SHIFT) + cmd_num;
    }
//
// If no command string founded 
//
    else
    {
	cmd_queue.xcmd_code = 0;
	cmd_queue.db_err = DbErr_ResourceNotDefined;
    }
//
// Leave server 
//
    return(&cmd_queue);
}
