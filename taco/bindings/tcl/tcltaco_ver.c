/* 
 * File	: 	tcltaco_ver.c
 *
 * Author(s): 	G. Pepellin
 *		$Author: jkrueger1 $
 *
 * Original: 	june, 1998
 *                 
 * Description: Set global variables in the scope of Tcl that provides version numbers
 * 
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-07 13:19:25 $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

static char RcsId[] = "$Revision: 1.2 $";
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

    
    
     
