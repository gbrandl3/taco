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
 * File:        $RCSfile: starter.h,v $
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
 * Version:	$Revision: 1.10 $
 *
 * Date:	$Date: 2008-04-06 09:07:54 $
 *
 *****************************************************************************/

#ifndef __STARTER_DEVICE_H__
#define __STARTER_DEVICE_H__

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <API.h>
#include <Device.h>
#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#include <string>
#include <vector>
#if HAVE_PTHREAD_H
#	include <pthread.h>
#endif

#define DevStartAll		3000L
#define DevStopAll		3001L
#define DevStart		3002L
#define DevGetRunningServers	3003L
#define DevGetStoppedServers	3004L
#define DevReadLog		3005L
#define UpdateServerInfo	3006L

struct ControlledServer
{
	std::string	serverName;
	std::string	persName;
	bool		controlled;
	DevShort	startupLevel;
	devserver	proxyDevice;


	ControlledServer()
		: serverName("")
		, persName("")
		, controlled(false)
		, startupLevel(0)
		, proxyDevice(NULL)
	{
	}
	~ControlledServer()
	{
		if (proxyDevice && dev_free(proxyDevice, &error) == DS_OK)
			proxyDevice = NULL;
	}
	bool running()
	{
		return (proxyDevice && dev_ping(proxyDevice, &error) == DS_OK);
	}

private:
	DevLong		error;
};


class StarterDevice : public Device
{
//	friend DevLong StarterSetup(char *pszServerName, DevLong *plError);
public:
	StarterDevice(std::string name, DevLong &error);
	~StarterDevice();

	static void *checking(void *);

protected:
//	DevLong GetResources(char *name, DevLong *error){}

private:
	/**
	 * TACO command for starting a Device server.
	 */
	long tacoDevRun(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for stopping a Device server.
	 */
	long tacoDevStop(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for restarting a Device server.
	 */
	long tacoDevRestart(DevArgument argin, DevArgument argout, DevLong *error); 

	/**
	 * TACO command for starting all device servers with a certain start level
         */
	long tacoStartAll(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for stopping all device servers with a certain start level
	 */
	long tacoStopAll(DevArgument argin, DevArgument argout, DevLong *error);
	
	/**
	 * TACO command for getting all running servers
         */
	long tacoGetRunning(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for getting all stopped servers.
	 */
	long tacoGetStopped(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for reading the log for a server
	 */
	long tacoReadLog(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for refreshing the database informations
	 */
	long tacoUpdateServerInfo(DevArgument argin, DevArgument argout, DevLong *error);

public:
	/**
	 * Implementation of TACO command DevRun
	 */
	void deviceRun(const std::string proc, const std::string pers, const std::string option, const std::string param);	
	
	/**
	 * Implementation of TACO command DevStop.
	 */
	void deviceStop(const std::string proc, const std::string pers);

	/**
	 * Implementation of TACO command DevRestart.
	 */
	void deviceReStart(const std::string proc, const std::string pers, const std::string option, const std::string param);	


protected:
	void v_Init(void);

	void checkServerStatus(void);
	
private:
	pid_t getpid(const std::string proc, const std::string pers);
	
	std::vector<ControlledServer*>	m_servers;

	pthread_mutex_t         m_refMutex;

        pthread_t               m_checking;

        int                     m_runningState;

        int                     m_updateRequest;
};


#endif
