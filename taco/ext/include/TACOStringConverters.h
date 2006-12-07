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

#ifndef TACO_STRING_CONVERTERS_H
#define TACO_STRING_CONVERTERS_H

#include <iomanip>
#include <sstream>
#include <string>
#include <list>

#include <stdlib.h>

#include <TACOException.h>
#include <TACOTypes.h>

namespace TACO {
	//! Allocates a device argument
	void* allocateDevArgument( DevType type) throw (Exception);

	//! Frees a device argument
	void freeDevArgument( void* arg) throw ();

	//! Frees a device argument and its data
	void freeDevArgument( DevType type, void* arg) throw (Exception);

	template<typename T> void copyDevVarArray( const T* src, std::stringstream& dest) throw ()
	{
		if (src->length > 0) {
			dest << src->sequence [0];
			for (unsigned int i = 1; i < src->length; ++i) {
				dest << " " << src->sequence [i];
			}
		}
	}

	void copy( const std::string& src, DevString* dest)
		throw (Exception);

	void copy( const std::string& src, DevVarCharArray* dest)
		throw (Exception);

	template<typename T> void copyToDevVarArray( std::stringstream& src,
		T* dest) throw (Exception)
	{
		typedef typename Type<T>::ValueType ValueType;
		std::list<ValueType> valueList;
		ValueType value;
		src >> value;
		while (src) {
			valueList.push_back( value);
			src >> value;
		}
		dest->length = valueList.size();
		if (dest->length > 0) {
			if ((dest->sequence = static_cast<ValueType*>( malloc( dest->length * sizeof( ValueType)))) == 0) {
				throw Exception( Error::NO_MEMORY);
			}
			typename std::list<ValueType>::const_iterator i( valueList.begin());
			while (i != valueList.end()) {
				*dest->sequence++ = *i++;
			}
		} else {
			dest->sequence = 0;
		}
	}

	std::string convertToString( DevType type, const void* input)
		throw (Exception);

	/**
	 * Converts a string to a device argument.
	 *
	 * @warning
	 * You must use freeDevArgument( type, arg) to free the created argument.
	 */
	void* convertToDevArgument( DevType type, const std::string& input)
		throw (Exception);

	/**
	  * Coverts a number to a string.
	  *
	  * @warning
	  * type of number must support c++ style << operator.
	  */
	template< class T > std::string numberToString( T v, short precision = 2 )
	{
		std::ostringstream os;
		os << std::fixed <<  std::setprecision(precision) << v;
		return os.str();
	}
}

#endif // TACO_STRING_CONVERTERS_H
