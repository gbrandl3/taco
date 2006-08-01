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

#ifndef TACO_SERVER_H
#define TACO_SERVER_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <API.h>
#include <DevServer.h>

#include <Device.h>

#include <TACOArgPutGet.h>
#include <TACOException.h>
#include <TACOExtensions.h>
#include <TACOResources.h>
#include <TACOStates.h>

namespace TACO {
	class Server;
}

//! Base class for all server devices
class TACO::Server : public Device {
public:
	//! Creates a server device
	Server( const std::string& name, DevLong& error) throw (Exception);

	//! Destroys the server device
	virtual ~Server() throw ()
	{
		// VOID
	}

	//! Switches the device on
	virtual void deviceOn() throw (Exception);

	//! Switches the device off
	virtual void deviceOff() throw (Exception);

	//! Returns true if the device is off
	bool isDeviceOff() throw ()
	{
		return deviceState() == State::DEVICE_OFF;
	}

	//! Resets the device
	virtual void deviceReset() throw (Exception);

	//! Returns the current device state
	virtual short deviceState() throw (Exception);

	//! Returns the current device status
	const std::string& deviceStatus() throw (Exception);

	//! Returns the device version
	const std::string& deviceVersion() throw (Exception);

	//! Updates the device resources
	virtual void deviceUpdate() throw (Exception);

	//! Returns the device name
	std::string deviceName() const throw ()
	{
		return mName;
	}

	//! Sets the server name
	static void setServerName(const std::string &name) throw (Exception);

	/**
	 * Returns the server name. 
	 *
	 * @warning You have to initalize the server name manually, for example in startup().
	 */
	static std::string serverName() throw (Exception);

	/**
	 * @name Internal Methods
	 */
	//@{

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	DevLong Command( DevCommand command, void* argin, DevType inputType, void* argout, DevType outputType, DevLong* error);

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	const char* GetClassName();

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	const char* GetDevType();

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	const char* GetDevName();

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	DevLong GetMinAccessRight( DevCommand);

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	unsigned GetCommandNumber();

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	DevLong CommandQuery( _dev_cmd_info* info);

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	unsigned GetEventNumber();

	/**
	 * Internal method.
	 *
	 * @warning This is an internal method.  Do not use it!
	 */
	DevLong EventQuery( _dev_event_info* info);

	//@}

protected:
	//! Device command function pointer type
	typedef void (*Function)(Server*, void*, void*);

	/**
	 * Returns a buffered copy of the string.
	 *
	 * Use this function to return temporary string objects in a %TACO command.
	 */
	static const std::string& bufferedString( const std::string& temporaryString) throw();

	//! Forbidden default constructor
	Server()
	{
		throw Exception( Error::INTERNAL_ERROR, "called forbidden default constructor");
	}

	//! Forbidden copy constructor
	Server( const Server& server)
		: Device()
	{
		throw Exception( Error::INTERNAL_ERROR, "called forbidden copy constructor");
	}

	//! Forbidden assignment operator
	Server& operator=( const Server& server)
	{
		throw Exception( Error::INTERNAL_ERROR, "called forbidden assignment operator");
	}

	//! Returns true if the device is initialized
	bool isDeviceInitialized() throw ()
	{
		return mInitialized;
	}

	/**
	 * %State machine.
	 *
	 * The state machine is invoked by the Command() method.  It assigns the current device state to
	 * <em>state</em> and the number of the current command to <em>command</em>.
	 * The state machine must return true, if the command execution is allowed in the current device state.
	 * If false is returned, the Command() method denies the command execution and throws the EXECUTION_DENIED exception.
	 */
	virtual bool stateMachine( short state, DevCommand command) throw (Exception);

	//! Sets a new device state
	virtual void setDeviceState( short state) throw (Exception);

	//! Sets the device version
	void setDeviceVersion( const std::string& version) throw (Exception);

	//! Returns the device types
	const std::vector<std::string>& deviceTypes() throw (Exception);

	//! Adds a device type
	void addDeviceType( const std::string& deviceType) throw ()
	{
		mTypes.push_back( deviceType);
	}

	//! Starts the device update
	void deviceUpdate( const std::string& resourceName) throw (Exception);

