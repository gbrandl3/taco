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

// C++ includes
#include <algorithm>
#include <string>

// C includes
#include <cctype>
#include <cstdlib>

// TACO includes
#include <TACOExtensions.h>

pthread_mutex_t TACO::mMutex = PTHREAD_MUTEX_INITIALIZER;

TACO::CommandInfoMap TACO::queryAllCommandInfo( devserver ds) throw (TACO::Exception)
{
	CommandInfoMap r;
	DevVarCmdArray info;
	DevLong e;
	pthread_mutex_lock(&TACO::mMutex);
	DevLong res = dev_cmd_query( ds, &info, &e);
	pthread_mutex_unlock(&TACO::mMutex);
	if (res != DS_OK) {
		throw Exception( e);
	}
	for (unsigned int i = 0; i < info.length; ++i) {
		r [info.sequence [i].cmd] = CommandInfo(
			info.sequence [i].cmd_name,
			info.sequence [i].in_type,
			info.sequence [i].in_name != 0 ? info.sequence [i].in_name : std::string(),
			info.sequence [i].out_type,
			info.sequence [i].out_name != 0 ? info.sequence [i].out_name : std::string()
		);
		free( info.sequence [i].in_name);
		free( info.sequence [i].out_name);
	}
	free( info.sequence);
	return r;
}

TACO::CommandInfo TACO::queryCommandInfo( devserver ds, DevCommand commandNumber) throw (TACO::Exception)
{
	CommandInfoMap info = queryAllCommandInfo( ds);
	CommandInfoMap::const_iterator i( info.find( commandNumber));
	if (i != info.end()) {
		return i->second;
	} else {
		throw Exception( Error::RUNTIME_ERROR, "command not supported");
	}
}

TACO::EventInfoMap TACO::queryAllEventInfo( devserver ds) throw (TACO::Exception)
{
	EventInfoMap r;
	DevVarEventArray info;
	DevLong e;
	pthread_mutex_lock(&TACO::mMutex);
	DevLong res = dev_event_query( ds, &info, &e);
	pthread_mutex_unlock(&TACO::mMutex);
	if (res != DS_OK) {
		throw Exception( e);
	}
	for (unsigned int i = 0; i < info.length; ++i) {
		r [info.sequence [i].event] = EventInfo(
			info.sequence [i].event_name,
			info.sequence [i].out_type,
			info.sequence [i].out_name
		);
		free( info.sequence [i].out_name);
	}
	free( info.sequence);
	return r;
}

TACO::EventInfo TACO::queryEventInfo( devserver ds, DevEvent eventNumber) throw (TACO::Exception)
{
	EventInfoMap info = queryAllEventInfo( ds);
	EventInfoMap::const_iterator i( info.find( eventNumber));
	if (i != info.end()) {
		return i->second;
	} else {
		throw Exception( Error::RUNTIME_ERROR, "event not supported");
	}
}

std::string TACO::toLowerCase( const std::string& s) throw ()
{
	std::string r( s);
	std::transform( r.begin(), r.end(), r.begin(), tolower);
	return r;
}

std::string TACO::toUpperCase( const std::string& s) throw ()
{
	std::string r( s);
	std::transform( r.begin(), r.end(), r.begin(), toupper);
	return r;
}

std::vector<std::string> TACO::queryDeviceList( const std::string& serverName) throw (TACO::Exception)
{
	char** list;
	unsigned int size;
	DevLong e;
	
	pthread_mutex_lock(&TACO::mMutex);
	DevLong res = db_getdevlist( const_cast<char*>(serverName.c_str()), &list, &size, &e);
	pthread_mutex_unlock(&TACO::mMutex);
	if (res != DS_OK) {
		throw Exception( e);
	}
	std::vector<std::string> r( size);
	for (unsigned int i = 0; i < size; ++i) {
		r [i] = list [i];
		free( list [i]);
	}
	free( list);
	return r;
}

void TACO::deleteResource( const std::string& deviceName, const std::string& resourceName) throw (TACO::Exception)
{
	char* r = const_cast<char*>( resourceName.c_str());
	DevLong e;
	pthread_mutex_lock(&TACO::mMutex);
	DevLong res = db_delresource( const_cast<char*>( deviceName.c_str()), &r, 1, &e);
	pthread_mutex_unlock(&TACO::mMutex);
	if (res != DS_OK) {
		throw Exception( e);
	}
}

std::vector< std::string > TACO::extractStringTokens( const std::string& input, const std::string& delimiters ) throw()
{       
	std::vector< std::string > tokens;
	std::string::size_type 	indexBegin = input.find_first_not_of( delimiters ), 
				indexEnd = std::string::npos;

	while( indexBegin != std::string::npos )
	{
		indexEnd = input.find_first_of( delimiters, indexBegin );
		if( indexEnd == std::string::npos )
			indexEnd = input.length();

		tokens.push_back( std::string( input, indexBegin, indexEnd - indexBegin  ) );

		indexBegin = input.find_first_not_of( delimiters, indexEnd );
	}
	
	return tokens;  
}

