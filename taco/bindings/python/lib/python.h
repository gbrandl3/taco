/*********************************************************************
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
 * Date:	$Date: 2004-05-14 15:02:07 $
 *
 * Version:	$Revision: 1.2 $
 *
 * Copyright(c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _PYTHON_H
#define _PYTHON_H


#define DevErr_PythonException			999
#define DevErr_PythonDataConversionError	998

typedef struct _PythonClassRec 	*PythonClass;
typedef struct _PythonRec 	*Python;

extern PythonClass 		pythonClass;

#endif /*_PYTHON_H*/
