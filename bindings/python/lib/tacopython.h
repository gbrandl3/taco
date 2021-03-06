/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2014 by European Synchrotron Radiation Facility,
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
 * File:	Python.h
 *
 * Project:	Taco server in Python
 *
 * Description:	
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Original:	
 *
 * Date:	$Date: 2008-10-15 14:48:18 $
 *
 * Version:	$Revision: 1.2 $
 */

#ifndef _PYTHON_H
#define _PYTHON_H


#define DevErr_PythonException			999
#define DevErr_PythonDataConversionError	998

typedef struct _PythonClassRec 	*PythonClass;
typedef struct _PythonRec 	*Python;

extern PythonClass 		pythonClass;

#endif /*_PYTHON_H*/
