static char RcsId[] = 
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/bindings/tcl/dc_io_debug.c,v 1.1 2003-03-18 16:28:26 jkrueger1 Exp $";
/* File		: dc_io_debug.c
   Author	: Denis Beauvois
                  june, 1996
                  
  Description	:
           This file contains the functions in order to use the debug mode for dc_io_get
           in a tcl script
           How to use :	
           		set var 1
           		dc_io_debug var
*/

/*
$Revision: 1.1 $
$Date: 2003-03-18 16:28:26 $
$Author: jkrueger1 $
$Log: not supported by cvs2svn $
 * Revision 4.4  98/10/13  15:56:22  15:56:22  pepellin (Gilbert Pepellin)
 * Add command dev_io_protocol
 * 
 * Revision 4.3  98/10/13  09:51:15  09:51:15  pepellin (Gilbert Pepellin)
 * Add command dev_io_timeout
 * 
 * Revision 4.2  98/06/05  10:52:35  10:52:35  pepellin (Gilbert Pepellin)
 * Change lib name from libtclapi to libtcltacoapi
 * 
 * Revision 4.1  98/05/13  14:20:04  14:20:04  pepellin (Gilbert Pepellin)
 * 1st version using 8.0
 * 
 * Revision 2.10  1997/11/21  16:19:37  pepellin
 * All function purified (dc_io_get not tested with all possible types)
 *
 * Revision 2.9  1997/11/14  14:06:42  pepellin
 * Fix MLK and bad returned values in db_io_get
 *
 * Revision 2.8  97/11/12  17:37:58  17:37:58  pepellin (G.Pepellin)
 * Fix a remaining bug into dev_io.c about the way to free the output
 * args when its type is D_VAR_STRINGARR. 
 * 
 * Revision 2.7  1997/10/24  06:48:36  pepellin
 * dev_io memory leaks fixed.
 *
 * Revision 2.6  1997/10/20  15:50:21  pepellin
 * GP takes control
 *
 * Revision 2.5  97/08/04  16:49:09  16:49:09  meyer (J.Meyer)
 * Corrected memory leaks in dev_io.c and db_io_put.c
 * 
 * Revision 2.4  97/07/23  08:40:22  08:40:22  meyer (J.Meyer)
 * Added dev_io_free() and dev_io_free_all().
 * 
 * Revision 2.3  97/07/19  01:03:46  01:03:46  pepellin (G.Pepellin)
 * Check in before adding free calls (by JM).
 * 
 * Revision 2.2  1997/07/01  13:22:46  pepellin
 * Support stateless dev_import
 * Not valid commands or arguments do not printf on xterm anymore. They
 * return DS_NOTOK and corresponding error number and strings
 *
 * Revision 2.1  97/02/18  17:18:46  17:18:46  pepellin (G.Pepellin)
 * First portable (hpux9 solaris) version
 * 
 * Revision 1.8  97/02/14  13:36:56  13:36:56  pepellin (G.Pepellin)
 * First Solaris port
 * 
 * Revision 1.7  97/01/22  14:36:23  14:36:23  pepellin (Gilbert Pepellin)
 * Cleaned code using Codecenter
 * 
 * Revision 1.4  96/11/08  11:20:26  11:20:26  pepellin (Gilbert Pepellin)
 * To put all *.c at the same source level
 * 
 * Revision 1.1  96/09/23  17:35:34  17:35:34  pepellin (Gilbert Pepellin)
 * Initial revision
 * 
*/

#include <dc_io.h>
#include <debug.h>


/* =============================================================== */
/* --------------------------------------------------------------- */
/*                  Function dc_io_debug                           */
/* Description :                                                   */
/*          This is the function called by the tcl interpreter     */
/*          It requires 1 argument : level_of_debugging (0 or 1)   */
/* --------------------------------------------------------------- */
/* =============================================================== */
int dc_io_debug (clientdata,interp,argc,argv)
/*SUPPRESS761*/
    ClientData clientdata;
    Tcl_Interp *interp;
    int argc;
    char *argv[];
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
     Tcl_AppendResult(interp,"ERROR in dc_io_debug : should be...... db_io_debug 'level'",NULL);
     return TCL_ERROR;
    }
 LevelString = Tcl_GetVar(interp,argv[1],0);
 ReturnedCode = Tcl_ExprLong (interp,LevelString,&Value);
 if (ReturnedCode==TCL_ERROR)
    {
     Tcl_ResetResult(interp);
     aux = (char *) malloc(80);
     sprintf (aux,"ERROR in dc_io_debug : bad argin %s",argv[1]);
     Tcl_AppendResult (interp,aux,NULL);
     free(aux); aux=NULL;
     return TCL_ERROR;
    }
 
 /* ---------------------- */
 /* Call of SetDBDebugMode */
 /* ---------------------- */
 ReturnedCode = SetDCDebugMode(Value);
 if (ReturnedCode==False)
    {
     Tcl_ResetResult(interp);
     sprintf (aux,"ERROR in dc_io_debug : bad value in argin %d",Value);
     Tcl_AppendResult (interp,aux,NULL);
     return TCL_ERROR;
    }
    
 return TCL_OK;

}

    
    
     
