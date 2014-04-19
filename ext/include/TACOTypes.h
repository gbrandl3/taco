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

#ifndef TACO_TYPES_H
#define TACO_TYPES_H

#include <API.h>

namespace TACO {
	// The constructor of the unspecialized template is private
	// and has no type() method in order to prevent the usage
	// of unsupported types.  This means if you get a compiler error
	// like '`TACO::Type<UNSUPPORTED_TYPE>::Type()' is private' or
	// 'no method `TACO::Type<UNSUPPORTED_TYPE>::type'' the type is
	// currently not supported.
	template<typename T> class Type {
	private:
		Type();
	};

	template <> class Type<DevVoid> {
	public:
		static DevType type()
		{
			return D_VOID_TYPE;
		}
	};
	
	template <> class Type<DevBoolean> {
	public:
		static DevType type()
		{
			return D_BOOLEAN_TYPE;
		}
	};

	template <> class Type<DevShort> {
	public:
		static DevType type()
		{
			return D_SHORT_TYPE;
		}
	};

	template <> class Type<DevUShort> {
	public:
		static DevType type()
		{
			return D_USHORT_TYPE;
		}
	};

	template <> class Type<DevLong> {
	public:
		static DevType type()
		{
			return D_LONG_TYPE;
		}
	};

	template <> class Type<DevULong> {
	public:
		static DevType type()
		{
			return D_ULONG_TYPE;
		}
	};

	template <> class Type<DevFloat> {
	public:
		static DevType type()
		{
			return D_FLOAT_TYPE;
		}
	};

	template <> class Type<DevDouble> {
	public:
		static DevType type()
		{
			return D_DOUBLE_TYPE;
		}
	};

	template <> class Type<DevString> {
	public:
		static DevType type()
		{
			return D_STRING_TYPE;
		}
	};

	template <> class Type<DevVarCharArray> {
	public:
		static DevType type()
		{
			return D_VAR_CHARARR;
		}

		typedef char ValueType;
	};

	template <> class Type<DevVarStringArray> {
	public:
		static DevType type()
		{
			return D_VAR_STRINGARR;
		}

		typedef DevString ValueType;
	};

	template <> class Type<DevVarShortArray> {
	public:
		static DevType type()
		{
			return D_VAR_SHORTARR;
		}

		typedef DevShort ValueType;
	};

	template <> class Type<DevVarUShortArray> {
	public:
		static DevType type()
		{
			return D_VAR_USHORTARR;
		}

		typedef DevUShort ValueType;
	};

	template <> class Type<DevVarLongArray> {
	public:
		static DevType type()
		{
			return D_VAR_LONGARR;
		}

		typedef DevLong ValueType;
	};

	template <> class Type<DevVarULongArray> {
	public:
		static DevType type()
		{
			return D_VAR_ULONGARR;
		}

		typedef DevULong ValueType;
	};

	template <> class Type<DevVarFloatArray> {
	public:
		static DevType type()
		{
			return D_VAR_FLOATARR;
		}

		typedef DevFloat ValueType;
	};

	template <> class Type<DevVarDoubleArray> {
	public:
		static DevType type()
		{
			return D_VAR_DOUBLEARR;
		}

		typedef DevDouble ValueType;
	};

	template<typename T> DevType type()
	{
		return Type<T>::type();
	}
}

#endif // TACO_TYPES_H
