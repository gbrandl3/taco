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
 * File	: 	dc_io_get.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains the functions required
 *              to use the data collector with TCL/TK
 *              You can only get informations
 *              The syntax is :
 *              dc_io_get stat error device command
 *
 * Author(s):   Denis Beauvois
 *              Gilbert Pepellin
 *              J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    June, 1996
 *
 * Version:     $Revision: 1.8 $
 *
 * Date:        $Date: 2013-05-17 13:54:36 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

/*SUPPRESS569...*/
/*SUPPRESS610...*/
#include <dc_io.h>
#include <dev_io.h>
#include <debug.h>
#include <maxe_xdr.h>

#include "convert.h"

/* ----------------- */
/* global variables  */
/* ----------------- */

/* debug mode */
static int DC_DEBUG_MODE = DC_DEBUG_0; /* no debug mode by default */

/* for keeping in memory the list of imported devices */
static DC_IMPORTED_DEVICE *DCImportedDevice;
static DC_IMPORTED_DEVICE *DCFirstDevice;
static DC_IMPORTED_DEVICE *DCCurrentDevice;

/* Hystory mode */
static int DC_MODE = DC_HISTORY_MODE_OFF;

#define 	STRING_TMP_SIZE	500

int dc_io_get (ClientData, Tcl_Interp *, int, char **);

int GetDCDebugMode()
{
	return DC_DEBUG_MODE;
}

int SetDCDebugMode(int Argin)
{
	switch (Argin)
        {
		case DC_DEBUG_0 :
			DC_DEBUG_MODE=DC_DEBUG_0;
			return True;
			break;
		case DC_DEBUG_1 :
			DC_DEBUG_MODE=DC_DEBUG_1;
			return True;
			break;
		default :
			return False;
			break;
	}
}

/* --------------------------------------------------------------- */
/*                       Function dc_io_get_hist                   */
/* Description :                                                   */
/*         This is the function called by the tcl interpreter      */
/*         It requires 4 arguments :                               */
/*            status, error, device_name, command_name ,argin      */
/* --------------------------------------------------------------- */
int dc_io_get_hist (ClientData clientdata, Tcl_Interp *interp, int argc, char **argv)
/*SUPPRESS761*/
{
	DC_MODE = DC_HISTORY_MODE_ON;
	return (dc_io_get(clientdata,interp,argc,argv));
}

/* --------------------------------------------------------------- */
/*                       Function dc_io_get                        */
/* Description :                                                   */
/*         This is the function called by the tcl interpreter      */
/*         It requires 4 arguments :                               */
/*            status, error, device_name, command_name             */
/* --------------------------------------------------------------- */
int dc_io_get (ClientData clientdata, Tcl_Interp *interp, int argc, char **argv)
{
	static int 	FirstCall = True; /* To initialize dynamical variables the 1st time */
	char 		*State,
			*Error,
			*DeviceName,
			*CommandName,
			*ArginName = NULL; /* Matched with the arguments of the command line */

	int 		i,
			j;
	long 		DeviceStatus;
	DevLong		DeviceError;
/*SUPPRESS765*/
	dc_dev_imp 	dc_imp; /* Necessary for dc_import */
	char 		*StringTemp = NULL,
			*StringAux = NULL;
	long 		readwrite = 0; /* To precise the device access */
	unsigned int 	Cmd; /* Number of the device's command */
	int 		ArgoutRequired; /* Number of the argout required */

	DC_IMPORTED_DEVICE 	*DCLastDevice;
	dc_hist 	DCHist;
	char 		*TclAux = NULL; /* For converting OutputArg into TCL variables */
	void 		*TclTemp = NULL; /* For converting OutputArg into TCL variables when var array is used */
/* 	int 		TCL_TEST = False;  */
	int 		R, /* For testing the returned code of the functions of conversion */
			test1;
	dc_devinf 	DCInfo; /* For the dc_dinfo function */
	DevLong 	TmpLong;

 
/* -------------------------------- */
/* Setting debug mode               */
/* -------------------------------- */
 	if (GetDCDebugMode() != DEBUG_LEVEL_0) 
		TCL_TEST=True;
 
/* ------------------------------------------------- */
/* First initialisation of dynamical variables       */
/* Only during the first call                        */
/* ------------------------------------------------- */
	if (FirstCall==True) 
	{
		if (TCL_TEST) 
			printf("dc_io_get. First call...\n"); 
		FirstCall=False;
		DCImportedDevice = (DC_IMPORTED_DEVICE *) malloc (sizeof(struct DC_IMPORTED_DEVICE));
		DCImportedDevice->Next = NULL;
		DCImportedDevice->Name = " ";
		DCFirstDevice = DCImportedDevice;
	}
    
/* Parse argument line  */
	if (TCL_TEST) 
		printf("dc_io_get. Parse argument line...\n");

	if ( (argc!=DC_ARGUMENTS_NUMBER) && (DC_MODE==DC_HISTORY_MODE_OFF))
	{
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"ERROR in dc_io_get\n",\
		   	"should be... dc_io_get status error device_name command_name",\
			   (char *)NULL);
		return TCL_ERROR;
	} 
	if ( (argc!=DC_ARGUMENTS_NUMBER+1) && (DC_MODE==DC_HISTORY_MODE_ON))
	{
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,"ERROR in dc_io_get\n",\
			   "should be... dc_io_get_hyst status error device_name command_name argin",\
			   (char *)NULL);
		return TCL_ERROR;
	}     
	State       = argv[1];
	Error       = argv[2];
	DeviceName  = argv[3];
	CommandName = argv[4];
	if (DC_MODE==DC_HISTORY_MODE_ON) 
		ArginName=argv[5];
 
	if (TCL_TEST) 
		printf("dc_io_get. Argument line parsed\n");
  
