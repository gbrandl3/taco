/* 
 * File: 	debug.c
 *
 * Author(s): 	Denis Beauvois
 * 		Gilbert Pepellin
 * 		J.Meyer
 *		$Author: jkrueger1 $
 *
 * Original:    june, 1996
 *              
 * Description: This file contains the necessary functions to manage the debug mode
 *          	In order to use the debug mode in a tcl script you must write
 *         	for example :
 *         		set var 1
 *         		dev_io_debug var
 *         
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-07 13:19:25 $
 *
 */

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
int dev_io_debug (ClientData clientdata, Tcl_Interp *interp, int argc, char *argv)
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
		printf("Error in SetDebugMode(Value)...bad Value %d\n",Value);
		return TCL_ERROR;
	}
    
	return TCL_OK;
}
      
 
    
 
 
 
 
