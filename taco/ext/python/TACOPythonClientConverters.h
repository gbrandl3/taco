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

#ifndef TACO_PYTHON_CLIENT_CONVERTERS_H
#define TACO_PYTHON_CLIENT_CONVERTERS_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdlib.h>

#include <Python.h>

#ifndef HAVE_PY_SSIZE_T
typedef int Py_ssize_t;
#endif

#include <API.h>
#include <TACOTypes.h>

namespace TACOPythonClient {
	bool createDevArgument( DevType type, DevArgument& arg);

	bool convertToDevArgument( PyObject* input, DevType inputType, DevArgument& argin);

	bool convertToDevLong( PyObject* input, DevLong& value);

	bool convertToDevDouble( PyObject* input, DevDouble& value);

	bool convertToDevBoolean( PyObject* input, DevBoolean& value);

	bool convertToDevShort( PyObject* input, DevShort& value);

	bool convertToDevUShort( PyObject* input, DevUShort& value);

	bool convertToDevULong( PyObject* input, DevULong& value);

	bool convertToDevFloat( PyObject* input, DevFloat& value);

	template<typename SCALAR> DevArgument convertToDevScalar( PyObject* input, bool (*convert)( PyObject*, SCALAR&))
	{
		DevArgument r;
		if (createDevArgument( TACO::type<SCALAR>(), r)) {
			if (!(*convert)( input, *static_cast<SCALAR*>( r))) {
				free( r);
				return 0;
			}
		}
		return r;
	}

	inline DevArgument convertToDevBoolean( PyObject* input)
	{
		return convertToDevScalar<DevBoolean>( input, &convertToDevBoolean);
	}

	inline DevArgument convertToDevShort( PyObject* input)
	{
		return convertToDevScalar<DevShort>( input, &convertToDevShort);
	}

	inline DevArgument convertToDevUShort( PyObject* input)
	{
		return convertToDevScalar<DevUShort>( input, &convertToDevUShort);
	}

	inline DevArgument convertToDevLong( PyObject* input)
	{
		return convertToDevScalar<DevLong>( input, &convertToDevLong);
	}

	inline DevArgument convertToDevULong( PyObject* input)
	{
		return convertToDevScalar<DevULong>( input, &convertToDevULong);
	}

	inline DevArgument convertToDevFloat( PyObject* input)
	{
		return convertToDevScalar<DevFloat>( input, &convertToDevFloat);
	}

	inline DevArgument convertToDevDouble( PyObject* input)
	{
		return convertToDevScalar<DevDouble>( input, &convertToDevDouble);
	}

	DevArgument convertToDevString( PyObject* input);

	DevArgument convertToDevVarCharArray( PyObject* input);

	DevArgument convertToDevVarStringArray( PyObject* input);

	template<typename ARRAY, typename SCALAR> DevArgument convertToDevVarArray(
		PyObject* input,
		bool (*convert)( PyObject*, SCALAR&)
	)
	{
		unsigned int size;
		PyObject* (*getItem)( PyObject*, Py_ssize_t);
		if (PyTuple_Check( input) != 0) {
			size = static_cast<unsigned int>( PyTuple_Size( input));
			getItem = &PyTuple_GetItem;
		} else if (PyList_Check( input) != 0) {
			size = static_cast<unsigned int>( PyList_Size( input));
			getItem = &PyList_GetItem;
		} else {
			return 0;
		}
		DevArgument r;
		if (createDevArgument( TACO::type<ARRAY>(), r)) {
			static_cast<ARRAY*>( r)->sequence = static_cast<SCALAR*>( malloc( sizeof( SCALAR) * size));
			if (static_cast<ARRAY*>( r)->sequence != 0) {
				for (unsigned int i = 0; i < size; ++i) {
					PyObject* tmp = (*getItem)( input, i);
					SCALAR value;
					if (tmp != 0 && (*convert)( tmp, value)) {
						static_cast<ARRAY*>( r)->sequence [i] = value;
					} else {
						free( static_cast<ARRAY*>( r)->sequence);
						free( r);
						return 0;
					}
				}
			} else {
				free( r);
				return 0;
			}
			static_cast<ARRAY*>( r)->length = size;
		}
		return r;
	}

	inline DevArgument convertToDevVarShortArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarShortArray, DevShort>( input, &convertToDevShort);
	}

	inline DevArgument convertToDevVarUShortArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarUShortArray, DevUShort>( input, &convertToDevUShort);
	}

	inline DevArgument convertToDevVarLongArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarLongArray, DevLong>( input, &convertToDevLong);
	}

	inline DevArgument convertToDevVarULongArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarULongArray, DevULong>( input, &convertToDevULong);
	}

	inline DevArgument convertToDevVarFloatArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarFloatArray, DevFloat>( input, &convertToDevFloat);
	}

	inline DevArgument convertToDevVarDoubleArray( PyObject* input)
	{
		return convertToDevVarArray<DevVarDoubleArray, DevDouble>( input, &convertToDevDouble);
	}

	bool convertToPyObject( DevType outputType, DevArgument argout, PyObject** output);

	PyObject* convertToPyTuple( DevVarStringArray* array);

	PyObject* convertToPyTuple( DevVarShortArray* array);

	PyObject* convertToPyTuple( DevVarUShortArray* array);

	PyObject* convertToPyTuple( DevVarLongArray* array);

	PyObject* convertToPyTuple( DevVarULongArray* array);

	PyObject* convertToPyTuple( DevVarFloatArray* array);

	PyObject* convertToPyTuple( DevVarDoubleArray* array);

	inline void freeDevArgument( DevArgument arg)
	{
		free( arg);
	}

	template<typename T> void freeDevVarArray( DevArgument arg)
	{
		if (arg != 0) {
			free( static_cast<T*>( arg)->sequence);
			freeDevArgument( arg);
		}
	}

	void freeInputArgument( long inputType, DevArgument argin);

	void freeOutputArgument( long outputType, DevArgument argout);

	inline void freeArguments( long inputType, void* argin, long outputType, void* argout)
	{
		freeInputArgument( inputType, argin);
		freeOutputArgument( outputType, argout);
	}
}

#endif // TACO_PYTHON_CLIENT_CONVERTERS_H
