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
 * File:        $RCSfile: StarterDevice.cpp,v $
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
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2008-10-15 15:10:09 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "StarterDevice.h"
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

#include <private/ApiP.h>

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

#include <db_setup.h>

extern log4cpp::Category       *logStream;

StarterDevice::StarterDevice(std::string name, DevLong &error)
	: Device(const_cast<char *>(name.c_str()), &error)
	, m_runningState(0)
{
        commands_map[DevState] = DeviceCommandListEntry(DevState, 
					static_cast<DeviceMemberFunction>( &Device::State),
					D_VOID_TYPE,
                                        D_SHORT_TYPE,
                                        READ_ACCESS, "DevState");
        commands_map[DevStatus] = DeviceCommandListEntry(DevStatus, 
					static_cast<DeviceMemberFunction>( &Device::Status),
					D_VOID_TYPE,
                                        D_STRING_TYPE,
                                        READ_ACCESS, "DevStatus");
        commands_map[DevRun] = DeviceCommandListEntry(DevRun, 
					static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRun),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRun");
        commands_map[DevStop] = DeviceCommandListEntry(DevStop, 
					static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevStop),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevStop");
        commands_map[DevRestore] = DeviceCommandListEntry(DevRestore, 
					static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRestart),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRestart");
	commands_map[DevStartAll] = DeviceCommandListEntry(DevStartAll, 
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoStartAll),
					D_USHORT_TYPE,
					D_VOID_TYPE,
					ADMIN_ACCESS, "DevStartAll");	
	commands_map[DevStopAll] = DeviceCommandListEntry(DevStopAll, 
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoStopAll),
					D_USHORT_TYPE,
					D_VOID_TYPE,
					ADMIN_ACCESS, "DevStopAll");
	commands_map[DevGetRunningServers] = DeviceCommandListEntry(DevGetRunningServers, 
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoGetRunning),
					D_BOOLEAN_TYPE,
					D_VAR_STRINGARR,
					READ_ACCESS, "DevGetRunningServers");
	commands_map[DevGetStoppedServers] = DeviceCommandListEntry(DevGetStoppedServers,
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoGetStopped),
					D_BOOLEAN_TYPE,
					D_VAR_STRINGARR,
					READ_ACCESS, "DevGetStoppedServers");
	commands_map[DevStart] = DeviceCommandListEntry(DevStart,
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoDevRun),
					D_VAR_STRINGARR,
					D_VOID_TYPE,
					ADMIN_ACCESS, "DevStart");
	commands_map[DevReadLog] = DeviceCommandListEntry(DevReadLog,
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoReadLog),
					D_STRING_TYPE,
					D_STRING_TYPE,
					READ_ACCESS, "DevReadLog");
	commands_map[UpdateServerInfo] = DeviceCommandListEntry(UpdateServerInfo,
					static_cast<DeviceMemberFunction>(&StarterDevice::tacoUpdateServerInfo),
					D_VOID_TYPE,
					D_VOID_TYPE,
					ADMIN_ACCESS, "UpdateServerInfo");

	try
	{
		pthread_mutex_init(&m_refMutex, NULL);		
		v_Init();
		m_runningState = 1;
		pthread_create(&this->m_checking, 0, StarterDevice::checking, this);
	}
	catch (const long &error)
	{
	}
	return;
}

StarterDevice::~StarterDevice()
{
	pthread_mutex_lock(&m_refMutex);
	m_runningState = 0;
	pthread_mutex_unlock(&m_refMutex);
	pthread_join(this->m_checking, NULL);
	pthread_mutex_destroy(&m_refMutex);
	DevLong	error;
	logStream->debugStream() << "Destructor" << log4cpp::eol;
	for (std::vector<ControlledServer*>::iterator i = m_servers.begin(); i < m_servers.end(); ++i)
		delete (*i);
	m_servers.clear();
	return;
}

