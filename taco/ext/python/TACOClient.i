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

%define DOCSTRING
"This module provides access to the TACO device hierarchy"
%enddef

%module(docstring = DOCSTRING) TACOClient

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
#include <TACOClient.h>

#include <TACOPythonClientConverters.h>

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
        TACOError = PyErr_NewException(const_cast<char *>("TACOClient.TACOError"), NULL, NULL);
        Py_INCREF(TACOError);
        PyModule_AddObject(m, "TACOError", TACOError);
#endif /*SWIGPYTHON*/
%}

%typemap(throws) ::TACO::Exception %{
#ifdef SWIGPYTHON
        PyErr_SetString(TACOError, $1.what()); 
        PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong($1));
        SWIG_fail;
#endif /*SWIGPYTHON*/
%}

%typemap(out) TACO::Client::DeviceTypeSet %{
#ifdef SWIGPYTHON
        $result = PyList_New(0);
        if (!$result)
        {
                PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
                SWIG_fail;
        }
        for (TACO::Client::DeviceTypeSet::iterator it = $1.begin(); it != $1.end(); ++it)
        {
                PyObject *o = PyString_FromString((*it).c_str());
                if (!o)
                {
                        PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
                        SWIG_fail; 
                }
                if (PyList_Append($result, o))
                {
                        PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
                        SWIG_fail; 
                }
                Py_DECREF(o);
        }
#endif /*SWIGPYTHON*/
%}

#ifdef SWIGPYTHON
%pythoncode %{
        TACOError = _TACOClient.TACOError
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
%feature("docstring", "Switches the device from the state 'off' into a working state.") deviceOn;
%feature("docstring", "Switches the device into the state 'off'.") deviceOff;
%feature("docstring", "Resets the device. Actions which are executed depend on the device.") deviceReset;
%feature("docstring", "Returns the current device state as string") deviceStatus;
%feature("docstring", "Returns the current device state as number") deviceState;
%feature("docstring", "Returns the version of the device as string") deviceVersion;
%feature("docstring", "Returns a list of strings describing the exported device types") deviceTypes;
%feature("docstring", "name - name of the requested resource\nReturns the value of the resource 'name' as a string.") deviceQueryResource;
%feature("docstring", \
"name - name of the resource\nvalue -n new value of the resource as string\nUpdates the resource 'name' with value.\n" \
"The value is stored in the server and in the database as well.\n" \
"This command is only available if the device is in state 'off'") deviceUpdateResource; 
%feature("docstring", "Initiate the reread of all device resources from the database This command is only available if the device is in state 'off'") deviceUpdate;
%feature("docstring", "Checks whether the device is in state 'off' or not. Returns 'True' if the device is in state 'off' otherwise 'False'") isDeviceOff;
%feature("docstring", \
"Returns the currently used network protocol.\n" \
"Normally will TCP (888) used. For small amount of transferred data UDP(999) may used as well.\n" \
"Use of UDP is faster than TCP, but TCP is safe.\n") clientNetworkProtocol;
%feature("docstring", \
"protocol - TCP (888) or UDP(999)\n Sets the currently used network protocol\n see also: 'clientNetworkProtocol'") setClientNetworkProtocol;
%feature("docstring", \
"Returns the timeout value (in s) of the network connection\n" \
"This value defines the timeslice for getting an answer from the device.\n" \
"If the device not answers inside this timeslice an exception (TACOError) will be thrown\n" \
"If the standard time (3 s) is to short, you may increase it.\n" \
"It does not increase the communication time.") clientNetworkTimeout;
%feature("docstring", \
"timeout - timeout in seconds\n" \
"Sets the maximum time for the communication between client and server.\n" \
"see also: 'clientNetworkTimeout'") setClientNetworkTimeout;
%feature("docstring", "eventNumber - number of the registered event\nListen to an event. Please do not use this method") listen;
%feature("docstring", \
"name - name of the TACO device
access - access level
connect - automatic connect 1(yes) or 0(no)
Basic TACO client class for the devices at the FRM-II.
This is a base class for TACO client. It provides standard methods which are
supported by every reasonable device. You have also direct access to the device
commands via the 'execute()' method.") Client;
%feature("docstring", "Returns the name of the TACO device") deviceName;

%feature("docstring", \
"name - name of the TACO device
access - access level
Multi purpose call to connect a client to a TACO Server device
In case of no given name the Client tries to connect with last
set parameters given by 'connectClient' or constructor.
If the access level is not given the level WRITE_ACCESS will be
used") connectClient;

%feature("docstring", "Returns whether the client should connect automatically is case of connection lost") isAutoClientConnectionEnabled;
%feature("docstring", \
"enables - true/false for automatic reconnection
Sets the strategy in case of lost connection to the server") enableAutoClientConnection;
%feature("docstring", "disconnects the client from the server") disconnectClient;
%feature("docstring", \
"reconnects the client to the TACO device given by the 
setup of the constructor call or by last connect call") reconnectClient;

%feature("docstring", "returns whether client is connected") isClientConnected;

%feature("docstring", "returns whether client is not connected") isClientDisconnected;

%include <TACOClient.h>

namespace std {
        %template(CommandInfoMap) map< DevCommand,  TACO::CommandInfo>;
        %template(DeviceTypeSet)  set< string > ;
}

#ifdef SWIGPYTHON
%rename(execute) TACO::Client::TACOClient_execute(long commandNumber , PyObject *optargs = NULL);
%varargs(PyObject *optargs = NULL) TACO::Client::TACOClient_execute(long commandNumber, PyObject *optargs = NULL);
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

%extend TACO::Client{
#ifdef SWIGPYTHON
        PyObject *TACOClient_execute(long commandNumber, PyObject *optargs=NULL)
        {
                DevArgType      inputType,
                                outputType;
                PyObject        *input = NULL;

                input = optargs;
                try
                {
                        TACO::CommandInfoMap map= self->deviceQueryCommandInfo();
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
                        PyErr_SetString(TACOError, e.what()); 
                        PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
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
                        self->execute(commandNumber, argin, inputType, argout, outputType);
                        PyObject *ret;
                        if (!TACOPythonClient::convertToPyObject(outputType, argout, &ret))
                                SWIG_fail; 
//                        free(argout);
                        TACOPythonClient::freeInputArgument(    inputType,argin);
                        TACOPythonClient::freeOutputArgument(   outputType,argout);


                        return ret;
                }
                catch (const TACO::Exception &e)
                {
                        PyErr_SetString(TACOError, e.what()); 
                        PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
                        SWIG_fail;
                }
                PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
                SWIG_fail;
        fail:
                return NULL;
        }
#endif /*SWIGPYTHON*/
};

