/*
 * File:
 *
 * Author(s):	Gilbert Pepellin
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
*  Function   : dev_io_timeout(s e $dev $value)
*  Purpose    :  get or set the RPC Time OUT for a device server
*  Args In    :  dev: device name. To simplify the programation, it will be
*                     always imported.
*                value: If <=0 then dev_io_timeout RETURNS the current timeout
*                       by calling dev_rpc_timeout (ds,CLGET_TIMEOUT, ...)
*                       If >0 then this will set the new TO to this value 
*                       by calling dev_rpc_timeout (ds,CLSET_TIMEOUT, ...)
*                       value must be passed by value and not by reference
*  Args Out   :  s: Tcl variable that will hold the status returned by 
*                   dev_import or dev_rpc_timeout (DSOK | DS_NOTOK). If s does
*                   not exists in the Tcl scope, it is created.
*                e: Tcl variable that will hold the error returned by
*                   dev_import or dev_rpc_timeout (if any). Its format is:
*                   "error message [error number]".If e does
*                   not exists in the Tcl scope, it is created.
*  returns    : The current TO if value <=0. 
*  Description: 
*
**********************************************************************-*/
#include <dev_io.h>
#include <debug.h>

int dev_io_timeout(ClientData clientdata, Tcl_Interp *interp,
	    int argc, char **argv)
{
	int DEBUG_TO;
	char *State,*Error,*DeviceName,*SValue;
	long LValue, ret;
	struct timeval ti;
	devserver ds;
	long DeviceError;
	char StringTemp[250];
	char *StringAux=NULL;
 
	DEBUG_TO = 0;
	if (argc != 5) 
	{
		Tcl_AppendResult(interp,"should be... dev_io_timeout 'status' 'error' 'device'  'value'", (char *)NULL);
		return TCL_ERROR; 
	}
	State       = argv[1];
	Error       = argv[2];
	DeviceName  = argv[3];
	SValue       = argv[4];
	if(DEBUG_TO) 
		printf("Enterring dev_io_timeout with Dev= %s SVal= %s\n", DeviceName,SValue);
 
 /* Init the 2 variables State and Error into the Tcl scope */
	Tcl_SetVar (interp,State,"0",0); 
	Tcl_SetVar (interp,Error,"",0);
	ret=Tcl_ExprLong(interp,SValue,&LValue);
	if (ret==TCL_ERROR) 
		printf("bad to value\n");
 
 /* Import the device (yes I know that it is simplist but ...) */
	ret = dev_import (DeviceName,0,&ds,&DeviceError);
	if (ret != DS_OK) /* dev_import failed */
	{
		snprintf (StringTemp, sizeof(StringTemp), "%d",ret);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, sizeof(StringTemp), "%s [%d]",StringAux,DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		if(StringAux != NULL) free(StringAux);
		return TCL_OK;
	}
 
/* LValue= -1 then read the current timeout */
	if(LValue <= 0)
	{
		ret = dev_rpc_timeout(ds,CLGET_TIMEOUT,&ti,&DeviceError);
		if (ret != DS_OK) 
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
		if(DEBUG_TO) 
			printf("TO returned: %d\n",ti.tv_sec);
		snprintf(StringTemp, sizeof(StringTemp), "%d",ti.tv_sec);
		Tcl_AppendResult(interp,StringTemp,NULL);
	} /* ends read TO */
	if(LValue >= 1)
	{
		ti.tv_sec = LValue; ti.tv_usec=0;
		ret = dev_rpc_timeout(ds,CLSET_TIMEOUT,&ti,&DeviceError);
		if (ret != DS_OK) 
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
	} /* ends read TO */
	return TCL_OK;
} /* Ends function dev_io_timeout() */

