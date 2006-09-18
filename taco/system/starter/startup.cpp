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
 * File:        $Id: startup.cpp,v 1.9 2006-09-18 21:45:36 jkrueger1 Exp $
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for implementing a starter server
 *
 * Author(s):   Jens Krüger
 * 		$Author: jkrueger1 $
 *
 * Original:	January 2003
 *
 * Version:	$Revision: 1.9 $
 *
 * Revision:	$Date: 2006-09-18 21:45:36 $
 *
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <private/ApiP.h>
#include <Device.h>
#include <DevServer.h>
#if HAVE_DLFCN_H
#	include <dlfcn.h>
#endif
#if HAVE_SIGNAL_H
#	include <signal.h>
#endif
#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif
#include <errno.h>
#include "starter.h"

#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */


static std::string	devServer;

static void CleanServer(void);
static long ServerSetup(char *, long *);

/**
 * This function is used as signal handler. This callback function for the sigaction function gets
 * the signal number send to the process from the operating system and performs some actions depending on 
 * the signal number.
 * @parma signal signal number
 */
static void SignalHandler (int signal)
{
	long lError;

	switch (signal)
	{
		case SIGQUIT:
		case SIGTERM:
		case SIGINT:
			CleanServer();
			exit(0);
			break;
		case SIGHUP:
			CleanServer();
			if (ServerSetup(const_cast<char *>(devServer.c_str()), &lError) != DS_OK)
				exit(1);
			break;
		case SIGALRM:
			break;
		case SIGCHLD:
			{
				int serrno(errno);
				while (1)
				{
					int status;
					int pid = waitpid (WAIT_ANY, &status, WNOHANG);
					if (pid < 0)
					{
						perror ("waitpid");
						break;
					}
					if (pid == 0)
					break;
//					notice_termination (pid, status);
				}
				errno = serrno;
			}
			break;
		default:
			std::cerr << "Got unexpected signal: " << signal << std::endl;
			break;
	}
	return;
}

/**
 * This function cleans up the server. It unregisters all devices exported by this server
 * and removes all devices from the memory.
 */
static void CleanServer(void)
{
	long 	lError;

	db_svc_unreg(const_cast<char *>(devServer.c_str()), &lError);
	return;
}

/**
 * This function is called from the TACO server the create and export the TACO devices. It also installs 
 * the signal handler.
 * @param pszServerName name of the server containing "process_name/personal name"
 * @param plError pointer to the error variable which has to be set in case of an error inside the function
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long startup(char *pszServerName, long *plError)
{
	struct sigaction sighand;
	sighand.sa_handler = SignalHandler;
	sighand.sa_flags = 0;
	if (sigaction (SIGHUP, &sighand, NULL) != 0
		|| sigaction (SIGINT, &sighand, NULL) != 0
		|| sigaction (SIGQUIT, &sighand, NULL) != 0
		|| sigaction (SIGTERM, &sighand, NULL) != 0
		|| sigaction (SIGALRM, &sighand, NULL) != 0
		|| sigaction (SIGCHLD, &sighand, NULL) != 0)
	{
		std::cerr <<" could not install signal handler" << std::endl;
		return DS_NOTOK;
	}
	return ServerSetup(pszServerName, plError);
}

/**
 * This methods sets up the devices. At first it looks for the desired device libraries by
 * calling loadLibraries and looks for the devices it has to export. Then it tries to create
 * and export all this devices. The information what is the type of the device is stored in the
 * database under key "device name/class" for each device.
 *
 * @param sClassType the device class 
 * @param sDeviceName the device name
 * @return in case of succes the pointer to the created device object otherwise NULL
 */
long ServerSetup(char *pszServerName, long *plError)
{
	devServer = std::string(pszServerName);

        char    hostname[HOST_NAME_LENGTH];

        if (taco_gethostname(hostname, HOST_NAME_LENGTH))
		return DS_NOTOK;
	std::string		devName("sys/start/");
	std::string::size_type	pos = std::string(hostname).find('.');
	if (pos != std::string::npos)
		devName += std::string(hostname).substr(0, pos);
	else
		devName += std::string(hostname);

	long    	lError;
	StarterDevice	*dev = new StarterDevice(devName, lError);
	if ((dev == NULL))
	{
		std::cerr << "Error when trying to create " << devName << " device" << std::endl;
		*plError = lError;
		return DS_NOTOK; 
	}
      	if (dev_export(const_cast<char *>(dev->GetDevName()), dev, &lError) != DS_OK)
	{
		std::cerr << "Starter Device  = " << dev->GetDevName() << " not exported." << std::endl;
		*plError = lError;
		return DS_NOTOK;
	}
#ifdef EBUG
    	std::cout << "Device server <" << pszServerName << "> started (pid = " << getpid() << ")." << std::endl;
#endif

	DevVarStringArray       server_list = {0, NULL};
	db_resource		server = {"default", D_VAR_STRINGARR, &server_list};
	if (db_getresource(const_cast<char *>(dev->GetDevName()), &server, 1, plError) != DS_OK)
	{
		dev_printerror_no(SEND, "Could not get the \"default\" resource ", *plError); 
		return DS_OK;
	}
	for (int i = 0; i < server_list.length; i++)
	{
		std::string server_name = server_list.sequence[i];
#ifdef EBUG
		std::cout << "start TACO server " << server_name << std::endl;
#endif
#ifndef _solaris
		if (std::count(server_name.begin(), server_name.end(), '/') != 1)
#else
		if (_sol::count(server_name.begin(), server_name.end(), '/') != 1)
#endif /* _solaris */
		{
			std::cerr << " Server name \"" << server_name << "\"not correctly defined" << std::endl;
			continue;
		}
		std::string::size_type pos = server_name.find('/');
#ifdef EBUG
		std::cerr << server_name.substr(0, pos) << " " << server_name.substr(pos + 1) << std::endl;
#endif
		try
		{
			dev->deviceRun(server_name.substr(0, pos), server_name.substr(pos + 1), "-m", "");
#ifdef EBUG
			std::cerr << " started" << std::endl;
#endif
		}
		catch (const long &e)
		{
		}
		sleep(1);
	}
    	return DS_OK;
}
