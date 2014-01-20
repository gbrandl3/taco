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
 * File:        dev_io_protocol.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: 
 *
 * Author(s):	Gilber Pepellin
 *              $Author: jkrueger1 $
 *
 * Original:	October 1998
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:34 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/*+*********************************************************************
*
*  Function   : dev_io_protocol(s e $dev $protocol)
*  Purpose    : set RPC protocol (UDP or TCP) for a device server
*  Args In    : dev: device name. To simplify the programation, it will be
*                     always imported.
*             : protocol: a string matching "udp" or "tcp"
*  Args Out   : s: Tcl variable that will hold the status returned by 
*                   dev_import or dev_rpc_protocol (DSOK | DS_NOTOK). If s does
*                   not exists in the Tcl scope, it is created.
*             : e: Tcl variable that will hold the error returned by
*                   dev_import or dev_rpc_protocol (if any). Its format is:
*                   "error message [error number]".If e does
*                   not exists in the Tcl scope, it is created.
*  returns    : nothing 
*  Description: 
*
**********************************************************************-*/
#include <dev_io.h>
#include <debug.h>

int dev_io_protocol(ClientData clientdata, Tcl_Interp *interp,
	    int argc, char **argv)
{
	int 		DEBUG_TO = 0;
	char 		*State,
			*Error,
			*DeviceName,
			*SProtocol;
	long		ret;
	devserver 	ds;
	DevLong 	DeviceError;
	char 		StringTemp[250];
	char 		*StringAux = NULL;
 
	DEBUG_TO = 0;
	if (argc != 5) 
	{
		Tcl_AppendResult(interp,"should be... dev_io_protocol status error device  udp | tcp "
			,(char *)NULL);
		return TCL_ERROR; 
	}
	State       = argv[1];
	Error       = argv[2];
	DeviceName  = argv[3];
	SProtocol   = argv[4];
	if(DEBUG_TO) 
		printf("Enterring dev_io_protocol with Dev= %s SPro= %s\n", DeviceName,SProtocol);
 
/* Init the 2 variables State and Error into the Tcl scope */
	Tcl_SetVar (interp,State,"0",0); 
	Tcl_SetVar (interp,Error,"",0); 
 
 /* Import the device (yes I know that it is simplist but ...) */
	ret = dev_import (DeviceName,0,&ds,&DeviceError);
	if (ret != DS_OK) /* dev_import failed */
	{
		snprintf (StringTemp, sizeof(StringTemp), "%ld",ret);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, sizeof(StringTemp), "%s [%ld]",StringAux, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		if(StringAux != NULL) 
			free(StringAux);
		return TCL_OK;
	}

	if(strcasecmp(SProtocol,"udp") == 0) 
	{
		ret = dev_rpc_protocol(ds,D_UDP,&DeviceError);
		if (ret != DS_OK) /* dev_rpc_protocol failed */
		{
			snprintf (StringTemp, sizeof(StringTemp), "%ld",ret);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, sizeof(StringTemp), "%s [%ld]",StringAux, (long)DeviceError);
			Tcl_SetVar (interp,Error,StringTemp,0);
			if(StringAux != NULL) 
				free(StringAux);
			return TCL_OK;
  		} 
	}
	else if(strcasecmp(SProtocol,"tcp") == 0) 
	{
		ret = dev_rpc_protocol(ds,D_TCP,&DeviceError);
		if (ret != DS_OK) /* dev_import failed */
		{
			snprintf (StringTemp, sizeof(StringTemp), "%ld",ret);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, sizeof(StringTemp), "%s [%ld]",StringAux, (long)DeviceError);
			Tcl_SetVar (interp,Error,StringTemp,0);
			if(StringAux != NULL) 
				free(StringAux);
			return TCL_OK;
		} 
	}
	else 
	{ /* SProtocol is neither UDP nor TCP */
		ret=DS_NOTOK;
		DeviceError=DevErr_UnknownInputParameter;
		snprintf(StringTemp,  sizeof(StringTemp), "%ld",ret);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp,  sizeof(StringTemp), "%s [%ld]",StringAux, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		if(StringAux != NULL) 
			free(StringAux);
		return TCL_OK;
	}

	return TCL_OK;
 
} /* Ends function dev_io_protocol() */