void StarterDevice::v_Init(void)
{
	DevVarStringArray	server_list = {0, NULL};
	db_resource		server = {"default", D_VAR_STRINGARR, &server_list};	
	DevLong 		error;

	char 			hostna[HOST_NAME_LENGTH];
	
	taco_gethostname(hostna,sizeof(hostna));
	logStream->debugStream() << "HOSTNAME : " << hostna << log4cpp::eol;
//
// Get the list of all devices to be started on default on this machine
// 
	for (std::vector<ControlledServer*>::iterator i = m_servers.begin(); i < m_servers.end(); ++i)
		delete (*i);
	m_servers.clear();
	if (db_getresource(const_cast<char *>(this->GetDevName()) , &server, 1, &error) == DS_OK)
	{
		for (int i = 0; i < server_list.length; ++i)
		{
			std::string tmpString = server_list.sequence[i];
			std::string::size_type pos = tmpString.find("/");
			if (pos == std::string::npos)
				continue;

			ControlledServer *tmp = new ControlledServer();
			tmp->serverName = tmpString.substr(0, pos);
			tmp->persName = tmpString.substr(pos + 1);
			tmp->controlled = true;
			db_svcinfo_call	s_info;
			if (db_servinfo(tmp->serverName.c_str(), tmp->persName.c_str(), &s_info, &error) == DS_OK)
			{
				if (strcmp(s_info.host_name, "not_exp") && strcmp(s_info.host_name, hostna))
				{
					delete tmp;
					continue;
				}	
				if (s_info.server->device_nb > 0)
				{
					if (dev_import(s_info.server->device[0].dev_name, 0, &tmp->proxyDevice, &error) == DS_NOTOK)
						logStream->debugStream() << "COULD NOT IMPORT : " << s_info.server->device[0].dev_name << log4cpp::eol;
					else if (!tmp->running())
						logStream->warnStream() << "COULD NOT PING : " << s_info.server->device[0].dev_name << log4cpp::eol;
				}
				m_servers.push_back(tmp);

				for (int k = 0; k < s_info.embedded_server_nb; ++k)
					free(s_info.server[k].device);
				free(s_info.server);
			}
			free(server_list.sequence[i]);
		}
		free(server_list.sequence);
	}
	long	nb_server;
	char	**ds_servers;
	if (db_getdsserverlist(&nb_server, &ds_servers, &error) == DS_OK)
	{
		for (int i = 0; i < nb_server; ++i)
		{
			std::string serverName = ds_servers[i];
			free(ds_servers[i]);

			std::transform(serverName.begin(), serverName.end(), serverName.begin(), ::tolower);
			if (serverName == "databaseds" || serverName == "startserver" || serverName == "lt-startserver")
				continue;

			long nb_persnames;
			char **pers_names;
			if (db_getdspersnamelist(const_cast<char *>(serverName.c_str()), &nb_persnames, &pers_names, &error) == DS_OK)
			{
				for (int j = 0; j < nb_persnames; ++j)
				{
					std::string persName = pers_names[j];
					free(pers_names[j]);

					std::transform(persName.begin(), persName.end(), persName.begin(), ::tolower);

					db_svcinfo_call	s_info;
					if (db_servinfo(serverName.c_str(), persName.c_str(), &s_info, &error) == DS_OK)
					{
						if (!strcmp(s_info.host_name, hostna))
						{
							ControlledServer *tmp = new ControlledServer();
							tmp->serverName = serverName;
							tmp->persName = persName;
							tmp->controlled = false;
							if (s_info.server->device_nb > 0)
							{
								if (dev_import(s_info.server->device[0].dev_name, 0, &tmp->proxyDevice, &error) == DS_NOTOK)
									logStream->debugStream() << "COULD NOT IMPORT : " << s_info.server->device[0].dev_name << log4cpp::eol;
								else if (!tmp->running())
									logStream->infoStream() << "COULD NOT PING : " << s_info.server->device[0].dev_name << log4cpp::eol;
							}
							m_servers.push_back(tmp);
						}
					}

					for (int k = 0; k < s_info.embedded_server_nb; ++k)
						free(s_info.server[k].device);
					free(s_info.server);
				}
				free(pers_names);
			}
			else 
				logStream->errorStream() << "    " << error << log4cpp::eol;
		}
		free(ds_servers);
	}
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
	{
		ControlledServer *tmp = *it;
		std::string tmpString = tmp->serverName + "/" + tmp->persName;
		if (!tmp->controlled)
			tmpString += " not";
		tmpString += " controlled and";
		if (!tmp->running())
			tmpString += " not";
		tmpString += " running";
		logStream->debugStream() << "Server : " << tmpString << log4cpp::eol;
	}
}
	

