/* 
 * File: 	db_io_debug.c
 *
 * Author(s): 	Denis Beauvois
 *		Gilbert Pepellin
 *		J.Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	june, 1996
 *                 
 * Description: This file contains the functions in order to use the debug 
 *          	mode for db_io in a tcl script
 *          	How to use :	
 *          		set var 1
 *          		db_io_debug var
 *
 * Version:	$Revision: 1.2 $
 *
 * Date: 	$Date: 2003-05-07 13:42:41 $
 *
*/

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
		snprintf (aux, 80, "ERROR in db_io_debug : bad value in argin %d",Value);
		Tcl_AppendResult (interp,aux,NULL);
		free(aux); 
		aux=NULL;
		return TCL_ERROR;
	}
    
	return TCL_OK;
}

    
    
     
