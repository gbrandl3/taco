/*********************************************************************
 *
 * File:	PythonP.h
 *
 * Project:	<PROJECT>
 *
 * Description:	<CLASSDESC>
 *
 * Author(s):	<AUTHOR>
 *
 * Original:	<DATE>	
 *
 * Copyright (c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _PYTHONP_H
#define _PYTHONP_H

#include <Python.h>


#define MAX_PY_DEVICE	100


typedef struct _PythonClassPart
{
	int	nada;
} PythonClassPart;



typedef struct _PythonPart
{
	PyObject	*Py_device;
	long	 	cmd_code;
} PythonPart;



typedef struct _PythonClassRec
{
   DevServerClassPart	devserver_class;
   PythonClassPart	python_class;
} PythonClassRec;



extern PythonClassRec pythonClassRec;


typedef struct _PythonRec
{
   DevServerPart 	devserver;
   PythonPart 		python;
} PythonRec;

#endif /*_PYTHONP_H*/