/* Initialize argout staus and error */
	Tcl_SetVar (interp,State,"0",0); /* No error by default */
	Tcl_SetVar (interp,Error,"",0);
 
/* Allocate some necessary working variables */
	StringTemp = (char*) calloc (STRING_TMP_SIZE,sizeof(char));
	OutputArg = NULL;

	if (TCL_TEST) 
		printf("dc_io_get. variable init...Done\n");
 
/* If this device is not already imported, do it*/
	if (TCL_TEST) 
		printf("dc_io_get. Check if the device %s is already imported\n",DeviceName);
	DCCurrentDevice = DCFirstDevice;
	test1 = 0;
	do
	{
		test1 = strcmp(DCCurrentDevice->Name,DeviceName); 
		if (test1==0) 
			break; /* This device is already imported */
		DCLastDevice = DCCurrentDevice;
		DCCurrentDevice = DCCurrentDevice->Next;
	}
	while (DCCurrentDevice != NULL);
 
	if (test1 != 0)      /* New Device */
	{
		if (TCL_TEST) 
			printf("dc_io_get. dc_import %s\n",DeviceName);
		dc_imp.device_name  = DeviceName;
		dc_imp.dc_access    = readwrite;
		dc_imp.dc_dev_error = &TmpLong;
	
		DeviceStatus = dc_import (&dc_imp,1,&DeviceError);
		if (DeviceStatus != DS_OK) /* Error in the import */
		{
			snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
			Tcl_SetVar (interp,State,StringTemp,0);
			StringAux = dev_error_str(*(dc_imp.dc_dev_error));
			snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)*(dc_imp.dc_dev_error));
			Tcl_SetVar (interp,Error,StringTemp,0);          
			if (TCL_TEST) 
				printf("ERROR during the import :%s\n",StringTemp);
			free(StringAux); 
			free (StringTemp);
			return TCL_OK;
		}
					
		DCCurrentDevice = (DC_IMPORTED_DEVICE *) malloc (sizeof(struct DC_IMPORTED_DEVICE));
		DCCurrentDevice->Next = NULL;
		DCCurrentDevice->Name = (char *) malloc (strlen(DeviceName)+1);
		strcpy (DCCurrentDevice->Name,DeviceName);
		DCCurrentDevice->dc = *(dc_imp.dc_ptr);
		DCLastDevice->Next = DCCurrentDevice;
	} /* ends 1st clause of if (test1 != 0) */
	else if (TCL_TEST) 
		printf("The device %s is already imported\n",DeviceName);
     
