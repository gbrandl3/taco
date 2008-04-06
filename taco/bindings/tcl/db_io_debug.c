/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * File: 	db_io_debug.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains the functions in order to use the debug 
 *          	mode for db_io in a tcl script
 *          	How to use :	
 *          		set var 1
 *          		db_io_debug var
 *
 * Author(s): 	Denis Beauvois
 *		Gilbert Pepellin
 *		J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	June, 1996
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:33 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <db_io.h>
#include <debug.h>
/* =============================================================== */
/* --------------------------------------------------------------- */
/*                  Function db_io_debug                           */
/* Description :                                                   */
/*          This is the function called by the tcl interpreter     */
/*          It requires 1 argument : level_of_debugging (0 or 1)   */
/* --------------------------------------------------------------- */
/* =============================================================== */
int db_io_debug (ClientData clientdata,Tcl_Interp *interp,int argc,char **argv)
/*SUPPRESS761*/
{
	char *LevelString,*aux;
	int ReturnedCode;
	long Value;
 
/* ---------------------------- */
/* Test of the input argument   */
/* ---------------------------- */
	if (argc !=2)
	{ 
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"ERROR in db_io_debug : should be...... db_io_debug 'level'",NULL);
		return TCL_ERROR;
	}
	LevelString = Tcl_GetVar(interp,argv[1],0);
	ReturnedCode = Tcl_ExprLong (interp,LevelString,&Value);
	if (ReturnedCode==TCL_ERROR)
	{
		Tcl_ResetResult(interp);
		aux = (char *)malloc(80);
		snprintf (aux, 80, "ERROR in db_io_debug : bad argin %s",argv[1]);
		Tcl_AppendResult (interp,aux,NULL);
		free(aux); 
		aux=NULL;
		return TCL_ERROR;
	}
	
/* ---------------------- */
/* Call of SetDBDebugMode */
/* ---------------------- */
	ReturnedCode = SetDBDebugMode(Value);
	if (ReturnedCode==False)
	{
		Tcl_ResetResult(interp);
		aux = (char *)malloc(80);
		snprintf (aux, 80, "ERROR in db_io_debug : bad value in argin %ld",Value);
		Tcl_AppendResult (interp,aux,NULL);
		free(aux); 
		aux=NULL;
		return TCL_ERROR;
	}
    
	return TCL_OK;
}

    
    
     