	//! Returns true if the device has this command
	bool hasCommand( DevCommand number) const throw ()
	{
		return mCommandMap.find( number) != mCommandMap.end();
	}

	//! Adds a command
	void addCommand( DevCommand number, Function function, DevType inputType, DevType outputType, DevLong access = 0, std::string name = "") throw (Exception)
	{
		if (hasCommand( number)) {
			throw Exception( Error::INTERNAL_ERROR, "command exists already");
		}
		mCommandMap [number] = CommandMapEntry( function, inputType, outputType);
//		commands_list[number] = DeviceCommandListEntry(number, NULL, (DevType)inputType, (DevType)outputType, access, commandName(number));
//		commands_list[number] = DeviceCommandListEntry(number, NULL, (DevType)inputType, (DevType)outputType, access);
//		this->n_commands = this->commands_list.size();
	}

	//! Returns true if the device has this resource
	bool hasResource( const std::string& name) const throw ()
	{
		return mResourceInfoSet.find( toLowerCase( name)) != mResourceInfoSet.end();
	}

	//! Adds a resource
	void addResource(
		const std::string& name,
		DevType type = D_VOID_TYPE,
		const std::string& info = std::string(),
		const std::string& defaults = std::string(),
		const std::string& format = std::string(),
		const std::string& min = std::string(),
		const std::string& max = std::string()
	) throw (Exception);

	//! Returns true if the device has this event
	bool hasEvent( DevEvent number) const throw ()
	{
		return mEventMap.find( number) != mEventMap.end();
	}

	//! Adds an event
	void addEvent( DevEvent number, DevType type) throw (Exception)
	{
		if (hasEvent( number)) {
			throw Exception( Error::INTERNAL_ERROR, "event exists already");
		}
		mEventMap [number] = EventMapEntry( type);
//		this->n_events = this->events_list.size();
	}

	//! Fires an event
	void fireEvent( DevEvent event, DevArgument argout, DevType argoutType, DevLong status = DS_OK, DevLong error = 0) throw (Exception)
	{
		lock();
#ifdef RUNTIME_TYPE_CHECK
		if (hasEvent( event)) {
			if (mEventMap [event].type != argoutType) {
				throw Exception( Error::INTERNAL_ERROR, "inappropriate output argument type");
			}
		} else {
			throw Exception( Error::INTERNAL_ERROR, "event not supported");
		}
#endif // RUNTIME_TYPE_CHECK
		dev_event_fire( this, event, argout, argoutType, status, error);
		unlock();
	}

	//! Fires an event
	template<typename T> void fireEvent( DevEvent event, const T& output, DevLong status = DS_OK, DevLong error = 0) throw (Exception)
	{
		lock();
		ArgPut<T> argout( output);
#ifdef RUNTIME_TYPE_CHECK
		if (hasEvent( event)) {
			if (mEventMap [event].type != argout.type()) {
				throw Exception( Error::INTERNAL_ERROR, "inappropriate output argument type");
			}
		} else {
			throw Exception( Error::INTERNAL_ERROR, "event not supported");
		}
#endif // RUNTIME_TYPE_CHECK
		dev_event_fire( this, event, argout.address(), argout.type(), status, error);
		unlock();
	}

	//! Fires an event with void parameter type
	void fireSimpleEvent( DevEvent event, DevLong status = DS_OK, DevLong error = 0) throw (Exception)
	{
		lock();
#ifdef RUNTIME_TYPE_CHECK
		if (hasEvent( event)) {
			if (mEventMap [event].type != D_VOID_TYPE) {
				throw Exception( Error::INTERNAL_ERROR, "inappropriate output argument type");
			}
		} else {
			throw Exception( Error::INTERNAL_ERROR, "event not supported");
		}
#endif // RUNTIME_TYPE_CHECK
		dev_event_fire( this, event, 0, D_VOID_TYPE, status, error);
		unlock();
	}

	//! Returns the value of the requested device resource
	virtual void deviceQueryResource() throw (Exception);

