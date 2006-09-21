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

#ifndef TACO_CONVERTERS_H
#define TACO_CONVERTERS_H

#include <string>
#include <vector>

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <dev_xdr.h>

#include <TACOException.h>

namespace TACO {
	inline bool convert( DevBoolean* argin) throw()
	{
		return (*argin == 0) ? false : true;
	}

	inline short convert( DevShort* argin) throw()
	{
		return *argin;
	}

	inline unsigned short convert( DevUShort* argin) throw()
	{
		return *argin;
	}

	inline long convert( DevLong* argin) throw()
	{
		return *argin;
	}

	inline unsigned long convert( DevULong* argin) throw()
	{
		return *argin;
	}

	inline float convert( DevFloat* argin) throw()
	{
		return *argin;
	}

	inline double convert( DevDouble* argin) throw()
	{
		return *argin;
	}

	inline std::string convert( DevVarCharArray* argin) throw()
	{
		return std::string( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<std::string > convert( DevVarStringArray* argin) throw()
	{
		std::vector<std::string> tmp;
		tmp.clear();
		for (unsigned int i = 0; i < argin->length; ++i)
			tmp.push_back(argin->sequence[i]);
		return tmp;
	}

	inline std::vector<short> convert( DevVarShortArray* argin) throw()
	{
		return std::vector<short>( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<unsigned short> convert( DevVarUShortArray* argin) throw()
	{
		return std::vector<unsigned short>( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<long> convert( DevVarLongArray* argin) throw()
	{
		return std::vector<long>( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<unsigned long> convert( DevVarULongArray* argin) throw()
	{
		return std::vector<unsigned long>( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<float> convert( DevVarFloatArray* argin) throw()
	{
		return std::vector<float>( argin->sequence, argin->sequence + argin->length);
	}

	inline std::vector<double> convert( DevVarDoubleArray* argin) throw()
	{
		return std::vector<double>( argin->sequence, argin->sequence + argin->length);
	}

	// The assignment functions assume that the right hand data is not modified in any way
	// during the usage of the left hand data.

	inline void assign( DevBoolean* left, bool right) throw ()
	{
		*left = right;
	}

	inline void assign( DevShort* left, short right) throw ()
	{
		*left = right;
	}

	inline void assign( DevUShort* left, unsigned short right) throw ()
	{
		*left = right;
	}

	inline void assign( DevLong* left, long right) throw ()
	{
		*left = right;
	}

	inline void assign( DevULong* left, unsigned long right) throw ()
	{
		*left = right;
	}

	inline void assign( DevFloat* left, float right) throw ()
	{
		*left = right;
	}

	inline void assign( DevDouble* left, double right) throw ()
	{
		*left = right;
	}

	inline void assign( DevVarCharArray* left, const std::string& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<char*>( right.data());
	}

	// This implementation works only due to the serial execution of TACO commands within a server process.
	// This function is not reentrant.  You have to ensure that nobody uses the previous assigned data
	// if you call the funciton again because every function call makes the previous assigned data invalid.
	inline void assign( DevVarStringArray* left, const std::vector<std::string>& right) throw ()
	{
		static DevString* p = 0;
		if (p != 0) {
			free( p);
			p = 0;
		}
		p = static_cast<DevString*>( malloc( sizeof( DevString*) * right.size()));
		if (p != 0) {
			for (std::vector<std::string>::size_type i = 0; i < right.size(); ++i) {
				p [i] = const_cast<char*>( right [i].c_str());
			}
			left->length = right.size();
		} else {
			left->length = 0;
		}
		left->sequence = p; 
	}

	inline void assign( DevVarShortArray* left, const std::vector<short>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<short*>( &right.front());
	}

	inline void assign( DevVarUShortArray* left, const std::vector<unsigned short>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<unsigned short*>( &right.front());
	}

	inline void assign( DevVarLongArray* left, const std::vector<long>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<long*>( &right.front());
	}

	inline void assign( DevVarULongArray* left, const std::vector<unsigned long>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<unsigned long*>( &right.front());
	}

	inline void assign( DevVarFloatArray* left, const std::vector<float>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<float*>( &right.front());
	}

	inline void assign( DevVarDoubleArray* left, const std::vector<double>& right) throw ()
	{
		left->length = right.size();
		left->sequence = (left->length == 0) ? 0 : const_cast<double*>( &right.front());
	}

	inline void assign( struct timespec& left, double right) throw (Exception)
	{
		if (0.0 <= right && right < LONG_MAX) {
			left.tv_nsec = static_cast<long>( modf( right, &right) * 1E9);
			left.tv_sec = static_cast<long>( right);
		} else {
			throw Exception( Error::RANGE_ERROR);
		}
	}

	inline void assign( double& left, struct timespec right) throw ()
	{
		left = static_cast<double>( right.tv_sec) + static_cast<double>( right.tv_nsec) / 1E9;
	}

	inline void assign( struct timeval& left, double right) throw (Exception)
	{
		if (0.0 <= right && right < LONG_MAX) {
			left.tv_usec = static_cast<long>( modf( right, &right) * 1E6);
			left.tv_sec = static_cast<long>( right);
		} else {
			throw Exception( Error::RANGE_ERROR);
		}
	}

	inline void assign( double& left, struct timeval right) throw ()
	{
		left = static_cast<double>( right.tv_sec) + static_cast<double>( right.tv_usec) / 1E6;
	}
}

#endif // TACO_CONVERTERS_H