long StarterDevice::tacoDevRun(DevArgument argin, DevArgument argout, DevLong *error) 
{
	DevVarStringArray	arr = *static_cast<DevVarStringArray *>(argin);
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StartServer")
			try 
			{
				logStream->debugStream() << "StarterDevice::tacoDevRun() : " << arr.length << log4cpp::eol;
				this->deviceRun(*arr.sequence, arr.sequence[1], arr.length > 2 ? arr.sequence[2] : "-m", "-d");
				v_Init();
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

long StarterDevice::tacoDevStop(DevArgument argin, DevArgument argout, DevLong *error) 
{
	DevVarStringArray	arr = *static_cast<DevVarStringArray *>(argin);
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StarterServer")
			try
			{
				this->deviceStop(arr.sequence[0], arr.sequence[1]);
				v_Init();
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

long StarterDevice::tacoDevRestart(DevArgument argin, DevArgument argout, DevLong *error) 
{
	DevVarStringArray	arr = *static_cast<DevVarStringArray *>(argin);
	
	if (arr.length > 1)
	{
		if (std::string(*arr.sequence) != "StarterServer")
			try
			{
				this->deviceReStart(arr.sequence[0], arr.sequence[1], arr.length > 2 ? arr.sequence[2] : "-m", "-d");
				v_Init();
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
	DevLong 		error;
	std::string		newServer = proc + '/' + pers;

	logStream->infoStream() << "Try to start server : " << newServer << log4cpp::eol;
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
			logStream->warnStream() << "Process " << proc << " already running." << log4cpp::eol;
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
				logStream->debugStream() << "Start Child : " << proc << " " << pers << " " << options << log4cpp::eol;
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

long StarterDevice::tacoStartAll(DevArgument input, DevArgument, DevLong *error)
{
	DevUShort level = *static_cast<DevUShort *>(input);
	logStream->infoStream() << "Start all at level " << level << log4cpp::eol;
	v_Init();
	*error = DS_OK;
	return DS_OK;
}

long StarterDevice::tacoStopAll(DevArgument input, DevArgument, DevLong *error)
{
	DevUShort level = *static_cast<DevUShort *>(input);
	logStream->infoStream() << "Stop all at level " << level << log4cpp::eol;
	v_Init();
	*error = DS_OK;
	return DS_OK;
}

long StarterDevice::tacoGetRunning(DevArgument input, DevArgument output, DevLong *error)
{
	static DevVarStringArray servers = {0, NULL};
	DevBoolean controlled = *static_cast<DevBoolean *>(input);
	logStream->infoStream() << "Get" << (controlled ? " all" : " controlled") << " running servers" << log4cpp::eol; 

	if (servers.sequence)
	{
		for (int i = 0; i < servers.length; ++i)
			delete [] servers.sequence[i];
		delete [] servers.sequence;
	}
	int count = 0;
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
		if ((*it)->running() && (!controlled || bool(controlled) == (*it)->controlled))
			count++;

	servers.length = count;
	servers.sequence = new DevString[count];
	int j = 0;
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
	{
		if ((*it)->running() && (!controlled || controlled == (*it)->controlled))
		{
			std::string tmp = (*it)->serverName + "/" + (*it)->persName;
			servers.sequence[j] = new char[tmp.length() + 1];
			snprintf(servers.sequence[j], tmp.length() + 1, "%s", tmp.c_str());
			++j;
		}
	}
	*static_cast<DevVarStringArray*>(output) = servers;
	*error = DS_OK;
	return DS_OK;
}

long StarterDevice::tacoGetStopped(DevArgument input, DevArgument output, DevLong *error)
{
	static DevVarStringArray servers = {0, NULL};
	DevBoolean controlled = *static_cast<DevBoolean *>(input);
	logStream->infoStream() << "Get" << (controlled ? " all" : " controlled") << " stopped servers" << log4cpp::eol; 

	if (servers.sequence)
	{
		for (int i = 0; i < servers.length; ++i)
			delete [] servers.sequence[i];
		delete [] servers.sequence;
	}
	int count = 0;
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
		if (!(*it)->running() && (!controlled || bool(controlled) == (*it)->controlled))
			count++;

	servers.length = count;
	servers.sequence = new DevString[count];
	int j = 0;
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
	{
		if (!(*it)->running() && (!controlled || controlled == (*it)->controlled))
		{
			std::string tmp = (*it)->serverName + "/" + (*it)->persName;
			servers.sequence[j] = new char[tmp.length() + 1];
			snprintf(servers.sequence[j], tmp.length() + 1, "%s", tmp.c_str());
			++j;
		}
	}
	*static_cast<DevVarStringArray*>(output) = servers;
	*error = DS_OK;
	return DS_OK;
}

long StarterDevice::tacoReadLog(DevArgument input, DevArgument output, DevLong *error)
{
	DevString value = *static_cast<DevString *>(input);
	*static_cast<DevString *>(output) = value;
	*error = DS_OK;
	return DS_OK;
}

long StarterDevice::tacoUpdateServerInfo(DevArgument, DevArgument, DevLong *error)
{
	v_Init();
	*error = DS_OK;
	return DS_OK;
}

void StarterDevice::checkServerStatus(void)
{
	logStream->debugStream() << "." << log4cpp::eol;
	for (std::vector<ControlledServer*>::iterator it = m_servers.begin(); it < m_servers.end(); ++it)
	{
		ControlledServer *tmp = *it;
		if (tmp->controlled) 
		{
			std::string tmpString = tmp->serverName;
			if (!tmp->running())
			{
				deviceRun(tmp->serverName, tmp->persName, "-m", "-d");
				tmpString += " not";
			}
			tmpString += " running";
			logStream->debugStream() << "Controlled Server : " << tmpString << log4cpp::eol;
// Try to restart the server
		}
	}
}

void *StarterDevice::checking(void *thisPointer)
{
	StarterDevice *p = static_cast<StarterDevice *>(thisPointer);
	int status;
	DevULong j = 0;
	do
	{
		j++;
		sleep(1);
		if (!(j % 30))
			p->v_Init();
		if (!(j % 10))
			p->checkServerStatus();
		pthread_mutex_lock(&(p->m_refMutex));
		status = p->m_runningState;
		pthread_mutex_unlock(&(p->m_refMutex));
	}while(status > 0);
	return NULL;
}
