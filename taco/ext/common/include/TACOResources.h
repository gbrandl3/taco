#ifndef TACO_RESOURCES_H
#define TACO_RESOURCES_H

#include <set>
#include <string>

#include <dev_xdr.h>
#include <DevXdrKernel.h>
#include <TACOException.h>

namespace TACO {
	const std::string RESOURCE_INFO_VERSION = "1";

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
