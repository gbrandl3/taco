/*
 * File:
 *
 * Author(s):	Gilber Pepellin
 *		$Author: jkrueger1 $
 *
 * Original:	October 1998
 *
 * Description:
 *
 * Version: 	$Revision: 1.2 $
 *		
 * Date:	$Date: 2003-05-07 13:19:25 $
 *
 */

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
	long 		LValue, 
			ret;
	struct timeval 	ti;
	devserver 	ds;
	long 		DeviceError;
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
		snprintf (StringTemp, sizeof(StringTemp), "%d",ret);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, sizeof(StringTemp), "%s [%d]",StringAux,DeviceError);
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
			snprintf (StringTemp, sizeof(StringTemp), "%d",ret);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, sizeof(StringTemp), "%s [%d]",StringAux,DeviceError);
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
			snprintf (StringTemp, sizeof(StringTemp), "%d",ret);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(DeviceError);
			snprintf (StringTemp, sizeof(StringTemp), "%s [%d]",StringAux,DeviceError);
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
		snprintf(StringTemp,  sizeof(StringTemp), "%d",ret);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp,  sizeof(StringTemp), "%s [%d]",StringAux,DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		if(StringAux != NULL) 
			free(StringAux);
		return TCL_OK;
	}

	return TCL_OK;
 
} /* Ends function dev_io_protocol() */





