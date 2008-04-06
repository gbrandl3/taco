// Server for the IPC stepper motor card
// Copyright (C) 2005 Jens Krüger

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

#include <Admin.h>
#include <private/ApiP.h>

#include "TACOMotorCommon.h"
#include "TACOMotor.h"

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
	dev_printdebug(DBG_TRACE | DBG_STARTUP,  "startup: starting device server: %s\n", serverName);

	devices.clear();
	TACO::Server::setServerName( serverName);

// Query the list of device names for the corresponding device server from the database
	std::vector<std::string> deviceList;
	try {
		deviceList = TACO::queryDeviceList( serverName);
	} catch (const ::TACO::Exception& tmp) {
		*e = tmp;
		dev_printerror(SEND, "startup: error: getting device list failed: %s", tmp.what());
		return DS_NOTOK;
	}

// Allocate memory for the devices
	if ( 0 < DEVICE_MAX && DEVICE_MAX < deviceList.size()) {
		dev_printerror(SEND, "startup: error: too many devices");
		return DS_NOTOK;
	}

	for (unsigned int i = 0; i < deviceList.size(); ++i) {
		dev_printdebug(DBG_TRACE | DBG_STARTUP, "startup: device: %s\n", deviceList[i].c_str());
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
			dev_printdiag(WRITE, "startup: error: cannot get type for: %s : %s\n", deviceList[i].c_str(), tmp.what());
			continue;
		}

// Create the device
		::TACO::Server *device(NULL);
		try {
			if (type == ::Motor::MOTOR_ID) {
				device = new TACO::Motor::Motor( deviceList [i], *e);
			} else {
				dev_printdiag(WRITE, "startup: error: unsupported type: %s\n", deviceList[i].c_str());
				continue;
			}
			dev_printdebug(DBG_TRACE | DBG_STARTUP, "startup: created device: %s\n", deviceList[i].c_str());
		} catch (const ::TACO::Exception& tmp) {
			*e = tmp;
			dev_printdiag(WRITE, "startup: error: cannot create device: %s : %s\n", deviceList[i].c_str(), tmp.what());
			continue;
		}

// Export the device
		if (dev_export (const_cast<char*>(deviceList [i].c_str()), device, e) != DS_OK) {
			delete device;
			dev_printdiag(WRITE, "startup: error: cannot export device: %s : %s\n", deviceList[i].c_str(), ::TACO::errorString( *e).c_str());
		} else {
			++counter;
			dev_printdebug(DBG_TRACE | DBG_STARTUP, "startup: exported device: %s\n", deviceList[i].c_str());
			devices.push_back(device);
		}
	}

	if (counter == deviceList.size()) {
		dev_printdebug(DBG_TRACE | DBG_STARTUP, "startup: success\n");
	} else if (counter != 0) {
		dev_printdiag(SEND, "startup: some errors occured\n");
		dev_printerror(SEND, "startup: some errors occured");
	} else {
		dev_printerror(SEND, "startup: failed");
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
		std::cerr << "delete device " << (*it)->deviceName() << std::endl;
		delete (*it);
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
			dev_printerror_no (SEND, "db_svc_unreg failed", error);
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
