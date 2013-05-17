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
 * File:        $RCSfile: StarterDevice.h,v $
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
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2013-05-17 07:55:50 $
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
	 * TACO command for status
         */
	long tacoDevStatus(DevArgument argin, DevArgument argout, DevLong *error);

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

#ifdef TACO_EXT
	/**
         * TACO command for reading the device version string
         */
	long tacoDevVersion(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command for reading the device type strings
	 */
	long tacoDeviceTypes(DevArgument argin, DevArgument argout, DevLong *errror);

	/**
         * TACO command to update a device resource
	 */
	long tacoDeviceUpdateResource(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command to query a resource
	 */
	long tacoDevQueryResource(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command
	 */
	long tacoDevUpdate(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command to query infos about the device resources
	 */
	long tacoDeviceQueryResourceInfo(DevArgument argin, DevArgument argout, DevLong *error);

	/**
	 * TACO command to init the device
	 */
	long tacoDevInit(DevArgument argin, DevArgument argout, DevLong *error);
#endif

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
	pid_t getpid(const std::string &proc, const std::string &pers);

        bool running(const std::string &proc, const std::string &pers);
	
	void clearControlledList();

private:
	std::vector<ControlledServer*>	m_servers;

	pthread_mutex_t         	m_refMutex;

        pthread_t               	m_checking;

        int                     	m_runningState;

        int                     	m_updateRequest;
};


#endif
