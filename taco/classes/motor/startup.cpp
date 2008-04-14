// Server for the IPC stepper motor card
// Copyright (C) 2005 Jens Kr�ger

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>

log4cpp::Category       *logStream;

#if HAVE_RPC_RPC_H
#	include <rpc/rpc.h>
#endif

#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#endif
#include <string>
#include <vector>
#include <iostream>

#include <TACOExtensions.h>
#include "TACOMotorCommon.h"
#include "TACOMotor.h"

#include <Admin.h>
#include <private/ApiP.h>

/**
 * This function will initialise the log4cpp logging service.
 * The service will be configured via the ${LOGCONFIG} environment variable.
 * If this is not found it will use the default logging mechanism.
 * The instance will be the "taco.server" + serverName
 *
 * @param serverName the name of the server
 */
static void init_logstream(const std::string serverName)
{
        const char *logpath = getenv("LOGCONFIG");
        std::string tmp = serverName;
        std::string::size_type pos = tmp.find('/');
        tmp[pos] = '.';
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
        logStream = &log4cpp::Category::getInstance("taco.server." + tmp);
        logStream->noticeStream() << "using " << logpath << " configuration file" << log4cpp::eol;
}

// list of all exported devices of the server
static std::vector< ::TACO::Server *> devices;

const unsigned int DEVICE_MAX = 0;

/**
 * The startup procedure is the first procedure called from main() when the device server starts up.
 * All toplevel devices to be created for the device server should be done in startup(). The startup
 * should make use of the database to determine which devices it should create. Initialisation of
 * devices is normally done from startup().
 *
 * This routine overwrites the startup routine provided by the TACO libraries by linking this file 
 * statically to the server. Due to the mechanism of dynamically loading unresolved symbols at runtime,
 * this routine will called if the startup routine is linked statically, otherwise the device server
 * will use the startup() provided the TACO libraries.
 *
 * @param serverName	the name of the server
 * @param e		the error code in case of error
 *
 * @return DS_NOTOK in case of error, DS_OK else
 */
long startup( char* serverName, DevLong *e)
{
        init_logstream(serverName);

        logStream->noticeStream() << "startup: starting device server: " << serverName << log4cpp::eol;

	devices.clear();
	TACO::Server::setServerName( serverName);

// Query the list of device names for the corresponding device server from the database
	std::vector<std::string> deviceList;
	try {
		deviceList = TACO::queryDeviceList( serverName);
	} catch (const ::TACO::Exception& tmp) {
		*e = tmp;
		logStream->errorStream() << "startup: error: getting device list failed: " << tmp.what() << log4cpp::eol;
		return DS_NOTOK;
	}

// Allocate memory for the devices
	if ( 0 < DEVICE_MAX && DEVICE_MAX < deviceList.size()) {
		logStream->errorStream() << "startup: error: too many devices" << log4cpp::eol;
		return DS_NOTOK;
	}

	for (unsigned int i = 0; i < deviceList.size(); ++i) {
		logStream->debugStream() << "startup: device: " << deviceList[i] << log4cpp::eol;
	}

// Create and export the devices
	unsigned int counter = 0;
	for (unsigned int i = 0; i < deviceList.size(); ++i) {
		// Determine which device should be created
		std::string type;
		try {
			type = TACO::queryResource<std::string>( deviceList [i], "type");
		} catch (const ::TACO::Exception& tmp) {
			*e = tmp;
			logStream->errorStream() << "startup: error: cannot get type for: "<< deviceList[i] << " : " << tmp.what() << log4cpp::eol;
			continue;
		}

// Create the device
		::TACO::Server *device(NULL);
		try {
			if (type == ::Motor::MOTOR_ID) {
				device = new TACO::Motor::Motor( deviceList [i], *e);
			} else {
				logStream->errorStream() << "startup: error: unsupported type: " << deviceList[i] << " : " << type << log4cpp::eol;
				continue;
			}
			logStream->infoStream() << "startup: created device: " << deviceList[i] << log4cpp::eol;
		} catch (const ::TACO::Exception& tmp) {
			*e = tmp;
			logStream->errorStream() << "startup: error: cannot create device: " << deviceList[i] << " : " << tmp.what() << log4cpp::eol;
			continue;
		}

// Export the device
		if (dev_export (const_cast<char*>(deviceList [i].c_str()), device, e) != DS_OK) {
			delete device;
			logStream->errorStream() << "startup: error: cannot export device: " << deviceList[i] << " : " << ::TACO::errorString( *e) << log4cpp::eol;
		} else {
			++counter;
			logStream->infoStream() << "startup: exported device: " << deviceList[i] << log4cpp::eol;
			devices.push_back(device);
		}
	}

	if (counter == deviceList.size()) {
		logStream->noticeStream() << "startup: success" << log4cpp::eol;
	} else if (counter != 0) {
		logStream->errorStream() << "startup: some errors occured. Not all devices exported." << log4cpp::eol;
	} else {
		logStream->fatalStream() << "startup: failed" << log4cpp::eol;
		return DS_NOTOK;
	}

	return DS_OK;
}


/**
 * Unregisters the device server from the static database and the portmapper and
 * closes open handles to database and messages services.
 */
void unregister_server (void)
{
	for (std::vector< ::TACO::Server *>::iterator it = devices.begin(); it != devices.end(); ++it)
	{
		if ((*it) == NULL)
			continue;
		logStream->noticeStream() << "delete device " << (*it)->deviceName() << log4cpp::eol;
		delete (*it);
		*it = NULL;
	}
	devices.clear();
	DevLong error = 0;
	LOCK(async_mutex);
//
// if this is a bona fida device server and it is using the database
// then unregister server from database device table
//
	if (config_flags.device_server == True)
	{
		if (!config_flags.no_database && (db_svc_unreg (config_flags.server_name, &error) != DS_OK))
			logStream->errorStream() << "db_svc_unreg failed" <<  error << log4cpp::eol;
//
// destroy open client handles to message and database servers
//		clnt_destroy (db_info.conf->clnt);
//		clnt_destroy (msg_info.conf->clnt);
//
	}
//
// unregister synchronous version (4) of server from portmapper
//
	pmap_unset (config_flags.prog_number, API_VERSION);
//
// unregister the asynchronous version (5) of the server from portmapper
//
	pmap_unset (config_flags.prog_number, ASYNCH_API_VERSION);
//
//  finally unregister version (1) used by gettransient_ut()
//
	pmap_unset (config_flags.prog_number, DEVSERVER_VERS);
//
// the server has been unregistred, so set flag to false!
// otherwise, there may be more than one attempt to unregister the server
// in multithreaded apps.
//
	config_flags.device_server = False;
	UNLOCK(async_mutex);
//
// returning here and calling exit() later from main_signal_handler() will
// permit us to call unregister_server() from a different signal handler
// and continue to do something useful afterwards
//
	return;
}
// TACODEVEL CODEGEN STARTUP END
