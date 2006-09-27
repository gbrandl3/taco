/*****************************************************************************
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
 *
 * File:        $RCSfile: starter.cpp,v $
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for implementing a starter server
 *
 * Author(s):   Jens Krueger
 * 		$Author: jkrueger1 $
 *
 * Original:    January 2003
 *
 * Version:	$Revision: 1.14 $
 *
 * Date:	$Date: 2006-09-27 12:19:43 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "starter.h"
#include <Starter.h>
#include <iostream>
#include <vector>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */
#include <DevSec.h>
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#	error Could not find signal.h
#endif
#include <cerrno>
#include <cstdio>
#if HAVE_FCNTL_H
#	include <fcntl.h>
#endif
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif

DevLong minimal_access = WRITE_ACCESS;

StarterDevice::StarterDevice(std::string name, DevLong &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
        commands_map[DevState] = DeviceCommandMapEntry(DevState, static_cast<DeviceMemberFunction>( &Device::State),
					D_VOID_TYPE,
                                        D_SHORT_TYPE,
                                        READ_ACCESS, "DevState");
        commands_map[DevStatus] = DeviceCommandMapEntry(DevStatus, static_cast<DeviceMemberFunction>( &Device::Status),
					D_VOID_TYPE,
                                        D_STRING_TYPE,
                                        READ_ACCESS, "DevStatus");
        commands_map[DevRun] = DeviceCommandMapEntry(DevRun, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRun),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRun");
        commands_map[DevStop] = DeviceCommandMapEntry(DevStop, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevStop),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevStart");
        commands_map[DevRestore] = DeviceCommandMapEntry(DevRestore, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRestart),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRestart");

	return;
}

StarterDevice::~StarterDevice()
{
	return;
}

DevLong StarterDevice::tacoDevRun(void *argin, void *argout, DevLong *error) 
{
	DevVarStringArray	arr = *(DevVarStringArray *)argin;
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StartServer")
			try 
			{
				std::cerr << "StarterDevice::tacoDevRun() : " << arr.length << std::endl;
				this->deviceRun(*arr.sequence, arr.sequence[1], arr.length > 2 ? arr.sequence[2] : "-m", "");
				return DS_OK;
			}
			catch (const DevLong &e)
			{
				*error = e;
			}
		else
			*error = DevLong(DevErr_DeviceIllegalParameter);
	}
	else
		*error = DevLong(DbErr_BadParameters);
	return DS_NOTOK;
}

DevLong StarterDevice::tacoDevStop(void *argin, void *argout, DevLong *error) 
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
			catch(const DevLong &e)
			{
				*error = e;
			}
		else
			*error = DevLong(DevErr_DeviceIllegalParameter);
	}
	else
		*error = DevLong(DbErr_BadParameters);
	
	return DS_NOTOK;
}

DevLong StarterDevice::tacoDevRestart(void *argin, void *argout, DevLong *error) 
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
			catch (const DevLong &e)
			{
				*error = e;
			}		
		else 
			*error = DevLong(DevErr_DeviceIllegalParameter);
	}
	else
		*error = DevLong(DbErr_BadParameters);
	return DS_NOTOK;
}

void StarterDevice::deviceRun(const std::string proc, const std::string pers, const std::string options, const std::string param)	
{
	DevVarStringArray	server_list = {0, NULL};
	db_resource		server = {"default", D_VAR_STRINGARR, &server_list};	
	DevLong 			error;
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
		{
			std::cerr << "Process " << proc << " already running." << std::endl;
			return;
		}
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
				std::cerr << "Start Child : " << proc << " " << pers << " " << options << std::endl;
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
				throw DevLong(DevErr_NoProcessWithPid);
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
		throw DevLong(DevErr_DeviceIllegalParameter);
	if (kill(pid, SIGTERM))
		switch (errno)
		{
			case ESRCH :
				throw DevLong(DevErr_NoProcessWithPid);
			default:
				throw DevLong(DevErr_CantKillProcess);
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
	catch (const DevLong &e)
	{
	}
	return;
}	

pid_t StarterDevice::getpid(const std::string proc, const std::string pers)
{	
	db_svcinfo_call	server_info;
	DevLong		error;

	if (db_servinfo(const_cast<char *>(proc.c_str()), const_cast<char *>(pers.c_str()), &server_info, &error) == DS_OK)
		return server_info.pid;
	throw error;
}
