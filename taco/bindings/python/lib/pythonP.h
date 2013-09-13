/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2013 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:	PythonP.h
 *
 * Project:	TACO server in Python
 *
 * Description:	
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Original:	
 *
 * Date:	$Date: 2005-07-25 13:33:42 $
 *
 * Version:	$Revision: 1.3 $
 */

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
	PythonClassPart		python_class;
} PythonClassRec;

extern PythonClassRec pythonClassRec;

typedef struct _PythonRec
{
	DevServerPart 	devserver;
	PythonPart	python;
} PythonRec;

#endif /*_PYTHONP_H*/

