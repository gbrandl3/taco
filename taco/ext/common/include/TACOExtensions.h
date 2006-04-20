#ifndef TACO_EXTENSIONS_H
#define TACO_EXTENSIONS_H

#include <string>
#include <vector>
#include <map>

#include <API.h>

#include <TACOException.h>
#include <TACOArgPutGet.h>

namespace TACO {
	//! Network protocol type
	enum NetworkProtocol {
		//! User Datagram Protocol
		UDP = D_UDP,

		//! Transmission Control Protocol
		TCP = D_TCP
	};

	//! %Command info
	struct CommandInfo {
		CommandInfo(
			const std::string& iName,
			DevType iInputType,
			const std::string& iInputName,
			DevType iOutputType,
			const std::string& iOutputName
		)
			: name( iName),
			  inputType( iInputType),
			  inputName( iInputName),
			  outputType( iOutputType),
			  outputName( iOutputName)
		{
			// VOID
		}

		CommandInfo()
		{
			// VOID
		}

		std::string name;

		DevType inputType;

		std::string inputName;

		DevType outputType;

		std::string outputName;
	};

	//! Maps a command number to its command info
	typedef std::map<DevCommand, CommandInfo> CommandInfoMap;

	//! Queries all command info of a server device
	CommandInfoMap queryAllCommandInfo( devserver ds) throw (Exception);

	//! Queries a command info of a server device
	CommandInfo queryCommandInfo( devserver ds, DevCommand commandNumber) throw (Exception);

	//! Event info
	struct EventInfo {
		EventInfo(
			const std::string& iName,
			DevType iOutputType,
			const std::string& iOutputName
		)
			: name( iName),
			  outputType( iOutputType),
			  outputName( iOutputName)
		{
			// VOID
		}

		EventInfo()
		{
			// VOID
		}

		std::string name;

		DevType outputType;

		std::string outputName;
	};

	//! Maps an event number to its event info
	typedef std::map<DevEvent, EventInfo> EventInfoMap;

	//! Queries all event info of a server device
	EventInfoMap queryAllEventInfo( devserver ds) throw (Exception);

	//! Queries an event info of a server device
	EventInfo queryEventInfo( devserver ds, DevEvent eventNumber) throw (Exception);

	//! Queries the device list of a server
	std::vector<std::string> queryDeviceList( const std::string& serverName) throw (Exception);

	//! Returns the string in lower case
	std::string toLowerCase( const std::string& s) throw ();

	//! Returns the string in upper case
	std::string toUpperCase( const std::string& s) throw ();

	/**
	 * Queries a database resource value.
	 *
	 * @param deviceName Device name.
	 *
	 * @param resourceName Resource name.
	 *
	 * @return Resource value.
	 *
	 * @exception Undocumented
	 * This method uses the db_getresource() function.
	 * The possible errors of this function are not documented.
	 */
	template<typename T> T queryResource( const std::string& deviceName, const std::string& resourceName) throw (Exception)
	{
		ResourceGet<T> a;
		db_resource r;
		r.resource_name = resourceName.c_str();
		r.resource_type = static_cast<short>( a.type());
		r.resource_adr = a.address();
		DevLong e;
		if (db_getresource( const_cast<char*>( deviceName.c_str()), &r, 1, &e) != DS_OK) {
			throw Exception( e);
		}
		return a.object();
	}

	/**
	 * Updates (or creates) a database resource.
	 *
	 * @param deviceName Device name.
	 *
	 * @param resourceName Resource name.
	 *
	 * @param resourceValue Resource value.
	 *
	 * @exception Undocumented
	 * This method uses the db_putresource() function.
	 * The possible errors of this function are not documented.
	 */
	template<typename T> void updateResource(
		const std::string& deviceName,
		const std::string& resourceName,
		const T& resourceValue
	) throw (Exception)
	{
		ResourcePut<T> a( resourceValue);
		db_resource r;
		r.resource_name = resourceName.c_str();
		r.resource_type = static_cast<short>( a.type());
		r.resource_adr = a.address();
		DevLong e;
		if (db_putresource( const_cast<char*>( deviceName.c_str()), &r, 1, &e) != DS_OK) {
			throw Exception( e);
		}
	}

	/**
	 * Deletes a database resource.
	 *
	 * @param deviceName Device name.
	 *
	 * @param resourceName Resource name.
	 *
	 * @exception Undocumented
	 * This method uses the db_delresource() function.
	 * The possible errors of this function are not documented.
	 */
	void deleteResource( const std::string& deviceName, const std::string& resourceName) throw (Exception);

	/**
	 * Splits the string into subparts, divided by delimiters
	 * 
	 * @param input String to split.
	 *
	 * @param delimiters Delimiters whose should divide the string.
	 * 
 	 * @return Vector containing all subparts.
	 */
	std::vector< std::string > extractStringTokens( const std::string& input, const std::string& delimiters = "," ) throw();
}

#endif // TACO_EXTENSIONS_H
