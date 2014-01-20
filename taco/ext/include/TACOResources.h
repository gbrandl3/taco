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

#ifndef TACO_RESOURCES_H
#define TACO_RESOURCES_H

#include <set>
#include <string>

#include <dev_xdr.h>
#include <DevXdrKernel.h>
#include <TACOException.h>

namespace TACO {
#ifndef TACO_JAVA 
	const std::string RESOURCE_INFO_VERSION = "1";

#endif
	const char RESOURCE_INFO_SEPERATOR = '\0';
	struct ResourceInfo {
		ResourceInfo(
			const std::string& iName,
			DevType iType = D_VOID_TYPE,
			const std::string& iInfo = std::string(),
			const std::string& iDefaults = std::string(),
			const std::string& iFormat = std::string(),
			const std::string& iMin = std::string(),
			const std::string& iMax = std::string()
		) throw ()
			: name( iName),
			  type( iType),
			  info( iInfo),
			  defaults( iDefaults),
			  format( iFormat),
			  min( iMin),
			  max( iMax)
		{
			// VOID
		}

		ResourceInfo() throw ()
		{
			// VOID
		}

		bool operator==( const ResourceInfo& right) const throw ()
		{
			return name == right.name;
		}

		bool operator!=( const ResourceInfo& right) const throw ()
		{
			return name != right.name;
		}

		bool operator<( const ResourceInfo& right) const throw ()
		{
			return name < right.name;
		}

		std::string name;

		DevType type;

		std::string info;

		std::string defaults;

		std::string format;

		std::string min;

		std::string max;
	};

	typedef std::set<ResourceInfo> ResourceInfoSet;
}

std::string& operator>>( std::string& src, TACO::ResourceInfo& dest) throw (TACO::Exception);

std::string& operator<<( std::string& dest, const TACO::ResourceInfo& src) throw ();

std::string& operator>>( std::string& src, TACO::ResourceInfoSet& dest) throw (TACO::Exception);

std::string& operator<<( std::string& dest, const TACO::ResourceInfoSet& src) throw ();

#endif // TACO_RESOURCES_H
