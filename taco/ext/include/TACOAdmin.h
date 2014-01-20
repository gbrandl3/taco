/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2014 Sebastian Huber <sebastian-huber@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef TACO_ADMIN_H
#define TACO_ADMIN_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <API.h>
#include <DevServer.h>

#include <Device.h>

#include <TACOServer.h>

namespace TACO {
	class Admin;
}

//! Base class for all server devices
class TACO::Admin : public TACO::Server {
public:
	//! Creates a server device
	Admin( const std::string& name, DevLong& error) throw (Exception);

	//! Destroys the server device
	virtual ~Admin() throw ()
	{
		// VOID
	}

protected:
	virtual void deviceRestart(const std::string &) throw (Exception);

	virtual void serverRestart(void) throw (Exception);

	virtual void kill(void) throw (Exception);

	virtual const std::vector<std::string>& classQuery(void) throw (Exception);

	virtual const std::vector<std::string>& deviceQuery(void) throw (Exception);
	
	/**
	 * %State machine.
	 *
	 * The state machine is invoked by the Command() method.  It assigns the current device state to
	 * <em>state</em> and the number of the current command to <em>command</em>.
	 * The state machine must return true, if the command execution is allowed in the current device state.
	 * If false is returned, the Command() method denies the command execution and throws the EXECUTION_DENIED exception.
	 */
//	virtual bool stateMachine( short state, DevCommand command) throw (Exception);

	//! Sets a new device state
//	virtual void setDeviceState( short state) throw (Exception);

	//! Returns the value of the requested device resource
//	virtual void deviceQueryResource() throw (Exception);

	static void tacoDeviceRestart( ::TACO::Server *server, void* argin, void* argout) throw (Exception);

	static void tacoServerRestart( ::TACO::Server *server, void* argin, void* argout) throw (Exception);

	static void tacoClassQuery( ::TACO::Server *server, void* argin, void* argout) throw (Exception);

	static void tacoDeviceQuery( ::TACO::Server *server, void* argin, void* argout) throw (Exception);

	static void tacoKill( ::TACO::Server *server, void* argin, void* argout) throw (Exception);
};

#endif // TACO_ADMIN_H
