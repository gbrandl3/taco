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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <TACOBasicCommands.h>
#include <TACOConverters.h>
#include <TACOStringConverters.h>

#include <TACOAdmin.h>
#include <TACOAdminCommands.h>

TACO::Admin::Admin( const std::string& name, DevLong& error) throw (TACO::Exception)
	: TACO::Server::Server(name, error)
{
	mCommandMap.clear();

        addCommand( ::TACO::Command::Admin::DEVICE_RESTART, &tacoDeviceRestart, D_STRING_TYPE, D_VOID_TYPE); // "DevRestart", "Device name"
        addCommand( ::TACO::Command::Admin::SERVER_RESTART, &tacoServerRestart, D_VOID_TYPE, D_VOID_TYPE);   // "RestartServer"
        addCommand( ::TACO::Command::Admin::CLASS_QUERY, &tacoClassQuery, D_VOID_TYPE, D_VAR_STRINGARR);     // "QueryClass", "Device server class(es) list"
        addCommand( ::TACO::Command::Admin::DEVICE_QUERY, &tacoDeviceQuery, D_VOID_TYPE, D_VAR_STRINGARR);   // "QueryDevice", "Device server device(s) list"
        addCommand( ::TACO::Command::Admin::DEVICE_KILL, &tacoKill, D_VOID_TYPE, D_VOID_TYPE); 		     // "Kill"

	setDeviceState(State::DEVICE_OFF);
	setServerName("TACO::Admin");
	setDeviceVersion(VERSION);
}

void TACO::Admin::deviceRestart(const std::string &deviceName) throw (::TACO::Exception)
{
	return;
}

void TACO::Admin::serverRestart(void) throw (::TACO::Exception)
{
	return;
}

const std::vector<std::string>& TACO::Admin::classQuery(void) throw (::TACO::Exception)
{
	static std::vector <std::string> tmp;
	return tmp;
}

const std::vector<std::string>& TACO::Admin::deviceQuery(void) throw (::TACO::Exception)
{
	static std::vector <std::string> tmp;
	return tmp;
}

void TACO::Admin::kill(void) throw (::TACO::Exception)
{
	return;
}

void TACO::Admin::tacoDeviceRestart( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	std::string deviceName( *static_cast<DevString *>( argin));
	Admin *s = dynamic_cast<Admin*>( server);
	if (s != 0) {
		s->deviceRestart(deviceName);
        } else {
                throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void TACO::Admin::tacoServerRestart( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{ 
	Admin *s = dynamic_cast<Admin*>( server);
	if (s != 0) {
		s->serverRestart();
        } else {
                throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
        }
}

void TACO::Admin::tacoClassQuery( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	Admin *s = dynamic_cast<Admin*>( server);
	if (s != 0) {
		assign( static_cast<DevVarStringArray*>( argout), s->classQuery());
        } else {
                throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void TACO::Admin::tacoDeviceQuery( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	Admin *s = dynamic_cast<Admin*>( server);
	if (s != 0) {
		assign( static_cast<DevVarStringArray*>( argout), s->deviceQuery());
        } else {
                throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

void TACO::Admin::tacoKill( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	Admin *s = dynamic_cast<Admin*>( server);
	if (s != 0) {
		s->kill();
        } else {
                throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
	}
}

