/*****************************************************************************
 * File:        $Id: starter.cpp,v 1.4 2003-12-08 15:17:42 jkrueger1 Exp $
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for implementing a starter server
 *
 * Author(s):   Jens Krüger
 * 		$Author: jkrueger1 $
 *
 * Original:    January 2003
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2003-12-08 15:17:42 $
 *
 * Copyright (C) 2003 Jens Krueger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *****************************************************************************/

#include "starter.h"
#include <Starter.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <DevSec.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

StarterDevice::StarterDevice(std::string name, long &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
        commands_list[DevRun] = DeviceCommandListEntry(DevRun, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRun),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRun");
        commands_list[DevStop] = DeviceCommandListEntry(DevStop, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevStop),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevStart");
        commands_list[DevRestore] = DeviceCommandListEntry(DevRestore, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRestart),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRestart");

	return;
}

StarterDevice::~StarterDevice()
{
	return;
}

long StarterDevice::tacoDevRun(void *argin, void *argout, long *error) 
{
	DevVarStringArray	arr = *(DevVarStringArray *)argin;
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StartServer")
			try 
			{
				this->deviceRun(*arr.sequence, arr.sequence[1], arr.length > 2 ? arr.sequence[2] : "-m", "");
				return DS_OK;
			}
			catch (const long &e)
			{
				*error = e;
			}
		else
			*error = long(DevErr_DeviceIllegalParameter);
	}
	else
		*error = long(DbErr_BadParameters);
	return DS_NOTOK;
}

long StarterDevice::tacoDevStop(void *argin, void *argout, long *error) 
{
	DevVarStringArray	arr = *(DevVarStringArray *)argin;
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StarterServer")
			try
			{
				this->deviceStop(arr.sequence[0], arr.sequence[1]);
				return DS_OK;
			}
			catch(const long &e)
			{
				*error = e;
			}
		else
			*error = long(DevErr_DeviceIllegalParameter);
	}
	else
		*error = long(DbErr_BadParameters);
	
	return DS_NOTOK;
}

long StarterDevice::tacoDevRestart(void *argin, void *argout, long *error) 
{
	DevVarStringArray	arr = *(DevVarStringArray *)argin;
	
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StarterServer")
			try
			{
				this->deviceReStart(arr.sequence[0], arr.sequence[1], arr.length > 2 ? arr.sequence[2] : "-m", "");
				return DS_OK;
			}
			catch (const long &e)
			{
				*error = e;
			}		
		else 
			*error = long(DevErr_DeviceIllegalParameter);
	}
	else
		*error = long(DbErr_BadParameters);
	return DS_NOTOK;
}

void StarterDevice::deviceRun(const std::string proc, const std::string pers, const std::string options, const std::string param)	
{
	DevVarStringArray	server_list = {0, NULL};
	db_resource		server = {"default", D_VAR_STRINGARR, &server_list};	
	long 			error;
	std::string		newServer = proc + '/' + pers;
	
//
// Get the list of all devices to be started on this machine
// 
	if (db_getresource(const_cast<char *>(this->GetDevName()) , &server, 1, &error) == DS_OK)
	{
		std::vector<std::string>	names_of_server;
		names_of_server.clear();
//
// Save all full servernames and free the got list
//
		for (int i = 0; i < server_list.length; ++i)
		{
			names_of_server.push_back(server_list.sequence[i]);
			free(server_list.sequence[i]);
		}
		free(server_list.sequence);	
//
// Exist the new server in the list?
// If not add it, else look for a running process with the got pid
//
		if (std::find(names_of_server.begin(), names_of_server.end(), newServer) == names_of_server.end())
			names_of_server.push_back(newServer);
		else if (this->getpid(proc, pers) != 0)
			return;
//
// Store the new list in the database 
//	
		server_list.length = names_of_server.size();
		server_list.sequence = new char *[names_of_server.size()];
		for (int i = 0; i < names_of_server.size(); ++i)
		{
			server_list.sequence[i] = new char[names_of_server[i].length() + 1];
			strcpy(server_list.sequence[i], names_of_server[i].c_str());
		}
		if (db_putresource(const_cast<char *>(this->GetDevName()) , &server, 1, &error) == DS_OK)
		{
			pid_t	pid = fork();
			if (pid == 0)
			{
#ifdef EBUG
				std::cerr << "Start Child" << endl;
#endif
				int 	fd = open("/dev/null", O_APPEND);
				dup2(fd, fileno(stdout));
				execlp(proc.c_str(), proc.c_str(), pers.c_str(), options.c_str(), NULL);
				close(fd);
				perror("Execute failed");
				delete server_list.sequence[--server_list.length];
				db_putresource(const_cast<char *>(this->GetDevName()) , &server, 1, &error);
				exit(-1);
			}
			else if (pid < 0)
				throw long(DevErr_NoProcessWithPid);
			return;
		}
		for (int i = 0; i < server_list.length; ++i)
			delete server_list.sequence[i];
		delete server_list.sequence;
	}
	throw error;
}
	
void StarterDevice::deviceStop(const std::string proc, const std::string pers)
{
	pid_t pid = this->getpid(proc, pers);

	if (pid == 0)
		return;
	if (pid == ::getpid())
		throw long(DevErr_DeviceIllegalParameter);
	if (kill(pid, SIGTERM))
		switch (errno)
		{
			case ESRCH :
				throw long(DevErr_NoProcessWithPid);
			default:
				throw long(DevErr_CantKillProcess);
		}
	int status;
	waitpid(pid, &status, 0); 
	return;
}

void StarterDevice::deviceReStart(const std::string proc, const std::string pers, const std::string option, const std::string param)
{
	try
	{
		this->deviceStop(proc, pers);
		this->deviceRun(proc, pers, option, param);
	}
	catch (const long &e)
	{
	}
	return;
}	

pid_t StarterDevice::getpid(const std::string proc, const std::string pers)
{	
	db_svcinfo_call	server_info;
	long		error;

	if (db_servinfo(const_cast<char *>(proc.c_str()), const_cast<char *>(pers.c_str()), &server_info, &error) == DS_OK)
		return server_info.pid;
	throw error;
}
