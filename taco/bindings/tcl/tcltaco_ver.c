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
 * File	: 	tcltaco_ver.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: Set global variables in the scope of Tcl that provides version numbers
 *
 * Author(s): 	G. Pepellin
 *              $Author: jkrueger1 $
 *
 * Original:    June, 1998
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-18 22:04:49 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

static char RcsId[] = "$Revision: 1.3 $";
static char LIBDSAPI_VER[] = "6.4";
static char LIBDBAPI_VER[] = "5.15";
static char LIBDCAPI_VER[] = "4.1";
static char LIBDSXDR_VER[] = "5.16";

int tcltaco_ver (ClientData clientdata, Tcl_Interp *interp, int argc, char **argv)
/*SUPPRESS761*/
{
	Tcl_SetVar(interp,"TclTacoApi_version",RcsId,TCL_GLOBAL_ONLY);  
	Tcl_SetVar(interp,"LIBDSAPI_VER",LIBDSAPI_VER,TCL_GLOBAL_ONLY);  
	Tcl_SetVar(interp,"LIBDBAPI_VER",LIBDBAPI_VER,TCL_GLOBAL_ONLY);  
	Tcl_SetVar(interp,"LIBDCAPI_VER",LIBDCAPI_VER,TCL_GLOBAL_ONLY);  
	Tcl_SetVar(interp,"LIBDSXDR_VER",LIBDSXDR_VER,TCL_GLOBAL_ONLY);      
	return TCL_OK;
}

    
    
     