/* Test if the command is available */
	if (TCL_TEST) 
		printf("dc_io_get. Check if the command %s is available\n",CommandName); 
	DeviceStatus = db_cmd_query (CommandName,&Cmd,&DeviceError); 
	if (DeviceStatus != DS_OK) /* Error in the db_cmd_query */
	{
		snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free(StringAux); 
		free (StringTemp);
		return TCL_OK;
	}

/* get argout type from dc_info */
	DeviceStatus = dc_dinfo (DeviceName,&DCInfo,&DeviceError);
	if (DeviceStatus != DS_OK) /* Error in the dc_dinfo */
	{
		snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
		Tcl_SetVar (interp,State,StringTemp,0);
		StringAux = dev_error_str(DeviceError);
		snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free(StringAux); 
		free (StringTemp);
		return TCL_OK;
	}

	test1 = False;
	for (i=0;i<DCInfo.devinf_nbcmd;i++)
	{
		if ( DCInfo.devcmd[i].devinf_cmd == Cmd )
		{
			test1 = True;
			ArgoutRequired = (DCInfo.devcmd[i]).devinf_argout;
			break;
		}
	}
   
	if (test1==False)
	{
		Tcl_ResetResult(interp);
		snprintf(StringTemp, STRING_TMP_SIZE, "The %s command is not available for dc_io_get\n",CommandName);
		Tcl_AppendResult(interp,"ERROR in dc_io_get...\n",StringTemp,NULL);
		free (StringTemp);
		return TCL_ERROR;
	}
 
	if (TCL_TEST) 
		printf("dc_io_get. Ready to call dc_devget\n");
       
 
