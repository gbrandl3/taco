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

#include <string.h>

#include <API.h>
#include <dev_xdr.h>

#include <TACOStringConverters.h>

void* TACO::allocateDevArgument( DevType type) throw (TACO::Exception)
{
	void* r = 0;
	if (type != D_VOID_TYPE) {
		DevDataListEntry data;
		DevLong e;
		if (xdr_get_type( type, &data, &e) != DS_OK) {
			throw Exception( e);
		}
		r = calloc( 1, static_cast<size_t>( data.size));
		if (r == 0) {
			throw Exception( Error::NO_MEMORY);
		}
	}
	return r;
}

void TACO::freeDevArgument( void* arg) throw ()
{
	free( arg);
	arg = 0;
}

void TACO::freeDevArgument( DevType type, void* arg) throw (TACO::Exception)
{
	switch (type) {
	case D_BOOLEAN_TYPE:
	case D_SHORT_TYPE:
	case D_USHORT_TYPE:
	case D_LONG_TYPE:
	case D_ULONG_TYPE:
	case D_FLOAT_TYPE:
	case D_DOUBLE_TYPE:
		// Do nothing
		break;
	case D_STRING_TYPE:
		free( *static_cast<DevString*>( arg));
		break;
	case D_VAR_CHARARR:
		free( static_cast<DevVarCharArray*>( arg)->sequence);
		break;
	case D_VAR_SHORTARR:
		free( static_cast<DevVarShortArray*>( arg)->sequence);
		break;
	case D_VAR_USHORTARR:
		free( static_cast<DevVarUShortArray*>( arg)->sequence);
		break;
	case D_VAR_LONGARR:
		free( static_cast<DevVarLongArray*>( arg)->sequence);
		break;
	case D_VAR_ULONGARR:
		free( static_cast<DevVarULongArray*>( arg)->sequence);
		break;
	case D_VAR_FLOATARR:
		free( static_cast<DevVarFloatArray*>( arg)->sequence);
		break;
	case D_VAR_DOUBLEARR:
		free( static_cast<DevVarDoubleArray*>( arg)->sequence);
		break;
	default:
		throw Exception( Error::INTERNAL_ERROR, "cannot free data of unsupported type");
	}
	free( arg);
	arg = 0;
}

void TACO::copy( const std::string& src, DevString* dest) throw (TACO::Exception)
{
	if ((*dest = static_cast<DevString>( malloc( strlen( src.c_str()) + 1))) == 0) {
		throw Exception( Error::NO_MEMORY);
	}
	strcpy( *dest, src.c_str());
}

void TACO::copy( const std::string& src, DevVarCharArray* dest) throw (TACO::Exception)
{
	dest->length = src.length();
	if (dest->length > 0) {
		if ((dest->sequence = static_cast<char*>( malloc( dest->length))) == 0) {
			throw Exception( Error::NO_MEMORY);
		}
		memcpy( dest->sequence, src.data(), dest->length);
	} else {
		dest->sequence = 0;
	}
}

std::string TACO::convertToString( DevType type, const void* input) throw (TACO::Exception)
{
	std::stringstream r;
	switch (type) {
	case D_BOOLEAN_TYPE:
		r << static_cast<bool>( *static_cast<const DevBoolean*>( input));
		break;
	case D_SHORT_TYPE:
		r << *static_cast<const DevShort*>( input);
		break;
	case D_USHORT_TYPE:
		r << *static_cast<const DevUShort*>( input);
		break;
	case D_LONG_TYPE:
		r << *static_cast<const DevLong*>( input);
		break;
	case D_ULONG_TYPE:
		r << *static_cast<const DevULong*>( input);
		break;
	case D_FLOAT_TYPE:
		r << *static_cast<const DevFloat*>( input);
		break;
	case D_DOUBLE_TYPE:
		r << *static_cast<const DevDouble*>( input);
		break;
	case D_STRING_TYPE:
		r.str( *static_cast<const DevString*>( input));
		break;
	case D_VAR_CHARARR:
		r.str( std::string(
			static_cast<const DevVarCharArray*>( input)->sequence,
			static_cast<const DevVarCharArray*>( input)->sequence
				+ static_cast<const DevVarCharArray*>( input)->length
		));
		break;
	case D_VAR_SHORTARR:
		copyDevVarArray( static_cast<const DevVarShortArray*>( input), r);
		break;
	case D_VAR_USHORTARR:
		copyDevVarArray( static_cast<const DevVarUShortArray*>( input), r);
		break;
	case D_VAR_LONGARR:
		copyDevVarArray( static_cast<const DevVarLongArray*>( input), r);
		break;
	case D_VAR_ULONGARR:
		copyDevVarArray( static_cast<const DevVarULongArray*>( input), r);
		break;
	case D_VAR_FLOATARR:
		copyDevVarArray( static_cast<const DevVarFloatArray*>( input), r);
		break;
	case D_VAR_DOUBLEARR:
		copyDevVarArray( static_cast<const DevVarDoubleArray*>( input), r);
		break;
	default:
		throw Exception( Error::TYPE_ERROR, "type not supported");
	}
	return r.str();
}

void* TACO::convertToDevArgument( DevType type, const std::string& input) throw (TACO::Exception)
{
	void* r = allocateDevArgument( type);
	std::stringstream s( input);
	switch (type) {
	case D_BOOLEAN_TYPE: {
		bool tmp;
		s >> tmp;
		*static_cast<DevBoolean*>( r) = tmp;
		break;
	}
	case D_SHORT_TYPE:
		s >> *static_cast<DevShort*>( r);
		break;
	case D_USHORT_TYPE:
		s >> *static_cast<DevUShort*>( r);
		break;
	case D_LONG_TYPE:
		s >> *static_cast<DevLong*>( r);
		break;
	case D_ULONG_TYPE:
		s >> *static_cast<DevULong*>( r);
		break;
	case D_FLOAT_TYPE:
		s >> *static_cast<DevFloat*>( r);
		break;
	case D_DOUBLE_TYPE:
		s >> *static_cast<DevDouble*>( r);
		break;
	case D_STRING_TYPE:
		copy( input, static_cast<DevString*>( r));
		break;
	case D_VAR_CHARARR:
		copy( input, static_cast<DevVarCharArray*>( r));
		break;
	case D_VAR_SHORTARR:
		copyToDevVarArray( s, static_cast<DevVarShortArray*>( r));
		break;
	case D_VAR_USHORTARR:
		copyToDevVarArray( s, static_cast<DevVarUShortArray*>( r));
		break;
	case D_VAR_LONGARR:
		copyToDevVarArray( s, static_cast<DevVarLongArray*>( r));
		break;
	case D_VAR_ULONGARR:
		copyToDevVarArray( s, static_cast<DevVarULongArray*>( r));
		break;
	case D_VAR_FLOATARR:
		copyToDevVarArray( s, static_cast<DevVarFloatArray*>( r));
		break;
	case D_VAR_DOUBLEARR:
		copyToDevVarArray( s, static_cast<DevVarDoubleArray*>( r));
		break;
	default:
		throw Exception( Error::TYPE_ERROR, "type not supported");
	}
	if (!s) {
		throw Exception( Error::TYPE_ERROR, "cannot convert argument");
	}
	return r;
}
