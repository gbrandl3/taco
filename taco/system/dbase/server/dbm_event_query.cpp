#include <API.h>

#include <fcntl.h>
#include <db_xdr.h>

/* Some C++ include files */

#include <string>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <iostream>



/**
 * To retrieve (from database) the command code associated to a command name (the string)
 * 
 * @param pevent_name A pointer to string which is the event name
 * 
 * @return This function returns a pointer to a structure of the event_que type
 *    The definition of this structure is :
 *    struct {
 *      int db_err;    The database error code 
 *                        0 if no error
 *      unsigned int xevent_code;	The command code.
 *				0 if the database query fails
 *            }
 */
event_que *NdbmServer::db_event_query_1_svc(nam *pevent_name)
{
	static event_que 	back;
	int 			i;
	bool			found(false);
	GDBM_FILE		tab;
	std::string 		fam,
				memb,
				r_name,
				event_str,
				req_event(*pevent_name);
#ifdef DEBUG
	std::cout << "Command name : " << req_event << std::endl;
#endif
//
// Initialize error code sended back to client */
//
	back.db_err = 0;
//
// Return error code if the server is not connected to the database files */
//
	if (dbgen.connected == False)
	{
		back.db_err = DbErr_DatabaseNotConnected;
		back.xevent_code = 0;
		return(&back);
	}
//
// Retrieve the right "table" in the table array */
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == "events")
		{
			tab = dbgen.tid[i];
			break;
		}

	if (i == dbgen.TblNum)
	{
		back.db_err = DbErr_DomainDefinition;
		back.xevent_code = 0;
		return(&back);
	}

/* Try to retrieve a resource in the EVENTS table with a resource value equal
   to the command name */

	datum 		key, 
			key2;
	for (key=gdbm_firstkey(tab);
		key.dptr!=NULL;
		key2 = key, key=gdbm_nextkey(tab, key), free(key2.dptr))
	{
		datum	content = gdbm_fetch(tab,key);
		if (content.dptr == NULL)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xevent_code = 0;
			return(&back);
		}

                event_str = std::string(content.dptr,content.dsize);
                free(content.dptr);
                if (event_str == req_event)
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
		std::string tmp = key.dptr;
		free(key.dptr);
		std::string::size_type	pos = tmp.find('|');
		if (pos == std::string::npos)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xevent_code = 0;
			return(&back);
		}
		fam = tmp.substr(0, pos);
		tmp.erase(0, pos + 1);
//
// Get member from key
//
		if ((pos = tmp.find('|')) == std::string::npos)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xevent_code = 0;
			return(&back);
		}
		memb = tmp.substr(0, pos);
		tmp.erase(0, pos + 1);
//
// Get resource name from key
//
		if ((pos = tmp.find('|')) == std::string::npos)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xevent_code = 0;
			return(&event_queue);
		}
		r_name = tmp.substr(0, pos);
//
// Build the event code
//
		int     team = atoi(fam.c_str()),
			ds_num = atoi(memb.c_str()),
			event_num = atoi(r_name.c_str());
		back.xevent_code = (team << DS_TEAM_SHIFT) + (ds_num << DS_IDENT_SHIFT) + event_num;
	}
//
// If no command event found
//
	else
	{
		free(key.dptr);
		back.xevent_code = 0;
		back.db_err = DbErr_ResourceNotDefined;
	}
//
// Leave server
//
	return(&back);
}
