/*****************************************************************************
 * Copyright (C) 2003-2013 Jens Krueger
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
 * File:        $RCSfile: startup.cpp,v $
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
 * Version:	$Revision: 1.14 $
 *
 * Revision:	$Date: 2013-05-17 07:55:50 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include "API.h"
#include "private/ApiP.h"
#include "Device.h"
#include "DevServer.h"
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
#include "StarterDevice.h"

#include <iostream>
#include <string>
#include <algorithm>
#ifdef _solaris
#	include "_count.h"
#endif /* _solaris */

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

log4cpp::Category       *logStream;

static std::string	devServer;

static void CleanServer(void);
static long ServerSetup(char *, DevLong *);

/**
 * This function is used as signal handler. This callback function for the sigaction function gets
 * the signal number send to the process from the operating system and performs some actions depending on 
 * the signal number.
 * @parma signal signal number
 */
static void SignalHandler (int signal)
{
	DevLong lError;

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
			logStream->warnStream() << "Got unexpected signal: " << signal << log4cpp::eol;
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
	DevLong lError;

	if (db_svc_unreg(const_cast<char *>(devServer.c_str()), &lError) == DS_OK)
	        logStream->noticeStream() << "cleanup: success" << log4cpp::eol;
	return;
}

/**
 * This function is called from the TACO server the create and export the TACO devices. It also installs 
 * the signal handler.
 * @param pszServerName name of the server containing "process_name/personal name"
 * @param plError pointer to the error variable which has to be set in case of an error inside the function
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long startup(char *pszServerName, DevLong *plError)
{
	const char              *logpath = getenv("LOGCONFIG");

        try
        {
                if (!logpath)
                        throw 0;
                log4cpp::PropertyConfigurator::configure(logpath);
        }
	catch (const log4cpp::ConfigureFailure &e)
	{
		std::cerr << e.what() << std::endl;
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
	}
        catch (...)
        {
                logpath = "no";
                log4cpp::BasicConfigurator::configure();
        }
        logStream = &log4cpp::Category::getInstance("taco.system.StartServer");
        logStream->noticeStream() << "using " << logpath << " configuration file" << log4cpp::eol;

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
		logStream->emergStream() <<" could not install signal handler" << log4cpp::eol;
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
long ServerSetup(char *pszServerName, DevLong *plError)
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

	DevLong	lError;
	StarterDevice	*dev = new StarterDevice(devName, lError);
	if ((dev == NULL))
	{
		logStream->emergStream() << "Error when trying to create " << devName << " device" << log4cpp::eol;
		*plError = lError;
		return DS_NOTOK; 
	}
      	if (dev_export(const_cast<char *>(dev->GetDevName()), dev, &lError) != DS_OK)
	{
		logStream->emergStream() << "Starter Device  = " << dev->GetDevName() << " not exported." << log4cpp::eol;
		*plError = lError;
		return DS_NOTOK;
	}
    	logStream->debugStream() << "Device server <" << pszServerName << "> started (pid = " << getpid() << ")." << log4cpp::eol;
        logStream->noticeStream() << "startup: success" << log4cpp::eol;
    	return DS_OK;
}
