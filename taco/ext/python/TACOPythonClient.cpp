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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#ifdef HAVE_FSTAT
#undef HAVE_FSTAT
#endif

#include <Python.h>

#ifndef HAVE_PY_SSIZE_T
typedef ssize_t Py_ssize_t;
#endif

#include "TACOClient.h"
#include "TACOPythonClientConverters.h"
#include "TACOBasicCommands.h"

class TACOClient : public TACO::Client
{
public:
	TACOClient( const std::string& name, long access = 0, bool connect = true) 
		: TACO::Client(name, access, connect) 
	{
		printf("name = %s, access = %ld, connect = %d\n", name.c_str(), access, int(connect));
	}
	
	void execute(long cmd, void *argin, long in_type, void *argout, long out_type)
	{
		TACO::Client::execute(cmd, argin, in_type, argout, out_type);
	}
};

// staticforward PyTypeObject TACOClientType;

typedef struct 
{
	PyObject_HEAD
	TACOClient		*m_Client;
	TACO::CommandInfoMap 	*m_CmdInfo; 
} TACOClientObject;

extern "C"
{
static PyObject	*TACOError;

static PyObject *TACOClient_execute(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		long 	commandNumber,
			inputType,
			outputType;
		PyObject *input;
		if (!PyArg_ParseTuple(args, "l|O;Parse error in execute", &commandNumber, &input))
			return NULL;
		
		TACO::CommandInfoMap::iterator it = ((TACOClientObject *)self)->m_CmdInfo->find(commandNumber);
		if (it == ((TACOClientObject *)self)->m_CmdInfo->end())
		{
			PyErr_SetString(PyExc_ValueError, "command not found in the device command list");
			return NULL;
		}
		inputType = it->second.inputType;
		outputType = it->second.outputType;

		DevArgument	argin,
				argout;

		if (!TACOPythonClient::convertToDevArgument(input, inputType, argin))
		{
			PyErr_SetString(PyExc_RuntimeError, "could not convert input argument");
			return NULL;
		}
		if (!TACOPythonClient::createDevArgument(outputType, argout))
		{
			PyErr_SetString(PyExc_RuntimeError, "could not convert output argument");
			return NULL;
		}

		try
		{
			p->m_Client->execute(commandNumber, argin, inputType, argout, outputType);
			PyObject *ret;
			if (!TACOPythonClient::convertToPyObject(outputType, argout, &ret))
				return NULL;
			return ret;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_clientNetworkProtocol(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			return PyInt_FromLong(p->m_Client->clientNetworkProtocol());
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_setClientNetworkProtocol(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		long protocol;
		if (!PyArg_ParseTuple(args, "l", &protocol))
			return NULL;
		try
		{
			p->m_Client->setClientNetworkProtocol(TACO::NetworkProtocol(protocol));
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_clientNetworkTimeout(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			return PyFloat_FromDouble(p->m_Client->clientNetworkTimeout());
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_setClientNetworkTimeout(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		double timeout;
		if (!PyArg_ParseTuple(args, "d", &timeout))
			return NULL;
		try
		{
			p->m_Client->setClientNetworkTimeout(timeout);
			Py_INCREF(Py_None);
			return Py_None;
		}	
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_listen(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NotImplementedError, "the event handling is not yet implemented");
	return NULL;
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		long 	 event;
		PyObject *function,
			 *data = 0;
		if (!PyArg_ParseTuple(args, "lO|O", &event, &function, &data))
			return NULL;
		long id;

		PyObject *me;
		if (PyCallable_Check(function))
		{
			if (PyMethod_Check(function))
			{
				me = PyMethod_GET_SELF(function);
				function = PyMethod_GET_FUNCTION(function);	
			}	
#if 0
//			Py_INCREF(function);
			if (me)
				functionArgs = Py_BuildValue("(O)", me);
			else
				functionArgs = Py_BuildValue("()");	
			Py_DECREF(functionArgs);
			return r;
//			Py_DEVREF(function);		
#endif
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "third argument must be callable");
			return NULL;
		}
#if 0
		if (((TACOClientObject *)->m_Client)->addEvent ....
#endif
		return PyLong_FromLong(id);
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceQueryResourceInfo(PyObject *self, PyObject *args);

static PyObject *TACOClient_deviceQueryCommandInfo(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		PyObject *ret;
		try
		{
//			TACO::CommandInfoMap info = p->m_Client->deviceQueryCommandInfo();

			if (!(ret = PyDict_New()))
			{
				PyErr_SetString(PyExc_RuntimeError, "could not create command device info dictionary");
				return NULL;
			}
			for (TACO::CommandInfoMap::iterator it = p->m_CmdInfo->begin(); it != p->m_CmdInfo->end(); ++it)
			{
				PyObject *key = PyString_FromString(const_cast<char *>(it->second.name.c_str())),
					*d = PyDict_New();
				if (key && d)
				{
					PyObject *tuple = PyTuple_New(2);
					if (tuple)
					{
						PyObject *inputType = PyLong_FromLong(it->second.inputType),
							*outputType = PyLong_FromLong(it->second.outputType),
							*key2 = PyString_FromString("cmd"),
							*key3 = PyString_FromString("paramtype"),
							*cmd = PyLong_FromLong(it->first);
						if (inputType && outputType && key2 && key3 && cmd)
						{
							PyTuple_SET_ITEM(tuple, 0, inputType);
							PyTuple_SET_ITEM(tuple, 1, outputType);
							PyDict_SetItem(d, key2, cmd);
							PyDict_SetItem(d, key3, tuple);
							PyDict_SetItem(ret, key, d);
						}
						Py_XDECREF(inputType);
						Py_XDECREF(outputType);
						Py_XDECREF(key2);
						Py_XDECREF(key3);
						Py_XDECREF(cmd);
					}
					Py_XDECREF(tuple);
				}
				Py_XDECREF(key);
				Py_XDECREF(d);

			}
			return ret;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceOn(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			p->m_Client->deviceOn();
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceOff(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			p->m_Client->deviceOff();
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceReset(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			p->m_Client->deviceReset();
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceState(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			return PyLong_FromLong(p->m_Client->deviceState());
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceStatus(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			return PyString_FromString(const_cast<char *>(p->m_Client->deviceStatus().c_str()));
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceVersion(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			return PyString_FromString(const_cast<char *>(p->m_Client->deviceVersion().c_str()));
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceTypes(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			TACO::Client::DeviceTypeSet types = p->m_Client->deviceTypes();
			PyObject *ret;
			ret = PyList_New(0);
			if (!ret)
			{
				PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
				return NULL;
			}
			for (TACO::Client::DeviceTypeSet::iterator it = types.begin(); it != types.end(); ++it)
			{
				PyObject *o = PyString_FromString((*it).c_str());
				if (!o)
				{
					PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
					return NULL;
				}
				if (PyList_Append(ret, o))
				{
					PyErr_SetString(PyExc_RuntimeError, "could not create the device type list");
					return NULL;
				}
				Py_DECREF(o);
			}
			return ret;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceUpdateResource(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		char 	*name,
			*value;
		if (!PyArg_ParseTuple(args, "ss", &name, &value))
			return NULL;
		try
		{
			p->m_Client->deviceUpdateResource(name, value);
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceQueryResource(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		char *name;
		if (!PyArg_ParseTuple(args, "s", &name))
			return NULL;
		try
		{
			return PyString_FromString(const_cast<char *>(p->m_Client->deviceQueryResource(name).c_str()));
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}

	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_deviceUpdate(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
			p->m_Client->deviceUpdate();
			Py_INCREF(Py_None);
			return Py_None;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static PyObject *TACOClient_isDeviceOff(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	if (p->m_Client)
	{
		try
		{
#if PYTHON_API_VERSION >= 1012
			return PyBool_FromLong(p->m_Client->deviceState() == TACO::State::DEVICE_OFF);
#else
			return PyInt_FromLong(p->m_Client->deviceState() == TACO::State::DEVICE_OFF);
#endif
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what()); 
			PyObject_SetAttrString(TACOError, "errcode", PyInt_FromLong(e));
			return NULL;
		}
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static void TACOClient_del(PyObject *self);
static PyObject *TACOClient_new(
#if PYTHON_API_VERSION > 1010
	PyTypeObject *type, 
#endif
	PyObject *args, PyObject *kw);

static struct PyMethodDef TACOClient_methods[] = {
	{"new_Client", TACOClient_new, METH_VARARGS, ""},
	{"execute", TACOClient_execute, METH_VARARGS, "commandNumber, input = None\n\n"
			"Multipurpose method for direct command execution.\n"
			"commandNumber - the command number of the desired command.\n"
			"input - (optional) should contain the input data if the command expects input.\n"
			"To provide input to command with no input has no effect.\n\n"
			"Returns - the ouput of the command."},
	{"clientNetworkProtocol", TACOClient_clientNetworkProtocol, METH_VARARGS, "Returns the currently used network protocol.\n"
			"Normally will TCP (888) used. For small amount of transferred data UDP(999) may used as well.\n"
			"Use of UDP is faster than TCP, but TCP is safe.\n"},
	{"setClientNetworkProtocol", TACOClient_setClientNetworkProtocol, METH_VARARGS, "protocol\n\n"
			"Sets the currently used network protocol"
			"see also: 'clientNetworkProtocol'"},
	{"clientNetworkTimeout", TACOClient_clientNetworkTimeout, METH_VARARGS, "Returns the timeout value (in s) of the network connection\n"
			"This value defines the timeslice for getting an answer from the device.\n"
			"If the device not answers inside this timeslice an exception (TACOError) will be thrown\n"
			"If the standard time (3 s) is to short, you may increase it.\n"
			"It does not increase the communication time."},
	{"setClientNetworkTimeout", TACOClient_setClientNetworkTimeout, METH_VARARGS, "seconds, useconds\n\n"
			"Sets the maximum time for the communication between client and server.\n"
			"see also: 'clientNetworkTimeout'"},
	{"listen", TACOClient_listen, METH_VARARGS, "eventNumber, function\n\n"
			"Listen to an event.\n"
			"Please do not use this method"},
	{"deviceQueryResourceInfo", TACOClient_deviceQueryResourceInfo, METH_VARARGS},
	{"deviceQueryCommandInfo", TACOClient_deviceQueryCommandInfo, METH_VARARGS},
	{"deviceOn", TACOClient_deviceOn, METH_VARARGS, "Switches the device from the state 'off' into a working state."},
	{"deviceOff", TACOClient_deviceOff, METH_VARARGS, "Switches the device into the state 'off'."},
	{"deviceReset", TACOClient_deviceReset, METH_VARARGS, "Resets the device. Actions which are executed depend on the device."},
	{"deviceStatus", TACOClient_deviceStatus, METH_VARARGS, "Returns the current device state as string"},
	{"deviceState", TACOClient_deviceState, METH_VARARGS, "Returns the current device state as number"},
	{"deviceVersion", TACOClient_deviceVersion, METH_VARARGS, "Returns the version of the device as string"},
	{"deviceTypes", TACOClient_deviceTypes, METH_VARARGS, "Returns a list of strings describing the exported device types"},
	{"deviceQueryResource", TACOClient_deviceQueryResource, METH_VARARGS, "resource\n\n"
			"Returns the value of the resource. The return value is a string"},
	{"deviceUpdateResource", TACOClient_deviceUpdateResource, METH_VARARGS, "resource, value\n\n"
			"Updates the resource with value.\n"
			"The values are stored in the server and in the database as well.\n"
			"This command is only available if the device is in state 'off'"},
	{"deviceUpdate", TACOClient_deviceUpdate, METH_VARARGS, "Initiate the reread of all device resources from the database"
			"This command is only available if the device is in state 'off'"},
	{"isDeviceOff", TACOClient_isDeviceOff, METH_VARARGS, "Checks whether the device is in state 'off' or not."
			"Returns 'True' if the device is in state 'off' otherwise 'False'"},
	{NULL, NULL},
};

static PyObject *TACOClient_str(PyObject *self, PyObject *args)
{
	return PyString_FromString("TACOClient");
}

static PyObject *TACOClient_getattr(PyObject *self, char *name)
{
	return Py_FindMethod(TACOClient_methods, self, name);
}

static char TACOClient_doc[] =
PyDoc_STR("Client(devicename, access = 0, connect = 1) --> a TACO client object\n\n" 
"devicename - name of the TACO device\n"
"access - access level\n"
"connect - automatic connect 1(yes) or 0(no)\n"
"Basic TACO client class for the devices at the FRM-II.\n"
"This is a base class for TACO client. It provides standard methods which are\n"
"supported by every reasonable device. You have also direct access to the device\n"
"commands via the 'execute()' method.");

static PyObject *TACOClient_alloc(PyTypeObject *type, Py_ssize_t nitems)
{
        PyObject *self;

        self = (PyObject *)PyObject_MALLOC(sizeof(TACOClientObject));
        if (self == NULL)
                return (PyObject *)PyErr_NoMemory();
        PyObject_INIT(self, type);
        return self;
}


static PyTypeObject TACOClientType = {
	PyObject_HEAD_INIT(&PyType_Type)	
	ob_size : 0,				// obj_size
	tp_name : "TACOClient.Client",		// tp_name
	tp_basicsize : sizeof(TACOClientObject),// tp_basicsize
	tp_itemsize : 0,			// tp_itemsize
	tp_dealloc : (destructor)TACOClient_del,// tp_dealloc
	tp_print : (printfunc)0,		// tp_print
	tp_getattr : (getattrfunc)TACOClient_getattr,	// tp_getattr
	tp_setattr : (setattrfunc)0,		// tp_setattr
#if PYTHON_API_VERSION == 1009
	tp_compare : 0,
#else
	tp_cmp : (cmpfunc)0,			// tp_cmp
#endif
	tp_repr : (reprfunc)0,			// tp_repr
	tp_as_number : 0,			// tp_as_number
	tp_as_sequence : 0,			// tp_as_sequence
	tp_as_mapping : 0,			// tp_as_mapping
	tp_hash : (hashfunc)0,			// tp_hash
	tp_call : (ternaryfunc)0,		// tp_call
	tp_str : (reprfunc)TACOClient_str,	// tp_str
#if PYTHON_API_VERSION >= 1012 
	tp_getattro : PyObject_GenericGetAttr,  // tp_getattro 
#else
	tp_getattro : 0,
#endif
        tp_setattro : 0,                        // tp_setattro
        tp_as_buffer : 0,                       // tp_as_buffer
        tp_flags : Py_TPFLAGS_DEFAULT 
#if PYTHON_API_VERSION > 1009 
	| Py_TPFLAGS_CHECKTYPES 
        | Py_TPFLAGS_BASETYPE
#endif
	,                    			// tp_flags 
        tp_doc : TACOClient_doc,                // tp_doc 
        tp_traverse : 0,                        // tp_traverse 
        tp_clear : 0,                           // tp_clear 
#if PYTHON_API_VERSION > 1009
        tp_richcompare : 0, //(richcmpfunc)time_richcompare,     // tp_richcompare 
        tp_tweaklistoffset : 0,                 // tp_weaklistoffset 
        tp_iter : 0,                            // tp_iter 
        tp_iternext : 0,                        // tp_iternext 
        tp_methods : TACOClient_methods,        // tp_methods 
        tmp_members : 0,                        // tp_members 
        tp_getset : 0, //time_getset,           // tp_getset 
        tp_base : 0,                            // tp_base 
        tp_dict : 0,                            // tp_dict 
        tp_descr_get : 0,                       // tp_descr_get 
        tp_descr_set : 0,                       // tp_descr_set 
        tp_dictoffset : 0,                      // tp_dictoffset 
        tp_init : 0,                            // tp_init 
        tp_alloc : TACOClient_alloc,            // tp_alloc 
        tp_new : TACOClient_new,                // tp_new 
        tp_free : 0,                            // tp_free 
#endif
};

static PyObject *TACOClient_deviceQueryResourceInfo(PyObject *self, PyObject *args)
{
	TACOClientObject *p = (TACOClientObject *)self;
	PyObject	*ret = PyDict_New();
	
	if (!ret)
	{
		PyErr_SetString(PyExc_RuntimeError, "could not create the resource info dictionary");
		return NULL;
	}
	if (p->m_Client)
	{
		TACO::ResourceInfoSet r = p->m_Client->deviceQueryResourceInfo();
		for (TACO::ResourceInfoSet::iterator it = r.begin(); it != r.end(); ++it)
		{
			PyObject *o = PyDict_New();
			PyObject *key = PyString_FromString(it->name.c_str());
			if (!o || !key)
			{
				PyErr_SetString(PyExc_RuntimeError, "could not create resource info dictionary");
				return NULL;
			}
			PyObject *key1,
				 *value;
			if ((key1 = PyString_FromString("type")) != NULL)
			{
				value = PyLong_FromLong(it->type);
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			if ((key1 = PyString_FromString("info")) != NULL)
			{
				value = PyString_FromString(const_cast<char *>(it->info.c_str()));
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			if ((key1 = PyString_FromString("defaults")) != NULL)
			{
				value = PyString_FromString(const_cast<char *>(it->defaults.c_str()));
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			if ((key1 = PyString_FromString("format")) != NULL)
			{
				value = PyString_FromString(const_cast<char *>(it->format.c_str()));
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			if ((key1 = PyString_FromString("max")) != NULL)
			{
				value = PyString_FromString(const_cast<char *>(it->max.c_str()));
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			if ((key1 = PyString_FromString("min")) != NULL)
			{
				value = PyString_FromString(const_cast<char *>(it->min.c_str()));
				PyDict_SetItem(o, key1, value);
				Py_DECREF(key1);
				Py_DECREF(value);
			}
			PyDict_SetItem(ret, key, o);
			Py_DECREF(key);
			Py_DECREF(o);
		}
		return ret;
	}
	if (p->m_Client)
	{
	}
	PyErr_SetString(PyExc_RuntimeError, "lost the TACO client");
	return NULL;
}

static void TACOClient_del(PyObject *self)
{
	delete ((TACOClientObject *)self)->m_Client;
	delete ((TACOClientObject *)self)->m_CmdInfo;
}

static char *TACOClient_kws[] = {(char*)"devicename", 
				 (char*)"access",
				 (char*)"connect",
				NULL};

static PyObject *TACOClient_new(
#if PYTHON_API_VERSION > 1010
	PyTypeObject *type,
#endif
	PyObject *args, PyObject *kw)
{
	char	*name;
	long	access = 0;
	int	connect = 1;

	TACOClientObject *self = NULL;
#if PYTHON_API_VERSION > 1009
	if (!PyArg_ParseTupleAndKeywords(args, kw, (char *)"s|li", TACOClient_kws, &name, &access, &connect))
		return NULL;
	self = (TACOClientObject *)(type->tp_alloc(type, 1));
#else
	if (!PyArg_ParseTuple(args, (char *)"s|li", &name, &access, &connect))
		return NULL;
	self = PyObject_New(TACOClientObject, &TACOClientType);
#endif
	connect = !!connect;
	if (self)
		try
		{
			printf("name = %s, access = %ld, connect = %d\n", name, access, connect);
			self->m_Client = new TACOClient(name, access, bool(connect));
			self->m_CmdInfo = new TACO::CommandInfoMap();
			TACO::CommandInfoMap info = self->m_Client->deviceQueryCommandInfo();
			*(self->m_CmdInfo) = info;
		}
		catch (const TACO::Exception &e)
		{
			PyErr_SetString(TACOError, e.what());
			long	err = e;
			PyObject_SetAttrString(TACOError, (char *)"errcode", Py_BuildValue((char *)"i", err));
			return NULL;
		}
	return (PyObject *)self;
}

static PyObject *TACOClient_sync(PyObject *self, PyObject *args)
{
	struct timeval t = {0, 0};
	DevLong		err;
	if (!PyArg_ParseTuple(args, (char *)"l|l", &t.tv_sec, &t.tv_usec))
		return NULL;
	if (dev_synch(&t, &err) != DS_OK)
	{
		PyErr_SetString(TACOError, TACO::errorString(err).c_str());
		PyObject_SetAttrString(TACOError, (char *)"errcode", PyLong_FromLong(err));
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *TACOClient_unlisten(PyObject *self, PyObject *args)
{
	long eventID;
	if (!PyArg_ParseTuple(args, (char *)"l", &eventID))
		return NULL;
#if 0
	
#endif
	PyErr_SetString(PyExc_NotImplementedError, "event handling not yet implemented");
	return NULL;
}

static struct PyMethodDef TACOClientType_methods [] = {
//	{"Client", TACOClientObject_new, METH_VARARGS,  "devicename - name of the TACO device\n\n"
//							"Basic TACO client class for the devices at the FRM-II.\n"
//							"This is a base class for TACO client. It provides standard methods which are\n"
//							"supported by every reasonable device. You have also direct access to the device\n"
//							"commands via the 'execute()' method."},
	{(char *)"sync", TACOClient_sync, METH_VARARGS, (char *)"seconds, microseconds\n\nthis function sync's the callbacks for the events"},
	{(char *)"unlisten", TACOClient_unlisten, METH_VARARGS, (char *)"eventId - event Id to unregister\n\nunregisters an event"},
	{NULL, NULL},
};

void initTACOClient(int i)
{
	PyObject 	*m = Py_InitModule3((char *)"TACOClient", TACOClientType_methods, (char *)"This modules contains the Client class for the FRM-II TACO devices"),
			*d,
			*tmp;
	DevLong		e;

	db_import(&e);

#if PYTHON_API_VERSION > 1010
	if (PyType_Ready(&TACOClientType) < 0)
		return;
#endif

	Py_INCREF(&TACOClientType);
        PyModule_AddObject(m, (char *)"Client", reinterpret_cast<PyObject *>(&TACOClientType));

	TACOError = PyErr_NewException((char *)"TACOClient.TACOError", NULL, NULL);
	Py_INCREF(TACOError);
	PyModule_AddObject(m, (char *)"TACOError", TACOError);

	d = PyModule_GetDict(m);
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_ON)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_ON", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_OFF)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_OFF", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_RESET)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_RESET", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_STATE)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_STATE", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_STATUS)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_STATUS", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_VERSION)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_VERSION", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_TYPES)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_TYPES", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_QUERY_RESOURCE)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_QUERY_RESOURCE", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_UPDATE_RESOURCE)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_UPDATE_RESOURCE", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_UPDATE)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_UPDATE", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(TACO::Command::DEVICE_QUERY_RESOURCE_INFO)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"DEVICE_QUERY_RESOURCE_INFO", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(D_UDP)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"UDP", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyLong_FromLong(D_TCP)) != NULL)
	{
		PyDict_SetItemString(d, (char *)"TCP", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyString_FromString("Jens Krueger")) != NULL)
	{
		PyDict_SetItemString(d, (char *)"__author__", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyString_FromString("$Revision: 1.4 $")) != NULL)
	{
		PyDict_SetItemString(d, (char *)"__revision__", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyString_FromString("$Date: 2008-10-23 10:24:03 $")) != NULL)
	{
		PyDict_SetItemString(d, (char *)"__date__", tmp);
		Py_DECREF(tmp);
	}
	if ((tmp = PyString_FromString("jens dot krueger add frm2 dot tum dot de")) != NULL)
	{
		PyDict_SetItemString(d, (char *)"__email__", tmp);
		Py_DECREF(tmp);
	}
}
}
