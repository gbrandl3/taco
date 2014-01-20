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

#include <sstream>

#include <TACOResources.h>

using namespace TACO;

std::string& operator>>( std::string& src, ResourceInfo& dest) throw (Exception)
{
	const std::string TYPE_ERROR_MESSAGE( "cannot convert 'std::string' to 'TACO::ResourceInfo'");

	// Name
	std::string::size_type begin( 0);
	std::string::size_type end( src.find( RESOURCE_INFO_SEPERATOR));
	if (end != std::string::npos) {
		dest.name = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Type
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		std::stringstream type( src.substr( begin, end - begin));
		type >> dest.type;
		if (!type) {
			throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
		}
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Info
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		dest.info = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Defaults
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		dest.defaults = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Format
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		dest.format = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Min
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		dest.min = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	// Max
	end = src.find( RESOURCE_INFO_SEPERATOR, begin);
	if (end != std::string::npos) {
		dest.max = src.substr( begin, end - begin);
		begin = end + 1;
	} else {
		throw Exception( Error::TYPE_ERROR, TYPE_ERROR_MESSAGE);
	}

	src.erase( 0, begin);
	return src;
}

std::string& operator<<( std::string& dest, const ResourceInfo& src) throw ()
{
	std::stringstream type;
	type << src.type;
	dest += src.name + RESOURCE_INFO_SEPERATOR
		+ type.str() + RESOURCE_INFO_SEPERATOR
		+ src.info + RESOURCE_INFO_SEPERATOR
		+ src.defaults + RESOURCE_INFO_SEPERATOR
		+ src.format + RESOURCE_INFO_SEPERATOR
		+ src.min + RESOURCE_INFO_SEPERATOR
		+ src.max + RESOURCE_INFO_SEPERATOR;
	return dest;
}

std::string& operator>>( std::string& src, ResourceInfoSet& dest) throw (Exception)
{
	// Check resource version
	std::string::size_type pos( src.find( RESOURCE_INFO_SEPERATOR));
	if (pos == std::string::npos || RESOURCE_INFO_VERSION != src.substr( 0, pos)) {
		throw Exception( Error::RUNTIME_ERROR, "incompatible client and server resource versions");
	}
	src.erase( 0, pos + 1);

	ResourceInfo res;
	while (!src.empty()) {
		src >> res;
		dest.insert( res);
	}
	return src;
}

std::string& operator<<( std::string& dest, const ResourceInfoSet& src) throw ()
{
	ResourceInfoSet::const_iterator i( src.begin());
	dest += RESOURCE_INFO_VERSION + RESOURCE_INFO_SEPERATOR;
	while (i != src.end()) {
		dest << *i++;
	}
	return dest;
}
