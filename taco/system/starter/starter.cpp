/*****************************************************************************
 *
 * File:        $Id: starter.cpp,v 1.2 2003-05-07 14:53:11 jkrueger1 Exp $
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
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-07 14:53:11 $
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
#include <DevSec.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

StarterDevice::StarterDevice(string name, long &error)
	: Device(const_cast<char *>(name.c_str()), &error)
{
        commands_list[DevRun] = DeviceCommandListEntry(DevRun, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRun),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRun");
        commands_list[DevStop] = DeviceCommandListEntry(DevStop, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevStop),
					D_LONG_TYPE,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevStart");
        commands_list[DevRestore] = DeviceCommandListEntry(DevRestore, static_cast<DeviceMemberFunction>( &StarterDevice::tacoDevRestart),
					D_VAR_STRINGARR,
                                        D_VOID_TYPE,
                                        ADMIN_ACCESS, "DevRestore");

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
		if (string(*arr.sequence) != "StartServer")
			try 
			{
				this->deviceRun(*arr.sequence, arr.sequence[1], "-m", "");
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
	try
	{
		this->deviceStop((pid_t)*(DevLong*)argin);
		return DS_OK;
	}
	catch(const long &e)
	{
		*error = e;
	}
	return DS_NOTOK;
}

long StarterDevice::tacoDevRestart(void *argin, void *argout, long *error) 
{
	DevVarStringArray	arr = *(DevVarStringArray *)argin;
	
	if (arr.length > 2)
	{
		if (string(*arr.sequence) != "StarterServer")
			try
			{
				pid_t pid = (pid_t)atol(*arr.sequence);
				this->deviceReStart(pid, arr.sequence[1], arr.sequence[2], "-m", "");
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
}

void StarterDevice::deviceRun(string proc, string pers, string options, string param)	
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
		exit(0);
//		throw long(DevErr_CantFindExecutable);
	}
	else if (pid < 0)
		throw long(DevErr_NoProcessWithPid);
	return;
}
	
void StarterDevice::deviceStop(pid_t pid)
{
	if (pid == getpid())
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

void StarterDevice::deviceReStart(pid_t pid, string proc, string pers, string option, string param)
{
	try
	{
		if (pid <= 1)
			throw long(0);
		this->deviceStop(pid);
	}
	catch (const long &e)
	{
	}
	this->deviceRun(proc, pers, option, param);
	return;
}	
	
