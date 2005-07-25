/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2005-07-25 08:28:17 $
 *
 */

#include <API.h>

#include <string.h>

#include <fcntl.h>
#include <db_xdr.h>

/* Some C++ include files */

#include <string>
#include <NdbmClass.h>
#include <iostream>
#include <NdbmServer.h>


/**
 * To retrieve (from database) the command code associated to a command name (the string)
 * 
 * @arg pcmd_name A pointer to string which is the command name
 * 
 * @return  This function returns a pointer to a structure of the cmd_que type
 *    struct {
 *      int db_err;    The database error code
 *                        0 if no error
 *      unsigned int xcmd_code;	The command code.
 *				0 if the database query fails
 *            }
 */
cmd_que *NdbmServer::db_cmd_query_1_svc(nam *pcmd_name)
{
	static cmd_que 	back;
	int 		i;
	bool		found = false;
	GDBM_FILE 	tab;
	unsigned int 	diff;
	int 		ds_num,
			team,
			cmd_num;
	std::string 	fam,
			memb,
			r_name,
			cmd_str,
			req_cmd(*pcmd_name);

#ifdef DEBUG
	std::cout << "Command name : " << req_cmd << std::endl;
#endif
//
// Initialize error code sended back to client
//
	back.db_err = 0;
//
// Return error code if the server is not connected to the database files
//
	if (dbgen.connected == False)
	{
		back.db_err = DbErr_DatabaseNotConnected;
		back.xcmd_code = 0;
		return(&back);
	}
//
// Retrieve the right "table" in the table array
//
	for (i = 0;i < dbgen.TblNum;i++)
		if (dbgen.TblName[i] == "cmds")
		{
			tab = dbgen.tid[i];
			break;
		}

	if (i == dbgen.TblNum)
	{
		back.db_err = DbErr_DomainDefinition;
		back.xcmd_code = 0;
		return(&back);
	}
//
// Try to retrieve a resource in the CMDS table with a resource value equal to the command name
//
	datum 		key, 
			key2;
	for (key=gdbm_firstkey(tab);
		key.dptr!=NULL;
		key2 = key, key=gdbm_nextkey(tab, key), free(key2.dptr))
	{
		datum content = gdbm_fetch(tab,key);
		if (content.dptr == NULL)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
			return(&back);
		}
		cmd_str = std::string(content.dptr, content.dsize);
                free(content.dptr);
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
		std::string tmp = key.dptr;
		free(key.dptr);
		std::string::size_type pos = tmp.find('|');
		if (pos == std::string::npos)
		{
			back.db_err = DbErr_DatabaseAccess;
			back.xcmd_code = 0;
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
			back.xcmd_code = 0;
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
			back.xcmd_code = 0;
			return(&back);
		}
		r_name = tmp.substr(0, pos);
//
// Build the command code
//
		int     ds_num = atoi(memb.c_str()),
			team = atoi(fam.c_str()),
			cmd_num = atoi(r_name.c_str());
		back.xcmd_code = (team << DS_TEAM_SHIFT) + (ds_num << DS_IDENT_SHIFT) + cmd_num;
	}
//
// If no command string found
//
	else
	{
		free(key.dptr);
		back.xcmd_code = 0;
		back.db_err = DbErr_ResourceNotDefined;
	}
//
// Leave server 
//
	return(&back);
}
