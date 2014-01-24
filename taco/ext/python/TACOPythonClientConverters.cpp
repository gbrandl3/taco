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

#include <iostream>
#if HAVE_SIGNAL_H
#	include <signal.h>
#endif
#include <float.h>
#include <limits.h>

#include <TACOExtensions.h>

#ifdef HAVE_FSTAT
#undef HAVE_FSTAT
#endif
#include <TACOPythonClientConverters.h>

#if PY_MAJOR_VERSION < 3
#	define PYSTRING_FROMSTRING 		PyString_FromString
#	define PYINT_FROMLONG			PyInt_FromLong
#	define PYSTRING_FROMSTRINGANDSIZE	PyString_FromStringAndSize
#	define PYSTRING_ASSTRING		PyString_AsString
#	define PYSTRING_CHECK			PyString_Check
#	define PYSTRING_SIZE			PyString_Size
#else
#	define PYINT_FROMLONG			PyLong_FromLong
#	define PYSTRING_FROMSTRING 		PyBytes_FromString
#	define PYSTRING_FROMSTRINGANDSIZE	PyBytes_FromStringAndSize
#	define PYSTRING_ASSTRING		PyBytes_AsString
#	define PYSTRING_CHECK			PyBytes_Check
#	define PYSTRING_SIZE			PyBytes_Size
#endif

bool TACOPythonClient::createDevArgument( DevType type, DevArgument& arg) throw (::TACO::Exception)
{
	if (type != D_VOID_TYPE) {
		DevDataListEntry data;
		DevLong e;
		if (xdr_get_type( type, &data, &e) != DS_OK) {
			arg = 0;
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "cannot get XDR type: " + TACO::errorString( e));
		}
		arg = static_cast<DevArgument>( malloc( static_cast<size_t>( data.size)));
		if (arg == NULL) {
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "cannot allocate argument");
			return false;
		}
		memset( arg, 0, static_cast<size_t>( data.size));
	} else {
		arg = NULL;
	}
	return true;
}

bool TACOPythonClient::convertToDevArgument( PyObject* input, DevType inputType, DevArgument& argin) throw (::TACO::Exception)
{
	if (inputType != D_VOID_TYPE) {
		if (input != NULL) {
			// Convert PyObject to DevArgument
			switch (inputType) {
			case D_BOOLEAN_TYPE:
				argin = convertToDevBoolean( input);
				break;
			case D_SHORT_TYPE:
				argin = convertToDevShort( input);
				break;
			case D_USHORT_TYPE:
				argin = convertToDevUShort( input);
				break;
			case D_LONG_TYPE:
				argin = convertToDevLong( input);
				break;
			case D_ULONG_TYPE:
				argin = convertToDevULong( input);
				break;
			case D_FLOAT_TYPE:
				argin = convertToDevFloat( input);
				break;
			case D_DOUBLE_TYPE:
				argin = convertToDevDouble( input);
				break;
			case D_STRING_TYPE:
				argin = convertToDevString( input);
				break;
			case D_VAR_CHARARR:
				argin = convertToDevVarCharArray( input);
				break;
			case D_VAR_STRINGARR:
				argin = convertToDevVarStringArray( input);
				break;
			case D_VAR_SHORTARR:
				argin = convertToDevVarShortArray( input);
				break;
			case D_VAR_USHORTARR:
				argin = convertToDevVarUShortArray( input);
				break;
			case D_VAR_LONGARR:
				argin = convertToDevVarLongArray( input);
				break;
			case D_VAR_ULONGARR:
				argin = convertToDevVarULongArray( input);
				break;
			case D_VAR_FLOATARR:
				argin = convertToDevVarFloatArray( input);
				break;
			case D_VAR_DOUBLEARR:
				argin = convertToDevVarDoubleArray( input);
				break;
			default:
				throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "unsupported or invalid input type");
			}
		} else {
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "missing input argument");
		}
		if (argin == NULL) {
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "cannot convert input argument");
		}
	} else {
		argin = NULL;
	}
	return true;
}

