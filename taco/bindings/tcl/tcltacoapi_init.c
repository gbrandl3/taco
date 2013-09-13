/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        tcltacoapi_init.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: 
 *
 * Author(s):	Gilbert Pepellin
 *              $Author: jkrueger1 $
 *
 * Original:	June, 1998
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-18 22:04:49 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/*
 * File:
 * 
 * 
 * Description:
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2006-09-18 22:04:49 $
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





