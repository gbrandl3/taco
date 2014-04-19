/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2013 Sebastian Huber <sebastian-huber@web.de>
 * Copyright (C) 2014 Jens Krüger <jens.krueger@frm2.tum.de>
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

%define DOCSTRING
"This module provides the access to the TACO system"
%enddef

%module(docstring = DOCSTRING) TACO

%feature("autodoc", "1");
%naturalvar;
%include <exception.i>
%include <std_string.i>
%include <std_map.i>
#ifdef SWIGPYTHON
/* not implemented for php yet */
%include <std_set.i>
#endif /*SWIGPYTHON*/
%include <TACOException.h>
%header %{
#include <log4cpp/Category.hh>
// Python.h defines OVERFLOW
#undef OVERFLOW

#include <TACOExtensions.h>

/*
#include <TACOPythonClientConverters.h>
*/

using namespace TACO;

%}

%runtime %{
// RUNTIME SECTION
%}

%wrapper %{
// WRAPPER SECTION
%}

%init %{
#ifdef SWIGPYTHON
        TACOError = PyErr_NewException(const_cast<char *>("TACO.TACOError"), NULL, NULL);
        Py_INCREF(TACOError);
        PyModule_AddObject(m, "TACOError", TACOError);
#endif /*SWIGPYTHON*/
%}

%typemap(throws) ::TACO::Exception %{
#ifdef SWIGPYTHON
        SWIG_PYTHON_THREAD_BEGIN_BLOCK;
        PyErr_SetString(TACOError, $1.what());
        PyObject_SetAttr(TACOError, PyString_FromString("errcode"), PyInt_FromLong($1));
        SWIG_PYTHON_THREAD_END_BLOCK;
        SWIG_fail;
#endif /*SWIGPYTHON*/
%}

#ifdef SWIGPYTHON
%pythoncode %{
        TACOError = _TACO.TACOError
%}
#endif /*SWIGPYTHON*/


%{
#ifdef SWIGPYTHON
        static PyObject         *TACOError;
#endif /*SWIGPYTHON*/
        log4cpp::Category       *logStream = NULL;
%}

%include <TACOExtensions.h>

// additional docstrings need to come before pasing by autodoc
%feature("docstring", "path - name of the requested resource(may include the nethost)\n" \
"Returns the value of the resource 'name' as a string.") queryResource;
%feature("docstring", \
"path - path of the resource (may include the nethost)\n" \
"value -n new value of the resource as string\n" \
"Updates the resource 'resName' with value in the database.\n") updateResource;
%feature("docstring", "path - path to the resource (may include the nethost)\n" \
"Deletes a resource in the database") deleteResource;

// %include <TACOExtensions.h>

namespace std {
}

#ifdef SWIGPYTHON
/*
%rename(queryResource) TACO::queryResource(long commandNumber , PyObject *optargs = NULL);
%varargs(PyObject *optargs = NULL) TACO::TACOClient_execute(long commandNumber, PyObject *optargs = NULL);
*/
#endif /*SWIGPYTHON*/

%define EXECUTE_DOC
"
Multipurpose method for direct command execution.\n
commandNumber - the command number of the desired command.\n
optargs - (optional) should contain the input data if the command expects input.\n
To provide input to command with no input has no effect.\n\n
Returns - the ouput of the command."
%enddef
%feature("docstring", EXECUTE_DOC) TACOClient_execute;

/*
%thread TACO::Client;
*/

%extend TACO {
#ifdef SWIGPYTHON
PyObject *TACO_queryResource(long commandNumber, PyObject *optargs=NULL)
{
#if 0
        DevArgType      inputType,
                        outputType;
        PyObject        *input = NULL;

        input = optargs;
        try
        {
                TACO::CommandInfoMap map = self->deviceQueryCommandInfo();
                TACO::CommandInfoMap::iterator it = map.find(commandNumber);
                if (it == map.end())
                {
                        PyErr_SetString(PyExc_ValueError, "command not found in the device command list");
                        SWIG_fail;
                }
                inputType = it->second.inputType;
                outputType = it->second.outputType;
        }
        catch (const TACO::Exception &e)
        {
                SWIG_PYTHON_THREAD_BEGIN_BLOCK;
                PyErr_SetString(TACOError, e.what());
                PyObject_SetAttr(TACOError, PyString_FromString("errcode"), PyInt_FromLong(e));
                SWIG_PYTHON_THREAD_END_BLOCK;
                SWIG_fail;
        }

        DevArgument     argin,
                        argout;

        if (!TACOPythonClient::convertToDevArgument(input, inputType, argin))
        {
                PyErr_SetString(PyExc_RuntimeError, "could not convert input argument");
                SWIG_fail;
        }
        if (!TACOPythonClient::createDevArgument(outputType, argout))
        {
                PyErr_SetString(PyExc_RuntimeError, "could not convert output argument");
                SWIG_fail;
        }

        try
        {
                SWIG_PYTHON_THREAD_BEGIN_BLOCK;
                self->execute(commandNumber, argin, inputType, argout, outputType);
                SWIG_PYTHON_THREAD_END_BLOCK;
                PyObject *ret;
                if (!TACOPythonClient::convertToPyObject(outputType, argout, &ret))
                                SWIG_fail;
//              free(argout);
                TACOPythonClient::freeInputArgument(    inputType,argin);
                TACOPythonClient::freeOutputArgument(   outputType,argout);

                return ret;
        }
        catch (const TACO::Exception &e)
        {
                SWIG_PYTHON_THREAD_BEGIN_BLOCK;
                PyErr_SetString(TACOError, e.what());
                PyObject_SetAttr(TACOError, PyString_FromString("errcode"), PyInt_FromLong(e));
                SWIG_PYTHON_THREAD_END_BLOCK;
                SWIG_fail;
        }
        PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
        SWIG_fail;
#endif
fail:
        return NULL;
}
#endif /*SWIGPYTHON*/
};

