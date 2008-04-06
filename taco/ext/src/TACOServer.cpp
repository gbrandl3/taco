/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>
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

#include <TACOServer.h>

#include <private/ApiP.h>

std::string TACO::Server::sServerName = config_flags.server_name;
std::string TACO::Server::sStringBuffer;

TACO::Server::Server( const std::string& name, DevLong& error) throw (TACO::Exception)
	: Device(const_cast<char *>(name.c_str()), &error)
	, mInitialized( false) 
	, mName( name) 
	, mVersion( "unknown")
	, mAlwaysAllowDeviceUpdate( false)
{
	addCommand( Command::DEVICE_ON, &tacoDeviceOn, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Device On");
	addCommand( Command::DEVICE_OFF, &tacoDeviceOff, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Device Off");
	addCommand( Command::DEVICE_RESET, &tacoDeviceReset, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Device Reset");
	addCommand( Command::DEVICE_STATE, &tacoDeviceState, D_VOID_TYPE, D_SHORT_TYPE, READ_ACCESS, "Device State");
	addCommand( Command::DEVICE_STATUS, &tacoDeviceStatus, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "Device Status");
	addCommand( Command::DEVICE_VERSION, &tacoDeviceVersion, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "Device Version");
	addCommand( Command::DEVICE_TYPES, &tacoDeviceTypes, D_VOID_TYPE, D_VAR_STRINGARR, READ_ACCESS, "Device Types");
	addCommand( Command::DEVICE_QUERY_RESOURCE, &tacoDeviceQueryResource, D_VAR_CHARARR, D_VAR_CHARARR, READ_ACCESS, "Device Query Resources");
	addCommand( Command::DEVICE_UPDATE_RESOURCE, &tacoDeviceUpdateResource, D_VAR_CHARARR, D_VOID_TYPE, SI_WRITE_ACCESS, "Device Update Resources");
	addCommand( Command::DEVICE_UPDATE, &tacoDeviceUpdate, D_VOID_TYPE, D_VOID_TYPE, SI_SU_ACCESS, "Device Update");
	addCommand( Command::DEVICE_QUERY_RESOURCE_INFO, &tacoDeviceQueryResourceInfo, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "Device Query Resource Info");
	setDeviceState(State::DEVICE_OFF);
//	setServerName("TACO::Server");
}

long TACO::Server::Command(DevCommand command, DevArgument argin, DevArgType inputType, DevArgument argout, DevArgType outputType, DevLong* error)
{
	DevShort status;
	try {
		lock();
		CommandMap::const_iterator i( mCommandMap.find( command));
		if (i != mCommandMap.end()) {
			if (i->second.inputType == inputType && i->second.outputType == outputType) {
				status = deviceState();
				switch (command)
				{
					case Command::DEVICE_STATE:
						assign(static_cast<DevShort*>(argout), status);
						break;
                			case Command::DEVICE_STATUS:
						assign(static_cast<DevVarCharArray*>(argout), bufferedString(stateDescription(status)));
						break;
					default:
						if (stateMachine(status, command)) {
							(*(i->second.function))( this, argin, argout);
						} else {
							throw Exception( Error::EXECUTION_DENIED);
						}
				}
			} else {
				throw Exception( Error::TYPE_ERROR);
			}
		} else {
			throw Exception( Error::RUNTIME_ERROR, "command not supported");
		}
		unlock();
	} catch (const Exception& e) {
		*error = e;
		pushErrorMessage( e.what());
		unlock();
		return DS_NOTOK;
	} catch (...) {
		*error = Exception( Error::UNEXPECTED_EXCEPTION);
		unlock();
		return DS_NOTOK;
	}
	return DS_OK;
}

const char* TACO::Server::GetClassName()
{
	return "TACO::Server";
}

const char* TACO::Server::GetDevType()
{
	return "TACO::Server";
}

const char* TACO::Server::GetDevName()
{
	return mName.c_str();
}

long TACO::Server::GetMinAccessRight(DevCommand command)
{
	CommandMap::const_iterator i( mCommandMap.find( command));
	if (i != mCommandMap.end()) {
		return i->second.minAccess;
	} else {
		throw Error::RUNTIME_ERROR;
	}
}

unsigned TACO::Server::GetCommandNumber()
{
	return mCommandMap.size();
}

long TACO::Server::CommandQuery( _dev_cmd_info* info)
{
	int i = 0;
	
	for (CommandMap::const_iterator it= mCommandMap.begin(); it != mCommandMap.end(); ++it, ++i) 
	{
		info [i].cmd = it->first;
		info [i].in_type = it->second.inputType;
		info [i].out_type = it->second.outputType;
	}
	return DS_OK;
}

unsigned TACO::Server::GetEventNumber()
{
	return mEventMap.size();
}

long TACO::Server::EventQuery( _dev_event_info* info)
{
	int i = 0;
	EventMap::const_iterator it( mEventMap.begin());
	while (it != mEventMap.end()) {
		info [i].event = it->first;
		info [i++].out_type = (it++)->second.type;
	}
	return DS_OK;
}

const std::string& TACO::Server::bufferedString( const std::string& temporaryString) throw()
{
	sStringBuffer = temporaryString;
	return sStringBuffer;
}

bool TACO::Server::stateMachine( DevShort state, DevCommand command) throw (TACO::Exception)
{
	switch (state) {
	case State::FAULT:
		switch (command) {
		case Command::DEVICE_STATE:
		case Command::DEVICE_STATUS:
		case Command::DEVICE_VERSION:
		case Command::DEVICE_RESET:
		case Command::DEVICE_TYPES:
			return true;
		default:
			return false;
		}
	case State::DEVICE_OFF:
		switch (command) {
		case Command::DEVICE_STATE:
		case Command::DEVICE_STATUS:
		case Command::DEVICE_ON:
		case Command::DEVICE_OFF:
		case Command::DEVICE_RESET:
		case Command::DEVICE_VERSION:
		case Command::DEVICE_TYPES:
		case Command::DEVICE_QUERY_RESOURCE:
		case Command::DEVICE_UPDATE_RESOURCE:
		case Command::DEVICE_UPDATE:
		case Command::DEVICE_QUERY_RESOURCE_INFO:
			return true;
		default:
			return false;
		}
	default:
		switch (command) {
		case Command::DEVICE_UPDATE_RESOURCE:
		case Command::DEVICE_UPDATE:
			return isDeviceUpdateAlwaysAllowed();
		default:
			return true;
		}
	}
}

void TACO::Server::deviceOn() throw (TACO::Exception)
{
	if (isDeviceOff()) {
		if (!isDeviceInitialized()) {
			deviceUpdate();
		}
		setDeviceState( State::DEVICE_NORMAL);
	}
}

void TACO::Server::deviceOff() throw (TACO::Exception)
{
	setDeviceState( State::DEVICE_OFF);
}

void TACO::Server::deviceReset() throw (TACO::Exception)
{
	deviceOff();
}

DevShort TACO::Server::deviceState() throw (TACO::Exception)
{
	return mState;
}

void TACO::Server::setDeviceState( short state) throw (TACO::Exception)
{
	mState = state;
}

const std::string& TACO::Server::deviceStatus() throw (TACO::Exception)
{
	return bufferedString( stateDescription( deviceState()));
}

const std::string& TACO::Server::deviceVersion() throw (TACO::Exception)
{
	return mVersion;
}

void TACO::Server::deviceQueryResource() throw (TACO::Exception)
{
	// VOID
}

void TACO::Server::deviceUpdate() throw (TACO::Exception)
{
	mInitialized = true;
}

std::string TACO::Server::serverName() throw (TACO::Exception)
{
	if (sServerName.empty()) {
		throw Exception( Error::INTERNAL_ERROR, "server name not initialized");
	} else {
		return sServerName;
	}
}

void TACO::Server::setServerName(const std::string &serverName) throw (::TACO::Exception)
{
	sServerName = serverName;
}

void TACO::Server::setDeviceVersion( const std::string& version) throw (TACO::Exception)
{
	mVersion = version;
}

const std::vector<std::string>& TACO::Server::deviceTypes() throw (TACO::Exception)
{
	return mTypes;
}

void TACO::Server::deviceUpdate( const std::string& resourceName) throw (Exception)
{
	// Initialize the data for the resourceUpdateRequest() method
	mResourceName = resourceName;
	mResourceActionRequest = false;

	// Do the update
	deviceUpdate();

	// Check if a update was requested
	if (!mResourceName.empty() && !mResourceActionRequest) {
		// Cleanup
		mResourceName.erase();

		throw Exception( Error::RUNTIME_ERROR, "resource not updated");
	}

	// Cleanup
	mResourceName.erase();
}

void TACO::Server::addResource(
	const std::string& name,
	DevType type,
	const std::string& info,
	const std::string& defaults,
	const std::string& format,
	const std::string& min,
	const std::string& max
) throw (TACO::Exception)
{
	if (hasResource( name)) {
		throw Exception( Error::INTERNAL_ERROR, "resource exists already");
	}
	mResourceInfoSet.insert( ResourceInfo( toLowerCase( name), type, info, defaults, format, min, max));
}

void TACO::Server::tacoDeviceOn( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	server->deviceOn();
}

void TACO::Server::tacoDeviceOff( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	server->deviceOff();
}

void TACO::Server::tacoDeviceReset( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	server->deviceReset();
}

void TACO::Server::tacoDeviceState( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	assign( static_cast<DevShort*>( argout), server->deviceState());
}

void TACO::Server::tacoDeviceStatus( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	assign( static_cast<DevVarCharArray*>( argout), server->deviceStatus());
}

void TACO::Server::tacoDeviceVersion( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	assign( static_cast<DevVarCharArray*>( argout), server->deviceVersion());
}

void TACO::Server::tacoDeviceTypes( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	assign( static_cast<DevVarStringArray*>( argout), server->deviceTypes());
}

void TACO::Server::tacoDeviceQueryResource( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	std::string resourceName( convert( static_cast<DevVarCharArray*>( argin)));

	// Check if the resouce is registered
	if (!server->hasResource( resourceName)) {
		throw Exception( Error::RUNTIME_ERROR, "resource not supported");
	}

	// Initialize the data for the resourceQueryRequest() method
	server->mResourceName = resourceName;
	server->mResourceActionRequest = false;

	// Do the query
	server->deviceQueryResource();

	// Cleanup
	server->mResourceName.erase();

	// Check if a query was requested
	if (!server->mResourceActionRequest) {
		throw Exception( Error::RUNTIME_ERROR, "resource not queried");
	}

	// Return the current resource value
	sStringBuffer = server->queryResource<std::string>( resourceName);
	assign( static_cast<DevVarCharArray*>( argout), sStringBuffer);
}

void TACO::Server::tacoDeviceUpdateResource( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	std::string input( convert( static_cast<DevVarCharArray*>( argin)));

	// Get resource name and value
	std::string::size_type pos = input.find( ' ');
	if (pos == std::string::npos) {
		throw Exception( Error::RUNTIME_ERROR, "cannot extract resource name");
	}
	std::string resourceName( input.substr( 0, pos));
	input.erase( 0, pos + 1);

	// Check if the resouce is registered
	if (!server->hasResource( resourceName)) {
		throw Exception( Error::RUNTIME_ERROR, "resource not supported");
	}

	// Save the current resource value and update it
	std::string lastValue( server->queryResource<std::string>( resourceName));
	server->updateResource<std::string>( resourceName, input);

	try {
		// Do the update
		server->deviceUpdate( resourceName);
	} catch (Exception) {
		// Restore the previous value
		server->updateResource<std::string>( resourceName, lastValue);

		throw;
	}
}

void TACO::Server::tacoDeviceUpdate( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	// The empty string indicates a full device update
	server->deviceUpdate( std::string());
}

void TACO::Server::tacoDeviceQueryResourceInfo( TACO::Server* server, void* argin, void* argout) throw (TACO::Exception)
{
	sStringBuffer.erase();
	sStringBuffer << server->mResourceInfoSet;
	assign( static_cast<DevVarCharArray*>( argout), sStringBuffer);
}
