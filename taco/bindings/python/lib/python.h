
/*********************************************************************
 *
 * File:	Python.h
 *
 * Project:	<PROJECT>
 *
 * Description:	<CLASSDESC>
 *		
 *
 * Author(s):	<AUTHOR>
 *
 * Original:	<DATE>
 *
 * Copyright(c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _PYTHON_H
#define _PYTHON_H


#define DevErr_PythonException			999
#define DevErr_PythonDataConversionError	998


typedef struct _PythonClassRec *PythonClass;
typedef struct _PythonRec *Python;

extern PythonClass pythonClass;

#endif /*_PYTHON_H*/