bool TACOPythonClient::convertToDevBoolean( PyObject* in, DevBoolean& value)
{
	DevLong tmp;
	if (convertToDevLong( in, tmp)) {
		value = (tmp == 0L) ? 0 : 1;
		return true;
	} else {
		return false;
	}
}

bool TACOPythonClient::convertToDevShort( PyObject* in, DevShort& value)
{
	DevLong tmpLong;
	if (convertToDevLong( in, tmpLong))
	{
#if 0
		if (tmpLong < -32768 || tmpLong > 32767)
			return false;
#endif
		DevShort tmp = tmpLong;
		if (SHRT_MIN <= tmp && tmp <= SHRT_MAX) {
			value = static_cast<DevShort>( tmp);
			return true;
		}
	}
	return false;
}

bool TACOPythonClient::convertToDevUShort( PyObject* in, DevUShort& value)
{
	DevULong tmp;
	if (convertToDevULong( in, tmp))
	{ 
		tmp &= 0xFFFF;
		if (0L <= tmp && tmp <= USHRT_MAX) {
			value = static_cast<DevUShort>( tmp);
			return true;
		}
	}
	return false;
}

bool TACOPythonClient::convertToDevLong( PyObject* in, DevLong& value)
{
	if (PyLong_Check( in) != 0) {
		value = PyLong_AsLong( in);
#if PY_MAJOR_VERSION < 3
	} else if (PyInt_Check( in) != 0) {
		value = PyInt_AsLong( in);
#endif
	} else if (PyFloat_Check( in) != 0) {
		double tmp = PyFloat_AsDouble( in);
		if (LONG_MIN <= tmp && tmp <= LONG_MAX) {
			value = static_cast<DevLong>( tmp);
		} else {
			return false;
		}
	} else if (PyTuple_Check( in) != 0 && PyTuple_GET_SIZE( in) == 1) {
		return convertToDevLong( PyTuple_GET_ITEM( in, 0), value);
	} else if (PyList_Check( in) != 0 && PyList_GET_SIZE( in) == 1) {
		return convertToDevLong( PyList_GET_ITEM( in, 0), value);
	} else {
		return false;
	}
	return true;
}

bool TACOPythonClient::convertToDevULong( PyObject* in, DevULong& value)
{
	if (PyLong_Check( in) != 0) {
		value = PyLong_AsUnsignedLong(in);
#if PY_MAJOR_VERSION < 3
	} else if (PyInt_Check(in)) {
		value = PyInt_AsLong(in);
		return true;
#endif
	} else if (PyFloat_Check( in) != 0) {
		double tmp = PyFloat_AsDouble( in);
		if (0 <= tmp && tmp <= ULONG_MAX) {
			value = static_cast<DevULong>( tmp);
		} else {
			return false;
		}
	} else if (PyTuple_Check( in) != 0 && PyTuple_GET_SIZE( in) == 1) {
		return convertToDevULong( PyTuple_GET_ITEM( in, 0), value);
	} else if (PyList_Check( in) != 0 && PyList_GET_SIZE( in) == 1) {
		return convertToDevULong( PyList_GET_ITEM( in, 0), value);
	} else {
		return false;
	}
	return true;
}

bool TACOPythonClient::convertToDevFloat( PyObject* in, DevFloat& value)
{
	DevDouble tmp;
	if (convertToDevDouble( in, tmp) && -FLT_MAX <= tmp && tmp <= FLT_MAX) {
		value = static_cast<DevFloat>( tmp);
		return true;
	}
	return false;
}

bool TACOPythonClient::convertToDevDouble( PyObject* in, DevDouble& value)
{
	if (PyFloat_Check( in) != 0) {
		value = PyFloat_AsDouble( in);
#if PY_MAJOR_VERSION < 3
	} else if (PyInt_Check( in) != 0) {
		value = static_cast<DevDouble>( PyInt_AsLong( in));
#endif
	} else if (PyLong_Check( in) != 0) {
		value = PyLong_AsDouble( in);
	} else if (PyTuple_Check( in) != 0 && PyTuple_GET_SIZE( in) == 1) {
		return convertToDevDouble( PyTuple_GET_ITEM( in, 0), value);
	} else if (PyList_Check( in) != 0 && PyList_GET_SIZE( in) == 1) {
		return convertToDevDouble( PyList_GET_ITEM( in, 0), value);
	} else {
		return false;
	}
	return true;
}

