/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
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
 * File:        debug.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains the necessary functions to manage the debug mode
 *          	In order to use the debug mode in a tcl script you must write
 *         	for example :
 *         		set var 1
 *         		dev_io_debug var
 *
 * Author(s):   Denis Beauvois
 *              Gilbert Pepellin
 *              J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    June, 1996
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:33 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <dev_io.h>


/* =============================================================== */
/* --------------------------------------------------------------- */
/*                       Function dev_io_debug                     */
/* Description :                                                   */
/*         This is the function called by the tcl interpreter      */
/*         It requires 1 argument :                                */
/*            level                                                */
/* --------------------------------------------------------------- */
/* =============================================================== */
int dev_io_debug (ClientData clientdata, Tcl_Interp *interp, int argc, char **argv)
/*SUPPRESS761*/
{
	char 	*LevelString;
	int 	ReturnedCode;
	long 	Value;
 
/* -------------------------- */
/* Test of the input argument */
/* -------------------------- */   
	if (argc != 2)
	{
		printf("dev_io_debug Error : should be.... dev_io_debug 'level'\n");
		return TCL_ERROR;
	}
	LevelString  = Tcl_GetVar(interp,argv[1],0);
	ReturnedCode = Tcl_ExprLong(interp,LevelString,&Value);
	if (ReturnedCode==TCL_ERROR)
	{
		printf("dev_io_debug Error : bad argin %s\n",argv[1]);
		return TCL_ERROR;
	}
    
/* ------------------------- */
/* Call of SetDebugMode      */
/* ------------------------- */
	ReturnedCode = SetDebugMode(Value);
	if (ReturnedCode==False)
	{
		printf("Error in SetDebugMode(Value)...bad Value %ld\n",Value);
		return TCL_ERROR;
	}
    
	return TCL_OK;
}
      
 
    
 
 
 
 
