#include <NdbmServer.h>


/****************************************************************************
*                                                                           *
*		Server code for db_event_query function                     *
*                               ------------                                *
*                                                                           *
*    Function rule : To retrieve (from database) the command code           *
*		     associated to a command name (the string)		    *
*                                                                           *
*    Argin : A pointer to string which is the command name		    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "event_que" type *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      int db_err;    The database error code                               *
*                        0 if no error                                      *
*      unsigned int xevent_code;	The command code.		    *
*				0 if the database query fails		    *
*            }                                                              *
*                                                                           *
****************************************************************************/
event_que *NdbmServer::db_event_query_1_svc(nam *pevent_name)
{
    int 		i,
			found;
    datum 		key;
    char 		*tbeg,
			*tend;
    DBM 		*tab;
    unsigned int 	diff;
    string		fam,
			memb,
			r_name,
			event_str;

#ifdef DEBUG
    cout << "Command name : " << *pevent_name << endl;
#endif
//
// Initialize error code sended event_queue to client 
//
    event_queue.db_err = 0;
    found = False;
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	event_queue.db_err = DbErr_DatabaseNotConnected;
	event_queue.xevent_code = 0;
	return(&event_queue);
    }
//
// Retrieve the right "table" in the table array 
//
    for (i = 0;i < dbgen.TblNum;i++)
    {
	if (dbgen.TblName[i] == "events")
	{
	    tab = dbgen.tid[i];
	    break;
	}
    }

    if (i == dbgen.TblNum)
    {
	event_queue.db_err = DbErr_DomainDefinition;
	event_queue.xevent_code = 0;
	return(&event_queue);
    }
//
// Try to retrieve a resource in the EVENTS table with a resource value equal
// to the command name 
//
    for (key=dbm_firstkey(tab);key.dptr!=NULL;key=dbm_nextkey(tab))
    {
	datum content = dbm_fetch(tab,key);
	if (content.dptr == NULL)
	{
	    event_queue.db_err = DbErr_DatabaseAccess;
	    event_queue.xevent_code = 0;
	    return(&event_queue);
	}
	event_str = string(content.dptr,content.dsize);
	if (event_str == *pevent_name)
	{
	    found = true;
	    break;
	}
    }

    if (!found)
    {
//
// Get family from key 
//
	string tmp = key.dptr;
	string::size_type	pos = tmp.find('|');
	if (pos == string::npos)
	{
	    event_queue.db_err = DbErr_DatabaseAccess;
	    event_queue.xevent_code = 0;
	    return(&event_queue);
	}
	fam = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
//
// Get member from key 
//
	if ((pos = tmp.find('|')) == string::npos)
	{
	    event_queue.db_err = DbErr_DatabaseAccess;
	    event_queue.xevent_code = 0;
	    return(&event_queue);
	}
	memb = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
//
// Get resource name from key 
//
	if ((pos = tmp.find('|')) == string::npos)
	{
	    event_queue.db_err = DbErr_DatabaseAccess;
	    event_queue.xevent_code = 0;
	    return(&event_queue);
	}
	r_name = tmp.substr(0, pos);
//
// Build the command code 
//
	int 	team = atoi(fam.c_str()),
		ds_num = atoi(memb.c_str()),
		event_num = atoi(r_name.c_str());
	event_queue.xevent_code = (team << DS_TEAM_SHIFT) + (ds_num << DS_IDENT_SHIFT) + event_num;
    }
//
// If no command string founded 
//
    else
    {
	event_queue.xevent_code = 0;
	event_queue.db_err = DbErr_ResourceNotDefined;
    }
//
// Leave server 
//
    return(&event_queue);
}