/* ------------------------------------------------------ */
/* Now setting the argout for dc_io_get                   */
/* OutputArg will contain a pointer on the specified type */
/* ------------------------------------------------------ */
	if (TCL_TEST) 
		printf("Preparing output variable...");
 
	OutputArg = NULL;
	switch (ArgoutRequired)
	{
		case D_VOID_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VOID_TYPE\n");
			break;
		case D_BOOLEAN_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_BOOLEAN_TYPE\n");
			F_D_BOOLEAN_TYPE(interp,"",OUTPUT);
			break;
		case D_USHORT_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_USHORT_TYPE\n");
			F_D_USHORT_TYPE(interp,"",OUTPUT);
			break;
		case D_SHORT_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_SHORT_TYPE\n");
			F_D_SHORT_TYPE(interp,"",OUTPUT);
			break;
		case D_ULONG_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_ULONG_TYPE\n");
			F_D_ULONG_TYPE(interp,"",OUTPUT);
			break;
		case D_LONG_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_LONG_TYPE\n");
			F_D_LONG_TYPE(interp,"",OUTPUT);
			break;
		case D_FLOAT_TYPE: 
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_FLOAT_TYPE\n");
			F_D_FLOAT_TYPE(interp,"",OUTPUT);
			break;
		case D_DOUBLE_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_DOUBLE_TYPE\n");
			F_D_DOUBLE_TYPE(interp,"",OUTPUT);
			break;
		case D_STRING_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_STRING_TYPE\n");
			F_D_STRING_TYPE(interp,"",OUTPUT);
			break;
		case D_INT_FLOAT_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_INT_FLOAT_TYPE\n");
			F_D_INT_FLOAT_TYPE(interp,"",OUTPUT);
			break;
		case D_FLOAT_READPOINT:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_FLOAT_READPOINT\n");
			F_D_FLOAT_READPOINT(interp,"",OUTPUT);
			break;
		case D_STATE_FLOAT_READPOINT:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_STATE_FLOAT_READPOINT\n");
			F_D_STATE_FLOAT_READPOINT(interp,"",OUTPUT);
			break;
		case D_LONG_READPOINT:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_LONG_READPOINT\n");
			F_D_LONG_READPOINT(interp,"",OUTPUT);
			break;
		case D_DOUBLE_READPOINT:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_DOUBLE_READPOINT\n");
			F_D_DOUBLE_READPOINT(interp,"",OUTPUT);
			break;
		case D_VAR_CHARARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_CHARARR\n");
			F_D_VAR_CHARARR(interp,"",OUTPUT);
			break;
		case D_VAR_STRINGARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_STRINGARR\n");
			F_D_VAR_STRINGARR(interp,"",OUTPUT);
			break;
		case D_VAR_USHORTARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_USHORTARR\n");
			F_D_VAR_USHORTARR(interp,"",OUTPUT);
			break;
		case D_VAR_SHORTARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_SHORTARR\n");
			F_D_VAR_SHORTARR(interp,"",OUTPUT);
			break;
		case D_VAR_ULONGARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_ULONGARR\n");
			F_D_VAR_ULONGARR(interp,"",OUTPUT);
			break;
		case D_VAR_LONGARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_LONGARR\n");
			F_D_VAR_LONGARR(interp,"",OUTPUT);
			break;
		case D_VAR_FLOATARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_FLOATARR\n");
			F_D_VAR_FLOATARR(interp,"",OUTPUT);
			break;
		case D_VAR_DOUBLEARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_DOUBLEARR\n");
			F_D_VAR_DOUBLEARR(interp,"",OUTPUT);
			break;
		case D_VAR_FRPARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_FRPARR\n");
			F_D_VAR_FRPARR(interp,"",OUTPUT);
			break;
		case D_VAR_SFRPARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_SFRPARR\n");
			F_D_VAR_SFRPARR(interp,"",OUTPUT);
			break;
		case D_VAR_LRPARR:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_VAR_LRPARR\n");
			F_D_VAR_LRPARR(interp,"",OUTPUT);
			break;
		case D_OPAQUE_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_OPAQUE_TYPE\n");
			F_D_OPAQUE_TYPE(interp,"",OUTPUT);
			break;
#if 0
		case D_MULMOVE_TYPE:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_MULMOVE_TYPE\n");
			F_D_MULMOVE_TYPE(interp,"",OUTPUT);
			break;
		case D_MOTOR_LONG:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_MOTOR_LONG\n");
			F_D_MOTOR_LONG(interp,"",OUTPUT);
			break;
		case D_MOTOR_FLOAT:
			if(TCL_TEST) 
				printf("dc_io_get: ArgoutRequired= D_MOTOR_FLOAT\n");
			F_D_MOTOR_FLOAT(interp,"",OUTPUT);
			break;
#endif
		default	:
			printf ("ERROR during the output argument setting.. \n");
			free (StringTemp); 
			return TCL_ERROR;
			break; 
	}
       
	if (TCL_TEST) 
		printf("OK\n");

