#include <MySqlServer.h>


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
cmd_que *MySQLServer::db_cmd_query_1_svc(nam *pcmd_name)
{
    int 		i;
    std::string 	req_cmd(*pcmd_name);

#ifdef DEBUG
    std::cout << "Command name : " << req_cmd << std::endl;
#endif
//
// Initialize error code sended cmd_queue to client 
//
    cmd_queue.db_err = 0;
//
// Return error code if the server is not connected to the database files 
//
    if (!dbgen.connected)
    {
	std::cerr << "I'm not connected to database." << std::endl;
	cmd_queue.db_err = DbErr_DatabaseNotConnected;
	cmd_queue.xcmd_code = 0;
	return(&cmd_queue);
    }
//
// Retrieve the right "table" in the table array 
//
    for (i = 0;i < dbgen.TblNum;i++)
	if (dbgen.TblName[i] == "cmds")		// DOMAIN = 'cmds'
	    break;
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
    std::string query;

    if (mysql_db == "tango")
    {
    	query = "SELECT family, member, name FROM property_device WHERE ";
	query += ("domain = 'cmds' AND value = '" + req_cmd + "'");
    }
    else
    {
    	query = "SELECT FAMILY, MEMBER, NAME FROM RESOURCE WHERE ";
    	query += ("DOMAIN = 'cmds' AND RESVAL = '" + req_cmd + "'");
    }
    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
	std::cerr << mysql_error(mysql_conn) << std::endl;
	cmd_queue.db_err = DbErr_DatabaseAccess;
	cmd_queue.xcmd_code = 0;
	return (&cmd_queue);
    }
    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)) != NULL)
	cmd_queue.xcmd_code = (atoi(row[0]) << DS_TEAM_SHIFT) + (atoi(row[1]) << DS_IDENT_SHIFT) + atoi(row[2]);
//
// If no command string founded 
//
    else
    {
	cmd_queue.xcmd_code = 0;
	cmd_queue.db_err = DbErr_ResourceNotDefined;
    }
    mysql_free_result(result);
//
// Leave server 
//
    return(&cmd_queue);
}