	//! Returns true if the resource should be queried
	bool resourceQueryRequest( const std::string& resourceName) throw ()
	{
		if (mResourceName == resourceName) {
			mResourceActionRequest = true;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Makes the resource query successful.
	 *
	 * Use this method if you have nothing to do during a resource query.
	 */
	void makeResourceQuerySuccessful() throw ()
	{
		mResourceActionRequest = true;
	}

	//! Returns true if the resource should be updated
	bool resourceUpdateRequest( const std::string& resourceName) throw ()
	{
		if (mResourceName.empty() || mResourceName == resourceName) {
			mResourceActionRequest = true;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Makes the device update successful.
	 *
	 * Use this method if you have nothing to do during a device update.
	 */
	void makeDeviceUpdateSuccessful() throw ()
	{
		mResourceActionRequest = true;
	}

	//! Allows always (in every device state) the device update if <em>always</em> is true
	void alwaysAllowDeviceUpdate( bool always = true) throw ()
	{
		mAlwaysAllowDeviceUpdate = always;
	}

	//! Returns true if the device update is always allowed
	bool isDeviceUpdateAlwaysAllowed() const throw ()
	{
		return mAlwaysAllowDeviceUpdate;
	}

	template<typename T> T queryResource( const std::string& resourceName) throw (Exception)
	{
		return ::TACO::queryResource<T>( deviceName(), resourceName);
	}

	template<typename T> void updateResource( const std::string& resourceName, const T& resourceValue) throw (Exception)
	{
		::TACO::updateResource<T>( deviceName(), resourceName, resourceValue);
	}

	void deleteResource( const std::string& resourceName) throw (Exception)
	{
		::TACO::deleteResource( deviceName(), resourceName);
	}

private:
	//! Internal server command map entry
	struct CommandMapEntry {
		CommandMapEntry()
			: function( 0), inputType( D_VOID_TYPE), outputType( D_VOID_TYPE), minAccess( 0)
		{
		}

		CommandMapEntry( Function iFunction, DevType iInputType, DevType iOutputType, DevLong iMinAccess = 0)
			: function( iFunction), inputType( iInputType), outputType( iOutputType), minAccess( iMinAccess)
		{
		}

		Function function;
		
		DevType inputType;
		
		DevType outputType;

		DevLong minAccess;
	};

	
	char *commandName(const DevLong num)
	{
		char 	*cmd_name = new char[16];
        	snprintf(cmd_name, 16, "(%li/%li/%li)", ((num >> DS_TEAM_SHIFT) & DS_TEAM_MASK), 
							((num >> DS_IDENT_SHIFT) & DS_IDENT_MASK), 
							(num & 0xFFF));
		return cmd_name;
	}
	
	//! Maps a command number to its command map entry
	typedef std::map<DevCommand, CommandMapEntry> CommandMap;

	//! Internal server event map entry
	struct EventMapEntry {
		EventMapEntry()
			: type( 0)
		{
			// VOID
		}

		EventMapEntry( DevType iType)
			: type( iType)
		{
			// VOID
		}
		
		DevType type;
	};
	
	//! Maps an event number to its event map entry
	typedef std::map<DevEvent, EventMapEntry> EventMap;

	//! Lock global server mutex
	static void lock() throw (Exception)
	{
		// VOID
	}

	//! Unlock global server mutex
	static void unlock() throw (Exception)
	{
		// VOID
	}

	//! Device command function
	static void tacoDeviceOn( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceOff( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceReset( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceState( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceStatus( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceVersion( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceTypes( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceUpdateResource( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceQueryResource( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceUpdate( Server* server, void* argin, void* argout) throw (Exception);

	//! Device command function
	static void tacoDeviceQueryResourceInfo( Server* server, void* argin, void* argout) throw (Exception);

	//! Name of the server of this device
	static std::string sServerName;

	//! General purpose string buffer
	static std::string sStringBuffer;

	//! Indicates if the device is initialized
	bool mInitialized;

	//! Device name
	std::string mName;

	//! Device version
	std::string mVersion;

	//! Current device state
	short mState;

	//! Indicates if the device update is always allowed
	bool mAlwaysAllowDeviceUpdate;

	//! Device command map
	CommandMap mCommandMap;

	//! Device resource set
	ResourceInfoSet mResourceInfoSet;

	//! Device event map
	EventMap mEventMap;

	//! Device types
	std::vector<std::string> mTypes;

	//! Indicates if a resource query or update was requested
	bool mResourceActionRequest;

	//! Current resource name
	std::string mResourceName;
};

#endif // TACO_SERVER_H