DevArgument TACOPythonClient::convertToDevString( PyObject* in)
{
	DevArgument r;
	if (createDevArgument( D_STRING_TYPE, r)) {
		if (PYSTRING_CHECK( in) != 0) {
			*static_cast<DevString*>( r) = PYSTRING_ASSTRING( in);
		} else {
			free( r);
			return NULL;
		}
	}
	return r;
}

DevArgument TACOPythonClient::convertToDevVarCharArray( PyObject* in)
{
	DevArgument r;
	if (createDevArgument( D_VAR_CHARARR, r)) {
		if (PYSTRING_CHECK( in) != 0) {
			static_cast<DevVarCharArray*>( r)->sequence = PYSTRING_ASSTRING( in);
			static_cast<DevVarCharArray*>( r)->length = PYSTRING_SIZE( in);
		} else {
			free( r);
			return NULL;
		}
	}
	return r;
}

DevArgument TACOPythonClient::convertToDevVarStringArray( PyObject* in)
{
	unsigned int size;
	PyObject* (*getItem)( PyObject*, Py_ssize_t);
	if (PyTuple_Check( in) != 0) {
		size = static_cast<unsigned int>( PyTuple_Size( in));
		getItem = &PyTuple_GetItem;
	} else if (PyList_Check( in) != 0) {
		size = static_cast<unsigned int>( PyList_Size( in));
		getItem = &PyList_GetItem;
	} else {
		return NULL;
	}
	DevArgument r;
	if (createDevArgument( D_VAR_STRINGARR, r)) {
		static_cast<DevVarStringArray*>( r)->sequence = static_cast<DevString*>( malloc( sizeof( DevString) * size));
		if (static_cast<DevVarStringArray*>( r)->sequence != NULL) {
			for (unsigned int i = 0; i < size; ++i) {
				PyObject* tmp = (*getItem)( in, i);
				if (tmp != NULL && PYSTRING_CHECK( tmp) != 0) {
					static_cast<DevVarStringArray*>( r)->sequence [i] = PYSTRING_ASSTRING( tmp);
				} else {
					free( static_cast<DevVarStringArray*>( r)->sequence);
					free( r);
					return NULL;
				}
			}
		} else {
			free( r);
			return NULL;
		}
		static_cast<DevVarStringArray*>( r)->length = size;
	}
	return r;
}