/* ------------------------------ */
/* Now we call dc_devget          */
/* ------------------------------ */
	if (DC_MODE==DC_HISTORY_MODE_OFF)
	{
		if (TCL_TEST) 
			printf("dc_io_get. Calling dc_devget...");
       					  	
		DeviceStatus = dc_devget (&(DCCurrentDevice->dc),Cmd,OutputArg,ArgoutRequired,&DeviceError);
		if (TCL_TEST) 
			printf("dc_ioget. ERROR=%ld \n",DeviceStatus);
		if (DeviceStatus != DS_OK) /* Error in the dc_devget */
		{
			if (TCL_TEST) 
				printf("dc_ioget. dc_devget failed");
	  		snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
  			Tcl_SetVar (interp,State,StringTemp,0);
  			StringAux = dev_error_str(DeviceError);
	  		snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
  			Tcl_SetVar (interp,Error,StringTemp,0);
	  		free(StringAux);
  			free (StringTemp); 
			free (OutputArg);
	  		return TCL_OK;
		}
		else if (TCL_TEST) 
			printf("dc_ioget. dc_devget succeeded\n"); 
 
	} /* ends 1st clause of if (DC_MODE==DC_HISTORY_MODE_OFF) */
	else /* history_mode */
	{
		printf("dc_io_get. calling dc_devget_history\n");
		DCHist.argout = OutputArg;
		if (TCL_TEST) 
			printf("Calling dc_devget_history...");
	
		DeviceStatus = dc_devget_history (&(DCCurrentDevice->dc),Cmd,&DCHist,ArgoutRequired,1,&DeviceError);
		if (TCL_TEST) 
			printf("ERROR=%ld ",DeviceStatus);
     					  	
		if (DeviceStatus != DS_OK) /* Error in the dc_devget */
		{
			snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
	  		Tcl_SetVar (interp,State,StringTemp,0);
	  		StringAux = dev_error_str(DCHist.cmd_error);
	  		snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DCHist.cmd_error);
	  		Tcl_SetVar (interp,Error,StringTemp,0);
	  		free(StringAux);
	  		free (StringTemp); 
			free (OutputArg);         
			if (TCL_TEST) 
				printf("exiting\n");     					  	
			return TCL_OK;
		}
            
		if (TCL_TEST) 
			printf("OK\n");
	} /* end of history mode */
      
        
 /* ------------------------------------------- */
 /* Now converting C variables in TCL variables */
 /* ------------------------------------------- */
 if (TCL_TEST) printf("Converting C variables in TCL variables...\n");
 
	Tcl_ResetResult (interp);
	TclAux = NULL;
	switch (ArgoutRequired)
	{
		case D_VOID_TYPE:
			break;
		case D_BOOLEAN_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((char*)OutputArg));        
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_USHORT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned short*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_SHORT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",*((short*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_ULONG_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld",*((DevULong*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_LONG_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld",*((DevLong*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_FLOAT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%f",*((float*)OutputArg));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_DOUBLE_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%17.8e",*((double*)OutputArg));
/*************************************************************************/                
/*			printf("TclAux=%17.8e\n",*((double*)OutputArg));       */
/*			fflush stdout;                                         */
/**************************************************************************/                    
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_STRING_TYPE:
			Tcl_AppendResult(interp,StringTypeOut,NULL);
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_INT_FLOAT_TYPE:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)(((DevIntFloat*)OutputArg)->state));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevIntFloat*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_FLOAT_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevFloatReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevFloatReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_STATE_FLOAT_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%d",(((DevStateFloatReadPoint*)OutputArg)->state));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevStateFloatReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevStateFloatReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_LONG_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)(((DevLongReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)(((DevLongReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_DOUBLE_READPOINT:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%g",(((DevDoubleReadPoint*)OutputArg)->set));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",(((DevDoubleReadPoint*)OutputArg)->read));
			Tcl_AppendResult(interp,TclAux,NULL); 
			break;
		case D_VAR_CHARARR:
			j = ((DevVarCharArray*)OutputArg)->length;      
			TclAux = (char *) malloc (j * sizeof(char) + 1 );
			strncpy ((char*)TclAux,(char *)((DevVarCharArray*)OutputArg)->sequence,j);
			*(TclAux+j) = '\0';
			Tcl_AppendResult(interp,TclAux,NULL);
                 
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, TCLAUX_LENGTH, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, TCLAUX_LENGTH, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_VAR_STRINGARR:
			j = ((DevVarStringArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				StringArray = (DevString*)( (((DevVarStringArray*)OutputArg)->sequence+i) );
				Tcl_AppendElement(interp,*StringArray);
			}                
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_VAR_USHORTARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarUShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (u_short*)( ((DevVarUShortArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((unsigned short*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf(StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
                	}    
			break;
		case D_VAR_SHORTARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (short*)( ((DevVarShortArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",*((short*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_ULONGARR		:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarULongArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (unsigned int*)( ((DevVarULongArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%ld",*((DevULong*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_LONGARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarLongArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (int*)( ((DevVarLongArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%ld",*((DevLong*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_FLOATARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarShortArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (float*)( ((DevVarFloatArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",*((float*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_DOUBLEARR:
			TclAux = (char *) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarDoubleArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (double*)( ((DevVarDoubleArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%17.8e",*((double*)TclTemp));
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_FRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarFloatReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevFloatReadPoint*)(((DevVarFloatReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevFloatReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevFloatReadPoint*)TclTemp)->read);
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_SFRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarStateFloatReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevFloatReadPoint*)(((DevVarStateFloatReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%d",((DevStateFloatReadPoint*)TclTemp)->state);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevStateFloatReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%g",((DevStateFloatReadPoint*)TclTemp)->read);
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_VAR_LRPARR:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			j = ((DevVarLongReadPointArray*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclTemp = (DevLongReadPoint*)(((DevVarLongReadPointArray*)OutputArg)->sequence+i);
				snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)((DevLongReadPoint*)TclTemp)->set);
				Tcl_AppendElement(interp,TclAux);
				snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)((DevLongReadPoint*)TclTemp)->read);         
				Tcl_AppendElement(interp,TclAux);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}    
			break;
		case D_OPAQUE_TYPE:
			j = ((DevOpaque*)OutputArg)->length;
			for (i=0;i<j;i++)
			{
				TclAux = (char*)(((DevOpaque*)OutputArg)->sequence+i);
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			DeviceStatus = dev_xdrfree(ArgoutRequired,(void*)OutputArg,&DeviceError);
			if (DeviceStatus != DS_OK) /* Error in the dev_xdrfree */
			{
				snprintf (StringTemp, STRING_TMP_SIZE, "%ld",DeviceStatus);
				Tcl_SetVar (interp,State,StringTemp,0);
				StringAux = dev_error_str(DeviceError);
				snprintf (StringTemp, STRING_TMP_SIZE, "%s [%ld]",StringAux, (long)DeviceError);
				Tcl_SetVar (interp,Error,StringTemp,0);
				free (StringTemp); 
				free (StringAux);
				return TCL_OK;
			}
			break;
		case D_MULMOVE_TYPE:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)( ((DevMulMove*)OutputArg)->action[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)( ((DevMulMove*)OutputArg)->delay[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			for (i=0;i<8;i++)
			{
				snprintf(TclAux, TCLAUX_LENGTH, "%f",( ((DevMulMove*)OutputArg)->position[i]));
				Tcl_AppendResult(interp,TclAux," ",NULL);
			}
			break;
		case D_MOTOR_LONG: 
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)( ((DevMotorLong*)OutputArg)->axisnum));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)( ((DevMotorLong*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		case D_MOTOR_FLOAT:
			TclAux = (char*) malloc (TCLAUX_LENGTH*sizeof(char));
			snprintf(TclAux, TCLAUX_LENGTH, "%ld", (long)( ((DevMotorFloat*)OutputArg)->axisnum));
			Tcl_AppendResult(interp,TclAux," ",NULL);
			snprintf(TclAux, TCLAUX_LENGTH, "%f",( ((DevMotorFloat*)OutputArg)->value));
			Tcl_AppendResult(interp,TclAux,NULL);
			break;
		default:
			printf ("ERROR, output argument type is unknown.. \n");
			free (StringTemp); 
			return TCL_ERROR;
			break;
	}       


	if (TCL_TEST)
	{
	printf ("dc_io_get() output arguments converted\n");
	fflush (stdout);
	}

	if (TCL_TEST)
		printf("OK\n");
 
 /* ------------------------------- */    					  	
 /* Converting time if history mode */
 /* ------------------------------- */
	if (DC_MODE==DC_HISTORY_MODE_ON)
	{
		if (TCL_TEST)
			printf("Converting the time in tcl...");
		free (TclAux);
		TclAux = (char *) calloc (TCLAUX_LENGTH,sizeof(char));
		snprintf(TclAux, TCLAUX_LENGTH, "%s", ctime(&(DCHist.time))); /* Converting time to string */
		R = strlen(TclAux);
		TclAux[R-1] = '\0'; /* Remove the \n character */
		Tcl_SetVar(interp,ArginName,TclAux,0);
     
		if (TCL_TEST)
			printf("OK\n");
	}
     
/* ----------------- */
/* free the memory   */
/* ----------------- */
	if (TCL_TEST)
		printf("Free the memory...");
     					  	
	DC_MODE = DC_HISTORY_MODE_OFF; /* default mode */
	if (TclAux != NULL) 
		free (TclAux);
	free (StringTemp); 
	free (OutputArg); 

	if (TCL_TEST)
		printf("OK\nExiting\n");
	return TCL_OK;  
}				
 
 
 
