/*
 * File:
 * 
 * Author(s):	Gilbert Pepellin
 *
 * Original:	June, 1998
 * 
 * Description:
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-07 13:19:25 $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

extern int tcltaco_ver();
extern int dev_io();
extern int dev_io_asyn();
extern int dev_io_debug();
extern int dev_io_free();
extern int dev_io_free_all();
extern int db_io_get ();
extern int db_io_debug ();
extern int db_io_put ();
extern int dc_io_debug ();
extern int dc_io_get ();
extern int dc_io_get_hist ();
extern int dev_io_timeout();
extern int dev_io_protocol();


int Tcltacoapi_Init(Tcl_Interp* interp)
{
	Tcl_CreateCommand(interp,"tcltaco_ver",tcltaco_ver,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL); 
                           
	Tcl_CreateCommand(interp,"dev_io",dev_io,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
/*
 *	Tcl_CreateCommand(interp,"dev_io_asyn",dev_io_asyn,(ClientData)NULL,
 *			(Tcl_CmdDeleteProc *)NULL)
 */

	Tcl_CreateCommand(interp,"dev_io_free",dev_io_free,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);                     
	Tcl_CreateCommand(interp,"dev_io_free_all",dev_io_free_all,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);                     
	Tcl_CreateCommand(interp,"dev_io_debug",dev_io_debug,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"db_io_debug",db_io_debug,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"db_io_get",db_io_get,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"db_io_put",db_io_put,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"dc_io_get",dc_io_get,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"dc_io_debug",dc_io_debug,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"dc_io_get_hist",dc_io_get_hist,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);
	Tcl_CreateCommand(interp,"dev_io_timeout",dev_io_timeout,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL); 
	Tcl_CreateCommand(interp,"dev_io_protocol",dev_io_protocol,(ClientData)NULL,
			(Tcl_CmdDeleteProc *)NULL);                   

	Tcl_PkgProvide(interp, "tcltacoapi", "4.4"); 
     
	return TCL_OK;
}