bool TACOPythonClient::convertToPyObject( DevType outputType, DevArgument argout, PyObject** output) throw (::TACO::Exception)
{
	if (outputType == D_VOID_TYPE) {
		Py_INCREF( Py_None);
		*output = Py_None;
	} else if (argout != NULL) {
		switch (outputType) {
		case D_BOOLEAN_TYPE:
			*output = PYINT_FROMLONG( static_cast<long>( *static_cast<DevBoolean*>( argout)));
			break;
		case D_SHORT_TYPE:
			*output = PYINT_FROMLONG( static_cast<long>( *static_cast<DevShort*>( argout)));
			break;
		case D_USHORT_TYPE:
			*output = PYINT_FROMLONG( static_cast<long>( *static_cast<DevUShort*>( argout)));
			break;
		case D_LONG_TYPE:
			*output = PyLong_FromLong( *static_cast<DevLong*>( argout));
			break;
		case D_ULONG_TYPE:
			*output = PyLong_FromUnsignedLong( *static_cast<DevULong*>( argout));
			break;
		case D_FLOAT_TYPE:
			*output = PyFloat_FromDouble( static_cast<double>( *static_cast<DevFloat*>( argout)));
			break;
		case D_DOUBLE_TYPE:
			*output = PyFloat_FromDouble( *static_cast<DevDouble*>( argout));
			break;
		case D_STRING_TYPE:
			*output = PYSTRING_FROMSTRING( *static_cast<DevString*>( argout));
			break;
		case D_VAR_CHARARR:
			*output = PYSTRING_FROMSTRINGANDSIZE(
				static_cast<DevVarCharArray*>( argout)->sequence,
				static_cast<DevVarCharArray*>( argout)->length
			);
			break;
		case D_VAR_STRINGARR:
			*output = convertToPyTuple( static_cast<DevVarStringArray*>( argout));
			break;
		case D_VAR_SHORTARR:
			*output = convertToPyTuple( static_cast<DevVarShortArray*>( argout));
			break;
		case D_VAR_USHORTARR:
			*output = convertToPyTuple( static_cast<DevVarUShortArray*>( argout));
			break;
		case D_VAR_LONGARR:
			*output = convertToPyTuple( static_cast<DevVarLongArray*>( argout));
			break;
		case D_VAR_ULONGARR:
			*output = convertToPyTuple( static_cast<DevVarULongArray*>( argout));
			break;
		case D_VAR_FLOATARR:
			*output = convertToPyTuple( static_cast<DevVarFloatArray*>( argout));
			break;
		case D_VAR_DOUBLEARR:
			*output = convertToPyTuple( static_cast<DevVarDoubleArray*>( argout));
			break;
		default:
			throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "unsupported output type");
		}
	} else {
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, "received invalid output argument");
	}
	return true;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarStringArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
		PyObject* tmp = PYSTRING_FROMSTRING( array->sequence [i]);
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarShortArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
#if PY_MAJOR_VERSION < 3
		PyObject* tmp = PyInt_FromLong( static_cast<long>( array->sequence [i]));
#else
		PyObject* tmp = PyLong_FromLong( static_cast<long>( array->sequence [i]));
#endif
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarUShortArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
#if PY_MAJOR_VERSION < 3
		PyObject* tmp = PyInt_FromLong( static_cast<long>( array->sequence [i]));
#else
		PyObject* tmp = PyLong_FromLong( static_cast<long>( array->sequence [i]));
#endif
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarLongArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
		PyObject* tmp = PyLong_FromLong( array->sequence [i]);
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarULongArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
		PyObject* tmp = PyLong_FromUnsignedLong( array->sequence [i]);
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarFloatArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
		PyObject* tmp = PyFloat_FromDouble( static_cast<double>( array->sequence [i]));
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyTuple( DevVarDoubleArray* array)
{
	PyObject* r = PyTuple_New( array->length);
	if (r == NULL) {
		return NULL;
	}
	for (unsigned int i = 0; i < array->length; ++i) {
		PyObject* tmp = PyFloat_FromDouble( array->sequence [i]);
		if (tmp == NULL) {
			Py_DECREF( r);
			return NULL;
		}
		PyTuple_SET_ITEM( r, i, tmp);
	}
	return r;
}

PyObject* TACOPythonClient::convertToPyString( DevType inputType)
{
	switch (inputType)
	{
		case D_BOOLEAN_TYPE:
			return PYSTRING_FROMSTRING("D_BOOLEAN_TYPE");
		case D_SHORT_TYPE:
			return PYSTRING_FROMSTRING("D_SHORT_TYPE");
		case D_USHORT_TYPE:
			return PYSTRING_FROMSTRING("D_USHORT_TYPE");
		case D_LONG_TYPE:
			return PYSTRING_FROMSTRING("D_LONG_TYPE");
		case D_ULONG_TYPE:
			return PYSTRING_FROMSTRING("D_ULONG_TYPE");
		case D_FLOAT_TYPE:
			return PYSTRING_FROMSTRING("D_FLOAT_TYPE");
		case D_DOUBLE_TYPE:
			return PYSTRING_FROMSTRING("D_DOUBLE_TYPE");
		case D_STRING_TYPE:
			return PYSTRING_FROMSTRING("D_STRING_TYPE");
		case D_VAR_CHARARR:
			return PYSTRING_FROMSTRING("D_VAR_CHARARR");
		case D_VAR_STRINGARR:
			return PYSTRING_FROMSTRING("D_VAR_STRINGARR");
		case D_VAR_SHORTARR:
			return PYSTRING_FROMSTRING("D_VAR_SHORTARR");
		case D_VAR_USHORTARR:
			return PYSTRING_FROMSTRING("D_VAR_USHORTARR");
		case D_VAR_LONGARR:
			return PYSTRING_FROMSTRING("D_VAR_LONGARR");
		case D_VAR_ULONGARR:
			return PYSTRING_FROMSTRING("D_VAR_ULONGARR");
		case D_VAR_FLOATARR:
			return PYSTRING_FROMSTRING("D_VAR_FLOATARR");
		case D_VAR_DOUBLEARR:
			return PYSTRING_FROMSTRING("D_VAR_DOUBLEARR");
		default:
			return PYSTRING_FROMSTRING("not supported type");
	}
}

void TACOPythonClient::freeInputArgument( DevType inputType, DevArgument argin)
{
	if (inputType != D_VOID_TYPE && argin != NULL) {
		switch (inputType) {
		case D_BOOLEAN_TYPE:
		case D_SHORT_TYPE:
		case D_USHORT_TYPE:
		case D_LONG_TYPE:
		case D_ULONG_TYPE:
		case D_FLOAT_TYPE:
		case D_DOUBLE_TYPE:
		case D_STRING_TYPE:
		case D_VAR_CHARARR:
			freeDevArgument( argin);
			break;
		case D_VAR_STRINGARR:
			freeDevVarArray<DevVarStringArray>( argin);
			break;
		case D_VAR_SHORTARR:
			freeDevVarArray<DevVarShortArray>( argin);
			break;
		case D_VAR_USHORTARR:
			freeDevVarArray<DevVarUShortArray>( argin);
			break;
		case D_VAR_LONGARR:
			freeDevVarArray<DevVarLongArray>( argin);
			break;
		case D_VAR_ULONGARR:
			freeDevVarArray<DevVarULongArray>( argin);
			break;
		case D_VAR_FLOATARR:
			freeDevVarArray<DevVarFloatArray>( argin);
			break;
		case D_VAR_DOUBLEARR:
			freeDevVarArray<DevVarDoubleArray>( argin);
			break;
		default:
			std::cerr << "ERROR: fatal internal error: free input argument" << std::endl;
			raise( SIGKILL);
		}
	}
}

void TACOPythonClient::freeOutputArgument( long outputType, DevArgument argout)
{
	if (outputType != D_VOID_TYPE && argout != NULL) {
		DevLong e;
		dev_xdrfree( outputType, argout, &e);
		switch (outputType) {
		case D_BOOLEAN_TYPE:
		case D_SHORT_TYPE:
		case D_USHORT_TYPE:
		case D_LONG_TYPE:
		case D_ULONG_TYPE:
		case D_FLOAT_TYPE:
		case D_DOUBLE_TYPE:
		case D_STRING_TYPE:
			freeDevArgument( argout);
			break;
		case D_VAR_CHARARR:
//			freeDevVarArray<DevVarCharArray>( argout);
			break;
		case D_VAR_STRINGARR:
			freeDevVarArray<DevVarStringArray>( argout);
			break;
		case D_VAR_SHORTARR:
			freeDevVarArray<DevVarShortArray>( argout);
			break;
		case D_VAR_USHORTARR:
			freeDevVarArray<DevVarUShortArray>( argout);
			break;
		case D_VAR_LONGARR:
			freeDevVarArray<DevVarLongArray>( argout);
			break;
		case D_VAR_ULONGARR:
			freeDevVarArray<DevVarULongArray>( argout);
			break;
		case D_VAR_FLOATARR:
			freeDevVarArray<DevVarFloatArray>( argout);
			break;
		case D_VAR_DOUBLEARR:
			freeDevVarArray<DevVarDoubleArray>( argout);
			break;
		default:
			std::cerr << "ERROR: fatal internal error: free output argument" << std::endl;
			raise( SIGKILL);
		}
	}
}
